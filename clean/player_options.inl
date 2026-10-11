// Included after the verified object/reflection helpers in weapons.cpp.
// Camera calculations remain native; the renderer consumes a paired world lens.
namespace player_options {
using AttributeFn=float (__thiscall *)(void *);
using FovFn=float (__thiscall *)(void *,float);
using AdvanceFovFn=void (__thiscall *)(void *);
AttributeFn attributeOriginal;
FovFn fovOriginal;
AdvanceFovFn advanceFovOriginal;
bool speedInstalled=false,fovInstalled=false,schemaReady=false;
Obj checkedPC=0,checkedPawn=0;
volatile LONG speedObserved=0,fovObserved=0;
Obj checkedCamera=0;
bool cameraSchemaReady=false,lensSourceSchemaReady=false;
LONG cameraObserved=0;
volatile LONG optionState=0;
// Written and consumed only on the native game thread. The render payload
// adapter pairs these two lenses with the final camera cache, never a menu
// value or a cache that was already modified by this mod.
struct LensSample {
    Obj controller=0,pawn=0,camera=0;
    float native=0,world=0;
    DWORD sampledAt=0;
    bool valid=false,corrected=false;
} lens;
struct RenderLens {float native=0,world=0;bool corrected=false,valid=false;};
struct DelayedLens {Obj controller=0,pawn=0,camera=0;float source=0;RenderLens optics={};DWORD sampledAt=0;} delayedLens;
thread_local void *scalingAttribute=nullptr;
const BYTE attributeSignature[]={0x83,0xec,0x10,0x57,0x8b,0xf9,0xf6,0x47,0x1c,0x01,0x0f,0x85,0x28,0x01,0,0};
const BYTE fovSignature[]={0x83,0xec,0x0c,0x0f,0x57,0xc0,0x53,0x55,0x56,0x57,0x33,0xdb,0x0f,0x28,0xc8,0x8b,0xe9};
const BYTE cameraFovCall[]={0xe8,0x0f,0xc4,0x94,0xff};
// ACamera::AdvanceFOV copies either the authored LockedFOV or final POV to
// FrameDelayedFOV, after the renderer has read the preceding frame's value.
const BYTE advanceFovSignature[]={0xf6,0x81,0x0c,0x02,0,0,0x01,0x74,0x0d,0xd9,0x81,0x10,0x02,0,0,
    0xd9,0x99,0x14,0x02,0,0,0xc3,0xd9,0x81,0xd0,0x03,0,0,0xd9,0x99,0x14,0x02,0,0,0xc3};

bool property(Obj object,const char *name,const char *kind,unsigned offset,unsigned size) {
    Obj f=field(object,name,kind);
    return f && read<unsigned>(f+0x48)==offset && read<unsigned>(f+0x30)==size && read<unsigned>(f+0x2c)==1;
}
void inspect(Obj pc,Obj pawn,bool ready) {
    if(!ready) {checkedPC=checkedPawn=checkedCamera=0;schemaReady=cameraSchemaReady=lensSourceSchemaReady=false;cameraObserved=0;lens={};delayedLens={};InterlockedExchange(&optionState,0);return;}
    Obj cam=ptr(pc+0x240);
    if(pc==checkedPC && pawn==checkedPawn && cam==checkedCamera) {
        LONG desired=InterlockedCompareExchange(&fovObserved,0,0);
        if(cameraSchemaReady && desired>=80 && desired<=130 && cameraObserved!=desired && ptr(cam+0x1fc)==pc) {
            float delayed=read<float>(cam+0x214),cached=read<float>(cam+0x3d0);
            if(lens.valid && fabsf(cached-lens.native)<0.1f && fabsf(delayed-cached)<0.1f) {
                modLog("Player FOV: original camera cache observed, delayed=%.2f POV=%.2f; requested renderer lens=%ld",delayed,cached,desired);
                cameraObserved=desired;
            }
        }
        return;
    }
    checkedPC=pc;checkedPawn=pawn;
    lens={};delayedLens={};
    checkedCamera=cam;cameraObserved=0;
    Obj cinematic=field(pc,"bCinematicMode","BoolProperty");
    schemaReady=property(pawn,"JogSpeedModifierAttrib","StructProperty",0x3ed0,36)
        && property(pawn,"SprintSpeedModifierAttrib","StructProperty",0x3ef4,36)
        && property(pc,"PlayerCamera","ObjectProperty",0x240,4)
        && cinematic && read<unsigned>(cinematic+0x48)==0x248 && read<unsigned>(cinematic+0x58)==0x20000;
    cameraSchemaReady=classIs(cam,"XCamera") && ptr(cam+0x1fc)==pc
        && property(cam,"PCOwner","ObjectProperty",0x1fc,4)
        && property(cam,"FrameDelayedFOV","FloatProperty",0x214,4)
        && property(cam,"CameraCache","StructProperty",0x3b4,32);
    Obj locked=field(cam,"bLockedFOV","BoolProperty");
    lensSourceSchemaReady=cameraSchemaReady && property(cam,"LockedFOV","FloatProperty",0x210,4)
        && locked && read<unsigned>(locked+0x48)==0x20c && read<unsigned>(locked+0x58)==1;
    modLog("Player options: schema %s, controller=%p pawn=%p",schemaReady?"verified":"unavailable",(void *)pc,(void *)pawn);
    modLog("Player FOV: camera schema %s, camera=%p",cameraSchemaReady?"verified":"unavailable",(void *)cam);
    InterlockedExchange(&optionState,schemaReady?1:-1);
}
bool live(Obj pc,Obj pawn) {
    return schemaReady && pc==checkedPC && pawn==checkedPawn && GetCurrentThreadId()==windowThread
        && ptr(pc+0x1fc)==pawn && ptr(pawn+0x218)==pc && read<float>(pawn+0x2ac)>0
        && !(read<unsigned>(pc+0x248)&0x20000);
}
float scaleValue(float native,int percent) {
    return native>=0 && native<10000 && percent>=100 && percent<=200 && percent%25==0
        ?native*((float)percent/100.0f):native;
}
float __fastcall attribute(void *value,void *) {
    Obj object=(Obj)value;
    bool relevant=speedInstalled && checkedPawn && (object==checkedPawn+0x3ed0 || object==checkedPawn+0x3ef4)
        && scalingAttribute!=value && live(checkedPC,checkedPawn);
    void *previous=scalingAttribute;
    if(relevant)scalingAttribute=value;
    float result=attributeOriginal(value);
    if(relevant) {
        scalingAttribute=previous;
        int percent=(int)InterlockedCompareExchange(&speedPercent,0,0);
        float changed=scaleValue(result,percent);
        if(InterlockedExchange(&speedObserved,percent)!=percent)
            modLog("Player speed: x%.2f; native modifier %.3f -> %.3f",(double)percent/100,result,changed);
        return changed;
    }
    return result;
}

struct FovPolicy {
    Obj controller=0,pawn=0;
    bool zooming=false;
    float hip=0,last=0;
    DWORD stableSince=0;
    static float relative(float native,float nativeHip,int desired) {
        if(!(native>1 && native<179) || !(nativeHip>1 && nativeHip<179) || desired<80 || desired>130)return native;
        const double radians=3.14159265358979323846/360.0;
        double value=atan(tan(desired*radians)*tan(native*radians)/tan(nativeHip*radians))/radians;
        return value>1 && value<179?(float)value:native;
    }
    float sample(Obj pc,Obj actor,float native,int desired,bool zoom,DWORD now) {
        if(controller!=pc || pawn!=actor){*this={};controller=pc;pawn=actor;}
        if(!(native>1 && native<179) || desired<80 || desired>130)return native;
        if(zoom) {zooming=true;last=native;stableSince=now;return relative(native,hip,desired);}
        if(zooming) {
            if(fabsf(native-last)>0.01f){last=native;stableSince=now;}
            if((hip==0 || native<hip-0.1f) && now-stableSince<250)return relative(native,hip,desired);
            zooming=false;
        }
        hip=native;return (float)desired;
    }
} fovPolicy;

// Provenance belongs to the actual native AdvanceFOV copy, rather than the
// current zoom flag: entry/release can change that flag between two frames.
// The delayed field and weapon zoom values themselves remain engine-owned.
void recordDelayedLens(Obj camera) {
    delayedLens={};
    Obj pc=ptr(camera+0x1fc);
    if(!fovInstalled || !lensSourceSchemaReady || camera!=checkedCamera || ptr(pc+0x240)!=camera || !live(pc,checkedPawn))return;
    float source=read<float>(camera+0x214);RenderLens optics;
    if(!(source>1 && source<179))return;
    if(read<unsigned>(camera+0x20c)&1) {
        if(fabsf(source-read<float>(camera+0x210))>=0.01f || fovPolicy.controller!=pc || fovPolicy.pawn!=checkedPawn
            || !(fovPolicy.hip>1 && fovPolicy.hip<179))return;
        int desired=(int)InterlockedCompareExchange(&fovDegrees,0,0);
        optics.native=source;optics.world=FovPolicy::relative(source,fovPolicy.hip,desired);
        optics.corrected=fabsf(optics.world-source)>0.01f;optics.valid=desired>=80 && desired<=130;
    } else if(lens.valid && lens.controller==pc && lens.pawn==checkedPawn && lens.camera==camera
        && GetTickCount()-lens.sampledAt<=250 && fabsf(source-lens.native)<0.01f
        && fabsf(source-read<float>(camera+0x3d0))<0.01f) {
        optics={lens.native,lens.world,lens.corrected,true};
    }
    if(optics.valid)delayedLens={pc,checkedPawn,camera,source,optics,GetTickCount()};
}
RenderLens projectionLens(Obj pc,float originalReturned) {
    if(!fovInstalled || !lensSourceSchemaReady || !live(pc,checkedPawn) || delayedLens.controller!=pc
        || delayedLens.pawn!=checkedPawn || delayedLens.camera!=checkedCamera || ptr(pc+0x240)!=checkedCamera
        || ptr(checkedCamera+0x1fc)!=pc || GetTickCount()-delayedLens.sampledAt>250
        || fabsf(delayedLens.source-originalReturned)>=0.01f || !(originalReturned>1 && originalReturned<179))return {};
    return delayedLens.optics;
}
struct alignas(16) AdvanceXmmState {BYTE values[128];};
__attribute__((always_inline)) inline void saveAdvanceXmm(AdvanceXmmState &saved) {
    __asm__ volatile("movaps %%xmm0,0(%0)\n\tmovaps %%xmm1,16(%0)\n\tmovaps %%xmm2,32(%0)\n\tmovaps %%xmm3,48(%0)\n\t"
        "movaps %%xmm4,64(%0)\n\tmovaps %%xmm5,80(%0)\n\tmovaps %%xmm6,96(%0)\n\tmovaps %%xmm7,112(%0)"
        : : "r"(saved.values) : "memory");
}
__attribute__((always_inline)) inline void restoreAdvanceXmm(const AdvanceXmmState &saved) {
    __asm__ volatile("movaps 0(%0),%%xmm0\n\tmovaps 16(%0),%%xmm1\n\tmovaps 32(%0),%%xmm2\n\tmovaps 48(%0),%%xmm3\n\t"
        "movaps 64(%0),%%xmm4\n\tmovaps 80(%0),%%xmm5\n\tmovaps 96(%0),%%xmm6\n\tmovaps 112(%0),%%xmm7"
        : : "r"(saved.values) : "memory","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7");
}
void __fastcall advanceFov(void *camera,void *) {
    // This native routine has no SSE side effects. Preserve every register
    // around the added observer for callers optimized against that property.
    AdvanceXmmState saved;saveAdvanceXmm(saved);
    advanceFovOriginal(camera);
    if(GetCurrentThreadId()==windowThread)recordDelayedLens((Obj)camera);
    restoreAdvanceXmm(saved);
}

float __fastcall fov(void *controller,void *,float input) {
    // Observe the native result but keep it in the engine's POV/cache. Particle
    // simulation and camera-relative effects are prepared before draw submission.
    // Only the verified local renderer getter consumes the requested world lens;
    // the foreground pass pairs it with the original camera lens.
    Obj caller=(Obj)__builtin_return_address(0),pc=(Obj)controller;
    float native=fovOriginal(controller,input);
    if(caller!=(Obj)host+0x938d81)return native;
    if(!fovInstalled || !live(pc,checkedPawn)
        || !cameraSchemaReady || ptr(pc+0x240)!=checkedCamera || ptr(checkedCamera+0x1fc)!=pc) {lens={};return native;}
    int desired=(int)InterlockedCompareExchange(&fovDegrees,0,0);
    bool zoom=(read<unsigned>(pc+0x690)&0x70000)!=0;
    float value=fovPolicy.sample(pc,checkedPawn,native,desired,zoom,GetTickCount());
    lens={pc,checkedPawn,checkedCamera,native,value,GetTickCount(),true,
        fabsf(value-native)>0.01f};
    if(value==(float)desired && InterlockedExchange(&fovObserved,desired)!=desired)
        modLog("Player FOV: native input=%.2f result=%.2f unchanged; renderer lens=%.2f, proportional aiming",input,native,value);
    return native;
}
bool installOne(BYTE *address,const BYTE *signature,size_t length,void *replacement,void **original) {
    if(memcmp(address,signature,length))return false;
    MH_STATUS result=MH_CreateHook(address,replacement,original);
    if(result!=MH_OK)return false;
    result=MH_EnableHook(address);
    if(result!=MH_OK){MH_RemoveHook(address);*original=nullptr;return false;}
    return true;
}
void install() {
    speedInstalled=installOne(host+0x2cd110,attributeSignature,sizeof(attributeSignature),(void *)attribute,(void **)&attributeOriginal);
    bool fovReady=!memcmp(host+0x938d7c,cameraFovCall,sizeof(cameraFovCall))
        && !memcmp(host+0x6ecdd0,advanceFovSignature,sizeof(advanceFovSignature))
        && installOne(host+0x285190,fovSignature,sizeof(fovSignature),(void *)fov,(void **)&fovOriginal);
    fovInstalled=fovReady && installOne(host+0x6ecdd0,advanceFovSignature,sizeof(advanceFovSignature),(void *)advanceFov,(void **)&advanceFovOriginal);
    if(fovReady && !fovInstalled){MH_DisableHook(host+0x285190);MH_RemoveHook(host+0x285190);fovOriginal=nullptr;}
    modLog("Player options: speed hook=%s native camera FOV hook=%s",speedInstalled?"ready":"unavailable",fovInstalled?"ready":"unavailable");
}
}
