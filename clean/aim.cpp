#include "aim.h"
#include "shared.h"
#include <stdint.h>
#include <string.h>

namespace aim_detail {
using ZoomFn=int (__thiscall *)(void *);
ZoomFn startZoom,stopZoom;
volatile LONG installed;
constexpr unsigned heldMask=0x20000,zoomMask=0x70000;
const BYTE startSignature[]={
    0x56,0x8b,0xf1,0x80,0xbe,0x86,0,0,0,3,0x73,5,0xe8,0xaf,0x80,0xd4,0xff,
    0x8b,0x8e,0xfc,1,0,0,0x81,0x8e,0x90,6,0,0,0,0,2,0,0x5e,0x85,0xc9,
    0x74,7,0x6a,0,0xe8,0x73,0x3b,0xfb,0xff,0xb8,1,0,0,0,0xc3
};
const BYTE stopSignature[]={
    0x56,0x8b,0xf1,0x80,0xbe,0x86,0,0,0,3,0x73,5,0xe8,0x0f,0xfb,0x18,0,
    0x81,0xa6,0x90,6,0,0,0xff,0xff,0xf8,0xff,0xb8,1,0,0,0,0x5e,0xc3
};
bool copy(uintptr_t address,void *out,size_t size) {
    SIZE_T got=0;
    return address && ReadProcessMemory(GetCurrentProcess(),(void *)address,out,size,&got) && got==size;
}
enum class Action {None,Start,Stop};
struct State {
    // Identity values are compared, never dereferenced after this update.
    uintptr_t controller=0,pawn=0;
    int key=0;
    bool enabled=false,blocked=false,held=false;
    Action sample(uintptr_t currentPC,uintptr_t currentPawn,int currentKey,
                  bool wanted,bool allowed,bool down,unsigned flags) {
        bool releasePrevious=controller==currentPC && pawn==currentPawn && enabled
            && (held || (flags&zoomMask));
        if(controller!=currentPC || pawn!=currentPawn || key!=currentKey) {
            *this={};controller=currentPC;pawn=currentPawn;key=currentKey;
            blocked=down;
        }
        if(!wanted) {
            bool release=releasePrevious || (enabled && (held || (flags&zoomMask)));
            enabled=false;held=false;blocked=down;
            return release?Action::Stop:Action::None;
        }
        if(!enabled) {enabled=true;blocked=down;held=false;}
        if(!allowed) {
            // A key held across alt-tab or a menu must be released first.
            blocked=true;held=false;
            return flags&zoomMask?Action::Stop:Action::None;
        }
        if(blocked) {
            if(!down)blocked=false;
            held=false;
            return flags&zoomMask?Action::Stop:Action::None;
        }
        if(down) {
            bool start=!held || !(flags&heldMask);
            held=true;
            return start?Action::Start:Action::None;
        }
        // The game's toggle may have consumed a complete click between two
        // camera frames. Reconcile native state even if we never saw its press.
        bool release=held || (flags&zoomMask);
        held=false;
        return release?Action::Stop:Action::None;
    }
} state;
}

bool installAim(BYTE *image) {
    using namespace aim_detail;
    InterlockedExchange(&installed,0);
    startZoom=nullptr;stopZoom=nullptr;state={};
    BYTE start[sizeof(startSignature)],stop[sizeof(stopSignature)];
    bool valid=image && copy((uintptr_t)image+0x4e4990,start,sizeof(start))
        && copy((uintptr_t)image+0x649e60,stop,sizeof(stop))
        && !memcmp(start,startSignature,sizeof(start)) && !memcmp(stop,stopSignature,sizeof(stop));
    if(valid) {
        startZoom=(ZoomFn)(image+0x4e4990);stopZoom=(ZoomFn)(image+0x649e60);
        InterlockedExchange(&installed,1);
    }
    modLog("Hold aim: native start/stop %s; no input bindings changed",valid?"verified":"unavailable");
    return valid;
}

bool aimReady() {return InterlockedCompareExchange(&aim_detail::installed,0,0)!=0;}

void updateAim(void *currentPC,HWND window,bool playerReady) {
    using namespace aim_detail;
    // Invalid actors are never called, even to clean up the previous session.
    // The engine owns destruction/reset of an actor after world transitions.
    if(!aimReady() || !startZoom || !stopZoom || !playerReady || !currentPC) {state={};return;}
    int key=(int)InterlockedCompareExchange(&aimKey,0,0);
    bool wanted=InterlockedCompareExchange(&aimWanted,0,0)!=0 && key>=1 && key<=254;
    if(!wanted && !state.enabled) {state={};return;}
    uintptr_t pc=(uintptr_t)currentPC,pawn=0;unsigned flags=0;
    if(!copy(pc+0x1fc,&pawn,sizeof(pawn)) || !pawn || !copy(pc+0x690,&flags,sizeof(flags))) {state={};return;}
    // Cheap live checks complement the caller's cached reflection validation.
    uintptr_t owner=0;float health=0;
    if(!copy(pawn+0x218,&owner,sizeof(owner)) || owner!=pc
        || !copy(pawn+0x2ac,&health,sizeof(health)) || !(health>0 && health<100000)) {state={};return;}
    bool down=key>=1 && key<=254 && (GetAsyncKeyState(key)&0x8000)!=0;
    bool allowed=window && GetForegroundWindow()==window && !InterlockedCompareExchange(&modMenuOpen,0,0);
    Action action=state.sample(pc,pawn,key,wanted,allowed,down,flags);
    if(action==Action::Start)startZoom(currentPC);
    else if(action==Action::Stop)stopZoom(currentPC);
}
