namespace position_fixture {
static float seen;static unsigned calls,depth;static bool recurse,alter;
static Obj expected;
static void __cdecl placement(void *object,void *location,void *rotation){
    CHECK((Obj)object==expected);++calls;seen=*(float *)((BYTE *)object+0x200);
    CHECK(location && rotation);
    ((float *)location)[0]=100+seen;((float *)location)[1]=200;((float *)location)[2]=300;
    if(recurse && !depth){++depth;firstperson_position::place(object,nullptr,location,rotation);--depth;}
    if(alter)*(float *)((BYTE *)object+0x200)=77;
}
}
static void testFirstpersonPosition(){
    using resource_fixture::Engine;using namespace position_fixture;
    host=(BYTE *)VirtualAlloc(nullptr,0x1400000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    CHECK(MH_Initialize()==MH_OK);
    BYTE *code=host+0x77c850;
    memcpy(code,firstperson_position::signature,sizeof(firstperson_position::signature));
    // Exact native aligned-frame prefix, followed by a deterministic receipt.
    const BYTE receipt[]={0x53,0xff,0x75,0x08,0x51,0xb8};
    memcpy(code+16,receipt,sizeof(receipt));*(Obj *)(code+22)=(Obj)position_fixture::placement;
    const BYTE tail[]={0xff,0xd0,0x83,0xc4,0x0c,0x5b,0x8b,0xe5,0x5d,0xc2,0x08,0};memcpy(code+26,tail,sizeof(tail));
    // Refuse either entry or offset-consumer drift before creating a hook.
    firstperson_position::install();CHECK(!firstperson_position::installed);
    memcpy(host+0x77ca68,firstperson_position::offsetXSignature,sizeof(firstperson_position::offsetXSignature));
    code[0]=0x90;firstperson_position::install();CHECK(!firstperson_position::installed);code[0]=0x55;
    firstperson_position::install();CHECK(firstperson_position::installed);FlushInstructionCache(GetCurrentProcess(),host,0x1400000);
    Engine e;BYTE pc[0x800]={},pawn[0x2100]={},root[0x400]={},other[0x400]={};Obj vt[0x300/4]={};
    auto *objectType=e.object("ObjectProperty",e.meta),*rootClass=e.object("XFirstPersonAttachment",e.meta),*vector=e.object("Vector",e.scriptType);
    e.property(e.pawnClass,"FirstPersonAttachment",objectType,0x1d38,4);
    e.property(rootClass,"Instigator",objectType,0xa0,4);
    auto *view=e.property(rootClass,"PlayerViewOffset",e.structType,0x200,12);*(Obj *)(view+0x58)=(Obj)vector;
    *(Obj *)(root+0x20)=(Obj)rootClass;*(Obj *)root=(Obj)vt;vt[0x2c8/4]=(Obj)host+0x77c850;
    *(Obj *)(pawn+0x20)=(Obj)e.pawnClass;*(Obj *)(pc+0x1fc)=(Obj)pawn;*(Obj *)(pawn+0x218)=(Obj)pc;
    *(Obj *)(pawn+0x1d38)=(Obj)root;*(Obj *)(root+0xa0)=(Obj)pawn;*(float *)(pawn+0x2ac)=100;
    *(float *)(root+0x200)=3;*(float *)(root+0x204)=4;*(float *)(root+0x208)=5;
    player_options::checkedPC=(Obj)pc;player_options::checkedPawn=(Obj)pawn;player_options::schemaReady=true;
    windowThread=GetCurrentThreadId();player_options::fovPolicy={};player_options::checkedCamera=0;
    firstperson_position::inspect((Obj)pc,(Obj)pawn,true);CHECK(firstperson_position::ready);
    firstperson_position::PlaceFn volatile call=(firstperson_position::PlaceFn)code;
    float pos[3]={};int rot[3]={};expected=(Obj)root;calls=depth=0;recurse=alter=false;
    for(int back:{0,2,4,6,8,10}){
        weaponBack=back;call(root,pos,rot);CHECK(seen==3-back && pos[0]==103-back);
        CHECK(*(float *)(root+0x200)==3 && *(float *)(root+0x204)==4 && *(float *)(root+0x208)==5);
    }
    weaponBack=4;recurse=true;calls=0;call(root,pos,rot);CHECK(calls==2 && seen==-1 && *(float *)(root+0x200)==3);recurse=false;
    // ADS/transition/cinematic/death/thread/ownership gates retain native input.
    *(unsigned *)(pc+0x690)=0x20000;call(root,pos,rot);CHECK(seen==3);*(unsigned *)(pc+0x690)=0;
    player_options::fovPolicy.zooming=true;call(root,pos,rot);CHECK(seen==3);player_options::fovPolicy.zooming=false;
    *(unsigned *)(pc+0x248)=0x20000;call(root,pos,rot);CHECK(seen==3);*(unsigned *)(pc+0x248)=0;
    *(float *)(pawn+0x2ac)=0;call(root,pos,rot);CHECK(seen==3);*(float *)(pawn+0x2ac)=100;
    windowThread=0;call(root,pos,rot);CHECK(seen==3);windowThread=GetCurrentThreadId();
    *(Obj *)(pawn+0x1d38)=(Obj)other;call(root,pos,rot);CHECK(seen==3);*(Obj *)(pawn+0x1d38)=(Obj)root;
    *(Obj *)(root+0xa0)=0;call(root,pos,rot);CHECK(seen==3);*(Obj *)(root+0xa0)=(Obj)pawn;
    for(int invalid:{-2,1,12,100}){weaponBack=invalid;call(root,pos,rot);CHECK(seen==3);}weaponBack=4;
    alter=true;call(root,pos,rot);CHECK(*(float *)(root+0x200)==77);alter=false;*(float *)(root+0x200)=3;
    *(float *)(root+0x204)=__builtin_nanf("");call(root,pos,rot);CHECK(seen==3);*(float *)(root+0x204)=4;
    *(unsigned *)(view+0x48)=0x204;firstperson_position::ready=false;firstperson_position::inspect((Obj)pc,(Obj)pawn,true);CHECK(!firstperson_position::ready);
    CHECK(MH_DisableHook(code)==MH_OK);CHECK(MH_RemoveHook(code)==MH_OK);CHECK(MH_Uninitialize()==MH_OK);
    firstperson_position::installed=firstperson_position::ready=false;firstperson_position::controller=firstperson_position::pawn=firstperson_position::root=0;
    firstperson_position::original=nullptr;firstperson_position::state=0;weaponBack=0;
    player_options::schemaReady=false;player_options::checkedPC=player_options::checkedPawn=0;windowThread=0;
    VirtualFree(host,0,MEM_RELEASE);host=nullptr;
    // Real profile API: strict bounded values and no state update on failed save.
    wchar_t cwd[MAX_PATH],path[MAX_PATH];CHECK(GetCurrentDirectoryW(MAX_PATH,cwd));swprintf(path,MAX_PATH,L"%ls\\position-%lu.ini",cwd,GetCurrentProcessId());
    CHECK(firstperson_setting::load(path)==0);
    CHECK(WritePrivateProfileStringW(L"Other",L"Keep",L"unchanged",path));
    for(int back:{0,2,4,6,8,10}){CHECK(firstperson_setting::save(path,weaponBack,back));CHECK(firstperson_setting::load(path)==back && weaponBack==back);}
    CHECK(!firstperson_setting::save(L"",weaponBack,2) && weaponBack==10);
    for(const wchar_t *invalid:{L"-2",L"3",L"12",L"4junk",L"999999999999999999999999999"}){
        CHECK(WritePrivateProfileStringW(L"FirstPerson",L"BackOffset",invalid,path));CHECK(firstperson_setting::load(path)==0);
    }
    wchar_t value[32];GetPrivateProfileStringW(L"Other",L"Keep",L"",value,32,path);CHECK(!wcscmp(value,L"unchanged"));
    CHECK(DeleteFileW(path));weaponBack=0;
    puts("PASS: native aligned ret8 placement trampoline, signature drift refusal, reflected root/Vector identity, bounded toward-camera offset, no accumulation/reentrant doubling, ADS/cinematic/death/thread/owner guards, native-change-preserving restore and real INI persistence. Game coverage unverified.");
}
