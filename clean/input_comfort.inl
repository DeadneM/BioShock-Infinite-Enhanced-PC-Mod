// Candidate input-event adapter. No viewmodel/camera-transform mutation.
namespace input_comfort {
using EventFn=void (__thiscall *)(void *,void *,void *,void *);
EventFn original;
bool installed=false,ready=false;
Obj input=0,event=0,pc=0,pawn=0;
unsigned offsets[3]={};
volatile LONG statusCode=0;
thread_local bool insideEvent=false;
HWND (WINAPI *foreground)()=GetForegroundWindow;
bool floatField(Obj object,const char *name,unsigned &offset){
    Obj f=field(object,name,"FloatProperty");
    unsigned value=read<unsigned>(f+0x48);
    if(!f || read<unsigned>(f+0x30)!=4 || read<unsigned>(f+0x2c)!=1 || value<0x28 || value>0x4000 || value%4)return false;
    offset=value;return true;
}
void inspect(Obj currentPC,Obj currentPawn,bool active){
    if(!active){ready=false;input=event=pc=pawn=0;InterlockedExchange(&statusCode,0);return;}
    Obj next=ptr(currentPC+0x340);
    if(pc==currentPC && pawn==currentPawn && input==next)return;
    pc=currentPC;pawn=currentPawn;input=next;event=0;ready=false;
    if(!player_options::property(pc,"PlayerInput","ObjectProperty",0x340,4)
        || !classIs(input,"XPlayerInput") || ptr(input+0x14)!=pc){InterlockedExchange(&statusCode,-1);return;}
    const Param params[]={{"DeltaTime","FloatProperty",0,4}};
    event=function(input,"PlayerInput",4,params,1);
    ready=event && floatField(input,"MouseSensitivity",offsets[0])
        && floatField(input,"LookRightScale",offsets[1]) && floatField(input,"LookUpScale",offsets[2])
        && offsets[0]!=offsets[1] && offsets[0]!=offsets[2] && offsets[1]!=offsets[2];
    InterlockedExchange(&statusCode,ready?1:-1);
    modLog("Input profiles: reflection %s; native event consumption not yet confirmed",ready?"qualified":"unavailable");
}
bool live(Obj object,Obj fn){
    return ready && object==input && fn==event && GetCurrentThreadId()==windowThread
        && player_options::live(pc,pawn) && ptr(pc+0x340)==input && ptr(input+0x14)==pc
        && foreground()==gameWindow && !InterlockedCompareExchange(&modMenuOpen,0,0);
}
int mode(){
    if(!(read<unsigned>(pc+0x690)&0x70000))return 0;
    Obj manager=ptr(pawn+0x314);int slot=read<int>(manager+0x2a0);
    Obj weapon=slot>0 && slot<36?ptr(manager+0x1fc+4*slot):0;
    if(!classIs(weapon,"XWeapon") || ptr(weapon+0xd88)!=pawn)return -1;
    Obj archetype=ptr(weapon+0x24);
    return nameIs(archetype,"SniperRifle") && nameIs(ptr(archetype+0x14),"PreCoalescedItemAssets")?2:1;
}
void __fastcall process(void *object,void *,void *fn,void *params,void *result){
    if(insideEvent || !live((Obj)object,(Obj)fn)){original(object,fn,params,result);return;}
    int context=mode();if(context<0){original(object,fn,params,result);return;}
    float values[3]={},changed[3]={};bool valid=true;
    int mouse=(int)InterlockedCompareExchange(&inputGains[context],0,0);
    int pad=(int)InterlockedCompareExchange(&inputGains[3+context],0,0);
    for(unsigned i=0;i<3;++i){if(!copy(input+offsets[i],&values[i],4))valid=false;changed[i]=values[i]*(i?pad:mouse)/100.0f;
        if(!input_profiles::valid(i?pad:mouse) || !std::isfinite(values[i]) || values[i]<0 || values[i]>100000)valid=false;}
    if(!valid){InterlockedExchange(&statusCode,-1);original(object,fn,params,result);return;}
    insideEvent=true;
    const Obj bound=input;
    for(unsigned i=0;i<3;++i)if(changed[i]!=values[i])memcpy((void *)(bound+offsets[i]),&changed[i],4);
    original(object,fn,params,result);
    // Restore only our own temporary value, never an intervening native change.
    if(ptr(pc+0x340)==bound && ptr(bound+0x14)==pc)
        for(unsigned i=0;i<3;++i)if(changed[i]!=values[i] && read<float>(bound+offsets[i])==changed[i])memcpy((void *)(bound+offsets[i]),&values[i],4);
    insideEvent=false;InterlockedExchange(&statusCode,2);
}
void install(){
    // Qualified core ProcessEvent RVA, three stack arguments, ret0C. The earlier
    // 5F6C00 wrapper forwards here; hook only this common boundary to avoid doubling.
    const BYTE prefix[]={0x55,0x8b,0xec,0x6a,0xff,0x68};
    const BYTE tail[]={0x64,0xa1,0,0,0,0,0x50,0x83,0xec,0x40};
    bool identity=!memcmp(host+0x8a0450,prefix,sizeof(prefix))
        && read<Obj>((Obj)host+0x8a0456)==(Obj)host+0xce3468
        && !memcmp(host+0x8a045a,tail,sizeof(tail));
    installed=identity && player_options::installOne(host+0x8a0450,prefix,sizeof(prefix),(void *)process,(void **)&original);
    modLog("Input profiles event hook: %s; all gains default x1, actual input-event coverage unverified",installed?"ready":"unavailable");
}
}
