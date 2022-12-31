#include "StdAfxRD.h"
#include "VertexFormat.h"
#include "IRenderDevice.h"
#include "SokolRender.h"
#include "DrawBuffer.h"
#include "Font.h"

// 2D primitives

void cSokolRender::DrawRectangle(int x1,int y1,int dx,int dy,sColor4c color,bool outline) {
    VISASSERT(ActiveScene);

    int x2=x1+dx,y2=y1+dy;
    if (0 <= dx) { if (x2<0 || x1>ScreenSize.x) return; }
    else if (x1 < 0 || x2 > ScreenSize.x) return;
    if (0 <= dy) { if (y2<0 || y1>ScreenSize.y) return; }
    else if (y1 < 0 || y2 > ScreenSize.y) return;

    SetNoMaterial(ALPHA_BLEND);
    UseOrthographicProjection();

    if (outline) {
        const float w = static_cast<float>(ScreenSize.y * (1.5 / 800.0) / 2.0);
        auto db = GetDrawBuffer(sVertexXYZDT1::fmt);
        sVertexXYZDT1* v;
        indices_t* ib;
        db->Lock<sVertexXYZDT1>(8, 24, v, ib);

        //Outer

        v[0].z=v[1].z=v[2].z=v[3].z=0;
        v[0].diffuse.v=v[1].diffuse.v=v[2].diffuse.v=v[3].diffuse.v=color.ABGR();
        v[0].x=v[1].x=static_cast<float>(x1) - w;
        v[0].y=v[2].y=static_cast<float>(y1) - w;
        v[3].x=v[2].x=static_cast<float>(x2) + w;
        v[1].y=v[3].y=static_cast<float>(y2) + w;

        v[0].u1()=0.0f; v[0].v1()=0.0f;
        v[1].u1()=0.0f; v[1].v1()=1.0f;
        v[2].u1()=1.0f; v[2].v1()=0.0f;
        v[3].u1()=1.0f; v[3].v1()=1.0f;

        //Inner

        v[4].z=v[5].z=v[6].z=v[7].z=0;
        v[4].diffuse.v=v[5].diffuse.v=v[6].diffuse.v=v[7].diffuse.v=color.ABGR();
        v[4].x=v[5].x=static_cast<float>(x1) + w;
        v[4].y=v[6].y=static_cast<float>(y1) + w;
        v[7].x=v[6].x=static_cast<float>(x2) - w;
        v[5].y=v[7].y=static_cast<float>(y2) - w;

        v[0].u1()=0.0f; v[0].v1()=0.0f;
        v[1].u1()=0.0f; v[1].v1()=1.0f;
        v[2].u1()=1.0f; v[2].v1()=0.0f;
        v[3].u1()=1.0f; v[3].v1()=1.0f;

        /* Join the dots, layout:
         * 
         *   0----------2
         *   |\        /|
         *   | 4------6 |
         *   | |      | |
         *   | 5------7 |
         *   |/        \|
         *   1----------3
         */
        
        
        size_t b = db->written_vertices;
        //Top
        ib[0] = b;
        ib[1] = ib[4] = b + 4;
        ib[2] = ib[3] = b + 2;
        ib[5] = b + 6;
        //Left
        ib[6] = b;
        ib[7] = ib[10] = b + 1;
        ib[8] = ib[9] = b + 4;
        ib[11] = b + 5;
        //Right
        ib[12] = b + 6;
        ib[13] = ib[16] = b + 7;
        ib[14] = ib[15] = b + 2;
        ib[17] = b + 3;
        //Bottom
        ib[18] = b + 5;
        ib[19] = ib[22] = b + 1;
        ib[20] = ib[21] = b + 7;
        ib[23] = b + 3;

        db->Unlock();
    } else {
        auto db = GetDrawBuffer(sVertexXYZDT1::fmt);
        sVertexXYZDT1* v = db->LockQuad<sVertexXYZDT1>(1);
        
        v[0].z=v[1].z=v[2].z=v[3].z=0;
        v[0].diffuse.v=v[1].diffuse.v=v[2].diffuse.v=v[3].diffuse.v=color.ABGR();
        v[0].x=v[1].x=static_cast<float>(x1);
        v[0].y=v[2].y=static_cast<float>(y1);
        v[3].x=v[2].x=static_cast<float>(x2);
        v[1].y=v[3].y=static_cast<float>(y2);
        
        v[0].u1()=0.0f; v[0].v1()=0.0f;
        v[1].u1()=0.0f; v[1].v1()=1.0f;
        v[2].u1()=1.0f; v[2].v1()=0.0f;
        v[3].u1()=1.0f; v[3].v1()=1.0f;
        
        db->Unlock();
    }
}

void cSokolRender::FlushPrimitive2D() {
}

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
    
    auto db = GetDrawBuffer(sVertexXYZDT1::fmt);
    sVertexXYZDT1* v = db->LockQuad<sVertexXYZDT1>(1);

    v[0].z=v[1].z=v[2].z=v[3].z=0;
    v[0].diffuse.v=v[1].diffuse.v=v[2].diffuse.v=v[3].diffuse.v=ColorMul.ABGR();
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
    
    auto db = GetDrawBuffer(sVertexXYZDT2::fmt);
    sVertexXYZDT2* v = db->LockQuad<sVertexXYZDT2>(1);
    
    v[0].z=v[1].z=v[2].z=v[3].z=0;
    v[0].diffuse.v=v[1].diffuse.v=v[2].diffuse.v=v[3].diffuse.v=ColorMul.ABGR();
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

    sColor4c diffuse(sColor4f(1.0f, alpha, lerp_factor, (1.0f - lerp_factor)));
    if (blend_mode == ALPHA_NONE && alpha < 1.0f) {
        blend_mode = ALPHA_BLEND;
    }
    
    SetNoMaterial(blend_mode, phase, Tex1, Tex2, mode);
    UseOrthographicProjection();
    SetFSUniformMode(PERIMETER_SOKOL_FS_MODE_MOD_COLOR_ADD_ALPHA); //Use special color mixing

    auto db = GetDrawBuffer(sVertexXYZDT2::fmt);
    sVertexXYZDT2* v = db->LockQuad<sVertexXYZDT2>(1);
    
    v[0].z=v[1].z=v[2].z=v[3].z=0;
    v[0].diffuse.v=v[1].diffuse.v=v[2].diffuse.v=v[3].diffuse.v=diffuse.ABGR();
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

// Text render

void cSokolRender::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode) {
    if (!CurrentFont) {
        VISASSERT(0 && "Font not set");
        return;
    }

    sColor4c diffuse(color);
    cFontInternal* cf=CurrentFont->GetInternal();

    float xOfs;
    float yOfs = static_cast<float>(y);
    float xSize = CurrentFont->GetScale().x*static_cast<float>(cf->GetTexture()->GetWidth());
    float ySize = CurrentFont->GetScale().y*cf->FontHeight*static_cast<float>(cf->GetTexture()->GetHeight());
    float v_add = static_cast<float>(cf->FontHeight + 1.0 / static_cast<double>(cf->GetTexture()->GetHeight()));
    
    if (CurrentFont->GetScale().x>1.01f || CurrentFont->GetScale().y>1.01f) {
        SetRenderState(RS_BILINEAR, 1);
    } else {
        SetRenderState(RS_BILINEAR, 0);
    }

    SetNoMaterial(blend_mode, 0, CurrentFont->GetTexture());
    UseOrthographicProjection();

    auto db = GetDrawBuffer(sVertexXYZDT1::fmt);
    for (const char* str=string; 0 != *str; str++, yOfs += ySize) {
        xOfs = static_cast<float>(x);
        if (0 <= align) {
            float StringWidth = GetFontLength(str);
            xOfs -= static_cast<float>(xm::round(StringWidth * (align == 0 ? 0.5f : 1)));
        }
        for (; *str!=10; str++) {
            ChangeTextColor(str, diffuse);
            if (*str==10) break;

            uint8_t c = *str;
            if (!c) goto out;
            if (c<32) {
                continue;
            }
            Vect3f& size=cf->Font[c];
            sVertexXYZDT1* v = db->LockQuad<sVertexXYZDT1>(1);
            
            v[0].z=v[1].z=v[2].z=v[3].z=0;
            v[0].diffuse.v=v[1].diffuse.v=v[2].diffuse.v=v[3].diffuse.v=diffuse.ABGR();
            v[0].x=v[1].x=static_cast<float>(xOfs);
            v[0].y=v[2].y=static_cast<float>(yOfs);
            xOfs+=xSize*size.z-1;
            v[3].x=v[2].x=static_cast<float>(xOfs);
            v[1].y=v[3].y=static_cast<float>(yOfs+ySize);

            v[0].u1()=size.x;        v[0].v1()=size.y;
            v[1].u1()=size.x;        v[1].v1()=size.y+v_add;
            v[2].u1()=size.x+size.z; v[2].v1()=size.y;
            v[3].u1()=size.x+size.z; v[3].v1()=size.y+v_add;

            db->Unlock();
        }
    }
    out:
    return;
}

void cSokolRender::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
                         cTexture* pTexture,eColorMode mode,Vect2f uv,Vect2f duv,float phase,float lerp_factor) {
    if (!CurrentFont) {
        VISASSERT(0 && "Font not set");
        return;
    }
    //TODO
    OutText(x, y, string, color, align, blend_mode);

    //TODO place this after prepare buff
    //SetFSUniformMode(PERIMETER_SOKOL_FS_MODE_MOD_COLOR_ADD_ALPHA); //Use special color mixing
    /*

    duv.x *= 1024.0f / GetSizeX();
    duv.y *= 768.0f / GetSizeY();
    float xOfs = (float) x, yOfs = (float) y;
    sColor4c diffuse(color);
    sColor4c lerp(255 * lerp_factor, 255 * lerp_factor, 255 * lerp_factor, 255 * (1 - lerp_factor));
    cFontInternal* cf = CurrentFont->GetInternal();

    if (!isOrthoSet) UseOrthographic();

//	SetNoMaterial(blend_mode,phase,CurrentFont->GetTexture(),pTexture,mode);
    SetNoMaterial(blend_mode, phase, pTexture, CurrentFont->GetTexture(), mode);
//	SetNoMaterial(blend_mode,phase,pTexture);

    uint32_t index1 = GetTextureStageState(1, D3DTSS_TEXCOORDINDEX);
    SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 1);
    SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
    SetRenderState(RS_BILINEAR, 0);

    {
        SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATECOLOR_ADDALPHA);
        SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
        SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
        SetRenderState(D3DRS_TEXTUREFACTOR, lerp.ARGB());

        SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

//		SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_MODULATE);
        SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
        SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
//	   DWORD dwNumPasses;
//	   RDCALL(lpD3DDevice->ValidateDevice( &dwNumPasses ));
    }


    float xSize = CurrentFont->GetScale().x * cf->GetTexture()->GetWidth(),
            ySize = CurrentFont->GetScale().y * cf->FontHeight * cf->GetTexture()->GetHeight();

    float v_add = cf->FontHeight + 1 / (double) (1 << cf->GetTexture()->GetY());

    QuadBufferXYZDT2.BeginDraw();

    for (const char* str = string; *str; str++, yOfs += ySize) {
        xOfs = (float) x;
        if (align >= 0) {
            float StringWidth = GetFontLength(str);
            if (align == 0)
                xOfs -= xm::round(StringWidth * 0.5f);
            else
                xOfs -= xm::round(StringWidth);
        }
        for (; *str != 10; str++) {
            ChangeTextColor(str, diffuse);

            int c = (unsigned char) *str;
            if (!c) goto LABEL_DRAW;
            if (c == 10)break;
            if (c < 32)continue;
            Vect3f& size = cf->Font[c];

            sVertexXYZDT2* v = QuadBufferXYZDT2.Get();
            sVertexXYZDT2& v1 = v[1], & v2 = v[0],
                    & v3 = v[2], & v4 = v[3];

            float x0, x1, y0, y1;
            x0 = xOfs;
            x1 = xOfs + xSize * size.z - 1;
            y0 = yOfs;
            y1 = yOfs + ySize;
            v1.x = v4.x = x0;
            v1.y = v2.y = y0;
            v3.x = v2.x = x1;
            v3.y = v4.y = y1;
            v1.u1() = v4.u1() = size.x;
            v1.v1() = v2.v1() = size.y;
            v3.u1() = v2.u1() = v1.u1() + size.z;
            v3.v1() = v4.v1() = v1.v1() + v_add;
            v1.diffuse = v2.diffuse = v3.diffuse = v4.diffuse = diffuse;
            v1.z = v2.z = v3.z = v4.z = 0.001f;

            v1.u2() = v4.u2() = (x0 - x) * duv.x + uv.x;
            v1.v2() = v2.v2() = (y0 - y) * duv.y + uv.y;
            v3.u2() = v2.u2() = (x1 - x) * duv.x + uv.x;
            v3.v2() = v4.v2() = (y1 - y) * duv.y + uv.y;

            xOfs = x1;
        }
    }
    LABEL_DRAW:

    QuadBufferXYZDT2.EndDraw();
    */
}

