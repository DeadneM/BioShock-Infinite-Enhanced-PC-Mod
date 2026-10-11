// Included after the verified object/reflection helpers in weapons.cpp.
// These adapters change return values, never saved pawn/camera properties.
namespace player_options {
using AttributeFn=float (__thiscall *)(void *);
using FovFn=float (__thiscall *)(void *,float);
AttributeFn attributeOriginal;
FovFn fovOriginal;
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

bool property(Obj object,const char *name,const char *kind,unsigned offset,unsigned size) {
    Obj f=field(object,name,kind);
    return f && read<unsigned>(f+0x48)==offset && read<unsigned>(f+0x30)==size && read<unsigned>(f+0x2c)==1;
}
void inspect(Obj pc,Obj pawn,bool ready) {
    if(!ready) {checkedPC=checkedPawn=checkedCamera=0;schemaReady=cameraSchemaReady=false;cameraObserved=0;InterlockedExchange(&optionState,0);return;}
    Obj cam=ptr(pc+0x240);
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
    checkedPC=pc;checkedPawn=pawn;
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
    modLog("Player options: speed hook=%s native camera FOV hook=%s",speedInstalled?"ready":"unavailable",fovInstalled?"ready":"unavailable");
}
}
