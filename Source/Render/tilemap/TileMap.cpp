#include <set>
#include "StdAfxRD.h"
#ifdef PERIMETER_D3D9
#include "D3DRender.h"
#endif
#include "TileMap.h"
#include "Scene.h"
#include "ObjLibrary.h"
#include "../../Game/Region.h"
#include "Font.h"
#include "PoolManager.h"
#include "TileMapRender.h"
#include "ClippingMesh.h"


static void fillVisPoly(uint8_t *buf, std::vector<Vect2f>& vert, int VISMAP_W, int VISMAP_H)
{
    if(vert.empty())return;
    const int VISMAP_W_MAX=128,VISMAP_H_MAX=128;
    VISASSERT(VISMAP_W<=VISMAP_W_MAX && VISMAP_H<=VISMAP_H_MAX);
    float lx[VISMAP_W_MAX], rx[VISMAP_H_MAX];
    int i, y, ytop, ybot;

    // find top/bottom y
    ytop = xm::floor(vert[0].y);
    ybot = xm::ceil(vert[0].y);
    for(i=1;i<vert.size();i++)
    {
        float y=vert[i].y;
        if (y < ytop) ytop = xm::floor(y);
        if (y > ybot) ybot = xm::ceil(y);
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
        int x1 = (int)max((float)xm::floor(lx[y]), 0.0f);
        int x2 = (int)min((float)xm::ceil(rx[y]), (float)VISMAP_W);
        if(x1>=x2)continue;
        memset(buf + y*VISMAP_W + x1, 1, x2-x1);
    }
}

void calcCMesh(cCamera *DrawNode, Vect2i TileNumber,Vect2i TileSize,CMesh& cmesh)
{
    AMesh mesh;
    float dx=TileNumber.x*TileSize.x;
    float dy=TileNumber.y*TileSize.y;
    Vect3f vmin(0,0,0);
    Vect3f vmax(dx,dy,256.0f);
    mesh.CreateABB(vmin, vmax);

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

void calcVisMapCMesh(cCamera *DrawNode, CMesh& cmesh, Vect2i TileNumber, Vect2i TileSize, uint8_t* visMap, bool clear)
{
    APolygons poly;
    cmesh.BuildPolygon(poly);
    if(clear)
        memset(visMap, 0, TileNumber.x*TileNumber.y);

    for(int i=0;i<poly.faces.size();i++)
    {
        std::vector<int>& inp=poly.faces[i];
        std::vector<Vect2f> points(inp.size());
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

void calcVisMap(cCamera *DrawNode, Vect2i TileNumber, Vect2i TileSize, uint8_t* visMap, bool clear)
{
}

void calcVisMap(cCamera *DrawNode, Vect2i TileNumber,Vect2i TileSize,Mat3f& direction,sBox6f& box)
{
}

void cTileMap::calcVisMap(cCamera *DrawNode, Vect2i TileNumber, Vect2i TileSize, uint8_t* visMap, bool clear) {
    CMesh cmesh;
    calcCMesh(DrawNode,TileNumber,TileSize,cmesh);
    calcVisMapCMesh(DrawNode,cmesh,TileNumber,TileSize,visMap,clear);
}

void cTileMap::calcVisMap(cCamera *DrawNode, Vect2i TileNumber,Vect2i TileSize, Mat3f& direction,sBox6f& box) {
    CMesh cmesh;
    calcCMesh(DrawNode,TileNumber,TileSize,cmesh);
    box=calcBoundInDirection(cmesh,direction);
}

cTileMap::cTileMap(cScene* pScene,TerraInterface* terra_) : cUnkObj(KIND_TILEMAP)
{
	MTINIT(lock_update_rect);
	terra=terra_;
	TileSize.set(TILEMAP_SIZE,TILEMAP_SIZE);
	TileNumber.set(0,0);
	Tile=0;
    TexturePoolSize = 512;

	tilesize.set(0,0,0);
	pTileMapRender=NULL;

	ShadowDrawNode=pScene->CreateCamera();
	LightDrawNode=new cCameraPlanarLight(pScene);
	LightMapType=0;
	zeroplastnumber=0;
	enable_debug_rect=false;
	debug_fade_interval=500;

	//Since we cant call SetScene in ctor
	IParent = pScene;
}
cTileMap::~cTileMap()
{
	RELEASE(terra);
	RELEASE(ShadowDrawNode);
	RELEASE(LightDrawNode);
    gb_RenderDevice->DeleteTilemap(this);
	if(Tile) { delete [] Tile; Tile=nullptr; }
	MTDONE(lock_update_rect);
    xassert(pTileMapRender == nullptr);
}

int cTileMap::CheckLightMapType()
{
	return Option_ShadowType;
}

cTexture* cTileMap::GetShadowMap()
{
#ifdef PERIMETER_D3D9
    if (gb_RenderDevice3D) {
        if (CheckLightMapType()) {
            return gb_RenderDevice3D->dtAdvance->GetShadowMap();
        } else {
            return gb_RenderDevice3D->dtFixed->GetShadowMap();
        }
    }
#endif
    return nullptr;
}

cTexture* cTileMap::GetLightMap()
{
#ifdef PERIMETER_D3D9
    if (gb_RenderDevice3D) {
        if (CheckLightMapType()) {
            return gb_RenderDevice3D->dtAdvance->GetLightMap();
        } else {
            return gb_RenderDevice3D->dtFixed->GetLightMap();
        }
    }
#endif
    return nullptr;
}

#ifdef PERIMETER_D3D9
IDirect3DSurface9* cTileMap::GetZBuffer()
{
    if (gb_RenderDevice3D) {
        if (CheckLightMapType()) {
            return gb_RenderDevice3D->dtAdvance->GetZBuffer();
        } else {
            return gb_RenderDevice3D->dtFixed->GetZBuffer();
        }
    }
    return nullptr;
}
#endif

void cTileMap::CreateLightmap()
{
#ifdef PERIMETER_D3D9
    if (gb_RenderDevice3D) {
        gb_RenderDevice3D->dtFixed->DeleteShadowTexture();
        gb_RenderDevice3D->dtAdvance->DeleteShadowTexture();
    }

	int width=256<<(Option_DrawMeshShadow-1);

	LightMapType=CheckLightMapType();

	if(Option_DrawMeshShadow>0 && gb_RenderDevice3D)
	{
		DrawType* draw=gb_RenderDevice3D->dtFixed;
		if(LightMapType) {
            draw = gb_RenderDevice3D->dtAdvance;
        }

		if(!draw->CreateShadowTexture(width))
		{
			gb_VisGeneric->SetShadowType((eShadowType)(int)Option_ShadowType,0);
		}
	}
#endif

	float SizeLightMap=terra->SizeX();
	float focus=1/SizeLightMap;
	matLightMap = Mat4f::ID;
	matLightMap.xx *= focus;
	matLightMap.yx *= focus;
	matLightMap.zx *= focus;
	matLightMap.xy *= focus;
	matLightMap.yy *= focus;
	matLightMap.zy *= focus;

	GetTexLibrary()->Compact();
}

//////////////////////////////////////////////////////////////////////////////////////////
// реализация интерфейса cIUnkObj
//////////////////////////////////////////////////////////////////////////////////////////

void cTileMap::PreDraw(cCamera *DrawNode)
{
	if(GetAttribute(ATTRUNKOBJ_IGNORE)) return;

	BuildRegionPoint();

	DrawNode->Attach(SCENENODE_OBJECT_TILEMAP,this);
    
    cTileMapRender* render = GetTilemapRender();
    if (render) {
        render->PreDraw(DrawNode);
    }
}

void cTileMap::Draw(cCamera *DrawNode)
{
	if(!Option_ShowType[SHOW_TILEMAP])
		return;

    cTileMapRender* render = GetTilemapRender();
    if (!render) return;

	if(DrawNode->GetAttribute(ATTRCAMERA_SHADOW))
	{
        gb_RenderDevice->Draw(GetScene()); // рисовать источники света
	}
	else if(DrawNode->GetAttribute(ATTRCAMERA_SHADOWMAP))
	{
		if(Option_ShadowType==SHADOW_MAP_SELF) {
            render->DrawBump(DrawNode, ALPHA_TEST, TILEMAP_ALL, true);
        }
	}
	else if(DrawNode->GetAttribute(ATTRCAMERA_REFLECTION))
	{ // рисовать отражение
		gb_RenderDevice->SetRenderState(RS_ALPHA_TEST_MODE, ALPHATEST_GT_254/*GetRefSurface()*/);
        render->DrawBump(DrawNode, ALPHA_TEST, TILEMAP_NOZEROPLAST, false);
		gb_RenderDevice->SetRenderState(RS_ALPHA_TEST_MODE, ALPHATEST_GT_0);
	}else
	{
		if(GetAttribute(ATTRUNKOBJ_REFLECTION)) {
		    // рисовать прямое изображение
			gb_RenderDevice->SetRenderState(RS_ALPHA_TEST_MODE, ALPHATEST_GT_1);
			render->DrawBump(DrawNode, ALPHA_BLEND, TILEMAP_ZEROPLAST, false);
			render->DrawBump(DrawNode, ALPHA_NONE, TILEMAP_NOZEROPLAST, false);
			gb_RenderDevice->SetRenderState(RS_ALPHA_TEST_MODE, ALPHATEST_GT_0);
		} else {
			render->DrawBump(DrawNode, ALPHA_NONE, TILEMAP_ALL, false);
		}
	}

	if(false)
	if(!DrawNode->GetParent())
	{
		int npoint=0;
		int zeroh=terra->GetHZeroPlast();
		for(int y=0;y<TileNumber.y;y++)
		for(int x=0;x<TileNumber.x;x++)
		{
			sTile& tile=GetTile(x,y);

			for(int ir=0;ir<tile.region_point.size();ir++)
			{
				std::vector<Vect2s>& point=tile.region_point[ir];
				npoint+=point.size();
				std::vector<Vect2s>::iterator it;
				FOR_EACH(point,it)
				{
					gb_RenderDevice->DrawPoint(Vect3f(it->x, it->y, zeroh), sColor4c(0, 255, 255, 100));
				}
			}
		}

		gb_RenderDevice->FlushPrimitive3D();

		cFont* pFont=gb_VisGeneric->CreateDebugFont();
		gb_RenderDevice->SetFont(pFont);
		char s[128];
		sprintf(s,"point=%i",npoint);
		gb_RenderDevice->OutText(10, 50, s, sColor4f(1, 1, 1, 1));
		gb_RenderDevice->SetFont(NULL);
		pFont->Release();
	}

	DrawLines();
}

//////////////////////////////////////////////////////////////////////////////////////////
// реализация cTileMap
//////////////////////////////////////////////////////////////////////////////////////////
void cTileMap::UpdateMap(const Vect2i& pos1,const Vect2i& pos2)
{
	MTEnter enter(lock_update_rect);
	UpdateRect rc;
	rc.p1=pos1;
	rc.p2=pos2;
	update_rect.push_back(rc);

	if(enable_debug_rect)
	{
		DebugRect rc;
		rc.p1=pos1;
		rc.p2=pos2;
		rc.time=debug_fade_interval;
		debug_rect.push_back(rc);
	}

	std::vector<UpdateMapData>::iterator it;
	FOR_EACH(func_update_map,it)
	{
		it->f(pos1,pos2,it->data);
	}
}

void cTileMap::SetBuffer(const Vect2i &size,int zeroplastnumber_)
{
	zeroplastnumber=zeroplastnumber_;
	zeroplast_color.resize(zeroplastnumber);
	for (int i=0;i<zeroplastnumber;i++) {
        zeroplast_color[i] = sColor4f(0, 1, 0, 1);
    }

	if (Tile) {
        gb_RenderDevice->DeleteTilemap(this);
        delete Tile;
        Tile = nullptr;
    }
    
	TileNumber.set(size.x/GetTileSize().x,size.y/GetTileSize().y);
	VISASSERT(TileNumber.x*GetTileSize().x==size.x);
	VISASSERT(TileNumber.y*GetTileSize().y==size.y);

	Tile = new sTile[GetTileNumber().x*GetTileNumber().y];
	gb_RenderDevice->CreateTilemap(this);

	UpdateMap(Vect2i(0,0), Vect2i(size.x-1,size.y-1));
}


void cTileMap::DrawLightmapShadow(cCamera *DrawNode)
{
#ifdef PERIMETER_D3D9
    if (!gb_RenderDevice3D) return;
#else
    return;
#endif
	if (Option_DrawMeshShadow && GetShadowMap()==nullptr
#ifdef PERIMETER_D3D9
    && gb_RenderDevice3D && gb_RenderDevice3D->nSupportTexture>1
#endif
    ) {
		CreateLightmap();
	} else if (GetShadowMap()) {
        if ((256 << (Option_DrawMeshShadow - 1)) != GetShadowMap()->GetWidth() ||
            LightMapType != CheckLightMapType()) {
            CreateLightmap();
        }
    }

	if(GetShadowMap()) 
	{ // shadow
		if(Option_IsShadowMap)
		{
			AddLightCamera(DrawNode);
			if(Option_ShadowType==SHADOW_MAP_SELF)
				AddPlanarCamera(DrawNode,true);
		}else
		{
			AddPlanarCamera(DrawNode,false);
		}
	}
}

void cTileMap::AddLightCamera(cCamera *DrawNode)
{
#ifdef PERIMETER_D3D9
    if (!gb_RenderDevice3D) return;
#else
    return;
#endif
	DrawNode->SetAttribute(ATTRCAMERA_ZMINMAX);
	DrawNode->SetCopy(ShadowDrawNode);
	DrawNode->AttachChild(ShadowDrawNode);
	ShadowDrawNode->SetAttribute(ATTRCAMERA_SHADOWMAP|ATTRUNKOBJ_NOLIGHT);
	ShadowDrawNode->ClearAttribute(ATTRCAMERA_PERSPECTIVE|ATTRCAMERA_ZMINMAX|ATTRCAMERA_SHOWCLIP);
#ifdef PERIMETER_D3D9
	ShadowDrawNode->SetRenderTarget(GetShadowMap(),GetZBuffer());
#endif


//	Vect2f z=CalcZ(DrawNode);

//	if(Option_ShadowType!=SHADOW_MAP_PERSPECTIVE)
//		z.x=30.0f;

//	Vect2f zplane=DrawNode->GetZPlane();
//	DrawNode->SetZPlaneTemp(z);

	if(Option_ShadowType==SHADOW_MAP_PERSPECTIVE)
		CalcShadowMapCameraProective(DrawNode);
	else
		CalcShadowMapCamera(DrawNode);
	
	ShadowDrawNode->Attach(SCENENODE_OBJECT,this);

//	DrawNode->SetZPlaneTemp(zplane);
}

void cTileMap::CalcShadowMapCamera(cCamera *DrawNode)
{
	Vect3f LightDirection(0,0,-1);
	DrawNode->GetScene()->GetLighting(&LightDirection);
	MatXf LightMatrix;
	LightMatrix.rot().zrow()=LightDirection;
	LightMatrix.rot().xrow().cross(Vect3f(1,0,0),LightMatrix.rot().zrow());
	LightMatrix.rot().xrow().Normalize();
	LightMatrix.rot().yrow().cross(LightMatrix.rot().zrow(),LightMatrix.rot().xrow());

	const float bound_z=1000.0f;

	Mat3f LightMatrixInv;
	sBox6f box;
	LightMatrixInv.invert(LightMatrix.rot());
    cTileMap::calcVisMap(DrawNode,GetTileNumber(),GetTileSize(),LightMatrix.rot(),box);

	Vect2f prevz(box.min.z,box.max.z);
	box.min.z-=bound_z;
	box.max.z+=bound_z;

	Vect3f PosLight((box.min.x+box.max.x)*0.5f,(box.min.y+box.max.y)*0.5f,box.min.z);
	LightMatrix.trans()=-PosLight;

	Vect2f Focus(1/(box.max.x-box.min.x),1/(box.max.y-box.min.y));
    Vect2f center(0.5f,0.5f);
    sRectangle4f clip(-0.5f,-0.5f,0.5f,0.5f);
    Vect2f zplane(0,box.max.z-box.min.z);
    ShadowDrawNode->SetFrustum(&center,&clip, &Focus, &zplane);
	ShadowDrawNode->SetPosition(LightMatrix);

//С одной стороны эта камера должна быть посчитана до того момента
//когда начнёт определяться, какие объекты видимы. С другой стороны она должна быть
//посчитанна позже, так как вызывается CalculateZMinMax
	fix_shadow.box=box;
	fix_shadow.LightMatrix=LightMatrix;

}

void cTileMap::FixShadowMapCamera(cCamera *DrawNode)
{
	if(!(Option_DrawMeshShadow && (Option_ShadowType==SHADOW_MAP_SELF || Option_ShadowType==SHADOW_MAP)))
		return;
	sBox6f box=fix_shadow.box;
	MatXf LightMatrix=fix_shadow.LightMatrix;

	Vect2f boxz=CalcZ(ShadowDrawNode);
	Vect2f objectz=DrawNode->CalculateZMinMax(ShadowDrawNode->GetMatrix());
	boxz.x=min(boxz.x,objectz.x);
	boxz.y=max(boxz.y,objectz.y);

	box.min.z+=boxz.x;//-DrawNode->GetZPlane().x;
	box.max.z=box.min.z+(boxz.y-boxz.x);

	Vect3f PosLight((box.min.x+box.max.x)*0.5f,(box.min.y+box.max.y)*0.5f,box.min.z);
	LightMatrix.trans()=-PosLight;

	Vect2f Focus(1/(box.max.x-box.min.x),1/(box.max.y-box.min.y));
    Vect2f center(0.5f,0.5f);
    sRectangle4f clip(-0.5f,-0.5f,0.5f,0.5f);
    Vect2f zplane(0,box.max.z-box.min.z);
    ShadowDrawNode->SetFrustum(&center,&clip, &Focus, &zplane);
	ShadowDrawNode->SetPosition(LightMatrix);
/*
	if(LightDrawNode->GetAttribute(ATTRCAMERA_NOCLEARTARGET))
	{//Освещение при проективной камере
//		Vect2f PosLightMap;
//		PosLightMap.x = (box.max.x+box.min.x)*0.5f;
//		PosLightMap.y = (box.max.y+box.min.y)*0.5f;

//		Vect3f vShadow(0,0,-1);//Всё равно с других положений криво освещает

//		Vect3f PosLight(PosLightMap.x,PosLightMap.y,-5000*vShadow.z);
		Vect3f LightDirection(0,0,-1);
		MatXf LightMatrix;
		LightMatrix.rot().zrow()=LightDirection;
		LightMatrix.rot().xrow().cross(Vect3f(1,0,0),LightMatrix.rot().zrow());
		LightMatrix.rot().xrow().Normalize();
		LightMatrix.rot().yrow().cross(LightMatrix.rot().zrow(),LightMatrix.rot().xrow());
		Vect3f PosLight((box.min.x+box.max.x)*0.5f,(box.min.y+box.max.y)*0.5f,-5000);
		LightMatrix.trans()=-PosLight;

		Vect2f Focus(1/(box.max.x-box.min.x),1/(box.max.y-box.min.y));
		LightDrawNode->SetFrustum(&Vect2f(0.5f,0.5f),&sRectangle4f(-0.5f,-0.5f,0.5f,0.5f),
			&Focus, &Vect2f(10,1e6f));
		
		LightDrawNode->SetPosition(LightMatrix);
	}
*/
}


sBox6f CalcZMinMax(const Mat4f* look)
{
	Vect3f in[8]=
	{
	Vect3f(-1,	1,	0),
	Vect3f(1,	1,	0),
	Vect3f(-1,	-1,	0),
	Vect3f(1,	-1,	0),

	Vect3f(-1,	1,	1),
	Vect3f(1,	1,	1),
	Vect3f(-1,	-1,	1),
	Vect3f(1,	-1,	1),
	};

	sBox6f box;
	box.SetInvalidBox();

	for(int i=0;i<8;i++)
	{
		Vect4f out;
        look->xform(in[i], out);
		out.x/=out.w;
		out.y/=out.w;
		out.z/=out.w;
		out.w/=out.w;

		out.x/=out.z;
		out.y/=out.z;
		box.AddBound(Vect3f(out.x,out.y,out.z));
	}

	return box;
}

void cTileMap::CalcShadowMapCameraProective(cCamera *DrawNode)
{
	Vect3f LightDirection(0,0,-1);
	DrawNode->GetScene()->GetLighting(&LightDirection);
	ShadowDrawNode->SetAttribute(ATTRCAMERA_PERSPECTIVE|ATTRCAMERA_NOT_CALC_PROJ);

	Vect4f light_in,light_out;
	light_in.x=-LightDirection.x;
	light_in.y=-LightDirection.y;
	light_in.z=-LightDirection.z;
	light_in.w=0;

	Mat4f look_light,look_proj;
    DrawNode->matViewProj.xform(light_in, light_out);

	ShadowDrawNode->PutAttribute(ATTRCAMERA_ZINVERT,light_out.w<0);
//	if(minus)light_out.w=-light_out.w;
	light_out.x/=light_out.w;
	light_out.y/=light_out.w;
	light_out.z/=light_out.w;
	light_out.w/=light_out.w;

    Vect3f a(light_out.x,light_out.y,light_out.z);
    Vect3f b(0,0,0.5f);
    Vect3f c(0,0,-1);
	Mat4fLookAtLH(&look_light,
		&a, &b, &c
	);

	sBox6f box=::CalcZMinMax(&look_light);

	Vect3f fix=(box.max-box.min)*0.05f;
	{
		float zf=box.max.z+fix.z;
		float zn=box.min.z-fix.z;
		float w=2/(box.max.x-box.min.x),
			  h=2/(box.max.y-box.min.y);

		float zz=zf/(zf-zn);
		look_proj=Mat4f(
			w,	0,	0,	0,
			0,	h,	0,	0,
			0,	0, zz,	1,
			0,	0,-zn*zz,0
			);
	}

	Mat4f proj;
	proj = look_light * look_proj;
	//proj=*(Mat4f*)DrawNode->matViewProj;
	ShadowDrawNode->matProj = DrawNode->matProj * proj;
//	ShadowDrawNode.matProj=DrawNode->matProj;

	{
		Vect4f t(0,0,0.5f,1),out;

        proj.xform(t, out);
	}

	//MP*MVL*P*MV
 
	Vect2f Focus(1,1);
    Vect2f center(0.5f,0.5f);
    sRectangle4f clip(-0.5f,-0.5f,0.5f,0.5f);
    Vect2f zplane(0,1000);
	ShadowDrawNode->SetFrustum(&center,&clip, &Focus, &zplane);
	ShadowDrawNode->SetPosition(DrawNode->GetMatrix());

	Vect4f out;
    Vect3f v(1024,1024,0);
    ShadowDrawNode->matViewProj.xform(v, out);

	Vect3f p[8];
	DrawNode->GetFrustumPoint(p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
	int i;
	for(i=0;i<8;i++)
	{
		Vect4f pOut,pN;
        DrawNode->matViewProj.xform(p[i], pOut);

		pN.x=pOut.x/pOut.w;
		pN.y=pOut.y/pOut.w;
		pN.z=pOut.z/pOut.w;
		pN.w=pOut.w/pOut.w;
		
		int k=0;
	}


	Vect3f in[8]=
	{
	Vect3f(-1,	1,	0),
	Vect3f(1,	1,	0),
	Vect3f(-1,	-1,	0),
	Vect3f(1,	-1,	0),

	Vect3f(-1,	1,	1),
	Vect3f(1,	1,	1),
	Vect3f(-1,	-1,	1),
	Vect3f(1,	-1,	1),
	};

	for(i=0;i<8;i++)
	{
		Vect4f pOut,pN;
        proj.xform(in[i], pOut);

		pN.x=pOut.x/pOut.w;
		pN.y=pOut.y/pOut.w;
		pN.z=pOut.z/pOut.w;
		pN.w=pOut.w/pOut.w;
		int k=0;
	}
}

void cTileMap::AddPlanarCamera(cCamera *DrawNode,bool light)
{
	cCamera *PlanarNode=light?LightDrawNode:ShadowDrawNode;
	float SizeLightMap=terra->SizeX();
	Vect3f PosLightMap;
	PosLightMap.x = terra->SizeX()/2;
	PosLightMap.y = terra->SizeX()/2;

	PosLightMap.z = SizeLightMap;

	Vect3f vShadow(0,0,-1);//Всё равно с других положений криво освещает

	Vect3f PosLight = -Vect3f(PosLightMap.x-5000*vShadow.x,PosLightMap.y-5000*vShadow.y,-5000*vShadow.z);
	MatXf LightMatrix;
	LightMatrix.rot().xrow().cross(vShadow,Vect3f(0,1,0)); // источник света в направлении оси x
	LightMatrix.rot().yrow()=Vect3f(0,-1,0);
	LightMatrix.rot().zrow()=vShadow;
	LightMatrix.trans()=LightMatrix.rot().xform( PosLight );
	LightMatrix.trans().x-=6;
	LightMatrix.trans().y+=3;

	DrawNode->SetCopy(PlanarNode);
	DrawNode->AttachChild(PlanarNode);

	Vect2f Focus(1/SizeLightMap,1/SizeLightMap);
	PlanarNode->SetAttribute(ATTRCAMERA_SHADOW|ATTRUNKOBJ_NOLIGHT);
	PlanarNode->ClearAttribute(ATTRCAMERA_PERSPECTIVE);
	PlanarNode->ClearAttribute(ATTRCAMERA_SHOWCLIP);
	PlanarNode->SetRenderTarget(light?GetLightMap():GetShadowMap(),NULL);
    Vect2f center(0.5f,0.5f);
    sRectangle4f clip(-0.5f,-0.5f,0.5f,0.5f);
    Vect2f zplane(10,1e6f);
    PlanarNode->SetFrustum(&center,&clip, &Focus, &zplane);
	
	PlanarNode->SetPosition(LightMatrix);
	PlanarNode->Attach(SCENENODE_OBJECT,this); // рисовать источники света							   
}

void cTileMap::AddFixedLightCamera(cCamera *DrawNode)
{
	cCamera *PlanarNode=LightDrawNode;
	float SizeLightMap=terra->SizeX();
	Vect3f PosLightMap;
	PosLightMap.x = terra->SizeX()/2;
	PosLightMap.y = terra->SizeX()/2;

	PosLightMap.z = SizeLightMap;

	Vect3f vShadow(0,0,-1);//Всё равно с других положений криво освещает

	Vect3f PosLight = -Vect3f(PosLightMap.x-5000*vShadow.x,PosLightMap.y-5000*vShadow.y,-5000*vShadow.z);
	MatXf LightMatrix;
	LightMatrix.rot().xrow().cross(vShadow,Vect3f(0,1,0)); // источник света в направлении оси x
	LightMatrix.rot().yrow()=Vect3f(0,-1,0);
	LightMatrix.rot().zrow()=vShadow;
	LightMatrix.trans()=LightMatrix.rot().xform( PosLight );

	DrawNode->SetCopy(PlanarNode);
	DrawNode->AttachChild(PlanarNode);

	Vect2f Focus(1/SizeLightMap,1/SizeLightMap);
	PlanarNode->SetAttribute(ATTRCAMERA_SHADOW|ATTRUNKOBJ_NOLIGHT);
	PlanarNode->ClearAttribute(ATTRCAMERA_PERSPECTIVE);
	PlanarNode->ClearAttribute(ATTRCAMERA_SHOWCLIP);
	PlanarNode->SetAttribute(ATTRCAMERA_NOCLEARTARGET);
	PlanarNode->SetRenderTarget(GetShadowMap(),NULL);
    Vect2f center(0.5f,0.5f);
    sRectangle4f clip(-0.5f,-0.5f,0.5f,0.5f);
    Vect2f zplane(10,1e6f);
    PlanarNode->SetFrustum(&center,&clip, &Focus, &zplane);
	
	PlanarNode->SetPosition(LightMatrix);
	PlanarNode->Attach(SCENENODE_OBJECT,this); // рисовать источники света
}

void cTileMap::CalcZMinMax(int x_tile,int y_tile)
{
	uint8_t zmin=255,zmax=0;
/*
	int dx=GetTileSize().x,dy=GetTileSize().y;
	int xMap=dx*GetTileNumber().x;
	int yMap=dy*GetTileNumber().y;
	for(int y=0;y<dy;y++)
	{
		int ofs=x_start*dx+(y+y_start*dy)*xMap;
		for(int x=0;x<dx;x++,ofs++)
		{
			BYTE z=vMap_GetZ(ofs);
			if(z<zmin)
				zmin=z;
			if(z>zmax)
				zmax=z;
		}
	}
/*/
	{
		int shift=terra->GetReductionShift();

		int dx=GetTileSize().x>>shift,dy=GetTileSize().y>>shift;
		int xMap=dx*GetTileNumber().x;
		int yMap=dy*GetTileNumber().y;

		int x_start=x_tile*dx,y_start=y_tile*dy;
		int x_end=x_start+dx,y_end=y_start+dy;
		for(int y=y_start;y<y_end;y++)
		{
			for(int x=x_start;x<x_end;x++)
			{
				uint8_t z=terra->GetReductionZ(x, y);
				if(z<zmin)
					zmin=z;
				if(z>zmax)
					zmax=z;
			}
		}
	}
	
/**/
	sTile& s=GetTile(x_tile,y_tile);
	s.zmin=zmin;
	s.zmax=zmax;
}


Vect2f cTileMap::CalcZ(cCamera *DrawNode)
{
	float tx=GetTileSize().x * GetScale().x;
	float ty=GetTileSize().y * GetScale().y;

	Vect2f z(1e20f,1e-20f);

	for(int x=0;x<TileNumber.x;x++)
	for(int y=0;y<TileNumber.y;y++)
	{
		sTile& s=GetTile(x,y);
		Vect3f c0,c1;
		c0.x=x*tx;
		c0.y=y*ty;
		c0.z=s.zmin;
		c1.x=c0.x+tx;
		c1.y=c0.y+ty;
		c1.z=s.zmax;

		if(DrawNode->TestVisible(c0,c1))
		{
			Vect3f p[8]=
			{
				Vect3f(c0.x,c0.y,c0.z),
				Vect3f(c1.x,c0.y,c0.z),
				Vect3f(c0.x,c1.y,c0.z),
				Vect3f(c1.x,c1.y,c0.z),
				Vect3f(c0.x,c0.y,c1.z),
				Vect3f(c1.x,c0.y,c1.z),
				Vect3f(c0.x,c1.y,c1.z),
				Vect3f(c1.x,c1.y,c1.z),
			};

			for(int i=0;i<8;i++)
			{
				Vect3f o=DrawNode->GetMatrix()*p[i];
				if(o.z<z.x)
					z.x=o.z;
				if(o.z>z.y)
					z.y=o.z;
			}
		}
	}

	if(z.y<z.x)
	{
		z=DrawNode->GetZPlane();
	}else
	{
		float addz=(z.y-z.x)*1e-2;
		addz=max(addz,10.0f);
		z.x-=addz;
		z.y+=addz;
		if(z.x<DrawNode->GetZPlane().x)
			z.x=DrawNode->GetZPlane().x;
		if(z.y>DrawNode->GetZPlane().y)
			z.y=DrawNode->GetZPlane().y;
	}

	return z;
}


static int cur_zeroplast_number=0;
static
void cTileMapBorderCall(void* data,Vect2f& p)
{
	cTileMap* tm=(cTileMap*)data;

	int x= (int) xm::round(p.x) >> TILEMAP_SHL;
	int y= (int) xm::round(p.y) >> TILEMAP_SHL;
	//xassert(x>=0 && x<tm->TileNumber.x);
	//xassert(y>=0 && y<tm->TileNumber.y);
	sTile& tile=tm->GetTile(x,y);
	if(tile.GetAttribute(ATTRTILE_UPDATE_POINT))
		tile.region_point[cur_zeroplast_number].push_back(p);
}

void cTileMap::BuildRegionPoint()
{
	terra->LockColumn();

	{
		MTEnter enter(lock_update_rect);

		std::vector<UpdateRect>::iterator it;
		FOR_EACH(update_rect,it)
		{
			Vect2i& pos1=it->p1;
			Vect2i& pos2=it->p2;

			VISASSERT(pos1.y<=pos2.y&&pos1.x<=pos2.x);
			int dx=GetTileSize().x,dy=GetTileSize().y;
			int j1=pos1.y/dy,j2=min(pos2.y/dy,GetTileNumber().y-1);
			int i1=pos1.x/dx,i2=min(pos2.x/dx,GetTileNumber().x-1);
			for(int j=j1;j<=j2;j++)
			for(int i=i1;i<=i2;i++)
			{
				sTile& p=GetTile(i,j);
				p.SetAttribute(ATTRTILE_UPDATELOD|ATTRTILE_UPDATE_POINT);
				p.region_point.clear();
				p.region_point.resize(zeroplastnumber);
				CalcZMinMax(i,j);
			}
		}
		update_rect.clear();
	}


	columns.resize(zeroplastnumber);
	int i;
	for(i=0;i<zeroplastnumber;i++)
	{
		columns[i] = terra->GetColumn(i);
	}

	for(i=0;i<zeroplastnumber;i++)
	{
		cur_zeroplast_number=i;
		Vect2sVect border;
		terra->GetBorder(i,cTileMapBorderCall,this);
/*
		Vect2sVect::iterator it;
		FOR_EACH(border,it)
		{
			int x=it->x>>TILEMAP_SHL;
			int y=it->y>>TILEMAP_SHL;
			xassert(x>=0 && x<TileNumber.x);
			xassert(y>=0 && y<TileNumber.y);
			sTile& tile=GetTile(x,y);
			if(tile.GetAttribute(ATTRTILE_UPDATE_POINT))
				tile.region_point[i].push_back(*it);
		}
*/
	}

	for(int y=0;y<TileNumber.y;y++)
	for(int x=0;x<TileNumber.x;x++)
	{
		sTile& tile=GetTile(x,y);
		tile.ClearAttribute(ATTRTILE_UPDATE_POINT);
	}

	terra->UnlockColumn();
}

Vect3f cTileMap::To3D(const Vect2f& pos)
{
	Vect3f p;
	p.x=pos.x;
	p.y=pos.y;

	int x = xm::round(pos.x), y = xm::round(pos.y);
	if(x >= 0 && x < terra->SizeX() && y >= 0 && y < terra->SizeY())
	{
		p.z=terra->GetZ(x,y);
	}else
		p.z=0;
	return p;
}

void cTileMap::Animate(float dt)
{
	if(enable_debug_rect)
	{
		for(std::list<DebugRect>::iterator it=debug_rect.begin();it!=debug_rect.end();)
		{
			DebugRect& r=*it;
			r.time-=dt;
			if(r.time<0)
				it=debug_rect.erase(it);
			else
				it++;
		}
	}
}

void cTileMap::DrawLines()
{
	if(enable_debug_rect)
	{
		std::list<DebugRect>::iterator it;
		FOR_EACH(debug_rect,it)
		{
			DebugRect& r=*it;
			sColor4c c(255, 255, 255, xm::round(255 * r.time / debug_fade_interval));
			Vect3f p0,p1,p2,p3;
			p0=To3D(Vect2f(r.p1.x,r.p1.y));
			p1=To3D(Vect2f(r.p2.x,r.p1.y));
			p2=To3D(Vect2f(r.p2.x,r.p2.y));
			p3=To3D(Vect2f(r.p1.x,r.p2.y));

			gb_RenderDevice->DrawLine(p0,p1,c);
			gb_RenderDevice->DrawLine(p1,p2,c);
			gb_RenderDevice->DrawLine(p2,p3,c);
			gb_RenderDevice->DrawLine(p3,p0,c);
		}
	}
}

void cTileMap::RegisterUpdateMap(UpdateMapFunction f,void* data)
{
	UpdateMapData d;
	d.f=f;
	d.data=data;
	std::vector<UpdateMapData>::iterator it=find(func_update_map.begin(),func_update_map.end(),d);
	bool is=it!=func_update_map.end();
	xassert(!is);
	if(is)
		return;
	func_update_map.push_back(d);
}

void cTileMap::UnRegisterUpdateMap(UpdateMapFunction f,void* data)
{
	UpdateMapData d;
	d.f=f;
	d.data=data;
	std::vector<UpdateMapData>::iterator it=find(func_update_map.begin(),func_update_map.end(),d);
	bool is=it!=func_update_map.end();
	xassert(is);
	if(!is)
		return;
	func_update_map.erase(it);
}
