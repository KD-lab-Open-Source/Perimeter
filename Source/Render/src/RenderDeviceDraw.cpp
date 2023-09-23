#include "StdAfxRD.h"
#include "Font.h"
#include "IRenderDevice.h"
#include "DrawBuffer.h"
#include "Silicon.h"
#include "Scene.h"

// 2D Sprite rendering functions

void cInterfaceRenderDevice::DrawSprite(int x1, int y1, int dx, int dy, float u1, float v1, float du, float dv, cTexture* Texture,
                                        const sColor4c& ColorMul, float phase, eBlendMode mode) {
    if (!Texture) return;

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

void cInterfaceRenderDevice::DrawSprite2(int x, int y, int dx, int dy, float u, float v, float du, float dv,
                                         cTexture *Tex1, cTexture *Tex2, const sColor4c &ColorMul, float phase)
{
    DrawSprite2(x, y, dx, dy,
                u, v, du, dv,
                u, v, du, dv,
                Tex1, Tex2, ColorMul, phase,
                COLOR_MOD, ALPHA_NONE);
}

void cInterfaceRenderDevice::DrawSprite2(int x1,int y1,int dx,int dy,
                                         float u0,float v0,float du0,float dv0,
                                         float u1,float v1,float du1,float dv1,
                                         cTexture *Tex1,cTexture *Tex2,const sColor4c &ColorMul,float phase,
                                         eColorMode mode,eBlendMode blend_mode) {
    if (!Tex1||!Tex2) return;

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

void cInterfaceRenderDevice::DrawSprite2(int x1,int y1,int dx,int dy,
                                         float u0,float v0,float du0,float dv0,
                                         float u1,float v1,float du1,float dv1,
                                         cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha,float phase,
                                         eColorMode mode,eBlendMode blend_mode) {
    if (!Tex1||!Tex2) return;

    int x2=x1+dx,y2=y1+dy;
    if (0 <= dx) { if (x2<0 || x1>ScreenSize.x) return; }
    else if (x1 < 0 || x2 > ScreenSize.x) return;
    if (0 <= dy) { if (y2<0 || y1>ScreenSize.y) return; }
    else if (y1 < 0 || y2 > ScreenSize.y) return;

    sColor4c diffuse(sColor4f(1.0f, 1.0f, 1.0f, alpha));

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

// Text rendering functions 

void cInterfaceRenderDevice::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode) {
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
            v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=ConvertColor(diffuse);
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

void cInterfaceRenderDevice::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
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
            v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=gb_RenderDevice->ConvertColor(diffuse);

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

// 2D primitives

float cInterfaceRenderDevice::getThinLineWidth() const {
    return static_cast<float>(ScreenSize.y * (1.0 / 768.0) / 2.0);
}

void cInterfaceRenderDevice::DrawLine(int x1,int y1,int x2,int y2,const sColor4c& color, float width) {    
    if (x1 <= x2) { if (x2<0 || x1>ScreenSize.x) return; }
    else if (x1 < 0 || x2 > ScreenSize.x) return;
    if (y1 <= y2) { if (y2<0 || y1>ScreenSize.y) return; }
    else if (y1 < 0 || y2 > ScreenSize.y) return;

    SetNoMaterial(ALPHA_BLEND);
    UseOrthographicProjection();
    
    // -o = outer   -i = inner
    float tf = getThinLineWidth();
    float w = width * tf;
    float bo = 2.0f * tf;
    float bi = 0;
    //Check if too thin
    if (w <= bo) {
        bo *= 0.5f;
    } else {
        bi = bo;
    }
    float phi = xm::atan2(static_cast<float>(x2 - x1), static_cast<float>(y2 - y1));
    float vc = xm::cos(phi);
    float vs = xm::sin(phi);
    float vco = vc * (w + bo);
    float vso = vs * (w + bo);
    float vci = vc * (w - bi);
    float vsi = vs * (w - bi);

    auto db = GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLESTRIP);
    sVertexXYZDT1* v = nullptr;
    indices_t* ib = nullptr;
    db->Lock(8, 16, v, ib, true);

    uint32_t diffuse = ConvertColor(color);
    sColor4c fade_color = color;
    fade_color.a = 0;
    //fade_color = sColor4c(0,0,255,255);
    uint32_t fade = ConvertColor(fade_color);
    for (int i = 0; i < 8; ++i) {
        v[i].z = 0;
        v[i].diffuse = i < 4 ? fade : diffuse;
    }

    //         0 ------------- 2
    //         |\      \      /|
    //  START  | 4 --------- 6 |  END
    //  POINT  |/|     \     |/| POINT
    //         | 5 --------- 7 |
    //         |/      \      \|
    //         1 ------------- 3
    //

    size_t b = db->written_vertices;
    size_t i = 0;
    ib[i++] = b + 3;
    ib[i++] = b + 2;
    ib[i++] = b + 7;
    ib[i++] = b + 6;
    ib[i++] = b + 2;
    ib[i++] = b; //0
    ib[i++] = b + 6;
    ib[i++] = b + 4;
    ib[i++] = b + 7;
    ib[i++] = b + 5;
    ib[i++] = b + 3;
    ib[i++] = b + 1;
    ib[i++] = b + 5;
    ib[i++] = b + 4;
    ib[i++] = b + 1;
    ib[i++] = b + 0;
    xassert(i == db->locked_indices);

    //Out
    v[0].x = static_cast<float>(x1 + vco - vso);
    v[0].y = static_cast<float>(y1 - vso - vco);
    v[1].x = static_cast<float>(x1 - vco - vso);
    v[1].y = static_cast<float>(y1 + vso - vco);
    v[2].x = static_cast<float>(x2 + vco + vso);
    v[2].y = static_cast<float>(y2 - vso + vco);
    v[3].x = static_cast<float>(x2 - vco + vso);
    v[3].y = static_cast<float>(y2 + vso + vco);

    //In
    v[4].x = static_cast<float>(x1 + vci - vsi);
    v[4].y = static_cast<float>(y1 - vsi - vci);
    v[5].x = static_cast<float>(x1 - vci - vsi);
    v[5].y = static_cast<float>(y1 + vsi - vci);
    v[6].x = static_cast<float>(x2 + vci + vsi);
    v[6].y = static_cast<float>(y2 - vsi + vci);
    v[7].x = static_cast<float>(x2 - vci + vsi);
    v[7].y = static_cast<float>(y2 + vsi + vci);

    v[0].u1() = v[4].u1() = 0.0f; v[0].v1() = v[4].v1() = 0.0f;
    v[1].u1() = v[5].u1() = 0.0f; v[1].v1() = v[5].v1() = 1.0f;
    v[2].u1() = v[6].u1() = 1.0f; v[2].v1() = v[6].v1() = 0.0f;
    v[3].u1() = v[7].u1() = 1.0f; v[3].v1() = v[7].v1() = 1.0f;

    db->Unlock();
    db->EndTriangleStrip();
}

void cInterfaceRenderDevice::DrawRectangle(int x1,int y1,int dx,int dy,const sColor4c& color, float outline) {
    int x2=x1+dx,y2=y1+dy;
    if (0 <= dx) { if (x2<0 || x1>ScreenSize.x) return; }
    else if (x1 < 0 || x2 > ScreenSize.x) return;
    if (0 <= dy) { if (y2<0 || y1>ScreenSize.y) return; }
    else if (y1 < 0 || y2 > ScreenSize.y) return;

    SetNoMaterial(ALPHA_BLEND);
    UseOrthographicProjection();
    uint32_t diffuse = ConvertColor(color);

    if (0 < outline) {
        float w = outline * getThinLineWidth();
        auto db = GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLESTRIP);
        sVertexXYZDT1* v = nullptr;
        indices_t* ib = nullptr;
        db->Lock<sVertexXYZDT1>(8, 10, v, ib, true);

        v[0].u1() = v[4].u1() = 0.0f; v[0].v1() = v[4].v1() = 0.0f;
        v[1].u1() = v[5].u1() = 0.0f; v[1].v1() = v[5].v1() = 1.0f;
        v[2].u1() = v[6].u1() = 1.0f; v[2].v1() = v[6].v1() = 0.0f;
        v[3].u1() = v[7].u1() = 1.0f; v[3].v1() = v[7].v1() = 1.0f;

        //Outer

        v[0].z=v[1].z=v[2].z=v[3].z=0;
        v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=diffuse;
        v[0].x=v[1].x=static_cast<float>(x1) - w;
        v[0].y=v[2].y=static_cast<float>(y1) - w;
        v[3].x=v[2].x=static_cast<float>(x2) + w;
        v[1].y=v[3].y=static_cast<float>(y2) + w;

        //Inner

        v[4].z=v[5].z=v[6].z=v[7].z=0;
        v[4].diffuse=v[5].diffuse=v[6].diffuse=v[7].diffuse=diffuse;
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
        //Top
        ib[0] = b;
        ib[1] = b + 4;
        ib[2] = b + 2;
        ib[3] = b + 6;
        //Left
        ib[4] = b + 3;
        ib[5] = b + 7;
        //Right
        ib[6] = b + 1;
        ib[7] = b + 5;
        //Bottom
        ib[8] = b;
        ib[9] = b + 4;

        db->Unlock();
        db->EndTriangleStrip();
    } else {
        auto db = GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLES);
        sVertexXYZDT1* v = db->LockQuad<sVertexXYZDT1>(1);

        v[0].z=v[1].z=v[2].z=v[3].z=0;
        v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=diffuse;
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

void cInterfaceRenderDevice::DrawPixel(int x1, int y1, const sColor4c& color) {
    DrawRectangle(x1, y1, 1, 1, color, 0);
}

void cInterfaceRenderDevice::FlushPrimitive2D() {
}

// 3D primitives

void cInterfaceRenderDevice::DrawBound(const MatXf &Matrix, const Vect3f &min, const Vect3f &max, bool wireframe, const sColor4c &Color) {
    VISASSERT(DrawNode);
    
    uint32_t zwrite = GetRenderState(RS_ZWRITEENABLE);
    SetRenderState(RS_ZWRITEENABLE, 1);
    if (wireframe && !WireframeMode) SetRenderState(RS_WIREFRAME, 1);
    SetWorldMatXf(Matrix);
    SetNoMaterial(ALPHA_BLEND);

    uint32_t diffuse = ConvertColor(sColor4c((150*Color.r)>>8,(155*Color.g)>>8,(155*Color.b)>>8,100));
    DrawBuffer* db = GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLES);
    sPolygon* p = nullptr;
    sVertexXYZDT1* v = nullptr;
    db->Lock(8, 12*sPolygon::PN, v, reinterpret_cast<indices_t*&>(p), true);
    
    v[0].pos.set(min.x,min.y,min.z);
    v[1].pos.set(max.x,min.y,min.z);
    v[2].pos.set(min.x,max.y,min.z);
    v[3].pos.set(max.x,max.y,min.z);
    v[4].pos.set(min.x,min.y,max.z);
    v[5].pos.set(max.x,min.y,max.z);
    v[6].pos.set(min.x,max.y,max.z);
    v[7].pos.set(max.x,max.y,max.z);
    v[0].diffuse=v[1].diffuse=v[2].diffuse=
    v[3].diffuse=v[4].diffuse=v[5].diffuse=
    v[6].diffuse=v[7].diffuse=diffuse;
    p[0].p1=1, p[0].p2=2, p[0].p3=0;
    p[1].p1=1, p[1].p2=3, p[1].p3=2;
    p[2].p1=4, p[2].p2=6, p[2].p3=5;
    p[3].p1=6, p[3].p2=7, p[3].p3=5;
    p[4].p1=0, p[4].p2=6, p[4].p3=4;
    p[5].p1=0, p[5].p2=2, p[5].p3=6;
    p[6].p1=0, p[6].p2=4, p[6].p3=5;
    p[7].p1=0, p[7].p2=5, p[7].p3=1;
    p[8].p1=1, p[8].p2=7, p[8].p3=3;
    p[9].p1=1, p[9].p2=5, p[9].p3=7;
    p[10].p1=2, p[10].p2=3, p[10].p3=7;
    p[11].p1=2, p[11].p2=7, p[11].p3=6;
    
    db->Unlock();
    db->Draw();

    if (wireframe) SetRenderState(RS_WIREFRAME, WireframeMode);
    SetRenderState(RS_ZWRITEENABLE, zwrite);
}

//Do a binary search until we find the closest point to camera
void TraceVisiblePoint(const Mat4f& m, const Vect3f& start, Vect3f& end) {
    Vect3f dir = end - start;
    double step_size = 0.5;
    double pos = step_size;
    const float dist_per_extra_step = 100;
    size_t steps = std::min(10, std::max(2, xm::round(end.distance2(start) / (dist_per_extra_step * dist_per_extra_step * 3))));
    for (int i = 0; i < steps; ++i) {
        Vect3f point = start + (dir * static_cast<float>(pos)); 
        double z = m.xz * point.x + m.yz * point.y + m.zz * point.z + m.wz;
        step_size /= 2.0;
        if (z <= 0) {
            pos -= step_size;
        } else {
            //This is ahead of camera position, save it as last position
            pos += step_size;
            end = point;
        }
    }
}

void cInterfaceRenderDevice::DrawLine(const Vect3f &v1,const Vect3f &v2, const sColor4c& color) {
    VISASSERT(DrawNode);
    if (!DrawNode) {
        return;
    }
    static Vect3f p1v, p1e, p2v, p2e;
    DrawNode->ConvertorWorldToViewPort(&v1, &p1v, &p1e);
    DrawNode->ConvertorWorldToViewPort(&v2, &p2v, &p2e);
    //Only draw if one of points is not behind camera
    if (0 >= p1v.z && 0 >= p2v.z) return;
    
    if (p1v.z <= 0) {
        //P2 is visible, calculate where P1 goes behind camera
        Vect3f v1v = v1;
        TraceVisiblePoint(DrawNode->matViewProjScr, v2, v1v);
        DrawNode->ConvertorWorldToViewPort(&v1v, &p1v, &p1e);
    } else if (p2v.z <= 0) {
        //P1 is visible, calculate where P2 goes behind camera
        Vect3f v2v = v2;
        TraceVisiblePoint(DrawNode->matViewProjScr, v1, v2v);
        DrawNode->ConvertorWorldToViewPort(&v2v, &p2v, &p2e);
    }
    //If still behind camera cancel it
    if (0 >= p1v.z && 0 >= p2v.z) return;
    
    DrawLine(p1e.x, p1e.y, p2e.x, p2e.y, color, 1.5f);
}

void cInterfaceRenderDevice::DrawPoint(const Vect3f &v1, const sColor4c& color) {
    DrawBound(MatXf::ID, v1, v1 + Vect3f(1,1,1), false, color);
}

void cInterfaceRenderDevice::FlushPrimitive3D() {
}

// Other render functions

void cInterfaceRenderDevice::Draw(class cScene *Scene) {
    std::map<cTexture*, std::vector<cUnkLight*>> lightsByTex;
    for(int i=0;i<Scene->GetNumberLight();i++) {
        cUnkLight* ULight=Scene->GetLight(i);
        if (ULight && ULight->GetAttr(ATTRLIGHT_SPHERICAL)) {
            cTexture* pTexture=ULight->GetTexture();
            auto& lights = lightsByTex[pTexture];
            lights.emplace_back(ULight);
        }
    }
    
    if (lightsByTex.empty()) {
        return;
    }

    DrawBuffer* db = GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLES);

    for (auto& pair : lightsByTex) {
        SetNoMaterial(ALPHA_ADDBLEND, 0, pair.first);
        size_t locked = std::min(pair.second.size(), static_cast<size_t>(50));
        
        indices_t* ib = nullptr;
        sVertexXYZDT1 *v = nullptr;
        
        for (cUnkLight* ULight : pair.second) {
            sColor4c Diffuse(ULight->GetDiffuse());
            db->AutoLockQuad(locked, 1, v, ib);
            Vect3f& p=ULight->GetPos();
            float r=ULight->GetRadius();
            v[0].pos.x=p.x-r; v[0].pos.y=p.y-r; v[0].pos.z=p.z; v[0].u1()=0; v[0].v1()=0;
            v[1].pos.x=p.x-r; v[1].pos.y=p.y+r; v[1].pos.z=p.z; v[1].u1()=0; v[1].v1()=1;
            v[2].pos.x=p.x+r; v[2].pos.y=p.y-r; v[2].pos.z=p.z; v[2].u1()=1; v[2].v1()=0;
            v[3].pos.x=p.x+r; v[3].pos.y=p.y+r; v[3].pos.z=p.z; v[3].u1()=1; v[3].v1()=1;

            v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=ConvertColor(Diffuse);
        }
    }
    db->AutoUnlock();
}

void cInterfaceRenderDevice::Draw(ElasticSphere *es) {
    SetWorldMatXf(es->GetGlobalMatrix());
    
    int is = es->theta_size;
    int js = es->psi_size;
    size_t points = (is+1) * (js+1) + (is+2) * (js-1);
    
    VISASSERT(es->GetTexture(0));
    
    SetNoMaterial(es->blendMode,es->GetFrame()->GetPhase(),es->GetTexture(0),es->GetTexture(1),COLOR_MOD);

    int cull=GetRenderState(RS_CULLMODE);
    SetRenderState(RS_CULLMODE,CULL_CW);

    DrawBuffer* db = GetDrawBuffer(sVertexXYZDT2::fmt, PT_TRIANGLESTRIP);
    indices_t* ib = nullptr;
    sVertexXYZDT2* vb = nullptr;
    db->Lock(points, points, vb, ib, false);
    if (!vb || !ib) {
        xassert(0);
        return;
    }

    uint32_t Diffuse = ConvertColor(es->GetDiffuse());
    float dv=0.5f-es->GetFrame()->GetPhase();
    int i;
    for (i=0; i<=is; i++) {
        const Vect3f &n=es->normal(0,i);
        vb->pos=es->point(0,i);
        vb->GetTexel().set(static_cast<float>(0) / static_cast<float>(es->psi_size),
                          static_cast<float>(i) / static_cast<float>(es->theta_size));
        vb->GetTexel2().set(n.y*0.5f+0.5f, n.z*0.5f+dv);
        vb->diffuse=Diffuse;
        
        ib[2*i] = db->lock_written_vertices;
        db->lock_written_indices++;
        db->lock_written_vertices++;
        vb++;
    }
    int j;
    for (j = 0; j < js; j++) {
        for (i = 0; i <= is; i++) {
            int isi = j & 1 ? (is - i) : i;
            const Vect3f& n = es->normal(j + 1, isi);
            vb->pos = es->point(j + 1, isi);
            vb->GetTexel().set(static_cast<float>(j + 1) / static_cast<float>(es->psi_size),
                               static_cast<float>(isi) / static_cast<float>(es->theta_size));
            vb->GetTexel2().set(n.y * 0.5f + 0.5f, n.z * 0.5f + dv);
            vb->diffuse = Diffuse;

            size_t s = (2 * is + 3) * j + 2 * i + 1;
            xassert(s < points);
            ib[s] = db->lock_written_vertices;
            db->lock_written_indices++;
            db->lock_written_vertices++;
            vb++;
        }
    }
    for (j = 0; j < js - 1; j++) {
        for (i = 0; i <= is; i++) {
            size_t s = (2 * is + 3) * j + 2 * i + 1;
            size_t d = (2 * is + 3) * (j + 1) + 2 * (is - i) + 0;
            xassert(s < points && d < points);
            ib[d] = ib[s];
            db->lock_written_indices++;
        }
        size_t s = (2 * is + 3) * j + 2 * is + 1;
        size_t d = (2 * is + 3) * j + 2 * is + 2;
        xassert(s < points && d < points);
        ib[d] = ib[s];
        db->lock_written_indices++;
    }
    db->AutoUnlock();
    db->Draw();

    SetRenderState(RS_CULLMODE,cull);
}
