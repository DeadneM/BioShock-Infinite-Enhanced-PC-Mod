// Focused upgrade ownership and weapon-reference tests. Include after
// resources_fixture.h; the existing synthetic name pool supplies real FNames.
static void testUpgrades() {
    using namespace progression_upgrades;
    CHECK(count==48);
    unsigned replacements=0;
    for(unsigned i=0;i<count;i++) {
        CHECK(choices[i].id && choices[i].name);
        for(unsigned j=0;j<i;j++)CHECK(choices[i].id!=choices[j].id);
        if(choices[i].replacement) {
            CHECK(choices[i].replacementName);replacements++;
            for(unsigned j=0;j<count;j++)CHECK(choices[i].replacement!=choices[j].id);
        }
    }
    CHECK(replacements==4);
    CHECK(choices[0].id==274 && choices[0].replacement==94);
    CHECK(choices[4].id==139 && choices[4].replacement==100);
    CHECK(choices[8].id==133 && choices[8].replacement==405);
    CHECK(choices[20].id==199 && choices[20].replacement==302);
    // This GOG campaign ships the authored reload modifier/loot ID 397;
    // database ID 45 is a localisation-only recoil entry with no loadable asset.
    CHECK(choices[15].id==397 && !strcmp(choices[15].name,"Upgrade_ThreeShotCarbine_ReloadSpeed"));
    BYTE entries[3][32]={};BYTE definition[132]={};
    progression_loot::Inventory inventory={(Obj)definition,(Obj)entries,3};
    for(unsigned i=0;i<count;i++) {
        const Choice &choice=choices[i];memset(entries,0,sizeof(entries));
        *(uint64_t *)entries[0]=choice.id;*(uint64_t *)entries[1]=choice.replacement;
        CHECK(owned(inventory,choice)==0);
        *(int *)(entries[0]+8)=1;CHECK(owned(inventory,choice)==1);
        *(int *)(entries[0]+8)=0;*(int *)(entries[1]+8)=1;
        CHECK(owned(inventory,choice)==(choice.replacement?1:0));
        *(int *)(entries[0]+8)=1;CHECK(owned(inventory,choice)==1); // Retail + bonus remain one logical upgrade.
        *(int *)(entries[1]+8)=-1;CHECK(owned(inventory,choice)==-1); // Never ignore an invalid inventory entry.
        memset(entries,0,sizeof(entries));
        *(uint64_t *)entries[0]=(UINT64_C(1)<<32)|choice.id;*(int *)(entries[0]+8)=1;
        CHECK(owned(inventory,choice)==0); // Compare all 64 item-ID bits.
    }
    memset(entries,0,sizeof(entries));
    *(uint64_t *)entries[0]=choices[0].id;*(int *)(entries[0]+8)=0x7fffffff;
    *(uint64_t *)entries[1]=choices[0].id;*(int *)(entries[1]+8)=1;
    CHECK(owned(inventory,choices[0])==-1); // Duplicate rows cannot overflow the total.
    *(int *)(entries[0]+8)=1;CHECK(progression_loot::quantity(inventory,choices[0].id)==2);
    CHECK(owned(inventory,choices[0])==1);
    BYTE before[sizeof(entries)];memcpy(before,entries,sizeof(before));
    CHECK(owned(inventory,choices[0])==1 && !memcmp(before,entries,sizeof(before)));
    inventory.definition=0;CHECK(owned(inventory,choices[0])==-1);inventory.definition=(Obj)definition;
    inventory.count=8193;CHECK(owned(inventory,choices[0])==-1);
    inventory.count=0;inventory.data=0;CHECK(owned(inventory,choices[0])==0);

    host=(BYTE *)VirtualAlloc(nullptr,0x1400000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    resource_fixture::Engine e;
    BYTE *weaponClass=e.object("XWeapon",e.meta);
    BYTE *package=e.object("PreCoalescedItemAssets",nullptr);
    BYTE *otherPackage=e.object("OtherWeaponPackage",nullptr);
    BYTE *archetype=e.object("PistolFounder",weaponClass);
    BYTE *instance=e.object("PistolInstance",weaponClass);
    *(Obj *)(archetype+0x14)=(Obj)package;*(Obj *)(instance+0x24)=(Obj)archetype;
    uint64_t packageName=read<uint64_t>((Obj)package+0x18),assetName=read<uint64_t>((Obj)archetype+0x18);
    BYTE manager[0x300]={};auto slot=[&](unsigned i)->Obj &{CHECK(i<36);return *(Obj *)(manager+0x1fc+i*4);};
    CHECK(!hasWeapon((Obj)manager,packageName,assetName));
    slot(0)=(Obj)instance;CHECK(!hasWeapon((Obj)manager,packageName,assetName)); // Reserved slot is excluded.
    slot(1)=(Obj)instance;CHECK(hasWeapon((Obj)manager,packageName,assetName));
    slot(1)=0;slot(35)=(Obj)instance;CHECK(hasWeapon((Obj)manager,packageName,assetName));
    *(unsigned *)(archetype+0x1c)=1;CHECK(!hasWeapon((Obj)manager,packageName,assetName));
    CHECK(hasWeapon((Obj)manager,packageName,read<uint64_t>((Obj)archetype+0x18)));
    *(unsigned *)(archetype+0x1c)=0;
    *(unsigned *)(package+0x1c)=1;CHECK(!hasWeapon((Obj)manager,packageName,assetName));
    CHECK(hasWeapon((Obj)manager,read<uint64_t>((Obj)package+0x18),assetName));
    *(unsigned *)(package+0x1c)=0;
    *(Obj *)(archetype+0x14)=(Obj)otherPackage;CHECK(!hasWeapon((Obj)manager,packageName,assetName));
    *(Obj *)(archetype+0x14)=(Obj)package;
    *(Obj *)(instance+0x20)=(Obj)e.pawnClass;CHECK(!hasWeapon((Obj)manager,packageName,assetName));
    *(Obj *)(instance+0x20)=(Obj)weaponClass;
    *(Obj *)(instance+0x24)=0;CHECK(!hasWeapon((Obj)manager,packageName,assetName));
    slot(35)=1;CHECK(!hasWeapon((Obj)manager,packageName,assetName));
    VirtualFree(host,0,MEM_RELEASE);host=nullptr;
    puts("PASS: 48 unique upgrade IDs, four bonus ownership aliases, malformed/overflowing quantities, full 64-bit IDs, exact weapon/package FNames, reserved/last slots and invalid weapon references.");
}
