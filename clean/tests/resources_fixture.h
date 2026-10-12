// Synthetic reflection and ProcessEvent exercise for the real resource actions.
namespace resource_fixture {
static int calls,lastAmount,lastType;
static float __fastcall attribute(void *value,void *) {return *(float *)((BYTE *)value+8);}
static void __fastcall event(void *object,void *,void *,void *raw,void *) {
    auto *args=(progression_resources::CurrencyArgs *)raw;
    BYTE *pc=(BYTE *)object;Obj def=*(Obj *)(pc+0x4b4);if(!def)def=(Obj)pc+0x4b8;
    CHECK(args->type==1 || args->type==6);CHECK(args->amount>0);
    unsigned offset=args->type==1?0x54:0x64;
    *(int *)(def+offset)+=args->amount;++calls;lastAmount=args->amount;lastType=args->type;
}
struct Engine {
    BYTE entries[64][112]={},objects[64][256]={};Obj names[64]={};unsigned nameCount=0,objectCount=0;
    BYTE *meta,*intType,*byteType,*structType,*scriptType,*fnType,*pcClass,*pawnClass;
    unsigned id(const char *text) {
        for(unsigned i=0;i<nameCount;++i)if(!strcmp((char *)entries[i]+16,text))return i;
        CHECK(nameCount<64);unsigned i=nameCount++;names[i]=(Obj)entries[i];
        *(unsigned *)(entries[i]+8)=i<<1;strcpy((char *)entries[i]+16,text);
        *(unsigned *)(host+0x13ccefc)=nameCount;return i;
    }
    BYTE *object(const char *name,BYTE *type) {
        CHECK(objectCount<64);BYTE *p=objects[objectCount++];
        *(unsigned *)(p+0x18)=id(name);*(Obj *)(p+0x20)=(Obj)type;return p;
    }
    BYTE *property(BYTE *owner,const char *name,BYTE *type,unsigned offset,unsigned size) {
        BYTE *p=object(name,type);*(unsigned *)(p+0x2c)=1;*(unsigned *)(p+0x30)=size;
        *(unsigned *)(p+0x48)=offset;*(Obj *)(p+0x28)=*(Obj *)(owner+0x38);*(Obj *)(owner+0x38)=(Obj)p;return p;
    }
    Engine() {
        *(Obj *)(host+0x13ccef8)=(Obj)names;id("None");
        meta=object("Class",nullptr);*(Obj *)(meta+0x20)=(Obj)meta;
        intType=object("IntProperty",meta);byteType=object("ByteProperty",meta);
        structType=object("StructProperty",meta);scriptType=object("ScriptStruct",meta);fnType=object("Function",meta);
        pcClass=object("XPlayerController",meta);pawnClass=object("XHuman",meta);
    }
};
}
static void testResources() {
    using namespace resource_fixture;
    host=(BYTE *)VirtualAlloc(nullptr,0x1400000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    Engine e;BYTE pc[0x700]={},pawn[0x4600]={},selected[132]={};Obj vtable[0x400/4]={};
    *(Obj *)pc=(Obj)vtable;*(Obj *)(pc+0x20)=(Obj)e.pcClass;*(Obj *)(pawn+0x20)=(Obj)e.pawnClass;
    *(Obj *)(pc+0x1fc)=(Obj)pawn;*(Obj *)(pawn+0x218)=(Obj)pc;*(float *)(pawn+0x2ac)=100;
    *(Obj *)((BYTE *)vtable+0x7c)=(Obj)host+0x5f6c00;*(Obj *)((BYTE *)vtable+0x360)=(Obj)host+0x6ce200;
    BYTE jump[]={0xb8,0,0,0,0,0xff,0xe0};*(Obj *)(jump+1)=(Obj)event;
    memcpy(host+0x5f6c00,jump,sizeof(jump));FlushInstructionCache(GetCurrentProcess(),host+0x5f6c00,sizeof(jump));
    BYTE *current=e.property(e.pcClass,"CurrentCharacterDefinition",e.structType,0x4b4,4);
    BYTE *pointer=e.object("Pointer",e.scriptType);*(Obj *)(current+0x58)=(Obj)pointer;
    e.property(pointer,"Dummy",e.intType,0,4);
    BYTE *fallback=e.property(e.pcClass,"DefaultCharacterDefinition",e.structType,0x4b8,132);
    BYTE *definition=e.object("XCharacterDefinition",e.scriptType);*(Obj *)(fallback+0x58)=(Obj)definition;
    BYTE *money=e.property(definition,"Money",e.intType,0x54,4);
    e.property(definition,"Lockpicks",e.intType,0x64,4);
    e.property(e.pawnClass,"WalletSizeAttrib",e.structType,0x4454,36);
    e.property(e.pawnClass,"MaxLockpicksAttrib",e.structType,0x4478,36);
    BYTE *fn=e.object("ClientAdjustCurrency",e.fnType);*(Obj *)(fn+0x28)=*(Obj *)(e.pcClass+0x38);*(Obj *)(e.pcClass+0x38)=(Obj)fn;
    fn[0x81]=2;*(uint16_t *)(fn+0x82)=8;*(uint16_t *)(fn+0x84)=0xffff;
    BYTE *type=e.property(fn,"CurrencyType",e.byteType,0,1);*(unsigned *)(type+0x34)=0x80;
    BYTE *amount=e.property(fn,"Cost",e.intType,4,4);*(unsigned *)(amount+0x34)=0x80;
    player_options::speedInstalled=true;player_options::attributeOriginal=(player_options::AttributeFn)attribute;
    windowThread=GetCurrentThreadId();calls=0;
    *(float *)(pawn+0x4454+8)=1000000;*(float *)(pawn+0x4478+8)=30;
    *(int *)(pc+0x4b8+0x54)=19857;*(int *)(pc+0x4b8+0x64)=29;
    grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==1 && lastType==1 && lastAmount==980143 && *(int *)(pc+0x4b8+0x54)==1000000);
    grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==1); // Already full: no duplicate engine events.
    grantLockpicks((Obj)pc,(Obj)pawn);CHECK(calls==2 && lastType==6 && lastAmount==1 && *(int *)(pc+0x4b8+0x64)==30);
    grantLockpicks((Obj)pc,(Obj)pawn);CHECK(calls==2);
    *(Obj *)(pc+0x4b4)=(Obj)selected;*(int *)(selected+0x54)=123;
    grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3 && *(int *)(selected+0x54)==1000000 && lastAmount==999877);
    *(int *)(selected+0x54)=1000001;grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3 && *(int *)(selected+0x54)==1000001);
    *(int *)(selected+0x54)=-1;grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);
    *(int *)(selected+0x54)=10;*(float *)(pawn+0x4454+8)=__builtin_nanf("");grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);
    *(float *)(pawn+0x4454+8)=2147483648.0f;grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);
    *(float *)(pawn+0x4454+8)=__builtin_inff();grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);
    *(float *)(pawn+0x4454+8)=-1;grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);
    *(float *)(pawn+0x4454+8)=0;grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);
    *(float *)(pawn+0x4454+8)=1000000;
    *(unsigned *)(money+0x48)=0x58;grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);*(unsigned *)(money+0x48)=0x54;
    *(unsigned *)(amount+0x48)=0;grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);*(unsigned *)(amount+0x48)=4;
    *(Obj *)(current+0x58)=(Obj)definition;grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);*(Obj *)(current+0x58)=(Obj)pointer;
    *(Obj *)((BYTE *)vtable+0x360)=0;grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);*(Obj *)((BYTE *)vtable+0x360)=(Obj)host+0x6ce200;
    *(Obj *)((BYTE *)vtable+0x7c)=0;grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);*(Obj *)((BYTE *)vtable+0x7c)=(Obj)host+0x5f6c00;
    *(float *)(pawn+0x2ac)=__builtin_nanf("");grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);*(float *)(pawn+0x2ac)=100;
    *(Obj *)(pc+0x1fc)=0;grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);*(Obj *)(pc+0x1fc)=(Obj)pawn;
    *(unsigned *)(pc+0x248)=0x20000;grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3);*(unsigned *)(pc+0x248)=0;
    windowThread=0;grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==3 && *(int *)(selected+0x54)==10);
    windowThread=GetCurrentThreadId();
    // Capacities come from the live attributes, not from the observed stock
    // 1,000,000/30 values. Match the native truncating integer conversion.
    *(float *)(pawn+0x4454+8)=120.75f;grantMoney((Obj)pc,(Obj)pawn);
    CHECK(calls==4 && lastAmount==110 && *(int *)(selected+0x54)==120);
    grantMoney((Obj)pc,(Obj)pawn);CHECK(calls==4);
    *(float *)(pawn+0x4478+8)=50;*(int *)(selected+0x64)=40;grantLockpicks((Obj)pc,(Obj)pawn);
    CHECK(calls==5 && lastType==6 && lastAmount==10 && *(int *)(selected+0x64)==50);
    grantLockpicks((Obj)pc,(Obj)pawn);CHECK(calls==5);
    player_options::speedInstalled=false;player_options::attributeOriginal=nullptr;
    VirtualFree(host,0,MEM_RELEASE);host=nullptr;strcpy(message,"Load a game to add a weapon.");
    puts("PASS: resources native-sized delta/parameter ABI, money/lockpick selectors, fallback/current character, repeated full inputs, dynamic/truncated capacities, no reduction above cap, invalid counts/caps/schema/native dispatch, stale pawn/cinematic/wrong thread.");
}
