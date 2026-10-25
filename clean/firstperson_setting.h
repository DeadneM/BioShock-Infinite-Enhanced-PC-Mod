#pragma once
#include <windows.h>
#include <cwchar>
#include <cstdlib>
namespace firstperson_setting {
inline bool valid(int distance){return distance>=-50 && distance<=50;}
inline int step(int distance,int direction){
    if(!valid(distance))distance=0;
    int value=distance+(direction<0?-2:2);
    return value<-50?-50:value>50?50:value;
}
inline int load(const wchar_t *path){
    wchar_t text[32],*end=nullptr;
    DWORD n=GetPrivateProfileStringW(L"FirstPerson",L"DistanceOffset",L"missing",text,32,path);
    if(!wcscmp(text,L"missing")){
        // 0.30.1 positive BackOffset meant closer. Convert without rewriting INI.
        n=GetPrivateProfileStringW(L"FirstPerson",L"BackOffset",L"0",text,32,path);
        if(!n || n>=31)return 0;long old=wcstol(text,&end,10);
        return end!=text && !*end && old>=0 && old<=10 && old%2==0?-(int)old:0;
    }
    if(!n || n>=31)return 0;long value=wcstol(text,&end,10);
    return end!=text && !*end && valid((int)value)?(int)value:0;
}
inline bool save(const wchar_t *path,volatile LONG &state,int distance){
    if(!path || !*path || !valid(distance))return false;
    wchar_t text[16];swprintf(text,16,L"%d",distance);
    if(!WritePrivateProfileStringW(L"FirstPerson",L"DistanceOffset",text,path))return false;
    InterlockedExchange(&state,distance);return true;
}
}
