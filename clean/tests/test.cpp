#include "../shared.h"
#include "../config.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "../aim.cpp"
#include "../weapons.cpp"
wchar_t modIni[MAX_PATH],gameIni[MAX_PATH];
volatile LONG configWanted=1,configRequest=0,configResult=1;
volatile LONG hudWanted=100,hudApplied=100,hudResult=1,aimWanted=0,aimKey=VK_MBUTTON,modMenuOpen=0;
volatile LONG speedPercent=100,fovDegrees=80;
volatile LONG crosshairHidden=0;
bool saveCrosshairSetting(bool hidden){InterlockedExchange(&crosshairHidden,hidden);return true;}
bool saveSpeedSetting(int value){if(value<100 || value>200 || value%25)return false;InterlockedExchange(&speedPercent,value);return true;}
bool saveFOVSetting(int value){if(value<80 || value>130)return false;InterlockedExchange(&fovDegrees,value);return true;}
bool saveHUDSetting(int percent){InterlockedExchange(&hudWanted,percent);return true;}
bool saveAimSetting(bool enabled){InterlockedExchange(&aimWanted,enabled);return true;}
bool saveAimKey(int key){InterlockedExchange(&aimKey,key);return true;}
void modLog(const char *format,...) {va_list args;va_start(args,format);vprintf(format,args);va_end(args);puts("");}
bool queueIntroSetting(bool enabled) {InterlockedExchange(&configWanted,enabled);return true;}
static HWND focus;
static SHORT keys[256];
static HWND testForeground(){return focus;}
static SHORT testKeys(int key){return keys[key];}
#define GetForegroundWindow testForeground
#define GetAsyncKeyState testKeys
#include "../overlay.cpp"
#undef GetForegroundWindow
#undef GetAsyncKeyState
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);exit(1);}}while(0)
#include "weapons_fixture.h"
#include "aim_fixture.h"
#include "ui_fixture.h"
#include "player_options_fixture.h"
#include "resources_fixture.h"
#include "viewmodel_fixture.h"
#include "reticle_fixture.h"
#include "loot_fixture.h"
#include "upgrades_fixture.h"
#include "gear_fixture.h"
#include "powers_fixture.h"
#include "vigor_upgrades_fixture.h"
#include "infusions_fixture.h"
static FactoryFn importSlot;
static HRESULT WINAPI delayFactory(REFIID iid,void **out) {
    importSlot=CreateDXGIFactory; // Simulate the game's real delay loader replacing the IAT.
    return importSlot(iid,out);
}
static void tap(IDXGISwapChain *chain,int key) {
    keys[key]=(SHORT)0x8000;CHECK(SUCCEEDED(chain->Present(0,0)));keys[key]=0;CHECK(SUCCEEDED(chain->Present(0,0)));
}
static void capture(IDXGISwapChain *chain,const char *path) {
    ID3D11Texture2D *buffer=nullptr,*copy=nullptr;
    CHECK(SUCCEEDED(chain->GetBuffer(0,__uuidof(ID3D11Texture2D),(void **)&buffer)));
    D3D11_TEXTURE2D_DESC desc;buffer->GetDesc(&desc);
    desc.Usage=D3D11_USAGE_STAGING;desc.BindFlags=0;desc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;desc.MiscFlags=0;
    CHECK(SUCCEEDED(device->CreateTexture2D(&desc,nullptr,&copy)));context->CopyResource(copy,buffer);
    D3D11_MAPPED_SUBRESOURCE map;CHECK(SUCCEEDED(context->Map(copy,0,D3D11_MAP_READ,0,&map)));
    BITMAPFILEHEADER h={};BITMAPINFOHEADER info={};h.bfType=0x4d42;h.bfOffBits=sizeof(h)+sizeof(info);h.bfSize=h.bfOffBits+desc.Width*desc.Height*4;
    info.biSize=sizeof(info);info.biWidth=desc.Width;info.biHeight=-(LONG)desc.Height;info.biPlanes=1;info.biBitCount=32;
    FILE *file=fopen(path,"wb");CHECK(file);fwrite(&h,sizeof(h),1,file);fwrite(&info,sizeof(info),1,file);
    for(UINT y=0;y<desc.Height;y++)for(UINT x=0;x<desc.Width;x++){
        BYTE *p=(BYTE *)map.pData+y*map.RowPitch+x*4;BYTE pixel[]={p[2],p[1],p[0],255};fwrite(pixel,4,1,file);
    }
    fclose(file);context->Unmap(copy,0);copy->Release();buffer->Release();
}
int main(int argc,char **argv) {
    testWeapons();
    testPlayerOptions();
    testViewmodel();
    testResources();
    testReticle();
    testLoot();
    testUpgrades();
    testGear();
    testPowers();
    testVigorUpgrades();
    testInfusions();
    testAim();
    testUIScale();
    if(argc==3 && (!strcmp(argv[1],"--apply-config") || !strcmp(argv[1],"--skip-logos"))) {
        wchar_t path[1024];CHECK(MultiByteToWideChar(CP_UTF8,0,argv[2],-1,path,1024));
        int result=enforceConfig(path,true,!strcmp(argv[1],"--skip-logos"));printf("Config result: %d\n",result);return result<0?1:0;
    }
    const std::string fixture="[URL]\r\nPort=7\r\n[FullScreenMovie]\r\nbForceNoMovies=FALSE\r\nStartupMovies=2KLogoSweep720p2997\r\nStartupMovies=LoopingMovieUntilStartupFinishes\r\nSkippableMovies=2KLogoSweep720p2997\r\n[Other]\r\nResolution=3840\r\n";
    std::string edited,again,restored;
    CHECK(editConfig(fixture,true,edited));CHECK(edited!=fixture);
    CHECK(editConfig(edited,true,again));CHECK(again==edited);
    CHECK(editConfig(edited,false,restored));CHECK(restored==fixture);
    CHECK(edited.find("SkippableMovies=LoopingMovie")==std::string::npos);
    CHECK(editConfig(fixture,true,edited,true));
    CHECK(edited.find("; InfiniteMod skipped logo: StartupMovies=2KLogoSweep720p2997")!=std::string::npos);
    CHECK(edited.find("\nStartupMovies=LoopingMovieUntilStartupFinishes\r\n")!=std::string::npos);
    CHECK(editConfig(edited,true,again,true));CHECK(again==edited);
    CHECK(editConfig(edited,false,restored,true));CHECK(restored==fixture);
    std::string engineRewrite="[FullScreenMovie]\nStartupMovies=LoopingMovieUntilStartupFinishes\n[Other]\nResolution=1920\n";
    CHECK(restoreRewrittenLogos(engineRewrite,fixture));
    CHECK(engineRewrite.find("StartupMovies=2KLogoSweep720p2997\nStartupMovies=LoopingMovieUntilStartupFinishes")!=std::string::npos);
    CHECK(engineRewrite.find("Resolution=1920")!=std::string::npos);
    again=engineRewrite;CHECK(restoreRewrittenLogos(engineRewrite,fixture));CHECK(engineRewrite==again);
    engineRewrite="[FullScreenMovie]\n";CHECK(!restoreRewrittenLogos(engineRewrite,fixture));
    CHECK(!editConfig("[Other]\n",true,edited));CHECK(!editConfig(std::string(configBegin),true,edited));
    CHECK(editConfig("\xef\xbb\xbf[FullScreenMovie]\n",true,edited));
    CHECK(edited.find("SkippableMoviesPC=AMDLogo720p")!=std::string::npos);
    for(int encoding=0;encoding<2;encoding++) {
        std::string bytes=fixture;wchar_t path[MAX_PATH];
        swprintf(path,MAX_PATH,L"config-%d-%lu.ini",encoding,GetTickCount());
        if(encoding) {bytes="\xff\xfe";for(char c:fixture){bytes+=c;bytes+='\0';}}
        HANDLE file=CreateFileW(path,GENERIC_WRITE,0,nullptr,CREATE_NEW,0,nullptr);CHECK(file!=INVALID_HANDLE_VALUE);
        DWORD n;CHECK(WriteFile(file,bytes.data(),(DWORD)bytes.size(),&n,nullptr));CloseHandle(file);
        CHECK(enforceConfig(path,true)==1);CHECK(enforceConfig(path,true)==0);CHECK(enforceConfig(path,false)==1);
        std::string read;CHECK(readConfigBytes(path,read));CHECK(read==bytes);
    }
    puts("PASS: INI idempotence, exact restore, loading list preserved, ANSI/UTF16, existing entries, malformed/missing sections.");
    importSlot=delayFactory;CHECK(captureFactoryImport((void **)&importSlot));
    IDXGIFactory *factory=nullptr;CHECK(SUCCEEDED(importSlot(__uuidof(IDXGIFactory),(void **)&factory)));
    CHECK(importSlot==gameCreateFactory); // Delay resolution did not silently remove the hook.
    IDXGIAdapter *adapter=nullptr;CHECK(SUCCEEDED(factory->EnumAdapters(0,&adapter)));
    ID3D11Device *dev=nullptr;ID3D11DeviceContext *ctx=nullptr;
    CHECK(SUCCEEDED(D3D11CreateDevice(adapter,D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&dev,nullptr,&ctx)));
    WNDCLASSW wc={};wc.lpfnWndProc=DefWindowProcW;wc.hInstance=GetModuleHandleW(nullptr);wc.lpszClassName=L"CleanOverlayTest";CHECK(RegisterClassW(&wc));
    HWND window=CreateWindowExW(0,wc.lpszClassName,L"",WS_POPUP,0,0,1280,720,nullptr,nullptr,wc.hInstance,nullptr);CHECK(window);
    DXGI_SWAP_CHAIN_DESC desc={};desc.BufferDesc.Width=1280;desc.BufferDesc.Height=720;desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.SampleDesc.Count=1;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.BufferCount=1;desc.OutputWindow=window;desc.Windowed=TRUE;
    IDXGISwapChain *chain=nullptr;CHECK(SUCCEEDED(factory->CreateSwapChain(dev,&desc,&chain)));
    tap(chain,VK_INSERT);CHECK(!menuOpen && !gui); // No initialization until explicit foreground open.
    focus=window;keys[VK_INSERT]=(SHORT)0x8000;CHECK(SUCCEEDED(chain->Present(0,0)));CHECK(menuOpen && gui);
    CHECK(SUCCEEDED(chain->Present(0,0)));CHECK(menuOpen);keys[VK_INSERT]=0;chain->Present(0,0);
    tap(chain,VK_F1);CHECK(configWanted==0);tap(chain,VK_F1);CHECK(configWanted==1);
    tap(chain,VK_F2);CHECK(selectedWeapon==weaponChoiceCount-1);tap(chain,VK_F3);CHECK(selectedWeapon==0);
    keys[VK_F3]=(SHORT)0x8000;chain->Present(0,0);chain->Present(0,0);CHECK(selectedWeapon==1);keys[VK_F3]=0;chain->Present(0,0);
    tap(chain,VK_F4);CHECK(pending==-1); // Main menu / no live player cannot enqueue.
    tap(chain,VK_F5);CHECK(selectedDestination==1);tap(chain,VK_F5);CHECK(selectedDestination==0);
    tap(chain,VK_F6);CHECK(pending==-1);
    // Ctrl commands must never trigger the corresponding ordinary weapon key.
    available=true;lastSeen=GetTickCount();keys[VK_CONTROL]=(SHORT)0x8000;
    int oldWeapon=selectedWeapon,oldDestination=selectedDestination;
    for(int i=0;i<7;i++) {
        keys[VK_F2+i]=(SHORT)0x8000;chain->Present(0,0);CHECK(pending==-3-i);
        pending=-1;chain->Present(0,0);CHECK(pending==-1); // A held input does not repeat.
        keys[VK_F2+i]=0;chain->Present(0,0);
        CHECK(selectedWeapon==oldWeapon && selectedDestination==oldDestination);
        CHECK(crosshairHidden==0); // Ctrl+F8 must not toggle the reticle setting.
        lastSeen=GetTickCount();
    }
    focus=nullptr;tap(chain,VK_F2);CHECK(pending==-1);focus=window;
    tap(chain,VK_INSERT);tap(chain,VK_F2);CHECK(pending==-1);tap(chain,VK_INSERT);
    keys[VK_CONTROL]=0;available=false;lastSeen=0;
    keys[VK_F8]=(SHORT)0x8000;chain->Present(0,0);chain->Present(0,0);CHECK(crosshairHidden==1);
    keys[VK_F8]=0;chain->Present(0,0);tap(chain,VK_F8);CHECK(crosshairHidden==0);
    focus=nullptr;tap(chain,VK_F8);CHECK(crosshairHidden==0);focus=window;
    tap(chain,VK_F9);CHECK(hudWanted==125 && hudApplied==100);
    tap(chain,VK_F9);CHECK(hudWanted==150);tap(chain,VK_F9);CHECK(hudWanted==200);
    tap(chain,VK_F9);CHECK(hudWanted==100);
    tap(chain,VK_F10);CHECK(aimWanted==1);tap(chain,VK_F10);CHECK(aimWanted==0);
    tap(chain,VK_F11);CHECK(aimKey==VK_RBUTTON);
    tap(chain,VK_SUBTRACT);CHECK(fovDegrees==80);
    keys[VK_ADD]=(SHORT)0x8000;chain->Present(0,0);chain->Present(0,0);CHECK(fovDegrees==85);keys[VK_ADD]=0;chain->Present(0,0);
    for(int i=0;i<12;i++)tap(chain,VK_ADD);CHECK(fovDegrees==130);
    for(int i=0;i<12;i++)tap(chain,VK_SUBTRACT);CHECK(fovDegrees==80);
    for(int expected:{125,150,175,200,100}){tap(chain,VK_F12);CHECK(speedPercent==expected);}
    keys[VK_F12]=(SHORT)0x8000;chain->Present(0,0);chain->Present(0,0);CHECK(speedPercent==125);keys[VK_F12]=0;chain->Present(0,0);
    focus=nullptr;tap(chain,VK_ADD);tap(chain,VK_F12);CHECK(fovDegrees==80 && speedPercent==125);focus=window;
    tap(chain,VK_OEM_PLUS);CHECK(fovDegrees==85);tap(chain,VK_OEM_MINUS);CHECK(fovDegrees==80);
    ID3DBlob *shaderCode=nullptr;const char *shader="RWByteAddressBuffer output:register(u0);[numthreads(1,1,1)]void main(uint3 id:SV_DispatchThreadID){output.Store(0,id.x);}";
    CHECK(SUCCEEDED(D3DCompile(shader,strlen(shader),nullptr,nullptr,nullptr,"main","cs_5_0",0,0,&shaderCode,nullptr)));
    ID3D11ComputeShader *compute=nullptr;CHECK(SUCCEEDED(dev->CreateComputeShader(shaderCode->GetBufferPointer(),shaderCode->GetBufferSize(),nullptr,&compute)));shaderCode->Release();ctx->CSSetShader(compute,nullptr,0);
    const UINT sizes[][2]={{1280,720},{3840,2160},{3440,1440}};
    for(auto &size:sizes) {
        CHECK(SUCCEEDED(chain->ResizeBuffers(0,size[0],size[1],DXGI_FORMAT_UNKNOWN,0)));
        ID3D11Texture2D *buffer=nullptr;ID3D11RenderTargetView *target=nullptr;
        CHECK(SUCCEEDED(chain->GetBuffer(0,__uuidof(ID3D11Texture2D),(void **)&buffer)));
        CHECK(SUCCEEDED(dev->CreateRenderTargetView(buffer,nullptr,&target)));buffer->Release();
        float background[]={0.025f,0.04f,0.07f,1};ctx->ClearRenderTargetView(target,background);ctx->OMSetRenderTargets(1,&target,nullptr);
        drawMenu(chain);drawMenu(chain);CHECK(ImGui::GetDrawData()->TotalVtxCount>0);
        ID3D11RenderTargetView *after=nullptr;ctx->OMGetRenderTargets(1,&after,nullptr);CHECK(after==target);after->Release();
        ID3D11ComputeShader *afterShader=nullptr;ctx->CSGetShader(&afterShader,nullptr,nullptr);CHECK(afterShader==compute);afterShader->Release();
        char path[64];snprintf(path,sizeof(path),"clean-%ux%u.bmp",size[0],size[1]);capture(chain,path);
        ctx->OMSetRenderTargets(0,nullptr,nullptr);target->Release();CHECK(SUCCEEDED(chain->Present(0,0)));
    }
    tap(chain,VK_INSERT);CHECK(!menuOpen);tap(chain,VK_F1);CHECK(configWanted==1);
    tap(chain,VK_ADD);tap(chain,VK_F12);CHECK(fovDegrees==80 && speedPercent==125);
    ctx->CSSetShader(nullptr,nullptr,0);compute->Release();releaseRenderer();
    for(unsigned i=0;i<slotCount;i++)CHECK(replacePointer(slots[i].address,slots[i].replacement,slots[i].original));
    CHECK(replacePointer((void **)&importSlot,(void *)gameCreateFactory,(void *)factoryOriginal));
    chain->Release();ctx->Release();dev->Release();adapter->Release();factory->Release();DestroyWindow(window);
    puts("PASS: real factory/device/swapchain capture, delay-IAT rearm, foreground/key edges, shader/target restoration, menu rendering, 720p/4K/ultrawide resize, release without retained buffers.");
    return 0;
}
