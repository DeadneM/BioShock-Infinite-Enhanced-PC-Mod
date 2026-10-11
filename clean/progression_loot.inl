// Shared native item transactions. Included inside weapons.cpp's anonymous namespace.
namespace progression_loot {
struct Inventory {Obj definition=0,data=0;int count=0;};
bool snapshot(Obj pc,Inventory &out) {
    out={};Obj def=progression_resources::characterDefinition(pc);
    Obj fallback=field(pc,"DefaultCharacterDefinition","StructProperty");
    Obj structure=ptr(fallback+0x58),items=progression_resources::structField(structure,"Items");
    Obj inner=ptr(items+0x58),itemStruct=ptr(inner+0x58);
    using progression_resources::propertyIs;
    if(!def || !propertyIs(items,"ArrayProperty",8,12) || !propertyIs(inner,"StructProperty",0,32)
        || !classIs(itemStruct,"ScriptStruct") || !nameIs(itemStruct,"XItemDefinition")
        || !propertyIs(progression_resources::structField(itemStruct,"ItemLookupID"),"QwordProperty",0,8)
        || !propertyIs(progression_resources::structField(itemStruct,"ItemAmount"),"IntProperty",8,4))return false;
    struct Array {Obj data;int count,capacity;} array={};
    if(!copy(def+8,&array,sizeof(array)) || array.count<0 || array.count>8192 || array.capacity<array.count
        || array.capacity>16384 || (array.count && !array.data))return false;
    if(array.count) {BYTE end; if(!copy(array.data+array.count*32-1,&end,1))return false;}
    out={def,array.data,array.count};return true;
}
int quantity(const Inventory &inventory,uint64_t id) {
    if(!inventory.definition || inventory.count<0 || inventory.count>8192)return -1;
    int total=0;
    for(int i=0;i<inventory.count;i++) {
        uint64_t entryID=0;int amount=0;Obj entry=inventory.data+i*32;
        if(!copy(entry,&entryID,8) || !copy(entry+8,&amount,4) || amount<0)return -1;
        if(entryID==id) {if(amount>0x7fffffff-total)return -1;total+=amount;}
    }
    return total;
}
Obj transaction(Obj pc) {
    static const Param params[]={
        {"ItemToBeAwardedID","QwordProperty",0,8},{"ItemQuantity","IntProperty",8,4},
        {"CurrencyType","ByteProperty",12,1},{"Cost","IntProperty",16,4}
    };
    if(!classIs(pc,"XPlayerController") || ptr(ptr(pc)+0x35c)!=(Obj)host+0x947690)return 0;
    return function(pc,"ServerTransactLootItem",20,params,4);
}
Obj database() {
    Obj db=ptr((Obj)host+0x1423700);
    if(!classIs(db,"XItemDatabase") || !progression_resources::propertyIs(
        field(db,"ItemLookupTable","ArrayProperty"),"ArrayProperty",0x2c,12))return 0;
    int count=read<int>(db+0x30),capacity=read<int>(db+0x34);
    return ptr(db+0x2c) && count>0 && count<=8192 && capacity>=count && capacity<=16384?db:0;
}
Obj getItem(Obj db,uint64_t id) {
    if(!db || database()!=db || GetCurrentThreadId()!=windowThread)return 0;
    // The native non-gear branch indexes this table without bounds checks.
    if(!(id>>32)) {if(id==0 || id>=(unsigned)read<int>(db+0x30))return 0;}
    else if((id>>32)!=0x10000 || ((unsigned)id&0xe0000000u))return 0;
    // GOG 1.1.25, including the relocated SEH handler. ABI is thiscall, ret 8.
    static const BYTE tail[]={0x64,0xa1,0,0,0,0,0x50,0x83,0xec,0x28};
    BYTE *fn=host+0xd27a0;
    if(fn[0]!=0x6a || fn[1]!=0xff || fn[2]!=0x68 || ptr((Obj)fn+3)!=(Obj)host+0xd2bf30
        || memcmp(fn+7,tail,sizeof(tail)))return 0;
    using Fn=Obj (__thiscall *)(void *,uint64_t);
    return ((Fn)fn)((void *)db,id);
}
#pragma pack(push,4)
struct AwardArgs {uint64_t id;int quantity;unsigned char currency,padding[3];int cost;};
#pragma pack(pop)
static_assert(sizeof(AwardArgs)==20,"ServerTransactLootItem native parameter size");
bool award(Obj pc,Obj fn,uint64_t id) {
    Obj pawn=0,manager=0;Inventory before;
    if(GetCurrentThreadId()!=windowThread || !player(pc,pawn,manager) || (read<unsigned>(pc+0x248)&0x20000)
        || !fn || transaction(pc)!=fn || !snapshot(pc,before))return false;
    int owned=quantity(before,id);if(owned<0)return false;if(owned>0)return true;
    int money=0;if(!copy(before.definition+0x54,&money,4) || money<0)return false;
    AwardArgs args={id,1,1,{},0};
    bool invoked=dispatch(pc,fn,&args);Inventory after;
    bool ok=invoked && ptr(pc+0x1fc)==pawn && snapshot(pc,after) && after.definition==before.definition
        && quantity(after,id)==1 && read<int>(after.definition+0x54)==money;
    modLog("Progression: item %08x:%08x cost=0 verified=%d",(unsigned)(id>>32),(unsigned)id,ok);
    return ok;
}
}
