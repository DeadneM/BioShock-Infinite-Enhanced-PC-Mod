// Engine-backed synthetic fixture for the one-shot F1 action. Uses real
// reflection helpers, native-sized thiscall arguments and ProcessEvent.
#include <initializer_list>
namespace vitals_fixture {
enum Mode {Normal,IgnoreHealth,IgnoreShield,IgnoreSalts,OverfillHealth,ChangeMax,ChangePawn,ChangeDefinition,KillPawn};
static Mode mode;
static int healthCalls,shieldCalls,saltsCalls,lastAmount;
static Obj controller,definition,otherDefinition;
static void __fastcall health(void *raw,void *,float amount) {
    Obj pawn=(Obj)raw;++healthCalls;
    CHECK(amount==read<float>(pawn+0x2600));
    if(mode!=IgnoreHealth)*(float *)(pawn+0x25fc)=amount+(mode==OverfillHealth?1.0f:0.0f);
    if(mode==ChangeMax)*(float *)(pawn+0x2600)+=10;
    if(mode==ChangePawn)*(Obj *)(controller+0x1fc)=0;
    if(mode==ChangeDefinition)*(Obj *)(controller+0x4b4)=otherDefinition;
    if(mode==KillPawn)*(float *)(pawn+0x2ac)=0;
}
static void __fastcall shield(void *raw,void *,float amount,int rewards) {
    Obj pawn=(Obj)raw;++shieldCalls;
    CHECK(rewards==0 && amount==read<float>(pawn+0x2608));
    if(mode!=IgnoreShield)*(float *)(pawn+0x2604)=amount;
}
static void __fastcall event(void *raw,void *,void *fn,void *args,void *result) {
    CHECK((Obj)raw==controller && fn && !result);
    auto *currency=(progression_resources::CurrencyArgs *)args;
    CHECK(currency->type==4 && !currency->padding[0] && !currency->padding[1] && !currency->padding[2]);
    CHECK(currency->amount>0 && currency->amount<=10000000);
    ++saltsCalls;lastAmount=currency->amount;
    if(mode!=IgnoreSalts)*(int *)(definition+0x58)+=currency->amount;
}
}
static void testVitals() {
    using namespace vitals_fixture;
    host=(BYTE *)VirtualAlloc(nullptr,0x1400000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    resource_fixture::Engine e;
    BYTE pc[0x700]={},pawn[0x4600]={},local[256]={},manager[0x100]={},selected[132]={},replacement[132]={};
    Obj vtable[0x400/4]={};
    BYTE *floatType=e.object("FloatProperty",e.meta),*boolType=e.object("BoolProperty",e.meta);
    BYTE *localClass=e.object("XLocalPlayer",e.meta),*managerClass=e.object("XInventoryManager",e.meta);
    *(Obj *)pc=(Obj)vtable;*(Obj *)(pc+0x20)=(Obj)e.pcClass;*(Obj *)(pawn+0x20)=(Obj)e.pawnClass;
    *(Obj *)(local+0x20)=(Obj)localClass;*(Obj *)(manager+0x20)=(Obj)managerClass;
    *(Obj *)(pc+0x1fc)=(Obj)pawn;*(Obj *)(pc+0x23c)=(Obj)local;*(Obj *)(pawn+0x218)=(Obj)pc;
    *(Obj *)(pawn+0x314)=(Obj)manager;*(Obj *)(manager+0x8c)=(Obj)pawn;*(float *)(pawn+0x2ac)=100;
    *(Obj *)((BYTE *)vtable+0x7c)=(Obj)host+0x5f6c00;*(Obj *)((BYTE *)vtable+0x360)=(Obj)host+0x6ce200;
    BYTE jump[]={0xb8,0,0,0,0,0xff,0xe0};*(Obj *)(jump+1)=(Obj)event;
    memcpy(host+0x5f6c00,jump,sizeof(jump));FlushInstructionCache(GetCurrentProcess(),host+0x5f6c00,sizeof(jump));
    BYTE *cinematic=e.property(e.pcClass,"bCinematicMode",boolType,0x248,4);*(unsigned *)(cinematic+0x58)=0x20000;
    e.property(e.pawnClass,"Health",floatType,0x2ac,4);
    BYTE *healthProp=e.property(e.pawnClass,"XHealth",floatType,0x25fc,4);
    e.property(e.pawnClass,"XHealthMax",floatType,0x2600,4);
    e.property(e.pawnClass,"XShield",floatType,0x2604,4);e.property(e.pawnClass,"XShieldMax",floatType,0x2608,4);
    BYTE *current=e.property(e.pcClass,"CurrentCharacterDefinition",e.structType,0x4b4,4);
    BYTE *pointer=e.object("Pointer",e.scriptType);*(Obj *)(current+0x58)=(Obj)pointer;e.property(pointer,"Dummy",e.intType,0,4);
    BYTE *fallback=e.property(e.pcClass,"DefaultCharacterDefinition",e.structType,0x4b8,132);
    BYTE *character=e.object("XCharacterDefinition",e.scriptType);*(Obj *)(fallback+0x58)=(Obj)character;
    e.property(character,"Money",e.intType,0x54,4);e.property(character,"Lockpicks",e.intType,0x64,4);
    BYTE *saltsProp=e.property(character,"VigorEnergy",e.intType,0x58,4);
    e.property(e.pawnClass,"VigorEnergySizeAttrib",e.structType,0x449c,36);
    BYTE *fn=e.object("ClientAdjustCurrency",e.fnType);*(Obj *)(fn+0x28)=*(Obj *)(e.pcClass+0x38);*(Obj *)(e.pcClass+0x38)=(Obj)fn;
    fn[0x81]=2;*(uint16_t *)(fn+0x82)=8;*(uint16_t *)(fn+0x84)=0xffff;
    BYTE *type=e.property(fn,"CurrencyType",e.byteType,0,1);*(unsigned *)(type+0x34)=0x80;
    BYTE *amount=e.property(fn,"Cost",e.intType,4,4);*(unsigned *)(amount+0x34)=0x80;
    player_options::speedInstalled=true;player_options::attributeOriginal=(player_options::AttributeFn)resource_fixture::attribute;
    windowThread=GetCurrentThreadId();controller=(Obj)pc;definition=(Obj)selected;otherDefinition=(Obj)replacement;
    *(Obj *)(pc+0x4b4)=definition;
    vitals::Bindings native={(vitals::HealthFn)health,(vitals::ShieldFn)shield};
    auto reset=[&]() {
        healthCalls=shieldCalls=saltsCalls=lastAmount=0;mode=Normal;
        *(Obj *)(pc+0x1fc)=(Obj)pawn;*(Obj *)(pc+0x4b4)=definition;*(unsigned *)(pc+0x248)=0;
        *(float *)(pawn+0x2ac)=100;*(float *)(pawn+0x25fc)=30;*(float *)(pawn+0x2600)=200;
        *(float *)(pawn+0x2604)=10;*(float *)(pawn+0x2608)=80;*(float *)(pawn+0x449c+8)=250;
        *(int *)(definition+0x58)=17;*(int *)(definition+0x54)=12345;*(int *)(definition+0x64)=22;
    };
    auto fill=[&](){return vitals::fillUsing(controller,(Obj)pawn,native);};
    reset();BYTE attribBefore[36];memcpy(attribBefore,pawn+0x449c,36);
    CHECK(fill() && healthCalls==1 && shieldCalls==1 && saltsCalls==1 && lastAmount==233);
    CHECK(*(float *)(pawn+0x25fc)==200 && *(float *)(pawn+0x2604)==80 && *(int *)(definition+0x58)==250);
    CHECK(*(float *)(pawn+0x2600)==200 && *(float *)(pawn+0x2608)==80 && !memcmp(attribBefore,pawn+0x449c,36));
    CHECK(*(int *)(definition+0x54)==12345 && *(int *)(definition+0x64)==22);
    for(int i=0;i<100;i++)CHECK(fill());
    CHECK(healthCalls==1 && shieldCalls==1 && saltsCalls==1); // Already full has no gain events.
    *(float *)(pawn+0x25fc)=99;*(float *)(pawn+0x2604)=0;*(int *)(definition+0x58)=0;
    CHECK(fill() && healthCalls==2 && shieldCalls==2 && saltsCalls==2 && lastAmount==250);
    reset();*(float *)(pawn+0x25fc)=205;*(float *)(pawn+0x2604)=85;*(int *)(definition+0x58)=255;
    CHECK(fill() && healthCalls==0 && shieldCalls==0 && saltsCalls==0);
    CHECK(*(float *)(pawn+0x25fc)==205 && *(float *)(pawn+0x2604)==85 && *(int *)(definition+0x58)==255);
    reset();*(float *)(pawn+0x2604)=*(float *)(pawn+0x2608)=0;
    CHECK(fill() && healthCalls==1 && shieldCalls==0 && saltsCalls==1 && *(float *)(pawn+0x2608)==0); // No story shield unlock.
    reset();*(float *)(pawn+0x449c+8)=120.75f;CHECK(fill() && *(int *)(definition+0x58)==120 && lastAmount==103);
    definition=(Obj)pc+0x4b8;reset();*(Obj *)(pc+0x4b4)=0;
    CHECK(fill() && healthCalls==1 && shieldCalls==1 && saltsCalls==1 && *(int *)(definition+0x58)==250);
    definition=(Obj)selected;
    for(Mode m:{IgnoreHealth,IgnoreShield,IgnoreSalts,OverfillHealth,ChangeMax}) {
        reset();mode=m;CHECK(!fill()); // Never report a ignored/overfilled/capacity-changing native action as success.
    }
    for(Mode m:{ChangePawn,ChangeDefinition,KillPawn}) {
        reset();mode=m;CHECK(!fill() && healthCalls==1 && shieldCalls==0 && saltsCalls==0);
    }
    reset();*(float *)(pawn+0x2ac)=0;CHECK(!fill() && !healthCalls && !shieldCalls && !saltsCalls);
    reset();*(float *)(pawn+0x25fc)=0;CHECK(!fill() && !healthCalls); // No revival.
    reset();*(unsigned *)(pc+0x248)=0x20000;CHECK(!fill() && !healthCalls);
    reset();windowThread=0;CHECK(!fill() && !healthCalls);windowThread=GetCurrentThreadId();
    reset();*(Obj *)(pawn+0x218)=0;CHECK(!fill() && !healthCalls);*(Obj *)(pawn+0x218)=(Obj)pc;
    reset();*(unsigned *)(healthProp+0x48)=0x2604;CHECK(!fill() && !healthCalls);*(unsigned *)(healthProp+0x48)=0x25fc;
    reset();*(unsigned *)(cinematic+0x58)=1;CHECK(!fill() && !healthCalls);*(unsigned *)(cinematic+0x58)=0x20000;
    reset();*(unsigned *)(saltsProp+0x48)=0x54;CHECK(!fill() && !healthCalls);*(unsigned *)(saltsProp+0x48)=0x58;
    reset();*(Obj *)((BYTE *)vtable+0x360)=0;CHECK(!fill() && !healthCalls);*(Obj *)((BYTE *)vtable+0x360)=(Obj)host+0x6ce200;
    for(float invalid:{-1.0f,__builtin_nanf(""),__builtin_inff(),10000000.0f}) {
        reset();*(float *)(pawn+0x2600)=invalid;CHECK(!fill() && !healthCalls);
        reset();*(float *)(pawn+0x2604)=invalid;CHECK(!fill() && !healthCalls);
        reset();*(float *)(pawn+0x449c+8)=invalid;CHECK(!fill() && !healthCalls);
    }
    reset();*(int *)(definition+0x58)=-1;CHECK(!fill() && !healthCalls);
    reset();CHECK(!vitals::fillUsing(controller,(Obj)pawn,{}) && !healthCalls);
    reset();CHECK(!vitals::bind().health);
    memcpy(host+0x3554d0,vitals::healthSignature,sizeof(vitals::healthSignature));
    memcpy(host+0x5eddf0,vitals::shieldSignature,sizeof(vitals::shieldSignature));
    const BYTE retHealth[]={0xc2,4,0},retShield[]={0xc2,8,0};
    memcpy(host+0x3555f3,retHealth,3);memcpy(host+0x5edf7e,retShield,3);
    CHECK(vitals::bind().health==(vitals::HealthFn)(host+0x3554d0) && vitals::bind().shield==(vitals::ShieldFn)(host+0x5eddf0));
    host[0x5edf7f]=4;CHECK(!vitals::bind().health);host[0x5edf7f]=8;
    host[0x3554d0]^=1;CHECK(!vitals::bind().health);
    player_options::speedInstalled=false;player_options::attributeOriginal=nullptr;
    VirtualFree(host,0,MEM_RELEASE);host=nullptr;strcpy(message,"Load a game to add a weapon.");
    puts("PASS: vitals native thiscall/ProcessEvent ABI, dynamic capacities, current and fallback schema, full/no-op/overcap/damaged/empty shield, ignored native changes, stale/dead/cinematic/thread guards and unchanged maxima/other currency.");
}
