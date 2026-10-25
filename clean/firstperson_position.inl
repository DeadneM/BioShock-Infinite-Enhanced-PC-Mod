// Native root placement only; no projection, mesh, particle or asset changes.
namespace firstperson_position {
using PlaceFn=void (__thiscall *)(void *,void *,void *);
PlaceFn original;
bool installed=false,ready=false;
Obj controller=0,pawn=0,root=0;
volatile LONG state=0;
thread_local bool active=false;
const BYTE signature[]={0x55,0x8b,0xec,0x83,0xe4,0xf0,0x81,0xec,0x94,0,0,0,0x53,0x8b,0x5d,0x0c};
const BYTE offsetXSignature[]={0xf3,0x0f,0x10,0xbf,0,2,0,0};
void inspect(Obj pc,Obj actor,bool live){
    if(!live){ready=false;controller=pawn=root=0;InterlockedExchange(&state,0);return;}
    Obj next=ptr(actor+0x1d38);
    if(pc==controller && pawn==actor && root==next && ready)return;
    controller=pc;pawn=actor;root=next;ready=false;
    Obj prop=field(root,"PlayerViewOffset","StructProperty");
    ready=classIs(root,"XFirstPersonAttachment")
        && player_options::property(actor,"FirstPersonAttachment","ObjectProperty",0x1d38,4)
        && player_options::property(root,"Instigator","ObjectProperty",0xa0,4)
        && ptr(root+0xa0)==pawn
        && player_options::property(root,"PlayerViewOffset","StructProperty",0x200,12)
        && nameIs(ptr(prop+0x58),"Vector")
        && ptr(ptr(root)+0x2c8)==(Obj)host+0x77c850;
    InterlockedExchange(&state,ready?1:-1);
    modLog("First-person position: root schema %s; root=%p; native placement coverage awaits gameplay",ready?"qualified":"unavailable",(void *)root);
}
bool eligible(Obj object){
    // Menus deliberately do not alter placement: opening the mod must not
    // visually switch an already active offset. No input is injected here.
    return installed && ready && object==root && player_options::live(controller,pawn)
        && ptr(pawn+0x1d38)==root && ptr(root+0xa0)==pawn
        && !(read<unsigned>(controller+0x690)&0x70000) && !player_options::fovPolicy.zooming
        && !(read<unsigned>(player_options::checkedCamera+0x20c)&1);
}
void __fastcall place(void *object,void *,void *location,void *rotation){
    int back=(int)InterlockedCompareExchange(&weaponBack,0,0);
    if(active || !firstperson_setting::valid(back) || !back || !eligible((Obj)object)){
        original(object,location,rotation);return;
    }
    float offset[3]={};
    if(!copy(root+0x200,offset,sizeof(offset)) || !std::isfinite(offset[0]) || !std::isfinite(offset[1]) || !std::isfinite(offset[2])
        || fabsf(offset[0])>1000 || fabsf(offset[1])>1000 || fabsf(offset[2])>1000){
        InterlockedExchange(&state,-1);original(object,location,rotation);return;
    }
    const Obj bound=root,boundPawn=pawn;
    // Negative local X moves the rig toward the camera, as requested.
    float changed=offset[0]-(float)back;
    active=true;memcpy((void *)(bound+0x200),&changed,4);
    original(object,location,rotation);
    // Restore only our exact temporary value while ownership is unchanged.
    // A native write or destroyed/replaced root is not overwritten.
    if(ptr(boundPawn+0x1d38)==bound && ptr(bound+0xa0)==boundPawn && read<float>(bound+0x200)==changed)
        memcpy((void *)(bound+0x200),&offset[0],4);
    active=false;
    LONG old=InterlockedExchange(&state,2);
    if(old!=2)modLog("First-person position: native placement matched; local X offset -%d units (toward camera); ADS bypassed",back);
}
void install(){
    bool consumer=!memcmp(host+0x77ca68,offsetXSignature,sizeof(offsetXSignature));
    installed=consumer && player_options::installOne(host+0x77c850,signature,sizeof(signature),(void *)place,(void **)&original);
    modLog("First-person position: native placement hook %s; default offset 0, no projection changes",installed?"ready":"unavailable");
}
}
