#include "StdAfxRD.h"
#include "IncTerra.h"
#include "TileMap.h"
#include "Scene.h"

#include "..\..\Game\Region.h"

#include <set>
#include <slist>
#include "clip\ClippingMesh.h"

#define BUMP_VTXTYPE   sVertexXYZT1
#define BUMP_VTXFORMAT (BUMP_VTXTYPE::fmt)
#define BUMP_VTXSIZE   sizeof(BUMP_VTXTYPE)
#define BUMP_IDXTYPE   unsigned short
#define BUMP_IDXSIZE   sizeof(BUMP_IDXTYPE)
#define BUMP_IDXFMT    D3DFMT_INDEX16

static int bumpGeoScale[TILEMAP_LOD] = { 1, 2, 3, 4, 5 };
static int bumpTexScale[TILEMAP_LOD] = { 0, 0, 1, 2, 3 };

const int texture_border=1;

struct sPlayerIB
{
	IndexPoolPage index;
	int nindex;
	int player;
};

struct VectDelta:public Vect2i
{
	int delta2;
	Vect2i delta;
	char player;

	enum
	{
		FIX_RIGHT=1,
		FIX_BOTTOM=2,
		FIX_FIXED=4,
		FIX_FIXEMPTY=8,
	};
	char fix;

	void copy_no_fix(const VectDelta& p)
	{
		x=p.x;
		y=p.y;
		delta2=p.delta2;
		delta=p.delta;
		player=p.player;
	}
};

struct sBumpTile
{
	//Only to read
	VertexPoolPage vtx;
	vector<sPlayerIB> index;
	Vect2i tile_pos;

	bool init;
	int age, LOD;
	int texPool, texPage;

	enum
	{
		U_LEFT=0,
		U_RIGHT=1,
		U_TOP=2,
		U_BOTTOM=3,
		U_ALL=4,
	};

	char border_lod[U_ALL];
protected:
	float vStart, vStep, uStart, uStep;
public:
	sBumpTile(cTileMap *TileMap, int lod,int xpos,int ypos);
	~sBumpTile();
	D3DLOCKED_RECT *LockTex();
	BYTE *LockVB();
	void UnlockTex();
	void UnlockVB();
	void Calc(cTileMap *TileMap,bool update_texture);

	void FindFreeTexture(int& Pool,int& Page,int tex_width,int tex_height);

	inline Vect2f GetUVStart(){return Vect2f(uStart,vStart);};
	inline Vect2f GetUVStep()
	{
		float div=1/(float)(1 << bumpGeoScale[LOD]);
		return Vect2f(uStep * div,vStep*div);
	};

	inline bool IsZeroplast()
	{
		int sz=index.size();
		if(sz>1)
			return true;
		if(sz==1)
			return index[0].player>=0;
		return false;
	}

	inline bool IsOnlyZeroplast()
	{
		return index.size()==1 && index[0].player>=0;
	}
protected:
	void CalcTexture(cTileMap *TileMap);
	void CalcPoint(cTileMap *TileMap);

	int FixLine(VectDelta* points,int ddv,cTileMap *TileMap);

	inline float SetVertexZ(TerraInterface* terra,int x,int y);
	void DeleteIndex();
};

class sTilemapTexturePool
{
	LPDIRECT3DTEXTURE9 texture;
	cD3DRender *renderer;
	int tileWidth, tileHeight;
	int tileRealWidth, tileRealHeight;
	int freePages;
	int *freePagesList;

	int texture_width,texture_height;
	vector<Vect2i> Pages;//x-xmin, y-ymin
	float ustep,vstep;
public:
	vector<sBumpTile*> tileRenderList;

	sTilemapTexturePool(cD3DRender *aRenderer, int w, int h);
	~sTilemapTexturePool();
	int allocPage();
	void freePage(int page);
	D3DLOCKED_RECT *lockPage(int page);
	void unlockPage(int page);
	Vect2f getUVStart(int page);
	inline float getVStep(){return vstep;};
	inline float getUStep(){return ustep;};

	inline int GetTileWidth(){return tileWidth;}
	inline int GetTileHeight(){return tileHeight;}
	inline int IsFree(){return freePages;}
	inline LPDIRECT3DTEXTURE9 GetTexture(){return texture;}

	void GetTextureMemory(int &total,int& free);
protected:
	void CalcPagesPos();
};

class cTileMapRender
{
	cTileMap *TileMap;
	vector<sBumpTile*> bumpTiles;
	vector<int> bumpDyingTiles;
	LPDIRECT3DINDEXBUFFER9 tilemapIB;
	int index_offset[TILEMAP_LOD];
	int index_size[TILEMAP_LOD];

	BYTE* visMap;
	char* vis_lod;

	char* update_stat;
	bool update_in_frame;

	void SaveUpdateStat();

	VectDelta* delta_buffer;
	vector<vector<sPolygon> > index_buffer;
public:
	void IncUpdate(sBumpTile* pbump);

	cTileMapRender(cTileMap *pTileMap);
	~cTileMapRender();

	void ClearTilemapPool();
	void RestoreTilemapPool();

	int bumpNumIndex(int lod){return index_size[lod];};
	int bumpIndexOffset(int lod){return index_offset[lod];};

	int bumpNumVertices(int lod);
	void bumpCreateIB(sPolygon* ib, int lod);
	int bumpTileValid(int id);
	int bumpTileAlloc(int lod,int xpos,int ypos);
	void bumpTileFree(int id);
	void bumpTilesDeath();

	void PreDraw(cCamera* DrawNode);
	void DrawBump(cCamera* DrawNode,eBlendMode MatMode,TILEMAP_DRAW tile_draw,bool shadow);

	sBumpTile* GetTile(int k,int n)
	{
		VISASSERT(k>=0 && k<TileMap->GetTileNumber().x);
		VISASSERT(n>=0 && n<TileMap->GetTileNumber().y);
		int bumpTileID = TileMap->GetTile(k, n).bumpTileID;
		if(bumpTileID>=0)
			return bumpTiles[bumpTileID];
		return NULL;
	}
	
	VectDelta* GetDeltaBuffer(){return delta_buffer;};
	vector<vector<sPolygon> >& GetIndexBuffer(){return index_buffer;};
};

static vector<sTilemapTexturePool*> bumpTexPools;


//
// **************** TILEMAP - SHARED ****************
// 
static void fillVisPoly(BYTE *buf,vector<Vect2f>& vert,int VISMAP_W,int VISMAP_H)
{
	if(vert.empty())return;
	const int VISMAP_W_MAX=128,VISMAP_H_MAX=128;
	VISASSERT(VISMAP_W<=VISMAP_W_MAX && VISMAP_H<=VISMAP_H_MAX);
	float lx[VISMAP_W_MAX], rx[VISMAP_H_MAX];
	int i, y, ytop, ybot;

	// find top/bottom y
	ytop = floor(vert[0].y);
	ybot = ceil(vert[0].y);
	for(i=1;i<vert.size();i++)
	{
		float y=vert[i].y;
		if (y < ytop) ytop = floor(y);
		if (y > ybot) ybot = ceil(y);
	}

	for (i = 0; i < VISMAP_H; i++)
	{
		lx[i] = VISMAP_W-1;
		rx[i] = 0;
	}

	// render edges
	for (i = 0; i < vert.size(); i++)
	{
		int i2=(i+1>=vert.size())?0:i+1;
		float x1, x2, y1, y2, t;
		x1=vert[i].x;  y1=vert[i].y;
		x2=vert[i2].x; y2=vert[i2].y;

		if (y1 > y2) { t = x1; x1 = x2; x2 = t; t = y1; y1 = y2; y2 = t; }

		int iy1 = (int)y1, iy2 = (int)y2;
		if(iy1>iy2)continue;

		float dy = (y2 == y1) ? 1 : (y2 - y1);
		float dy1 =1/dy;
		for (y = max(iy1, 0); y <= min(iy2, VISMAP_H-1); y++)
		{
			float ix1 = x1 + (y-y1) * (x2-x1) * dy1;
			float ix2 = x1 + (y-y1+1) * (x2-x1) * dy1;
			if (y == iy1) ix1 = x1;
			if (y == iy2) ix2 = x2;
			lx[y] = min(min(lx[y], ix1), ix2);
			rx[y] = max(max(rx[y], ix1), ix2);
		}
	}

	// fill the buffer
	for (y = max(0, ytop); y <= min(ybot, VISMAP_H-1); y++)
	{
		if (lx[y] > rx[y]) continue;
		int x1 = (int)max((float)floor(lx[y]), 0.0f);
		int x2 = (int)min((float)ceil(rx[y]), (float)VISMAP_W);
		if(x1>=x2)continue;
		memset(buf + y*VISMAP_W + x1, 1, x2-x1);
	}
}
void calcCMesh(cCamera *DrawNode, Vect2i TileNumber,Vect2i TileSize,CMesh& cmesh)
{
	AMesh mesh;
	float dx=TileNumber.x*TileSize.x;
	float dy=TileNumber.y*TileSize.y;
	mesh.CreateABB(Vect3f(0,0,0),Vect3f(dx,dy,256.0f));

	cmesh.Set(mesh);

	for(int i=0;i<DrawNode->GetNumberPlaneClip3d();i++)
	{
		sPlane4f& plane=DrawNode->GetPlaneClip3d(i);
		cmesh.Clip(plane);
	}
}

void drawCMesh(CMesh& cmesh)
{
	for(int i=0;i<cmesh.E.size();i++)
	if(cmesh.E[i].visible)
	{
		int iv0=cmesh.E[i].vertex[0];
		int iv1=cmesh.E[i].vertex[1];
		Vect3f v0,v1;
		v0=cmesh.V[iv0].point;
		v1=cmesh.V[iv1].point;
		gb_RenderDevice->DrawLine(v0,v1,sColor4c(0,0,0,255));
	}
}

void calcVisMap(cCamera *DrawNode, CMesh& cmesh, Vect2i TileNumber,Vect2i TileSize,BYTE* visMap,bool clear)
{
	APolygons poly;
	cmesh.BuildPolygon(poly);
	if(clear)
		memset(visMap, 0, TileNumber.x*TileNumber.y);

	for(int i=0;i<poly.faces.size();i++)
	{
		vector<int>& inp=poly.faces[i];
		vector<Vect2f> points(inp.size());
		for(int j=0;j<inp.size();j++)
		{
			Vect3f& p=poly.points[inp[j]];
			points[j].x=p.x/TileSize.x;
			points[j].y=p.y/TileSize.y;
		}
		
		fillVisPoly(visMap, points,TileNumber.x,TileNumber.y);
	}

	if(false)
		drawCMesh(cmesh);
}

sBox6f calcBoundInDirection(CMesh& cmesh,Mat3f& m)
{
	sBox6f box;
	box.SetInvalidBox();
	for(int i=0;i<cmesh.E.size();i++)
	if(cmesh.E[i].visible)
	{
		for(int j=0;j<2;j++)
		{
			int iv=cmesh.E[i].vertex[j];
			Vect3f v;
			v=m*cmesh.V[iv].point;
			box.AddBound(v);
		}
	}
	return box;
}

void calcVisMap(cCamera *DrawNode, Vect2i TileNumber,Vect2i TileSize,BYTE* visMap,bool clear)
{
	CMesh cmesh;
	calcCMesh(DrawNode,TileNumber,TileSize,cmesh);
	calcVisMap(DrawNode,cmesh,TileNumber,TileSize,visMap,clear);
}

void calcVisMap(cCamera *DrawNode, cTileMap *TileMap,BYTE* visMap,bool clear)
{
	calcVisMap(DrawNode, TileMap->GetTileNumber(),TileMap->GetTileSize(),visMap,clear);
}

void calcVisMap(cCamera *DrawNode, Vect2i TileNumber,Vect2i TileSize,Mat3f& direction,sBox6f& box)
{
	CMesh cmesh;
	calcCMesh(DrawNode,TileNumber,TileSize,cmesh);
	box=calcBoundInDirection(cmesh,direction);
}

int cD3DRender::Create(class cTileMap *TileMap)
{
	cTileMapRender* p=new cTileMapRender(TileMap);
	TileMap->SetTilemapRender(p);
	p->RestoreTilemapPool();
	tilemaps.push_back(p);
	return 0;
}

void cD3DRender::GetTilemapTextureMemory(int& total,int& free)
{
	vector<sTilemapTexturePool*>::iterator it;
	total=free=0;
	FOR_EACH(bumpTexPools,it)
	{
		int cur_total,cur_free;
		(*it)->GetTextureMemory(cur_total,cur_free);
		total+=cur_total;
		free+=cur_free;
	}
}

void sTilemapTexturePool::GetTextureMemory(int &total,int& free)
{
	total=texture_width*texture_height*4;
	free=freePages*tileWidth*tileHeight*4;
}

int cD3DRender::Delete(class cTileMap *TileMap)
{
	cTileMapRender* p=TileMap->GetTilemapRender();
	if(p==NULL)
		return true;
	
	p->ClearTilemapPool();
	for(int i=0;i<tilemaps.size();i++)
	{
		if(tilemaps[i]==p)
		{
			tilemaps.erase(tilemaps.begin()+i);
			delete p;
			return true;
		}
	}

	VISASSERT(0);
	return false;
}

void cD3DRender::Draw(cTileMap *TileMap,eBlendMode MatMode,TILEMAP_DRAW tile_draw,bool shadow)
{
	TileMap->GetTilemapRender()->DrawBump(DrawNode,MatMode,tile_draw,shadow);
}

void cD3DRender::ClearTilemapPool()
{
	vector<cTileMapRender*>::iterator it;
	FOR_EACH(tilemaps,it)
		(*it)->ClearTilemapPool();
}

void cD3DRender::RestoreTilemapPool()
{
	vector<cTileMapRender*>::iterator it;
	FOR_EACH(tilemaps,it)
		(*it)->RestoreTilemapPool();
}

/////////////////////cTileMapRender////////////////////

cTileMapRender::cTileMapRender(cTileMap *pTileMap)
{
	TileMap=pTileMap;
	tilemapIB=NULL;

	int dxy=TileMap->GetTileNumber().x*TileMap->GetTileNumber().y;
	visMap=new BYTE[dxy];
	vis_lod=new char[dxy];
	for(int i=0;i<dxy;i++)
		vis_lod[i]=-1;

	update_stat=NULL;
//	update_stat=new char[dxy*TILEMAP_LOD];
	update_in_frame=false;

	int ddv=TILEMAP_SIZE+1;
	delta_buffer=new VectDelta[ddv*ddv];
}

cTileMapRender::~cTileMapRender()
{
	RELEASE(tilemapIB);
	delete visMap;
	delete vis_lod;

	delete update_stat;
	delete delta_buffer;

	vector<sBumpTile*>::iterator it;
	FOR_EACH(bumpTiles,it)
		delete *it;
	bumpTiles.clear();
}

void cTileMapRender::ClearTilemapPool()
{
	for(int y=0;y<TileMap->GetTileNumber().y;y++)
	for(int x=0;x<TileMap->GetTileNumber().x;x++)
	{
		sTile &Tile = TileMap->GetTile(x, y);
		int &bumpTileID = Tile.bumpTileID;
		bumpTileID = -1;
	}

	bumpDyingTiles.clear();
	for(int i=0;i<bumpTiles.size();i++)
	{
		delete bumpTiles[i];
	}
	bumpTiles.clear();

	RELEASE(tilemapIB);
	int i;
	for(i=0;i<bumpTexPools.size();i++)
		delete bumpTexPools[i];
	bumpTexPools.clear();

}

void cTileMapRender::RestoreTilemapPool()
{
	if(tilemapIB)
		return;
	int cur_offset=0;
	Vect2i TileSize=TileMap->GetTileSize();

	for (int iLod = 0; iLod < TILEMAP_LOD; iLod++)
	{
		int xs = TileSize.x >> bumpGeoScale[iLod];
		int ys = TileSize.y >> bumpGeoScale[iLod];

		index_offset[iLod]=cur_offset;
		index_size[iLod]=6*xs*ys;//?????? может 2 а не 6
		cur_offset+=index_size[iLod];
	}

	sPolygon *pIndex = 0;
	RDCALL(gb_RenderDevice3D->lpD3DDevice->CreateIndexBuffer(
		cur_offset * BUMP_IDXSIZE, D3DUSAGE_WRITEONLY,
		BUMP_IDXFMT, D3DPOOL_DEFAULT,
		&tilemapIB,NULL));
	RDCALL(tilemapIB->Lock(0, 0, (void**)&pIndex,
		D3DLOCK_NOSYSLOCK));
	int iLod;
	for (iLod = 0; iLod < TILEMAP_LOD; iLod++)
	{
		bumpCreateIB(pIndex+bumpIndexOffset(iLod)/3, iLod);
	}
	tilemapIB->Unlock();
}

void cTileMapRender::bumpCreateIB(sPolygon *ib, int lod)
{
	Vect2i TileSize=TileMap->GetTileSize();
	VISASSERT(TileSize.x==TileSize.y);

	int dd = (TileSize.x >> bumpGeoScale[lod]);
	int ddv=dd+1;
	for(int y=0;y<dd;y++)
	for(int x=0;x<dd;x++)
	{
		int base=x+y*ddv;
		ib++->set(base,base+ddv,base+1);
		ib++->set(base+ddv,base+ddv+1,base+1);
	}

}

void cTileMapRender::PreDraw(cCamera* DrawNode)
{
	for(int y=0;y<TileMap->GetTileNumber().y;y++)
	for(int x=0;x<TileMap->GetTileNumber().x;x++)
	{
		sTile &Tile = TileMap->GetTile(x, y);
		int &bumpTileID = Tile.bumpTileID;
		if(!Tile.GetAttribute(ATTRTILE_DRAWLOD))
		{
			bumpTileFree(bumpTileID);
			bumpTileID = -1;
		}

		Tile.ClearAttribute(ATTRTILE_DRAWLOD);
	}

	if(update_stat)
	{
		SaveUpdateStat();
		update_in_frame=false;
		int dxy=TileMap->GetTileNumber().x*TileMap->GetTileNumber().y;
		memset(update_stat,0,dxy*TILEMAP_LOD);
	}

	bumpTilesDeath();
}

int cTileMapRender::bumpNumVertices(int lod)
{
	int xs = (TileMap->GetTileSize().x >> bumpGeoScale[lod]);
	int ys = (TileMap->GetTileSize().y >> bumpGeoScale[lod]);

	return (xs+1)*(ys+1);
}

int cTileMapRender::bumpTileValid(int id)
{
	return ((id >= 0) && (id < bumpTiles.size()) && (bumpTiles[id]));
}

int cTileMapRender::bumpTileAlloc(int lod,int xpos,int ypos)
{
	int i;
	for (i = 0; i < bumpTiles.size(); i++)
		if (!bumpTiles[i]) break;
	if (i == bumpTiles.size()) bumpTiles.push_back(NULL);
	bumpTiles[i] = new sBumpTile(TileMap, lod, xpos, ypos);
	return i;
}

void cTileMapRender::bumpTileFree(int id)
{
	if (bumpTileValid(id))
		bumpDyingTiles.push_back(id);
}

void cTileMapRender::bumpTilesDeath()
{
	for (int i = 0; i < bumpDyingTiles.size(); i++)
	{
		int tile=bumpDyingTiles[i];
		if (++bumpTiles[tile]->age > 4)
		{
			delete bumpTiles[tile];
			bumpTiles[tile] = NULL;
			bumpDyingTiles.erase(bumpDyingTiles.begin() + i);
			i--;
		}
	}
}


///////////////////////////sTilemapTexturePool/////////////////////////////////
sTilemapTexturePool::sTilemapTexturePool(cD3DRender *aRenderer, int w, int h)
{
	texture_width=texture_height=512;
	renderer = aRenderer;
	tileWidth = w;
	tileHeight = h;

	tileRealWidth=tileWidth+texture_border*2;
	tileRealHeight=tileHeight+texture_border*2;
	ustep=1.0f/texture_height;
	vstep=1.0f/texture_width;

	CalcPagesPos();

	freePagesList = new int[Pages.size()];
	freePages = Pages.size();
	for (int i = 0; i < Pages.size(); i++)
		freePagesList[i] = i;

	RDCALL(renderer->lpD3DDevice->CreateTexture(
		texture_width, texture_height, 1,
		0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture,NULL));
/* Этот кусок теоретически лучше, но надо проверить на Geforce 2
	RDCALL(renderer->lpD3DDevice->CreateTexture(
		texture_width, texture_height, 1,
		D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture,NULL));
*/
}

sTilemapTexturePool::~sTilemapTexturePool()
{
	delete[] freePagesList;
	texture->Release();
}

void sTilemapTexturePool::CalcPagesPos()
{
	for(int y=0;y<=texture_height-tileRealHeight;y+=tileRealHeight)
	{
		for(int x=0;x<=texture_width-tileRealHeight;x+=tileRealWidth)
		{
			Vect2f page;
			page.x=x;
			page.y=y;
			Pages.push_back(page);
		}
	}
}

int sTilemapTexturePool::allocPage()
{
	if (!freePages) return -1;
	freePages--;
	return freePagesList[freePages];
}

void sTilemapTexturePool::freePage(int page)
{
	freePagesList[freePages] = page;
	freePages++;
}

D3DLOCKED_RECT *sTilemapTexturePool::lockPage(int page)
{
	static D3DLOCKED_RECT lockedRect;
	Vect2i pos=Pages[page];
	RECT rect;

	rect.left   = pos.x;
	rect.top    = pos.y;
	rect.right  = rect.left+ tileRealWidth;
	rect.bottom = rect.top + tileRealHeight;
	RDCALL(texture->LockRect(0, &lockedRect, &rect, 0));
	return &lockedRect;
}

void sTilemapTexturePool::unlockPage(int page)
{
	RDCALL(texture->UnlockRect(0));
}

Vect2f sTilemapTexturePool::getUVStart(int page)
{
	VISASSERT(page>=0 && page<Pages.size());
	Vect2i pos=Pages[page];
	return Vect2f((pos.x+texture_border)*ustep,(pos.y+texture_border)*vstep);
}

//////////////////////////sBumpTile//////////////////////////////////////
float sBumpTile::SetVertexZ(TerraInterface* terra,int x,int y)
{
	float zi=terra->GetZf(x,y);
	if(zi<1)
		zi=-15;

	return zi;
}

sBumpTile::sBumpTile(cTileMap *TileMap, int lod,int xpos,int ypos)
{
	tile_pos.set(xpos,ypos);
	int w = TileMap->GetTileSize().x >> bumpTexScale[lod];
	int h = TileMap->GetTileSize().y >> bumpTexScale[lod];
	int numVertex = TileMap->GetTilemapRender()->bumpNumVertices(lod);

	gb_RenderDevice3D->GetVertexPool()->CreatePage(vtx,VertexPoolParameter(numVertex,BUMP_VTXFORMAT));

	FindFreeTexture(texPool,texPage,w,h);

	sTilemapTexturePool* curpool=bumpTexPools[texPool];

	Vect2f uvStart=curpool->getUVStart(texPage);
	uStart = uvStart.x; 
	vStep = curpool->getVStep() * (1 << (bumpGeoScale[lod] - bumpTexScale[lod]));
	vStart = uvStart.y;
	uStep = curpool->getUStep() * (1 << (bumpGeoScale[lod] - bumpTexScale[lod]));

	init = false;
	LOD = lod;
	age = 0;

	for(int i=0;i<U_ALL;i++)
		border_lod[i]=lod;
}

sBumpTile::~sBumpTile()
{
	gb_RenderDevice3D->GetVertexPool()->DeletePage(vtx);
	bumpTexPools[texPool]->freePage(texPage);

	DeleteIndex();
}

void sBumpTile::DeleteIndex()
{
	vector<sPlayerIB>::iterator it;
	FOR_EACH(index,it)
	if(it->index.page>=0)
	{
		gb_RenderDevice3D->GetIndexPool()->DeletePage(it->index);
	}
	index.clear();
}

void sBumpTile::FindFreeTexture(int& Pool,int& Page,int tex_width,int tex_height)
{
	int i;
	for (i = 0; i < bumpTexPools.size(); i++)
		if (bumpTexPools[i] && bumpTexPools[i]->IsFree()
			&& bumpTexPools[i]->GetTileWidth() == tex_width
			&& bumpTexPools[i]->GetTileHeight() == tex_height)
				break;
	if (i >= bumpTexPools.size())
	{
		for (i = 0; i < bumpTexPools.size(); i++)
			if (bumpTexPools[i] == NULL) break;
		if (i >= bumpTexPools.size())
		{
			i = bumpTexPools.size();
			bumpTexPools.push_back(NULL);
		}
		bumpTexPools[i] = new sTilemapTexturePool((cD3DRender*)gb_RenderDevice,
			tex_width, tex_height);
	}

	Pool = i;
	Page = bumpTexPools[i]->allocPage();
}

D3DLOCKED_RECT *sBumpTile::LockTex()
{
	return bumpTexPools[texPool]->lockPage(texPage);
}

BYTE *sBumpTile::LockVB()
{
	return (BYTE*)gb_RenderDevice3D->GetVertexPool()->LockPage(vtx);
}

void sBumpTile::UnlockTex()
{
	bumpTexPools[texPool]->unlockPage(texPage);
}

void sBumpTile::UnlockVB()
{
	gb_RenderDevice3D->GetVertexPool()->UnlockPage(vtx);
}

inline int IUCLAMP(int val,int clamp)
{ 
	if(val<0)return 0;
	if(val>=clamp)return clamp-1;
	return val;
}

void sBumpTile::CalcTexture(cTileMap *TileMap)
{
	int tilex = TileMap->GetTileSize().x;
	int tiley = TileMap->GetTileSize().y;
	int xStart = tile_pos.x * tilex;
	int yStart = tile_pos.y * tiley;
	int xFinish = xStart + tilex;
	int yFinish = yStart + tiley;

	D3DLOCKED_RECT *texRect = LockTex();
	int dd = 1 << bumpTexScale[LOD];

	TerraInterface* terra=TileMap->GetTerra();
	terra->GetTileColor((char*)texRect->pBits,texRect->Pitch,
		xStart -texture_border*dd, yStart -texture_border*dd,
		xFinish+texture_border*dd, yFinish+texture_border*dd,dd);

	UnlockTex();
}

void sBumpTile::Calc(cTileMap *TileMap,bool update_texture)
{
	if(update_texture)
		CalcTexture(TileMap);
	CalcPoint(TileMap);
	init = true;
}

//////////////////////////////////////////////////////////////////////////////
void cD3DRender::PreDraw(cTileMap *TileMap)
{
	TileMap->GetTilemapRender()->PreDraw(DrawNode);
}

static bool save=false;

void cTileMapRender::DrawBump(cCamera* DrawNode,eBlendMode MatMode,TILEMAP_DRAW tile_draw,bool shadow)
{
	cCamera* pShadowMapCamera=DrawNode->FindCildCamera(ATTRCAMERA_SHADOWMAP);
	int reflection = DrawNode->GetAttribute(ATTRCAMERA_REFLECTION);
	cCamera* pNormalCamera=DrawNode->GetRoot();
	cTileMapRender* tr=TileMap->GetTilemapRender();
	bool use_shadow_map=false;

	Vect3f dcoord(
		TileMap->GetTileSize().x * TileMap->GetScale().x,
		TileMap->GetTileSize().y * TileMap->GetScale().y,
		255 * TileMap->GetScale().z),
		coord(0, 0, 0);

	gb_RenderDevice3D->SetMatrix(D3DTS_WORLD, MatXf::ID);

	// setup rendering
	if(TileMap->GetShadowMap() && !shadow && Option_ShadowType==SHADOW_MAP && 
		//gb_RenderDevice3D->dtAdvance==gb_RenderDevice3D->dtFixed && 
		!reflection)
	{
		gb_RenderDevice3D->SetNoMaterial(MatMode,0,0,TileMap->GetShadowMap(),COLOR_MOD);

		cTexture* pShadowMap=pShadowMapCamera->GetRenderTarget();
		int width=pShadowMap->GetWidth();
		float tOffs = 0;//(0.5f / width);
		D3DXMATRIX matTexAdj(0.5f,    0.0f,        0.0f,        0.0f,
							 0.0f,   -0.5f,        0.0f,        0.0f,
							 0.0f,    0.0f,        0.0f,        0.0f,
						   0.5f+tOffs,0.5f +tOffs, 1.0f,        1.0f);

		D3DXMATRIX mat;
		D3DXMatrixMultiply(&mat,pShadowMapCamera->matViewProj,&matTexAdj);
		
		// set texture coord 1
		gb_RenderDevice3D->SetTextureTransform(1, &mat);
		
		gb_RenderDevice3D->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		gb_RenderDevice3D->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MINFILTER, gb_RenderDevice3D->texture_interpolation);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MAGFILTER, gb_RenderDevice3D->texture_interpolation);
	}else
	if(TileMap->GetShadowMap() && !shadow && (Option_ShadowType!=SHADOW_MAP && Option_ShadowType!=SHADOW_MAP_PERSPECTIVE) && !reflection)
	{

		gb_RenderDevice3D->SetNoMaterial(MatMode,0,0,TileMap->GetShadowMap(),COLOR_MOD4);

		// set texture coord 1
		gb_RenderDevice3D->SetTextureTransform(1, TileMap->matLightMap);
		
		gb_RenderDevice3D->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		gb_RenderDevice3D->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	} else
	{
		gb_RenderDevice3D->SetNoMaterial(MatMode);
	}

	gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	//gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	
	gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	gb_RenderDevice3D->SetFVF(BUMP_VTXFORMAT);

	calcVisMap(DrawNode, TileMap, visMap, true);

	DWORD minfilter1=gb_RenderDevice3D->GetSamplerState(1,D3DSAMP_MINFILTER);
	DWORD magfilter1=gb_RenderDevice3D->GetSamplerState(1,D3DSAMP_MAGFILTER);

	DWORD zfunc=gb_RenderDevice3D->GetRenderState(D3DRS_ZFUNC);
	DWORD cull=gb_RenderDevice3D->GetRenderState(D3DRS_CULLMODE);
//	gb_RenderDevice3D->SetRenderState( RS_CULLMODE, D3DCULL_NONE );

	if(shadow)
	{
		if(DrawNode->GetAttribute(ATTRCAMERA_ZINVERT))
		{
			gb_RenderDevice3D->SetRenderState(D3DRS_ZFUNC,D3DCMP_GREATEREQUAL);
			gb_RenderDevice3D->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
		}

		gb_RenderDevice3D->dtAdvance->SetMaterialTilemapShadow();
	}else
	{
		if(pShadowMapCamera && !reflection)
		{
			gb_RenderDevice3D->dtAdvance->SetMaterialTilemap(TileMap);
			use_shadow_map=true;
		}
	}

int st_VBSw = 0, st_TexSw = 0, st_Poly = 0;
//start_timer(Calc_TileMap, 1);

	// clear render lists
	int i;
	for (i = 0; i < bumpTexPools.size(); i++)
		bumpTexPools[i]->tileRenderList.clear();

	// create/update tiles' vertices/textures
	int n,k;
	int dn=TileMap->GetTileNumber().y,dk=TileMap->GetTileNumber().x;
	float lod_focus=Option_MapLevel * pNormalCamera->GetFocusViewPort().x;
	float DistLevelDetail[TILEMAP_LOD] = { // was: 1,2,4,6
		0.5f * lod_focus,
		1.5f * lod_focus,
		3.0f * lod_focus,
		6.0f * lod_focus,
		12.0f* lod_focus
	};

	for (n = 0; n < dn; n++, coord.x = 0, coord.y += dcoord.y)
	for (k = 0; k < dk; k++, coord.x += dcoord.x)
//*
	if(visMap[k+n*dk])
/*/
	if (DrawNode->TestVisible(coord, coord+dcoord))
/**/
	{
		// process visible tile
		sTile &Tile = TileMap->GetTile(k, n);
		int &bumpTileID = Tile.bumpTileID;

		// calc LOD считается всегда по отгошению к прямой камере для 
		// избежания случая 2 разных LOD в одно время 
		float dist = pNormalCamera->GetPos().distance(coord + dcoord/2);
		int iLod;
		for (iLod = 0; iLod < TILEMAP_LOD; iLod++)
			if (dist < DistLevelDetail[iLod]) break;
		if (iLod >= TILEMAP_LOD) iLod = TILEMAP_LOD-1;
		vis_lod[k+n*dk]=iLod;

		// create/update render tile
		if (tr->bumpTileValid(bumpTileID)
			&& tr->bumpTiles[bumpTileID]->LOD != iLod && !shadow)
		{
			// LOD changed, free old tile and allocate new
			bumpTileFree(bumpTileID);
			bumpTileID = bumpTileAlloc(iLod,k,n);
		} else if (!bumpTileValid(bumpTileID))
		{
			// no tile assigned, allocate one
			bumpTileID = bumpTileAlloc(iLod,k,n);
		}

		sBumpTile *bumpTile = bumpTiles[bumpTileID];
		// set flags, add tile to list
		Tile.SetDraw();

		bool add=false;
		switch(tile_draw)
		{
		case TILEMAP_ALL:
			add=true;
			break;
		case TILEMAP_ZEROPLAST:
			add=bumpTile->IsZeroplast();
			break;
		case TILEMAP_NOZEROPLAST:
			add=!bumpTile->IsOnlyZeroplast();
			break;
		}

		if(add)
			bumpTexPools[bumpTile->texPool]->tileRenderList.push_back(bumpTile);
	}else
		vis_lod[k+n*dk]=-1;

//stop_timer(Calc_TileMap, 1);

	TileMap->GetTerra()->LockColumn();
	for (n = 0; n < dn; n++)
	for (k = 0; k < dk; k++)
	{
		sTile &Tile = TileMap->GetTile(k, n);
		int bumpTileID = Tile.bumpTileID;
		if(bumpTileID<0)continue;
		sBumpTile *bumpTile = bumpTiles[bumpTileID];
		int LOD=bumpTile->LOD;
		bool update_line=false;
		if(k>0)
		{
			char lod=vis_lod[k-1+n*dk];
			char& cur_lod=bumpTile->border_lod[sBumpTile::U_LEFT];
			if(lod>=LOD && cur_lod!=lod)
			{
				cur_lod=lod;
				update_line=true;
			}
		}

		if(k<dk-1)
		{
			char lod=vis_lod[k+1+n*dk];
			char& cur_lod=bumpTile->border_lod[sBumpTile::U_RIGHT];
			if(lod>=LOD && cur_lod!=lod)
			{
				cur_lod=lod;
				update_line=true;
			}
		}

		if(n>0)
		{
			char lod=vis_lod[k+(n-1)*dk];
			char& cur_lod=bumpTile->border_lod[sBumpTile::U_TOP];
			if(lod>=LOD && cur_lod!=lod)
			{
				cur_lod=lod;
				update_line=true;
			}
		}

		if(n<dn-1)
		{
			char lod=vis_lod[k+(n+1)*dk];
			char& cur_lod=bumpTile->border_lod[sBumpTile::U_BOTTOM];
			if(lod>=LOD && cur_lod!=lod)
			{
				cur_lod=lod;
				update_line=true;
			}
		}

		if((!bumpTile->init) || Tile.GetUpdate() || update_line)
		{
			bumpTile->Calc(TileMap,!bumpTile->init || Tile.GetUpdate() );
			Tile.ClearUpdate();
		}

	}
	TileMap->GetTerra()->UnlockColumn();

//	if(pNormalCamera==DrawNode)
//		gb_RenderDevice3D->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	DWORD tss_colorarg2=gb_RenderDevice3D->GetTextureStageState(0,D3DTSS_COLORARG2);
	DWORD tss_alphaarg2=gb_RenderDevice3D->GetTextureStageState(0,D3DTSS_ALPHAARG2);
	gb_RenderDevice3D->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
	gb_RenderDevice3D->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TFACTOR);

	int first_texture_number=use_shadow_map?gb_RenderDevice3D->dtAdvance->GetOffsetTextureNumber(true):0;

	int lastPool = -1, nTiles = 0;
	for (i = 0; i < bumpTexPools.size(); i++)
	{
		sTilemapTexturePool* curpool=bumpTexPools[i];
		if (curpool->tileRenderList.empty())
			continue;

		if(shadow)
		{
			gb_RenderDevice3D->SetTexture(0, gb_RenderDevice3D->dtAdvance->GetTilemapShadow0());
		}else
		{
			gb_RenderDevice3D->SetTexture(first_texture_number, curpool->GetTexture());
		}


		vector<sBumpTile*>::iterator it_tile;
		FOR_EACH(curpool->tileRenderList,it_tile)
		{
			sBumpTile *bumpTile = *it_tile;

			int iLod = bumpTile->LOD;
			int nVertex = bumpNumVertices(iLod);

			if (lastPool != bumpTile->vtx.pool)
			{
				gb_RenderDevice3D->GetVertexPool()->Select(bumpTile->vtx);
				lastPool = bumpTile->vtx.pool;
				st_VBSw++;
			}

			int nPolygon=0;
			DWORD pageSize=gb_RenderDevice3D->GetVertexPool()->GetPageSize(bumpTile->vtx);
			DWORD base_vertex_index=pageSize * bumpTile->vtx.page;

			for(int i=0;i<bumpTile->index.size();i++)
			{
				sPlayerIB& pib=bumpTile->index[i];
				if(pib.player==-1)
				{
					if(tile_draw==TILEMAP_ZEROPLAST)
						continue;
				}else
				{
					if(tile_draw==TILEMAP_NOZEROPLAST)
						continue;
				}

				sColor4f color=TileMap->GetZeroplastColor(pib.player);
				if(MatMode!=ALPHA_BLEND)
					color.a=1;

				gb_RenderDevice3D->dtAdvance->SetTileColor(color);
				if(pib.nindex==-1)
				{
					int nIndex = bumpNumIndex(iLod);
					int nIndexOffset=bumpIndexOffset(iLod);

					gb_RenderDevice3D->SetIndices(tilemapIB);

					nPolygon=nIndex/3;
					RDCALL(gb_RenderDevice3D->lpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
						base_vertex_index,
						0, nVertex, nIndexOffset, nPolygon));
				}else
				{
					int nIndex = pib.nindex;
					int nIndexOffset=gb_RenderDevice3D->GetIndexPool()->GetBaseIndex(pib.index);

					gb_RenderDevice3D->GetIndexPool()->Select(pib.index);

					nPolygon=nIndex/3;
					RDCALL(gb_RenderDevice3D->lpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
						base_vertex_index,
						0, nVertex, nIndexOffset, nPolygon));
				}
			}
	
			gb_RenderDevice3D->NumberPolygon += nPolygon;
			gb_RenderDevice3D->NumDrawObject++;
			nTiles++;
			st_Poly += nPolygon;
			
			if(false)
			if(pNormalCamera==DrawNode)
			{
				int x=bumpTile->tile_pos.x;
				int y=bumpTile->tile_pos.y;
				int xs = TileMap->GetTileSize().x;
				int ys = TileMap->GetTileSize().y;

				int z=TileMap->GetTile(x,y).zmax;

				Vect3f v00(xs*x    ,ys*y    ,z),
					   v01(xs*(x+1),ys*y    ,z),
					   v10(xs*x    ,ys*(y+1),z),
					   v11(xs*(x+1),ys*(y+1),z);

				sColor4c color(0,0,255,255);
				gb_RenderDevice->DrawLine(v00,v01,color);
				gb_RenderDevice->DrawLine(v01,v11,color);
				gb_RenderDevice->DrawLine(v11,v10,color);
				gb_RenderDevice->DrawLine(v10,v00,color);
			}
		}
	}

	if(save && !shadow)
	{
		gb_RenderDevice3D->SaveStates();
	}

	// restore state
	gb_RenderDevice3D->SetTextureStageState(0,D3DTSS_COLORARG2,tss_colorarg2);
	gb_RenderDevice3D->SetTextureStageState(0,D3DTSS_ALPHAARG2,tss_alphaarg2);

	gb_RenderDevice3D->SetSamplerState( 0, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP );
	gb_RenderDevice3D->SetSamplerState( 0, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP );
	gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP );
	gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP );
	gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_MINFILTER, minfilter1);
	gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_MAGFILTER, magfilter1);

	gb_RenderDevice3D->SetVertexShader(NULL);
	gb_RenderDevice3D->SetPixelShader(NULL);

	gb_RenderDevice3D->SetRenderState(D3DRS_ZFUNC,zfunc);
	gb_RenderDevice3D->SetRenderState(D3DRS_CULLMODE,cull);

	gb_RenderDevice3D->SetTexture(NULL,0,3);
	gb_RenderDevice3D->SetTexture(NULL,0,4);
	gb_RenderDevice3D->SetTexture(NULL,0,5);
	//gb_RenderDevice3D->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
/*
char msg[256];
int st_mem = 0;
for (i = 0; i < bumpTexPools.size(); i++)
	st_mem += bumpTexPools[i]->tileWidth * BUMP_TEXPOOL_H * 4;
for (i = 0; i < bumpVtxPools.size(); i++)
	st_mem += bumpVtxPools[i]->totalPages * bumpVtxPools[i]->pageSize * bumpVtxPools[i]->vertexSize;
sprintf(msg, "tiles=%d vbsw=%d texsw=%d poly=%d texpools=%d mem=%d",
	nTiles, st_VBSw, st_TexSw, st_Poly, bumpTexPools.size(), st_mem);
OutText(0, 80, msg);
/**/
	if(false)//shadow)//pNormalCamera==DrawNode)
	{
		int dx=TileMap->GetTileNumber().x,dy=TileMap->GetTileNumber().y;
		int minx=9,miny=888 - 256;

		int r,g,b,a=80;
		if(reflection)
		{
			r=255;
			g=b=0;
		}else
		{
			r=0;
			g=b=255;
		}

		int mul=dx>32?2:4;

		for(int y=0;y<dy;y++)
		for(int x=0;x<dx;x++)
		{
			if(visMap[y*dx+x])
			{
				int xx=x*mul+minx,yy=y*mul+miny;
				gb_RenderDevice3D->DrawRectangle(xx,yy,mul,mul,sColor4c(r,g,b,a));
			}
		}
	}

	if(false)//shadow)
	{
		CMesh cmesh;
		calcCMesh(pNormalCamera,TileMap->GetTileNumber(),TileMap->GetTileSize(),cmesh);
		drawCMesh(cmesh);
		gb_RenderDevice3D->FlushPrimitive3D();
	}
}

void sBumpTile::CalcPoint(cTileMap *TileMap)
{
	Column** columns=TileMap->GetColumn();
	Vect2i pos=tile_pos;

	TileMap->GetTilemapRender()->IncUpdate(this);

	int tilenumber=TileMap->GetZeroplastNumber();
	int step=bumpGeoScale[LOD];

	char dd=TILEMAP_SIZE>>step;
	int xstep=1<<step;
	int xstep2=1<<(step-1);

	int minx=pos.x*TILEMAP_SIZE;
	int miny=pos.y*TILEMAP_SIZE;
	int maxx=minx+TILEMAP_SIZE;
	int maxy=miny+TILEMAP_SIZE;

	int ddv=dd+1;
	VectDelta* points=TileMap->GetTilemapRender()->GetDeltaBuffer();

	int x,y;

	for(y=0;y<ddv;y++)
	for(x=0;x<ddv;x++)
	{
		VectDelta& p=points[x+y*ddv];
		p.set(x*xstep+minx,y*xstep+miny);
		p.delta.set(0,0);
		p.delta2=INT_MAX;
		p.player=0;
		p.fix=0;

		int xx=min(p.x,maxx-1);
		int yy=min(p.y,maxy-1);
		if(columns)
		for(int player=0;player<tilenumber;player++)
		{
			if(columns[player]->filled(xx,yy))
			{
				xassert(p.player==0);
				p.player=player+1;
			}
		}
	}

	int fix_out=FixLine(points,ddv,TileMap);

	int preceeded_point=0;
	for(int player=0;player<tilenumber;player++)
	{
		char dd2=dd/2;
		for(int y=-1;y<=1;y++)
		for(int x=-1;x<=1;x++)
		{
			Vect2i cur_tile_pos(tile_pos.x+x,tile_pos.y+y);
			if(cur_tile_pos.x>=0 && cur_tile_pos.x<TileMap->GetTileNumber().x)
			if(cur_tile_pos.y>=0 && cur_tile_pos.y<TileMap->GetTileNumber().y)
			{
				vector<Vect2s>* region=TileMap->GetCurRegion(cur_tile_pos,player);
				vector<Vect2s>::iterator it;
				preceeded_point+=region->size();

				bool fix_border_x=false;
				bool fix_border_y=false;
				if(x==-1 && (fix_out&(1<<U_LEFT)))
					fix_border_x=true;
				if(y==-1 && (fix_out&(1<<U_TOP)))
					fix_border_y=true;
				if(x==+1 && (fix_out&(1<<U_RIGHT)))
					fix_border_x=true;
				if(y==+1 && (fix_out&(1<<U_BOTTOM)))
					fix_border_y=true;

				FOR_EACH(*region,it)
				{
					Vect2i p(it->x,it->y);

					Vect2i pround;
					VectDelta* pnt=NULL;

					pround.x=(p.x-minx+xstep)>>(step+1);
					pround.y=(p.y-miny+xstep)>>(step+1);
					if(!(pround.x<0 || pround.x>dd2 || pround.y<0 || pround.y>dd2))
					{
						pround.x*=2;
						pround.y*=2;
						pnt=&points[pround.x+pround.y*ddv];
						if(!pnt->fix)
						{
							pnt=NULL;
						}
					}

					if(!pnt)
					{
						pround.x=(p.x-minx+xstep2)>>step;
						pround.y=(p.y-miny+xstep2)>>step;
						if(pround.x<0 || pround.x>dd || pround.y<0 || pround.y>dd)
							continue;
					}

					if(pround.x<0)
					{
						VISASSERT(0);
						pround.x=0;
					}
					if(pround.y<0)
					{
						VISASSERT(0);
						pround.y=0;
					}
					if(pround.x>dd)
					{
						VISASSERT(0);
						pround.x=dd;
					}
					if(pround.y>dd)
					{
						VISASSERT(0);
						pround.y=dd;
					}

					pnt=&points[pround.x+pround.y*ddv];
					if(pnt->fix&VectDelta::FIX_FIXED)
					{
						pround.x&=~1;
						pround.y&=~1;
						pnt=&points[pround.x+pround.y*ddv];
						VISASSERT(pnt->fix&(VectDelta::FIX_RIGHT|VectDelta::FIX_BOTTOM));
					}

					int delta2=sqr(pnt->x-p.x)+sqr(pnt->y-p.y);
					if(delta2<pnt->delta2)
					{
						pnt->delta2=delta2;
						pnt->delta=p-*pnt;
						pnt->player=-1;
						if(pnt->fix)
						{
							if(pnt->fix&VectDelta::FIX_RIGHT)
							{
								VectDelta& p=points[pround.x+1+pround.y*ddv];
								VISASSERT(pnt->x==p.x && pnt->y==p.y);
								VISASSERT(p.fix==VectDelta::FIX_FIXED);
								p.copy_no_fix(*pnt);
							}
							if(pnt->fix&VectDelta::FIX_BOTTOM)
							{
								VectDelta& p=points[pround.x+(pround.y+1)*ddv];
								VISASSERT(pnt->x==p.x && pnt->y==p.y);
								VISASSERT(p.fix==VectDelta::FIX_FIXED);
								p.copy_no_fix(*pnt);
							}
						}
					}
				}
			}
		}
	}

	vector<vector<sPolygon> >& index=TileMap->GetTilemapRender()->GetIndexBuffer();

	{
		index.resize(tilenumber+1);
		for(int i=0;i<index.size();i++)
			index[i].clear();
	}

	sPolygon poly;
	for(y=0;y<dd;y++)
	for(x=0;x<dd;x++)
	{
		int base=x+y*ddv;
		VectDelta *p00,*p01,*p10,*p11;
		p00=points+base;
		p01=points+(base+1);
		p10=points+(base+ddv);
		p11=points+(base+ddv+1);

		bool no;
		char cur_player_add;

#define X(p) \
		{ \
			if(cur_player_add<0) \
				cur_player_add=p->player; \
			else \
				if(p->player>=0 && cur_player_add!=p->player)\
					no=true;\
		}

		if(p01->player==p10->player || p01->player<0 || p10->player<0)
		{
			cur_player_add=-1;
			no=false;
			X(p00);
			X(p01);
			X(p10);

			if(cur_player_add<0 || no)
			{
				int x=(p00->x+p00->delta.x+p01->x+p01->delta.x+p10->x+p10->delta.x)/3;
				int y=(p00->y+p00->delta.y+p01->y+p01->delta.y+p10->y+p10->delta.y)/3;
				cur_player_add=0;
				if(columns)
				for(int i=0;i<tilenumber;i++)
				if(columns[i]->filled(x,y))
				{
					cur_player_add=i+1;
					break;
				}
			}

			xassert(cur_player_add>=0 && cur_player_add<=tilenumber);
			poly.set(base,base+ddv,base+1);
			index[cur_player_add].push_back(poly);

			cur_player_add=-1;
			no=false;
			X(p01);
			X(p10);
			X(p11);

			if(cur_player_add<0 || no)
			{
				int x=(p01->x+p01->delta.x+p10->x+p10->delta.x+p11->x+p11->delta.x)/3;
				int y=(p01->y+p01->delta.y+p10->y+p10->delta.y+p11->y+p11->delta.y)/3;
				cur_player_add=0;
				if(columns)
				for(int i=0;i<tilenumber;i++)
				if(columns[i]->filled(x,y))
				{
					cur_player_add=i+1;
					break;
				}
			}

			xassert(cur_player_add>=0 && cur_player_add<=tilenumber);
			poly.set(base+ddv,base+ddv+1,base+1);
			index[cur_player_add].push_back(poly);
		}else
		{
			cur_player_add=-1;
			no=false;
			X(p00);
			X(p01);
			X(p11);


			if(cur_player_add<0 || no)
			{
				int x=(p00->x+p00->delta.x+p01->x+p01->delta.x+p11->x+p11->delta.x)/3;
				int y=(p00->y+p00->delta.y+p01->y+p01->delta.y+p11->y+p11->delta.y)/3;
				cur_player_add=0;
				if(columns)
				for(int i=0;i<tilenumber;i++)
				if(columns[i]->filled(x,y))
				{
					cur_player_add=i+1;
					break;
				}
			}

			xassert(cur_player_add>=0 && cur_player_add<=tilenumber);
			poly.set(base,base+ddv+1,base+1);
			index[cur_player_add].push_back(poly);

			cur_player_add=-1;
			no=false;
			X(p00);
			X(p10);
			X(p11);

			if(cur_player_add<0 || no)
			{
				int x=(p00->x+p00->delta.x+p10->x+p10->delta.x+p11->x+p11->delta.x)/3;
				int y=(p00->y+p00->delta.y+p10->y+p10->delta.y+p11->y+p11->delta.y)/3;
				cur_player_add=0;
				if(columns)
				for(int i=0;i<tilenumber;i++)
				if(columns[i]->filled(x,y))
				{
					cur_player_add=i+1;
					break;
				}
			}

			xassert(cur_player_add>=0 && cur_player_add<=tilenumber);
			poly.set(base,base+ddv,base+ddv+1);
			index[cur_player_add].push_back(poly);
		}
	}
#undef X

	/////////////////////set vertex buffer
	int is = TileMap->GetTileSize().x; // tile size in vMap units
	int js = TileMap->GetTileSize().y;
	int xMap = is * TileMap->GetTileNumber().x; // vMap size
	int yMap = js * TileMap->GetTileNumber().y;
	int xStart = tile_pos.x * is;
	int yStart = tile_pos.y * js;
	int xFinish = xStart + is;
	int yFinish = yStart + js;

	float ux_step=uStep/(1 << bumpGeoScale[LOD]);
	float ux_base=uStart-xStart*ux_step;
	float vy_step=vStep/(1 << bumpGeoScale[LOD]);
	float vy_base=vStart-yStart*vy_step;


	BUMP_VTXTYPE *vb,*vbbegin;
	vbbegin=vb= (BUMP_VTXTYPE*)LockVB();

	TerraInterface* terra=TileMap->GetTerra();

	for(y=0;y<ddv;y++)
	for(x=0;x<ddv;x++)
	{
		int i=x+y*ddv;
		VectDelta& p=points[i];
		int xx=p.x+p.delta.x;
		int yy=p.y+p.delta.y;

		vb->pos.x = xx;
		vb->pos.y = yy;
		vb->pos.z=//terra->GetHZeroPlast();
				  SetVertexZ(terra,IUCLAMP(xx, xMap),IUCLAMP(yy, yMap));
		
		vb->u1() = xx*ux_step+ux_base;
		vb->v1() = yy*vy_step+vy_base;
		vb++;
	}

	UnlockVB();

	////////////////////set index buffer
	DeleteIndex();

	int num_non_empty=0;
	int one_player=0;
	for(int i=0;i<index.size();i++)
	{
		if(index[i].size()>0)
		{
			num_non_empty++;
			one_player=i-1;
		}
	}

	if(num_non_empty<=1)
	{
		sBumpTile::index.resize(1);
		sBumpTile::index[0].player=one_player;
		sBumpTile::index[0].nindex=-1;
	}else
	{
		int sum_index=0;
		sBumpTile::index.resize(num_non_empty);
		int cur=0;
		for(int i=0;i<index.size();i++)
		if(index[i].size()>0)
		{
			int num=index[i].size()*3;
			sum_index+=num;
			int num2=Power2up(num);
			gb_RenderDevice3D->GetIndexPool()->CreatePage(sBumpTile::index[cur].index,num2);

			sPolygon* p=gb_RenderDevice3D->GetIndexPool()->LockPage(sBumpTile::index[cur].index);
			memcpy(p,&(index[i][0]),num*sizeof(WORD));
			gb_RenderDevice3D->GetIndexPool()->UnlockPage(sBumpTile::index[cur].index);

			sBumpTile::index[cur].nindex=num;
			sBumpTile::index[cur].player=i-1;
			cur++;
		}

		int lod_index=TileMap->GetTilemapRender()->bumpNumIndex(LOD);
		VISASSERT(lod_index==sum_index);
	}
}

int sBumpTile::FixLine(VectDelta* points,int ddv,cTileMap *TileMap)
{
	int fix_out=0;
	int dmax=ddv*ddv;
	int dv=ddv-1;
	for(int side=0;side<U_ALL;side++)
	{
		if(border_lod[side]<=LOD)
			continue;
//		VISASSERT(border_lod[side]==LOD+1);

		fix_out|=1<<side;
		
		VectDelta* cur;
		int delta;
		char fix=0;
		switch(side)
		{
		case U_LEFT:
			cur=points;
			delta=ddv;
			fix=VectDelta::FIX_BOTTOM;
			break;
		case U_RIGHT:
			cur=points+ddv-1;
			delta=ddv;
			fix=VectDelta::FIX_BOTTOM;
			points[ddv*ddv-1].fix|=VectDelta::FIX_FIXEMPTY;
			break;
		case U_TOP:
			cur=points;
			delta=1;
			fix=VectDelta::FIX_RIGHT;
			break;
		case U_BOTTOM:
			cur=points+ddv*(ddv-1);
			delta=1;
			fix=VectDelta::FIX_RIGHT;
			points[ddv*ddv-1].fix|=VectDelta::FIX_FIXEMPTY;
			break;
		}

		for(int i=0;i<dv;i+=2,cur+=2*delta)
		{
//			VISASSERT(cur-points<dmax);
			VectDelta* p=cur+delta;
//			VISASSERT(p-points<dmax);
			p->copy_no_fix(*cur);
			//*p=*cur;
			VISASSERT(p->fix==0);
			cur->fix|=fix;
			p->fix|=VectDelta::FIX_FIXED;
		}
	}


	return fix_out;
}

void cTileMapRender::IncUpdate(sBumpTile* pbump)
{
	if(update_stat)
	{
		int dx=TileMap->GetTileNumber().x;
		int dxy=dx*TileMap->GetTileNumber().y;
		update_in_frame=true;
		update_stat[pbump->LOD*dxy+pbump->tile_pos.y*dx+pbump->tile_pos.x]++;
	}
}


bool SaveTga(const char* filename,int width,int height,unsigned char* buf,int byte_per_pixel);
void cTileMapRender::SaveUpdateStat()
{
	static int tga_num=-1;
	tga_num++;
	if(!update_in_frame)return;
	int dx=TileMap->GetTileNumber().x,dy=TileMap->GetTileNumber().y;
	int dxy=dx*dy;

	BYTE* buf=new BYTE[dxy*4];
	memset(buf,0,dxy*4);
	for(int y=0;y<dy;y++)
	for(int x=0;x<dx;x++)
	{
		BYTE* color=&buf[(y*dx+x)*4];
		int num_update=0;
		int lod=-1;
		bool change_lod=false;
		for(int i=0;i<TILEMAP_LOD;i++)
		{
			char u=update_stat[i*dxy+y*dx+x];
			num_update+=u;
			if(u)
			{
				if(lod==-1)
					lod=i;
				else
					change_lod=true;
			}
		}

		if(change_lod)
			color[0]=128;
		color[1]=min(num_update,4)*63;
	}

	char fname[100];
	sprintf(fname,"tga\\%05i.tga",tga_num);
	SaveTga(fname,dx,dy,buf,4);
	delete buf;

}