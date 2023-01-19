#include "StdAfxRD.h"
#include "xmath.h"
#include "VertexFormat.h"
#include "IRenderDevice.h"
#include "SokolRender.h"
#include "DrawBuffer.h"
#include "Font.h"

// Sprites / 2d textures

void cSokolRender::DrawSprite2(int x1,int y1,int dx,int dy,
                               float u0,float v0,float du0,float dv0,
                               float u1,float v1,float du1,float dv1,
                               cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha,float phase,
                               eColorMode mode,eBlendMode blend_mode) {
    if (!Tex1||!Tex2) return;
    VISASSERT(ActiveScene);
    
    if (blend_mode == ALPHA_NONE && alpha < 1.0f) {
        blend_mode = ALPHA_BLEND;
    }

    int extra_mode = (static_cast<int>(lerp_factor * 255) << 8) | PERIMETER_SOKOL_COLOR_MODE_MOD_COLOR_ADD_ALPHA;
    SetNoMaterial(blend_mode, phase, Tex2, Tex1,  static_cast<eColorMode>(extra_mode | mode));
    
    cInterfaceRenderDevice::DrawSprite2(x1, y1, dx, dy, u0, v0, du0, dv0, u1, v1, du1, dv1, Tex1, Tex2, lerp_factor, alpha, phase, mode, blend_mode);
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

