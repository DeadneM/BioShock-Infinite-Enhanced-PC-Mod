static void testWeapons() {
    host=(BYTE *)VirtualAlloc(nullptr,0x1400000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    BYTE entries[8][112]={};Obj names[8]={};
    const char *labels[]={"None","Class","Function","IntProperty","TestClass","TestCall","Amount","TestObject"};
    for(unsigned i=0;i<8;i++){names[i]=(Obj)entries[i];*(unsigned *)(entries[i]+8)=i<<1;strcpy((char *)entries[i]+16,labels[i]);}
    *(Obj *)(host+0x13ccef8)=(Obj)names;*(unsigned *)(host+0x13ccefc)=8;
    BYTE classClass[256]={},fnClass[256]={},intClass[256]={},cls[256]={},obj[256]={},fn[256]={},param[256]={};
    auto setup=[&](BYTE *p,unsigned name,BYTE *type){*(unsigned *)(p+0x18)=name;*(Obj *)(p+0x20)=(Obj)type;};
    setup(classClass,1,classClass);setup(fnClass,2,classClass);setup(intClass,3,classClass);setup(cls,4,classClass);
    setup(obj,7,cls);setup(fn,5,fnClass);setup(param,6,intClass);
    *(Obj *)(cls+0x38)=(Obj)fn;*(Obj *)(fn+0x38)=(Obj)param;
    fn[0x81]=1;*(uint16_t *)(fn+0x82)=4;*(uint16_t *)(fn+0x84)=0xffff;
    *(unsigned *)(param+0x2c)=1;*(unsigned *)(param+0x30)=4;*(unsigned *)(param+0x34)=0x80;
    Param expected[]={ {"Amount","IntProperty",0,4} };
    CHECK(function((Obj)obj,"TestCall",4,expected,1)==(Obj)fn);
    *(uint16_t *)(fn+0x84)=0;CHECK(!function((Obj)obj,"TestCall",4,expected,1));*(uint16_t *)(fn+0x84)=0xffff;
    *(unsigned *)(param+0x48)=4;CHECK(!function((Obj)obj,"TestCall",4,expected,1));*(unsigned *)(param+0x48)=0;
    fn[0x81]=2;CHECK(!function((Obj)obj,"TestCall",4,expected,1));fn[0x81]=1;
    *(uint16_t *)(fn+0x82)=8;CHECK(!function((Obj)obj,"TestCall",4,expected,1));
    CHECK(!classIs(1,"XHuman"));CHECK(!dispatch((Obj)obj,(Obj)fn,nullptr));
    CHECK(!requestWeapon(0));available=true;lastSeen=GetTickCount();CHECK(requestWeapon(2));CHECK(!requestWeapon(3));
    CHECK(pending==2);queuedAt=GetTickCount()-3000;char text[192];weaponStatus(text,sizeof(text));CHECK(pending==-1);
    CHECK(strstr(text,"expired"));CHECK(!requestWeapon(-1));CHECK(!requestWeapon(weaponChoiceCount));
    CHECK(!requestWeapon(0,2));CHECK(requestWeapon(0,1));CHECK(pendingDestination==1);CHECK(!requestAmmo());pending=-1;
    CHECK(requestAmmo());CHECK(pending==-2);CHECK(!requestWeapon(0));queuedAt=GetTickCount()-3000;weaponStatus(text,sizeof(text));CHECK(pending==-1);
    CHECK(!requestProgression((ProgressionAction)-1));CHECK(!requestProgression(ProgressionAction::Count));
    latestController=0x1234;latestPawn=0x5678;
    for(int i=0;i<(int)ProgressionAction::Count;i++) {
        CHECK(requestProgression((ProgressionAction)i));CHECK(pending==-3-i);
        CHECK(queuedController==latestController && queuedPawn==latestPawn);
        CHECK(!requestProgression(ProgressionAction::Money));CHECK(!requestAmmo());CHECK(!requestWeapon(0));
        queuedAt=GetTickCount()-3000;weaponStatus(text,sizeof(text));CHECK(pending==-1);
    }
    progressionEnabled=false;CHECK(!requestProgression(ProgressionAction::Money));progressionEnabled=true;
    busy=true;CHECK(!requestProgression(ProgressionAction::Gear));busy=false;
    lastSeen=GetTickCount()-2000;CHECK(!requestProgression(ProgressionAction::Powers));
    latestController=latestPawn=queuedController=queuedPawn=0;
    available=false;lastSeen=0;strcpy(message,"Load a game to add a weapon.");
    // Exercise the actual x86 MinHook trampoline and thiscall -> fastcall bridge
    // on synthetic code, never on a running game. Preserve both pointer arguments.
    const BYTE cameraCode[]={0x55,0x8b,0xec,0x83,0xe4,0xf0,0x81,0xec,0xa4,0,0,0,0x53,0x56,
        0xff,0x01,0x8b,0x45,0x08,0x89,0x41,0x04,0x8b,0x45,0x0c,0x89,0x41,0x08,
        0x5e,0x5b,0x8b,0xe5,0x5d,0xc2,0x08,0x00};
    memcpy(host+0x25030,cameraCode,sizeof(cameraCode));*(Obj *)(host+0x11e5294)=(Obj)host+0x5f6c00;
    FlushInstructionCache(GetCurrentProcess(),host+0x25030,sizeof(cameraCode));CHECK(installWeapons(host));
    Obj echo[3]={};CameraFn volatile call=(CameraFn)(host+0x25030);
    for(int i=0;i<100;i++)call(echo,(void *)0x1234,(void *)0x5678);
    CHECK(echo[0]==100 && echo[1]==0x1234 && echo[2]==0x5678);
    CHECK(MH_DisableHook(host+0x25030)==MH_OK);CHECK(MH_RemoveHook(host+0x25030)==MH_OK);CHECK(MH_Uninitialize()==MH_OK);
    VirtualFree(host,0,MEM_RELEASE);host=nullptr;cameraOriginal=nullptr;
    puts("PASS: weapon schema, corrupt pointers, signature drift, invalid dispatch, queue single-flight, expiry and bounds.");
}
