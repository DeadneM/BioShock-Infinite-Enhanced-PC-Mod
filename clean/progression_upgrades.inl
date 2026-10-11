namespace progression_upgrades {
struct Choice {unsigned id;const char *name;unsigned replacement=0;const char *replacementName=nullptr;};
// Four retail upgrades for each of the twelve campaign weapons, damage I
// before damage II. Promotional damage-I replacements count as already owned.
static const Choice choices[]={
    {274,"Upgrade_Pistol_Damage1",94,"Upgrade_Pistol_Damage1SP"},{150,"Upgrade_Pistol_Damage2"},
    {371,"Upgrade_Pistol_ClipSize"},{149,"Upgrade_Pistol_Reserve"},
    {139,"Upgrade_MachineGun_Damage1",100,"Upgrade_MachineGun_Damage1SP"},{375,"Upgrade_MachineGun_Damage2"},
    {140,"Upgrade_MachineGun_ClipSize"},{233,"Upgrade_MachineGun_Spread"},
    {133,"Upgrade_Shotgun_Damage1",405,"Upgrade_Shotgun_Damage1CE"},{78,"Upgrade_Shotgun_Damage2"},
    {170,"Upgrade_Shotgun_ReloadSpeed"},{340,"Upgrade_Shotgun_ConeSize"},
    {132,"Upgrade_ThreeShotCarbine_Damage1"},{309,"Upgrade_ThreeShotCarbine_Damage2"},
    {406,"Upgrade_ThreeShotCarbine_ClipSize"},{397,"Upgrade_ThreeShotCarbine_ReloadSpeed"},
    {143,"Upgrade_HandCannon_Damage1"},{141,"Upgrade_HandCannon_Damage2"},
    {223,"Upgrade_Handcannon_ReloadSpeed"},{331,"Upgrade_Handcannon_Recoil"},
    {199,"Upgrade_SniperRifle_Damage1",302,"Upgrade_SniperRifle_Damage1CE"},{301,"Upgrade_SniperRifle_Damage2"},
    {178,"Upgrade_SniperRifle_FireRate"},{138,"Upgrade_SniperRifle_Recoil"},
    {188,"Upgrade_RPG_Damage1"},{383,"Upgrade_RPG_Damage2"},
    {110,"Upgrade_RPG_ProjectileSpeed"},{193,"Upgrade_RPG_ClipSize"},
    {44,"Upgrade_FlakCannon_Damage1"},{46,"Upgrade_FlakCannon_Damage2"},
    {35,"Upgrade_FlakCannon_ClipSize"},{47,"Upgrade_FlakCannon_ExplosionRadius"},
    {96,"Upgrade_MachineGun_VP_Damage1"},{98,"Upgrade_MachineGun_VP_Damage2"},
    {303,"Upgrade_MachineGun_VP_ClipSize"},{228,"Upgrade_MachineGun_VP_Recoil"},
    {185,"Upgrade_ThreeShotCarbine_VP_Damage1"},{82,"Upgrade_ThreeShotCarbine_VP_Damage2"},
    {19,"Upgrade_ThreeShotCarbine_VP_ReserveSize"},{339,"Upgrade_ThreeShotCarbine_VP_Recoil"},
    {396,"Upgrade_Shotgun_VP_Damage1"},{205,"Upgrade_Shotgun_VP_Damage2"},
    {127,"Upgrade_Shotgun_VP_ReloadSpeed"},{457,"Upgrade_Shotgun_VP_ConeSize"},
    {176,"Upgrade_FlakCannon_VP_Damage1"},{136,"Upgrade_FlakCannon_VP_Damage2"},
    {288,"Upgrade_FlakCannon_VP_ClipSize"},{134,"Upgrade_FlakCannon_VP_ExplosionRadius"}
};
static const unsigned count=sizeof(choices)/sizeof(*choices);
static_assert(count==48,"retail weapon upgrade catalogue");
bool property(Obj object,const char *name,const char *kind,unsigned offset,unsigned size) {
    return progression_resources::propertyIs(field(object,name,kind),kind,offset,size);
}
Obj resolve(Obj db,unsigned id,const char *name) {
    if(!id || id>=(unsigned)read<int>(db+0x30))return 0;
    Obj row=ptr(db+0x2c)+id*0x5c;
    // FName is at row+0x4c; nameIs reads object+0x18 only.
    if(!nameIs(row+0x34,name) || read<unsigned>(row+0x50)!=0
        || !nameIs(ptr(row+4),"XWeaponModItem")) {
        modLog("Upgrades: row mismatch id=%u name=%s fname=%u:%u class=%p",id,name,read<unsigned>(row+0x4c),read<unsigned>(row+0x50),(void *)ptr(row+4));return 0;
    }
    Obj item=progression_loot::getItem(db,id);
    if(!item) {
        // If normal resolution fails, also try the same native
        // find-if-load-fails path as the weapon giver;
        // retain all identity, ID and reflected-property checks afterwards.
        using LoadFn=void *(__cdecl *)(void *,void *,const wchar_t *,const wchar_t *,unsigned,void *,int);
        wchar_t path[160];swprintf(path,160,L"PreCoalescedItemAssets.%hs",name);
        item=(Obj)((LoadFn)(host+0x1066f0))((void *)ptr(row+4),nullptr,path,nullptr,0x2002,nullptr,1);
        modLog("Upgrades: native direct resolution id=%u result=%p",id,(void *)item);
    }
    if(!classIs(item,"XWeaponModItem") || !nameIs(item,name) || read<unsigned>(item+0x1c)!=0
        || !property(item,"ItemLookupID","QwordProperty",0x2c,8) || read<uint64_t>(item+0x2c)!=id
        || !property(item,"GameplayModDefinitions","ObjectProperty",0xe4,4) || !ptr(item+0xe4)
        || !property(item,"ObsolescentItemID","QwordProperty",0xe8,8)
        || !property(item,"WeaponArchetypeRef","XWeakReferenceProperty",0x94,32)) {
        modLog("Upgrades: item rejected id=%u obj=%p class=%p fname=%u:%u lookup=%08x:%08x mods=%p expectedName=%d exactClass=%d",
            id,(void *)item,(void *)ptr(item+0x20),read<unsigned>(item+0x18),read<unsigned>(item+0x1c),
            read<unsigned>(item+0x30),read<unsigned>(item+0x2c),(void *)ptr(item+0xe4),nameIs(item,name),classIs(item,"XWeaponModItem"));return 0;
    }
    return item;
}
int owned(const progression_loot::Inventory &inventory,const Choice &choice) {
    int standard=progression_loot::quantity(inventory,choice.id);
    if(standard<0)return -1;
    int alternate=choice.replacement?progression_loot::quantity(inventory,choice.replacement):0;
    return alternate<0?-1:(standard>0 || alternate>0?1:0);
}
bool hasWeapon(Obj manager,uint64_t package,uint64_t asset) {
    for(unsigned i=1;i<36;i++) {
        Obj weapon=ptr(manager+0x1fc+i*4),archetype=ptr(weapon+0x24);
        if(classIs(weapon,"XWeapon") && archetype && read<uint64_t>(archetype+0x18)==asset
            && read<uint64_t>(ptr(archetype+0x14)+0x18)==package)return true;
    }
    return false;
}
}
static void grantWeaponUpgrades(Obj pc,Obj pawn,Obj manager) {
    using namespace progression_upgrades;
    progression_loot::Inventory inventory;Obj db=progression_loot::database(),fn=progression_loot::transaction(pc);
    if(GetCurrentThreadId()!=windowThread || ptr(pc+0x1fc)!=pawn || !db || !fn
        || !inventoryLayout(manager) || !progression_loot::snapshot(pc,inventory)) {
        status("Ameliorations : inventaire indisponible, aucun ajout.");return;
    }
    unsigned present=0,weaponCount=0;uint64_t packages[12]={},assets[12]={};bool catalogueOK=true;
    for(unsigned i=0;i<count;i++) {
        const Choice &choice=choices[i];Obj item=resolve(db,choice.id,choice.name);
        int already=owned(inventory,choice);
        if(!item || already<0) {
            modLog("Upgrades: invalid catalogue %s id=%u",choice.name,choice.id);
            catalogueOK=false;continue;
        }
        if(choice.replacement && progression_loot::quantity(inventory,choice.replacement)>0) {
            Obj replacement=resolve(db,choice.replacement,choice.replacementName);
            if(!replacement || read<uint64_t>(replacement+0xe8)!=choice.id) {
                status("Ameliorations : variante bonus non reconnue, aucun ajout.");return;
            }
        }
        if(already){present++;continue;}
        uint64_t package=read<uint64_t>(item+0x94),asset=read<uint64_t>(item+0xa4);
        if(!package || !asset || read<uint64_t>(item+0x9c)!=0) {
            status("Ameliorations : reference d'arme non reconnue, aucun ajout.");return;
        }
        bool found=hasWeapon(manager,package,asset);
        for(unsigned j=0;j<weaponCount;j++)if(packages[j]==package && assets[j]==asset)found=true;
        if(!found) {
            if(weaponCount==12){status("Ameliorations : catalogue d'armes incoherent.");return;}
            packages[weaponCount]=package;assets[weaponCount++]=asset;
        }
    }
    if(!catalogueOK){status("Ameliorations : catalogue non reconnu, aucun ajout. Voir le journal.");return;}
    if(present==count){status("Les 48 ameliorations d'armes sont deja possedees.");return;}
    unsigned freeSlots=0;for(unsigned i=1;i<36;i++)if(!ptr(manager+0x1fc+i*4))freeSlots++;
    // The native modifier may acquire its associated weapon. Reserve room
    // before starting instead of silently replacing an existing inventory slot.
    if(freeSlots<weaponCount){status("Ameliorations : inventaire plein, aucun objet remplace.");return;}
    unsigned added=0;
    for(unsigned i=0;i<count;i++) {
        if(ptr(pc+0x1fc)!=pawn || !progression_loot::snapshot(pc,inventory))break;
        int already=owned(inventory,choices[i]);if(already<0)break;if(already)continue;
        if(!progression_loot::award(pc,fn,choices[i].id))break;
        added++;
    }
    unsigned confirmed=0;
    if(progression_loot::snapshot(pc,inventory))
        for(unsigned i=0;i<count;i++)if(owned(inventory,choices[i])==1)confirmed++;
    char text[192];snprintf(text,sizeof(text),"Ameliorations : %u / %u possedees, %u ajoutee(s), %u deja presentes.%s",confirmed,count,added,present,
        confirmed==count?"":" Ajout interrompu, voir le journal.");status(text);
}
