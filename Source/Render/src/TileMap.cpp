#include "StdAfxRD.h"
#include "TileMap.h"
#include "Scene.h"
#include "ObjLibrary.h"
#include "..\..\Game\Region.h"
#include "font.h"

cTileMap::cTileMap(cScene* pScene,TerraInterface* terra_) : cUnkObj(KIND_TILEMAP)
{
	MTINIT(lock_update_rect);
	terra=terra_;
	TileSize.set(TILEMAP_SIZE,TILEMAP_SIZE);
	TileNumber.set(0,0);
	Tile=0;

	tilesize.set(0,0,0);
	pTileMapRender=NULL;

	ShadowDrawNode=pScene->CreateCamera();
	LightDrawNode=new cCameraPlanarLight(pScene);
	LightMapType=0;
	zeroplastnumber=0;
	enable_debug_rect=false;
	debug_fade_interval=500;

	SetScene(pScene);
}
cTileMap::~cTileMap()
{
	RELEASE(terra);
	RELEASE(ShadowDrawNode);
	RELEASE(LightDrawNode);
	gb_RenderDevice3D->Delete(this);
	if(Tile) { delete [] Tile; Tile=0; }
	MTDONE(lock_update_rect);
}

int cTileMap::CheckLightMapType()
{
	return Option_ShadowType;
}

cTexture* cTileMap::GetShadowMap()
{
	if(CheckLightMapType())
		return gb_RenderDevice3D->dtAdvance->GetShadowMap();
	return gb_RenderDevice3D->dtFixed->GetShadowMap();
}

cTexture* cTileMap::GetLightMap()
{
	if(CheckLightMapType())
		return gb_RenderDevice3D->dtAdvance->GetLightMap();
	return gb_RenderDevice3D->dtFixed->GetLightMap();
}

LPDIRECT3DSURFACE9 cTileMap::GetZBuffer()
{
	if(CheckLightMapType())
		return gb_RenderDevice3D->dtAdvance->GetZBuffer();
	return gb_RenderDevice3D->dtFixed->GetZBuffer();
}

void cTileMap::CreateLightmap()
{
	gb_RenderDevice3D->dtFixed->DeleteShadowTexture();
	gb_RenderDevice3D->dtAdvance->DeleteShadowTexture();

	int width=256<<(Option_DrawMeshShadow-1);

	LightMapType=CheckLightMapType();

	if(Option_DrawMeshShadow>0)
	{
		DrawType* draw=gb_RenderDevice3D->dtFixed;
		if(LightMapType)
			draw=gb_RenderDevice3D->dtAdvance;

		if(!draw->CreateShadowTexture(width))
		{
			gb_VisGeneric->SetShadowType((eShadowType)(int)Option_ShadowType,0);
		}
	}

	float SizeLightMap=terra->SizeX();
	float focus=1/SizeLightMap;
	matLightMap.identity();
	matLightMap._11 *= focus;
	matLightMap._21 *= focus;
	matLightMap._31 *= focus;
	matLightMap._12 *= focus;
	matLightMap._22 *= focus;
	matLightMap._32 *= focus;

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
	gb_RenderDevice3D->PreDraw(this);
}

void cTileMap::Draw(cCamera *DrawNode)
{
	if(!Option_ShowType[SHOW_TILEMAP])
		return;

	cD3DRender *Render=gb_RenderDevice3D;
	if(DrawNode->GetAttribute(ATTRCAMERA_SHADOW))
	{
		Render->Draw(GetScene()); // рисовать источники света
	}
	else if(DrawNode->GetAttribute(ATTRCAMERA_SHADOWMAP))
	{
		if(Option_ShadowType==SHADOW_MAP_SELF)
			Render->Draw(this,ALPHA_TEST,TILEMAP_ALL,true);
	}
	else if(DrawNode->GetAttribute(ATTRCAMERA_REFLECTION))
	{ // рисовать отражение
		Render->SetRenderState(RS_ALPHAREF,254/*GetRefSurface()*/);
		Render->SetRenderState(RS_ALPHAFUNC,D3DCMP_GREATER);
		Render->Draw(this,ALPHA_TEST,TILEMAP_NOZEROPLAST,false);
		Render->SetRenderState(RS_ALPHAFUNC,D3DCMP_GREATER);
		Render->SetRenderState(RS_ALPHAREF,0);
	}else
	{
		if(GetAttribute(ATTRUNKOBJ_REFLECTION))
		{ // рисовать прямое изображение
			Render->SetRenderState(RS_ALPHAREF,1);
			Render->Draw(this,ALPHA_BLEND,TILEMAP_ZEROPLAST,false);
			Render->Draw(this,ALPHA_NONE,TILEMAP_NOZEROPLAST,false);
			Render->SetRenderState(RS_ALPHAREF,0);
		}else
		{
			Render->Draw(this,ALPHA_NONE,TILEMAP_ALL,false);
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
				vector<Vect2s>& point=tile.region_point[ir];
				npoint+=point.size();
				vector<Vect2s>::iterator it;
				FOR_EACH(point,it)
				{
					Render->DrawPoint(Vect3f(it->x,it->y,zeroh),sColor4c(0,255,255,100));
				}
			}
		}

		Render->FlushPrimitive3D();

		cFont* pFont=gb_VisGeneric->CreateDebugFont();
		Render->SetFont(pFont);
		char s[128];
		sprintf(s,"point=%i",npoint);
		Render->OutText(10,50,s,sColor4f(1,1,1,1));
		Render->SetFont(NULL);
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

	vector<UpdateMapData>::iterator it;
	FOR_EACH(func_update_map,it)
	{
		it->f(pos1,pos2,it->data);
	}
}

void cTileMap::SetBuffer(const Vect2i &size,int zeroplastnumber_)
{
	zeroplastnumber=zeroplastnumber_;
	zeroplast_color.resize(zeroplastnumber);
	for(int i=0;i<zeroplastnumber;i++)
		zeroplast_color[i]=sColor4f(0,1,0,1);

	if(Tile) { gb_RenderDevice3D->Delete(this); delete Tile; }
	TileNumber.set(size.x/GetTileSize().x,size.y/GetTileSize().y);
	VISASSERT(TileNumber.x*GetTileSize().x==size.x);
	VISASSERT(TileNumber.y*GetTileSize().y==size.y);

	Tile=new sTile[GetTileNumber().x*GetTileNumber().y];
	gb_RenderDevice3D->Create(this);

	UpdateMap(Vect2i(0,0), Vect2i(size.x-1,size.y-1));
}


void cTileMap::DrawLightmapShadow(cCamera *DrawNode)
{
	
	if(Option_DrawMeshShadow && GetShadowMap()==0 && gb_RenderDevice->nSupportTexture>1)
	{
		CreateLightmap();
	}else 
	if(GetShadowMap())
	if( (256<<(Option_DrawMeshShadow-1))!=GetShadowMap()->GetWidth() || 
		LightMapType!=CheckLightMapType())
	{
		CreateLightmap();
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
	DrawNode->SetAttribute(ATTRCAMERA_ZMINMAX);
	DrawNode->SetCopy(ShadowDrawNode);
	DrawNode->AttachChild(ShadowDrawNode);
	ShadowDrawNode->SetAttribute(ATTRCAMERA_SHADOWMAP|ATTRUNKOBJ_NOLIGHT);
	ShadowDrawNode->ClearAttribute(ATTRCAMERA_PERSPECTIVE|ATTRCAMERA_ZMINMAX|ATTRCAMERA_SHOWCLIP);
	ShadowDrawNode->SetRenderTarget(GetShadowMap(),GetZBuffer());


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
	calcVisMap(DrawNode,GetTileNumber(),GetTileSize(),LightMatrix.rot(),box);

	Vect2f prevz(box.min.z,box.max.z);
	box.min.z-=bound_z;
	box.max.z+=bound_z;

	Vect3f PosLight((box.min.x+box.max.x)*0.5f,(box.min.y+box.max.y)*0.5f,box.min.z);
	LightMatrix.trans()=-PosLight;

	Vect2f Focus(1/(box.max.x-box.min.x),1/(box.max.y-box.min.y));
	ShadowDrawNode->SetFrustum(&Vect2f(0.5f,0.5f),&sRectangle4f(-0.5f,-0.5f,0.5f,0.5f),
		&Focus, &Vect2f(0,box.max.z-box.min.z));
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
	ShadowDrawNode->SetFrustum(&Vect2f(0.5f,0.5f),&sRectangle4f(-0.5f,-0.5f,0.5f,0.5f),
		&Focus, &Vect2f(0,box.max.z-box.min.z));
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


sBox6f CalcZMinMax(const D3DXMATRIX* look)
{
	D3DXVECTOR3 in[8]=
	{
	D3DXVECTOR3(-1,	1,	0),
	D3DXVECTOR3(1,	1,	0),
	D3DXVECTOR3(-1,	-1,	0),
	D3DXVECTOR3(1,	-1,	0),

	D3DXVECTOR3(-1,	1,	1),
	D3DXVECTOR3(1,	1,	1),
	D3DXVECTOR3(-1,	-1,	1),
	D3DXVECTOR3(1,	-1,	1),
	};

	sBox6f box;
	box.SetInvalidBox();

	for(int i=0;i<8;i++)
	{
		D3DXVECTOR4 out;
		D3DXVec3Transform(&out,in+i,look);
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

	D3DXVECTOR4 light_in,light_out;
	light_in.x=-LightDirection.x;
	light_in.y=-LightDirection.y;
	light_in.z=-LightDirection.z;
	light_in.w=0;

	D3DXMATRIX look_light,look_proj;
	D3DXVec4Transform(&light_out,&light_in,DrawNode->matViewProj);

	{
		Vect2f zplane=DrawNode->GetZPlane();
		float dist=(zplane.x+zplane.y)/(zplane.y-zplane.x);

		//float d=D3DXVec3Length((D3DXVECTOR3*)&light_out);
		float d=D3DXVec4Length(&light_out);
		int k=0;
	}

	ShadowDrawNode->PutAttribute(ATTRCAMERA_ZINVERT,light_out.w<0);
//	if(minus)light_out.w=-light_out.w;
	light_out.x/=light_out.w;
	light_out.y/=light_out.w;
	light_out.z/=light_out.w;
	light_out.w/=light_out.w;

	D3DXMatrixLookAtLH(&look_light,
		&D3DXVECTOR3(light_out.x,light_out.y,light_out.z),
		&D3DXVECTOR3(0,0,0.5f),
		&D3DXVECTOR3(0,0,-1)
	);

	sBox6f box=::CalcZMinMax(&look_light);

	Vect3f fix=(box.max-box.min)*0.05f;
/*
	D3DXMatrixPerspectiveFovLH(&look_proj,M_PI/3,1, (box.min.z-fix.z),(box.max.z+fix.z));
/*/
	{
		float zf=box.max.z+fix.z;
		float zn=box.min.z-fix.z;
		float w=2/(box.max.x-box.min.x),
			  h=2/(box.max.y-box.min.y);

		float zz=zf/(zf-zn);
		look_proj=D3DXMATRIX(
			w,	0,	0,	0,
			0,	h,	0,	0,
			0,	0, zz,	1,
			0,	0,-zn*zz,0
			);
	}
/**/
	D3DXMATRIX proj;
	D3DXMatrixMultiply(&proj, &look_light, &look_proj);
	//proj=*(D3DXMATRIX*)DrawNode->matViewProj;
	D3DXMatrixMultiply(ShadowDrawNode->matProj, DrawNode->matProj, &proj);
//	ShadowDrawNode.matProj=DrawNode->matProj;

	{
		float f=D3DXMatrixDeterminant(&look_proj);
		D3DXVECTOR4 t(0,0,0.5f,1),out;

		D3DXVec4Transform(&out,&t,&proj);

		int k=0;

	}

	//MP*MVL*P*MV
 
	Vect2f Focus(1,1);
	ShadowDrawNode->SetFrustum(&Vect2f(0.5f,0.5f),&sRectangle4f(-0.5f,-0.5f,0.5f,0.5f),
		&Focus, &Vect2f(0,1000));
	ShadowDrawNode->SetPosition(DrawNode->GetMatrix());

	D3DXVECTOR4 out;
	D3DXVec3Transform(&out,&D3DXVECTOR3(1024,1024,0),ShadowDrawNode->matProj);

	Vect3f p[8];
	DrawNode->GetFrustumPoint(p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
	int i;
	for(i=0;i<8;i++)
	{
		D3DXVECTOR4 pOut,pN;
		D3DXVec3Transform(&pOut,(D3DXVECTOR3*)(p+i),DrawNode->matViewProj);

		pN.x=pOut.x/pOut.w;
		pN.y=pOut.y/pOut.w;
		pN.z=pOut.z/pOut.w;
		pN.w=pOut.w/pOut.w;
		
		int k=0;
	}


	D3DXVECTOR3 in[8]=
	{
	D3DXVECTOR3(-1,	1,	0),
	D3DXVECTOR3(1,	1,	0),
	D3DXVECTOR3(-1,	-1,	0),
	D3DXVECTOR3(1,	-1,	0),

	D3DXVECTOR3(-1,	1,	1),
	D3DXVECTOR3(1,	1,	1),
	D3DXVECTOR3(-1,	-1,	1),
	D3DXVECTOR3(1,	-1,	1),
	};

	for(i=0;i<8;i++)
	{
		D3DXVECTOR4 pOut,pN;
		D3DXVec3Transform(&pOut,in+i,&proj);

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

	Vect3f PosLight(PosLightMap.x-5000*vShadow.x,PosLightMap.y-5000*vShadow.y,-5000*vShadow.z);
	MatXf LightMatrix;
	LightMatrix.rot().xrow().cross(vShadow,Vect3f(0,1,0)); // источник света в направлении оси x
	LightMatrix.rot().yrow()=Vect3f(0,-1,0);
	LightMatrix.rot().zrow()=vShadow;
	LightMatrix.trans()=LightMatrix.rot().xform( -PosLight );
	LightMatrix.trans().x-=6;
	LightMatrix.trans().y+=3;

	DrawNode->SetCopy(PlanarNode);
	DrawNode->AttachChild(PlanarNode);

	Vect2f Focus(1/SizeLightMap,1/SizeLightMap);
	PlanarNode->SetAttribute(ATTRCAMERA_SHADOW|ATTRUNKOBJ_NOLIGHT);
	PlanarNode->ClearAttribute(ATTRCAMERA_PERSPECTIVE);
	PlanarNode->ClearAttribute(ATTRCAMERA_SHOWCLIP);
	PlanarNode->SetRenderTarget(light?GetLightMap():GetShadowMap(),NULL);
	PlanarNode->SetFrustum(&Vect2f(0.5f,0.5f),&sRectangle4f(-0.5f,-0.5f,0.5f,0.5f),
		&Focus, &Vect2f(10,1e6f));
	
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

	Vect3f PosLight(PosLightMap.x-5000*vShadow.x,PosLightMap.y-5000*vShadow.y,-5000*vShadow.z);
	MatXf LightMatrix;
	LightMatrix.rot().xrow().cross(vShadow,Vect3f(0,1,0)); // источник света в направлении оси x
	LightMatrix.rot().yrow()=Vect3f(0,-1,0);
	LightMatrix.rot().zrow()=vShadow;
	LightMatrix.trans()=LightMatrix.rot().xform( -PosLight );

	DrawNode->SetCopy(PlanarNode);
	DrawNode->AttachChild(PlanarNode);

	Vect2f Focus(1/SizeLightMap,1/SizeLightMap);
	PlanarNode->SetAttribute(ATTRCAMERA_SHADOW|ATTRUNKOBJ_NOLIGHT);
	PlanarNode->ClearAttribute(ATTRCAMERA_PERSPECTIVE);
	PlanarNode->ClearAttribute(ATTRCAMERA_SHOWCLIP);
	PlanarNode->SetAttribute(ATTRCAMERA_NOCLEARTARGET);
	PlanarNode->SetRenderTarget(GetShadowMap(),NULL);
	PlanarNode->SetFrustum(&Vect2f(0.5f,0.5f),&sRectangle4f(-0.5f,-0.5f,0.5f,0.5f),
		&Focus, &Vect2f(10,1e6f));
	
	PlanarNode->SetPosition(LightMatrix);
	PlanarNode->Attach(SCENENODE_OBJECT,this); // рисовать источники света
}

void cTileMap::CalcZMinMax(int x_tile,int y_tile)
{
	BYTE zmin=255,zmax=0;
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
				BYTE z=terra->GetReductionZ(x,y);
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

	int x=round(p.x)>>TILEMAP_SHL;
	int y=round(p.y)>>TILEMAP_SHL;
	xassert(x>=0 && x<tm->TileNumber.x);
	xassert(y>=0 && y<tm->TileNumber.y);
	sTile& tile=tm->GetTile(x,y);
	if(tile.GetAttribute(ATTRTILE_UPDATE_POINT))
		tile.region_point[cur_zeroplast_number].push_back(p);
}

void cTileMap::BuildRegionPoint()
{
	terra->LockColumn();

	{
		MTEnter enter(lock_update_rect);

		vector<UpdateRect>::iterator it;
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

	int x = round(pos.x), y = round(pos.y);
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
		for(list<DebugRect>::iterator it=debug_rect.begin();it!=debug_rect.end();)
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
		list<DebugRect>::iterator it;
		FOR_EACH(debug_rect,it)
		{
			DebugRect& r=*it;
			sColor4c c(255,255,255,round(255*r.time/debug_fade_interval));
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
	vector<UpdateMapData>::iterator it=find(func_update_map.begin(),func_update_map.end(),d);
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
	vector<UpdateMapData>::iterator it=find(func_update_map.begin(),func_update_map.end(),d);
	bool is=it!=func_update_map.end();
	xassert(is);
	if(!is)
		return;
	func_update_map.erase(it);
}
