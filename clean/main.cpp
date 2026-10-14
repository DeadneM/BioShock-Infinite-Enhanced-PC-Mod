#include "shared.h"
#include "config.h"
#include "weapons.h"
#include "aim.h"
#include "ui_config.h"
#include "aim_zoom_setting.h"
#include <shlobj.h>
#include <stdio.h>
#include <stdarg.h>
wchar_t modIni[MAX_PATH],gameIni[MAX_PATH];
volatile LONG configWanted=1,configRequest=0,configResult=0;
volatile LONG hudWanted=100,hudApplied=0,hudResult=0,aimWanted=0,aimKey=VK_MBUTTON,modMenuOpen=0;
volatile LONG speedPercent=100,fovDegrees=80;
volatile LONG nativeAimZoom=0;
bool saveNativeAimZoom(bool enabled) {
    return aim_zoom_setting::save(modIni,nativeAimZoom,enabled);
}
volatile LONG crosshairHidden=0;
bool saveCrosshairSetting(bool hidden) {
    if(!WritePrivateProfileStringW(L"Display",L"HideCrosshair",hidden?L"1":L"0",modIni))return false;
    InterlockedExchange(&crosshairHidden,hidden);return true;
}
static const int speedSteps[]={100,125,150,175,200};
static const wchar_t *speedValues[]={L"1.00",L"1.25",L"1.50",L"1.75",L"2.00"};
bool saveSpeedSetting(int percent) {
    for(int i=0;i<5;i++)if(speedSteps[i]==percent) {
        if(!WritePrivateProfileStringW(L"Player",L"SpeedMultiplier",speedValues[i],modIni))return false;
        InterlockedExchange(&speedPercent,percent);return true;
    }
    return false;
}
bool saveFOVSetting(int degrees) {
    if(degrees<80 || degrees>130)return false;
    wchar_t value[16];swprintf(value,16,L"%d",degrees);
    if(!WritePrivateProfileStringW(L"Display",L"FOV",value,modIni))return false;
    InterlockedExchange(&fovDegrees,degrees);return true;
}
static void loadPlayerSettings() {
    wchar_t value[32],*end=nullptr;
    LONG speed=100,fov=80;
    DWORD count=GetPrivateProfileStringW(L"Player",L"SpeedMultiplier",L"1.00",value,32,modIni);
    if(count && count<31) {
        double multiplier=wcstod(value,&end);
        if(end!=value && *end==0)for(int step:speedSteps)if(multiplier==(double)step/100.0){speed=step;break;}
    }
    count=GetPrivateProfileStringW(L"Display",L"FOV",L"80",value,32,modIni);
    if(count && count<31) {
        long degrees=wcstol(value,&end,10);
        if(end!=value && *end==0 && degrees>=80 && degrees<=130)fov=degrees;
    }
    InterlockedExchange(&speedPercent,speed);InterlockedExchange(&fovDegrees,fov);
    // New key: a rejected experiment's ProportionalAim setting cannot enable this.
    InterlockedExchange(&nativeAimZoom,aim_zoom_setting::load(modIni));
}
bool saveHUDSetting(int percent) {
    if(!nativeUIScale(percent))return false;
    wchar_t value[16];swprintf(value,16,L"%.2f",(double)percent/100);
    if(!WritePrivateProfileStringW(L"Display",L"HUDScale",value,modIni))return false;
    InterlockedExchange(&hudWanted,percent);return true;
}
bool saveAimSetting(bool enabled) {
    if(!WritePrivateProfileStringW(L"Controls",L"HoldAim",enabled?L"1":L"0",modIni))return false;
    InterlockedExchange(&aimWanted,enabled);return true;
}
bool saveAimKey(int key) {
    wchar_t value[16];swprintf(value,16,L"%d",key);
    if(!WritePrivateProfileStringW(L"Controls",L"AimKey",value,modIni))return false;
    InterlockedExchange(&aimKey,key);return true;
}
static HMODULE self;
static HANDLE logFile=INVALID_HANDLE_VALUE;
void modLog(const char *format,...) {
    char body[1024],line[1152];SYSTEMTIME time;GetLocalTime(&time);
    va_list args;va_start(args,format);vsnprintf(body,sizeof(body),format,args);va_end(args);
    int n=snprintf(line,sizeof(line),"%02u:%02u:%02u | %s\r\n",time.wHour,time.wMinute,time.wSecond,body);
    DWORD written;if(logFile!=INVALID_HANDLE_VALUE && n>0) WriteFile(logFile,line,(DWORD)n,&written,nullptr);
}
bool queueIntroSetting(bool enabled) {
    if(!WritePrivateProfileStringW(L"Intros",L"SkipIntroLogos",enabled?L"1":L"0",modIni)) return false;
    InterlockedExchange(&configWanted,enabled);InterlockedExchange(&configRequest,1);return true;
}
static DWORD WINAPI start(void *) {
    HMODULE pin; if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,(LPCWSTR)&start,&pin)) return 1;
    wchar_t path[MAX_PATH];DWORD len=GetModuleFileNameW(self,path,MAX_PATH);
    if(!len || len>MAX_PATH-32) return 2;
    wchar_t *slash=wcsrchr(path,L'\\');if(!slash)return 3;slash[1]=0;
    swprintf(modIni,MAX_PATH,L"%lsInfiniteMod.ini",path);
    wchar_t logPath[MAX_PATH];swprintf(logPath,MAX_PATH,L"%lsInfiniteMod.log",path);
    logFile=CreateFileW(logPath,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_ALWAYS,0,nullptr);
    modLog("InfiniteMod clean 0.24.1 EXPERIMENTAL; accepted English 0.18.1 base, explicit ON/OFF aim control; aiming experiment still unvalidated.");
    if(!GetPrivateProfileIntW(L"General",L"Enabled",1,modIni)) {modLog("Disabled.");return 0;}
    BYTE *image=(BYTE *)GetModuleHandleW(nullptr);
    auto *dos=(IMAGE_DOS_HEADER *)image;
    auto *nt=(IMAGE_NT_HEADERS32 *)(image+dos->e_lfanew);
    if(nt->FileHeader.Machine!=IMAGE_FILE_MACHINE_I386 || nt->FileHeader.TimeDateStamp!=0x53263b17 || nt->OptionalHeader.SizeOfImage!=0x1642000) {
        modLog("Unsupported host: no hooks and no configuration edits.");return 0;
    }
    // Capture the game's factory before disk work. No device or window is created here.
    if(GetPrivateProfileIntW(L"Overlay",L"Enabled",1,modIni))
        modLog("Graphics import capture: %s",installGameGraphicsImport(image)?"ready; waiting for game's factory":"failed");
    loadPlayerSettings();
    bool commands=installWeapons(image);
    modLog("Gameplay command initialization: %s",commands?"ready":"unavailable; no gameplay calls");
    if(commands)installAim(image);
    wchar_t documents[MAX_PATH];
    if(SUCCEEDED(SHGetFolderPathW(nullptr,CSIDL_PERSONAL,nullptr,SHGFP_TYPE_CURRENT,documents)) && wcslen(documents)<MAX_PATH-72)
        swprintf(gameIni,MAX_PATH,L"%ls\\My Games\\BioShock Infinite\\XGame\\Config\\XEngine.ini",documents);
    configWanted=GetPrivateProfileIntW(L"Intros",L"SkipIntroLogos",1,modIni)!=0;
    crosshairHidden=GetPrivateProfileIntW(L"Display",L"HideCrosshair",0,modIni)!=0;
    // The old boolean selected an inverted, excessive ceiling. Do not migrate it.
    wchar_t hudValue[32],*hudEnd=nullptr;
    DWORD hudCount=GetPrivateProfileStringW(L"Display",L"HUDScale",L"1.00",hudValue,32,modIni);
    double hudScale=wcstod(hudValue,&hudEnd);hudWanted=100;
    if(hudCount && hudCount<31 && hudEnd!=hudValue && *hudEnd==0)
        for(int percent:{100,125,150,200})if(hudScale==(double)percent/100)hudWanted=percent;
    aimWanted=GetPrivateProfileIntW(L"Controls",L"HoldAim",0,modIni)!=0;
    aimKey=GetPrivateProfileIntW(L"Controls",L"AimKey",VK_MBUTTON,modIni);
    if(aimKey<1 || aimKey>254)aimKey=VK_MBUTTON;
    float expectedUICap=1;bool uiCheck=false;DWORD uiCheckStart=GetTickCount();
    wchar_t uiPath[MAX_PATH]={};wcscpy(uiPath,gameIni);wchar_t *uiSlash=wcsrchr(uiPath,L'\\');
    if(uiSlash) {
        wcscpy(uiSlash+1,L"XUI.ini");int result=enforceUIScale(uiPath,(int)hudWanted);
        hudResult=result<0?-1:2;
        if(result>=0) {
            std::string bytes,text,value;bool wide=false;
            if(readConfigBytes(uiPath,bytes) && decodeUIConfig(bytes,text,wide) && uiScaleValue(text,value)) {
                expectedUICap=1.0f+(float)strtod(value.c_str(),nullptr);uiCheck=true;
            } else hudResult=-1;
        }
        modLog("Native UI startup: requested=%ld%%, result=%d; no live changes",hudWanted,result);
    }
    InterlockedExchange(&configRequest,1);
    for(;;) {
        if(uiCheck) {
            float current=0;SIZE_T read=0;
            if(ReadProcessMemory(GetCurrentProcess(),image+0x135aca0,&current,sizeof(current),&read)
                && read==sizeof(current) && current==expectedUICap) {
                InterlockedExchange(&hudApplied,(LONG)(expectedUICap*100));InterlockedExchange(&hudResult,1);uiCheck=false;
                modLog("Native UI cap observed: %.3f",current);
            } else if(GetTickCount()-uiCheckStart>60000) {
                InterlockedExchange(&hudResult,-1);uiCheck=false;modLog("Native UI cap not confirmed: expected %.3f observed %.3f",expectedUICap,current);
            }
        }
        if(InterlockedExchange(&configRequest,0)) {
            int result=enforceConfig(gameIni,InterlockedCompareExchange(&configWanted,0,0)!=0,true);
            InterlockedExchange(&configResult,result<0?-1:result+1);
            modLog("INI startup logos: %s; no decoder/code changes.",result<0?"FAILED":result?"updated; restart required if already loaded":"verified unchanged");
        }
        Sleep(100);
    }
}
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,LPVOID) {
    if(reason==DLL_PROCESS_ATTACH) {self=instance;DisableThreadLibraryCalls(instance);HANDLE worker=CreateThread(nullptr,0,start,nullptr,0,nullptr);if(worker)CloseHandle(worker);}
    return TRUE;
}
