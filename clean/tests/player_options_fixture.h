static int recursiveAttributeDepth;
static float __fastcall recursiveAttributeFixture(void *value,void *) {
    if(recursiveAttributeDepth)return 1.5f;
    ++recursiveAttributeDepth;
    float native=player_options::attribute(value,nullptr);
    --recursiveAttributeDepth;return native;
}
static void testPlayerOptions() {
    using namespace player_options;
    CHECK(scaleValue(0,200)==0 && fabsf(scaleValue(1.2f,150)-1.8f)<0.00001f);
    CHECK(scaleValue(-1,200)==-1 && scaleValue(2,300)==2);
    host=(BYTE *)VirtualAlloc(nullptr,0x940000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    CHECK(MH_Initialize()==MH_OK);
    install();CHECK(!speedInstalled);
    memcpy(host+0x2cd110,attributeSignature,sizeof(attributeSignature));
    // The real prologue and branch, followed by a deterministic x87 float return.
    const BYTE tail[]={0xd9,0x47,0x04,0x5f,0x83,0xc4,0x10,0xc3};
    memcpy(host+0x2cd110+sizeof(attributeSignature),tail,sizeof(tail));
    const BYTE fovSignature[]={0x83,0xec,0x0c,0x0f,0x57,0xc0,0x53,0x55,0x56,0x57,0x33,0xdb,0x0f,0x28,0xc8,0x8b,0xe9};
    const BYTE cameraFovCall[]={0xe8,0x0f,0xc4,0x94,0xff};
    memcpy(host+0x285190,fovSignature,sizeof(fovSignature));
    // CalcFOV(pc, base) uses its real prologue and callee-cleaned float argument.
    // Fixture native result = base + pc.FOVOffset, reproducing the live 75 + 7.5.
    const BYTE fovTail[]={0xd9,0x44,0x24,0x20,0xd8,0x85,0x90,0x02,0,0,
        0x5f,0x5e,0x5d,0x5b,0x83,0xc4,0x0c,0xc2,0x04,0};
    memcpy(host+0x285190+sizeof(fovSignature),fovTail,sizeof(fovTail));
    const BYTE forwardArgument[]={0xff,0x74,0x24,0x04};
    memcpy(host+0x938d78,forwardArgument,sizeof(forwardArgument));
    memcpy(host+0x938d7c,cameraFovCall,sizeof(cameraFovCall));
    const BYTE returnArgument[]={0xc2,0x04,0};memcpy(host+0x938d81,returnArgument,sizeof(returnArgument));
    FlushInstructionCache(GetCurrentProcess(),host,0x940000);
    install();CHECK(speedInstalled);
    BYTE pc[0x800]={},pawn[0x4000]={},cam[0x400]={},other[40]={};
    checkedPC=(Obj)pc;checkedPawn=(Obj)pawn;schemaReady=true;windowThread=GetCurrentThreadId();
    *(Obj *)(pc+0x1fc)=(Obj)pawn;*(Obj *)(pc+0x240)=(Obj)cam;*(Obj *)(pawn+0x218)=(Obj)pc;
    *(Obj *)(cam+0x1fc)=(Obj)pc;*(float *)(pawn+0x2ac)=100;*(float *)(pawn+0x3ed0+4)=1.5f;*(float *)(pawn+0x3ef4+4)=2;
    *(float *)(other+4)=3;*(float *)(cam+0x214)=82.5f;*(float *)(pc+0x290)=7.5f;
    AttributeFn volatile getAttribute=(AttributeFn)(host+0x2cd110);
    using FovFn=float (__thiscall *)(void *,float);
    FovFn volatile getNativeFov=(FovFn)(host+0x285190),projectFov=(FovFn)(host+0x938d78);
    for(int percent:{100,125,150,175,200}) {
        speedPercent=percent;
        for(int i=0;i<100;i++)CHECK(getAttribute(pawn+0x3ed0)==1.5f*(percent/100.0f));
        CHECK(getAttribute(pawn+0x3ef4)==2*(percent/100.0f));CHECK(getAttribute(other)==3);
        CHECK(*(float *)(pawn+0x3ed0+4)==1.5f); // No persistent writes or accumulated factors.
    }
    fovDegrees=130;CHECK(getNativeFov(pc,75)==82.5f);
    for(int i=0;i<100;i++)CHECK(projectFov(pc,75)==82.5f);
    AttributeFn savedAttribute=attributeOriginal;
    attributeOriginal=(AttributeFn)recursiveAttributeFixture;
    CHECK(attribute(pawn+0x3ed0,nullptr)==3.0f); // Recursive native refresh scales once.
    attributeOriginal=savedAttribute;
    CHECK(*(float *)(cam+0x214)==82.5f);
    // The game owns its cache: even if it contains our last FOV, policy must use
    // the unmodified native calculation, otherwise release from ADS can stick.
    *(float *)(cam+0x214)=130;
    *(unsigned *)(pc+0x690)=0x20000;CHECK(projectFov(pc,32.5f)==40);
    *(unsigned *)(pc+0x690)=0;CHECK(projectFov(pc,42.5f)==50);CHECK(projectFov(pc,75)==82.5f);
    CHECK(*(float *)(pc+0x290)==7.5f && *(float *)(cam+0x214)==130);
    *(unsigned *)(pc+0x248)=0x20000;CHECK(projectFov(pc,75)==82.5f);CHECK(getAttribute(pawn+0x3ed0)==1.5f);
    *(unsigned *)(pc+0x248)=0;*(Obj *)(cam+0x1fc)=0;CHECK(projectFov(pc,75)==82.5f);*(Obj *)(cam+0x1fc)=(Obj)pc;
    CHECK(!memcmp(host+0x285190,fovSignature,sizeof(fovSignature)));
    *(unsigned *)(pc+0x248)=0;*(Obj *)(pc+0x1fc)=0;CHECK(projectFov(pc,75)==82.5f);CHECK(getAttribute(pawn+0x3ed0)==1.5f);
    CHECK(MH_DisableHook(host+0x2cd110)==MH_OK);CHECK(MH_RemoveHook(host+0x2cd110)==MH_OK);
    CHECK(MH_RemoveHook(host+0x285190)==MH_ERROR_NOT_CREATED);
    CHECK(host[0x108380]==0);
    CHECK(MH_Uninitialize()==MH_OK);VirtualFree(host,0,MEM_RELEASE);host=nullptr;
    attributeOriginal=nullptr;speedInstalled=schemaReady=false;
    checkedPC=checkedPawn=0;windowThread=0;speedPercent=100;fovDegrees=80;speedObserved=0;
    puts("PASS: speed x1..x2, native x87 float-argument ABI/trampolines, no persistent writes, local-player filtering, unhooked native CalcFOV and unchanged ADS/cache, cinematic and world changes.");
}
