#include "StdAfxRD.h"
#include "xmath.h"
#include "VertexFormat.h"
#include "IRenderDevice.h"
#include "SokolRender.h"
#include "DrawBuffer.h"
#include "Font.h"
#include "MeshTri.h"
#include "ObjMesh.h"

void cSokolRender::SetNoMaterial(eBlendMode blend, float Phase, cTexture* Texture0, cTexture* Texture1,
                                 eColorMode color_mode) {
    SokolTexture2D* tex0 = emptyTexture;
    SokolTexture2D* tex1 = nullptr;
    if (Texture0) {
        if (blend == ALPHA_NONE && Texture0->IsAlphaTest()) {
            blend = ALPHA_TEST;
        }
        if (blend <= ALPHA_TEST && Texture0->IsAlpha()) {
            blend = ALPHA_BLEND;
        }
    }

    SetColorMode(color_mode);
    SetBlendState(blend);
    SetTextures(Phase, Texture0, Texture1);
}

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
    
    SetTex2Lerp(lerp_factor);
    SetNoMaterial(blend_mode, phase, Tex2, Tex1, mode);
    
    cInterfaceRenderDevice::DrawSprite2(x1, y1, dx, dy, u0, v0, du0, dv0, u1, v1, du1, dv1, Tex1, Tex2, lerp_factor, alpha, phase, mode, blend_mode);
    SetTex2Lerp(-1);
}

// Text render

void cSokolRender::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode) {    
    SetNoMaterial(blend_mode, 0, CurrentFont->GetTexture());

    cInterfaceRenderDevice::OutText(x, y, string, color, align, blend_mode);
}

void cSokolRender::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
                         cTexture* pTexture,eColorMode mode,Vect2f uv,Vect2f duv,float phase,float lerp_factor) {
    SetTex2Lerp(lerp_factor);
    SetNoMaterial(blend_mode, phase, pTexture, CurrentFont->GetTexture(), mode);
    
    cInterfaceRenderDevice::OutText(x, y, string, color, align, blend_mode, pTexture, mode, uv, duv, phase, lerp_factor);
    SetTex2Lerp(-1);
}

// Mesh draw

void cSokolRender::BeginDrawMesh(bool obj_mesh, bool use_shadow) {
}

void cSokolRender::EndDrawMesh() {
}

void cSokolRender::SetSimplyMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) {
}

void cSokolRender::DrawNoMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) {
}

void cSokolRender::BeginDrawShadow(bool shadow_map) {
}

void cSokolRender::EndDrawShadow() {
}

void cSokolRender::SetSimplyMaterialShadow(cObjMesh* mesh, cTexture* texture) {
}

void cSokolRender::DrawNoMaterialShadow(cObjMesh* mesh) {
}
