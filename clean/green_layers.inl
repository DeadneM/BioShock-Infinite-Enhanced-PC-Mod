// Experimental visual selector. Pipeline groups are NOT native ownership layers.
#include <d3dcompiler.h>
#include "MinHook.h"
namespace green_layers {
struct Entry {ID3D11VertexShader *vs;ID3D11PixelShader *ps;unsigned group;};
static Entry entries[192]={};static unsigned count,selected=~0u,groupCursor[3]={};
static bool enabled,held[5]={},focusedBefore,hooksReady;
static unsigned width,height,frameDraws,matched;
static ID3D11Device *owner;
static ID3D11Texture2D *mask;
static ID3D11RenderTargetView *target;
static ID3D11ShaderResourceView *image;
static ID3D11PixelShader *green;
static ID3D11BlendState *solid;
static ID3D11DepthStencilState *noDepth;
static SRWLOCK lock=SRWLOCK_INIT;
template<class T> static void drop(T *&p){if(p)p->Release();p=nullptr;}
static void dropMask(){drop(image);drop(target);drop(mask);width=height=0;}
static void reset() {
    dropMask();drop(green);drop(solid);drop(noDepth);drop(owner);
    for(unsigned i=0;i<count;++i){drop(entries[i].vs);drop(entries[i].ps);}
    count=0;selected=~0u;for(auto &c:groupCursor)c=0;
}
static bool prepare(ID3D11Device *dev,unsigned w,unsigned h) {
    if(owner!=dev){reset();owner=dev;owner->AddRef();}
    if(!green) {
        const char code[]="float4 main():SV_Target{return float4(0,1,0,1);}";
        ID3DBlob *blob=nullptr,*error=nullptr;
        HRESULT hr=D3DCompile(code,sizeof(code)-1,nullptr,nullptr,nullptr,"main","ps_4_0",0,0,&blob,&error);
        drop(error);if(FAILED(hr))return false;
        hr=dev->CreatePixelShader(blob->GetBufferPointer(),blob->GetBufferSize(),nullptr,&green);drop(blob);
        if(FAILED(hr))return false;
    }
    if(!solid){D3D11_BLEND_DESC d={};d.RenderTarget[0].RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
        if(FAILED(dev->CreateBlendState(&d,&solid)))return false;}
    if(!noDepth){D3D11_DEPTH_STENCIL_DESC d={};d.DepthFunc=D3D11_COMPARISON_ALWAYS;
        if(FAILED(dev->CreateDepthStencilState(&d,&noDepth)))return false;}
    if(width!=w || height!=h)dropMask();
    if(!mask){D3D11_TEXTURE2D_DESC d={};d.Width=w;d.Height=h;d.MipLevels=d.ArraySize=1;
        d.Format=DXGI_FORMAT_R8G8B8A8_UNORM;d.SampleDesc.Count=1;d.Usage=D3D11_USAGE_DEFAULT;
        d.BindFlags=D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;
        if(FAILED(dev->CreateTexture2D(&d,nullptr,&mask)) || FAILED(dev->CreateRenderTargetView(mask,nullptr,&target))
            || FAILED(dev->CreateShaderResourceView(mask,nullptr,&image))){dropMask();return false;}
        width=w;height=h;
    }
    return true;
}
static unsigned classify(bool blend,bool depthWrite) {return blend?2:depthWrite?0:1;}
static void choose(unsigned group) {
    unsigned ids[192],n=0;for(unsigned i=0;i<count;++i)if(entries[i].group==group)ids[n++]=i;
    if(!n){selected=~0u;return;}
    unsigned &cursor=groupCursor[group];selected=ids[cursor++%n];
    modLog("Green diagnostic: group=%u shader-pair=%u (candidate, ownership UNKNOWN)",group+1,selected+1);
}
static void input(bool focused,SHORT (*query)(int)) {
    const int keys[]={VK_F5,VK_F6,VK_F7,VK_F8,VK_F5};
    bool ctrl=(query(VK_CONTROL)&0x8000)!=0,alt=(query(VK_MENU)&0x8000)!=0;
    bool shift=(query(VK_SHIFT)&0x8000)!=0,win=(query(VK_LWIN)&0x8000)||(query(VK_RWIN)&0x8000);
    for(unsigned i=0;i<5;++i) {
        bool down=(query(keys[i])&0x8000)!=0;
        bool edge=focused && focusedBefore && down && !held[i];held[i]=down;
        if(!edge)continue;
        if(i==4 && ctrl && alt && !shift && !win){enabled=!enabled;selected=~0u;modLog("Green diagnostic %s",enabled?"ON":"OFF");}
        else if(i<4 && enabled && !ctrl && !alt && !shift && !win){if(i==3)selected=~0u;else choose(i);}
    }
    focusedBefore=focused;
    if(!focused)selected=~0u;
}
typedef void (STDMETHODCALLTYPE *IndexedFn)(ID3D11DeviceContext *,UINT,UINT,INT);
typedef void (STDMETHODCALLTYPE *DrawFn)(ID3D11DeviceContext *,UINT,UINT);
typedef void (STDMETHODCALLTYPE *IndexedInstFn)(ID3D11DeviceContext *,UINT,UINT,UINT,INT,UINT);
typedef void (STDMETHODCALLTYPE *InstFn)(ID3D11DeviceContext *,UINT,UINT,UINT,UINT);
template<class F> static void inspect(ID3D11DeviceContext *ctx,F replay) {
    if(!enabled || !hooksReady || !target || ctx->GetType()!=D3D11_DEVICE_CONTEXT_IMMEDIATE || !TryAcquireSRWLockExclusive(&lock))return;
    ID3D11Device *actual=nullptr;ctx->GetDevice(&actual);bool correctDevice=actual==owner;drop(actual);
    if(!correctDevice){ReleaseSRWLockExclusive(&lock);return;}
    // Every getter reference is released even on a rejected candidate.
    ID3D11RenderTargetView *rt[8]={};ID3D11DepthStencilView *depth=nullptr;
    ID3D11VertexShader *vs=nullptr;ID3D11PixelShader *ps=nullptr;
    ID3D11GeometryShader *gs=nullptr;ID3D11HullShader *hs=nullptr;ID3D11DomainShader *ds=nullptr;
    ID3D11BlendState *blend=nullptr;ID3D11DepthStencilState *depthState=nullptr;
    ID3D11Buffer *so[4]={};ID3D11UnorderedAccessView *uav[8]={};
    ID3D11ClassInstance *classes[256]={};UINT classCount=256;
    ctx->OMGetRenderTargets(8,rt,&depth);ctx->VSGetShader(&vs,nullptr,nullptr);ctx->PSGetShader(&ps,classes,&classCount);
    ctx->GSGetShader(&gs,nullptr,nullptr);ctx->HSGetShader(&hs,nullptr,nullptr);ctx->DSGetShader(&ds,nullptr,nullptr);
    ctx->SOGetTargets(4,so);ctx->OMGetRenderTargetsAndUnorderedAccessViews(0,nullptr,nullptr,0,8,uav);
    FLOAT factor[4];UINT sampleMask,stencil;ctx->OMGetBlendState(&blend,factor,&sampleMask);ctx->OMGetDepthStencilState(&depthState,&stencil);
    D3D11_VIEWPORT vp[16];UINT nvp=16;ctx->RSGetViewports(&nvp,vp);
    D3D11_PRIMITIVE_TOPOLOGY topology;ctx->IAGetPrimitiveTopology(&topology);
    bool compatible=vs && ps && !classCount && rt[0] && rt[0]!=target && depth && !gs && !hs && !ds && nvp==1
        && vp[0].TopLeftX==0 && vp[0].TopLeftY==0 && vp[0].Width==width && vp[0].Height==height
        && topology==D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    for(auto *p:so)if(p)compatible=false;for(auto *p:uav)if(p)compatible=false;
    ID3D11Resource *resource=nullptr;ID3D11Texture2D *texture=nullptr;
    if(compatible){rt[0]->GetResource(&resource);resource->QueryInterface(__uuidof(ID3D11Texture2D),(void **)&texture);
        if(texture){D3D11_TEXTURE2D_DESC d;texture->GetDesc(&d);compatible=d.Width==width && d.Height==height && d.SampleDesc.Count==1;}else compatible=false;}
    if(compatible) {
        D3D11_BLEND_DESC b={};if(blend)blend->GetDesc(&b);
        D3D11_DEPTH_STENCIL_DESC d={};d.DepthEnable=TRUE;d.DepthWriteMask=D3D11_DEPTH_WRITE_MASK_ALL;
        if(depthState)depthState->GetDesc(&d);
        unsigned group=classify(b.RenderTarget[0].BlendEnable,d.DepthEnable && d.DepthWriteMask==D3D11_DEPTH_WRITE_MASK_ALL);
        unsigned id=0;for(;id<count;++id)if(entries[id].vs==vs && entries[id].ps==ps && entries[id].group==group)break;
        if(id==count && count<192){entries[count++]={vs,ps,group};vs->AddRef();ps->AddRef();}
        if(id<count && id==selected && frameDraws<2048) {
            ++frameDraws;++matched;
            // Silhouette-only replay into our own transparent texture. Never write native depth/G-buffer.
            ctx->OMSetRenderTargets(1,&target,nullptr);ctx->OMSetDepthStencilState(noDepth,0);
            ctx->OMSetBlendState(solid,nullptr,~0u);ctx->PSSetShader(green,nullptr,0);
            replay();
            ctx->PSSetShader(ps,nullptr,0);ctx->OMSetBlendState(blend,factor,sampleMask);
            ctx->OMSetDepthStencilState(depthState,stencil);ctx->OMSetRenderTargets(8,rt,depth);
        }
    }
    drop(texture);drop(resource);for(auto *&p:rt)drop(p);drop(depth);drop(vs);drop(ps);drop(gs);drop(hs);drop(ds);
    drop(blend);drop(depthState);for(auto *&p:so)drop(p);for(auto *&p:uav)drop(p);
    for(UINT i=0;i<classCount;++i)drop(classes[i]);
    ReleaseSRWLockExclusive(&lock);
}
struct DrawHook {void *address=nullptr,*original=nullptr;};
static DrawHook drawHooks[4][8];
template<unsigned N> static void STDMETHODCALLTYPE indexed(ID3D11DeviceContext *c,UINT n,UINT start,INT base){
    auto f=(IndexedFn)drawHooks[0][N].original;f(c,n,start,base);inspect(c,[&]{f(c,n,start,base);});}
template<unsigned N> static void STDMETHODCALLTYPE draw(ID3D11DeviceContext *c,UINT n,UINT start){
    auto f=(DrawFn)drawHooks[1][N].original;f(c,n,start);inspect(c,[&]{f(c,n,start);});}
template<unsigned N> static void STDMETHODCALLTYPE indexedInst(ID3D11DeviceContext *c,UINT n,UINT instances,UINT start,INT base,UINT first){
    auto f=(IndexedInstFn)drawHooks[2][N].original;f(c,n,instances,start,base,first);inspect(c,[&]{f(c,n,instances,start,base,first);});}
template<unsigned N> static void STDMETHODCALLTYPE inst(ID3D11DeviceContext *c,UINT n,UINT instances,UINT start,UINT first){
    auto f=(InstFn)drawHooks[3][N].original;f(c,n,instances,start,first);inspect(c,[&]{f(c,n,instances,start,first);});}
#define GREEN_CALLBACKS(f) {(void *)f<0>,(void *)f<1>,(void *)f<2>,(void *)f<3>,(void *)f<4>,(void *)f<5>,(void *)f<6>,(void *)f<7>}
static void *callbacks[4][8]={GREEN_CALLBACKS(indexed),GREEN_CALLBACKS(draw),GREEN_CALLBACKS(indexedInst),GREEN_CALLBACKS(inst)};
#undef GREEN_CALLBACKS
static bool attach(ID3D11DeviceContext *ctx) {
    MH_STATUS init=MH_Initialize();if(init!=MH_OK && init!=MH_ERROR_ALREADY_INITIALIZED){hooksReady=false;return false;}
    const unsigned slots[]={12,13,20,21};bool ok=true,changed=false;
    for(unsigned g=0;g<4;++g) {
        void *address=(*(void ***)ctx)[slots[g]];unsigned n=0;
        for(;n<8;++n)if(!drawHooks[g][n].address || drawHooks[g][n].address==address)break;
        if(n==8){ok=false;continue;}if(drawHooks[g][n].address)continue;
        auto &h=drawHooks[g][n];
        if(MH_CreateHook(address,callbacks[g][n],&h.original)!=MH_OK){ok=false;continue;}
        h.address=address;
        if(MH_QueueEnableHook(address)!=MH_OK){MH_RemoveHook(address);h={};ok=false;continue;}
        changed=true;
    }
    if(changed && MH_ApplyQueued()!=MH_OK)ok=false;
    hooksReady=ok;return ok;
}
[[maybe_unused]] static void detach(){enabled=false;for(auto &group:drawHooks)for(auto &h:group)if(h.address){MH_DisableHook(h.address);MH_RemoveHook(h.address);h={};}}
static void endFrame(ID3D11DeviceContext *ctx){
    ID3D11Device *actual=nullptr;ctx->GetDevice(&actual);
    if(target && actual==owner){const float clear[4]={};ctx->ClearRenderTargetView(target,clear);}
    drop(actual);frameDraws=matched=0;
}
}
