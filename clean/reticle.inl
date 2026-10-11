// Included after weapons.cpp's read/reflection helpers. This follows the
// controller's live HUD binding array; it never scans or modifies other HUDs.
namespace reticle {
volatile LONG state=0; // 0 waiting/off, 1 applied, -1 unsupported layout
struct Binding {unsigned name,number;Obj widget;};
struct Array {Obj data;int count,capacity;};
struct Saved {
    Obj object=0,type=0,outer=0;
    unsigned name=0,number=0;
    bool visible=false;
};
struct DisplayInfo {float values[10];unsigned flags;};
using GetDisplayInfo=DisplayInfo *(__thiscall *)(void *,DisplayInfo *);
using SetVisible=void (__thiscall *)(void *,int);
static GetDisplayInfo getDisplayInfo=nullptr;
static SetVisible setVisible=nullptr;
static BYTE *nativeHost=nullptr;
static const BYTE getSignature[]={0x81,0xec,0xb4,0,0,0,0x56,0x8b,0xf1,0x8d,0x4c,0x24,0x34,0xe8,0xae,0x3d,0x2a,0,0x8d,0x4c,0x24,0x74,0xe8,0xa5,0x3d,0x2a,0};
static const BYTE setSignature[]={0x81,0xec,0xb4,0,0,0,0x56,0x8b,0xf1,0x8d,0x4c,0x24,0x34,0xe8,0x0e,0xca,0x3f,0,0x8d,0x4c,0x24,0x74,0xe8,0x05,0xca,0x3f,0};
static bool prepare() {
    if(nativeHost==host)return getDisplayInfo && setVisible;
    nativeHost=host;getDisplayInfo=nullptr;setVisible=nullptr;
    if(!host || memcmp(host+0x876d10,getSignature,sizeof(getSignature))
        || memcmp(host+0x71e0b0,setSignature,sizeof(setSignature)))return false;
    getDisplayInfo=(GetDisplayInfo)(host+0x876d10);setVisible=(SetVisible)(host+0x71e0b0);return true;
}
static Saved saved[8];
static unsigned savedCount=0;
static Obj controller=0,hud=0,checkedClass=0;
static bool schema=false;
static DWORD scannedAt=0;

static Obj member(Obj owner,const char *label,const char *kind,unsigned offset,unsigned size) {
    for(Obj child=ptr(owner+0x38),n=0;child && n<512;child=ptr(child+0x28),++n)
        if(nameIs(child,label) && classIs(child,kind) && read<unsigned>(child+0x48)==offset
            && read<unsigned>(child+0x30)==size && read<unsigned>(child+0x2c)==1)return child;
    return 0;
}
static bool property(Obj object,const char *label,const char *kind,unsigned offset,unsigned size) {
    Obj p=field(object,label,kind);
    return p && read<unsigned>(p+0x48)==offset && read<unsigned>(p+0x30)==size && read<unsigned>(p+0x2c)==1;
}
static bool layout(Obj pc,Obj screen) {
    if(!classIs(pc,"XPlayerController") || !classIs(screen,"XSinglePlayerGFxHUD")
        || !property(pc,"ControllerHUD","ObjectProperty",0x17e8,4)
        || !property(screen,"PlayerOwner","ObjectProperty",0x5c,4)
        || !property(screen,"ClikWidgetBindingHandler","StructProperty",0x1a8,24))return false;
    Obj handler=ptr(field(screen,"ClikWidgetBindingHandler","StructProperty")+0x58);
    if(!classIs(handler,"ScriptStruct") || !nameIs(handler,"XClikWidgetBindingHandler"))return false;
    Obj list=member(handler,"BoundClikWidgets","ArrayProperty",0,12),inner=ptr(list+0x58),bound=ptr(inner+0x58);
    return list && classIs(inner,"StructProperty") && read<unsigned>(inner+0x30)==12
        && classIs(bound,"ScriptStruct") && nameIs(bound,"XBoundClikWidget")
        && member(bound,"WidgetName","NameProperty",0,8) && member(bound,"XClikWidget","ObjectProperty",8,4);
}
static bool widgetLayout(Obj object) {
    if(!classIs(object,"XClikHUDCrosshair"))return false;
    Obj shown=field(object,"IsShown","BoolProperty"),combat=field(object,"IsInCombat","BoolProperty");
    Obj center=field(object,"IsCenterpointVisible","BoolProperty");
    return property(object,"IsShown","BoolProperty",0x118,4)
        && property(object,"IsInCombat","BoolProperty",0x118,4)
        && property(object,"IsCenterpointVisible","BoolProperty",0x118,4)
        // Live class reflection: shown=1, combat=2, centerpoint=4. The native
        // animation branch at RVA 0x1a3be0 tests combat, not centerpoint.
        && read<unsigned>(shown+0x58)==1 && read<unsigned>(combat+0x58)==2
        && read<unsigned>(center+0x58)==4;
}
static bool bindings(Obj pc,Obj screen,Array &list) {
    if(!pc || !screen || !classIs(pc,"XPlayerController") || !classIs(screen,"XSinglePlayerGFxHUD")
        || ptr(pc+0x17e8)!=screen || ptr(screen+0x5c)!=pc || !copy(screen+0x1a8,&list,sizeof(list)))return false;
    return list.count>=0 && list.count<=256 && list.capacity>=list.count && list.capacity<=1024
        && (!list.count || list.data) && !(list.data&3);
}
static bool same(const Saved &item) {
    return item.object && ptr(item.object+0x20)==item.type && ptr(item.object+0x14)==item.outer
        && read<unsigned>(item.object+0x18)==item.name && read<unsigned>(item.object+0x1c)==item.number
        && classIs(item.object,"XClikHUDCrosshair");
}
static bool contains(const Array &list,Obj object) {
    for(int i=0;i<list.count;++i)if(ptr(list.data+i*sizeof(Binding)+8)==object)return true;
    return false;
}
static bool display(Obj object,DisplayInfo &info) {
    // GFxValue must contain a live DisplayObject (8). The getter returns
    // FGFxDisplayInfo (44 bytes), with visibility at +0x28 bit0 and its validity
    // bit11. Both offsets are verified in native conversion RVA 0x9e2360.
    if(!getDisplayInfo || !setVisible || (read<unsigned>(object+0x2c)&0x8f)!=8
        || !ptr(object+0x28) || !ptr(object+0x30))return false;
    info={};getDisplayInfo((void *)object,&info);return (info.flags&0x800)!=0;
}
static bool visibility(const Saved &item,bool wanted) {
    DisplayInfo info={};
    if(!same(item) || !display(item.object,info))return false;
    if(((info.flags&1)!=0)!=wanted) {
        // This is the native GFxObject setter, which changes Flash visibility.
        // IsShown/IsCenterpointVisible are animation state and MUST stay native.
        setVisible((void *)item.object,wanted?1:0);
        if(!display(item.object,info) || ((info.flags&1)!=0)!=wanted)return false;
    }
    return true;
}
static void restore() {
    Array list={};unsigned restored=0;
    // A cached address alone is not sufficient after a load/GC. Only restore
    // instances still linked from their original, validated controller HUD.
    if(schema && bindings(controller,hud,list))for(unsigned i=0;i<savedCount;++i)
        if(contains(list,saved[i].object) && same(saved[i]) && visibility(saved[i],saved[i].visible))++restored;
    if(savedCount)modLog("Reticle: restored native Flash visibility for %u/%u linked widgets",restored,savedCount);
    savedCount=0;
}
static void reset() {
    restore();controller=hud=checkedClass=0;schema=false;scannedAt=0;InterlockedExchange(&state,0);
}
static bool scan(const Array &list) {
    Saved found[8]={};unsigned count=0;
    for(int i=0;i<list.count;++i) {
        Obj object=ptr(list.data+i*sizeof(Binding)+8);
        if(!classIs(object,"XClikHUDCrosshair") || ptr(object+0x14)!=hud)continue;
        if(count==8 || !widgetLayout(object))return false;
        Saved item;item.object=object;item.type=ptr(object+0x20);item.outer=hud;
        item.name=read<unsigned>(object+0x18);item.number=read<unsigned>(object+0x1c);
        DisplayInfo info={};if(!display(object,info))continue;item.visible=(info.flags&1)!=0;
        for(unsigned j=0;j<savedCount;++j)if(saved[j].object==object && same(saved[j]))item.visible=saved[j].visible;
        bool duplicate=false;for(unsigned j=0;j<count;++j)if(found[j].object==object)duplicate=true;
        if(!duplicate)found[count++]=item;
    }
    // Retired widgets no longer belong to the active array and are not touched.
    memcpy(saved,found,sizeof(found));savedCount=count;return true;
}
void update(Obj pc,Obj pawn,bool ready) {
    if(GetCurrentThreadId()!=windowThread)return;
    bool wanted=InterlockedCompareExchange(&crosshairHidden,0,0)!=0;
    if(!wanted || !ready || !pawn || ptr(pc+0x1fc)!=pawn || ptr(pawn+0x218)!=pc) {reset();return;}
    Obj current=ptr(pc+0x17e8),currentClass=ptr(current+0x20);
    if(pc!=controller || current!=hud || currentClass!=checkedClass) {
        reset();controller=pc;hud=current;checkedClass=currentClass;
        schema=prepare() && layout(pc,current);
        modLog("Reticle: controller HUD schema %s, HUD=%p",schema?"verified":"unavailable",(void *)hud);
    }
    if(!schema){InterlockedExchange(&state,-1);return;}
    Array list={};
    if(!bindings(pc,hud,list)){restore();InterlockedExchange(&state,0);return;}
    DWORD now=GetTickCount();
    if(!scannedAt || now-scannedAt>=250) {
        scannedAt=now;
        if(!scan(list)){restore();schema=false;InterlockedExchange(&state,-1);return;}
    }
    unsigned applied=0;
    for(unsigned i=0;i<savedCount;++i)if(contains(list,saved[i].object) && same(saved[i]) && visibility(saved[i],false))++applied;
    LONG result=applied && applied==savedCount?1:0;
    if(InterlockedExchange(&state,result)!=result && result==1)
        modLog("Reticle: native Flash visibility hidden and read back for %u widgets; animation state untouched",applied);
}
}
