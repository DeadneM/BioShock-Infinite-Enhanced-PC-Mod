#include "shared.h"
#include "weapons.h"
#include "aim.h"
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
static bool menuOpen,insertHeld,f1Held,settingError;
static bool f2Held,f3Held,f4Held,f5Held,f6Held,fovMinusHeld,fovPlusHeld,f9Held,f10Held,f11Held,f12Held;
static bool vigorUpgradesHeld,reticleHeld;
static int selectedWeapon,selectedDestination;
static const int aimKeys[]={VK_MBUTTON,VK_RBUTTON,'Z',VK_XBUTTON1,VK_XBUTTON2};
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
    ImGui::SetNextWindowSize(ImVec2(740*scale,0),ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.97f);
    ImGui::Begin("InfiniteMod",nullptr,ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::TextColored(ImVec4(0.96f,0.72f,0.3f,1),"INFINITEMOD");
    ImGui::TextDisabled("Inser : ouvrir / fermer");ImGui::Separator();
    ImGui::TextColored(ImVec4(0.96f,0.72f,0.3f,1),"ARMES");
    ImGui::Text("F2 / F3   Choisir : %s",weaponChoices[selectedWeapon].label);
    ImGui::Text("F5        Emplacement : %s",selectedDestination?"SECONDAIRE":"EN MAIN");
    ImGui::Text("F4        Placer l'arme dans cet emplacement");
    ImGui::Text("F6        Remplir les munitions des deux armes");
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.96f,0.72f,0.3f,1),"PROGRESSION");
    ImGui::Text("Ctrl + F2   Tous les pouvoirs");
    ImGui::Text("Ctrl + F3   Tous les cadeaux (equipements)");
    ImGui::Text("Ctrl + F4   Toutes les ameliorations d'armes");
    ImGui::Text("Ctrl + F5   Argent au maximum");
    ImGui::Text("Ctrl + F6   Crochets au maximum");
    ImGui::Text("Ctrl + F7   Toutes les ameliorations de pouvoirs");
    ImGui::Text("Ctrl + F8   Infusions sante, sels et bouclier au maximum");
    char weaponText[192];weaponStatus(weaponText,sizeof(weaponText));ImGui::TextWrapped("%s",weaponText);
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.96f,0.72f,0.3f,1),"DEPLACEMENT ET FOV");
    ImGui::Text("F12       Vitesse du joueur : x%.2f",(double)InterlockedCompareExchange(&speedPercent,0,0)/100.0);
    ImGui::Text("- / +     Champ de vision : %ld deg",InterlockedCompareExchange(&fovDegrees,0,0));
    char optionText[160];playerOptionsStatus(optionText,sizeof(optionText));ImGui::TextWrapped("%s",optionText);
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.96f,0.72f,0.3f,1),"AFFICHAGE ET VISEE");
    bool hideCrosshair=InterlockedCompareExchange(&crosshairHidden,0,0)!=0;
    LONG reticleState=crosshairStatus();
    ImGui::Text("F8   Masquer les reticules : %s",!hideCrosshair?"NON":reticleState>0?"OUI":reticleState<0?"INDISPONIBLE":"EN ATTENTE");
    if(hideCrosshair && reticleState<0)ImGui::TextColored(ImVec4(1,0.4f,0.35f,1),"Masquage indisponible : voir le journal.");
    LONG hud=InterlockedCompareExchange(&hudWanted,0,0),applied=InterlockedCompareExchange(&hudApplied,0,0);
    if(hud==100)ImGui::Text("F9   Taille interface : compacte (native)");
    else ImGui::Text("F9   Taille interface : jusqu'a x%.2f",(double)hud/100);
    ImGui::TextDisabled("Compacte = taille native fixe. Concerne le HUD et les menus.");
    if(InterlockedCompareExchange(&hudResult,0,0)<0)ImGui::TextColored(ImVec4(1,0.4f,0.35f,1),"Reglage HUD non applique : voir le journal.");
    else if(InterlockedCompareExchange(&hudResult,0,0)==2)ImGui::TextDisabled("Verification du reglage HUD au demarrage...");
    else if(hud!=applied)ImGui::TextDisabled("Changement enregistre : relancer le jeu.");
    else ImGui::TextDisabled("Taille confirmee. Tout changement demande de relancer le jeu.");
    ImGui::Text("F10  Visee maintenue : %s",InterlockedCompareExchange(&aimWanted,0,0)?"ON":"OFF");
    if(!aimReady())ImGui::TextColored(ImVec4(1,0.4f,0.35f,1),"Visee maintenue indisponible : voir le journal.");
    ImGui::Text("F11  Touche de visee : %s",aimKeyLabel((int)InterlockedCompareExchange(&aimKey,0,0)));
    ImGui::TextDisabled("Choisir la meme touche que dans les commandes du jeu.");
    ImGui::Separator();
    bool wanted=InterlockedCompareExchange(&configWanted,0,0)!=0;
    ImGui::Text("F1   Passer les logos au demarrage : %s",wanted?"OUI":"NON");
    ImGui::TextDisabled("Chargement initial conserve. Modification au prochain lancement.");
    LONG status=InterlockedCompareExchange(&configResult,0,0);
    if(status==1) ImGui::TextColored(ImVec4(0.4f,0.86f,0.62f,1),"Configuration verifiee.");
    else if(status==2) ImGui::TextColored(ImVec4(0.96f,0.72f,0.3f,1),"Configuration mise a jour. Relancer le jeu.");
    else if(status<0 || settingError) ImGui::TextColored(ImVec4(1,0.4f,0.35f,1),"Enregistrement impossible. Voir le journal.");
    else ImGui::TextDisabled("Verification de la configuration...");
    ImGui::Separator();ImGui::TextDisabled("Menu au clavier. Le jeu continue en arriere-plan.");
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
            bool insert=(GetAsyncKeyState(VK_INSERT)&0x8000)!=0,f1=(GetAsyncKeyState(VK_F1)&0x8000)!=0;
            if(focused && insert && !insertHeld) {menuOpen=!menuOpen;InterlockedExchange(&modMenuOpen,menuOpen);modLog("Overlay %s",menuOpen?"opened":"closed");}
            if(focused && menuOpen && f1 && !f1Held) settingError=!queueIntroSetting(!InterlockedCompareExchange(&configWanted,0,0));
            bool f2=(GetAsyncKeyState(VK_F2)&0x8000)!=0,f3=(GetAsyncKeyState(VK_F3)&0x8000)!=0,f4=(GetAsyncKeyState(VK_F4)&0x8000)!=0;
            bool f5=(GetAsyncKeyState(VK_F5)&0x8000)!=0,f6=(GetAsyncKeyState(VK_F6)&0x8000)!=0;
            bool vigorUpgrade=(GetAsyncKeyState(VK_F7)&0x8000)!=0,reticle=(GetAsyncKeyState(VK_F8)&0x8000)!=0;
            bool f7=((GetAsyncKeyState(VK_SUBTRACT)|GetAsyncKeyState(VK_OEM_MINUS))&0x8000)!=0,f8=((GetAsyncKeyState(VK_ADD)|GetAsyncKeyState(VK_OEM_PLUS))&0x8000)!=0,f12=(GetAsyncKeyState(VK_F12)&0x8000)!=0;
            bool f9=(GetAsyncKeyState(VK_F9)&0x8000)!=0,f10=(GetAsyncKeyState(VK_F10)&0x8000)!=0,f11=(GetAsyncKeyState(VK_F11)&0x8000)!=0;
            if(focused && menuOpen) {
                bool control=(GetAsyncKeyState(VK_CONTROL)&0x8000)!=0;
                if(control) {
                    if(f2 && !f2Held)requestProgression(ProgressionAction::Powers);
                    if(f3 && !f3Held)requestProgression(ProgressionAction::Gear);
                    if(f4 && !f4Held)requestProgression(ProgressionAction::WeaponUpgrades);
                    if(f5 && !f5Held)requestProgression(ProgressionAction::Money);
                    if(f6 && !f6Held)requestProgression(ProgressionAction::Lockpicks);
                    if(vigorUpgrade && !vigorUpgradesHeld)requestProgression(ProgressionAction::VigorUpgrades);
                    if(reticle && !reticleHeld)requestProgression(ProgressionAction::Infusions);
                } else {
                if(f2 && !f2Held)selectedWeapon=(selectedWeapon+weaponChoiceCount-1)%weaponChoiceCount;
                if(f3 && !f3Held)selectedWeapon=(selectedWeapon+1)%weaponChoiceCount;
                if(f5 && !f5Held)selectedDestination=1-selectedDestination;
                if(f4 && !f4Held)requestWeapon(selectedWeapon,selectedDestination);
                if(f6 && !f6Held)requestAmmo();
                }
                if(f7 && !fovMinusHeld) {
                    int value=(int)InterlockedCompareExchange(&fovDegrees,0,0);
                    if(value>80)settingError=!saveFOVSetting(value<85?80:value-5);
                }
                if(f8 && !fovPlusHeld) {
                    int value=(int)InterlockedCompareExchange(&fovDegrees,0,0);
                    if(value<130)settingError=!saveFOVSetting(value>125?130:value+5);
                }
                if(f12 && !f12Held) {
                    int value=(int)InterlockedCompareExchange(&speedPercent,0,0);
                    settingError=!saveSpeedSetting(value>=100 && value<200 && value%25==0?value+25:100);
                }
                if(!control && reticle && !reticleHeld)settingError=!saveCrosshairSetting(!InterlockedCompareExchange(&crosshairHidden,0,0));
                if(f9 && !f9Held) {
                    int value=(int)InterlockedCompareExchange(&hudWanted,0,0);
                    settingError=!saveHUDSetting(value==100?125:value==125?150:value==150?200:100);
                }
                if(f10 && !f10Held)settingError=!saveAimSetting(!InterlockedCompareExchange(&aimWanted,0,0));
                if(f11 && !f11Held) {
                    int old=(int)InterlockedCompareExchange(&aimKey,0,0),next=0;
                    for(int i=0;i<5;i++)if(aimKeys[i]==old){next=(i+1)%5;break;}
                    settingError=!saveAimKey(aimKeys[next]);
                }
            }
            f2Held=f2;f3Held=f3;f4Held=f4;
            f5Held=f5;f6Held=f6;f9Held=f9;f10Held=f10;f11Held=f11;
            vigorUpgradesHeld=vigorUpgrade;reticleHeld=reticle;
            fovMinusHeld=f7;fovPlusHeld=f8;f12Held=f12;
            insertHeld=insert;f1Held=f1;
            if(menuOpen && focused) drawMenu(chain);
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
