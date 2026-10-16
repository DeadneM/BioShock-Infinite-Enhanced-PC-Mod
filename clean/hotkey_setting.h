#pragma once
#include <windows.h>
inline bool readGlobalHotkeys(const wchar_t *path) {
    return GetPrivateProfileIntW(L"Controls",L"GlobalHotkeys",1,path)!=0;
}
inline bool persistGlobalHotkeys(const wchar_t *path,bool enabled) {
    return WritePrivateProfileStringW(L"Controls",L"GlobalHotkeys",enabled?L"1":L"0",path)!=FALSE;
}
