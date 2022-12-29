#include "StdAfxRD.h"
#include "VertexFormat.h"
#include "IRenderDevice.h"
#include "SokolRender.h"

void cSokolRender::DrawRectangle(int x1,int y1,int dx,int dy,sColor4c color,bool outline) {
    VISASSERT(ActiveScene);

    int x2=x1+dx,y2=y1+dy;
    if (0 <= dx) { if (x2<0 || x1>ScreenSize.x) return; }
    else if (x1 < 0 || x2 > ScreenSize.x) return;
    if (0 <= dy) { if (y2<0 || y1>ScreenSize.y) return; }
    else if (y1 < 0 || y2 > ScreenSize.y) return;

    SetNoMaterial(ALPHA_BLEND);
    SetVSUniformMatrix(&orthoMat, false);

    /* TODO
    if (outline) {
        PrepareBuffers(8, 24, sVertexXYZD::fmt);
        size_t vc = activeCommand.vertices;

        sVertexXYZD* v = static_cast<sVertexXYZD*>(LockVertexBuffer(vertexBuffer));
        v = &v[vc];
        for (int i = 0; i < 8; ++i) {
            v[i].z=0;
            v[i].diffuse.v=color.ABGR();
        }
        v[0].x=v[1].x=static_cast<float>(x1);
        v[3].x=v[2].x=static_cast<float>(x2);
        v[0].y=v[2].y=static_cast<float>(y1);
        v[1].y=v[3].y=static_cast<float>(y2);
        UnlockVertexBuffer(vertexBuffer);

        sPolygon* p = BufferSeekPolygon(LockIndexBuffer(indexBuffer), activeCommand.indices);
        p[0].set(vc + 0, vc + 1, vc + 2);
        p[1].set(vc + 2, vc + 1, vc + 3);
        UnlockIndexBuffer(indexBuffer);

        activeCommand.vertices += 8;
        activeCommand.indices += 24;
    } else {
    */
        PrepareBuffers(4, 6, sVertexXYZDT1::fmt);
        size_t vc = activeCommand.vertices;

        sVertexXYZDT1* v = static_cast<sVertexXYZDT1*>(LockVertexBuffer(vertexBuffer));
        v = &v[vc];
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
        UnlockVertexBuffer(vertexBuffer);

        sPolygon* p = BufferSeekPolygon(LockIndexBuffer(indexBuffer), activeCommand.indices);
        p[0].set(vc + 0, vc + 1, vc + 2);
        p[1].set(vc + 2, vc + 1, vc + 3);
        UnlockIndexBuffer(indexBuffer);

        activeCommand.vertices += 4;
        activeCommand.indices += 6;
    //}
}

void cSokolRender::FlushPrimitive2D() {
}

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

    PrepareBuffers(4, 6, sVertexXYZDT1::fmt);
    SetVSUniformMatrix(&orthoMat, false);
    size_t vc = activeCommand.vertices;

    sVertexXYZDT1* v = static_cast<sVertexXYZDT1*>(LockVertexBuffer(vertexBuffer));
    v = &v[vc];
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
    UnlockVertexBuffer(vertexBuffer);

    sPolygon* p = BufferSeekPolygon(LockIndexBuffer(indexBuffer), activeCommand.indices);
    p[0].set(vc + 0, vc + 1, vc + 2);
    p[1].set(vc + 2, vc + 1, vc + 3);
    UnlockIndexBuffer(indexBuffer);
    
    activeCommand.vertices += 4;
    activeCommand.indices += 6;
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

    PrepareBuffers(4, 6, sVertexXYZDT1::fmt);
    SetVSUniformMatrix(&orthoMat, false);
    size_t vc = activeCommand.vertices;
    SetVSUniformMatrix(&orthoMat, false);

    sVertexXYZDT2* v = static_cast<sVertexXYZDT2*>(LockVertexBuffer(vertexBuffer));
    v = &v[vc];
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
    UnlockVertexBuffer(vertexBuffer);

    sPolygon* p = BufferSeekPolygon(LockIndexBuffer(indexBuffer), activeCommand.indices);
    p[0].set(vc + 0, vc + 1, vc + 2);
    p[1].set(vc + 2, vc + 1, vc + 3);
    UnlockIndexBuffer(indexBuffer);

    activeCommand.vertices += 4;
    activeCommand.indices += 6;
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

    PrepareBuffers(4, 6, sVertexXYZDT2::fmt);
    SetVSUniformMatrix(&orthoMat, false);
    SetFSUniformMode(PERIMETER_SOKOL_FS_MODE_MOD_COLOR_ADD_ALPHA); //Use special color mixing
    size_t vc = activeCommand.vertices;
    
    sVertexXYZDT2* v = static_cast<sVertexXYZDT2*>(LockVertexBuffer(vertexBuffer));
    v = &v[vc];
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
    UnlockVertexBuffer(vertexBuffer);

    sPolygon* p = BufferSeekPolygon(LockIndexBuffer(indexBuffer), activeCommand.indices);
    p[0].set(vc + 0, vc + 1,vc + 2);
    p[1].set(vc + 2, vc + 1,vc + 3);
    UnlockIndexBuffer(indexBuffer);

    activeCommand.vertices += 4;
    activeCommand.indices += 6;
}
