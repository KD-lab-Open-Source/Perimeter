#include "StdAfxRD.h"

#include "ForceField.h"

class FFDData
{
public:
	int xsize,ysize;
	sPtrIndexBuffer ib;
	sPtrVertexBuffer vb;
	int pagesize;//in vertex
	int pagenumber;
	int curpage;

	int GetNumIndices(){return (2*xsize+2)*ysize;}
	void SetIB(sPolygon* pIndex);
};

void cD3DRender::CreateFFDData(class FieldDispatcher *ffd)
{
	FFDData* p=new FFDData;
	ffd->pDrawData=p;

	p->xsize = p->ysize = 1<<(ffd->tile_scale-ffd->scale);

	CreateIndexBuffer(p->ib,p->GetNumIndices());
	sPolygon* pIndex=LockIndexBuffer(p->ib);
	p->SetIB(pIndex);
	UnlockIndexBuffer(p->ib);

	p->pagesize=(p->xsize+1)*(p->ysize+1);
	p->pagenumber=8;
	p->curpage=0;
	CreateVertexBuffer(p->vb,p->pagesize*p->pagenumber,sVertexXYZDT2::fmt,true);
}

void cD3DRender::DeleteFFDData(FieldDispatcher* ffd)
{
	ffd->pDrawData->vb.Destroy();
	if(ffd->pDrawData)
		delete ffd->pDrawData;
	ffd->pDrawData=NULL;
}

void FFDData::SetIB(sPolygon* pIndex)
{
	#define RIDX(x,y) ((x)+xs*(y))

	int xs = 1 + xsize;
	int ys = ysize;

	WORD *ib=(WORD*)pIndex;

	for (int j = 0; j < ys; j++)
	if (j & 1)
	{
		for (int i = xs-1; i >= 0; i--)
		{
			*ib++ = RIDX(i,j);
			*ib++ = RIDX(i,j+1);
		}
	} else
	{
		for (int i = 0; i < xs; i++)
		{
			*ib++ = RIDX(i,j);
			*ib++ = RIDX(i,j+1);
		}
	}
	#undef RIDX
#ifdef _DEBUG
	int num=ib-(WORD*)pIndex;
	VISASSERT(num==GetNumIndices());
#endif //_DEBUG
}

void cD3DRender::Draw(FieldDispatcher *ffd)
{
	int cull=GetRenderState(D3DRS_CULLMODE);
	SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if(DrawNode->GetCameraPass()==SCENENODE_OBJECT){
		if(ffd->tile_global & FieldCluster::TT_OPAQUE)
			Draw(ffd,FieldCluster::TT_OPAQUE);
	}
	else{
		if(ffd->tile_global & FieldCluster::TT_TRANSPARENT_ADD)
			Draw(ffd,FieldCluster::TT_TRANSPARENT_ADD);
		if(ffd->tile_global & FieldCluster::TT_TRANSPARENT_MOD)
			Draw(ffd,FieldCluster::TT_TRANSPARENT_MOD);
	}
	
	SetRenderState(D3DRS_CULLMODE, cull );
}

void cD3DRender::Draw(FieldDispatcher *ffd,BYTE transparent)
{
//	start_timer_auto(Draw_ForceField,1);

	VISASSERT(DrawNode && DrawNode->GetScene());
	if(ffd->GetAttribute(ATTRUNKOBJ_WIREFRAME)) 
		SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	SetMatrix(D3DTS_WORLD, MatXf::ID);

	VISASSERT( ffd->GetTexture(0) && ffd->GetTexture(1) );
	Vect3f uv[2];
	Mat3f& mC = DrawNode->GetMatrix().rot();
	uv[0].set(0.5f*mC[0][0],0.5f*mC[0][1],0.5f*mC[0][2]);
	uv[1].set(0.5f*mC[1][0],0.5f*mC[1][1],0.5f*mC[1][2]);
	SetFVF(sVertexXYZDT2::fmt);

	FFDData* pfd=ffd->pDrawData;
	MTEnter lock(ffd->hmap_lock);
	float t=ffd->InterpolationFactor();
	float t_=1-t;

	LPDIRECT3DVERTEXBUFFER9 vb=pfd->vb.ptr->p;
	SetStreamSource(pfd->vb);

	DWORD AlphaTest = GetRenderState(D3DRS_ALPHATESTENABLE);
	DWORD AlphaRef = GetRenderState(D3DRS_ALPHAREF);
	DWORD zwrite = GetRenderState(D3DRS_ZWRITEENABLE);
	
	if(transparent==FieldCluster::TT_TRANSPARENT_ADD){
		SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
		SetNoMaterial(ALPHA_ADDBLENDALPHA,ffd->GetFrame()->GetPhase(),ffd->GetTexture(0),
						ffd->GetTexture(1),COLOR_ADD);
	}
	else if(transparent==FieldCluster::TT_TRANSPARENT_MOD){
		SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
		SetNoMaterial(ALPHA_BLEND,ffd->GetFrame()->GetPhase(),ffd->GetTexture(0),
						ffd->GetTexture(1),COLOR_ADD);
	}
	else{
		SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		SetNoMaterial(ALPHA_NONE,ffd->GetFrame()->GetPhase(),ffd->GetTexture(0),
						ffd->GetTexture(1),COLOR_ADD);

//		SetRenderState(D3DRS_ALPHAREF,0);
//		SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
	}

	for(int jTile = 0; jTile < ffd->tileMapSizeY(); jTile++)
		for(int iTile = 0; iTile < ffd->tileMapSizeX(); iTile++){
			if(!(ffd->tileMap(iTile, jTile) & transparent)) 
				continue;
			
			if(!DrawNode->TestVisible(Vect3f(ffd->t2w(iTile), ffd->t2w(jTile),0),
				Vect3f(ffd->t2w(iTile + 1), ffd->t2w(jTile + 1), 255)))
				continue;
			
			int tile_size = ffd->t2m(1);
			int numvertex = (tile_size + 1)*(tile_size + 1);
			float phase = ffd->GetFrame()->GetPhase();
			
			sVertexXYZDT2* pv;
			int bytepagesize = pfd->pagesize*pfd->vb.size;
			if(pfd->curpage<pfd->pagenumber){
				RDCALL(vb->Lock(pfd->curpage*bytepagesize,bytepagesize,
					(void**)&pv,D3DLOCK_NOOVERWRITE));
			}
			else{
				RDCALL(vb->Lock(0,0,(void**)&pv,D3DLOCK_DISCARD));
				pfd->curpage = 0;
			}
			
			int flDraw = 0;
			int x_begin = ffd->t2m(iTile);
			int y_begin = ffd->t2m(jTile);
			for(int y = 0; y <= tile_size; y++)
				for(int x = 0; x <= tile_size; x++){
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
					float zw = ffd->interpolateHeight(xm,ym,t,t_);
/**/
					if(cell.specified()){
						xw += cell.specify_delta.x;
						yw += cell.specify_delta.y;
						zw = FieldCluster::ZeroGround;
					}
					sVertexXYZDT2& v = *pv++;
					v.pos.set(xw, yw, zw);
					if(cell.cluster && v.pos.z > FieldCluster::ZeroGround && cell.cluster->GetTT() == transparent){
						v.diffuse.RGBA() = cell.cluster->GetColor(); 
						flDraw = 1;
					}
					else 
						v.diffuse.RGBA() = 0x00000000;
					const Vect3f& n = !cell.specified() ? ffd->normal(xm,ym) : cell.specify_normal;
					v.GetTexel().set(n.dot(uv[0]) + 0.5f, n.dot(uv[1]) + 0.5f);
					v.GetTexel2().set((n.y + 1)*0.5f, (n.z + 1)*0.5f - phase);
				}
				vb->Unlock();
				
				if(!flDraw) 
					continue;
				
				SetIndices(pfd->ib);
				RDCALL(lpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
					pfd->curpage*pfd->pagesize,
					0, numvertex, 0, pfd->GetNumIndices()-2));
				pfd->curpage++;
		}
		
	if( nSupportTexture>=3 )
	{
		SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );
		SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
		SetTexture( 0, 0, 2 );
		SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2 );
		SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	}
	
	if(ffd->GetAttribute(ATTRUNKOBJ_WIREFRAME))
		SetRenderState(D3DRS_FILLMODE,(bWireFrame==0)?D3DFILL_SOLID:D3DFILL_WIREFRAME);
	
	SetRenderState(D3DRS_ZWRITEENABLE,zwrite);
	SetRenderState(D3DRS_ALPHATESTENABLE,AlphaTest);
	SetRenderState(D3DRS_ALPHAREF,AlphaRef);
}

