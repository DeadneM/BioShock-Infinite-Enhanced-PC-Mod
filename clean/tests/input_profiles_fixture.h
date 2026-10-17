namespace input_fixture {
static HWND focus;static Obj expectedInput,expectedFunction;static void *expectedParams,*expectedResult;
static float seen[3];static unsigned calls,depth;static bool recurse,alter;
static HWND WINAPI foreground(){return focus;}
static void __cdecl event(void *object,void *fn,void *params,void *result){
    CHECK((Obj)object==expectedInput && (Obj)fn==expectedFunction && params==expectedParams && result==expectedResult);
    ++calls;for(unsigned i=0;i<3;++i)seen[i]=*(float *)((BYTE *)object+0x60+4*i);
    if(recurse && !depth){++depth;input_comfort::process(object,nullptr,fn,params,result);--depth;}
    if(alter)*(float *)((BYTE *)object+0x60)=77;
}
}
static void testInputProfiles(){
    using resource_fixture::Engine;
    using namespace input_fixture;
    host=(BYTE *)VirtualAlloc(nullptr,0x1400000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    CHECK(MH_Initialize()==MH_OK);
    const BYTE prefix[]={0x55,0x8b,0xec,0x6a,0xff,0x68};
    const BYTE nativeTail[]={0x64,0xa1,0,0,0,0,0x50,0x83,0xec,0x40};
    BYTE *code=host+0x8a0450;memcpy(code,prefix,sizeof(prefix));*(Obj *)(code+6)=(Obj)host+0xce3468;memcpy(code+10,nativeTail,sizeof(nativeTail));
    // Native prologue + deterministic cdecl receipt, retaining the actual ret0C ABI.
    const BYTE receipt[]={0xff,0x75,0x10,0xff,0x75,0x0c,0xff,0x75,0x08,0x51,0xb8};
    memcpy(code+20,receipt,sizeof(receipt));*(Obj *)(code+31)=(Obj)input_fixture::event;
    const BYTE end[]={0xff,0xd0,0x83,0xc4,0x10,0x8b,0xe5,0x5d,0xc2,0x0c,0};memcpy(code+35,end,sizeof(end));
    input_comfort::install();CHECK(input_comfort::installed);FlushInstructionCache(GetCurrentProcess(),host,0x1400000);
    Engine e;BYTE pc[0x800]={},pawn[0x4000]={},in[0x100]={},weapon[0xe00]={},manager[0x400]={};
    auto *objectType=e.object("ObjectProperty",e.meta),*floatType=e.object("FloatProperty",e.meta);
    auto *inputClass=e.object("XPlayerInput",e.meta),*weaponClass=e.object("XWeapon",e.meta);
    auto *fn=e.object("PlayerInput",e.fnType),*param=e.property(fn,"DeltaTime",floatType,0,4);
    fn[0x81]=1;*(uint16_t *)(fn+0x82)=4;*(uint16_t *)(fn+0x84)=0xffff;*(unsigned *)(param+0x34)=0x80;*(Obj *)(inputClass+0x38)=(Obj)fn;
    e.property(e.pcClass,"PlayerInput",objectType,0x340,4);
    e.property(inputClass,"MouseSensitivity",floatType,0x60,4);e.property(inputClass,"LookRightScale",floatType,0x64,4);e.property(inputClass,"LookUpScale",floatType,0x68,4);
    *(Obj *)(pc+0x20)=(Obj)e.pcClass;*(Obj *)(pc+0x340)=(Obj)in;*(Obj *)(pc+0x1fc)=(Obj)pawn;
    *(Obj *)(in+0x20)=(Obj)inputClass;*(Obj *)(in+0x14)=(Obj)pc;
    *(Obj *)(pawn+0x218)=(Obj)pc;*(float *)(pawn+0x2ac)=100;*(Obj *)(pawn+0x314)=(Obj)manager;
    *(int *)(manager+0x2a0)=1;*(Obj *)(manager+0x200)=(Obj)weapon;*(Obj *)(weapon+0x20)=(Obj)weaponClass;*(Obj *)(weapon+0xd88)=(Obj)pawn;
    auto *archetype=e.object("SniperRifle",weaponClass);*(Obj *)(archetype+0x14)=(Obj)e.object("PreCoalescedItemAssets",e.meta);*(Obj *)(weapon+0x24)=(Obj)archetype;
    *(float *)(in+0x60)=60;*(float *)(in+0x64)=250;*(float *)(in+0x68)=250;
    player_options::checkedPC=(Obj)pc;player_options::checkedPawn=(Obj)pawn;player_options::schemaReady=true;
    windowThread=GetCurrentThreadId();gameWindow=input_fixture::focus=(HWND)1;auto savedForeground=input_comfort::foreground;input_comfort::foreground=input_fixture::foreground;
    input_comfort::inspect((Obj)pc,(Obj)pawn,true);CHECK(input_comfort::ready && input_comfort::event==(Obj)fn);
    expectedInput=(Obj)in;expectedFunction=(Obj)fn;float delta=0.016f;expectedParams=&delta;int result=0;expectedResult=&result;
    input_comfort::EventFn volatile call=(input_comfort::EventFn)code;
    const int gains[]={50,75,25,125,150,200};for(unsigned i=0;i<6;++i)inputGains[i]=gains[i];
    for(int context=0;context<3;++context){*(unsigned *)(pc+0x690)=context?0x20000:0;if(context==1)*(Obj *)(weapon+0x24)=0;else *(Obj *)(weapon+0x24)=(Obj)archetype;
        recurse=true;calls=0;call(in,fn,&delta,&result);CHECK(calls==2);
        CHECK(seen[0]==60*gains[context]/100.0f && seen[1]==250*gains[3+context]/100.0f && seen[2]==seen[1]);
        CHECK(*(float *)(in+0x60)==60 && *(float *)(in+0x64)==250 && *(float *)(in+0x68)==250);}
    recurse=false;modMenuOpen=1;call(in,fn,&delta,&result);CHECK(seen[0]==60 && seen[1]==250);modMenuOpen=0;
    input_fixture::focus=nullptr;call(in,fn,&delta,&result);CHECK(seen[0]==60);input_fixture::focus=(HWND)1;
    *(unsigned *)(pc+0x248)=0x20000;call(in,fn,&delta,&result);CHECK(seen[0]==60);*(unsigned *)(pc+0x248)=0;
    alter=true;call(in,fn,&delta,&result);CHECK(*(float *)(in+0x60)==77);alter=false;
    CHECK(MH_DisableHook(code)==MH_OK);CHECK(MH_RemoveHook(code)==MH_OK);CHECK(MH_Uninitialize()==MH_OK);
    input_comfort::foreground=savedForeground;input_comfort::ready=input_comfort::installed=false;input_comfort::input=input_comfort::event=input_comfort::pc=input_comfort::pawn=0;
    input_comfort::original=nullptr;player_options::schemaReady=false;player_options::checkedPC=player_options::checkedPawn=0;windowThread=0;gameWindow=nullptr;
    for(auto &gain:inputGains)gain=100;VirtualFree(host,0,MEM_RELEASE);host=nullptr;
    puts("PASS: native ProcessEvent ret0C/SEH-prefix trampoline, reflected input schema, six gains, sniper archetype, no accumulation/reentrant double scaling, focus/menu/cinematic guards and native-change-preserving restore.");
}
