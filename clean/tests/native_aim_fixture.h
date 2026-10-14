// Actual no-argument thiscall getter, ordinary/TBar branches and the three
// qualified x87 camera consumers. The fixture changes target returns only.
static void * __fastcall nativeAimFlags(void *object,void *,void *) {return (BYTE *)object+4;}
static void testNativeAimTarget() {
    using namespace player_options;
    CHECK(fabsf(opticalAimTarget(60,82.5f,120)-97.5f)<0.0001f);
    for(int desired:{80,100,120,130})for(float native:{10.0f,40.0f,60.0f,65.0f}) {
        float changed=opticalAimTarget(native,82.5f,desired);
        CHECK(changed>1 && changed<desired);
        CHECK(fabsf(tanf(changed*0.00872664626f)/tanf(desired*0.00872664626f)
            -tanf(native*0.00872664626f)/tanf(82.5f*0.00872664626f))<0.00001f);
    }
    CHECK(opticalAimTarget(60,0,120)==60 && opticalAimTarget(90,82.5f,120)==90);
    host=(BYTE *)VirtualAlloc(nullptr,0x940000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);CHECK(host);
    CHECK(MH_Initialize()==MH_OK);
    // Supported getter bytes: no absolute host references or saved-state writes.
    const BYTE getter[]={
        0x83,0xec,0x08,0x56,0x8b,0xf1,0x8b,0x86,0x88,0x0d,0,0,0x85,0xc0,0x74,0x34,
        0x8d,0x88,0xd4,0x03,0,0,0x8b,0x01,0x8b,0x40,0x24,0x8d,0x54,0x24,0x04,0x52,
        0xff,0xd0,0x83,0x38,0,0x74,0x1d,0x83,0x78,0x04,0,0x74,0x17,
        0xf3,0x0f,0x10,0x86,0x4c,0x08,0,0,0xf3,0x0f,0x11,0x44,0x24,0x04,0xd9,0x44,0x24,0x04,
        0x5e,0x83,0xc4,0x08,0xc3,0xf3,0x0f,0x10,0x86,0x48,0x08,0,0,
        0xf3,0x0f,0x11,0x44,0x24,0x04,0xd9,0x44,0x24,0x04,0x5e,0x83,0xc4,0x08,0xc3};
    memcpy(host+0x108380,getter,sizeof(getter));
    memcpy(host+0x285190,fovSignature,sizeof(fovSignature));
    const BYTE fovTail[]={0xd9,0x44,0x24,0x20,0xd8,0x85,0x90,0x02,0,0,0x5f,0x5e,0x5d,0x5b,0x83,0xc4,0x0c,0xc2,0x04,0};
    memcpy(host+0x285190+sizeof(fovSignature),fovTail,sizeof(fovTail));
    const BYTE argument[]={0xff,0x74,0x24,0x04};memcpy(host+0x938d78,argument,sizeof(argument));
    memcpy(host+0x938d7c,cameraFovCall,sizeof(cameraFovCall));host[0x938d81]=0xc2;host[0x938d82]=4;
    const unsigned calls[]={0x11db17,0x18296c,0x1829fe};
    for(unsigned i=0;i<3;++i)memcpy(host+calls[i],aimTargetCalls[i],5);
    host[calls[0]+5]=0xc3;
    // Synthetic argument setup followed by the actual native entry/release
    // x87 blend instruction sequences. Camera stores remain engine-owned.
    const BYTE setup[]={0x56,0x83,0xec,0x10,0x8b,0x74,0x24,0x18,0x8b,0x44,0x24,0x1c,
        0x89,0x44,0x24,0x0c,0x8b,0x44,0x24,0x20,0x89,0x44,0x24,0x08};
    const BYTE entry[]={0xd9,0x44,0x24,0x0c,0x83,0x8e,0x0c,0x02,0,0,1,0xdc,0xe9,0xd9,0xc9,
        0xd8,0x4c,0x24,8,0xde,0xc1,0xd9,0x9e,0x10,0x02,0,0,0x83,0xc4,0x10,0x5e,0xc2,0x0c,0};
    const BYTE leave[]={0xd9,0x44,0x24,0x0c,0x83,0x8e,0x0c,0x02,0,0,1,0xdc,0xe9,0xd9,0xe8,
        0xd8,0x64,0x24,8,0xde,0xca,0xde,0xc1,0xd9,0x9e,0x10,0x02,0,0,0x83,0xc4,0x10,0x5e,0xc2,0x0c,0};
    for(unsigned i=1;i<3;++i) {memset(host+calls[i]-40,0x90,40);memcpy(host+calls[i]-40,setup,sizeof(setup));}
    memcpy(host+calls[1]+5,entry,sizeof(entry));memcpy(host+calls[2]+5,leave,sizeof(leave));
    CHECK(installOne(host+0x108380,aimTargetSignature,sizeof(aimTargetSignature),(void *)aimTarget,(void **)&aimTargetOriginal));
    CHECK(installOne(host+0x285190,fovSignature,sizeof(fovSignature),(void *)fov,(void **)&fovOriginal));fovInstalled=true;
    aimTargetInstalled=true;
    FlushInstructionCache(GetCurrentProcess(),host,0x940000);
    BYTE pc[0x800]={},pawn[0x4000]={},cam[0x400]={},weapon[0xe00]={},manager[0x400]={};
    void *vt[10]={};vt[9]=(void *)nativeAimFlags;
    *(void **)(pawn+0x3d4)=vt;*(unsigned *)(pawn+0x3d8)=0;*(unsigned *)(pawn+0x3dc)=0;
    *(Obj *)(weapon+0xd88)=(Obj)pawn;*(float *)(weapon+0x848)=60;*(float *)(weapon+0x84c)=65;
    *(Obj *)(pc+0x1fc)=(Obj)pawn;*(Obj *)(pc+0x240)=(Obj)cam;*(Obj *)(pawn+0x218)=(Obj)pc;*(float *)(pawn+0x2ac)=100;
    *(Obj *)(pawn+0x314)=(Obj)manager;*(Obj *)(manager+0x8c)=(Obj)pawn;*(int *)(manager+0x2a0)=1;*(Obj *)(manager+0x200)=(Obj)weapon;
    *(Obj *)(cam+0x1fc)=(Obj)pc;
    *(float *)(pc+0x290)=7.5f;
    checkedPC=(Obj)pc;checkedPawn=(Obj)pawn;checkedCamera=(Obj)cam;checkedWeapon=(Obj)weapon;
    schemaReady=cameraSchemaReady=aimSchemaReady=true;windowThread=GetCurrentThreadId();hipLens={checkedPC,checkedPawn,checkedCamera,82.5f};
    AimTargetFn volatile direct=(AimTargetFn)(host+0x108380),instant=(AimTargetFn)(host+calls[0]);
    using Blend=void (__thiscall *)(void *,void *,float,float);
    Blend volatile enter=(Blend)(host+calls[1]-40),release=(Blend)(host+calls[2]-40);
    fovDegrees=120;nativeAimZoom=0;CHECK(instant(weapon)==60);
    FovFn volatile hip=(FovFn)(host+0x938d78);CHECK(hip(pc,75)==120);
    nativeAimZoom=1;float expected=opticalAimTarget(60,82.5f,120);
    *(unsigned *)(pc+0x690)=0x20000;
    for(int i=0;i<2000;++i) {
        CHECK(fabsf(instant(weapon)-expected)<0.0001f && direct(weapon)==60);
        CHECK(hip(pc,75)==120 && hipLens.angle==82.5f);
        float alpha=(float)(i%101)/100;
        enter(weapon,cam,120,alpha);CHECK(fabsf(*(float *)(cam+0x210)-(120+(expected-120)*alpha))<0.0001f);
        release(weapon,cam,120,alpha);CHECK(fabsf(*(float *)(cam+0x210)-(120+(expected-120)*(1-alpha)))<0.0001f);
        CHECK(*(float *)(weapon+0x848)==60 && *(float *)(weapon+0x84c)==65);
    }
    *(unsigned *)(pawn+0x3d8)=*(unsigned *)(pawn+0x3dc)=1;
    CHECK(fabsf(instant(weapon)-opticalAimTarget(65,82.5f,120))<0.0001f && direct(weapon)==65);
    nativeAimZoom=0;CHECK(instant(weapon)==65 && hip(pc,75)==82.5f);nativeAimZoom=1;
    *(unsigned *)(pc+0x690)=0;CHECK(hip(pc,75)==120);
    hipLens={};CHECK(instant(weapon)==65);hipLens={checkedPC,checkedPawn,checkedCamera,82.5f};
    *(Obj *)(manager+0x200)=0;CHECK(instant(weapon)==65);*(Obj *)(manager+0x200)=(Obj)weapon;
    *(unsigned *)(pc+0x248)=0x20000;CHECK(instant(weapon)==65);*(unsigned *)(pc+0x248)=0;
    *(Obj *)(cam+0x1fc)=0;CHECK(instant(weapon)==65);*(Obj *)(cam+0x1fc)=(Obj)pc;
    windowThread=0;CHECK(instant(weapon)==65);windowThread=GetCurrentThreadId();
    aimSchemaReady=false;CHECK(instant(weapon)==65);
    CHECK(MH_DisableHook(host+0x108380)==MH_OK);CHECK(MH_RemoveHook(host+0x108380)==MH_OK);CHECK(instant(weapon)==65);
    CHECK(MH_DisableHook(host+0x285190)==MH_OK);CHECK(MH_RemoveHook(host+0x285190)==MH_OK);
    CHECK(MH_Uninitialize()==MH_OK);VirtualFree(host,0,MEM_RELEASE);host=nullptr;
    aimTargetOriginal=nullptr;aimTargetInstalled=aimSchemaReady=schemaReady=cameraSchemaReady=false;
    checkedPC=checkedPawn=checkedCamera=checkedWeapon=0;hipLens={};windowThread=0;nativeAimZoom=0;fovDegrees=80;aimState=0;
    fovInstalled=false;fovOriginal=nullptr;fovPolicy={};
    puts("PASS: native ordinary/TBar target ABI, 2000 entry/release blends, caller/owner/active-weapon/thread/baseline guards, OFF and hook removal, no weapon-property writes.");
}
