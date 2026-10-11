// Catalogue and native weak-reference validation for retail vigor upgrades.
// Cost-zero/idempotent native awards are exercised by loot_fixture.h.
static void testVigorUpgrades() {
    using namespace progression_vigor_upgrades;
    unsigned perVigor[8]={};CHECK(count==16);
    const unsigned omitted[]={21,109,187,189,202,337};
    for(unsigned i=0;i<count;i++) {
        CHECK(choices[i].vigor<8);perVigor[choices[i].vigor]++;
        CHECK(!strstr(choices[i].name,"NoEscape"));
        for(unsigned id:omitted)CHECK(choices[i].id!=id);
        for(unsigned j=0;j<i;j++)CHECK(choices[i].id!=choices[j].id);
    }
    for(unsigned n:perVigor)CHECK(n==2);
    host=(BYTE *)VirtualAlloc(nullptr,0x1400000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    resource_fixture::Engine e;
    BYTE *weaponClass=e.object("XWeapon",e.meta),*derived=e.object("XWeaponRollingThunder",e.meta);
    *(Obj *)(derived+0x34)=(Obj)weaponClass;
    BYTE *package=e.object("PreCoalescedItemAssets",nullptr),*otherPackage=e.object("OtherAssets",nullptr);
    BYTE item[0xf0]={},manager[0x300]={};Obj instances[8]={},archetypes[8]={};
    for(unsigned i=0;i<8;i++) {
        BYTE *type=i==2?derived:weaponClass;
        archetypes[i]=(Obj)e.object(vigorNames[i],type);*(Obj *)(archetypes[i]+0x14)=(Obj)package;
        instances[i]=(Obj)e.object("OwnedVigor",type);*(Obj *)(instances[i]+0x24)=archetypes[i];
        *(Obj *)(manager+0x1fc+(i+1)*4)=instances[i];
        memcpy(item+0x94,package+0x18,8);memcpy(item+0xa4,(void *)(archetypes[i]+0x18),8);
        CHECK(referenceMatches((Obj)item,i));CHECK(!referenceMatches((Obj)item,(i+1)%8));
        CHECK(hasVigor((Obj)manager,i));
        *(unsigned *)(item+0xa8)=1;CHECK(!referenceMatches((Obj)item,i));*(unsigned *)(item+0xa8)=0;
        *(unsigned *)(item+0x98)=1;CHECK(!referenceMatches((Obj)item,i));*(unsigned *)(item+0x98)=0;
        *(uint64_t *)(item+0x9c)=1;CHECK(!referenceMatches((Obj)item,i));*(uint64_t *)(item+0x9c)=0;
        *(Obj *)(archetypes[i]+0x14)=(Obj)otherPackage;CHECK(!hasVigor((Obj)manager,i));
        *(Obj *)(archetypes[i]+0x14)=(Obj)package;
        *(unsigned *)(archetypes[i]+0x1c)=1;CHECK(!hasVigor((Obj)manager,i));*(unsigned *)(archetypes[i]+0x1c)=0;
    }
    CHECK(!referenceMatches(0,0) && !referenceMatches((Obj)item,8) && !hasVigor((Obj)manager,8));
    CHECK(!classIs(instances[2],"XWeapon") && hasVigor((Obj)manager,2));
    *(Obj *)(manager+0x1fc+3*4)=0;*(Obj *)(manager+0x1fc)=instances[2];
    CHECK(!hasVigor((Obj)manager,2)); // Slot zero is not owned inventory.
    *(Obj *)(manager+0x1fc+35*4)=instances[2];CHECK(hasVigor((Obj)manager,2));
    *(Obj *)(derived+0x34)=(Obj)derived;CHECK(!hasVigor((Obj)manager,2));
    *(Obj *)(derived+0x34)=(Obj)weaponClass;
    *(Obj *)(instances[2]+0x24)=0;CHECK(!hasVigor((Obj)manager,2));
    VirtualFree(host,0,MEM_RELEASE);host=nullptr;
    puts("PASS: 16 unique retail vigor upgrades, two per power, obsolete trap variants excluded, exact weak-reference names/numbers, derived vigor instances, reserved/last slots and invalid references.");
}
