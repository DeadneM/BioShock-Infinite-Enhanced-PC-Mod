#pragma once
#include <windows.h>
#include <cwchar>
#include <cstdlib>
namespace firstperson_setting {
inline bool valid(int back){return back>=0 && back<=10 && back%2==0;}
inline int next(int back){return valid(back) && back<10?back+2:0;}
inline int load(const wchar_t *path){
    wchar_t text[32],*end=nullptr;
    DWORD n=GetPrivateProfileStringW(L"FirstPerson",L"BackOffset",L"0",text,32,path);
    if(!n || n>=31)return 0;long value=wcstol(text,&end,10);
    return end!=text && !*end && valid((int)value)?(int)value:0;
}
inline bool save(const wchar_t *path,volatile LONG &state,int back){
    if(!path || !*path || !valid(back))return false;
    wchar_t text[16];swprintf(text,16,L"%d",back);
    if(!WritePrivateProfileStringW(L"FirstPerson",L"BackOffset",text,path))return false;
    InterlockedExchange(&state,back);return true;
}
}
