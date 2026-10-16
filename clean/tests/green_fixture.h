static void testGreenLayers(ID3D11Device *dev,ID3D11DeviceContext *ctx) {
    using namespace green_layers;
    CHECK(!enabled);CHECK(classify(false,true)==0 && classify(false,false)==1 && classify(true,true)==2);
    CHECK(prepare(dev,32,32));CHECK(attach(ctx));
    ID3DBlob *blob=nullptr;
    const char *vertex="float4 main(uint id:SV_VertexID):SV_Position{float2 p[3]={float2(-1,-1),float2(-1,3),float2(3,-1)};return float4(p[id],0.5,1);}";
    CHECK(SUCCEEDED(D3DCompile(vertex,strlen(vertex),nullptr,nullptr,nullptr,"main","vs_4_0",0,0,&blob,nullptr)));
    ID3D11VertexShader *vs=nullptr;CHECK(SUCCEEDED(dev->CreateVertexShader(blob->GetBufferPointer(),blob->GetBufferSize(),nullptr,&vs)));drop(blob);
    const char *pixel="float4 main():SV_Target{return float4(0,0,1,1);}";
    CHECK(SUCCEEDED(D3DCompile(pixel,strlen(pixel),nullptr,nullptr,nullptr,"main","ps_4_0",0,0,&blob,nullptr)));
    ID3D11PixelShader *ps=nullptr;CHECK(SUCCEEDED(dev->CreatePixelShader(blob->GetBufferPointer(),blob->GetBufferSize(),nullptr,&ps)));drop(blob);
    D3D11_TEXTURE2D_DESC td={};td.Width=td.Height=32;td.ArraySize=td.MipLevels=1;td.SampleDesc.Count=1;
    td.Format=DXGI_FORMAT_R8G8B8A8_UNORM;td.BindFlags=D3D11_BIND_RENDER_TARGET;
    ID3D11Texture2D *color=nullptr,*z=nullptr;ID3D11RenderTargetView *rt=nullptr;ID3D11DepthStencilView *dsv=nullptr;
    CHECK(SUCCEEDED(dev->CreateTexture2D(&td,nullptr,&color)));CHECK(SUCCEEDED(dev->CreateRenderTargetView(color,nullptr,&rt)));
    td.Format=DXGI_FORMAT_D24_UNORM_S8_UINT;td.BindFlags=D3D11_BIND_DEPTH_STENCIL;
    CHECK(SUCCEEDED(dev->CreateTexture2D(&td,nullptr,&z)));CHECK(SUCCEEDED(dev->CreateDepthStencilView(z,nullptr,&dsv)));
    D3D11_RASTERIZER_DESC rd={};rd.FillMode=D3D11_FILL_SOLID;rd.CullMode=D3D11_CULL_NONE;rd.DepthClipEnable=TRUE;
    ID3D11RasterizerState *rs=nullptr;CHECK(SUCCEEDED(dev->CreateRasterizerState(&rd,&rs)));
    D3D11_VIEWPORT vp={0,0,32,32,0,1};ctx->RSSetViewports(1,&vp);ctx->RSSetState(rs);
    ctx->IASetInputLayout(nullptr);ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->VSSetShader(vs,nullptr,0);ctx->PSSetShader(ps,nullptr,0);ctx->OMSetRenderTargets(1,&rt,dsv);
    ctx->OMSetDepthStencilState(nullptr,3);FLOAT factor[]={0.2f,0.3f,0.4f,0.5f};ctx->OMSetBlendState(nullptr,factor,~0u);
    ctx->ClearDepthStencilView(dsv,D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1,0);endFrame(ctx);
    ctx->Draw(3,0);CHECK(count==0); // Default OFF performs no classification/replay.
    enabled=true;ctx->ClearDepthStencilView(dsv,D3D11_CLEAR_DEPTH,1,0);ctx->Draw(3,0);CHECK(count==1);
    keys[VK_F5]=(SHORT)0x8000;CHECK(overlayKeyQuery(VK_F5)==0);keys[VK_F5]=0;
    choose(0);CHECK(selected==0);ctx->ClearDepthStencilView(dsv,D3D11_CLEAR_DEPTH,1,0);ctx->Draw(3,0);CHECK(matched==1);
    UINT indices[]={0,1,2};D3D11_BUFFER_DESC ibd={};ibd.ByteWidth=sizeof(indices);ibd.BindFlags=D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA ibs={};ibs.pSysMem=indices;ID3D11Buffer *ib=nullptr;
    CHECK(SUCCEEDED(dev->CreateBuffer(&ibd,&ibs,&ib)));ctx->IASetIndexBuffer(ib,DXGI_FORMAT_R32_UINT,0);
    ctx->ClearDepthStencilView(dsv,D3D11_CLEAR_DEPTH,1,0);ctx->DrawIndexed(3,0,0);
    ctx->ClearDepthStencilView(dsv,D3D11_CLEAR_DEPTH,1,0);ctx->DrawInstanced(3,1,0,0);
    ctx->ClearDepthStencilView(dsv,D3D11_CLEAR_DEPTH,1,0);ctx->DrawIndexedInstanced(3,1,0,0,0);
    CHECK(matched==4);
    auto pixelAtCenter=[&](ID3D11Texture2D *source,unsigned r,unsigned g,unsigned b,unsigned a){
        D3D11_TEXTURE2D_DESC d;source->GetDesc(&d);d.Usage=D3D11_USAGE_STAGING;d.BindFlags=0;d.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
        ID3D11Texture2D *staging=nullptr;CHECK(SUCCEEDED(dev->CreateTexture2D(&d,nullptr,&staging)));ctx->CopyResource(staging,source);
        D3D11_MAPPED_SUBRESOURCE map;CHECK(SUCCEEDED(ctx->Map(staging,0,D3D11_MAP_READ,0,&map)));
        auto *p=(BYTE *)map.pData+16*map.RowPitch+16*4;CHECK(p[0]==r && p[1]==g && p[2]==b && p[3]==a);
        ctx->Unmap(staging,0);drop(staging);
    };
    pixelAtCenter(mask,0,255,0,255);pixelAtCenter(color,0,0,255,255);
    ID3D11PixelShader *after=nullptr;ctx->PSGetShader(&after,nullptr,nullptr);CHECK(after==ps);drop(after);
    ID3D11RenderTargetView *afterRT=nullptr;ID3D11DepthStencilView *afterZ=nullptr;
    ctx->OMGetRenderTargets(1,&afterRT,&afterZ);CHECK(afterRT==rt && afterZ==dsv);drop(afterRT);drop(afterZ);
    ID3D11DepthStencilState *afterD=nullptr;UINT ref;ctx->OMGetDepthStencilState(&afterD,&ref);CHECK(!afterD && ref==3);
    ID3D11BlendState *afterB=nullptr;FLOAT bf[4];UINT sm;ctx->OMGetBlendState(&afterB,bf,&sm);
    CHECK(!afterB && !memcmp(bf,factor,sizeof(bf)) && sm==~0u);
    // Restore/clear produces transparent mask and no new selection on focus loss.
    endFrame(ctx);pixelAtCenter(mask,0,0,0,0);input(false,testKeys);CHECK(selected==~0u);
    keys[VK_F5]=(SHORT)0x8000;keys[VK_CONTROL]=(SHORT)0x8000;keys[VK_MENU]=(SHORT)0x8000;
    input(true,testKeys);CHECK(enabled); // Held on focus acquisition must not toggle.
    keys[VK_F5]=0;input(true,testKeys);keys[VK_F5]=(SHORT)0x8000;input(true,testKeys);CHECK(!enabled);
    memset(keys,0,sizeof(keys));input(true,testKeys);
    CHECK(prepare(dev,64,32));CHECK(width==64 && height==32);
    ctx->ClearState();drop(ib);drop(vs);drop(ps);drop(color);drop(z);drop(rt);drop(dsv);drop(rs);reset();
    puts("PASS: green replay pixels, untouched native color, shader/RT/depth/blend restoration, OFF guard, focus edges and mask resize.");
}
