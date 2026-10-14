// Private filesystem integration entry point. Never shipped in a package.
#include <windows.h>
#include <shellapi.h>
#include "../native_fov_config.h"
int main() {
    int count=0;wchar_t **args=CommandLineToArgvW(GetCommandLineW(),&count);
    if(!args || count!=3){if(args)LocalFree(args);return 1;}
    wchar_t path[MAX_PATH],ini[MAX_PATH];
    if(wcslen(args[2])>MAX_PATH-60){LocalFree(args);return 1;}
    swprintf(path,MAX_PATH,L"%ls\\native-options.ini",args[2]);
    swprintf(ini,MAX_PATH,L"%ls\\Binaries\\Win32\\InfiniteMod.ini",args[2]);
    bool restore=!wcscmp(args[1],L"restore");
    if(!restore && wcscmp(args[1],L"apply")){LocalFree(args);return 1;}
    int result=applyNativeFOV(path,GetPrivateProfileIntW(L"Display",L"FOV",80,ini),restore);
    LocalFree(args);return result<0?1:0;
}
