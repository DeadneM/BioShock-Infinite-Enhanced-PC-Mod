namespace foreground_fixture {
using foreground_math::Matrix;using foreground_math::Fields;
static Matrix identityMatrix(){Matrix r={};for(int i=0;i<4;i++)r.v[i][i]=1;return r;}
static Matrix projectionMatrix(double fov,double aspect){
    Matrix r={};double x=1/tan(fov*3.14159265358979323846/360.0);
    r.v[0][0]=(float)x;r.v[1][1]=(float)(x*aspect);r.v[2][2]=1;r.v[2][3]=1;r.v[3][2]=-2;return r;
}
static bool closeNumber(double a,double b,double eps=0.0002){return fabs(a-b)<eps;}
static int ctorCalls,copyCalls,passCalls,rhiCalls;static DWORD observedGroup,observedOption,observedFlag;
static Matrix observedP,observedRhi;static Obj observedDirty;
static float observedScale;
static Obj __fastcall mockCtor(void *self,void *,viewmodel::CtorArgs args){
    ++ctorCalls;BYTE *out=(BYTE *)self;
    memset(out,0,0x6d0);*(Obj *)(out+4)=args.values[0];*(Obj *)(out+8)=args.values[1];*(Obj *)(out+0x14)=args.values[4];
    Matrix v=identityMatrix();v.v[3][0]=4;v.v[3][1]=-6;Matrix p=projectionMatrix(120,16.0/9);
    Fields base={};CHECK(foreground_math::adapt(v,identityMatrix(),Fields{p,{},{},{},{},{},{}},1,base));
    memcpy(out+0x70,&v,64);Matrix tv=identityMatrix();memcpy(out+0x170,&tv,64);
    viewmodel::writeFields((Obj)out,base);*(float *)(out+0x500)=120.0f/75;
    return (Obj)self;
}
static Obj __fastcall mockCopy(void *self,void *,const void *source){
    ++copyCalls;memcpy(self,source,0x6d0);return (Obj)self;
}
static void __fastcall mockPass(void *renderer,void *,DWORD group,DWORD option,void *dirty,DWORD flag){
    ++passCalls;observedGroup=group;observedOption=option;observedFlag=flag;observedDirty=(Obj)dirty;
    Obj view=*(Obj *)((BYTE *)renderer+0x50);memcpy(&observedP,(void *)(view+0xb0),64);
    observedScale=*(float *)(view+0x500);*(int *)dirty|=4;
    // Emulate arbitrary shader work clobbering SSE, as allowed by this complex routine.
    __asm__ volatile("pxor %%xmm2,%%xmm2" : : : "xmm2");
}
static void __fastcall mockRhi(void *,void *,void *,const Matrix *vp,const void *){
    ++rhiCalls;observedRhi=*vp;__asm__ volatile("pcmpeqd %%xmm2,%%xmm2" : : : "xmm2");
}
static void jump(BYTE *at,Obj fn){at[0]=0xb8;*(Obj *)(at+1)=fn;at[5]=0xff;at[6]=0xe0;}
static void nativeMocks(){
    const BYTE align[]={0x55,0x8b,0xec,0x83,0xe4,0xf0,0x8b,0xe5,0x5d};
    memcpy(host+0x8d6470,align,sizeof(align));jump(host+0x8d6470+sizeof(align),(Obj)mockCtor);
    BYTE copy[]={0x6a,0xff,0x68,0,0,0,0,0x64,0xa1,0,0,0,0,0x50,0x51,0x53,0x83,0xc4,0x14};
    *(Obj *)(copy+3)=(Obj)host+0xcfb575;memcpy(host+0x8758b0,copy,sizeof(copy));jump(host+0x8758b0+sizeof(copy),(Obj)mockCopy);
    BYTE pass[]={0x6a,0xff,0x68,0,0,0,0,0x64,0xa1,0,0,0,0,0x50,0x83,0xec,0x34,0x53,0x83,0xc4,0x44};
    *(Obj *)(pass+3)=(Obj)host+0xcfc1b0;memcpy(host+0x47c020,pass,sizeof(pass));jump(host+0x47c020+sizeof(pass),(Obj)mockPass);
    const BYTE proj[]={0x8b,0x81,0x40,0x02,0,0,0x85,0xc0,0x74,0x07,0xd9,0x80,0x14,0x02,0,0,0xc3};
    memcpy(host+0x646ad0,proj,sizeof(proj));
    const BYTE pcCall[]={0xe8,0xe1,0xe7,0x1c,0,0xc3};memcpy(host+0x4782ea,pcCall,sizeof(pcCall));
    // The caller copies all 96 native stack bytes and restores ESI/EDI.
    const BYTE ctorCaller[]={0x56,0x57,0x81,0xec,0x60,0,0,0,0x8d,0x74,0x24,0x6c,0x8b,0xfc,
        0x8b,0xc1,0xb9,0x18,0,0,0,0xf3,0xa5,0x8b,0xc8,0xe8,0xf2,0xd5,0x45,0,0x5f,0x5e,0xc2,0x60,0};
    memcpy(host+0x478e60,ctorCaller,sizeof(ctorCaller));
    BYTE copyCaller[]={0xff,0x74,0x24,0x04,0xe8,0,0,0,0,0xc2,0x04,0};
    *(int32_t *)(copyCaller+5)=(int32_t)(0x8758b0-(0x8ebfa6+5));memcpy(host+0x8ebfa2,copyCaller,sizeof(copyCaller));
    // Four arguments copied in reverse order while ESP changes.
    BYTE passCaller[]={0xff,0x74,0x24,0x10,0xff,0x74,0x24,0x10,0xff,0x74,0x24,0x10,0xff,0x74,0x24,0x10,
        0xe8,0x45,0x4f,0x19,0,0xc2,0x10,0};
    memcpy(host+0x2e70c6,passCaller,sizeof(passCaller));
    jump(host+0x90f6b0,(Obj)mockRhi);FlushInstructionCache(GetCurrentProcess(),host,0x1642000);
}
static void maths(){
    for(double aspect:{16.0/9,21.0/9,4.0/3})for(double native:{10.0,40.0,82.5})for(double hip:{80.0,110.0,120.0,130.0}){
        double radians=3.14159265358979323846/360,ratio=tan(hip*radians)/tan(82.5*radians);
        double world=atan(tan(native*radians)*ratio)/radians;
        Matrix v=identityMatrix(),tv=identityMatrix();v.v[3][0]=-31000;v.v[3][1]=26000;
        Fields raw={};raw.projection=projectionMatrix(world,aspect);raw.projection.v[2][0]=0.001f;raw.projection.v[2][1]=-0.002f;
        Fields out;CHECK(foreground_math::adapt(v,tv,raw,ratio,out));
        Matrix expected=projectionMatrix(native,aspect);
        CHECK(closeNumber(out.projection.v[0][0],expected.v[0][0])&&closeNumber(out.projection.v[1][1],expected.v[1][1]));
        CHECK(out.projection.v[2][0]==raw.projection.v[2][0]&&out.projection.v[2][1]==raw.projection.v[2][1]);
        CHECK(out.projection.v[3][2]==raw.projection.v[3][2]&&out.projection.v[2][2]==raw.projection.v[2][2]);
        Matrix product=foreground_math::multiply(out.projection,out.inverseProjection);
        for(int i=0;i<4;i++)for(int j=0;j<4;j++)CHECK(closeNumber(product.v[i][j],i==j));
        // Inverting a full translated VP preserves the camera instead of scaling world actors.
        product=foreground_math::multiply(out.VP,out.inverseVP);
        for(int i=0;i<4;i++)for(int j=0;j<4;j++)CHECK(closeNumber(product.v[i][j],i==j,0.004));
    }
    Fields raw={},out;raw.projection=projectionMatrix(120,16.0/9);
    CHECK(foreground_math::adapt(identityMatrix(),identityMatrix(),raw,1,out));
    auto saved=out;raw.projection.v[0][0]=__builtin_nanf("");CHECK(!foreground_math::adapt(identityMatrix(),identityMatrix(),raw,2,out));CHECK(!memcmp(&saved,&out,sizeof(out)));
}
static void adapter(){
    BYTE source[0x6d0]={},view[0x11a0]={},renderer[0x100]={},other[0x11a0]={},rhi[0x10]={};Obj rv[0x200/4]={};
    *(Obj *)rhi=(Obj)rv;rv[0x1ac/4]=(Obj)host+0x90f6b0;*(Obj *)(host+0x13d124c)=(Obj)rhi;
    windowThread=GetCurrentThreadId();viewmodel::pending={0x1234,0x5678,82.5f,120,GetTickCount(),true};
    viewmodel::CtorArgs args={};args.values[0]=0x4321;args.values[1]=0x5678;args.values[4]=0x1234;
    auto ctor=(viewmodel::CtorFn)(host+0x478e60);CHECK(ctor(source,args)==(Obj)source);
    viewmodel::Record rec;CHECK(viewmodel::lookup((Obj)source,rec));CHECK(rec.native==82.5f&&rec.world==120);
    auto copy=(viewmodel::CopyFn)(host+0x8ebfa2);CHECK(copy(view,source)==(Obj)view);CHECK(viewmodel::lookup((Obj)view,rec));
    *(Obj *)(renderer+0x50)=(Obj)view;*(int *)(renderer+0x54)=*(int *)(renderer+0x58)=1;
    *(Obj *)(renderer+4)=(Obj)view;*(Obj *)(renderer+8)=0;*(int *)(renderer+12)=1;*(Obj *)(view+4)=(Obj)renderer+4;
    BYTE before[sizeof(view)];memcpy(before,view,sizeof(view));int dirty=0;passCalls=rhiCalls=0;
    auto pass=(viewmodel::PassFn)(host+0x2e70c6);
    pass(renderer,1,55,&dirty,99);CHECK(passCalls==1&&rhiCalls==0&&observedGroup==1&&observedOption==55&&observedFlag==99&&dirty==4);
    CHECK(!memcmp(&observedP,view+0xb0,64)&&!memcmp(before,view,sizeof(view)));
    pass(renderer,2,88,&dirty,77);CHECK(passCalls==2&&rhiCalls==1&&observedDirty==(Obj)&dirty);
    Matrix expected=projectionMatrix(82.5,16.0/9);CHECK(closeNumber(observedP.v[0][0],expected.v[0][0]));
    CHECK(closeNumber(observedScale,82.5/75));CHECK(!memcmp(before,view,sizeof(view)));
    CHECK(!memcmp(&observedRhi,view+0x1b0,64)); // GPU constants rebound after CPU restoration.
    // The complex native pass may change SSE, but the added RHI restoration
    // must not replace its post-call register state. mockPass leaves XMM2 zero;
    // mockRhi deliberately writes all ones, so observe the actual wrapper exit.
    float sseSeed[]={1.25f,-2.5f,3.75f,4.125f},sseObserved[4]={},sseZero[4]={};
    BYTE ssePass[]={0x0f,0x10,0x15,0,0,0,0,
        0xff,0x74,0x24,0x10,0xff,0x74,0x24,0x10,0xff,0x74,0x24,0x10,0xff,0x74,0x24,0x10,
        0xe8,0,0,0,0,0x0f,0x11,0x15,0,0,0,0,0xc2,0x10,0};
    *(Obj *)(ssePass+3)=(Obj)sseSeed;
    *(int32_t *)(ssePass+24)=(int32_t)(0x2e70c6-(0x160020+28));
    *(Obj *)(ssePass+31)=(Obj)sseObserved;
    memcpy(host+0x160020,ssePass,sizeof(ssePass));FlushInstructionCache(GetCurrentProcess(),host+0x160020,sizeof(ssePass));
    ((viewmodel::PassFn)(host+0x160020))(renderer,2,88,&dirty,77);
    CHECK(!memcmp(sseObserved,sseZero,sizeof(sseZero))&&!memcmp(before,view,sizeof(view)));
    for(int i=0;i<1000;i++)pass(renderer,2,88,&dirty,77);CHECK(!memcmp(before,view,sizeof(view)));
    int prior=rhiCalls;((viewmodel::PassFn)(host+0x47c020))(renderer,2,88,&dirty,77);
    CHECK(rhiCalls==prior&&!memcmp(&observedP,view+0xb0,64)); // Sole callsite restriction.
    *(Obj *)(view+0x14)=0x9999;pass(renderer,2,88,&dirty,77);CHECK(rhiCalls==prior);*(Obj *)(view+0x14)=0x1234;
    *(int *)(renderer+0x54)=2;pass(renderer,2,88,&dirty,77);CHECK(rhiCalls==prior);*(int *)(renderer+0x54)=1;
    *(Obj *)(renderer+4)=(Obj)other;pass(renderer,2,88,&dirty,77);CHECK(rhiCalls==prior);*(Obj *)(renderer+4)=(Obj)view;
    *(Obj *)(view+8)=0x5555;pass(renderer,2,88,&dirty,77);CHECK(rhiCalls==prior);*(Obj *)(view+8)=0x5678;
    rv[0x1ac/4]=0;pass(renderer,2,88,&dirty,77);CHECK(rhiCalls==prior&&!memcmp(before,view,sizeof(view)));rv[0x1ac/4]=(Obj)host+0x90f6b0;
    // All untouched fields, near depth, world-pass matrices and actor/component state survive.
    CHECK(!memcmp(before,view,sizeof(view)));windowThread=0;
}
static void originalCameraRendererBridge(){
    BYTE pc[0x800]={},pawn[0x4000]={},camera[0x400]={};
    windowThread=GetCurrentThreadId();
    *(Obj *)(pc+0x1fc)=(Obj)pawn;*(Obj *)(pc+0x240)=(Obj)camera;
    *(Obj *)(pawn+0x218)=(Obj)pc;*(float *)(pawn+0x2ac)=100;
    *(Obj *)(camera+0x1fc)=(Obj)pc;*(float *)(camera+0x214)=82.5f;*(float *)(camera+0x3d0)=82.5f;
    player_options::checkedPC=(Obj)pc;player_options::checkedPawn=(Obj)pawn;player_options::checkedCamera=(Obj)camera;
    player_options::schemaReady=player_options::cameraSchemaReady=player_options::lensSourceSchemaReady=true;
    player_options::lens={(Obj)pc,(Obj)pawn,(Obj)camera,82.5f,120,GetTickCount(),true,true};
    player_options::recordDelayedLens((Obj)camera);
    viewmodel::identity={(Obj)pc,(Obj)pawn,0,0x5678,true};
    auto rendererGetter=(viewmodel::ProjectionFn)(host+0x4782ea);
    auto ordinaryGetter=(viewmodel::ProjectionFn)(host+0x646ad0);
    for(int i=0;i<100;i++){
        CHECK(rendererGetter(pc)==120);CHECK(ordinaryGetter(pc)==82.5f);
        CHECK(viewmodel::pending.valid&&viewmodel::pending.native==82.5f&&viewmodel::pending.world==120);
        CHECK(*(float *)(camera+0x214)==82.5f&&*(float *)(camera+0x3d0)==82.5f);
    }
    *(Obj *)(camera+0x1fc)=0;CHECK(rendererGetter(pc)==82.5f&&!viewmodel::pending.valid);
    player_options::lens={};player_options::delayedLens={};
    player_options::checkedPC=player_options::checkedPawn=player_options::checkedCamera=0;
    player_options::schemaReady=player_options::cameraSchemaReady=player_options::lensSourceSchemaReady=false;
    viewmodel::identity={};viewmodel::pending={};windowThread=0;
}
}
static void testViewmodel(){
    using namespace foreground_fixture;maths();
    host=(BYTE *)VirtualAlloc(nullptr,0x1642000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    CHECK(MH_Initialize()==MH_OK);player_options::fovInstalled=true;
    viewmodel::install();CHECK(!viewmodel::installed);nativeMocks();viewmodel::install();CHECK(viewmodel::installed);
    adapter();originalCameraRendererBridge();
    for(unsigned rva:{0x646ad0,0x8d6470,0x8758b0,0x47c020}){CHECK(MH_DisableHook(host+rva)==MH_OK);CHECK(MH_RemoveHook(host+rva)==MH_OK);}
    CHECK(MH_Uninitialize()==MH_OK);viewmodel::installed=false;viewmodel::state=0;player_options::fovInstalled=false;
    viewmodel::pending={};viewmodel::identity={};memset(viewmodel::records,0,sizeof(viewmodel::records));
    VirtualFree(host,0,MEM_RELEASE);host=nullptr;
    puts("PASS: common foreground projection across hip/scoped FOVs and aspect ratios, depth/jitter preserved, full matrix inverses, native ctor ret96/copy ret4/pass ret16 hooks, copied-view provenance, world/foreign/multiview/caller guards, whole-pass restoration, RHI rebound and no accumulation.");
}
