#include "StdAfxRD.h"
#include "Font.h"
#include "IRenderDevice.h"
#include "DrawBuffer.h"
#include "Silicon.h"
#include "Scene.h"

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
            v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=gb_RenderDevice->ConvertColor(diffuse);
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

            /*
            v[1].u1() = v[3].u1() = size.x;
            v[1].v1() = v[0].v1() = size.y;
            v[2].u1() = v[0].u1() = v[1].u1() + size.z;
            v[2].v1() = v[3].v1() = v[1].v1() + v_add;

            v[1].u2() = v[3].u2() = (x0 - x) * duv.x + uv.x;
            v[1].v2() = v[0].v2() = (y0 - y) * duv.y + uv.y;
            v[2].u2() = v[0].u2() = (x1 - x) * duv.x + uv.x;
            v[2].v2() = v[3].v2() = (y1 - y) * duv.y + uv.y;
            /*/
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

// Other render functions

void cInterfaceRenderDevice::DrawBound(const MatXf &Matrix,Vect3f &min,Vect3f &max,bool wireframe,const sColor4c &Color) {
    /*
    VISASSERT(DrawNode);
    int BytePerVertex=8*sizeof(sVertexXYZD);
    int BytePerPolygon=12*sizeof(sPolygon);
    VISASSERT((BytePerVertex+BytePerPolygon)<Buffer.length());
    sVertexXYZD *v=(sVertexXYZD*)&Buffer[0];
    sColor4c diffuse((150*Color.r)>>8,(155*Color.g)>>8,(155*Color.b)>>8,100);
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
    v[6].diffuse=v[7].diffuse=ConvertColor(diffuse);
    sPolygon *p=(sPolygon*)&Buffer[BytePerVertex];
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
    SetFVF(v->fmt);
    SetRenderState( RS_ZWRITEENABLE, FALSE );
    if(wireframe) SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
    SetWorldMatXf(Matrix);
    SetNoMaterial(ALPHA_BLEND);

    RDCALL(lpD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,8,12,
                                               p,PERIMETER_D3D_INDEX_FMT,v,sizeof(v[0])));

    if(wireframe) SetRenderState(D3DRS_FILLMODE,bWireFrame==0?D3DFILL_SOLID:D3DFILL_WIREFRAME);
    SetRenderState( RS_ZWRITEENABLE, TRUE );
    */
}

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
    
    Vect3f uv[2];
    Mat3f &mC=DrawNode->GetMatrix().rot();
    uv[0].set(0.5f * mC[0][0], 0.5f * mC[0][1], 0.5f * mC[0][2]);
    uv[1].set(0.5f * mC[1][0], 0.5f * mC[1][1], 0.5f * mC[1][2]);
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
