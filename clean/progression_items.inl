// Included inside weapons.cpp's anonymous namespace. All calls are made by
// its validated game-thread command queue, never from the overlay thread.
static const char *const vigorNames[]={
    "Plasmid_EnrageFounder","Plasmid_DevilsKiss","Plasmid_BuckingBroncoFounder","Plasmid_Charge",
    "Plasmid_MurderOfCrowsFounder","Plasmid_UndertowFounder","Plasmid_VoltSwarmFounder","Plasmid_ReturnToSenderInsta"
};

namespace progression_powers {
Obj weaponBase(Obj object) {
    // Bucking Bronco and Murder of Crows are XWeapon subclasses, as are their
    // inventory instances. Validate the UClass chain instead of exact class.
    for(Obj type=ptr(object+0x20),depth=0;object && type && depth<32;type=ptr(type+0x34),depth++) {
        if(!classIs(type,"Class"))return 0;
        if(nameIs(type,"XWeapon"))return type;
    }
    return 0;
}
Obj ownedWeapon(Obj manager,Obj archetype) {
    Obj base=weaponBase(archetype);
    if(!base || !inventoryLayout(manager))return 0;
    for(unsigned i=1;i<36;i++) {
        Obj item=ptr(manager+0x1fc+i*4);
        if(item && ptr(item+0x24)==archetype && weaponBase(item)==base)return item;
    }
    return 0;
}
}

static void grantPowers(Obj pc,Obj pawn,Obj manager) {
    using namespace progression_powers;
    (void)pc;
    static const Param acquireP[]={{"WeaponArchetype","ObjectProperty",0,4},{"EquipWeaponItems","ByteProperty",4,1}};
    Obj acquire=function(pawn,"AcquireWeapon",5,acquireP,2);
    if(!acquire || !inventoryLayout(manager)) {
        status("Vigors: unrecognized inventory; nothing added.");return;
    }
    Obj weaponClass=0;
    int freeSlots=0;
    for(unsigned i=1;i<36;i++) {
        Obj item=ptr(manager+0x1fc+i*4);
        if(!item)freeSlots++;
        else if(Obj base=weaponBase(item))weaponClass=base;
    }
    if(!weaponClass) {status("Vigors: equip a weapon in-game first.");return;}
    using LoadFn=void *(__cdecl *)(void *,void *,const wchar_t *,const wchar_t *,unsigned,void *,int);
    Obj archetypes[8]={};int present=0;
    // Resolve the complete catalogue before acquiring anything. The native
    // loader and package are the same as the tested weapon giver.
    for(unsigned i=0;i<8;i++) {
        wchar_t path[128];swprintf(path,128,L"PreCoalescedItemAssets.%hs",vigorNames[i]);
        archetypes[i]=(Obj)((LoadFn)(host+0x1066f0))((void *)weaponClass,nullptr,path,nullptr,0x2002,nullptr,1);
        if(weaponBase(archetypes[i])!=weaponClass || !nameIs(archetypes[i],vigorNames[i])
            || !nameIs(ptr(archetypes[i]+0x14),"PreCoalescedItemAssets")) {
            modLog("Powers: catalogue missing %s",vigorNames[i]);
            status("Vigors: incomplete catalog in this game; nothing added.");return;
        }
        if(ownedWeapon(manager,archetypes[i]))present++;
    }
    if(present==8) {status("All 8 Vigors are already owned.");return;}
    if(8-present>freeSlots) {status("Vigors: inventory full; no items replaced.");return;}
    int added=0;
    for(unsigned i=0;i<8;i++) {
        if(ownedWeapon(manager,archetypes[i]))continue;
        struct Acquire {Obj archetype;unsigned char noEquip;unsigned char padding[3];} args={archetypes[i],1,{}};
        bool ok=dispatch(pawn,acquire,&args) && ownedWeapon(manager,archetypes[i]);
        modLog("Powers: acquire %s confirmed=%d",vigorNames[i],ok);
        if(!ok) {
            char text[192];snprintf(text,sizeof(text),"Vigors: %d added; acquisition interrupted. See the log.",added);
            status(text);return;
        }
        added++;
    }
    char text[192];snprintf(text,sizeof(text),"8 Vigors owned: %d added, %d already present.",added,present);status(text);
}

namespace progression_gear {
// These are 49 fixed SP pickup definitions verified in this build.
// Each pair is the *authored* slot/static-effect combination, not a Cartesian
// product of the gear database. The three _2 bonus pickups live in a different
// package. Two further SP components have localised pickup names but were not
// loaded in the capture; resolve their authored definitions separately below.
struct Choice {const char *pickup;unsigned char slot,effect;bool bonus;};
static const Choice choices[]={
    {"AlwaysPrepared",13,185,false},{"AmateurArmsBearer",11,187,false},
    {"AmateurChemist",11,128,false},{"Betrayer",12,188,false},
    {"BloodyDangerous",13,189,false},{"Carver",13,190,false},
    {"DashingStrike",13,191,false},{"Demolitionist",11,192,false},
    {"Driven",11,193,false},{"ElectricAura",11,181,false},
    {"ElectricTouch",10,180,false},{"Executioner",11,130,false},
    {"ExpeditiousRetreat",10,194,false},{"FieryAura",11,139,false},
    {"FighterAce",12,137,false},{"FistsOfFire",10,179,false},
    {"FleetFeet",12,195,false},{"FoolsLuck",10,196,false},
    {"HandymanManhandler",12,197,false},{"Headhunter",13,150,false},
    {"Journalist",10,198,false},{"LeftHandSpellsLove",12,146,false},
    {"MesmerizingMelee",10,199,false},{"NewtonsLaw",12,200,false},
    {"OneInchPunch",13,201,false},{"OneMoreInTheChamber",11,202,false},
    {"Overkill",12,203,false},{"Phoenix",13,142,false},
    {"ReinforcedShield",10,204,false},{"RightHandSpellsHate",11,144,false},
    {"RushPatchJob",13,205,false},{"SecondChance",13,133,false},
    {"SecurityShadow",10,184,false},{"SkylineAccuracy",11,131,false},
    {"SkylinePotence",12,145,false},{"SkylinePotence_2",12,170,false},
    {"SoulTrap",13,206,false},{"Storm",10,134,false},
    {"SugarRush",11,207,false},{"TheLevine",12,186,false},
    {"ThirdHand",13,8,false},{"ThirstForBlood",12,136,false},
    {"ThrottleControl",10,208,false},{"TrickHands",10,209,false},
    {"TunnelVision",12,183,false},{"WinterShield",11,210,false},
    {"ElectricTouch_2",10,219,true},{"OneMoreInTheChamber_2",11,218,true},
    {"SoulTrap_2",13,217,true}
};
static const unsigned count=sizeof(choices)/sizeof(*choices);
static_assert(count==49,"fixed SP gear catalogue");
static const Choice optionalChoices[]={
    {"BloodPoweredPlasmids",0,132,false},{"RisingBloodlust",0,143,false}
};

uint64_t itemID(const Choice &c) {
    // SP's definitions and all 46 pre-existing items captured on 2026-10-03
    // agree on rarity=0, level=0, dynamic-effect=0. Validate those definitions
    // again below before accepting even one of these IDs.
    return (uint64_t(1)<<48)|(uint64_t(c.effect)<<21)|(uint64_t(c.slot)<<3);
}
bool layout(Obj object,const char *name,const char *kind,unsigned offset,unsigned size) {
    return progression_resources::propertyIs(field(object,name,kind),kind,offset,size);
}
Obj component(Obj db,const char *mapName,unsigned offset,unsigned key,const char *kind,const char *keyField="LookupID",unsigned keyOffset=0x28) {
    if(!layout(db,mapName,"MapProperty",offset,60))return 0;
    Obj map=db+offset,data=ptr(map);int slots=read<int>(map+4),capacity=read<int>(map+8);
    int bits=read<int>(map+0x20);
    if(!data || slots<1 || slots>256 || capacity<slots || capacity>512 || bits!=slots)return 0;
    // TMap contains a TSparseArray. Consult its allocation bitmap, including
    // the out-of-line bitmap used by the 220-entry static-effect map.
    Obj allocation=ptr(map+0x1c);if(!allocation)allocation=map+0xc;
    if(bits>128 && allocation==map+0xc)return 0;
    Obj result=0;
    for(int i=0;i<slots;i++) {
        unsigned mask=0;
        if(!copy(allocation+(i/32)*4,&mask,4))return 0;
        if(!(mask&(1u<<(i%32))) || read<unsigned char>(data+i*16)!=key)continue;
        Obj candidate=ptr(data+i*16+4);
        if(result || !classIs(candidate,kind)
            || !layout(candidate,keyField,"ByteProperty",keyOffset,1)
            || read<unsigned char>(candidate+keyOffset)!=key)return 0;
        result=candidate;
    }
    return result;
}
bool weakReferenceIs(Obj reference,Obj object) {
    // XWeakReference stores full package/object FNames, including Number.
    // A _2 bonus component shares the base FName with its ordinary version.
    uint64_t packageName=0,objectName=0,actualPackage=0,actualObject=0;
    Obj outer=ptr(object+0x14);
    return object && outer && copy(reference,&packageName,8) && copy(reference+16,&objectName,8)
        && copy(outer+0x18,&actualPackage,8) && copy(object+0x18,&actualObject,8)
        && packageName==actualPackage && objectName==actualObject
        && read<uint64_t>(reference+8)==0;
}
bool singleFilter(Obj definition,const char *name,unsigned offset,unsigned char value) {
    Obj property=field(definition,name,"ArrayProperty"),inner=ptr(property+0x58);
    return progression_resources::propertyIs(property,"ArrayProperty",offset,12)
        && progression_resources::propertyIs(inner,"ByteProperty",0,1)
        && read<int>(definition+offset+4)==1 && read<int>(definition+offset+8)>=1
        && ptr(definition+offset) && read<unsigned char>(ptr(definition+offset))==value;
}
bool exactRule(Obj definition,const char *name,unsigned offset,Obj expected) {
    Obj property=field(definition,name,"StructProperty"),structure=ptr(property+0x58);
    if(!progression_resources::propertyIs(property,"StructProperty",offset,24)
        || !classIs(structure,"ScriptStruct") || !nameIs(structure,"XArchetypeInclusionExclusionRules"))return false;
    Obj inclusion=progression_resources::structField(structure,"InclusionList");
    Obj exclusion=progression_resources::structField(structure,"ExclusionList");
    if(!progression_resources::propertyIs(inclusion,"ArrayProperty",0,12)
        || !progression_resources::propertyIs(exclusion,"ArrayProperty",12,12)
        || !progression_resources::propertyIs(ptr(inclusion+0x58),"XWeakReferenceProperty",0,32)
        || read<int>(definition+offset+4)!=1 || read<int>(definition+offset+8)<1
        || read<int>(definition+offset+16)!=0)return false;
    return weakReferenceIs(ptr(definition+offset),expected);
}
bool authored(Obj pickup,Obj db,const Choice &choice,Obj noDynamic) {
    if(!classIs(pickup,"XLootContainerKActor")
        || !layout(pickup,"LootDefinitionList","ArrayProperty",0x5fc,12)
        || read<int>(pickup+0x600)!=1 || read<int>(pickup+0x604)<1)return false;
    Obj definition=ptr(ptr(pickup+0x5fc));
    if(!classIs(definition,"XLootDropGearItemDefinition"))return false;
    static const char *const levels[]={"MinLevelFilter","MaxLevelFilter","MinRelativeLevelFilter","MaxRelativeLevelFilter"};
    for(unsigned i=0;i<4;i++)
        if(!layout(definition,levels[i],"IntProperty",0x80+i*4,4) || read<int>(definition+0x80+i*4)!=0)return false;
    Obj effect=component(db,"GearStaticEffectComponents",0x128,choice.effect,"XGearStaticEffectComponent");
    if(!effect || !nameIs(ptr(effect+0x14),"Arc_XGearComponents_SP")
        || !layout(effect,"ItemPickup","XWeakReferenceProperty",0xa0,32)
        || !weakReferenceIs(effect+0xa0,pickup))return false;
    return singleFilter(definition,"SlotFilter",0x90,choice.slot)
        && singleFilter(definition,"RarityFilter",0x9c,0)
        && exactRule(definition,"DynamicEffectInclusionExclusionRules",0xa8,noDynamic)
        && exactRule(definition,"StaticEffectInclusionExclusionRules",0xc0,effect);
}
bool pickupName(Obj pickup,const char *suffix) {
    char name[96];snprintf(name,sizeof(name),"Pickup_Gear_%s",suffix);
    size_t length=strlen(name);unsigned number=0;
    if(length>2 && !strcmp(name+length-2,"_2")){name[length-2]=0;number=3;}
    return nameIs(pickup,name) && read<unsigned>(pickup+0x1c)==number;
}
Obj loadPickup(Obj objectClass,const Choice &choice) {
    const char *package=choice.bonus?"Arc_Gear_SP_Pickups":"PreCoalescedItemAssets";
    wchar_t path[160];swprintf(path,160,L"%hs.Pickup_Gear_%hs",package,choice.pickup);
    using LoadFn=void *(__cdecl *)(void *,void *,const wchar_t *,const wchar_t *,unsigned,void *,int);
    Obj pickup=(Obj)((LoadFn)(host+0x1066f0))((void *)objectClass,nullptr,path,nullptr,0x2002,nullptr,1);
    return pickupName(pickup,choice.pickup) && nameIs(ptr(pickup+0x14),package)?pickup:0;
}
bool optional(Obj objectClass,Obj db,Obj noDynamic,const Choice &candidate,Choice &result) {
    Obj pickup=loadPickup(objectClass,candidate);
    if(!classIs(pickup,"XLootContainerKActor")
        || !layout(pickup,"LootDefinitionList","ArrayProperty",0x5fc,12)
        || read<int>(pickup+0x600)!=1 || read<int>(pickup+0x604)<1)return false;
    Obj definition=ptr(ptr(pickup+0x5fc));
    if(!classIs(definition,"XLootDropGearItemDefinition")
        || !layout(definition,"SlotFilter","ArrayProperty",0x90,12)
        || read<int>(definition+0x94)!=1 || read<int>(definition+0x98)<1)return false;
    unsigned char slot=0;if(!copy(ptr(definition+0x90),&slot,1) || slot<10 || slot>13)return false;
    result=candidate;result.slot=slot;
    // No guessed slot: the optional ID comes entirely from an exact shipped
    // pickup definition, and receives the same checks as the captured 49.
    return authored(pickup,db,result,noDynamic)
        && classIs(progression_loot::getItem(db,itemID(result)),"XGearItem");
}
}

static void grantGear(Obj pc,Obj pawn,Obj manager) {
    using namespace progression_gear;
    (void)manager;
    progression_loot::Inventory inventory;
    Obj transaction=progression_loot::transaction(pc),db=progression_loot::database();
    if(GetCurrentThreadId()!=windowThread || ptr(pc+0x1fc)!=pawn || !transaction || !db
        || !progression_loot::snapshot(pc,inventory)) {
        status("Gear: character or inventory unavailable; nothing added.");return;
    }
    Obj noDynamic=component(db,"GearDynamicEffectComponents",0xec,0,"XGearDynamicEffectComponent");
    // GetItem dereferences each component without a native null check. Verify
    // all five maps and the shared result object before entering that branch.
    bool baseComponents=component(db,"GearRarityComponents",0x38,0,"XGearRarityComponent","ItemRarity")
        && component(db,"GearLevelComponents",0xb0,0,"XGearLevelComponent","ItemLevel")
        && classIs(ptr(db+0x188),"XGearItem");
    for(unsigned slot=10;slot<=13;slot++)
        baseComponents=component(db,"GearSlotComponents",0x74,slot,"XGearSlotComponent","GearSlot",0x2c) && baseComponents;
    Obj objectClass=ptr(pc+0x20);
    for(unsigned depth=0;objectClass && !nameIs(objectClass,"Object") && depth<32;depth++)objectClass=ptr(objectClass+0x34);
    if(!baseComponents || !objectClass || !nameIs(objectClass,"Object") || !nameIs(noDynamic,"DynamicEffect_None")) {
        status("Gear: unrecognized catalog; nothing added.");return;
    }
    Choice catalogue[count+2];memcpy(catalogue,choices,sizeof(choices));unsigned catalogueCount=count;
    unsigned present=0;
    // Validate the entire catalogue before a transaction. Loading archetypes
    // only resolves data; it neither spawns world pickups nor rolls random loot.
    for(unsigned i=0;i<count;i++) {
        const Choice &choice=choices[i];Obj pickup=loadPickup(objectClass,choice);
        uint64_t id=itemID(choice);
        if(!pickup || !authored(pickup,db,choice,noDynamic) || !classIs(progression_loot::getItem(db,id),"XGearItem")) {
            modLog("Gear: invalid authored pickup %s id=%08x:%08x",choice.pickup,(unsigned)(id>>32),(unsigned)id);
            status("Gear: incomplete or unrecognized catalog; nothing added. See the log.");return;
        }
        int quantity=progression_loot::quantity(inventory,id);
        if(quantity<0){status("Gear: inconsistent inventory; nothing added.");return;}
        if(quantity>0)present++;
    }
    for(const Choice &candidate:optionalChoices) {
        Choice resolved={};
        if(!optional(objectClass,db,noDynamic,candidate,resolved)) {
            modLog("Gear: optional pickup %s has no verified authored definition; skipped",candidate.pickup);continue;
        }
        uint64_t id=itemID(resolved);int quantity=progression_loot::quantity(inventory,id);
        if(quantity<0){status("Gear: inconsistent inventory; nothing added.");return;}
        catalogue[catalogueCount++]=resolved;if(quantity>0)present++;
        modLog("Gear: optional authored pickup %s id=%08x:%08x",candidate.pickup,(unsigned)(id>>32),(unsigned)id);
    }
    if(present==catalogueCount) {
        char text[192];snprintf(text,sizeof(text),"All %u catalog gear items are already owned.",catalogueCount);status(text);return;
    }
    unsigned added=0;
    for(unsigned i=0;i<catalogueCount;i++) {
        uint64_t id=itemID(catalogue[i]);
        // Reacquire the item array after each award: a transaction can relocate
        // it and can acquire additional items through the game's own events.
        if(ptr(pc+0x1fc)!=pawn || !progression_loot::snapshot(pc,inventory))break;
        int quantity=progression_loot::quantity(inventory,id);
        if(quantity<0)break;
        if(quantity>0)continue;
        if(!progression_loot::award(pc,transaction,id))break;
        added++;
    }
    unsigned owned=0;
    if(progression_loot::snapshot(pc,inventory))
        for(unsigned i=0;i<catalogueCount;i++)if(progression_loot::quantity(inventory,itemID(catalogue[i]))>0)owned++;
    char text[192];
    snprintf(text,sizeof(text),"Gear: %u / %u catalog items owned, %u added, %u already present.%s",owned,catalogueCount,added,present,
        owned==catalogueCount?"":" Grant interrupted; see the log.");status(text);
}
