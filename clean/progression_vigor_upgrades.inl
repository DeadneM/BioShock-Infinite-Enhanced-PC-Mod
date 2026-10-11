// Included after progression_items.inl: vigor instances can be XWeapon subclasses.
namespace progression_vigor_upgrades {
struct Choice {unsigned id;const char *name;unsigned vigor;};
// The two retail upgrades for each campaign vigor. The six *_NoEscape rows
// remaining in the item database are not part of the retail upgrade catalogue.
static const Choice choices[]={
    {198,"Tonic_Mesmerize_MezHumans",0},{203,"Tonic_Mesmerize_ShotCount",0},
    {330,"Tonic_DevilsKiss_Cluster",1},{412,"Tonic_DevilsKiss_ExplosiveDamage",1},
    {433,"Tonic_BuckingBronco_Duration",2},{522,"Tonic_BuckingBronco_ChainBronco",2},
    {114,"Tonic_Charge_ShieldRecharge",3},{316,"Tonic_Charge_ExplosiveDamage",3},
    {333,"Tonic_MurderOfCrows_CorpseTrap",4},{395,"Tonic_MurderOfCrows_StunDuration",4},
    {409,"Tonic_Undertow_NumberEffected",5},{18,"Tonic_Undertow_Range",5},
    {186,"Tonic_VoltSwarm_ChainSteps",6},{201,"Tonic_VoltSwarm_ChainSteps2",6},
    {338,"Tonic_ReturnToSender_AbsorbAmmo",7},{334,"Tonic_ReturnToSender_ReduceCost",7}
};
static const unsigned count=sizeof(choices)/sizeof(*choices);
static_assert(count==16,"retail vigor upgrade catalogue");
bool referenceMatches(Obj item,unsigned vigor) {
    if(!item || vigor>=8)return false;
    // XWeakReference stores package FName, optional nested outer FName, and
    // asset FName. nameIs reads an FName at +0x18; adjust to these inline names.
    return nameIs(item+0x94-0x18,"PreCoalescedItemAssets") && read<unsigned>(item+0x98)==0
        && read<uint64_t>(item+0x9c)==0 && nameIs(item+0xa4-0x18,vigorNames[vigor])
        && read<unsigned>(item+0xa8)==0;
}
bool hasVigor(Obj manager,unsigned vigor) {
    if(vigor>=8)return false;
    for(unsigned i=1;i<36;i++) {
        Obj instance=ptr(manager+0x1fc+i*4),archetype=ptr(instance+0x24);
        if(progression_powers::weaponBase(instance) && progression_powers::weaponBase(archetype)
            && nameIs(archetype,vigorNames[vigor]) && read<unsigned>(archetype+0x1c)==0
            && nameIs(ptr(archetype+0x14),"PreCoalescedItemAssets")
            && read<unsigned>(ptr(archetype+0x14)+0x1c)==0)return true;
    }
    return false;
}
}

static void grantVigorUpgrades(Obj pc,Obj pawn,Obj manager) {
    using namespace progression_vigor_upgrades;
    progression_loot::Inventory inventory;
    Obj db=progression_loot::database(),transaction=progression_loot::transaction(pc);
    if(GetCurrentThreadId()!=windowThread || ptr(pc+0x1fc)!=pawn || !db || !transaction
        || !inventoryLayout(manager) || !progression_loot::snapshot(pc,inventory)) {
        status("Ameliorations des pouvoirs : inventaire indisponible, aucun ajout.");return;
    }
    unsigned present=0;bool missingVigors[8]={},catalogueOK=true;
    // Resolve every native item and its actual gameplay modifier before the
    // first award. Localised database rows alone do not prove a usable item.
    for(unsigned i=0;i<count;i++) {
        const Choice &choice=choices[i];
        Obj item=progression_upgrades::resolve(db,choice.id,choice.name);
        int quantity=progression_loot::quantity(inventory,choice.id);
        if(!item || quantity<0 || !referenceMatches(item,choice.vigor)
            || !classIs(ptr(item+0xe4),"XEffectDynamicGameplayAttributeModMultiple")) {
            modLog("Vigor upgrades: invalid native item id=%u name=%s item=%p mods=%p reference=%d",
                choice.id,choice.name,(void *)item,(void *)ptr(item+0xe4),referenceMatches(item,choice.vigor));
            catalogueOK=false;continue;
        }
        if(quantity>0)present++;
        else if(!hasVigor(manager,choice.vigor))missingVigors[choice.vigor]=true;
    }
    if(!catalogueOK){status("Ameliorations des pouvoirs : catalogue non reconnu, aucun ajout. Voir le journal.");return;}
    if(present==count){status("Les 16 ameliorations des pouvoirs sont deja possedees.");return;}
    unsigned needed=0,freeSlots=0;
    for(bool missing:missingVigors)if(missing)needed++;
    for(unsigned i=1;i<36;i++)if(!ptr(manager+0x1fc+i*4))freeSlots++;
    // Applying a native modifier can acquire its vigor. Reserve one slot per
    // missing vigor, never one per upgrade, before issuing a transaction.
    if(freeSlots<needed){status("Ameliorations des pouvoirs : inventaire plein, aucun objet remplace.");return;}
    unsigned added=0;
    for(unsigned i=0;i<count;i++) {
        if(ptr(pc+0x1fc)!=pawn || !progression_loot::snapshot(pc,inventory))break;
        int quantity=progression_loot::quantity(inventory,choices[i].id);
        if(quantity<0)break;if(quantity>0)continue;
        if(!progression_loot::award(pc,transaction,choices[i].id))break;
        added++;
    }
    unsigned confirmed=0;
    if(progression_loot::snapshot(pc,inventory))
        for(unsigned i=0;i<count;i++)if(progression_loot::quantity(inventory,choices[i].id)>0)confirmed++;
    char text[192];
    snprintf(text,sizeof(text),"Ameliorations des pouvoirs : %u / %u possedees, %u ajoutee(s), %u deja presentes.%s",
        confirmed,count,added,present,confirmed==count?"":" Ajout interrompu, voir le journal.");status(text);
}
