#include <windows.h>
#include <shlobj.h>
#include <tlhelp32.h>
#include <stdio.h>
#include "native_fov_config.h"
int main() {
    int count=0;wchar_t **args=CommandLineToArgvW(GetCommandLineW(),&count);
    if(!args || count!=3 || (wcscmp(args[1],L"apply") && wcscmp(args[1],L"restore"))) {
        fwprintf(stderr,L"Usage: NativeFOV.exe apply|restore <game directory>\n");if(args)LocalFree(args);return 1;
    }
    HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(snapshot==INVALID_HANDLE_VALUE){LocalFree(args);return 1;}
    PROCESSENTRY32W process={};process.dwSize=sizeof(process);bool running=false;
    for(BOOL ok=Process32FirstW(snapshot,&process);ok;ok=Process32NextW(snapshot,&process))
        if(!_wcsicmp(process.szExeFile,L"BioShockInfinite.exe")){running=true;break;}
    CloseHandle(snapshot);
    if(running){fwprintf(stderr,L"Close the game normally first.\n");LocalFree(args);return 1;}
    wchar_t documents[MAX_PATH],path[MAX_PATH],ini[MAX_PATH];
    if(FAILED(SHGetFolderPathW(nullptr,CSIDL_PERSONAL,nullptr,SHGFP_TYPE_CURRENT,documents))
        || wcslen(documents)>MAX_PATH-90 || wcslen(args[2])>MAX_PATH-50){LocalFree(args);return 1;}
    swprintf(path,MAX_PATH,L"%ls\\My Games\\BioShock Infinite\\XGame\\Config\\XUserOptions.ini",documents);
    swprintf(ini,MAX_PATH,L"%ls\\Binaries\\Win32\\InfiniteMod.ini",args[2]);
    if(GetFileAttributesW(ini)==INVALID_FILE_ATTRIBUTES){fwprintf(stderr,L"The installed mod INI is missing.\n");LocalFree(args);return 1;}
    bool restore=!wcscmp(args[1],L"restore");
    int preference=GetPrivateProfileIntW(L"Display",L"FOV",80,ini);
    int result=applyNativeFOV(path,preference,restore);LocalFree(args);
    if(result<0){fwprintf(stderr,L"Native FOV option was not changed: missing, ambiguous or unwritable configuration/backup.\n");return 1;}
    wprintf(L"Native FOV range %ls. Only MaxUserFOVOffsetPercent is managed; slider and other settings preserved.\n",restore?L"restored":L"configured");
    wprintf(L"Original native configuration: %ls.InfiniteMod-native-fov-backup\n",path);return 0;
}
