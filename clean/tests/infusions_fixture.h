// Real reflected transaction ABI with bounded stacking and independent native
// attribute readback. No external game memory or save data is modified.
namespace infusion_fixture {
static int calls,mode,total;
enum { Normal, Ignore, InventoryOnly, AttributeOnly, OneEffectOnly, Overshoot, Spend, ChangePawn, ChangeDefinition };
static Obj expectedFunction,otherDefinition;
static void __fastcall event(void *object,void *,void *function,void *raw,void *result) {
    CHECK((Obj)function==expectedFunction && !result);
    auto *args=(progression_loot::AwardArgs *)raw;
    CHECK(args->quantity>0 && args->quantity<=10 && args->currency==1 && args->cost==0);
    CHECK(!args->padding[0] && !args->padding[1] && !args->padding[2]);
    ++calls;total+=args->quantity;if(mode==Ignore)return;
    Obj pc=(Obj)object,def=ptr(pc+0x4b4);if(!def)def=pc+0x4b8;
    Obj data=ptr(def+8),entry=0;int n=read<int>(def+12);
    for(int i=0;i<n;i++)if(read<uint64_t>(data+i*32)==args->id)entry=data+i*32;
    if(!entry) {CHECK(n<read<int>(def+16));entry=data+n*32;*(uint64_t *)entry=args->id;*(int *)(def+12)=n+1;}
    if(mode!=AttributeOnly)*(int *)(entry+8)+=args->quantity+(mode==Overshoot?1:0);
    if(mode!=InventoryOnly) {
        const progression_infusions::Choice *choice=nullptr;
        for(const auto &c:progression_infusions::choices)if(c.id==args->id)choice=&c;
        CHECK(choice);float step=choice->id==235?15.0f:(choice->id==276?0.15f:0.3f);
        *(float *)(ptr(pc+0x1fc)+choice->offset+8)+=step*(mode==OneEffectOnly?1:args->quantity);
    }
    if(mode==Spend)--*(int *)(def+0x54);
    if(mode==ChangePawn)*(Obj *)(pc+0x1fc)=0;
    if(mode==ChangeDefinition)*(Obj *)(pc+0x4b4)=otherDefinition;
}
}
static void testInfusions() {
    using namespace progression_infusions;
    CHECK(count==3 && choices[0].id==276 && choices[1].id==235 && choices[2].id==329);
    host=(BYTE *)VirtualAlloc(nullptr,0x1642000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    {
        resource_fixture::Engine e;
        BYTE *tonicType=e.object("XTonicItem",e.meta),*objectType=e.object("ObjectProperty",e.meta);
        BYTE *qwordType=e.object("QwordProperty",e.meta),*arrayType=e.object("ArrayProperty",e.meta);
        BYTE *boolType=e.object("BoolProperty",e.meta),*floatType=e.object("FloatProperty",e.meta);
        BYTE *nameType=e.object("NameProperty",e.meta),*multipleType=e.object("XEffectDynamicGameplayAttributeModMultiple",e.meta);
        BYTE *effectType=e.object("XEffectDynamicGameplayAttributeMod",e.meta);
        BYTE *weakType=e.object("XWeakReferenceProperty",e.meta);
        e.property(tonicType,"ItemLookupID",qwordType,0x2c,8);
        BYTE *max=e.property(tonicType,"MaxCapacity",e.intType,0xc0,4);
        e.property(tonicType,"GameplayModDefintions",objectType,0xbc,4);
        e.property(multipleType,"GameplayModEffects",arrayType,0x3c,12);
        BYTE *definition=e.property(effectType,"ModDefinition",e.structType,0x3c,56);
        BYTE *structure=e.object("XGameplayModDefinition",e.scriptType);*(Obj *)(definition+0x58)=(Obj)structure;
        e.property(structure,"StatToAffect",nameType,0,8);e.property(structure,"Operation",e.byteType,0x28,1);
        e.property(structure,"ArchetypeFilter",weakType,8,32);
        e.property(structure,"Value",floatType,0x2c,4);e.property(structure,"Duration",floatType,0x30,4);
        BYTE *base=e.property(structure,"ApplyToBaseValue",boolType,0x34,4);*(unsigned *)(base+0x58)=1;
        BYTE *item=e.object(choices[0].name,tonicType),*multiple=e.object("ModMultiple",multipleType);
        BYTE *first=e.object("XEffectDynamicGameplayAttributeMod",effectType),*second=e.object("XEffectDynamicGameplayAttributeMod",effectType);
        *(unsigned *)(item+0x1c)=2;*(uint64_t *)(item+0x2c)=choices[0].id;*(int *)(item+0xc0)=10;
        *(Obj *)(item+0xbc)=(Obj)multiple;*(Obj *)(multiple+0x14)=(Obj)item;
        Obj effects[2]={(Obj)first,(Obj)second};*(Obj *)(multiple+0x3c)=(Obj)effects;
        *(int *)(multiple+0x40)=2;*(int *)(multiple+0x44)=2;
        for(BYTE *p:{first,second}) {
            *(Obj *)(p+0x14)=(Obj)multiple;*(unsigned *)(p+0x70)=1;
            *(unsigned *)(p+0x44)=e.id("Arc_XPlayer");*(unsigned *)(p+0x54)=e.id("XPlayerPawn");
            *(float *)(p+0x68)=0.15f;
        }
        *(unsigned *)(first+0x3c)=e.id(choices[0].stat);
        *(unsigned *)(second+0x3c)=e.id("HealthBatteryCapacityMaxModifierAttrib");
        int cap=-1;CHECK(validateItem((Obj)item,choices[0],cap) && cap==10);
        *(unsigned *)(item+0x1c)=3;CHECK(!validateItem((Obj)item,choices[0],cap) && cap==0);*(unsigned *)(item+0x1c)=2;
        *(uint64_t *)(item+0x2c)=UINT64_C(0x100000114);CHECK(!validateItem((Obj)item,choices[0],cap));*(uint64_t *)(item+0x2c)=276;
        *(int *)(item+0xc0)=1;CHECK(!validateItem((Obj)item,choices[0],cap));*(int *)(item+0xc0)=10;
        *(unsigned *)(max+0x48)=0xbc;CHECK(!validateItem((Obj)item,choices[0],cap));*(unsigned *)(max+0x48)=0xc0;
        *(Obj *)(multiple+0x14)=0;CHECK(!validateItem((Obj)item,choices[0],cap));*(Obj *)(multiple+0x14)=(Obj)item;
        *(int *)(multiple+0x40)=1;CHECK(!validateItem((Obj)item,choices[0],cap));*(int *)(multiple+0x40)=2;
        *(unsigned *)(first+0x40)=1;CHECK(!validateItem((Obj)item,choices[0],cap));*(unsigned *)(first+0x40)=0;
        *(float *)(first+0x6c)=10;CHECK(!validateItem((Obj)item,choices[0],cap));*(float *)(first+0x6c)=0;
        *(unsigned *)(base+0x58)=2;CHECK(!validateItem((Obj)item,choices[0],cap));*(unsigned *)(base+0x58)=1;
        *(unsigned *)(second+0x70)=0;CHECK(!validateItem((Obj)item,choices[0],cap));*(unsigned *)(second+0x70)=1;
        first[0x64]=1;CHECK(!validateItem((Obj)item,choices[0],cap));first[0x64]=0;
        *(float *)(first+0x68)=0.3f;CHECK(!validateItem((Obj)item,choices[0],cap));*(float *)(first+0x68)=0.15f;
        *(unsigned *)(first+0x48)=1;CHECK(!validateItem((Obj)item,choices[0],cap));*(unsigned *)(first+0x48)=0;
        *(uint64_t *)(first+0x4c)=1;CHECK(!validateItem((Obj)item,choices[0],cap));*(uint64_t *)(first+0x4c)=0;
        CHECK(validateItem((Obj)item,choices[0],cap));
    }
    {
        using namespace infusion_fixture;
        loot_fixture::Fixture f;Obj pc=f.controller();BYTE pawn[0x4600]={};memcpy(pawn,f.pawn,sizeof(f.pawn));
        BYTE jump[]={0xb8,0,0,0,0,0xff,0xe0};*(Obj *)(jump+1)=(Obj)event;memcpy(host+0x5f6c00,jump,sizeof(jump));
        FlushInstructionCache(GetCurrentProcess(),host+0x5f6c00,sizeof(jump));
        for(const auto &c:choices)f.e.property(f.e.pawnClass,c.stat,f.e.structType,c.offset,36);
        player_options::speedInstalled=true;player_options::attributeOriginal=(player_options::AttributeFn)resource_fixture::attribute;
        windowThread=GetCurrentThreadId();expectedFunction=(Obj)f.fn;otherDefinition=(Obj)f.selected;
        BYTE db[0x40]={},rows[400*0x5c]={};
        BYTE *dbType=f.e.object("XItemDatabase",f.e.meta),*arrayType=f.e.object("ArrayProperty",f.e.meta);
        f.e.property(dbType,"ItemLookupTable",arrayType,0x2c,12);*(Obj *)(db+0x20)=(Obj)dbType;
        *(Obj *)(db+0x2c)=(Obj)rows;*(int *)(db+0x30)=400;*(int *)(db+0x34)=400;
        *(Obj *)(host+0x1423700)=(Obj)db;
        auto reset=[&]() {
            f.reset();*(Obj *)(pc+0x1fc)=(Obj)pawn;*(Obj *)(f.manager+0x8c)=(Obj)pawn;
            calls=total=0;mode=Normal;
            memset(unconfirmed,0,sizeof(unconfirmed));
            *(float *)(pawn+choices[0].offset+8)=1;*(float *)(pawn+choices[1].offset+8)=205;*(float *)(pawn+choices[2].offset+8)=3.1f;
            f.entry(0,235,7);f.entry(1,329,7);f.entry(2,346,1);*(int *)(f.fallback()+12)=3;
        };
        reset();f.entry(2,346,0);grantInfusions(pc,(Obj)pawn);
        CHECK(calls==0 && read<int>(f.fallback()+0x54)==2500 && strstr(message,"bouclier initial"));
        f.entry(2,346,-1);grantInfusions(pc,(Obj)pawn);CHECK(calls==0);
        reset();
        for(const auto &c:choices)CHECK(award(pc,(Obj)pawn,(Obj)f.fn,c,10));
        CHECK(calls==3 && total==16 && read<int>(f.fallback()+0x54)==2500);
        progression_loot::Inventory inv;CHECK(progression_loot::snapshot(pc,inv));
        for(const auto &c:choices)CHECK(progression_loot::quantity(inv,c.id)==10);
        CHECK(progression_loot::quantity(inv,346)==1);
        for(const auto &c:choices)CHECK(award(pc,(Obj)pawn,(Obj)f.fn,c,10));CHECK(calls==3 && total==16);
        reset();f.entry(0,235,11);CHECK(award(pc,(Obj)pawn,(Obj)f.fn,choices[1],10) && calls==0);
        CHECK(read<int>((Obj)f.entries+8)==11); // Existing over-cap state is preserved.
        for(int failure:{Ignore,InventoryOnly,AttributeOnly,OneEffectOnly,Overshoot,Spend,ChangePawn,ChangeDefinition}) {
            reset();mode=failure;CHECK(!award(pc,(Obj)pawn,(Obj)f.fn,choices[1],10) && calls==1);
            if(failure==InventoryOnly || failure==OneEffectOnly || failure==Spend) {
                CHECK(!award(pc,(Obj)pawn,(Obj)f.fn,choices[1],10) && calls==1);
                CHECK(unconfirmed[1].pc==pc);
                *(float *)(pawn+choices[1].offset+8)=250;*(int *)(f.fallback()+0x54)=2500;
                CHECK(award(pc,(Obj)pawn,(Obj)f.fn,choices[1],10) && calls==1 && !unconfirmed[1].pc);
            }
        }
        reset();mode=InventoryOnly;CHECK(!award(pc,(Obj)pawn,(Obj)f.fn,choices[1],10));
        *(Obj *)(pc+0x4b4)=(Obj)f.selected;f.entry(0,235,10);*(int *)(f.selected+12)=1;
        CHECK(award(pc,(Obj)pawn,(Obj)f.fn,choices[1],10) && calls==1 && !unconfirmed[1].pc);
        reset();windowThread=0;CHECK(!award(pc,(Obj)pawn,(Obj)f.fn,choices[1],10));windowThread=GetCurrentThreadId();
        CHECK(!award(pc,(Obj)pawn,(Obj)f.fn,choices[1],11));CHECK(!award(pc,(Obj)pawn,(Obj)f.fn,choices[1],1));
        CHECK(!award(pc,(Obj)pawn,0,choices[1],10));
        *(unsigned *)(pc+0x248)=0x20000;CHECK(!award(pc,(Obj)pawn,(Obj)f.fn,choices[1],10));*(unsigned *)(pc+0x248)=0;
        *(float *)(pawn+choices[1].offset+8)=__builtin_nanf("");CHECK(!award(pc,(Obj)pawn,(Obj)f.fn,choices[1],10));
        *(float *)(pawn+choices[1].offset+8)=205;f.entry(0,235,-1);CHECK(!award(pc,(Obj)pawn,(Obj)f.fn,choices[1],10));
        CHECK(calls==0);
        player_options::speedInstalled=false;player_options::attributeOriginal=nullptr;
    }
    VirtualFree(host,0,MEM_RELEASE);host=nullptr;
    puts("PASS: campaign infusion IDs/name numbers, native ten-unit capacities/effects, quantity-delta cost-zero transaction ABI, attribute and inventory confirmation, initial shield untouched, repeated inputs/over-cap preservation, no-op/quantity-only/stat-only/overshoot/spend/stale character detection and wrong-thread/cinematic guards.");
}
