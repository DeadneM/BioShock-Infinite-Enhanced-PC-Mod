// One-shot current-value refill. Capacity attributes and infusion inventory
// are read-only; this action neither revives a dead pawn nor unlocks a shield.
namespace vitals {
using HealthFn=void (__thiscall *)(void *,float);
using ShieldFn=void (__thiscall *)(void *,float,int);
struct Bindings {HealthFn health=nullptr;ShieldFn shield=nullptr;};
const BYTE healthSignature[]={0x51,0x56,0x57,0x8b,0xf9,0x8b,0x87,0xa0,0x03,0,0,0x8b,0x90,0xac,0,0,0};
const BYTE shieldSignature[]={0x0f,0x57,0xd2,0xf3,0x0f,0x10,0x44,0x24,0x04,0x0f,0x2f,0xd0,0x53,0x55,0x56,0x8b,0xf1};
Bindings bind() {
    // This GOG build's health setter clamps against the native health trait,
    // updates XHealth, the health RTPC and section HUD; ret 4. Shield setter
    // clamps against XShieldMax and updates the HUD/effects; ret 8. The final
    // int disables full/empty shield reward events (zero is also used by the
    // native forced full refill at VA 0x6E05E8). No raw value is written.
    const BYTE retHealth[]={0xc2,0x04,0},retShield[]={0xc2,0x08,0};
    if(!host || memcmp(host+0x3554d0,healthSignature,sizeof(healthSignature))
        || memcmp(host+0x3555f3,retHealth,sizeof(retHealth))
        || memcmp(host+0x5eddf0,shieldSignature,sizeof(shieldSignature))
        || memcmp(host+0x5edf7e,retShield,sizeof(retShield)))return {};
    return {(HealthFn)(host+0x3554d0),(ShieldFn)(host+0x5eddf0)};
}
bool property(Obj object,const char *name,const char *kind,unsigned offset,unsigned size) {
    return progression_resources::propertyIs(field(object,name,kind),kind,offset,size);
}
bool currentPlayer(Obj pc,Obj pawn) {
    Obj actual=0,manager=0;
    return GetCurrentThreadId()==windowThread && player(pc,actual,manager) && actual==pawn
        && !(read<unsigned>(pc+0x248)&0x20000) && read<float>(pawn+0x25fc)>0;
}
struct Snapshot {
    Obj definition=0,adjust=0;
    float health=0,healthMax=0,shield=0,shieldMax=0;
    int salts=0,saltsMax=0;
};
bool bounded(float value,float maximum,bool allowZeroMaximum) {
    // Capacity is the live native value. The upper bound only prevents unsafe
    // non-finite/extreme values and does not invent a gameplay maximum.
    return value>=0 && value<10000000 && maximum>=0 && maximum<10000000
        && (allowZeroMaximum || maximum>0);
}
bool snapshot(Obj pc,Obj pawn,Snapshot &out) {
    if(!currentPlayer(pc,pawn))return false;
    Obj cinematic=field(pc,"bCinematicMode","BoolProperty");
    if(!property(pc,"bCinematicMode","BoolProperty",0x248,4)
        || read<unsigned>(cinematic+0x58)!=0x20000
        || !property(pawn,"Health","FloatProperty",0x2ac,4)
        || !property(pawn,"XHealth","FloatProperty",0x25fc,4)
        || !property(pawn,"XHealthMax","FloatProperty",0x2600,4)
        || !property(pawn,"XShield","FloatProperty",0x2604,4)
        || !property(pawn,"XShieldMax","FloatProperty",0x2608,4))return false;
    Snapshot value;
    value.definition=progression_resources::characterDefinition(pc);
    Obj fallback=field(pc,"DefaultCharacterDefinition","StructProperty"),structure=ptr(fallback+0x58);
    static const Param params[]={{"CurrencyType","ByteProperty",0,1},{"Cost","IntProperty",4,4}};
    value.adjust=function(pc,"ClientAdjustCurrency",8,params,2);
    if(!value.definition || !value.adjust || ptr(ptr(pc)+0x360)!=(Obj)host+0x6ce200
        || !progression_resources::propertyIs(progression_resources::structField(structure,"VigorEnergy"),"IntProperty",0x58,4)
        || !progression_resources::maximum(pawn,"VigorEnergySizeAttrib",0x449c,value.saltsMax)
        || value.saltsMax>=10000000
        || !copy(value.definition+0x58,&value.salts,4)
        || value.salts<0 || value.salts>=10000000)return false;
    if(!copy(pawn+0x25fc,&value.health,4) || !copy(pawn+0x2600,&value.healthMax,4)
        || !copy(pawn+0x2604,&value.shield,4) || !copy(pawn+0x2608,&value.shieldMax,4)
        || !bounded(value.health,value.healthMax,false) || !bounded(value.shield,value.shieldMax,true))return false;
    out=value;return currentPlayer(pc,pawn) && progression_resources::characterDefinition(pc)==out.definition;
}
bool samePlayer(Obj pc,Obj pawn,const Snapshot &before) {
    return currentPlayer(pc,pawn) && progression_resources::characterDefinition(pc)==before.definition;
}
bool reached(float value,float previous,float maximum) {
    // Preserve any legitimate over-cap value instead of reducing it.
    float expected=previous>=maximum?previous:maximum;
    return fabsf(value-expected)<=0.0001f*(1.0f+fabsf(expected));
}
bool fillUsing(Obj pc,Obj pawn,const Bindings &native) {
    Snapshot before;
    if(!native.health || !native.shield || !snapshot(pc,pawn,before)) {
        status("Jauges : joueur ou structure non reconnus, aucune recharge.");return false;
    }
    bool invoked=true;
    if(before.health<before.healthMax) {
        if(!samePlayer(pc,pawn,before))invoked=false;
        else native.health((void *)pawn,before.healthMax);
    }
    if(before.shield<before.shieldMax && invoked) {
        if(!samePlayer(pc,pawn,before))invoked=false;
        else native.shield((void *)pawn,before.shieldMax,0);
    }
    if(before.salts<before.saltsMax && invoked) {
        if(!samePlayer(pc,pawn,before))invoked=false;
        else {
            // Native currency selector four is VigorEnergy. ClientAdjustCurrency
            // updates the selected character, HUD and native currency observers.
            progression_resources::CurrencyArgs args={4,{},before.saltsMax-before.salts};
            invoked=dispatch(pc,before.adjust,&args);
        }
    }
    Snapshot after;
    bool observed=invoked && samePlayer(pc,pawn,before) && snapshot(pc,pawn,after)
        && before.definition==after.definition;
    bool ok=observed && before.healthMax==after.healthMax && before.shieldMax==after.shieldMax
        && before.saltsMax==after.saltsMax && reached(after.health,before.health,before.healthMax)
        && reached(after.shield,before.shield,before.shieldMax)
        && after.salts==(before.salts>=before.saltsMax?before.salts:before.saltsMax);
    modLog("Vitals: health %.3f->%.3f/%.3f shield %.3f->%.3f/%.3f salts %d->%d/%d observed=%d verified=%d",
        before.health,observed?after.health:-1.0f,before.healthMax,
        before.shield,observed?after.shield:-1.0f,before.shieldMax,
        before.salts,observed?after.salts:-1,before.saltsMax,observed,ok);
    char text[192];
    if(ok)snprintf(text,sizeof(text),"Jauges rechargees : sante %.0f / %.0f, bouclier %.0f / %.0f, sels %d / %d.",
        after.health,after.healthMax,after.shield,after.shieldMax,after.salts,after.saltsMax);
    else snprintf(text,sizeof(text),"Jauges : recharge non confirmee, voir le journal.");
    status(text);return ok;
}
void refill(Obj pc,Obj pawn) {fillUsing(pc,pawn,bind());}
}
