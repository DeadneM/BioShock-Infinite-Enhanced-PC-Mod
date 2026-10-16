#pragma once
#include "shared.h"

namespace aim_zoom_setting {
inline LONG load(const wchar_t *path) {
    return path && *path && GetPrivateProfileIntW(L"Display",L"NativeAimZoom",1,path)==1?1:0;
}
inline bool save(const wchar_t *path,volatile LONG &state,bool enabled) {
    // Empty paths must not fall back to the Windows default profile file.
    if(!path || !*path) {
        modLog("Aim zoom setting: save failed (empty path); effective=%s",InterlockedCompareExchange(&state,0,0)?"ON":"OFF");
        return false;
    }
    if(!WritePrivateProfileStringW(L"Display",L"NativeAimZoom",enabled?L"1":L"0",path)) {
        modLog("Aim zoom setting: save failed error=%lu; requested=%s effective=%s",GetLastError(),enabled?"ON":"OFF",InterlockedCompareExchange(&state,0,0)?"ON":"OFF");
        return false;
    }
    InterlockedExchange(&state,enabled);
    modLog("Aim zoom setting: saved; effective=%s",enabled?"ON":"OFF");
    return true;
}
}
