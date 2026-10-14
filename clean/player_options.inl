// Native FOV experiment: gameplay speed only; no camera/aim/render hooks.
namespace player_options {
using AttributeFn=float (__thiscall *)(void *);
AttributeFn attributeOriginal;
bool speedInstalled=false,schemaReady=false;
Obj checkedPC=0,checkedPawn=0;
volatile LONG speedObserved=0,optionState=0;
thread_local void *scalingAttribute=nullptr;
const BYTE attributeSignature[]={0x83,0xec,0x10,0x57,0x8b,0xf9,0xf6,0x47,0x1c,0x01,0x0f,0x85,0x28,0x01,0,0};
bool property(Obj object,const char *name,const char *kind,unsigned offset,unsigned size) {
    Obj f=field(object,name,kind);
    return f && read<unsigned>(f+0x48)==offset && read<unsigned>(f+0x30)==size && read<unsigned>(f+0x2c)==1;
}
void inspect(Obj pc,Obj pawn,bool ready) {
    if(!ready){checkedPC=checkedPawn=0;schemaReady=false;InterlockedExchange(&optionState,0);return;}
    if(pc==checkedPC && pawn==checkedPawn)return;
    checkedPC=pc;checkedPawn=pawn;
    Obj cinematic=field(pc,"bCinematicMode","BoolProperty");
    schemaReady=property(pawn,"JogSpeedModifierAttrib","StructProperty",0x3ed0,36)
        && property(pawn,"SprintSpeedModifierAttrib","StructProperty",0x3ef4,36)
        && cinematic && read<unsigned>(cinematic+0x48)==0x248 && read<unsigned>(cinematic+0x58)==0x20000;
    modLog("Player speed: schema %s, controller=%p pawn=%p",schemaReady?"verified":"unavailable",(void *)pc,(void *)pawn);
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
    modLog("Player speed hook: %s. FOV/aim target/render hooks: NOT INSTALLED; native game calculation retained.",speedInstalled?"ready":"unavailable");
}
}
