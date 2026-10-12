// Included inside weapons.cpp's anonymous namespace. All calls run on its game-thread queue.
namespace progression_resources {
struct CurrencyArgs {unsigned char type;unsigned char padding[3];int amount;};
static_assert(sizeof(CurrencyArgs)==8,"ClientAdjustCurrency parameter layout");

bool propertyIs(Obj f,const char *kind,unsigned offset,unsigned size) {
    return f && classIs(f,kind) && read<unsigned>(f+0x48)==offset
        && read<unsigned>(f+0x30)==size && read<unsigned>(f+0x2c)==1;
}
Obj structField(Obj structure,const char *name) {
    for(Obj child=ptr(structure+0x38),n=0;child && n<128;child=ptr(child+0x28),++n)
        if(nameIs(child,name))return child;
    return 0;
}
Obj characterDefinition(Obj pc) {
    Obj current=field(pc,"CurrentCharacterDefinition","StructProperty");
    Obj fallback=field(pc,"DefaultCharacterDefinition","StructProperty");
    if(!propertyIs(current,"StructProperty",0x4b4,4)
        || !propertyIs(fallback,"StructProperty",0x4b8,132))return 0;
    // CurrentCharacterDefinition is a reflected native pointer, whereas the
    // fallback is an inline XCharacterDefinition. Do not treat an arbitrary
    // four-byte StructProperty as a pointer if a different build changes it.
    Obj pointer=ptr(current+0x58);
    if(!classIs(pointer,"ScriptStruct") || !nameIs(pointer,"Pointer")
        || !propertyIs(structField(pointer,"Dummy"),"IntProperty",0,4))return 0;
    Obj definition=ptr(fallback+0x58);
    if(!classIs(definition,"ScriptStruct") || !nameIs(definition,"XCharacterDefinition")
        || !propertyIs(structField(definition,"Money"),"IntProperty",0x54,4)
        || !propertyIs(structField(definition,"Lockpicks"),"IntProperty",0x64,4))return 0;
    Obj selected=ptr(pc+0x4b4);
    return selected?selected:pc+0x4b8;
}
bool maximum(Obj pawn,const char *name,unsigned offset,int &cap) {
    if(!propertyIs(field(pawn,name,"StructProperty"),"StructProperty",offset,36))return false;
    // Reuse the native trampoline when the speed adapter owns the getter.
    // Currency attributes never receive the movement multiplier.
    player_options::AttributeFn getter=nullptr;
    if(player_options::speedInstalled)getter=player_options::attributeOriginal;
    else if(!memcmp(host+0x2cd110,player_options::attributeSignature,sizeof(player_options::attributeSignature)))
        getter=(player_options::AttributeFn)(host+0x2cd110);
    if(!getter)return false;
    float value=getter((void *)(pawn+offset));
    // The game's integer conversion truncates. Reject non-finite/unsafe results;
    // this is an overflow guard, not an invented gameplay capacity.
    if(!(value>=1.0f && value<2147483648.0f))return false;
    cap=(int)value;return cap>0;
}
void fill(Obj pc,Obj pawn,bool locks) {
    const char *label=locks?"Lockpicks":"Money";
    const unsigned offset=locks?0x64:0x54;
    static const Param params[]={{"CurrencyType","ByteProperty",0,1},{"Cost","IntProperty",4,4}};
    if(GetCurrentThreadId()!=windowThread || !classIs(pc,"XPlayerController") || !classIs(pawn,"XHuman")
        || ptr(pc+0x1fc)!=pawn || ptr(pawn+0x218)!=pc || !(read<float>(pawn+0x2ac)>0)
        || (read<unsigned>(pc+0x248)&0x20000)) {
        status("Resources: player unavailable; no changes.");return;
    }
    Obj definition=characterDefinition(pc),adjust=function(pc,"ClientAdjustCurrency",8,params,2);
    int cap=0,current=0;
    if(!definition || !adjust || ptr(ptr(pc)+0x360)!=(Obj)host+0x6ce200
        || !copy(definition+offset,&current,sizeof(current)) || current<0
        || !maximum(pawn,locks?"MaxLockpicksAttrib":"WalletSizeAttrib",locks?0x4478:0x4454,cap)) {
        status("Resources: unrecognized structure or cap; no changes.");return;
    }
    if(characterDefinition(pc)!=definition || ptr(pc+0x1fc)!=pawn) {
        status("Resources: character changed; grant canceled.");return;
    }
    char text[192];
    if(current>=cap) {
        snprintf(text,sizeof(text),"%s already maxed: %d / %d.",label,current,cap);
        status(text);return; // No duplicate gain events/statistics on repeated inputs.
    }
    // Both operands are non-negative signed integers and current < cap.
    // Therefore delta is positive and current+delta cannot overflow.
    CurrencyArgs args={static_cast<unsigned char>(locks?6:1),{},cap-current};
    bool invoked=dispatch(pc,adjust,&args);int after=-1;
    bool observed=invoked && characterDefinition(pc)==definition
        && copy(definition+offset,&after,sizeof(after));
    modLog("Resources: %s before=%d cap=%d requested=%d after=%d verified=%d",label,current,cap,args.amount,after,observed && after==cap);
    if(observed && after==cap)snprintf(text,sizeof(text),"%s maxed: %d / %d.",label,after,cap);
    else snprintf(text,sizeof(text),"%s: refill incomplete (%d / %d); see the log.",label,after,cap);
    status(text);
}
}
void grantMoney(Obj pc,Obj pawn){progression_resources::fill(pc,pawn,false);}
void grantLockpicks(Obj pc,Obj pawn){progression_resources::fill(pc,pawn,true);}
