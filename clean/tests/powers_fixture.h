// Synthetic native loader and ProcessEvent exercise for the real vigor grant.
// Every address belongs to this test process; no running game is inspected.
namespace powers_fixture {
struct Engine;
static Engine *current;
static void *__cdecl load(void *,void *,const wchar_t *,const wchar_t *,unsigned,void *,int);
static void __fastcall event(void *,void *,void *,void *,void *);

struct Engine {
    BYTE entries[128][112]={},objects[128][256]={};Obj names[128]={};
    unsigned nameCount=0,objectCount=0;
    BYTE pawn[0x400]={},manager[0x300]={};Obj vtable[32]={};
    BYTE *meta,*objectType,*byteType,*intType,*fnType,*pawnClass,*managerClass,*weaponClass;
    BYTE *inventoryProperty,*equipParameter,*acquire;
    Obj archetypes[8]={},instances[8]={},ordinary[2]={},fillers[33]={};
    int loads=0,calls=0,missing=-1,refuse=-1;unsigned acquired[8]={};
    unsigned id(const char *text) {
        for(unsigned i=0;i<nameCount;i++)if(!strcmp((char *)entries[i]+16,text))return i;
        CHECK(nameCount<128);unsigned i=nameCount++;names[i]=(Obj)entries[i];
        *(unsigned *)(entries[i]+8)=i<<1;strcpy((char *)entries[i]+16,text);
        *(unsigned *)(host+0x13ccefc)=nameCount;return i;
    }
    BYTE *object(const char *name,BYTE *type) {
        CHECK(objectCount<128);BYTE *p=objects[objectCount++];
        *(unsigned *)(p+0x18)=id(name);*(Obj *)(p+0x20)=(Obj)type;return p;
    }
    BYTE *property(BYTE *owner,const char *name,BYTE *type,unsigned offset,unsigned size,unsigned count=1) {
        BYTE *p=object(name,type);*(unsigned *)(p+0x2c)=count;*(unsigned *)(p+0x30)=size;
        *(unsigned *)(p+0x48)=offset;*(Obj *)(p+0x28)=*(Obj *)(owner+0x38);*(Obj *)(owner+0x38)=(Obj)p;return p;
    }
    void jump(unsigned offset,Obj target) {
        BYTE code[]={0xb8,0,0,0,0,0xff,0xe0};*(Obj *)(code+1)=target;
        memcpy(host+offset,code,sizeof(code));FlushInstructionCache(GetCurrentProcess(),host+offset,sizeof(code));
    }
    Engine() {
        current=this;*(Obj *)(host+0x13ccef8)=(Obj)names;id("None");
        meta=object("Class",nullptr);*(Obj *)(meta+0x20)=(Obj)meta;
        objectType=object("ObjectProperty",meta);byteType=object("ByteProperty",meta);
        intType=object("IntProperty",meta);fnType=object("Function",meta);
        pawnClass=object("XHuman",meta);managerClass=object("XInventoryManager",meta);weaponClass=object("XWeapon",meta);
        *(Obj *)(pawn+0x20)=(Obj)pawnClass;*(Obj *)(manager+0x20)=(Obj)managerClass;
        *(Obj *)(pawn+0x314)=(Obj)manager;*(Obj *)(manager+0x8c)=(Obj)pawn;
        *(Obj *)pawn=(Obj)vtable;vtable[0x7c/4]=(Obj)host+0x5f6c00;
        jump(0x5f6c00,(Obj)event);jump(0x1066f0,(Obj)load);
        inventoryProperty=property(managerClass,"InventoryItems",objectType,0x1fc,4,36);
        property(managerClass,"EquippedWeaponIndex",intType,0x2a0,4);
        property(managerClass,"BackupWeaponIndex",intType,0x2a4,4);
        acquire=object("AcquireWeapon",fnType);*(Obj *)(pawnClass+0x38)=(Obj)acquire;
        acquire[0x81]=2;*(uint16_t *)(acquire+0x82)=5;*(uint16_t *)(acquire+0x84)=0xffff;
        BYTE *arg=property(acquire,"WeaponArchetype",objectType,0,4);*(unsigned *)(arg+0x34)=0x80;
        equipParameter=property(acquire,"EquipWeaponItems",byteType,4,1);*(unsigned *)(equipParameter+0x34)=0x80;
        BYTE *package=object("PreCoalescedItemAssets",meta);
        BYTE *broncoClass=object("XWeaponRollingThunder",meta),*crowsClass=object("XWeaponMurderOfCrows",meta);
        *(Obj *)(broncoClass+0x34)=(Obj)weaponClass;*(Obj *)(crowsClass+0x34)=(Obj)weaponClass;
        for(unsigned i=0;i<8;i++) {
            BYTE *type=i==2?broncoClass:i==4?crowsClass:weaponClass;
            archetypes[i]=(Obj)object(vigorNames[i],type);*(Obj *)(archetypes[i]+0x14)=(Obj)package;
            instances[i]=(Obj)object("OwnedVigor",type);*(Obj *)(instances[i]+0x24)=archetypes[i];
        }
        for(unsigned i=0;i<2;i++)ordinary[i]=(Obj)object("OrdinaryWeapon",weaponClass);
        for(unsigned i=0;i<33;i++)fillers[i]=(Obj)object("OtherInventoryWeapon",weaponClass);
        reset(0);
    }
    Obj &slot(unsigned index){CHECK(index<36);return *(Obj *)(manager+0x1fc+index*4);}
    void reset(unsigned present) {
        memset(manager+0x1fc,0,36*4);slot(0)=fillers[0];slot(1)=ordinary[0];slot(2)=ordinary[1];
        *(int *)(manager+0x2a0)=1;*(int *)(manager+0x2a4)=2;
        unsigned next=3;for(unsigned i=0;i<8;i++)if(present&(1u<<i))slot(next++)=instances[i];
        loads=calls=0;missing=refuse=-1;memset(acquired,0,sizeof(acquired));
    }
    void selections() {
        CHECK(*(int *)(manager+0x2a0)==1 && *(int *)(manager+0x2a4)==2);
        CHECK(slot(0)==fillers[0] && slot(1)==ordinary[0] && slot(2)==ordinary[1]);
    }
    void allOwned() {
        for(unsigned i=0;i<8;i++) {
            unsigned found=0;for(unsigned s=1;s<36;s++)if(slot(s) && ptr(slot(s)+0x24)==archetypes[i])found++;
            CHECK(found==1);CHECK(progression_powers::ownedWeapon((Obj)manager,archetypes[i])==instances[i]);
        }
        selections();
    }
};
static void *__cdecl load(void *type,void *outer,const wchar_t *path,const wchar_t *file,unsigned flags,void *sandbox,int reconcile) {
    Engine &e=*current;
    CHECK(type==e.weaponClass && !outer && !file && flags==0x2002 && !sandbox && reconcile==1);
    int index=-1;for(unsigned i=0;i<8;i++) {
        wchar_t expected[128];swprintf(expected,128,L"PreCoalescedItemAssets.%hs",vigorNames[i]);
        if(!wcscmp(path,expected)){index=(int)i;break;}
    }
    CHECK(index>=0);++e.loads;return index==e.missing?nullptr:(void *)e.archetypes[index];
}
static void __fastcall event(void *object,void *,void *function,void *raw,void *result) {
    Engine &e=*current;CHECK(object==e.pawn && function==e.acquire && !result);
    struct Acquire {Obj archetype;unsigned char noEquip;unsigned char padding[3];};
    const Acquire &args=*(Acquire *)raw;CHECK(args.noEquip==1);
    CHECK(!args.padding[0] && !args.padding[1] && !args.padding[2]);
    int index=-1;for(unsigned i=0;i<8;i++)if(args.archetype==e.archetypes[i])index=(int)i;
    CHECK(index>=0 && !progression_powers::ownedWeapon((Obj)e.manager,args.archetype));++e.calls;++e.acquired[index];
    if(index==e.refuse)return;
    for(unsigned i=1;i<36;i++)if(!e.slot(i)){e.slot(i)=e.instances[index];return;}
    CHECK(false); // The command must reject inadequate capacity before dispatch.
}
}

static void testPowers() {
    using namespace powers_fixture;
    host=(BYTE *)VirtualAlloc(nullptr,0x1400000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    Engine e;BYTE pawnBefore[sizeof(e.pawn)];memcpy(pawnBefore,e.pawn,sizeof(e.pawn));
    auto grant=[&](){grantPowers(0,(Obj)e.pawn,(Obj)e.manager);CHECK(!memcmp(pawnBefore,e.pawn,sizeof(e.pawn)));e.selections();};
    CHECK(!classIs(e.archetypes[2],"XWeapon") && !classIs(e.archetypes[4],"XWeapon"));
    CHECK(progression_powers::weaponBase(e.archetypes[2])==(Obj)e.weaponClass);
    CHECK(progression_powers::weaponBase(e.archetypes[4])==(Obj)e.weaponClass);
    grant();CHECK(e.loads==8 && e.calls==8);e.allOwned();
    for(unsigned i=0;i<8;i++)CHECK(e.acquired[i]==1);
    BYTE before[sizeof(e.manager)];memcpy(before,e.manager,sizeof(before));
    grant();CHECK(e.loads==16 && e.calls==8 && !memcmp(before,e.manager,sizeof(before)));e.allOwned();
    CHECK(strstr(message,"deja possedes"));
    // Each archetype is independently found when owned and acquired when absent.
    for(unsigned missing=0;missing<8;missing++) {
        e.reset(255u^(1u<<missing));grant();CHECK(e.loads==8 && e.calls==1);e.allOwned();
        for(unsigned i=0;i<8;i++)CHECK(e.acquired[i]==(i==missing?1u:0u));
        grant();CHECK(e.calls==1);e.allOwned();
    }
    e.reset(0x55);grant();CHECK(e.calls==4);e.allOwned();
    for(unsigned i=0;i<8;i++)CHECK(e.acquired[i]==((0x55u&(1u<<i))?0u:1u));
    // Insufficient room includes a completely full inventory and seven free
    // slots: neither case may award even the first of eight missing vigors.
    for(unsigned free=0;free<=7;free+=7) {
        e.reset(0);for(unsigned i=3;i<36-free;i++)e.slot(i)=e.fillers[i-3];
        memcpy(before,e.manager,sizeof(before));grant();
        CHECK(e.loads==8 && !e.calls && !memcmp(before,e.manager,sizeof(before)));
        CHECK(strstr(message,"inventaire plein"));
    }
    // A missing final archetype proves that the whole catalogue is checked
    // before any native acquisition. Exercise every earlier failure as well.
    for(int missing=7;missing>=0;missing--) {
        e.reset(0);e.missing=missing;memcpy(before,e.manager,sizeof(before));grant();
        CHECK(e.loads==missing+1 && !e.calls && !memcmp(before,e.manager,sizeof(before)));
        CHECK(strstr(message,"catalogue incomplet"));
    }
    e.reset(0);*(unsigned *)(e.inventoryProperty+0x2c)=35;grant();CHECK(!e.loads && !e.calls);
    *(unsigned *)(e.inventoryProperty+0x2c)=36;
    *(unsigned *)(e.equipParameter+0x48)=0;grant();CHECK(!e.loads && !e.calls);
    *(unsigned *)(e.equipParameter+0x48)=4;
    // Refuse missing/cyclic/unrelated UClass chains without accepting arbitrary
    // same-named objects. Both real derived vigor classes are otherwise covered
    // by every missing/already-owned/capacity test above.
    Obj broncoClass=ptr(e.archetypes[2]+0x20);
    *(Obj *)(broncoClass+0x34)=0;e.reset(0);grant();CHECK(e.loads==3 && !e.calls);
    *(Obj *)(broncoClass+0x34)=broncoClass;e.reset(0);grant();CHECK(e.loads==3 && !e.calls);
    *(Obj *)(broncoClass+0x34)=(Obj)e.pawnClass;e.reset(0);grant();CHECK(e.loads==3 && !e.calls);
    *(Obj *)(broncoClass+0x34)=(Obj)e.weaponClass;
    // Native success must be confirmed by an inventory instance. A refused
    // acquisition stops immediately, then a retry adds only the remaining set.
    e.reset(0);e.refuse=4;grant();CHECK(e.calls==5);
    for(unsigned i=0;i<8;i++)CHECK(bool(progression_powers::ownedWeapon((Obj)e.manager,e.archetypes[i]))==(i<4));
    CHECK(strstr(message,"acquisition interrompue"));
    e.refuse=-1;grant();CHECK(e.calls==9);e.allOwned();
    current=nullptr;VirtualFree(host,0,MEM_RELEASE);host=nullptr;
    strcpy(message,"Chargez une partie pour ajouter une arme.");
    puts("PASS: all eight vigor loader paths and native ABI, real Bronco/Crows subclasses, invalid class chains, mixed/owned/missing grants, no-equip and selection preservation, no duplicates, capacity/catalogue preflight, schema rejection, interrupted acquisition and retry.");
}
