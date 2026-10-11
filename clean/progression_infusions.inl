// Campaign infusions use three stackable tonic IDs, each with native capacity
// ten. Numbered legacy tonic variants have capacity one and are not awarded.
namespace progression_infusions {
struct Choice {unsigned id;const char *name,*stat;unsigned offset;};
static const Choice choices[]={
    {276,"Tonic_HP_Bonus","HealthMaxModifierAttrib",0x2534},
    {235,"Tonic_EVE_Bonus","VigorEnergySizeAttrib",0x449c},
    {329,"Tonic_Shield_Bonus","ShieldCapacityMaxModifierAttrib",0x2580}
};
static const unsigned count=sizeof(choices)/sizeof(*choices);
static_assert(count==3,"campaign infusion catalogue");
struct Unconfirmed {Obj pc=0,pawn=0,definition=0;float expected=0;int money=0;};
static Unconfirmed unconfirmed[count];
bool property(Obj obj,const char *name,const char *kind,unsigned offset,unsigned size) {
    return progression_resources::propertyIs(field(obj,name,kind),kind,offset,size);
}
float gain(const Choice &choice){return choice.id==235?15.0f:(choice.id==276?0.15f:0.3f);}
bool effect(Obj object,Obj owner,const char *stat,float value) {
    if(!classIs(object,"XEffectDynamicGameplayAttributeMod") || ptr(object+0x14)!=owner
        || !property(object,"ModDefinition","StructProperty",0x3c,56)
        || !nameIs(object+0x24,stat) || read<unsigned>(object+0x40)!=0)return false;
    Obj prop=field(object,"ModDefinition","StructProperty"),structure=ptr(prop+0x58);
    using progression_resources::propertyIs;using progression_resources::structField;
    Obj base=structField(structure,"ApplyToBaseValue");
    return classIs(structure,"ScriptStruct")
        && propertyIs(structField(structure,"StatToAffect"),"NameProperty",0,8)
        && propertyIs(structField(structure,"ArchetypeFilter"),"XWeakReferenceProperty",8,32)
        && propertyIs(structField(structure,"Operation"),"ByteProperty",0x28,1)
        && propertyIs(structField(structure,"Value"),"FloatProperty",0x2c,4)
        && propertyIs(structField(structure,"Duration"),"FloatProperty",0x30,4)
        && propertyIs(base,"BoolProperty",0x34,4) && read<unsigned>(base+0x58)==1
        && nameIs(object+0x2c,"Arc_XPlayer") && read<unsigned>(object+0x48)==0
        && read<uint64_t>(object+0x4c)==0
        && nameIs(object+0x3c,"XPlayerPawn") && read<unsigned>(object+0x58)==0
        && read<unsigned char>(object+0x64)==0 && read<float>(object+0x68)==value
        && read<float>(object+0x6c)==0 && (read<unsigned>(object+0x70)&1);
}
bool validateItem(Obj item,const Choice &choice,int &cap) {
    cap=0;
    if(!classIs(item,"XTonicItem") || !nameIs(item,choice.name) || read<unsigned>(item+0x1c)!=2
        || !property(item,"ItemLookupID","QwordProperty",0x2c,8) || read<uint64_t>(item+0x2c)!=choice.id
        || !property(item,"MaxCapacity","IntProperty",0xc0,4) || read<int>(item+0xc0)!=10
        || !property(item,"GameplayModDefintions","ObjectProperty",0xbc,4))return false;
    Obj mods=ptr(item+0xbc);
    if(!classIs(mods,"XEffectDynamicGameplayAttributeModMultiple") || ptr(mods+0x14)!=item
        || !property(mods,"GameplayModEffects","ArrayProperty",0x3c,12))return false;
    struct Array {Obj data;int count,capacity;} array={};
    unsigned expected=choice.id==276?2:1;
    if(!copy(mods+0x3c,&array,sizeof(array)) || array.count!=(int)expected
        || array.capacity<array.count || array.capacity>16 || !array.data
        || !effect(ptr(array.data),mods,choice.stat,gain(choice)))return false;
    if(expected==2 && !effect(ptr(array.data+4),mods,"HealthBatteryCapacityMaxModifierAttrib",gain(choice)))return false;
    cap=read<int>(item+0xc0);return true;
}
Obj resolve(Obj db,const Choice &choice,int &cap) {
    cap=0;
    if(!db || choice.id>=(unsigned)read<int>(db+0x30))return 0;
    Obj row=ptr(db+0x2c)+choice.id*0x5c;
    if(!nameIs(row+0x34,choice.name) || read<unsigned>(row+0x50)!=2
        || !nameIs(ptr(row+4),"XTonicItem"))return 0;
    Obj item=progression_loot::getItem(db,choice.id);
    if(!item) {
        using LoadFn=void *(__cdecl *)(void *,void *,const wchar_t *,const wchar_t *,unsigned,void *,int);
        wchar_t path[160];swprintf(path,160,L"PreCoalescedItemAssets.%hs_1",choice.name);
        item=(Obj)((LoadFn)(host+0x1066f0))((void *)ptr(row+4),nullptr,path,nullptr,0x2002,nullptr,1);
    }
    return validateItem(item,choice,cap)?item:0;
}
bool stat(Obj pawn,const Choice &choice,float &value) {
    if(!property(pawn,choice.stat,"StructProperty",choice.offset,36))return false;
    player_options::AttributeFn getter=nullptr;
    if(player_options::speedInstalled)getter=player_options::attributeOriginal;
    else if(!memcmp(host+0x2cd110,player_options::attributeSignature,sizeof(player_options::attributeSignature)))
        getter=(player_options::AttributeFn)(host+0x2cd110);
    if(!getter)return false;
    value=getter((void *)(pawn+choice.offset));return value>0 && value<10000000;
}
bool award(Obj pc,Obj pawn,Obj fn,const Choice &choice,int cap) {
    Obj current=0,manager=0;progression_loot::Inventory before;
    if(GetCurrentThreadId()!=windowThread || !player(pc,current,manager) || current!=pawn
        || (read<unsigned>(pc+0x248)&0x20000) || !fn || progression_loot::transaction(pc)!=fn
        || cap!=10 || !progression_loot::snapshot(pc,before))return false;
    int owned=progression_loot::quantity(before,choice.id);if(owned<0)return false;
    int money=read<int>(before.definition+0x54);float valueBefore=0,valueAfter=0;
    if(money<0 || !stat(pawn,choice,valueBefore))return false;
    unsigned index=0;for(;index<count;index++)if(choices[index].id==choice.id)break;
    if(index==count)return false;
    Unconfirmed &receipt=unconfirmed[index];
    if(receipt.pc && (receipt.pc!=pc || receipt.pawn!=pawn || receipt.definition!=before.definition))receipt={};
    float tolerance=0.001f*(1.0f+gain(choice));
    if(receipt.pc) {
        bool recovered=owned>=cap && money==receipt.money
            && valueBefore>=receipt.expected-tolerance && valueBefore<=receipt.expected+tolerance;
        if(!recovered) {
            modLog("Infusions: %s previous transaction remains unconfirmed quantity=%d attribute=%.3f expected=%.3f; no repeat award",
                choice.name,owned,valueBefore,receipt.expected);return false;
        }
        receipt={};
    }
    if(owned>=cap)return true;
    progression_loot::AwardArgs args={choice.id,cap-owned,1,{},0};
    bool invoked=dispatch(pc,fn,&args);progression_loot::Inventory after;int quantity=-1;
    bool observed=invoked && ptr(pc+0x1fc)==pawn && progression_loot::snapshot(pc,after)
        && before.definition==after.definition;
    if(observed)quantity=progression_loot::quantity(after,choice.id);
    float expected=valueBefore+args.quantity*gain(choice);
    bool increased=observed && stat(pawn,choice,valueAfter) && valueAfter>valueBefore
        && valueAfter>=expected-tolerance && valueAfter<=expected+tolerance;
    bool ok=observed && quantity==cap && read<int>(after.definition+0x54)==money && increased;
    if(observed && quantity>=cap && !ok)receipt={pc,pawn,before.definition,expected,money};
    else if(ok)receipt={};
    modLog("Infusions: %s before=%d cap=%d requested=%d after=%d attribute=%.3f->%.3f verified=%d",
        choice.name,owned,cap,args.quantity,quantity,valueBefore,valueAfter,ok);
    return ok;
}
}
void grantInfusions(Obj pc,Obj pawn) {
    using namespace progression_infusions;
    Obj actual=0,manager=0,db=progression_loot::database(),fn=progression_loot::transaction(pc);
    progression_loot::Inventory inventory;
    if(GetCurrentThreadId()!=windowThread || !player(pc,actual,manager) || actual!=pawn
        || (read<unsigned>(pc+0x248)&0x20000) || !db || !fn || !progression_loot::snapshot(pc,inventory)) {
        status("Infusions : joueur ou inventaire indisponible, aucun ajout.");return;
    }
    // The initial shield is a story unlock. This command only upgrades an
    // existing shield and never grants its initial tonic or advances that scene.
    if(progression_loot::quantity(inventory,346)<=0) {
        status("Infusions : obtenez d'abord le bouclier initial dans l'histoire.");return;
    }
    int caps[count]={},before[count]={};float values[count]={};
    for(unsigned i=0;i<count;i++) {
        before[i]=progression_loot::quantity(inventory,choices[i].id);
        if(!resolve(db,choices[i],caps[i]) || before[i]<0 || !stat(pawn,choices[i],values[i])) {
            modLog("Infusions: catalogue rejected id=%u name=%s quantity=%d cap=%d",choices[i].id,choices[i].name,before[i],caps[i]);
            status("Infusions : catalogue ou attribut non reconnu, aucun ajout.");return;
        }
    }
    bool ok=true;
    for(unsigned i=0;i<count;i++)if(!award(pc,pawn,fn,choices[i],caps[i])){ok=false;break;}
    int after[count]={-1,-1,-1};
    if(ptr(pc+0x1fc)==pawn && progression_loot::snapshot(pc,inventory)) {
        for(unsigned i=0;i<count;i++) {after[i]=progression_loot::quantity(inventory,choices[i].id);if(after[i]<caps[i])ok=false;}
    } else ok=false;
    char text[224];snprintf(text,sizeof(text),"Infusions%s : sante %d / %d, pouvoirs %d / %d, bouclier %d / %d.%s",
        ok?" au maximum":"",after[0],caps[0],after[1],caps[1],after[2],caps[2],ok?"":" Ajout interrompu, voir le journal.");
    status(text);
}
