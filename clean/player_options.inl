// Included after the verified object/reflection helpers in weapons.cpp.
// These adapters change return values, never saved pawn/camera properties.
namespace player_options {
using AttributeFn=float (__thiscall *)(void *);
using FovFn=float (__thiscall *)(void *,float);
using AimTargetFn=float (__thiscall *)(void *);
AttributeFn attributeOriginal;
FovFn fovOriginal;
AimTargetFn aimTargetOriginal;
bool aimTargetInstalled=false,aimSchemaReady=false;
Obj checkedWeapon=0;
volatile LONG aimState=0;
struct HipLens {Obj pc=0,pawn=0,camera=0;float angle=0;} hipLens;
bool speedInstalled=false,fovInstalled=false,schemaReady=false;
Obj checkedPC=0,checkedPawn=0;
volatile LONG speedObserved=0,fovObserved=0;
Obj checkedCamera=0;
bool cameraSchemaReady=false;
LONG cameraObserved=0;
volatile LONG optionState=0;
thread_local void *scalingAttribute=nullptr;
const BYTE attributeSignature[]={0x83,0xec,0x10,0x57,0x8b,0xf9,0xf6,0x47,0x1c,0x01,0x0f,0x85,0x28,0x01,0,0};
const BYTE fovSignature[]={0x83,0xec,0x0c,0x0f,0x57,0xc0,0x53,0x55,0x56,0x57,0x33,0xdb,0x0f,0x28,0xc8,0x8b,0xe9};
const BYTE cameraFovCall[]={0xe8,0x0f,0xc4,0x94,0xff};
const BYTE aimTargetSignature[]={0x83,0xec,0x08,0x56,0x8b,0xf1,0x8b,0x86,0x88,0x0d,0,0};
const BYTE aimTargetCalls[][5]={{0xe8,0x64,0xa8,0xfe,0xff},{0xe8,0x0f,0x5a,0xf8,0xff},{0xe8,0x7d,0x59,0xf8,0xff}};

bool property(Obj object,const char *name,const char *kind,unsigned offset,unsigned size) {
    Obj f=field(object,name,kind);
    return f && read<unsigned>(f+0x48)==offset && read<unsigned>(f+0x30)==size && read<unsigned>(f+0x2c)==1;
}
void inspect(Obj pc,Obj pawn,bool ready) {
    if(!ready) {checkedPC=checkedPawn=checkedCamera=checkedWeapon=0;hipLens={};aimSchemaReady=false;schemaReady=cameraSchemaReady=false;cameraObserved=0;InterlockedExchange(&optionState,0);InterlockedExchange(&aimState,0);return;}
    Obj cam=ptr(pc+0x240);
    Obj manager=ptr(pawn+0x314);int slot=read<int>(manager+0x2a0);
    Obj weapon=slot>0 && slot<36?ptr(manager+0x1fc+4*slot):0;
    if(weapon!=checkedWeapon || pc!=checkedPC || pawn!=checkedPawn) {
        checkedWeapon=weapon;
        Obj items=field(manager,"InventoryItems","ObjectProperty");
        aimSchemaReady=weapon && classIs(weapon,"XWeapon") && ptr(manager+0x8c)==pawn
            && items && read<unsigned>(items+0x48)==0x1fc && read<unsigned>(items+0x2c)==36 && read<unsigned>(items+0x30)==4
            && property(manager,"EquippedWeaponIndex","IntProperty",0x2a0,4)
            && property(weapon,"OwningPawn","ObjectProperty",0xd88,4) && ptr(weapon+0xd88)==pawn
            && property(weapon,"FirstPersonZoomedFOV","FloatProperty",0x848,4)
            && property(weapon,"FirstPersonTBarZoomedFOV","FloatProperty",0x84c,4);
        modLog("Native aim target: weapon schema %s, weapon=%p",aimSchemaReady?"verified":"unavailable",(void *)weapon);
    }
    InterlockedExchange(&aimState,aimTargetInstalled && aimSchemaReady && hipLens.pc==pc && hipLens.pawn==pawn && hipLens.camera==cam?1:0);
    if(pc==checkedPC && pawn==checkedPawn && cam==checkedCamera) {
        LONG desired=InterlockedCompareExchange(&fovObserved,0,0);
        if(cameraSchemaReady && desired>=80 && desired<=130 && cameraObserved!=desired && ptr(cam+0x1fc)==pc) {
            float delayed=read<float>(cam+0x214),cached=read<float>(cam+0x3d0);
            if(fabsf(delayed-desired)<0.1f && fabsf(cached-desired)<0.1f) {
                modLog("Player FOV: native cache observed, delayed=%.2f POV=%.2f requested=%ld",delayed,cached,desired);
                cameraObserved=desired;
            }
        }
        return;
    }
    hipLens={};checkedPC=pc;checkedPawn=pawn;
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
    float sample(Obj pc,Obj actor,float native,int desired,bool zoom,DWORD now) {
        if(controller!=pc || pawn!=actor){*this={};controller=pc;pawn=actor;}
        if(!(native>1 && native<179) || desired<80 || desired>130)return native;
        if(zoom) {zooming=true;last=native;stableSince=now;return native;}
        if(zooming) {
            if(fabsf(native-last)>0.01f){last=native;stableSince=now;}
            if((hip==0 || native<hip-0.1f) && now-stableSince<250)return native;
            zooming=false;
        }
        hip=native;return (float)desired;
    }
} fovPolicy;

float opticalAimTarget(float native,float hip,int desired) {
    if(!(hip>1 && hip<179 && native>1 && native<hip) || desired<80 || desired>130)return native;
    constexpr float radians=0.017453292519943295f;
    float value=2.0f*atanf(tanf(native*radians*0.5f)*tanf(desired*radians*0.5f)/tanf(hip*radians*0.5f))/radians;
    return value>1 && value<desired?value:native;
}
bool aimContext() {
    if(!aimTargetInstalled || !aimSchemaReady || !checkedWeapon || !live(checkedPC,checkedPawn)
        || !cameraSchemaReady || ptr(checkedPC+0x240)!=checkedCamera || ptr(checkedCamera+0x1fc)!=checkedPC
        || ptr(checkedWeapon+0xd88)!=checkedPawn || hipLens.pc!=checkedPC || hipLens.pawn!=checkedPawn
        || hipLens.camera!=checkedCamera || !(hipLens.angle>1 && hipLens.angle<179))return false;
    Obj manager=ptr(checkedPawn+0x314);int slot=read<int>(manager+0x2a0);
    return slot>0 && slot<36 && ptr(manager+0x8c)==checkedPawn && ptr(manager+0x1fc+4*slot)==checkedWeapon;
}
float __fastcall aimTarget(void *object,void *) {
    Obj caller=(Obj)__builtin_return_address(0),weapon=(Obj)object;
    // The original selects ordinary/TBar zoom. Convert only its target return;
    // the three qualified callers own their lock and interpolation afterwards.
    float native=aimTargetOriginal(object);
    if(!aimTargetInstalled || !InterlockedCompareExchange(&nativeAimZoom,0,0)
        || (caller!=(Obj)host+0x11db1c && caller!=(Obj)host+0x182971 && caller!=(Obj)host+0x182a03)
        || weapon!=checkedWeapon || !aimContext())return native;
    int desired=(int)InterlockedCompareExchange(&fovDegrees,0,0);
    float value=opticalAimTarget(native,hipLens.angle,desired);
    static Obj reportedWeapon=0;static int reportedFov=0;static float reportedNative=0;
    if(value!=native && (reportedWeapon!=weapon || reportedFov!=desired || reportedNative!=native)) {
        reportedWeapon=weapon;reportedFov=desired;reportedNative=native;
        modLog("Native aim target: weapon=%p caller=%p native=%.3f hip=%.3f world=%d target=%.3f; engine transition retained",object,(void *)caller,native,hipLens.angle,desired,value);
    }
    return value;
}

float __fastcall fov(void *controller,void *,float input) {
    // AXCamera's native calculation receives the change before it stores its POV.
    // Its cache, delayed FOV and first-person consumers then share the same lens.
    // The input and the original result remain native, so zoom recovery cannot
    // mistake a previously modified camera cache for the unzoomed baseline.
    Obj caller=(Obj)__builtin_return_address(0),pc=(Obj)controller;
    float native=fovOriginal(controller,input);
    if(!fovInstalled || caller!=(Obj)host+0x938d81 || !live(pc,checkedPawn)
        || !cameraSchemaReady || ptr(pc+0x240)!=checkedCamera || ptr(checkedCamera+0x1fc)!=pc)return native;
    int desired=(int)InterlockedCompareExchange(&fovDegrees,0,0);
    bool zoom=(read<unsigned>(pc+0x690)&0x70000)!=0;
    float value=fovPolicy.sample(pc,checkedPawn,native,desired,zoom,GetTickCount());
    // The native blend's other endpoint is CameraCache.POV.FOV. Keep that
    // unzoomed endpoint at the requested hip lens while the optional target
    // adapter is active; otherwise the accepted zoom pass-through would make
    // it fall back to native hip mid-transition. LockedFOV owns rendered ADS.
    if(InterlockedCompareExchange(&nativeAimZoom,0,0) && (zoom || fovPolicy.zooming) && aimContext())value=(float)desired;
    if(!zoom && !fovPolicy.zooming && !(read<unsigned>(checkedCamera+0x20c)&1) && value==(float)desired && native>1 && native<179) {
        hipLens={pc,checkedPawn,checkedCamera,native};
        InterlockedExchange(&aimState,aimTargetInstalled && aimSchemaReady?1:0);
    }
    if(value==(float)desired && InterlockedExchange(&fovObserved,desired)!=desired)
        modLog("Player FOV: native input=%.2f result=%.2f -> %.2f; shared camera cache, native zoom retained",input,native,value);
    return value;
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
    fovInstalled=!memcmp(host+0x938d7c,cameraFovCall,sizeof(cameraFovCall))
        && installOne(host+0x285190,fovSignature,sizeof(fovSignature),(void *)fov,(void **)&fovOriginal);
    bool aimCalls=!memcmp(host+0x11db17,aimTargetCalls[0],5) && !memcmp(host+0x18296c,aimTargetCalls[1],5) && !memcmp(host+0x1829fe,aimTargetCalls[2],5);
    aimTargetInstalled=fovInstalled && aimCalls && installOne(host+0x108380,aimTargetSignature,sizeof(aimTargetSignature),(void *)aimTarget,(void **)&aimTargetOriginal);
    modLog("Player options: speed hook=%s native camera FOV hook=%s",speedInstalled?"ready":"unavailable",fovInstalled?"ready":"unavailable");
    modLog("Native aim target hook: %s; optional policy %s",aimTargetInstalled?"ready":"unavailable",InterlockedCompareExchange(&nativeAimZoom,0,0)?"ON":"OFF");
}
}
