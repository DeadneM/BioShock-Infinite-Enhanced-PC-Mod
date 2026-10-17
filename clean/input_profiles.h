#pragma once
#include <windows.h>
#include <cwchar>
#include <cstdlib>
#include <cmath>
namespace input_profiles {
inline constexpr int steps[]={10,25,50,75,100,125,150,200};
inline constexpr const wchar_t *keys[]={L"MouseNormal",L"MouseADS",L"MouseSniper",L"ControllerNormal",L"ControllerADS",L"ControllerSniper"};
inline bool valid(int percent){for(int step:steps)if(step==percent)return true;return false;}
inline int next(int percent){for(unsigned i=0;i<8;++i)if(steps[i]==percent)return steps[(i+1)%8];return 100;}
inline int load(const wchar_t *path,unsigned index){
    if(index>=6)return 100;
    wchar_t text[32],*end=nullptr;DWORD n=GetPrivateProfileStringW(L"Input",keys[index],L"1.00",text,32,path);
    if(!n || n>=31)return 100;double value=wcstod(text,&end);
    if(end==text || *end)return 100;
    for(int step:steps)if(value==step/100.0)return step;
    return 100;
}
inline bool save(const wchar_t *path,volatile LONG *state,unsigned index,int percent){
    if(!path || !*path || index>=6 || !valid(percent))return false;
    wchar_t text[16];swprintf(text,16,L"%.2f",percent/100.0);
    if(!WritePrivateProfileStringW(L"Input",keys[index],text,path))return false;
    InterlockedExchange(&state[index],percent);return true;
}
}
