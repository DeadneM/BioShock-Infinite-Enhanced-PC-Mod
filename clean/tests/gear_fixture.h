// Exercises the two layout details which can silently select the wrong gear:
// sparse map occupancy, and numbered FNames used by the bonus _2 items.
// Include after resources_fixture.h; call testGear() from the main test runner.
static void testGear() {
    using namespace progression_gear;
    CHECK(count==49);
    CHECK(itemID(choices[0])==UINT64_C(0x0001000017200068));
    CHECK(itemID(choices[48])==UINT64_C(0x000100001b200068));
    for(unsigned i=0;i<count;i++) {
        CHECK((itemID(choices[i])>>32)==0x10000);
        for(unsigned j=0;j<i;j++)CHECK(itemID(choices[i])!=itemID(choices[j]));
    }
    host=(BYTE *)VirtualAlloc(nullptr,0x1400000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    resource_fixture::Engine e;
    BYTE *mapType=e.object("MapProperty",e.meta),*dbType=e.object("XItemDatabase",e.meta);
    BYTE *effectType=e.object("XGearStaticEffectComponent",e.meta);
    BYTE *mapProperty=e.property(dbType,"GearStaticEffectComponents",mapType,0x128,60);
    e.property(effectType,"LookupID",e.byteType,0x28,1);
    BYTE *effect=e.object("StaticEffect_SoulTrap",effectType);effect[0x28]=217;
    BYTE db[0x200]={},entries[140*16]={};unsigned bitmap[5]={};
    *(Obj *)(db+0x20)=(Obj)dbType;BYTE *map=db+0x128;
    *(Obj *)map=(Obj)entries;*(int *)(map+4)=140;*(int *)(map+8)=140;
    *(Obj *)(map+0x1c)=(Obj)bitmap;*(int *)(map+0x20)=140;
    entries[139*16]=217;*(Obj *)(entries+139*16+4)=(Obj)effect;
    bitmap[4]=1u<<11;
    CHECK(component((Obj)db,"GearStaticEffectComponents",0x128,217,"XGearStaticEffectComponent")== (Obj)effect);
    bitmap[4]=0;CHECK(!component((Obj)db,"GearStaticEffectComponents",0x128,217,"XGearStaticEffectComponent"));
    bitmap[4]=1u<<11;
    // A freed entry with the same key/object must never count as a duplicate.
    entries[3*16]=217;*(Obj *)(entries+3*16+4)=(Obj)effect;
    CHECK(component((Obj)db,"GearStaticEffectComponents",0x128,217,"XGearStaticEffectComponent")== (Obj)effect);
    bitmap[0]=1u<<3;CHECK(!component((Obj)db,"GearStaticEffectComponents",0x128,217,"XGearStaticEffectComponent"));bitmap[0]=0;
    effect[0x28]=206;CHECK(!component((Obj)db,"GearStaticEffectComponents",0x128,217,"XGearStaticEffectComponent"));effect[0x28]=217;
    *(Obj *)(map+0x1c)=0;CHECK(!component((Obj)db,"GearStaticEffectComponents",0x128,217,"XGearStaticEffectComponent"));
    // Small maps use the embedded allocation bitmap.
    *(int *)(map+4)=4;*(int *)(map+8)=4;*(int *)(map+0x20)=4;*(unsigned *)(map+0xc)=1u<<3;
    CHECK(component((Obj)db,"GearStaticEffectComponents",0x128,217,"XGearStaticEffectComponent")== (Obj)effect);
    *(unsigned *)(mapProperty+0x30)=56;CHECK(!component((Obj)db,"GearStaticEffectComponents",0x128,217,"XGearStaticEffectComponent"));
    // Live regression: rarity and slot keys are not named LookupID, and the
    // slot's key follows its interface pointer at +0x2c rather than +0x28.
    BYTE *rarityType=e.object("XGearRarityComponent",e.meta),*slotType=e.object("XGearSlotComponent",e.meta);
    e.property(rarityType,"ItemRarity",e.byteType,0x28,1);e.property(slotType,"GearSlot",e.byteType,0x2c,1);
    e.property(dbType,"GearRarityComponents",mapType,0x38,60);e.property(dbType,"GearSlotComponents",mapType,0x74,60);
    BYTE *rarity=e.object("Common",rarityType),*slot=e.object("Hat",slotType);slot[0x2c]=10;
    BYTE rarityEntry[16]={},slotEntry[16]={};*(Obj *)(rarityEntry+4)=(Obj)rarity;slotEntry[0]=10;*(Obj *)(slotEntry+4)=(Obj)slot;
    for(unsigned offset:{0x38u,0x74u}) {
        *(Obj *)(db+offset)=(Obj)(offset==0x38?rarityEntry:slotEntry);
        *(int *)(db+offset+4)=1;*(int *)(db+offset+8)=1;*(unsigned *)(db+offset+12)=1;*(int *)(db+offset+0x20)=1;
    }
    CHECK(!component((Obj)db,"GearRarityComponents",0x38,0,"XGearRarityComponent"));
    CHECK(component((Obj)db,"GearRarityComponents",0x38,0,"XGearRarityComponent","ItemRarity")==(Obj)rarity);
    CHECK(!component((Obj)db,"GearSlotComponents",0x74,10,"XGearSlotComponent","GearSlot"));
    CHECK(component((Obj)db,"GearSlotComponents",0x74,10,"XGearSlotComponent","GearSlot",0x2c)==(Obj)slot);
    BYTE *package=e.object("Arc_Gear_SP_Pickups",nullptr);
    BYTE *pickup=e.object("Pickup_Gear_SoulTrap",nullptr);*(Obj *)(pickup+0x14)=(Obj)package;
    BYTE reference[32]={};memcpy(reference,package+0x18,8);memcpy(reference+16,pickup+0x18,8);
    CHECK(pickupName((Obj)pickup,"SoulTrap"));CHECK(!pickupName((Obj)pickup,"SoulTrap_2"));
    CHECK(weakReferenceIs((Obj)reference,(Obj)pickup));
    *(unsigned *)(pickup+0x1c)=3;
    CHECK(pickupName((Obj)pickup,"SoulTrap_2"));CHECK(!pickupName((Obj)pickup,"SoulTrap"));
    CHECK(!weakReferenceIs((Obj)reference,(Obj)pickup));
    memcpy(reference+16,pickup+0x18,8);CHECK(weakReferenceIs((Obj)reference,(Obj)pickup));
    *(unsigned *)(package+0x1c)=1;CHECK(!weakReferenceIs((Obj)reference,(Obj)pickup));
    VirtualFree(host,0,MEM_RELEASE);host=nullptr;
    puts("PASS: gear catalogue IDs unique, sparse-map inline/heap occupancy, freed/duplicate/wrong components, schema drift, bonus FName numbers and full weak-reference identity.");
}
