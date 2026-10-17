#include "shared.h"
#include "weapons.h"
#include "aim.h"
#include "input_profiles.h"
#include "MinHook.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

const WeaponChoice weaponChoices[]={
    {"Pistol",L"PistolFounder"},{"Machine Gun",L"MachineGunFounder"},
    {"Shotgun",L"ShotgunFounder"},{"Carbine",L"CarbineFounder"},
    {"Hand Cannon",L"HandCannonFounder"},{"Sniper Rifle",L"SniperRifleFounder"},
    {"RPG",L"RPGFounder"},{"Volley Gun",L"FlakCannonFounder"}
};
const int weaponChoiceCount=sizeof(weaponChoices)/sizeof(*weaponChoices);
namespace {
using Obj=uintptr_t;
BYTE *host;
using CameraFn=void (__thiscall *)(void *,void *,void *);
CameraFn cameraOriginal;
SRWLOCK stateLock=SRWLOCK_INIT;
char message[192]="Load a game to add a weapon.";
int pending=-1,pendingDestination=0;DWORD queuedAt,lastSeen,lastPoll,windowThread;
HWND gameWindow;
bool available,busy,grantsEnabled=true,progressionEnabled=true;
Obj latestController=0,latestPawn=0;
Obj queuedController=0,queuedPawn=0;
thread_local bool inside;

// Reads fail closed. All engine calls below run on the window/game thread,
// with the current controller supplied by the engine, never a cached actor.
bool copy(Obj p,void *out,size_t size) {
    SIZE_T got=0;return p && ReadProcessMemory(GetCurrentProcess(),(void *)p,out,size,&got) && got==size;
}
template<class T> T read(Obj p) {T v={};copy(p,&v,sizeof(v));return v;}
Obj ptr(Obj p) {return read<Obj>(p);}
bool nameIs(Obj object,const char *expected) {
    if(!object || !host)return false;
    unsigned id=read<unsigned>(object+0x18),num=read<unsigned>((Obj)host+0x13ccefc);
    if(!num || num>500000 || id>=num)return false;
    Obj pool=ptr((Obj)host+0x13ccef8),entry=ptr(pool+id*4);
    unsigned flags=read<unsigned>(entry+8);
    if((flags>>1)!=id || (flags&1))return false;
    char text[96]={};size_t n=strlen(expected)+1;
    return n<=sizeof(text) && copy(entry+0x10,text,n) && !strcmp(text,expected);
}
bool classIs(Obj object,const char *expected) {
    Obj cls=ptr(object+0x20);
    return object && nameIs(cls,expected) && nameIs(ptr(cls+0x20),"Class");
}
Obj field(Obj object,const char *name,const char *kind) {
    Obj cls=ptr(object+0x20);
    for(int depth=0;cls && depth<32;depth++,cls=ptr(cls+0x34)) {
        Obj child=ptr(cls+0x38);
        for(int n=0;child && n<4096;n++,child=ptr(child+0x28))
            if(nameIs(child,name) && classIs(child,kind))return child;
    }
    return 0;
}
struct Param {const char *name;const char *kind;unsigned offset,size;};
Obj function(Obj object,const char *name,unsigned size,const Param *params,unsigned count) {
    Obj f=field(object,name,"Function");
    if(!f || read<uint16_t>(f+0x82)!=size || read<uint8_t>(f+0x81)!=count)return 0;
    unsigned found=0;uint16_t returnOffset=0xffff;Obj child=ptr(f+0x38);
    for(unsigned n=0;child && n<128;n++,child=ptr(child+0x28)) {
        if(!(read<unsigned>(child+0x34)&0x80))continue;
        bool matched=false;
        for(unsigned i=0;i<count;i++)if(nameIs(child,params[i].name) && classIs(child,params[i].kind)
            && read<unsigned>(child+0x48)==params[i].offset && read<unsigned>(child+0x30)==params[i].size
            && read<unsigned>(child+0x2c)==1 && !(found&(1u<<i))) {
                found|=1u<<i;matched=true;if(read<unsigned>(child+0x34)&0x400)returnOffset=(uint16_t)params[i].offset;break;
            }
        if(!matched)return 0;
    }
    return found==((1u<<count)-1) && read<uint16_t>(f+0x84)==returnOffset?f:0;
}
bool dispatch(Obj object,Obj f,void *params) {
    Obj call=ptr(ptr(object)+0x7c);
    if(!f || (call!=(Obj)host+0x5f6c00 && call!=(Obj)host+0x8a0450))return false;
    using Fn=void (__thiscall *)(void *,void *,void *,void *);
    ((Fn)call)((void *)object,(void *)f,params,nullptr);return true;
}
void status(const char *text) {
    AcquireSRWLockExclusive(&stateLock);snprintf(message,sizeof(message),"%s",text);ReleaseSRWLockExclusive(&stateLock);
    modLog("Weapons: %s",text);
}
bool player(Obj pc,Obj &pawn,Obj &manager) {
    if(!classIs(pc,"XPlayerController") || !classIs(ptr(pc+0x23c),"XLocalPlayer"))return false;
    pawn=ptr(pc+0x1fc);manager=ptr(pawn+0x314);
    float health=read<float>(pawn+0x2ac);
    return classIs(pawn,"XHuman") && classIs(manager,"XInventoryManager")
        && ptr(pawn+0x218)==pc && ptr(manager+0x8c)==pawn && health>0 && health<100000;
}
#include "player_options.inl"
#include "reticle.inl"
#include "input_comfort.inl"
Obj carried(Obj manager,Obj archetype) {
    // InventoryItems is a fixed 36-element array, verified through reflection.
    Obj prop=field(manager,"InventoryItems","ObjectProperty");
    if(!prop || read<unsigned>(prop+0x48)!=0x1fc || read<unsigned>(prop+0x2c)!=36 || read<unsigned>(prop+0x30)!=4)return 0;
    for(unsigned i=1;i<36;i++) {
        Obj weapon=ptr(manager+0x1fc+i*4);
        if(weapon && ptr(weapon+0x24)==archetype && classIs(weapon,"XWeapon"))return weapon;
    }
    return 0;
}
static bool inventoryLayout(Obj manager) {
    Obj prop=field(manager,"InventoryItems","ObjectProperty");
    Obj active=field(manager,"EquippedWeaponIndex","IntProperty"),backup=field(manager,"BackupWeaponIndex","IntProperty");
    return prop && read<unsigned>(prop+0x48)==0x1fc && read<unsigned>(prop+0x2c)==36 && read<unsigned>(prop+0x30)==4
        && active && read<unsigned>(active+0x48)==0x2a0 && backup && read<unsigned>(backup+0x48)==0x2a4;
}
static int inventoryIndex(Obj manager,Obj weapon) {
    if(!weapon)return -1;
    for(int i=1;i<36;i++)if(ptr(manager+0x1fc+i*4)==weapon)return i;
    return -1;
}
static Obj slotWeapon(Obj manager,unsigned slotOffset) {
    int index=read<int>(manager+slotOffset);
    return index>0 && index<36?ptr(manager+0x1fc+index*4):0;
}
struct AmmoResult {int clip=-1,reserve=-1,maxClip=-1,maxReserve=-1;bool verified=false;};
static AmmoResult refill(Obj weapon) {
    AmmoResult result;
    static const Param ammoP[]={{"Amount","IntProperty",0,4},{"NotifyGameStats","BoolProperty",4,4}};
    static const Param spareP[]={{"Amount","IntProperty",0,4},{"NotifyGameStats","BoolProperty",4,4},{"ReturnValue","IntProperty",8,4}};
    static const Param countP[]={{"ReturnValue","IntProperty",0,4}};
    if(!classIs(weapon,"XWeapon"))return result;
    Obj add=function(weapon,"AddAmmo",8,ammoP,2),spare=function(weapon,"AddSpareAmmo",12,spareP,3);
    Obj clipMax=function(weapon,"GetRawMaxAmmoCount",4,countP,1),reserveMax=function(weapon,"GetRawMaxSpareAmmoCount",4,countP,1);
    Obj clipNow=function(weapon,"GetRawAmmoCount",4,countP,1),reserveNow=function(weapon,"GetRawSpareAmmoCount",4,countP,1);
    if(!add || !spare || !clipMax || !reserveMax || !clipNow || !reserveNow)return result;
    if(!dispatch(weapon,clipMax,&result.maxClip) || !dispatch(weapon,reserveMax,&result.maxReserve)
        || result.maxClip<0 || result.maxClip>100000 || result.maxReserve<0 || result.maxReserve>100000)return result;
    int oldClip=-1,oldReserve=-1;dispatch(weapon,clipNow,&oldClip);dispatch(weapon,reserveNow,&oldReserve);
    struct Spare {int amount,notify,added;} spareArgs={result.maxReserve,0,0};
    struct Ammo {int amount,notify;} ammoArgs={result.maxClip,0};
    // Reserve refills may auto-load an empty clip, so fill the clip first.
    bool ok=dispatch(weapon,add,&ammoArgs) && dispatch(weapon,spare,&spareArgs);
    ok=dispatch(weapon,clipNow,&result.clip) && dispatch(weapon,reserveNow,&result.reserve) && ok;
    result.verified=ok && result.clip>=result.maxClip && result.reserve>=result.maxReserve;
    modLog("Ammo: weapon=%p clip=%d->%d/%d reserve=%d->%d/%d verified=%d",(void *)weapon,
        oldClip,result.clip,result.maxClip,oldReserve,result.reserve,result.maxReserve,result.verified);
    return result;
}
static void refillBoth(Obj manager) {
    if(!inventoryLayout(manager)){status("Unrecognized inventory: ammo unchanged.");return;}
    AmmoResult active=refill(slotWeapon(manager,0x2a0)),backup=refill(slotWeapon(manager,0x2a4));
    char text[192];snprintf(text,sizeof(text),"Main %d / %d; secondary %d / %d (magazine / reserve).%s",active.clip,active.reserve,backup.clip,backup.reserve,
        active.verified && backup.verified?"":" Refill incomplete; see the log.");status(text);
}
void give(Obj pawn,Obj manager,int selected,int destination) {
    static const Param acquireP[]={{"WeaponArchetype","ObjectProperty",0,4},{"EquipWeaponItems","ByteProperty",4,1}};
    static const Param equipP[]={{"WeaponToEquip","ObjectProperty",0,4},{"FullBody","BoolProperty",4,4},{"TransferCurrentToBackup","BoolProperty",8,4}};
    static const Param getP[]={{"Specifier","ByteProperty",0,1},{"ReturnValue","ObjectProperty",4,4}};
    static const Param backupP[]={{"InBackupWeaponIndex","IntProperty",0,4}};
    Obj acquire=function(pawn,"AcquireWeapon",5,acquireP,2),equip=function(manager,"EquipWeapon",12,equipP,3);
    Obj get=function(pawn,"GetEquippedWeapon",8,getP,2);
    Obj setBackup=function(manager,"ClientSetBackupWeaponIndex",4,backupP,1);
    if(!acquire || !equip || !get || !setBackup || !inventoryLayout(manager)) {status("Unrecognized game structure: no weapon added.");return;}
    struct Equipped {unsigned selector;Obj weapon;} equipped={};
    if(!dispatch(pawn,get,&equipped) || !classIs(equipped.weapon,"XWeapon")) {status("Equip a standard weapon in-game first.");return;}
    Obj weaponClass=ptr(equipped.weapon+0x20);
    wchar_t path[128];swprintf(path,128,L"PreCoalescedItemAssets.%ls",weaponChoices[selected].asset);
    // StaticLoadObject signature derived from this build's execDynamicLoadObject.
    // Calling the native loader avoids handing a non-engine FString allocation to UE3.
    using LoadFn=void *(__cdecl *)(void *,void *,const wchar_t *,const wchar_t *,unsigned,void *,int);
    Obj archetype=(Obj)((LoadFn)(host+0x1066f0))((void *)weaponClass,nullptr,path,nullptr,0x2002,nullptr,1);
    if(!classIs(archetype,"XWeapon") || !nameIs(ptr(archetype+0x14),"PreCoalescedItemAssets")) {status("Weapon model unavailable in this game.");return;}
    Obj prop=field(manager,"InventoryItems","ObjectProperty");
    if(!prop || read<unsigned>(prop+0x2c)!=36 || read<unsigned>(prop+0x48)!=0x1fc) {status("Unrecognized inventory: grant canceled.");return;}
    Obj weapon=carried(manager,archetype);
    if(!weapon) {
        bool room=false;for(unsigned i=1;i<36;i++)if(!ptr(manager+0x1fc+i*4)){room=true;break;}
        if(!room){status("Weapon inventory full: no weapon replaced.");return;}
        struct Acquire {Obj archetype;unsigned char noEquip;unsigned char padding[3];} args={archetype,1,{}};
        modLog("Weapons: acquire %ls, pawn=%p manager=%p thread=%lu",path,(void *)pawn,(void *)manager,GetCurrentThreadId());
        if(!dispatch(pawn,acquire,&args)){status("Weapon acquisition call rejected.");return;}
        weapon=carried(manager,archetype);
    }
    if(!weapon){status("The game did not add the weapon. See the log.");return;}
    Obj previous=equipped.weapon,previousBackup=slotWeapon(manager,0x2a4);
    if(destination==1) {
        if(weapon==previous){status("This weapon is already in the main slot. Choose another for the secondary slot.");return;}
        int index=inventoryIndex(manager,weapon);
        if(index<1 || !dispatch(manager,setBackup,&index)){status("Weapon present; secondary slot unconfirmed.");return;}
    } else {
        // Replace the hand slot while retaining the secondary. Selecting the
        // existing secondary swaps the pair instead of duplicating its pointer.
        struct Equip {Obj weapon;int fullBody,backup;} args={weapon,0,weapon==previousBackup?1:0};
        if(!dispatch(manager,equip,&args)){status("Weapon present; equip result unconfirmed.");return;}
    }
    AmmoResult ammo=refill(weapon);
    equipped={};dispatch(pawn,get,&equipped);
    bool placed=destination==1?slotWeapon(manager,0x2a4)==weapon && equipped.weapon==previous:equipped.weapon==weapon;
    char text[192];snprintf(text,sizeof(text),"%s: %s. Magazine %d / reserve %d.%s",weaponChoices[selected].label,
        placed?(destination==1?"secondary slot":"main slot"):"slot unconfirmed",ammo.clip,ammo.reserve,ammo.verified?"":" Refill unconfirmed.");status(text);
    modLog("Weapons: inventory instance=%p equipped=%p archetype=%p",(void *)weapon,(void *)equipped.weapon,(void *)archetype);
}
#include "progression_resources.inl"
#include "progression_loot.inl"
#include "progression_upgrades.inl"
#include "progression_items.inl"
#include "progression_vigor_upgrades.inl"
#include "progression_infusions.inl"
#include "vitals.inl"
void __fastcall camera(void *pc,void *,void *location,void *rotation) {
    cameraOriginal(pc,location,rotation);
    if(inside)return;
    HWND window=(HWND)InterlockedCompareExchangePointer((void *volatile *)&gameWindow,nullptr,nullptr);
    DWORD thread=(DWORD)InterlockedCompareExchange((volatile LONG *)&windowThread,0,0);
    if(!window || GetCurrentThreadId()!=thread)return;
    DWORD now=GetTickCount();int selected=-1,destination=0;DWORD age=0;Obj requestedPC=0,requestedPawn=0;
    inside=true;
    // This callback is also the aim update point. No engine call is made from Present.
    Obj aimPawn=ptr((Obj)pc+0x1fc);
    updateAim(pc,window,available && latestController==(Obj)pc && latestPawn==aimPawn);
    reticle::update((Obj)pc,aimPawn,available && latestController==(Obj)pc && latestPawn==aimPawn);
    AcquireSRWLockExclusive(&stateLock);
    if(now-lastPoll<500 && pending==-1){ReleaseSRWLockExclusive(&stateLock);inside=false;return;}
    lastPoll=now;
    if(pending!=-1){selected=pending;destination=pendingDestination;pending=-1;age=now-queuedAt;busy=true;requestedPC=queuedController;requestedPawn=queuedPawn;}
    ReleaseSRWLockExclusive(&stateLock);
    Obj pawn=0,manager=0;bool ready=player((Obj)pc,pawn,manager);
    latestController=(Obj)pc;latestPawn=pawn;
    player_options::inspect((Obj)pc,pawn,ready);
    input_comfort::inspect((Obj)pc,pawn,ready);
    AcquireSRWLockExclusive(&stateLock);bool changed=available!=ready;available=ready;lastSeen=now;ReleaseSRWLockExclusive(&stateLock);
    if(changed)modLog("Weapons: player %s, game thread=%lu",ready?"ready":"unavailable",thread);
    if(selected!=-1) {
        if(!ready || age>2000 || GetForegroundWindow()!=window
            || requestedPC!=(Obj)pc || requestedPawn!=pawn || (read<unsigned>((Obj)pc+0x248)&0x20000))status("Grant canceled: return to active gameplay.");
        else if(selected==-2)refillBoth(manager);
        else if(selected==-3)grantPowers((Obj)pc,pawn,manager);
        else if(selected==-4)grantGear((Obj)pc,pawn,manager);
        else if(selected==-5)grantWeaponUpgrades((Obj)pc,pawn,manager);
        else if(selected==-6)grantMoney((Obj)pc,pawn);
        else if(selected==-7)grantLockpicks((Obj)pc,pawn);
        else if(selected==-8)grantVigorUpgrades((Obj)pc,pawn,manager);
        else if(selected==-9)grantInfusions((Obj)pc,pawn);
        else if(selected==-10)vitals::refill((Obj)pc,pawn);
        else give(pawn,manager,selected,destination);
        AcquireSRWLockExclusive(&stateLock);busy=false;ReleaseSRWLockExclusive(&stateLock);
    }
    inside=false;
}
}
void weaponWindow(HWND window) {
    DWORD process=0,thread=GetWindowThreadProcessId(window,&process);
    if(process!=GetCurrentProcessId())return;
    InterlockedExchange((volatile LONG *)&windowThread,(LONG)thread);
    InterlockedExchangePointer((void *volatile *)&gameWindow,window);
}
bool requestWeapon(int index,int destination) {
    AcquireSRWLockExclusive(&stateLock);DWORD now=GetTickCount();
    bool ok=grantsEnabled && index>=0 && index<weaponChoiceCount && (destination==0 || destination==1) && available && now-lastSeen<1500 && pending==-1 && !busy;
    if(ok){pending=index;pendingDestination=destination;queuedAt=now;queuedController=latestController;queuedPawn=latestPawn;snprintf(message,sizeof(message),"Grant requested...");}
    else if(pending==-1 && !busy)snprintf(message,sizeof(message),"%s",grantsEnabled?"Load active gameplay with Booker and his weapons.":"Weapon grants disabled in InfiniteMod.ini.");
    ReleaseSRWLockExclusive(&stateLock);return ok;
}
bool requestAmmo() {
    AcquireSRWLockExclusive(&stateLock);DWORD now=GetTickCount();
    bool ok=grantsEnabled && available && now-lastSeen<1500 && pending==-1 && !busy;
    if(ok){pending=-2;queuedAt=now;queuedController=latestController;queuedPawn=latestPawn;snprintf(message,sizeof(message),"Ammo requested for both weapons...");}
    else if(pending==-1 && !busy)snprintf(message,sizeof(message),"%s",grantsEnabled?"Load active gameplay with Booker and his weapons.":"Weapon grants disabled in InfiniteMod.ini.");
    ReleaseSRWLockExclusive(&stateLock);return ok;
}
bool requestProgression(ProgressionAction action) {
    const int index=(int)action;
    AcquireSRWLockExclusive(&stateLock);DWORD now=GetTickCount();
    bool ok=progressionEnabled && index>=0 && index<(int)ProgressionAction::Count
        && available && now-lastSeen<1500 && pending==-1 && !busy;
    if(ok) {
        pending=-3-index;queuedAt=now;queuedController=latestController;queuedPawn=latestPawn;
        snprintf(message,sizeof(message),"Grant requested...");
    } else if(pending==-1 && !busy)snprintf(message,sizeof(message),"%s",progressionEnabled
        ?"Load active gameplay with Booker.":"Progression grants disabled in InfiniteMod.ini.");
    ReleaseSRWLockExclusive(&stateLock);return ok;
}
bool requestVitals() {
    AcquireSRWLockExclusive(&stateLock);DWORD now=GetTickCount();
    bool ok=available && now-lastSeen<1500 && pending==-1 && !busy;
    if(ok){pending=-10;queuedAt=now;queuedController=latestController;queuedPawn=latestPawn;snprintf(message,sizeof(message),"Health, shield, and salts refill requested...");}
    else if(pending==-1 && !busy)snprintf(message,sizeof(message),"Load active gameplay with Booker.");
    ReleaseSRWLockExclusive(&stateLock);return ok;
}
void weaponStatus(char *text,size_t capacity) {
    AcquireSRWLockExclusive(&stateLock);
    if(pending!=-1 && GetTickCount()-queuedAt>2000){pending=-1;snprintf(message,sizeof(message),"Request expired: nothing added.");}
    snprintf(text,capacity,"%s",message);ReleaseSRWLockExclusive(&stateLock);
}
void playerOptionsStatus(char *text,size_t capacity) {
    LONG state=InterlockedCompareExchange(&player_options::optionState,0,0);
    snprintf(text,capacity,"%s",!player_options::speedInstalled || !player_options::fovInstalled
        ?"Settings unavailable: see the log.":state<0?"Unrecognized player: settings not applied."
        :state==0?"Settings saved. Load a game to apply them."
        :"Player settings active.");
}
LONG crosshairStatus(){return InterlockedCompareExchange(&reticle::state,0,0);}
LONG nativeAimZoomStatus(){return !player_options::aimTargetInstalled?-1:InterlockedCompareExchange(&player_options::aimState,0,0);}
LONG inputProfileStatus(){return !input_comfort::installed?-1:InterlockedCompareExchange(&input_comfort::statusCode,0,0);}
bool installWeapons(BYTE *image) {
    // Exact GOG 1.1.25.5165 signatures; no rendering code is detoured.
    const BYTE signature[]={0x55,0x8b,0xec,0x83,0xe4,0xf0,0x81,0xec,0xa4,0,0,0,0x53,0x56};
    if(memcmp(image+0x25030,signature,sizeof(signature)) || *(DWORD *)(image+0x11e5294)!=(DWORD)(image+0x5f6c00))return false;
    host=image;
    grantsEnabled=GetPrivateProfileIntW(L"Weapons",L"Enabled",1,modIni)!=0;
    progressionEnabled=GetPrivateProfileIntW(L"Progression",L"Enabled",1,modIni)!=0;
    MH_STATUS init=MH_Initialize();if(init!=MH_OK && init!=MH_ERROR_ALREADY_INITIALIZED)return false;
    MH_STATUS result=MH_CreateHook(image+0x25030,(void *)camera,(void **)&cameraOriginal);
    if(result==MH_OK)result=MH_EnableHook(image+0x25030);
    modLog("Weapons: camera command seam %s",MH_StatusToString(result));
    if(result==MH_OK){player_options::install();input_comfort::install();}
    return result==MH_OK;
}
