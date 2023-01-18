#include "StdAfxRD.h"
#include "xmath.h"
#include "VertexFormat.h"
#include "IRenderDevice.h"
#include "SokolRender.h"
#include "DrawBuffer.h"
#include "Font.h"

// Sprites / 2d textures

void cSokolRender::DrawSprite(int x1, int y1, int dx, int dy, float u1, float v1, float du, float dv, cTexture* Texture,
                              const sColor4c& ColorMul, float phase, eBlendMode mode) {
    if (!Texture) return;
    VISASSERT(ActiveScene);

    int x2=x1+dx,y2=y1+dy;
    if (0 <= dx) { if (x2<0 || x1>ScreenSize.x) return; }
    else if (x1 < 0 || x2 > ScreenSize.x) return;
    if (0 <= dy) { if (y2<0 || y1>ScreenSize.y) return; }
    else if (y1 < 0 || y2 > ScreenSize.y) return;

    bool alpha = ColorMul.a < 255 || Texture->IsAlpha();
    if (mode <= ALPHA_TEST && alpha) mode = ALPHA_BLEND;

    SetNoMaterial(mode,phase,Texture);
    UseOrthographicProjection();
    
    auto db = GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLES);
    sVertexXYZDT1* v = db->LockQuad<sVertexXYZDT1>(1);

    v[0].z=v[1].z=v[2].z=v[3].z=0;
    v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=gb_RenderDevice->ConvertColor(ColorMul);
    v[0].x=v[1].x=static_cast<float>(x1);
    v[0].y=v[2].y=static_cast<float>(y1);
    v[3].x=v[2].x=static_cast<float>(x2);
    v[1].y=v[3].y=static_cast<float>(y2);

    v[0].u1()=u1;    v[0].v1()=v1;
    v[1].u1()=u1;    v[1].v1()=v1+dv;
    v[2].u1()=u1+du; v[2].v1()=v1;
    v[3].u1()=u1+du; v[3].v1()=v1+dv;
    
    db->Unlock();
}

void cSokolRender::DrawSprite2(int x1,int y1,int dx,int dy,
                               float u0,float v0,float du0,float dv0,
                               float u1,float v1,float du1,float dv1,
                               cTexture *Tex1,cTexture *Tex2,const sColor4c &ColorMul,float phase,
                               eColorMode mode,eBlendMode blend_mode) {
    if (!Tex1||!Tex2) return;
    VISASSERT(ActiveScene);

    int x2=x1+dx,y2=y1+dy;
    if (0 <= dx) { if (x2<0 || x1>ScreenSize.x) return; }
    else if (x1 < 0 || x2 > ScreenSize.x) return;
    if (0 <= dy) { if (y2<0 || y1>ScreenSize.y) return; }
    else if (y1 < 0 || y2 > ScreenSize.y) return;

    if (blend_mode == ALPHA_NONE && ColorMul.a < 255) {
        blend_mode = ALPHA_BLEND;
    }
    
    SetNoMaterial(blend_mode, phase, Tex1, Tex2, mode);
    UseOrthographicProjection();
    
    auto db = GetDrawBuffer(sVertexXYZDT2::fmt, PT_TRIANGLES);
    sVertexXYZDT2* v = db->LockQuad<sVertexXYZDT2>(1);
    
    v[0].z=v[1].z=v[2].z=v[3].z=0;
    v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=gb_RenderDevice->ConvertColor(ColorMul);
    v[0].x=v[1].x=static_cast<float>(x1);
    v[0].y=v[2].y=static_cast<float>(y1);
    v[3].x=v[2].x=static_cast<float>(x2);
    v[1].y=v[3].y=static_cast<float>(y2);

    v[0].u1()=u0;     v[0].v1()=v0;
    v[1].u1()=u0;     v[1].v1()=v0+dv0;
    v[2].u1()=u0+du0; v[2].v1()=v0;
    v[3].u1()=u0+du0; v[3].v1()=v0+dv0;

    v[0].u2()=u1;     v[0].v2()=v1;
    v[1].u2()=u1;     v[1].v2()=v1+dv1;
    v[2].u2()=u1+du1; v[2].v2()=v1;
    v[3].u2()=u1+du1; v[3].v2()=v1+dv1;
    
    db->Unlock();
}

void cSokolRender::DrawSprite2(int x1,int y1,int dx,int dy,
                               float u0,float v0,float du0,float dv0,
                               float u1,float v1,float du1,float dv1,
                               cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha,float phase,
                               eColorMode mode,eBlendMode blend_mode) {
    if (!Tex1||!Tex2) return;
    VISASSERT(ActiveScene);

    int x2=x1+dx,y2=y1+dy;
    if (0 <= dx) { if (x2<0 || x1>ScreenSize.x) return; }
    else if (x1 < 0 || x2 > ScreenSize.x) return;
    if (0 <= dy) { if (y2<0 || y1>ScreenSize.y) return; }
    else if (y1 < 0 || y2 > ScreenSize.y) return;

    sColor4c diffuse(sColor4f(1.0f, 1.0f, 1.0f, alpha));
    if (blend_mode == ALPHA_NONE && alpha < 1.0f) {
        blend_mode = ALPHA_BLEND;
    }

    int extra_mode = (static_cast<int>(lerp_factor * 255) << 8) | PERIMETER_SOKOL_COLOR_MODE_MOD_COLOR_ADD_ALPHA;
    SetNoMaterial(blend_mode, phase, Tex2, Tex1,  static_cast<eColorMode>(extra_mode | mode));
    UseOrthographicProjection();

    auto db = GetDrawBuffer(sVertexXYZDT2::fmt, PT_TRIANGLES);
    sVertexXYZDT2* v = db->LockQuad<sVertexXYZDT2>(1);
    
    v[0].z=v[1].z=v[2].z=v[3].z=0;
    v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=gb_RenderDevice->ConvertColor(diffuse);
    v[0].x=v[1].x=static_cast<float>(x1);
    v[0].y=v[2].y=static_cast<float>(y1);
    v[3].x=v[2].x=static_cast<float>(x2);
    v[1].y=v[3].y=static_cast<float>(y2);

    //UV swapping is intentional as Tex2 is provided in tex 0 and Tex1 in tex 1 
    v[0].u2()=u0;     v[0].v2()=v0;
    v[1].u2()=u0;     v[1].v2()=v0+dv0;
    v[2].u2()=u0+du0; v[2].v2()=v0;
    v[3].u2()=u0+du0; v[3].v2()=v0+dv0;

    v[0].u1()=u1;     v[0].v1()=v1;
    v[1].u1()=u1;     v[1].v1()=v1+dv1;
    v[2].u1()=u1+du1; v[2].v1()=v1;
    v[3].u1()=u1+du1; v[3].v1()=v1+dv1;
    
    db->Unlock();
}

// Text render

void cSokolRender::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode) {    
    SetNoMaterial(blend_mode, 0, CurrentFont->GetTexture());

    cInterfaceRenderDevice::OutText(x, y, string, color, align, blend_mode);
}

void cSokolRender::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
                         cTexture* pTexture,eColorMode mode,Vect2f uv,Vect2f duv,float phase,float lerp_factor) {
    
    int extra_mode = (static_cast<int>(lerp_factor * 255) << 8) | PERIMETER_SOKOL_COLOR_MODE_MOD_COLOR_ADD_ALPHA;
    mode = static_cast<eColorMode>(extra_mode | mode);
    
    SetNoMaterial(blend_mode, phase, pTexture, CurrentFont->GetTexture(), mode);

    cInterfaceRenderDevice::OutText(x, y, string, color, align, blend_mode, pTexture, mode, uv, duv, phase, lerp_factor);
}

