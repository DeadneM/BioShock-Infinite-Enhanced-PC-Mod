// Synthetic item transactions through the actual reflected ProcessEvent ABI.
// Include after resources_fixture.h; no game process or save data is touched.
namespace loot_fixture {
static int calls,mode;
static Obj expectedFunction,otherDefinition;
static uint64_t lastID;
enum { Append, Ignore, Spend, ChangePawn, ChangeDefinition, AddTooMany };
static void __fastcall event(void *object,void *,void *function,void *raw,void *result) {
    CHECK((Obj)function==expectedFunction && !result);
    auto *args=(progression_loot::AwardArgs *)raw;
    CHECK(args->quantity==1 && args->currency==1 && args->cost==0);
    CHECK(!args->padding[0] && !args->padding[1] && !args->padding[2]);
    ++calls;lastID=args->id;
    if(mode==Ignore)return;
    Obj pc=(Obj)object,def=ptr(pc+0x4b4);if(!def)def=pc+0x4b8;
    Obj data=ptr(def+8);int count=read<int>(def+12);
    CHECK(count>=0 && count<read<int>(def+16));
    memset((void *)(data+count*32),0,32);
    *(uint64_t *)(data+count*32)=args->id;*(int *)(data+count*32+8)=mode==AddTooMany?2:args->quantity;
    *(int *)(def+12)=count+1;
    if(mode==Spend)--*(int *)(def+0x54);
    if(mode==ChangePawn)*(Obj *)(pc+0x1fc)=0;
    if(mode==ChangeDefinition)*(Obj *)(pc+0x4b4)=otherDefinition;
}
struct Fixture {
    resource_fixture::Engine e;
    BYTE pc[0x700]={},pawn[0x400]={},manager[0x100]={},local[0x40]={};
    BYTE selected[132]={},entries[16*32]={};Obj vtable[0x400/4]={};
    BYTE *current,*pointer,*definition,*items,*inner,*lookup,*amount,*fn,*quantityParam,*costParam;
    Obj controller() {return (Obj)pc;}
    Obj fallback() {return (Obj)pc+0x4b8;}
    void array(Obj def,int count=0) {
        *(Obj *)(def+8)=(Obj)entries;*(int *)(def+12)=count;*(int *)(def+16)=16;
        *(int *)(def+0x54)=2500;
    }
    void entry(unsigned index,uint64_t id,int quantity) {
        CHECK(index<16);*(uint64_t *)(entries+index*32)=id;*(int *)(entries+index*32+8)=quantity;
    }
    void reset() {
        calls=0;mode=Append;lastID=0;memset(entries,0,sizeof(entries));
        *(Obj *)(pc+0x4b4)=0;*(Obj *)(pc+0x1fc)=(Obj)pawn;
        array(fallback());array((Obj)selected);
    }
    Fixture() {
        BYTE *localType=e.object("XLocalPlayer",e.meta),*managerType=e.object("XInventoryManager",e.meta);
        BYTE *arrayType=e.object("ArrayProperty",e.meta),*qwordType=e.object("QwordProperty",e.meta);
        *(Obj *)pc=(Obj)vtable;*(Obj *)(pc+0x20)=(Obj)e.pcClass;
        *(Obj *)(pawn+0x20)=(Obj)e.pawnClass;*(Obj *)(local+0x20)=(Obj)localType;
        *(Obj *)(manager+0x20)=(Obj)managerType;
        *(Obj *)(pc+0x23c)=(Obj)local;*(Obj *)(pawn+0x218)=(Obj)pc;
        *(Obj *)(pawn+0x314)=(Obj)manager;*(Obj *)(manager+0x8c)=(Obj)pawn;
        *(float *)(pawn+0x2ac)=100;
        *(Obj *)((BYTE *)vtable+0x7c)=(Obj)host+0x5f6c00;
        *(Obj *)((BYTE *)vtable+0x35c)=(Obj)host+0x947690;
        BYTE jump[]={0xb8,0,0,0,0,0xff,0xe0};*(Obj *)(jump+1)=(Obj)event;
        memcpy(host+0x5f6c00,jump,sizeof(jump));
        FlushInstructionCache(GetCurrentProcess(),host+0x5f6c00,sizeof(jump));
        current=e.property(e.pcClass,"CurrentCharacterDefinition",e.structType,0x4b4,4);
        pointer=e.object("Pointer",e.scriptType);*(Obj *)(current+0x58)=(Obj)pointer;
        e.property(pointer,"Dummy",e.intType,0,4);
        BYTE *fallbackProperty=e.property(e.pcClass,"DefaultCharacterDefinition",e.structType,0x4b8,132);
        definition=e.object("XCharacterDefinition",e.scriptType);*(Obj *)(fallbackProperty+0x58)=(Obj)definition;
        e.property(definition,"Money",e.intType,0x54,4);e.property(definition,"Lockpicks",e.intType,0x64,4);
        items=e.property(definition,"Items",arrayType,8,12);
        inner=e.object("ItemElement",e.structType);*(unsigned *)(inner+0x2c)=1;*(unsigned *)(inner+0x30)=32;
        *(Obj *)(items+0x58)=(Obj)inner;
        BYTE *itemDefinition=e.object("XItemDefinition",e.scriptType);*(Obj *)(inner+0x58)=(Obj)itemDefinition;
        lookup=e.property(itemDefinition,"ItemLookupID",qwordType,0,8);
        amount=e.property(itemDefinition,"ItemAmount",e.intType,8,4);
        fn=e.object("ServerTransactLootItem",e.fnType);
        *(Obj *)(fn+0x28)=*(Obj *)(e.pcClass+0x38);*(Obj *)(e.pcClass+0x38)=(Obj)fn;
        fn[0x81]=4;*(uint16_t *)(fn+0x82)=20;*(uint16_t *)(fn+0x84)=0xffff;
        BYTE *idParam=e.property(fn,"ItemToBeAwardedID",qwordType,0,8);*(unsigned *)(idParam+0x34)=0x80;
        quantityParam=e.property(fn,"ItemQuantity",e.intType,8,4);*(unsigned *)(quantityParam+0x34)=0x80;
        BYTE *currencyParam=e.property(fn,"CurrencyType",e.byteType,12,1);*(unsigned *)(currencyParam+0x34)=0x80;
        costParam=e.property(fn,"Cost",e.intType,16,4);*(unsigned *)(costParam+0x34)=0x80;
        expectedFunction=(Obj)fn;otherDefinition=(Obj)selected;reset();
    }
};
}
static void testLoot() {
    using namespace loot_fixture;
    using progression_loot::Inventory;using progression_loot::snapshot;using progression_loot::quantity;
    using progression_loot::transaction;using progression_loot::award;
    host=(BYTE *)VirtualAlloc(nullptr,0x1400000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    Fixture f;Obj pc=f.controller();windowThread=GetCurrentThreadId();Inventory inventory;
    const uint64_t id=UINT64_C(0x0001000017200068),second=UINT64_C(0x000100001b200068);
    CHECK(snapshot(pc,inventory) && inventory.definition==f.fallback() && inventory.count==0);
    CHECK(quantity(inventory,id)==0 && transaction(pc)==(Obj)f.fn);
    f.entry(0,id,2);f.entry(1,second,7);f.entry(2,id,3);*(int *)(f.fallback()+12)=3;
    CHECK(snapshot(pc,inventory) && quantity(inventory,id)==5 && quantity(inventory,second)==7);
    CHECK(quantity(inventory,12345)==0);
    f.entry(2,id,0x7fffffff);CHECK(quantity(inventory,id)==-1);
    f.entry(2,id,-1);CHECK(quantity(inventory,id)==-1);
    f.entry(2,second,-1);CHECK(quantity(inventory,id)==-1); // Invalid unrelated entries fail closed, too.
    CHECK(quantity({},id)==-1);
    Inventory corrupt={f.fallback(),1,1};CHECK(quantity(corrupt,id)==-1);
    corrupt={f.fallback(),(Obj)f.entries,8193};CHECK(quantity(corrupt,id)==-1);
    f.reset();*(Obj *)(pc+0x4b4)=(Obj)f.selected;f.entry(0,id,4);*(int *)(f.selected+12)=1;
    CHECK(snapshot(pc,inventory) && inventory.definition==(Obj)f.selected && quantity(inventory,id)==4);

    // Array bounds and the reflected 32-byte item layout are prerequisites.
    f.reset();*(int *)(f.fallback()+12)=-1;CHECK(!snapshot(pc,inventory) && !inventory.definition);
    *(int *)(f.fallback()+12)=8193;CHECK(!snapshot(pc,inventory));
    *(int *)(f.fallback()+12)=2;*(int *)(f.fallback()+16)=1;CHECK(!snapshot(pc,inventory));
    *(int *)(f.fallback()+16)=16385;CHECK(!snapshot(pc,inventory));
    f.reset();*(int *)(f.fallback()+12)=1;*(Obj *)(f.fallback()+8)=0;CHECK(!snapshot(pc,inventory));
    *(Obj *)(f.fallback()+8)=1;CHECK(!snapshot(pc,inventory));
    f.reset();*(unsigned *)(f.items+0x48)=12;CHECK(!snapshot(pc,inventory));*(unsigned *)(f.items+0x48)=8;
    *(unsigned *)(f.inner+0x30)=28;CHECK(!snapshot(pc,inventory));*(unsigned *)(f.inner+0x30)=32;
    *(unsigned *)(f.lookup+0x48)=4;CHECK(!snapshot(pc,inventory));*(unsigned *)(f.lookup+0x48)=0;
    *(unsigned *)(f.amount+0x30)=8;CHECK(!snapshot(pc,inventory));*(unsigned *)(f.amount+0x30)=4;
    *(Obj *)(f.current+0x58)=(Obj)f.definition;CHECK(!snapshot(pc,inventory));*(Obj *)(f.current+0x58)=(Obj)f.pointer;
    *(Obj *)(pc+0x4b4)=1;CHECK(!snapshot(pc,inventory));f.reset();
    *(uint16_t *)(f.fn+0x82)=24;CHECK(!transaction(pc));*(uint16_t *)(f.fn+0x82)=20;
    *(unsigned *)(f.costParam+0x48)=12;CHECK(!transaction(pc));*(unsigned *)(f.costParam+0x48)=16;
    *(unsigned *)(f.quantityParam+0x2c)=2;CHECK(!transaction(pc));*(unsigned *)(f.quantityParam+0x2c)=1;

    // Only missing items issue native events; existing quantities never increase.
    CHECK(award(pc,(Obj)f.fn,id) && calls==1 && lastID==id);
    CHECK(snapshot(pc,inventory) && quantity(inventory,id)==1 && read<int>(f.fallback()+0x54)==2500);
    CHECK(award(pc,(Obj)f.fn,id) && calls==1);
    CHECK(award(pc,(Obj)f.fn,second) && calls==2 && lastID==second);
    CHECK(snapshot(pc,inventory) && quantity(inventory,id)==1 && quantity(inventory,second)==1);
    CHECK(read<int>(f.fallback()+0x54)==2500);
    f.reset();*(Obj *)(pc+0x4b4)=(Obj)f.selected;
    CHECK(award(pc,(Obj)f.fn,id) && calls==1 && read<int>((Obj)f.selected+12)==1 && read<int>(f.fallback()+12)==0);
    CHECK(award(pc,(Obj)f.fn,id) && calls==1 && read<int>((Obj)f.selected+0x54)==2500);
    f.reset();mode=Ignore;CHECK(!award(pc,(Obj)f.fn,id) && calls==1);
    CHECK(snapshot(pc,inventory) && quantity(inventory,id)==0 && read<int>(f.fallback()+0x54)==2500);
    f.reset();CHECK(award(pc,(Obj)f.fn,id));mode=Ignore;CHECK(!award(pc,(Obj)f.fn,second) && calls==2);
    CHECK(snapshot(pc,inventory) && quantity(inventory,id)==1 && quantity(inventory,second)==0);
    f.reset();mode=Spend;CHECK(!award(pc,(Obj)f.fn,id) && calls==1 && read<int>(f.fallback()+0x54)==2499);
    f.reset();mode=AddTooMany;CHECK(!award(pc,(Obj)f.fn,id) && calls==1);
    CHECK(snapshot(pc,inventory) && quantity(inventory,id)==2 && read<int>(f.fallback()+0x54)==2500);
    f.reset();mode=ChangePawn;CHECK(!award(pc,(Obj)f.fn,id) && calls==1);
    f.reset();mode=ChangeDefinition;CHECK(!award(pc,(Obj)f.fn,id) && calls==1);

    // Every rejected precondition must avoid dispatch and leave the money alone.
    f.reset();windowThread=0;CHECK(!award(pc,(Obj)f.fn,id));windowThread=GetCurrentThreadId();
    CHECK(!award(pc,0,id));CHECK(!award(pc,(Obj)f.fn+4,id));
    *(Obj *)(pc+0x23c)=0;CHECK(!award(pc,(Obj)f.fn,id));*(Obj *)(pc+0x23c)=(Obj)f.local;
    *(Obj *)(pc+0x1fc)=0;CHECK(!award(pc,(Obj)f.fn,id));*(Obj *)(pc+0x1fc)=(Obj)f.pawn;
    *(Obj *)(f.pawn+0x218)=0;CHECK(!award(pc,(Obj)f.fn,id));*(Obj *)(f.pawn+0x218)=pc;
    *(Obj *)(f.manager+0x8c)=0;CHECK(!award(pc,(Obj)f.fn,id));*(Obj *)(f.manager+0x8c)=(Obj)f.pawn;
    *(float *)(f.pawn+0x2ac)=0;CHECK(!award(pc,(Obj)f.fn,id));
    *(float *)(f.pawn+0x2ac)=__builtin_nanf("");CHECK(!award(pc,(Obj)f.fn,id));
    *(float *)(f.pawn+0x2ac)=100000;CHECK(!award(pc,(Obj)f.fn,id));*(float *)(f.pawn+0x2ac)=100;
    *(unsigned *)(pc+0x248)=0x20000;CHECK(!award(pc,(Obj)f.fn,id));*(unsigned *)(pc+0x248)=0;
    *(Obj *)((BYTE *)f.vtable+0x35c)=0;CHECK(!award(pc,(Obj)f.fn,id));
    *(Obj *)((BYTE *)f.vtable+0x35c)=(Obj)host+0x947690;
    *(Obj *)((BYTE *)f.vtable+0x7c)=0;CHECK(!award(pc,(Obj)f.fn,id));
    *(Obj *)((BYTE *)f.vtable+0x7c)=(Obj)host+0x5f6c00;
    *(unsigned *)(f.items+0x30)=8;CHECK(!award(pc,(Obj)f.fn,id));*(unsigned *)(f.items+0x30)=12;
    *(unsigned *)(f.costParam+0x48)=12;CHECK(!award(pc,(Obj)f.fn,id));*(unsigned *)(f.costParam+0x48)=16;
    CHECK(calls==0 && read<int>(f.fallback()+0x54)==2500 && read<int>(f.fallback()+12)==0);
    *(int *)(f.fallback()+0x54)=-1;CHECK(!award(pc,(Obj)f.fn,id) && calls==0);
    *(int *)(f.fallback()+0x54)=2500;f.entry(0,second,-1);*(int *)(f.fallback()+12)=1;
    CHECK(!award(pc,(Obj)f.fn,id) && calls==0 && read<int>(f.fallback()+0x54)==2500);
    VirtualFree(host,0,MEM_RELEASE);host=nullptr;expectedFunction=otherDefinition=0;
    puts("PASS: loot reflected inventory/transaction ABI, exact quantity-one cost-zero arguments, missing-only/idempotent grants, current/fallback character, no-op/partial/spend detection, stale player/character, invalid arrays/schema/native dispatch and wrong-thread rejection.");
}
