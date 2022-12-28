#include "StdAfxRD.h"
#include "IRenderDevice.h"
#include "EmptyRenderDevice.h"
#include "SokolRender.h"
#include "VertexFormat.h"

void cSokolRender::DrawSprite(int x1, int y1, int dx, int dy, float u1, float v1, float du, float dv, cTexture* Texture,
                              const sColor4c& ColorMul, float phase, eBlendMode mode) {
    if (!Texture) return;
    VISASSERT(ActiveScene);

    size_t vertices = 4;
    size_t polygons = 2;
    SetupVertexBuffer(vertices, polygons, sVertexXYZDT1::fmt);
    
    int x2=x1+dx,y2=y1+dy;

    bool alpha=ColorMul.a<255 || Texture->IsAlpha();
    if(mode<=ALPHA_TEST && alpha)
        mode=ALPHA_BLEND;
    SetNoMaterial(mode,phase,Texture);

    sVertexXYZDT1* v = static_cast<sVertexXYZDT1*>(LockVertexBuffer(vertexBuffer));
    v = &v[vertices];
    v[0].z=v[1].z=v[2].z=v[3].z=0.001f;
    v[0].w=v[1].w=v[2].w=v[3].w=0.001f;
    v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=ColorMul;
    v[0].x=v[1].x=static_cast<float>(x1); v[0].y=v[2].y=static_cast<float>(y1);
    v[3].x=v[2].x=static_cast<float>(x2); v[1].y=v[3].y=static_cast<float>(y2);

    v[0].u1()=u1;    v[0].v1()=v1;
    v[1].u1()=u1;    v[1].v1()=v1+dv;
    v[2].u1()=u1+du; v[2].v1()=v1;
    v[3].u1()=u1+du; v[3].v1()=v1+dv;
    UnlockVertexBuffer(vertexBuffer);

    sPolygon* polygon=LockIndexBuffer(indexBuffer);
    polygon[polygons+0].set(2*polygons+0,2*polygons+1,2*polygons+2);
    polygon[polygons+1].set(2*polygons+2,2*polygons+1,2*polygons+3);
    UnlockIndexBuffer(indexBuffer);
}

void cSokolRender::DrawSprite2(int x, int y, int dx, int dy, float u, float v, float du, float dv, float u1, float v1,
                               float du1, float dv1, cTexture* Tex1, cTexture* Tex2, const sColor4c& ColorMul,
                               float phase, eColorMode mode, eBlendMode blend_mode) {
    if (!Tex1||!Tex2) return;
    VISASSERT(ActiveScene);
    //TODO
    cEmptyRender::DrawSprite2(x, y, dx, dy, u, v, du, dv, u1, v1, du1, dv1, Tex1, Tex2, ColorMul, phase, mode,
                              blend_mode);
}

void cSokolRender::DrawSprite2(int x, int y, int dx, int dy, float u, float v, float du, float dv, float u1, float v1,
                               float du1, float dv1, cTexture* Tex1, cTexture* Tex2, float lerp_factor, float alpha,
                               float phase, eColorMode mode, eBlendMode blend_mode) {
    if (!Tex1||!Tex2) return;
    VISASSERT(ActiveScene);
    //TODO
    cEmptyRender::DrawSprite2(x, y, dx, dy, u, v, du, dv, u1, v1, du1, dv1, Tex1, Tex2, lerp_factor, alpha, phase, mode,
                              blend_mode);
}
