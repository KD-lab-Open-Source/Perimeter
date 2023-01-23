#include "StdAfxRD.h"
#include "VertexFormat.h"
#include "DrawBuffer.h"
#include "ForceField.h"
#include "SafeCast.h"
#ifdef PERIMETER_D3D9
#include "D3DRender.h"
#endif

class FFDData
{
public:
    ~FFDData() {
        db.Destroy();
    }
    
	int xsize,ysize;
	DrawBuffer db;
	size_t TileVertices;

    size_t GetNumIndices() { return (2 * xsize + 2) * ysize; }
    void SetIB();
};

void cInterfaceRenderDevice::CreateFFDData(class FieldDispatcher *ffd) {
	FFDData* p = new FFDData;
    ffd->pDrawData=p;

	p->xsize = p->ysize = 1<<(ffd->tile_scale-ffd->scale);

	p->TileVertices = (p->xsize + 1) * (p->ysize + 1);

    p->db.Create(p->TileVertices, true, p->GetNumIndices(), false, sVertexXYZDT2::fmt, PT_TRIANGLESTRIP);
    p->SetIB();
}

void cInterfaceRenderDevice::DeleteFFDData(FieldDispatcher* ffd) {
    delete ffd->pDrawData;
	ffd->pDrawData = nullptr;
}

void FFDData::SetIB() {
    sVertexXYZDT2* unused = nullptr;
    indices_t* pib = nullptr;
    db.Lock(0, db.ib.NumberIndices, unused, pib, false);
	int xs = 1 + xsize;
	int ys = ysize;
    for (int j = 0; j < ys; j++) {
        if (j & 1) {
            for (int i = xs - 1; i >= 0; i--) {
                pib[db.lock_written_indices++] = i + xs * j;
                pib[db.lock_written_indices++] = i + xs * (j+1);
            }
        } else {
            for (int i = 0; i < xs; i++) {
                pib[db.lock_written_indices++] = i + xs * j;
                pib[db.lock_written_indices++] = i + xs * (j+1);
            }
        }
    }
    db.Unlock();
    VISASSERT(db.written_indices == GetNumIndices());
}

void cInterfaceRenderDevice::Draw(FieldDispatcher *ffd)
{
	int cull=GetRenderState(RS_CULLMODE);
	SetRenderState(RS_CULLMODE, CULL_NONE);

    if (DrawNode->GetCameraPass()==SCENENODE_OBJECT) {
        if (ffd->tile_global & FieldCluster::TT_OPAQUE) {
            Draw(ffd, FieldCluster::TT_OPAQUE);
        }
	} else {
        if (ffd->tile_global & FieldCluster::TT_TRANSPARENT_ADD) {
            Draw(ffd, FieldCluster::TT_TRANSPARENT_ADD);
        }
        if (ffd->tile_global & FieldCluster::TT_TRANSPARENT_MOD) {
            Draw(ffd, FieldCluster::TT_TRANSPARENT_MOD);
        }
	}
	
	SetRenderState(RS_CULLMODE, cull);
}

void cInterfaceRenderDevice::Draw(FieldDispatcher *ffd, uint8_t transparent) {
//	start_timer_auto(Draw_ForceField,1);

	VISASSERT(DrawNode && DrawNode->GetScene());
    uint32_t Wireframe = 0; 
	if (ffd->GetAttribute(ATTRUNKOBJ_WIREFRAME)) {
        Wireframe = GetRenderState(RS_WIREFRAME);
        SetRenderState(RS_WIREFRAME, 1);
    }
    SetWorldMatXf(MatXf::ID);

	VISASSERT( ffd->GetTexture(0) && ffd->GetTexture(1) );
	Vect3f uv[2];
	Mat3f& mC = DrawNode->GetMatrix().rot();
	uv[0].set(0.5f*mC[0][0],0.5f*mC[0][1],0.5f*mC[0][2]);
	uv[1].set(0.5f*mC[1][0],0.5f*mC[1][1],0.5f*mC[1][2]);

	FFDData* pfd=ffd->pDrawData;
	MTEnter lock(ffd->hmap_lock);
	float t=ffd->InterpolationFactor();
	float t_=1-t;

	uint32_t AlphaTest = GetRenderState(RS_ALPHA_TEST_MODE);
	uint32_t zwrite = GetRenderState(RS_ZWRITEENABLE);
	
	if (transparent==FieldCluster::TT_TRANSPARENT_ADD) {
		SetRenderState(RS_ZWRITEENABLE,0);
		SetNoMaterial(ALPHA_ADDBLENDALPHA,ffd->GetFrame()->GetPhase(),ffd->GetTexture(0),
						ffd->GetTexture(1),COLOR_ADD);
	} else if(transparent==FieldCluster::TT_TRANSPARENT_MOD) {
		SetRenderState(RS_ZWRITEENABLE,0);
		SetNoMaterial(ALPHA_BLEND,ffd->GetFrame()->GetPhase(),ffd->GetTexture(0),
						ffd->GetTexture(1),COLOR_ADD);
	} else {
		SetRenderState(RS_ZWRITEENABLE,1);
		SetNoMaterial(ALPHA_NONE,ffd->GetFrame()->GetPhase(),ffd->GetTexture(0),
						ffd->GetTexture(1),COLOR_ADD);
	}

    for (int jTile = 0; jTile < ffd->tileMapSizeY(); jTile++) {
        DrawBuffer& db = pfd->db;
        for (int iTile = 0; iTile < ffd->tileMapSizeX(); iTile++) {
            if (!(ffd->tileMap(iTile, jTile) & transparent)) {
                continue;
            }

            if (!DrawNode->TestVisible(Vect3f(ffd->t2w(iTile), ffd->t2w(jTile), 0),
                                       Vect3f(ffd->t2w(iTile + 1), ffd->t2w(jTile + 1), 255))) {
                continue;
            }

            int tile_size = ffd->t2m(1);
            float phase = ffd->GetFrame()->GetPhase();
            
            sVertexXYZDT2* pv = nullptr;
            indices_t* unusedib = nullptr;
            db.Lock(pfd->TileVertices, 0, pv, unusedib, false);

            int flDraw = 0;
            int x_begin = ffd->t2m(iTile);
            int y_begin = ffd->t2m(jTile);
            for (int y = 0; y <= tile_size; y++) {
                for (int x = 0; x <= tile_size; x++) {
                    int xm = x + x_begin;
                    int ym = y + y_begin;
                    int xw = ffd->m2w(xm);
                    int yw = ffd->m2w(ym);
                    xm = clamp(xm, 1, ffd->mapSizeX() - 2);
                    ym = clamp(ym, 1, ffd->mapSizeY() - 2);
                    const FieldDispatcher::Cell& cell = ffd->map(xm, ym);
                    /*
                    float zw = cell.height;
                    /*/
                    float zw = ffd->interpolateHeight(xm, ym, t, t_);
                    /**/
                    if (cell.specified()) {
                        xw += cell.specify_delta.x;
                        yw += cell.specify_delta.y;
                        zw = FieldCluster::ZeroGround;
                    }
                    sVertexXYZDT2& v = pv[db.lock_written_vertices];
                    db.lock_written_vertices += 1;
                    v.pos.set(xw, yw, zw);
                    if (cell.cluster && v.pos.z > FieldCluster::ZeroGround && cell.cluster->GetTT() == transparent) {
                        v.diffuse = ConvertColor(cell.cluster->GetColor());
                        flDraw = 1;
                    } else {
                        v.diffuse = 0x00000000;
                    }
                    const Vect3f& n = !cell.specified() ? ffd->normal(xm, ym) : cell.specify_normal;
                    v.GetTexel().set(n.dot(uv[0]) + 0.5f, n.dot(uv[1]) + 0.5f);
                    v.GetTexel2().set((n.y + 1) * 0.5f, (n.z + 1) * 0.5f - phase);
                }
            }
            xassert(db.lock_written_vertices == pfd->TileVertices);
            db.Unlock();

            if (!flDraw) {
                db.Backwind();
            } else {
                db.Draw();
            }
        }
    }

#ifdef PERIMETER_D3D9
    if (GetRenderSelection() == DEVICE_D3D9) {
        cD3DRender* cd3d = safe_cast<cD3DRender*>(this);
        if (cd3d->nSupportTexture >= 3) {
            cd3d->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
            cd3d->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
            cd3d->SetTexture(0, 0, 2);
            cd3d->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 2);
            cd3d->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
        }
    }
#endif
	
	if (ffd->GetAttribute(ATTRUNKOBJ_WIREFRAME)) {
        SetRenderState(RS_WIREFRAME, Wireframe);
    }	
	SetRenderState(RS_ZWRITEENABLE,zwrite);
	SetRenderState(RS_ALPHA_TEST_MODE,AlphaTest);
}
