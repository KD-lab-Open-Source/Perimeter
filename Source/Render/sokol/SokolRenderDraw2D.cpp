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
        auto db = GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLESTRIP);
        //db->Draw();
        sVertexXYZDT1* v = nullptr;
        indices_t* ib = nullptr;
        db->Lock<sVertexXYZDT1>(8, 12, v, ib, true);

        v[0].u1() = v[4].u1() = 0.0f; v[0].v1() = v[4].v1() =0.0f;
        v[1].u1() = v[5].u1() = 0.0f; v[1].v1() = v[5].v1() =1.0f;
        v[2].u1() = v[6].u1() = 1.0f; v[2].v1() = v[6].v1() =0.0f;
        v[3].u1() = v[7].u1() = 1.0f; v[3].v1() = v[7].v1() =1.0f;

        //Outer

        v[0].z=v[1].z=v[2].z=v[3].z=0;
        v[0].diffuse.v=v[1].diffuse.v=v[2].diffuse.v=v[3].diffuse.v=color.ABGR();
        v[0].x=v[1].x=static_cast<float>(x1) - w;
        v[0].y=v[2].y=static_cast<float>(y1) - w;
        v[3].x=v[2].x=static_cast<float>(x2) + w;
        v[1].y=v[3].y=static_cast<float>(y2) + w;

        //Inner

        v[4].z=v[5].z=v[6].z=v[7].z=0;
        v[4].diffuse.v=v[5].diffuse.v=v[6].diffuse.v=v[7].diffuse.v=color.ABGR();
        v[4].x=v[5].x=static_cast<float>(x1) + w;
        v[4].y=v[6].y=static_cast<float>(y1) + w;
        v[7].x=v[6].x=static_cast<float>(x2) - w;
        v[5].y=v[7].y=static_cast<float>(y2) - w;

        /* Join the dots using triangle strip, layout:
         * 
         *   0---------2
         *   |\   /   /|
         *   | 4-----6 |
         *   | |     | |
         *   |\|     |\|
         *   | |     | |
         *   | 5-----7 |
         *   |/   /   \|
         *   1---------3
         */
        
        
        size_t b = db->written_vertices;
        //Start degenerate triangle
        ib[0] = b;
        //Top
        ib[1] = b;
        ib[2] = b + 4;
        ib[3] = b + 2;
        ib[4] = b + 6;
        //Left
        ib[5] = b + 3;
        ib[6] = b + 7;
        //Right
        ib[7] = b + 1;
        ib[8] = b + 5;
        //Bottom
        ib[9] = b;
        ib[10] = b + 4;
        //End degenerate triangle
        ib[11] = b + 4;

        db->Unlock();
    } else {
        auto db = GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLES);
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
    
    auto db = GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLES);
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
    
    auto db = GetDrawBuffer(sVertexXYZDT2::fmt, PT_TRIANGLES);
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
    v[0].diffuse.v=v[1].diffuse.v=v[2].diffuse.v=v[3].diffuse.v=diffuse.ABGR();
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
    
    SetNoMaterial(blend_mode, 0, CurrentFont->GetTexture());
    UseOrthographicProjection();
    
    indices_t* i = nullptr;
    sVertexXYZDT1* v = nullptr;
    auto db = GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLES);
    for (const char* str = string; 0 != *str; str++, yOfs += ySize) {
        xOfs = static_cast<float>(x);
        size_t chars;
        float StringWidth = GetFontLength(str, &chars);
        if (0 <= align) {
            xOfs -= static_cast<float>(xm::round(StringWidth * (align == 0 ? 0.5f : 1)));
        }
        for (; *str != 10; str++) {
            ChangeTextColor(str, diffuse);
            uint8_t c = *str;
            if (!c || c == 10) break;
            if (c < 32) continue;

            db->AutoLockQuad<sVertexXYZDT1>(std::min(chars, static_cast<size_t>(10)), 1, v, i);
            
            Vect3f& size = cf->Font[c];

            v[0].z = v[1].z = v[2].z = v[3].z = 0;
            v[0].diffuse.v = v[1].diffuse.v = v[2].diffuse.v = v[3].diffuse.v = diffuse.ABGR();
            v[0].x = v[1].x = static_cast<float>(xOfs);
            v[0].y = v[2].y = static_cast<float>(yOfs);
            xOfs += xSize * size.z - 1;
            v[3].x = v[2].x = static_cast<float>(xOfs);
            v[1].y = v[3].y = static_cast<float>(yOfs + ySize);

            v[0].u1() = size.x;
            v[0].v1() = size.y;
            v[1].u1() = size.x;
            v[1].v1() = size.y + v_add;
            v[2].u1() = size.x + size.z;
            v[2].v1() = size.y;
            v[3].u1() = size.x + size.z;
            v[3].v1() = size.y + v_add;
        }
        db->AutoUnlock();
        if (*str == 0) break;
    }
}

void cSokolRender::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
                         cTexture* pTexture,eColorMode mode,Vect2f uv,Vect2f duv,float phase,float lerp_factor) {
    if (!CurrentFont) {
        VISASSERT(0 && "Font not set");
        return;
    }

    duv.x *= 1024.0f / static_cast<float>(GetSizeX());
    duv.y *= 768.0f / static_cast<float>(GetSizeY());
    sColor4c diffuse(color);
    cFontInternal* cf=CurrentFont->GetInternal();

    float xOfs;
    float yOfs = static_cast<float>(y);
    float xSize = CurrentFont->GetScale().x*static_cast<float>(cf->GetTexture()->GetWidth());
    float ySize = CurrentFont->GetScale().y*cf->FontHeight*static_cast<float>(cf->GetTexture()->GetHeight());
    float v_add = static_cast<float>(cf->FontHeight + 1.0 / static_cast<double>(cf->GetTexture()->GetHeight()));

    int extra_mode = (static_cast<int>(lerp_factor * 255) << 8) | PERIMETER_SOKOL_COLOR_MODE_MOD_COLOR_ADD_ALPHA;
    SetNoMaterial(blend_mode, phase, pTexture, CurrentFont->GetTexture(), static_cast<eColorMode>(extra_mode | mode));
    UseOrthographicProjection();

    indices_t* i = nullptr;
    sVertexXYZDT2* v = nullptr;
    auto db = GetDrawBuffer(sVertexXYZDT2::fmt, PT_TRIANGLES);
    for (const char* str=string; 0 != *str; str++, yOfs += ySize) {
        xOfs = static_cast<float>(x);
        size_t chars;
        float StringWidth = GetFontLength(str, &chars);
        if (0 <= align) {
            xOfs -= static_cast<float>(xm::round(StringWidth * (align == 0 ? 0.5f : 1)));
        }
        for (; *str!=10; str++) {
            ChangeTextColor(str, diffuse);
            uint8_t c = *str;
            if (!c || c == 10) break;
            if (c < 32) continue;

            Vect3f& size = cf->Font[c];

            db->AutoLockQuad<sVertexXYZDT2>(std::min(chars, static_cast<size_t>(10)), 1, v, i);

            float x0, x1, y0, y1;
            x0 = xOfs;
            x1 = xOfs + xSize * size.z - 1;
            y0 = yOfs;
            y1 = yOfs + ySize;
            v[1].x = v[3].x = x0;
            v[1].y = v[0].y = y0;
            v[2].x = v[0].x = x1;
            v[2].y = v[3].y = y1;

            v[0].z=v[1].z=v[2].z=v[3].z=0;
            v[0].diffuse.v=v[1].diffuse.v=v[2].diffuse.v=v[3].diffuse.v=diffuse.ABGR();            
            
            v[1].u2() = v[3].u2() = size.x;
            v[1].v2() = v[0].v2() = size.y;
            v[2].u2() = v[0].u2() = v[1].u2() + size.z;
            v[2].v2() = v[3].v2() = v[1].v2() + v_add;

            v[1].u1() = v[3].u1() = (x0 - x) * duv.x + uv.x;
            v[1].v1() = v[0].v1() = (y0 - y) * duv.y + uv.y;
            v[2].u1() = v[0].u1() = (x1 - x) * duv.x + uv.x;
            v[2].v1() = v[3].v1() = (y1 - y) * duv.y + uv.y;

            xOfs = x1;
        }
        db->AutoUnlock();
        if (*str == 0) break;
    }
}

