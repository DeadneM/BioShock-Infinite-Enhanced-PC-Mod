// Common first-person projection. Native component/proxy transforms stay untouched.
#include "foreground_math.h"
#include <stddef.h>
namespace viewmodel {
using foreground_math::Matrix;
using foreground_math::Fields;
volatile LONG state=0;
bool installed=false;
struct Identity {Obj pc=0,pawn=0,local=0,viewState=0;bool ready=false;} identity;
struct Pending {Obj pawn=0,viewState=0;float native=0,world=0;DWORD time=0;bool valid=false;} pending;
struct Record {Obj view=0,pawn=0,viewState=0;Matrix viewMatrix={};float px=0,py=0,ratio=1,native=0,world=0;DWORD time=0;bool valid=false;};
static Record records[256];static unsigned nextRecord=0;
static SRWLOCK registryLock=SRWLOCK_INIT;
static thread_local bool insidePass=false;
static float lastReported=0;
struct CtorArgs {DWORD values[24];};
static_assert(sizeof(CtorArgs)==0x60,"Native FSceneView constructor stack layout");
using CtorFn=Obj (__thiscall *)(void *,CtorArgs);
using CopyFn=Obj (__thiscall *)(void *,const void *);
using PassFn=void (__thiscall *)(void *,DWORD,DWORD,void *,DWORD);
using ProjectionFn=float (__thiscall *)(void *);
using SetViewFn=void (__thiscall *)(void *,void *,const Matrix *,const void *);
static CtorFn ctorOriginal=nullptr;
static CopyFn copyOriginal=nullptr;
static PassFn passOriginal=nullptr;
static ProjectionFn projectionOriginal=nullptr;
struct alignas(16) XmmState {BYTE values[128];};
__attribute__((always_inline)) inline void saveXmm(XmmState &s){
    __asm__ volatile("movaps %%xmm0,0(%0)\n\tmovaps %%xmm1,16(%0)\n\tmovaps %%xmm2,32(%0)\n\tmovaps %%xmm3,48(%0)\n\t"
        "movaps %%xmm4,64(%0)\n\tmovaps %%xmm5,80(%0)\n\tmovaps %%xmm6,96(%0)\n\tmovaps %%xmm7,112(%0)"
        : : "r"(s.values) : "memory");
}
__attribute__((always_inline)) inline void restoreXmm(const XmmState &s){
    __asm__ volatile("movaps 0(%0),%%xmm0\n\tmovaps 16(%0),%%xmm1\n\tmovaps 32(%0),%%xmm2\n\tmovaps 48(%0),%%xmm3\n\t"
        "movaps 64(%0),%%xmm4\n\tmovaps 80(%0),%%xmm5\n\tmovaps 96(%0),%%xmm6\n\tmovaps 112(%0),%%xmm7"
        : : "r"(s.values) : "memory","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7");
}
static bool fingerprint(Obj view,Record &r){
    Matrix p={};r.pawn=ptr(view+0x14);r.viewState=ptr(view+8);
    if(!r.pawn||!r.viewState||!copy(view+0x70,&r.viewMatrix,64)||!copy(view+0xb0,&p,64)
        ||!foreground_math::valid(r.viewMatrix)||!foreground_math::valid(p))return false;
    if(fabs(p.v[3][3])>1e-5||fabs(p.v[2][3]-1)>1e-5||p.v[0][0]<=0||p.v[1][1]<=0)return false;
    r.px=p.v[0][0];r.py=p.v[1][1];return true;
}
static void forget(Obj view){
    AcquireSRWLockExclusive(&registryLock);
    for(Record &r:records)if(r.view==view)r={};
    ReleaseSRWLockExclusive(&registryLock);
}
static void remember(const Record &value){
    AcquireSRWLockExclusive(&registryLock);
    for(Record &r:records)if(r.view==value.view){r=value;ReleaseSRWLockExclusive(&registryLock);return;}
    records[nextRecord++%256]=value;ReleaseSRWLockExclusive(&registryLock);
}
static bool lookup(Obj view,Record &out){
    Record current; if(!fingerprint(view,current))return false;
    AcquireSRWLockShared(&registryLock);bool found=false;
    for(const Record &r:records)if(r.valid&&r.view==view&&r.pawn==current.pawn&&r.viewState==current.viewState
        &&r.px==current.px&&r.py==current.py&&!memcmp(&r.viewMatrix,&current.viewMatrix,64)
        &&GetTickCount()-r.time<5000){out=r;found=true;break;}
    ReleaseSRWLockShared(&registryLock);return found;
}
void refresh(Obj pc,Obj pawn,bool ready){
    if(GetCurrentThreadId()!=windowThread)return;
    Identity current;
    if(ready&&player_options::live(pc,pawn)&&classIs(pc,"XPlayerController")&&classIs(pawn,"XHuman")
        &&ptr(pc+0x1fc)==pawn&&ptr(pawn+0x218)==pc){
        Obj local=ptr(pc+0x23c);
        if(classIs(local,"XLocalPlayer")&&ptr(local+0x74))current={pc,pawn,local,ptr(local+0x74),true};
    }
    if(current.pc!=identity.pc||current.pawn!=identity.pawn||current.viewState!=identity.viewState)pending={};
    identity=current;
    if(!current.ready)InterlockedExchange(&state,0);
}
float __fastcall projectionGetter(void *controller,void *){
    XmmState entry;saveXmm(entry);
    Obj caller=(Obj)__builtin_return_address(0),pc=(Obj)controller;
    float value=projectionOriginal(controller);
    if(installed&&caller==(Obj)host+0x4782ef&&GetCurrentThreadId()==windowThread){
        pending={};
        const auto lens=player_options::projectionLens(pc,value);
        if(identity.ready&&identity.pc==pc&&player_options::live(pc,identity.pawn)&&lens.valid
            &&lens.native>1&&lens.native<179&&lens.world>1&&lens.world<179){
            value=lens.world;pending={identity.pawn,identity.viewState,lens.native,lens.world,GetTickCount(),true};
        }
    }
    restoreXmm(entry);return value;
}
Obj __fastcall constructor(void *view,void *,CtorArgs args){
    Obj caller=(Obj)__builtin_return_address(0);
    Obj result=ctorOriginal(view,args);XmmState after;saveXmm(after);
    forget((Obj)view);
    if(result==(Obj)view&&installed&&caller==(Obj)host+0x478e7e&&GetCurrentThreadId()==windowThread
        &&pending.valid&&GetTickCount()-pending.time<250){
        Record r;
        if(fingerprint((Obj)view,r)&&r.pawn==pending.pawn&&r.viewState==pending.viewState){
            const double half=3.14159265358979323846/360.0;
            double ratio=tan(pending.world*half)/tan(pending.native*half);
            if(foreground_math::finite(ratio)&&ratio>=0.2&&ratio<=5){
                r.view=(Obj)view;r.ratio=(float)ratio;r.native=pending.native;r.world=pending.world;r.time=pending.time;r.valid=true;
                remember(r);
            }
        }
    }
    restoreXmm(after);return result;
}
Obj __fastcall copyConstructor(void *view,void *,const void *source){
    Obj caller=(Obj)__builtin_return_address(0);
    Obj result=copyOriginal(view,source);XmmState after;saveXmm(after);
    if((Obj)view!=(Obj)source)forget((Obj)view);
    Record r;
    if(installed&&result==(Obj)view&&caller==(Obj)host+0x8ebfab&&lookup((Obj)source,r)){
        Record destination;
        if(fingerprint((Obj)view,destination)&&destination.pawn==r.pawn&&destination.viewState==r.viewState
            &&destination.px==r.px&&destination.py==r.py&&!memcmp(&destination.viewMatrix,&r.viewMatrix,64)){
            r.view=(Obj)view;remember(r);
        }
    }
    restoreXmm(after);return result;
}
static const unsigned offsets[]={0xb0,0x1b0,0x1f0,0x240,0x280,0x300,0x340};
static bool readFields(Obj view,Fields &f){
    BYTE *p=(BYTE *)&f;for(unsigned i=0;i<7;i++)if(!copy(view+offsets[i],p+i*64,64))return false;return true;
}
static void writeFields(Obj view,const Fields &f){
    const BYTE *p=(const BYTE *)&f;for(unsigned i=0;i<7;i++)memcpy((void *)(view+offsets[i]),p+i*64,64);
}
struct Scope {Obj view=0;Fields saved;float scale=0;bool applied=false;};
static bool begin(Obj renderer,Scope &scope){
    // This task supports the game's one local viewport. Captures/other targets
    // and mixed multi-view renderers keep their native rendering parameters.
    if(!renderer||read<int>(renderer+0x54)!=1||read<int>(renderer+0x58)<1)return false;
    Obj view=ptr(renderer+0x50),family=renderer+4;
    if(!view||ptr(view+4)!=family||read<int>(family+8)!=1)return false;
    Obj familyViews=ptr(family+4);if(!familyViews)familyViews=family;
    if(ptr(familyViews)!=view)return false;
    Record r;if(!lookup(view,r)||fabsf(r.ratio-1)<0.00001f)return false;
    Obj rhi=ptr((Obj)host+0x13d124c);
    if(!rhi||ptr(ptr(rhi)+0x1ac)!=(Obj)host+0x90f6b0)return false;
    Matrix v={},tv={};Fields raw,changed;
    if(!copy(view+0x70,&v,64)||!copy(view+0x170,&tv,64)||!readFields(view,raw)
        ||!foreground_math::adapt(v,tv,raw,r.ratio,changed))return false;
    float scale=read<float>(view+0x500);
    if(!foreground_math::finite(scale)||scale<=0)return false;
    scope.view=view;scope.saved=raw;scope.scale=scale;
    writeFields(view,changed);*(float *)(view+0x500)=scale*r.native/r.world;scope.applied=true;
    InterlockedExchange(&state,1);
    if(fabsf(lastReported-r.ratio)>0.03f){
        lastReported=r.ratio;modLog("Viewmodel: common foreground projection nativeFOV=%.2f worldFOV=%.2f ratio=%.4f; meshes, props and FX share one view",r.native,r.world,r.ratio);
    }
    return true;
}
static void end(Scope &scope){
    if(!scope.applied)return;
    writeFields(scope.view,scope.saved);*(float *)(scope.view+0x500)=scope.scale;
    // Restore the original per-view RHI constants as well as CPU parameters,
    // so later world/postprocess passes cannot inherit the foreground lens.
    Obj rhi=ptr((Obj)host+0x13d124c);
    if(rhi&&ptr(ptr(rhi)+0x1ac)==(Obj)host+0x90f6b0)
        ((SetViewFn)(host+0x90f6b0))((void *)rhi,(void *)scope.view,
            (const Matrix *)(scope.view+0x1b0),(const void *)(scope.view+0x380));
}
void __fastcall foregroundPass(void *renderer,void *,DWORD group,DWORD option,void *dirty,DWORD flag){
    XmmState entry;saveXmm(entry);
    Obj caller=(Obj)__builtin_return_address(0);
    Scope scope;bool active=false,entered=false;
    if(installed&&group==2&&caller==(Obj)host+0x2e70db&&!insidePass){
        insidePass=true;entered=true;active=begin((Obj)renderer,scope);
    }
    restoreXmm(entry);
    passOriginal(renderer,group,option,dirty,flag);
    XmmState after;saveXmm(after);
    if(active)end(scope);
    if(entered)insidePass=false;
    restoreXmm(after);
}
static bool sehPrefix(unsigned rva,unsigned handler,const BYTE *tail,size_t n){
    BYTE *p=host+rva;return p[0]==0x6a&&p[1]==0xff&&p[2]==0x68
        &&ptr((Obj)p+3)==(Obj)host+handler&&!memcmp(p+7,tail,n);
}
void install(){
    const BYTE ctor[]={0x55,0x8b,0xec,0x83,0xe4,0xf0};
    const BYTE copyTail[]={0x64,0xa1,0,0,0,0,0x50,0x51,0x53};
    const BYTE passTail[]={0x64,0xa1,0,0,0,0,0x50,0x83,0xec,0x34,0x53};
    const BYTE proj[]={0x8b,0x81,0x40,0x02,0,0,0x85,0xc0,0x74,0x07,0xd9,0x80,0x14,0x02,0,0,0xc3};
    const BYTE projCall[]={0xe8,0xe1,0xe7,0x1c,0};
    const BYTE ctorCall[]={0xe8,0xf2,0xd5,0x45,0};
    const BYTE passCall[]={0xe8,0x45,0x4f,0x19,0};
    bool supported=host&&player_options::fovInstalled
        &&!memcmp(host+0x8d6470,ctor,sizeof(ctor))
        &&sehPrefix(0x8758b0,0xcfb575,copyTail,sizeof(copyTail))
        &&sehPrefix(0x47c020,0xcfc1b0,passTail,sizeof(passTail))
        &&!memcmp(host+0x646ad0,proj,sizeof(proj))
        &&!memcmp(host+0x4782ea,projCall,sizeof(projCall))
        &&!memcmp(host+0x478e79,ctorCall,sizeof(ctorCall))
        &&!memcmp(host+0x2e70d6,passCall,sizeof(passCall));
    const unsigned rvas[]={0x646ad0,0x8d6470,0x8758b0,0x47c020};
    void *callbacks[]={(void *)projectionGetter,(void *)constructor,(void *)copyConstructor,(void *)foregroundPass};
    void **originals[]={(void **)&projectionOriginal,(void **)&ctorOriginal,(void **)&copyOriginal,(void **)&passOriginal};
    unsigned created=0;
    if(supported)for(;created<4;created++){
        MH_STATUS r=MH_CreateHook(host+rvas[created],callbacks[created],originals[created]);
        if(r!=MH_OK)break;r=MH_EnableHook(host+rvas[created]);
        if(r!=MH_OK){MH_RemoveHook(host+rvas[created]);*originals[created]=nullptr;break;}
    }
    installed=supported&&created==4;
    if(!installed)for(unsigned i=0;i<created;i++){MH_DisableHook(host+rvas[i]);MH_RemoveHook(host+rvas[i]);*originals[i]=nullptr;}
    InterlockedExchange(&state,installed?0:-1);
    modLog("Viewmodel: common foreground projection and proportional aiming hooks %s",installed?"ready":"unavailable");
}
}
