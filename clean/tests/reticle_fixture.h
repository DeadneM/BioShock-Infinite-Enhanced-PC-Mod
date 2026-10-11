// Include after resources_fixture.h for its synthetic reflection builder.
namespace reticle_fixture {
static int calls;
static reticle::DisplayInfo *__fastcall get(void *object,void *,reticle::DisplayInfo *out) {
    CHECK(GetCurrentThreadId()==windowThread);
    *out={};out->flags=0x800|(*(unsigned *)((BYTE *)object+0x40)&1);return out;
}
static void __fastcall set(void *object,void *,int visible) {
    CHECK(GetCurrentThreadId()==windowThread && (visible==0 || visible==1));
    *(unsigned *)((BYTE *)object+0x40)=(unsigned)visible;++calls;
}
}
static void testReticle() {
    host=(BYTE *)VirtualAlloc(nullptr,0x1400000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);CHECK(host);
    reticle::nativeHost=nullptr;CHECK(!reticle::prepare());
    memcpy(host+0x876d10,reticle::getSignature,sizeof(reticle::getSignature));
    memcpy(host+0x71e0b0,reticle::setSignature,sizeof(reticle::setSignature));
    reticle::nativeHost=nullptr;CHECK(reticle::prepare());
    reticle::getDisplayInfo=(reticle::GetDisplayInfo)reticle_fixture::get;
    reticle::setVisible=(reticle::SetVisible)reticle_fixture::set;reticle_fixture::calls=0;
    resource_fixture::Engine e;
    BYTE *objectType=e.object("ObjectProperty",e.meta),*boolType=e.object("BoolProperty",e.meta);
    BYTE *arrayType=e.object("ArrayProperty",e.meta),*nameType=e.object("NameProperty",e.meta);
    BYTE *hudClass=e.object("XSinglePlayerGFxHUD",e.meta),*widgetClass=e.object("XClikHUDCrosshair",e.meta);
    BYTE *otherClass=e.object("XClikHUDHealthElements",e.meta);
    e.property(e.pcClass,"ControllerHUD",objectType,0x17e8,4);
    e.property(hudClass,"PlayerOwner",objectType,0x5c,4);
    BYTE *handler=e.property(hudClass,"ClikWidgetBindingHandler",e.structType,0x1a8,24);
    BYTE *handlerType=e.object("XClikWidgetBindingHandler",e.scriptType);*(Obj *)(handler+0x58)=(Obj)handlerType;
    BYTE *boundArray=e.property(handlerType,"BoundClikWidgets",arrayType,0,12);
    BYTE *boundInner=e.object("BoundClikWidgets",e.structType);*(Obj *)(boundArray+0x58)=(Obj)boundInner;*(unsigned *)(boundInner+0x30)=12;
    BYTE *boundType=e.object("XBoundClikWidget",e.scriptType);*(Obj *)(boundInner+0x58)=(Obj)boundType;
    e.property(boundType,"WidgetName",nameType,0,8);e.property(boundType,"XClikWidget",objectType,8,4);
    BYTE *shown=e.property(widgetClass,"IsShown",boolType,0x118,4);*(unsigned *)(shown+0x58)=1;
    BYTE *combat=e.property(widgetClass,"IsInCombat",boolType,0x118,4);*(unsigned *)(combat+0x58)=2;
    // Captured live reflection, not inferred from the native combat branch:
    // IsShown=1, IsInCombat=2, IsCenterpointVisible=4, IsInHitTargetAnimation=8.
    BYTE *center=e.property(widgetClass,"IsCenterpointVisible",boolType,0x118,4);*(unsigned *)(center+0x58)=4;
    BYTE pc[0x1800]={},pawn[0x300]={},hudA[0x200]={},hudB[0x200]={},first[0x120]={},second[0x120]={},health[0x120]={},replacement[0x120]={};
    auto object=[&](BYTE *p,BYTE *type,const char *name,Obj outer) {
        *(Obj *)(p+0x20)=(Obj)type;*(unsigned *)(p+0x18)=e.id(name);*(Obj *)(p+0x14)=outer;
        // Actual linked HUD widgets use managed DisplayObject type 0x48.
        *(Obj *)(p+0x28)=1;*(unsigned *)(p+0x2c)=0x48;*(Obj *)(p+0x30)=1;
    };
    object(pc,e.pcClass,"XPlayerController",0);object(pawn,e.pawnClass,"XHuman",0);
    object(hudA,hudClass,"XSinglePlayerGFxHUD",0);object(hudB,hudClass,"XSinglePlayerGFxHUD",0);
    object(first,widgetClass,"XClikHUDCrosshair",(Obj)hudA);object(second,widgetClass,"XClikHUDCrosshair",(Obj)hudA);
    object(health,otherClass,"XClikHUDHealthElements",(Obj)hudA);object(replacement,widgetClass,"XClikHUDCrosshair",(Obj)hudB);
    *(Obj *)(pc+0x1fc)=(Obj)pawn;*(Obj *)(pawn+0x218)=(Obj)pc;*(Obj *)(pc+0x17e8)=(Obj)hudA;
    *(Obj *)(hudA+0x5c)=(Obj)pc;*(Obj *)(hudB+0x5c)=(Obj)pc;
    reticle::Binding entries[]={{0,0,(Obj)first},{0,0,(Obj)second},{0,0,(Obj)health}},next[]={{0,0,(Obj)replacement}};
    *(reticle::Array *)(hudA+0x1a8)={(Obj)entries,3,3};*(reticle::Array *)(hudB+0x1a8)={(Obj)next,1,1};
    auto bits=[](BYTE *p)->unsigned & {return *(unsigned *)(p+0x118);};
    auto visible=[](BYTE *p)->unsigned & {return *(unsigned *)(p+0x40);};
    auto tick=[&](){reticle::update((Obj)pc,(Obj)pawn,true);};
    windowThread=GetCurrentThreadId();crosshairHidden=0;reticle::reset();
    bits(first)=0xa3;bits(second)=0xb2;bits(health)=0xf3;bits(replacement)=0xc1;
    visible(first)=1;visible(second)=0;visible(health)=1;visible(replacement)=1;
    tick();CHECK(visible(first)==1 && visible(second)==0 && visible(health)==1 && reticle_fixture::calls==0);
    crosshairHidden=1;tick();CHECK(reticle::state==1 && visible(first)==0 && visible(second)==0 && visible(health)==1);
    CHECK(bits(first)==0xa3 && bits(second)==0xb2 && bits(health)==0xf3 && reticle_fixture::calls==1);
    // Verify actual Flash visibility, with all cached animation/centerpoint bits
    // untouched. Native re-show is suppressed on the next camera callback.
    visible(first)=1;bits(first)=0xd3;tick();CHECK(visible(first)==0 && bits(first)==0xd3 && reticle_fixture::calls==2);
    crosshairHidden=0;tick();CHECK(visible(first)==1 && visible(second)==0 && visible(health)==1);
    crosshairHidden=1;tick();reticle::update((Obj)pc,(Obj)pawn,false);
    CHECK(visible(first)==1 && visible(second)==0 && reticle::state==0);
    // HUD replacement acquires the new widgets, and does not write an old HUD
    // no longer linked to the current controller (which may have been freed).
    tick();*(Obj *)(pc+0x17e8)=(Obj)hudB;tick();CHECK(visible(replacement)==0);
    crosshairHidden=0;tick();CHECK(visible(replacement)==1 && visible(first)==0);
    *(Obj *)(pc+0x17e8)=(Obj)hudA;visible(first)=1;visible(second)=0;
    crosshairHidden=1;windowThread=0;tick();CHECK(visible(first)==1);windowThread=GetCurrentThreadId();
    // Any changed reflected offset/mask or array structure refuses the write.
    *(unsigned *)(center+0x58)=2;tick();CHECK(reticle::state==-1 && visible(first)==1);
    crosshairHidden=0;tick();*(unsigned *)(center+0x58)=4;
    crosshairHidden=1;*(unsigned *)(handler+0x48)=0x1b0;tick();CHECK(reticle::state==-1 && visible(first)==1);
    crosshairHidden=0;tick();*(unsigned *)(handler+0x48)=0x1a8;
    crosshairHidden=1;((reticle::Array *)(hudA+0x1a8))->count=257;tick();CHECK(visible(first)==1);
    ((reticle::Array *)(hudA+0x1a8))->count=3;tick();CHECK(visible(first)==0);
    // Name serial/outer changes identify a reused object: no stale restore.
    ++*(unsigned *)(first+0x1c);crosshairHidden=0;tick();CHECK(visible(first)==0 && visible(second)==0);
    visible(first)=1;crosshairHidden=1;tick();*(Obj *)(first+0x14)=(Obj)hudB;
    crosshairHidden=0;tick();CHECK(visible(first)==0);
    *(Obj *)(first+0x14)=(Obj)hudA;visible(first)=1;
    *(Obj *)(hudA+0x5c)=0;crosshairHidden=1;tick();CHECK(visible(first)==1);
    crosshairHidden=0;reticle::reset();windowThread=0;
    CHECK(VirtualFree(host,0,MEM_RELEASE));host=nullptr;
    reticle::nativeHost=nullptr;reticle::getDisplayInfo=nullptr;reticle::setVisible=nullptr;
    puts("PASS: reticle native getter/setter ABI and signature guards, actual visibility/restoration with animation bits untouched, other HUD preserved, native reassertion, lifecycle guards, malformed arrays, schema drift and wrong thread.");
}
