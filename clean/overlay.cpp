#include "shared.h"
#include "weapons.h"
#include "aim.h"
#include "keybinds.h"
#include <d3d11.h>
#include <stdio.h>
#include "imgui.h"
#include "backends/imgui_impl_dx11.h"

// Observe objects created by the game. No probe window/device, code detours,
// thread suspension or changes to the game's device creation parameters.
struct Slot {void **address;void *original;void *replacement;};
static Slot slots[32];static unsigned slotCount;
static SRWLOCK slotLock=SRWLOCK_INIT,drawLock=SRWLOCK_INIT;
static bool replacePointer(void **address,void *expected,void *next) {
    DWORD protection,ignored;
    if(!VirtualProtect(address,sizeof(void *),PAGE_READWRITE,&protection)) return false;
    void *observed=InterlockedCompareExchangePointer(address,next,expected);
    bool restored=VirtualProtect(address,sizeof(void *),protection,&ignored)!=FALSE;
    if(!restored) modLog("Pointer page protection restoration failed: %lu",GetLastError());
    return observed==expected;
}
static bool hookSlot(void **address,void *replacement) {
    AcquireSRWLockExclusive(&slotLock);
    for(unsigned i=0;i<slotCount;i++) if(slots[i].address==address) {
        bool ok=*address==replacement;ReleaseSRWLockExclusive(&slotLock);return ok;
    }
    bool ok=false;
    if(slotCount<32) {
        Slot &slot=slots[slotCount];slot={address,*address,replacement};
        // Publish while the lock is held; callbacks wait until the original exists.
        if(replacePointer(address,slot.original,replacement)) {++slotCount;ok=true;}
    }
    ReleaseSRWLockExclusive(&slotLock);return ok;
}
static void *originalFor(void **address) {
    AcquireSRWLockShared(&slotLock);void *original=nullptr;
    for(unsigned i=0;i<slotCount;i++) if(slots[i].address==address) {original=slots[i].original;break;}
    ReleaseSRWLockShared(&slotLock);return original;
}
typedef HRESULT (WINAPI *FactoryFn)(REFIID,void **);
typedef HRESULT (STDMETHODCALLTYPE *SwapFn)(IDXGIFactory *,IUnknown *,DXGI_SWAP_CHAIN_DESC *,IDXGISwapChain **);
typedef HRESULT (STDMETHODCALLTYPE *PresentFn)(IDXGISwapChain *,UINT,UINT);
static FactoryFn factoryOriginal;
static void **factoryImport;
static bool menuOpen,bindingsOpen,bindingsReturnToMain,settingError,bindingsLoaded,bindingsError;
static unsigned selectedBinding;
static int selectedWeapon;
static keybinds::Engine overlayBindings;
static char bindingsNotice[256];
static bool navigationHeld[4],navigationFocused;
static constexpr unsigned bindingsPageSize=8;
static const int aimKeys[]={VK_MBUTTON,VK_RBUTTON,'Z',VK_XBUTTON1,VK_XBUTTON2};
static SHORT overlayKeyQuery(int key) {return GetAsyncKeyState(key);}
static std::string shortcutLabel(keybinds::Action action) {
    keybinds::Chord chord=overlayBindings.binding(action);
    if(chord.modifiers==0 && chord.key==VK_OEM_MINUS)return "-";
    if(chord.modifiers==0 && chord.key==VK_OEM_PLUS)return "+";
    return overlayBindings.bindingLabel(action);
}
static void loadOverlayBindings() {
    if(bindingsLoaded)return;
    keybinds::LoadReport result=overlayBindings.load(modIni);bindingsLoaded=true;
    if(result.invalid || result.conflicts) {
        bindingsError=true;
        snprintf(bindingsNotice,sizeof(bindingsNotice),"Raccourcis invalides (%u) ou en conflit (%u) : valeurs par defaut restaurees.",result.invalid,result.conflicts);
    }
    modLog("Keybinds loaded: invalid=%u, conflicts=%u",result.invalid,result.conflicts);
}
static void publishMenuState() {InterlockedExchange(&modMenuOpen,menuOpen || bindingsOpen);}
static void closeBindingsMenu() {
    bindingsOpen=false;menuOpen=bindingsReturnToMain;publishMenuState();
}
static void updateMenuInput(bool focused) {
    using Action=keybinds::Action;
    loadOverlayBindings();
    const int navigationKeys[4]={VK_UP,VK_DOWN,VK_RETURN,VK_ESCAPE};
    bool navigationEdges[4]={};
    for(unsigned i=0;i<4;++i) {
        bool down=(overlayKeyQuery(navigationKeys[i])&0x8000)!=0;
        navigationEdges[i]=focused && navigationFocused && down && !navigationHeld[i];
        navigationHeld[i]=down;
    }
    navigationFocused=focused;
    keybinds::Events events=overlayBindings.pollEdges(focused,menuOpen && !bindingsOpen,false,overlayKeyQuery);
    if(overlayBindings.capturing()) {
        keybinds::CaptureResult result=overlayBindings.pollCapture(focused,overlayKeyQuery);
        if(result.status==keybinds::CaptureStatus::Conflict) {
            bindingsError=true;
            snprintf(bindingsNotice,sizeof(bindingsNotice),"%s est deja utilise pour : %s.",keybinds::chordLabel(result.candidate).c_str(),keybinds::definitions[static_cast<unsigned>(result.conflict)].label);
        } else if(result.status==keybinds::CaptureStatus::Invalid) {
            bindingsError=true;snprintf(bindingsNotice,sizeof(bindingsNotice),"Appuie sur une seule touche, avec Ctrl, Alt ou Maj si souhaite.");
        } else if(result.status==keybinds::CaptureStatus::Cancelled) {
            bindingsError=false;snprintf(bindingsNotice,sizeof(bindingsNotice),"Modification annulee. Le raccourci precedent est conserve.");
        } else if(result.status==keybinds::CaptureStatus::Captured) {
            keybinds::RebindResult saved=overlayBindings.rebindAndSave(result.action,result.candidate,modIni);
            bindingsError=!saved.ok;
            if(saved.ok) {
                snprintf(bindingsNotice,sizeof(bindingsNotice),"Enregistre : %s = %s.",keybinds::definitions[static_cast<unsigned>(result.action)].label,keybinds::chordLabel(result.candidate).c_str());
                modLog("Keybind saved: action=%ls, shortcut=%s",keybinds::definitions[static_cast<unsigned>(result.action)].iniName,keybinds::chordLabel(result.candidate).c_str());
                overlayBindings.cancelCapture();
            } else snprintf(bindingsNotice,sizeof(bindingsNotice),"Enregistrement impossible. Le raccourci precedent est conserve. Echap : annuler.");
        }
        return;
    }
    // Plain navigation belongs to the editor while it is open. A user may bind
    // either menu to an arrow/Enter/Escape without making the editor unusable.
    if(focused && bindingsOpen) {
        bool plain=true;
        for(int key:{VK_CONTROL,VK_LCONTROL,VK_RCONTROL,VK_MENU,VK_LMENU,VK_RMENU,VK_SHIFT,VK_LSHIFT,VK_RSHIFT,VK_LWIN,VK_RWIN})
            if(overlayKeyQuery(key)&0x8000){plain=false;break;}
        if(plain && (navigationEdges[0] || navigationEdges[1] || navigationEdges[2] || navigationEdges[3])) {
            if(navigationEdges[3]){closeBindingsMenu();return;}
            if(navigationEdges[0])selectedBinding=(selectedBinding+keybinds::count-1)%keybinds::count;
            if(navigationEdges[1])selectedBinding=(selectedBinding+1)%keybinds::count;
            if(navigationEdges[2])overlayBindings.beginCapture(static_cast<Action>(selectedBinding));
            return;
        }
    }
    if(events.has(Action::MainMenu)) {
        if(bindingsOpen){bindingsOpen=false;menuOpen=true;}
        else menuOpen=!menuOpen;
        publishMenuState();modLog("Overlay %s",menuOpen?"opened":"closed");return;
    }
    if(events.has(Action::BindingsMenu)) {
        if(bindingsOpen)closeBindingsMenu();
        else {bindingsReturnToMain=menuOpen;menuOpen=false;bindingsOpen=true;publishMenuState();}
        modLog("Keybind menu %s",bindingsOpen?"opened":"closed");return;
    }
    if(!focused)return;
    if(bindingsOpen)return;
    if(!menuOpen)return;
    if(events.has(Action::Vitals))requestVitals();
    if(events.has(Action::Ammo))requestAmmo();
    if(events.has(Action::WeaponPrevious))selectedWeapon=(selectedWeapon+weaponChoiceCount-1)%weaponChoiceCount;
    if(events.has(Action::WeaponNext))selectedWeapon=(selectedWeapon+1)%weaponChoiceCount;
    if(events.has(Action::GiveMain))requestWeapon(selectedWeapon,0);
    if(events.has(Action::GiveSecondary))requestWeapon(selectedWeapon,1);
    if(events.has(Action::Powers))requestProgression(ProgressionAction::Powers);
    if(events.has(Action::Gear))requestProgression(ProgressionAction::Gear);
    if(events.has(Action::WeaponUpgrades))requestProgression(ProgressionAction::WeaponUpgrades);
    if(events.has(Action::VigorUpgrades))requestProgression(ProgressionAction::VigorUpgrades);
    if(events.has(Action::Infusions))requestProgression(ProgressionAction::Infusions);
    if(events.has(Action::Money))requestProgression(ProgressionAction::Money);
    if(events.has(Action::Lockpicks))requestProgression(ProgressionAction::Lockpicks);
    if(events.has(Action::SkipIntros))settingError=!queueIntroSetting(!InterlockedCompareExchange(&configWanted,0,0));
    if(events.has(Action::Speed)) {
        int value=(int)InterlockedCompareExchange(&speedPercent,0,0);
        settingError=!saveSpeedSetting(value>=100 && value<200 && value%25==0?value+25:100);
    }
    if(events.has(Action::FovDecrease)) {
        int value=(int)InterlockedCompareExchange(&fovDegrees,0,0);
        if(value>80)settingError=!saveFOVSetting(value<85?80:value-5);
    }
    if(events.has(Action::FovIncrease)) {
        int value=(int)InterlockedCompareExchange(&fovDegrees,0,0);
        if(value<130)settingError=!saveFOVSetting(value>125?130:value+5);
    }
    if(events.has(Action::Crosshair))settingError=!saveCrosshairSetting(!InterlockedCompareExchange(&crosshairHidden,0,0));
    if(events.has(Action::HUD)) {
        int value=(int)InterlockedCompareExchange(&hudWanted,0,0);
        settingError=!saveHUDSetting(value==100?125:value==125?150:value==150?200:100);
    }
    if(events.has(Action::HoldAim))settingError=!saveAimSetting(!InterlockedCompareExchange(&aimWanted,0,0));
    if(events.has(Action::AimKey)) {
        int old=(int)InterlockedCompareExchange(&aimKey,0,0),next=0;
        for(int i=0;i<5;++i)if(aimKeys[i]==old){next=(i+1)%5;break;}
        settingError=!saveAimKey(aimKeys[next]);
    }
}
static const char *aimKeyLabel(int key) {
    switch(key){case VK_MBUTTON:return "bouton central";case VK_RBUTTON:return "clic droit";case 'Z':return "Z";
    case VK_XBUTTON1:return "bouton lateral 1";case VK_XBUTTON2:return "bouton lateral 2";default:return "touche personnalisee (INI)";}
}
static unsigned long frames;
static ID3D11Device *device;
static ID3D11DeviceContext *context;
static ImGuiContext *gui;
static LARGE_INTEGER lastTime,frequency;

static void releaseRenderer() {
    if(gui) {ImGui::SetCurrentContext(gui);ImGui_ImplDX11_Shutdown();ImGui::DestroyContext(gui);gui=nullptr;}
    if(context) {context->Release();context=nullptr;}
    if(device) {device->Release();device=nullptr;}
}
static bool renderer(IDXGISwapChain *chain) {
    ID3D11Device *current=nullptr;
    if(FAILED(chain->GetDevice(__uuidof(ID3D11Device),(void **)&current))) return false;
    if(device==current && gui) {current->Release();return true;}
    releaseRenderer();device=current;device->GetImmediateContext(&context);
    gui=ImGui::CreateContext();ImGuiIO &io=ImGui::GetIO();io.IniFilename=nullptr;io.LogFilename=nullptr;
    ImFontConfig font;font.SizePixels=24;io.Fonts->AddFontDefault(&font);
    ImGui::StyleColorsDark();
    if(!ImGui_ImplDX11_Init(device,context)) {releaseRenderer();return false;}
    QueryPerformanceFrequency(&frequency);QueryPerformanceCounter(&lastTime);
    modLog("Overlay renderer initialized on GAME device %p",device);return true;
}
static void drawShortcut(keybinds::Action action,const char *description) {
    ImGui::TextWrapped("%s   %s",shortcutLabel(action).c_str(),description);
}
static void drawMenuSection(const char *label) {ImGui::TextColored(ImVec4(0.96f,0.72f,0.3f,1),"%s",label);}
static void drawMainMenuContents() {
    using Action=keybinds::Action;
    drawMenuSection("INFINITEMOD");
    ImGui::TextDisabled("%s : menu | %s : raccourcis",shortcutLabel(Action::MainMenu).c_str(),shortcutLabel(Action::BindingsMenu).c_str());
    ImGui::TextDisabled("Commandes actives avec ce menu ouvert. Le jeu continue.");
    ImGui::Separator();
    if(ImGui::BeginTable("mainColumns",2,ImGuiTableFlags_SizingStretchSame)) {
        ImGui::TableNextRow();ImGui::TableSetColumnIndex(0);
        drawMenuSection("SANTE ET ARMES");
        drawShortcut(Action::Vitals,"Remplir sante, bouclier et sels");
        drawShortcut(Action::Ammo,"Remplir les munitions des deux armes");
        ImGui::TextWrapped("%s / %s   Choisir : %s",shortcutLabel(Action::WeaponPrevious).c_str(),shortcutLabel(Action::WeaponNext).c_str(),weaponChoices[selectedWeapon].label);
        drawShortcut(Action::GiveMain,"Donner l'arme selectionnee en main");
        drawShortcut(Action::GiveSecondary,"Donner l'arme selectionnee en secondaire");
        ImGui::Separator();drawMenuSection("PROGRESSION");
        drawShortcut(Action::Powers,"Tous les pouvoirs");
        drawShortcut(Action::Gear,"Tous les cadeaux (equipements)");
        drawShortcut(Action::WeaponUpgrades,"Toutes les ameliorations d'armes");
        drawShortcut(Action::VigorUpgrades,"Toutes les ameliorations de pouvoirs");
        drawShortcut(Action::Infusions,"Infusions des trois jauges au maximum");
        drawShortcut(Action::Money,"Argent au maximum");
        drawShortcut(Action::Lockpicks,"Crochets au maximum");
        ImGui::Separator();
        char weaponText[192];weaponStatus(weaponText,sizeof(weaponText));ImGui::TextWrapped("%s",weaponText);
        ImGui::TableSetColumnIndex(1);
        drawMenuSection("DEPLACEMENT ET FOV");
        ImGui::TextWrapped("%s   Vitesse : x%.2f",shortcutLabel(Action::Speed).c_str(),(double)InterlockedCompareExchange(&speedPercent,0,0)/100.0);
        ImGui::TextWrapped("%s / %s   FOV du monde : %ld deg",shortcutLabel(Action::FovDecrease).c_str(),shortcutLabel(Action::FovIncrease).c_str(),InterlockedCompareExchange(&fovDegrees,0,0));
        char optionText[160];playerOptionsStatus(optionText,sizeof(optionText));ImGui::TextWrapped("%s",optionText);
        ImGui::Separator();drawMenuSection("AFFICHAGE ET VISEE");
        bool hideCrosshair=InterlockedCompareExchange(&crosshairHidden,0,0)!=0;LONG reticleState=crosshairStatus();
        ImGui::TextWrapped("%s   Masquer les reticules : %s",shortcutLabel(Action::Crosshair).c_str(),!hideCrosshair?"NON":reticleState>0?"OUI":reticleState<0?"INDISPONIBLE":"EN ATTENTE");
        if(hideCrosshair && reticleState<0)ImGui::TextColored(ImVec4(1,0.4f,0.35f,1),"Masquage indisponible : voir le journal.");
        LONG hud=InterlockedCompareExchange(&hudWanted,0,0),applied=InterlockedCompareExchange(&hudApplied,0,0);
        if(hud==100)ImGui::TextWrapped("%s   Interface : compacte (native)",shortcutLabel(Action::HUD).c_str());
        else ImGui::TextWrapped("%s   Interface : jusqu'a x%.2f",shortcutLabel(Action::HUD).c_str(),(double)hud/100);
        if(InterlockedCompareExchange(&hudResult,0,0)<0)ImGui::TextColored(ImVec4(1,0.4f,0.35f,1),"Reglage HUD non applique : voir le journal.");
        else if(InterlockedCompareExchange(&hudResult,0,0)==2)ImGui::TextDisabled("Verification de l'interface au demarrage...");
        else if(hud!=applied)ImGui::TextDisabled("Interface : relancer pour appliquer le changement.");
        else ImGui::TextDisabled("Interface : changement au prochain lancement.");
        ImGui::TextWrapped("%s   Visee maintenue : %s",shortcutLabel(Action::HoldAim).c_str(),InterlockedCompareExchange(&aimWanted,0,0)?"ON":"OFF");
        if(!aimReady())ImGui::TextColored(ImVec4(1,0.4f,0.35f,1),"Visee maintenue indisponible : voir le journal.");
        ImGui::TextWrapped("%s   Touche de visee : %s",shortcutLabel(Action::AimKey).c_str(),aimKeyLabel((int)InterlockedCompareExchange(&aimKey,0,0)));
        ImGui::TextDisabled("Meme touche que dans les commandes du jeu.");
        ImGui::Separator();drawMenuSection("DEMARRAGE");
        bool wanted=InterlockedCompareExchange(&configWanted,0,0)!=0;
        ImGui::TextWrapped("%s   Passer les logos : %s",shortcutLabel(Action::SkipIntros).c_str(),wanted?"OUI":"NON");
        ImGui::TextDisabled("Au prochain lancement. Chargement conserve.");
        LONG status=InterlockedCompareExchange(&configResult,0,0);
        if(status<0 || settingError)ImGui::TextColored(ImVec4(1,0.4f,0.35f,1),"Enregistrement impossible : voir le journal.");
        else if(status==2)ImGui::TextDisabled("Configuration enregistree. Relancer le jeu.");
        else if(status==1)ImGui::TextDisabled("Configuration verifiee.");
        else ImGui::TextDisabled("Verification de la configuration...");
        ImGui::EndTable();
    }
}
static void drawBindingsMenuContents() {
    using Action=keybinds::Action;
    drawMenuSection("INFINITEMOD / RACCOURCIS");
    ImGui::TextDisabled("%s : fermer | %s : menu principal",shortcutLabel(Action::BindingsMenu).c_str(),shortcutLabel(Action::MainMenu).c_str());
    if(overlayBindings.capturing()) {
        ImGui::TextColored(ImVec4(0.96f,0.72f,0.3f,1),"Nouvelle touche : %s",keybinds::definitions[selectedBinding].label);
        ImGui::TextWrapped("Relache les touches, puis presse ton raccourci (Ctrl, Alt ou Maj possibles). Echap : annuler.");
    } else ImGui::TextDisabled("Haut / Bas : choisir | Entree : modifier | Echap : retour");
    ImGui::Separator();
    unsigned first=(selectedBinding/bindingsPageSize)*bindingsPageSize,last=first+bindingsPageSize;
    if(last>keybinds::count)last=keybinds::count;
    if(ImGui::BeginTable("bindingRows",2,ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Action",ImGuiTableColumnFlags_WidthStretch,0.70f);
        ImGui::TableSetupColumn("Touche",ImGuiTableColumnFlags_WidthStretch,0.30f);
        for(unsigned i=first;i<last;++i) {
            ImGui::TableNextRow();ImGui::TableSetColumnIndex(0);
            if(i==selectedBinding)ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(0.96f,0.72f,0.3f,1));
            ImGui::TextWrapped("%s %s",i==selectedBinding?">":" ",keybinds::definitions[i].label);
            ImGui::TableSetColumnIndex(1);ImGui::TextWrapped("%s",shortcutLabel(static_cast<Action>(i)).c_str());
            if(i==selectedBinding)ImGui::PopStyleColor();
        }
        ImGui::EndTable();
    }
    ImGui::Separator();
    ImGui::TextDisabled("Page %u / %u - %u raccourcis configurables",selectedBinding/bindingsPageSize+1,(keybinds::count+bindingsPageSize-1)/bindingsPageSize,keybinds::count);
    if(*bindingsNotice) {
        ImGui::PushStyleColor(ImGuiCol_Text,bindingsError?ImVec4(1,0.4f,0.35f,1):ImVec4(0.4f,0.86f,0.62f,1));
        ImGui::TextWrapped("%s",bindingsNotice);ImGui::PopStyleColor();
    }
    ImGui::TextDisabled("Les modifications sont enregistrees automatiquement.");
    ImGui::TextDisabled("Haut / Bas / Entree / Echap reserves dans cet editeur.");
    ImGui::TextDisabled("Aucune commande du mod n'est executee pendant le reglage.");
}
static void drawMenu(IDXGISwapChain *chain) {
    if(!renderer(chain)) return;
    ImGui::SetCurrentContext(gui);
    ID3D11Texture2D *buffer=nullptr;
    if(FAILED(chain->GetBuffer(0,__uuidof(ID3D11Texture2D),(void **)&buffer))) return;
    D3D11_TEXTURE2D_DESC desc;buffer->GetDesc(&desc);
    ID3D11RenderTargetView *target=nullptr;
    HRESULT result=device->CreateRenderTargetView(buffer,nullptr,&target);buffer->Release();
    if(FAILED(result)) {modLog("Overlay target error %08lx",result);return;}
    float scale=(float)desc.Height/1080; if(scale<0.65f)scale=0.65f;if(scale>3)scale=3;
    ImGuiIO &io=ImGui::GetIO();io.DisplaySize=ImVec2((float)desc.Width,(float)desc.Height);io.FontGlobalScale=scale*0.8f;
    LARGE_INTEGER now;QueryPerformanceCounter(&now);
    double delta=(double)(now.QuadPart-lastTime.QuadPart)/frequency.QuadPart;lastTime=now;
    io.DeltaTime=delta>0 && delta<1?(float)delta:1.0f/60;
    ImGuiStyle &style=ImGui::GetStyle();style.WindowPadding=ImVec2(20*scale,18*scale);style.ItemSpacing=ImVec2(10*scale,10*scale);style.WindowRounding=8*scale;
    ImGui_ImplDX11_NewFrame();ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(desc.Width*0.5f,desc.Height*0.5f),ImGuiCond_Always,ImVec2(0.5f,0.5f));
    float menuWidth=1100*scale;if(menuWidth>desc.Width-40*scale)menuWidth=desc.Width-40*scale;
    ImGui::SetNextWindowSize(ImVec2(menuWidth,0),ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.97f);
    ImGui::Begin("InfiniteMod",nullptr,ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_AlwaysAutoResize);
    if(bindingsOpen)drawBindingsMenuContents();else drawMainMenuContents();
    ImGui::End();ImGui::Render();

    // Restore every state touched by the backend, including stages it clears.
    ID3D11RenderTargetView *oldTargets[8]={};ID3D11DepthStencilView *depth=nullptr;
    ID3D11HullShader *hs=nullptr;ID3D11DomainShader *ds=nullptr;ID3D11ComputeShader *cs=nullptr;
    ID3D11ClassInstance *hi[256]={},*di[256]={},*ci[256]={};UINT hc=256,dc=256,cc=256;
    context->HSGetShader(&hs,hi,&hc);context->DSGetShader(&ds,di,&dc);context->CSGetShader(&cs,ci,&cc);
    context->OMGetRenderTargets(8,oldTargets,&depth);context->OMSetRenderTargets(1,&target,nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    context->OMSetRenderTargets(8,oldTargets,depth);
    context->HSSetShader(hs,hi,hc);context->DSSetShader(ds,di,dc);context->CSSetShader(cs,ci,cc);
    for(auto *old:oldTargets)if(old)old->Release();if(depth)depth->Release();
    if(hs)hs->Release();if(ds)ds->Release();if(cs)cs->Release();
    for(UINT i=0;i<hc;i++)if(hi[i])hi[i]->Release();for(UINT i=0;i<dc;i++)if(di[i])di[i]->Release();for(UINT i=0;i<cc;i++)if(ci[i])ci[i]->Release();
    target->Release(); // ResizeBuffers never sees a retained back buffer.
}
static HRESULT STDMETHODCALLTYPE gamePresent(IDXGISwapChain *chain,UINT sync,UINT flags) {
    PresentFn original=(PresentFn)originalFor(&(*(void ***)chain)[8]);
    if(!original)return E_UNEXPECTED;
    if(!(flags&DXGI_PRESENT_TEST) && TryAcquireSRWLockExclusive(&drawLock)) {
        ++frames;
        DXGI_SWAP_CHAIN_DESC desc={};
        if(SUCCEEDED(chain->GetDesc(&desc))) {
            weaponWindow(desc.OutputWindow);
            if(frames==1 || frames==120 || frames==600) modLog("GAME Present #%lu: %ux%u, window=%p, thread=%lu",frames,desc.BufferDesc.Width,desc.BufferDesc.Height,desc.OutputWindow,GetCurrentThreadId());
            bool focused=desc.OutputWindow && GetForegroundWindow()==desc.OutputWindow;
            updateMenuInput(focused);
            if((menuOpen || bindingsOpen) && focused) drawMenu(chain);
        }
        ReleaseSRWLockExclusive(&drawLock);
    }
    HRESULT result=original(chain,sync,flags);
    if(FAILED(result)) modLog("GAME Present failed %08lx",result);
    return result;
}
static HRESULT STDMETHODCALLTYPE gameCreateSwap(IDXGIFactory *factory,IUnknown *dev,DXGI_SWAP_CHAIN_DESC *desc,IDXGISwapChain **out) {
    SwapFn original=(SwapFn)originalFor(&(*(void ***)factory)[10]);if(!original)return E_UNEXPECTED;
    HRESULT result=original(factory,dev,desc,out);
    if(SUCCEEDED(result) && out && *out) {
        bool ok=hookSlot(&(*(void ***)*out)[8],(void *)gamePresent);
        modLog("GAME swap chain created: capture=%s, result=%08lx",ok?"ready":"failed",result);
    } else modLog("GAME swap chain creation failed %08lx",result);
    return result;
}
static HRESULT WINAPI gameCreateFactory(REFIID iid,void **out) {
    FactoryFn original=factoryOriginal;HRESULT result=original(iid,out);
    // The delay loader overwrites its own IAT slot on first use. Re-arm only our slot.
    if(factoryImport && *factoryImport!=(void *)gameCreateFactory) {
        void *resolved=*factoryImport;
        factoryOriginal=(FactoryFn)resolved;
        replacePointer(factoryImport,resolved,(void *)gameCreateFactory);
    }
    if(SUCCEEDED(result) && out && *out) {
        bool ok=hookSlot(&(*(void ***)*out)[10],(void *)gameCreateSwap);
        modLog("GAME DXGI factory created: capture=%s",ok?"ready":"failed");
    }
    return result;
}
static bool captureFactoryImport(void **address) {
    factoryImport=address;factoryOriginal=(FactoryFn)*address;
    return factoryOriginal && replacePointer(address,(void *)factoryOriginal,(void *)gameCreateFactory);
}
bool installGameGraphicsImport(BYTE *image) {
    // Exact inspected GOG x86 import thunk and relocated operand.
    BYTE *thunk=image+0xad6f28;void **address=(void **)(image+0x1379ae8);DWORD operand=0;
    memcpy(&operand,thunk+2,4);
    if(thunk[0]!=0xff || thunk[1]!=0x25 || operand!=(DWORD)(ULONG_PTR)address) {
        modLog("DXGI import signature mismatch; no writes.");return false;
    }
    return captureFactoryImport(address);
}
