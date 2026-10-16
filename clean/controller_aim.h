#pragma once
#include <windows.h>
#include <xinput.h>
#include <cwchar>
namespace controller_aim {
inline constexpr const wchar_t *names[]={L"LT",L"RT",L"LB",L"RB",L"L3",L"R3",L"A",L"B",L"X",L"Y"};
inline constexpr const char *labels[]={"Left trigger","Right trigger","Left bumper","Right bumper","Left stick click","Right stick click","A","B","X","Y"};
inline constexpr unsigned count=10;
inline bool valid(int index){return index>=0 && index<(int)count;}
inline int load(const wchar_t *path) {
    wchar_t value[32];GetPrivateProfileStringW(L"Controls",L"ControllerAimButton",L"LT",value,32,path);
    for(unsigned i=0;i<count;++i)if(!_wcsicmp(value,names[i]))return (int)i;
    return 0;
}
inline bool save(const wchar_t *path,volatile LONG &state,int index) {
    if(!path || !*path || !valid(index) || !WritePrivateProfileStringW(L"Controls",L"ControllerAimButton",names[index],path))return false;
    InterlockedExchange(&state,index);return true;
}
inline bool pressed(const XINPUT_GAMEPAD &pad,int binding,unsigned threshold) {
    if(!valid(binding) || threshold>254)return false;
    if(binding==0)return pad.bLeftTrigger>threshold;
    if(binding==1)return pad.bRightTrigger>threshold;
    constexpr WORD buttons[]={XINPUT_GAMEPAD_LEFT_SHOULDER,XINPUT_GAMEPAD_RIGHT_SHOULDER,
        XINPUT_GAMEPAD_LEFT_THUMB,XINPUT_GAMEPAD_RIGHT_THUMB,XINPUT_GAMEPAD_A,XINPUT_GAMEPAD_B,XINPUT_GAMEPAD_X,XINPUT_GAMEPAD_Y};
    return (pad.wButtons&buttons[binding-2])!=0;
}
using Query=DWORD (WINAPI *)(DWORD,XINPUT_STATE *);
inline bool down(Query query,int binding,unsigned threshold) {
    if(!query || !valid(binding))return false;
    for(DWORD i=0;i<4;++i){XINPUT_STATE state={};if(query(i,&state)==ERROR_SUCCESS && pressed(state.Gamepad,binding,threshold))return true;}
    return false;
}
}
