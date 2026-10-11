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
    FovPolicy policy;
    auto zoomMatches=[](float hip,float aimed,float nativeHip,float nativeAimed) {
        const double radians=3.14159265358979323846/360;
        return fabs(tan(hip*radians)/tan(aimed*radians)-tan(nativeHip*radians)/tan(nativeAimed*radians))<0.00001;
    };
    CHECK(policy.sample(1,2,82.5f,110,false,100)==110);
    CHECK(zoomMatches(110,policy.sample(1,2,40,110,true,200),82.5f,40));
    CHECK(zoomMatches(110,policy.sample(1,2,50,110,false,300),82.5f,50));
    CHECK(zoomMatches(110,policy.sample(1,2,70,110,false,400),82.5f,70));
    CHECK(policy.sample(1,2,82.5f,110,false,450)==110);
    CHECK(policy.sample(1,2,82.5f,130,false,500)==130);
    CHECK(policy.sample(1,2,82.5f,200,false,550)==82.5f);
    CHECK(policy.sample(3,4,40,100,true,600)==40);
    CHECK(policy.sample(3,4,75,100,false,700)==75);
    CHECK(policy.sample(3,4,75,100,false,1000)==100);
    // The requested hip angle may change while aiming. Preserve the authored
    // magnification (including scoped weapons) across every supported setting.
    for(int target:{80,100,110,120,130})for(float authored:{10.0f,25.0f,40.0f,55.0f,70.0f,82.5f}) {
        FovPolicy each;CHECK(each.sample(5,6,82.5f,target,false,100)==target);
        CHECK(zoomMatches((float)target,each.sample(5,6,authored,target,true,200),82.5f,authored));
    }
    FovPolicy dynamic;
    CHECK(dynamic.sample(1,2,82.5f,120,false,100)==120);
    CHECK(dynamic.sample(1,2,90,120,false,150)==120); // Native sprint/effect baseline follows live samples.
    CHECK(zoomMatches(120,dynamic.sample(1,2,40,120,true,200),90,40));
    CHECK(zoomMatches(120,dynamic.sample(1,2,82.5f,120,false,300),90,82.5f));
    CHECK(dynamic.sample(1,2,82.5f,120,false,600)==120); // Stable changed native hip recovers.
    CHECK(FovPolicy::relative(40,0,120)==40 && FovPolicy::relative(40,82.5f,200)==40);

    host=(BYTE *)VirtualAlloc(nullptr,0x940000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    CHECK(MH_Initialize()==MH_OK);
    install();CHECK(!speedInstalled && !fovInstalled);
    memcpy(host+0x2cd110,attributeSignature,sizeof(attributeSignature));
    // The real prologue and branch, followed by a deterministic x87 float return.
    const BYTE tail[]={0xd9,0x47,0x04,0x5f,0x83,0xc4,0x10,0xc3};
    memcpy(host+0x2cd110+sizeof(attributeSignature),tail,sizeof(tail));
    memcpy(host+0x285190,fovSignature,sizeof(fovSignature));
    memcpy(host+0x6ecdd0,advanceFovSignature,sizeof(advanceFovSignature));
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
    install();CHECK(speedInstalled && fovInstalled);
    BYTE pc[0x800]={},pawn[0x4000]={},cam[0x400]={},other[40]={};
    checkedPC=(Obj)pc;checkedPawn=(Obj)pawn;checkedCamera=(Obj)cam;schemaReady=cameraSchemaReady=lensSourceSchemaReady=true;windowThread=GetCurrentThreadId();
    *(Obj *)(pc+0x1fc)=(Obj)pawn;*(Obj *)(pc+0x240)=(Obj)cam;*(Obj *)(pawn+0x218)=(Obj)pc;
    *(Obj *)(cam+0x1fc)=(Obj)pc;
    *(float *)(pawn+0x2ac)=100;*(float *)(pawn+0x3ed0+4)=1.5f;*(float *)(pawn+0x3ef4+4)=2;
    *(float *)(other+4)=3;*(float *)(cam+0x214)=82.5f;*(float *)(pc+0x290)=7.5f;
    AttributeFn volatile getAttribute=(AttributeFn)(host+0x2cd110);
    FovFn volatile getNativeFov=(FovFn)(host+0x285190),projectFov=(FovFn)(host+0x938d78);
    AdvanceFovFn volatile getAdvanceFov=(AdvanceFovFn)(host+0x6ecdd0);
    for(int percent:{100,125,150,175,200}) {
        speedPercent=percent;
        for(int i=0;i<100;i++)CHECK(getAttribute(pawn+0x3ed0)==1.5f*(percent/100.0f));
        CHECK(getAttribute(pawn+0x3ef4)==2*(percent/100.0f));CHECK(getAttribute(other)==3);
        CHECK(*(float *)(pawn+0x3ed0+4)==1.5f); // No persistent writes or accumulated factors.
    }
    fovDegrees=130;CHECK(getNativeFov(pc,75)==82.5f);
    for(int i=0;i<100;i++){CHECK(projectFov(pc,75)==82.5f);CHECK(lens.valid&&lens.world==130&&lens.native==82.5f);}
    AttributeFn savedAttribute=attributeOriginal;
    attributeOriginal=(AttributeFn)recursiveAttributeFixture;
    CHECK(attribute(pawn+0x3ed0,nullptr)==3.0f); // Recursive native refresh scales once.
    attributeOriginal=savedAttribute;
    CHECK(*(float *)(cam+0x214)==82.5f);
    // The requested lens is sidecar data; native callers retain their exact
    // result, even if a stale cache contains a former requested angle.
    *(float *)(cam+0x214)=130;
    *(unsigned *)(pc+0x690)=0x20000;CHECK(projectFov(pc,32.5f)==40);CHECK(zoomMatches(130,lens.world,82.5f,40));
    CHECK(lens.valid && lens.corrected && lens.native==40);
    *(unsigned *)(pc+0x690)=0;CHECK(projectFov(pc,42.5f)==50);CHECK(zoomMatches(130,lens.world,82.5f,50));CHECK(projectFov(pc,75)==82.5f&&lens.world==130);
    CHECK(*(float *)(pc+0x290)==7.5f && *(float *)(cam+0x214)==130);
    // Native AdvanceFOV must still copy its own authored source, then expose a
    // paired return value for the renderer. Locked ADS bypasses the POV cache.
    *(float *)(cam+0x3d0)=82.5f;*(unsigned *)(cam+0x20c)=0;
    getAdvanceFov(cam);CHECK(*(float *)(cam+0x214)==82.5f);
    RenderLens projected=projectionLens((Obj)pc,82.5f);
    CHECK(projected.valid && projected.corrected && projected.native==82.5f && projected.world==130);
    *(float *)(cam+0x210)=40;*(unsigned *)(cam+0x20c)=1;
    getAdvanceFov(cam);CHECK(*(float *)(cam+0x214)==40 && *(float *)(cam+0x210)==40 && *(float *)(cam+0x3d0)==82.5f);
    float preserved=0,marker=123.25f;void *advanceCamera=cam;
    // Native AdvanceFOV has no SSE side effects; a caller may retain a live
    // zero/vector register even though normal x86 ABI marks it volatile.
    __asm__ volatile("movss %2,%%xmm2\n\tcall *%3\n\tmovss %%xmm2,%0"
        : "=m"(preserved),"+c"(advanceCamera) : "m"(marker),"r"(getAdvanceFov)
        : "eax","edx","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7","memory","cc");
    CHECK(preserved==marker);
    projected=projectionLens((Obj)pc,40);
    CHECK(projected.valid && projected.corrected && projected.native==40 && zoomMatches(130,projected.world,82.5f,40));
    // Entry/release flags can disagree with the delayed lens; copied-source
    // provenance wins. Repeated reads do not compound the tangent factor.
    *(unsigned *)(pc+0x690)=0;
    CHECK(projectionLens((Obj)pc,40).world==projected.world);
    for(int i=0;i<100;++i)CHECK(projectionLens((Obj)pc,40).world==projected.world);
    CHECK(!projectionLens((Obj)pc,130).valid);
    *(unsigned *)(cam+0x20c)=0;getAdvanceFov(cam);
    CHECK(projectionLens((Obj)pc,82.5f).world==130 && *(float *)(cam+0x214)==82.5f);
    delayedLens.sampledAt=GetTickCount()-251;CHECK(!projectionLens((Obj)pc,82.5f).valid);
    getAdvanceFov(cam);lensSourceSchemaReady=false;CHECK(!projectionLens((Obj)pc,82.5f).valid);lensSourceSchemaReady=true;
    // Invalid or missing metadata never guesses a lens already modified by us.
    lens.valid=false;getAdvanceFov(cam);CHECK(!projectionLens((Obj)pc,82.5f).valid);
    CHECK(projectFov(pc,75)==82.5f);getAdvanceFov(cam);
    // A cached requested value is never mistaken for this native source.
    *(float *)(cam+0x3d0)=130;getAdvanceFov(cam);CHECK(!projectionLens((Obj)pc,130).valid);
    *(float *)(cam+0x3d0)=82.5f;getAdvanceFov(cam);CHECK(projectionLens((Obj)pc,82.5f).world==130);
    *(unsigned *)(pc+0x248)=0x20000;CHECK(projectFov(pc,75)==82.5f);CHECK(getAttribute(pawn+0x3ed0)==1.5f);
    *(unsigned *)(pc+0x248)=0;*(Obj *)(cam+0x1fc)=0;CHECK(projectFov(pc,75)==82.5f);*(Obj *)(cam+0x1fc)=(Obj)pc;
    cameraSchemaReady=false;CHECK(projectFov(pc,75)==82.5f);cameraSchemaReady=true;
    *(unsigned *)(pc+0x248)=0;*(Obj *)(pc+0x1fc)=0;CHECK(projectFov(pc,75)==82.5f);CHECK(getAttribute(pawn+0x3ed0)==1.5f);
    CHECK(MH_DisableHook(host+0x2cd110)==MH_OK);CHECK(MH_RemoveHook(host+0x2cd110)==MH_OK);
    CHECK(MH_DisableHook(host+0x285190)==MH_OK);CHECK(MH_RemoveHook(host+0x285190)==MH_OK);
    CHECK(MH_DisableHook(host+0x6ecdd0)==MH_OK);CHECK(MH_RemoveHook(host+0x6ecdd0)==MH_OK);
    CHECK(MH_Uninitialize()==MH_OK);VirtualFree(host,0,MEM_RELEASE);host=nullptr;
    attributeOriginal=nullptr;fovOriginal=nullptr;advanceFovOriginal=nullptr;speedInstalled=fovInstalled=schemaReady=cameraSchemaReady=lensSourceSchemaReady=false;
    lens={};delayedLens={};
    checkedPC=checkedPawn=checkedCamera=0;cameraObserved=0;windowThread=0;speedPercent=100;fovDegrees=80;fovPolicy={};speedObserved=fovObserved=0;
    puts("PASS: speed x1..x2, native x87/thiscall returns unchanged, requested world lens sidecar, proportional ADS and recovery, native delayed LockedFOV provenance, stale corrected cache refused, no double application or persistent writes, local/cinematic/schema/world guards.");
}
