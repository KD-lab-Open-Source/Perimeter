#include "StdAfxRD.h"
#include "scene.h"
#include "TileMap.h"
#include "ObjNode.h"
#include "SpriteNode.h"
#include "Particle.h"
#include "Trail.h"
#include "Line3d.h"
#include "ObjLibrary.h"
#include "cZPlane.h"
#include "CChaos.h"

#include "..\client\Silicon.h"
#include <typeinfo.h>
#include "..\3dx\Lib3dx.h"
//#include <map>
//using namespace std;

FILE *gb_fSceneLog=NULL;

cScene::cScene() : cUnknownClass(KIND_SCENE)
{
	MTINIT(lock_draw);
	ObjLibrary=0;
	Size.set(0,0);
	CurrentTime=PreviousTime=0;

	TileMap=NULL;
	TileNumber.set(0,0);
	tile_size=0;

	StrencilShadowDrawNode=CreateCamera();
	ReflectionDrawNode=CreateCamera();
	disable_tilemap_visible_test=false;
}
cScene::~cScene()
{
	DeleteAutoObject();
	ObjLibrary=0;
	Size.set(0,0);
	CurrentTime=PreviousTime=0;
	VISASSERT(grid.size()==0);
	grid.Release();
/*	for(int i=GetNumberLight()-1;i>=0;i--)
	{
		cUnkLight *ULight=GetLight(i);
		DetachLight(ULight);
		ULight->Release();
	}
*/
	UpdateLists(INT_MAX);
	VISASSERT(grid.size()==0);
	VISASSERT(GetNumberLight()==0);

//	GetTexLibrary()->Compact();
	RELEASE(StrencilShadowDrawNode);
	RELEASE(ReflectionDrawNode);

	MTDONE(lock_draw);
}

void SaveKindObjNotFree()
{
#ifdef C_CHECK_DELETE
	if(!cCheckDelete::GetDebugRoot())
	{
		DeleteFile("obj_notfree.txt");
		return;
	}

	FILE* f=fopen("obj_notfree.txt","wt");
	typedef map<string,int> idmap;
	idmap ids;
	cCheckDelete* p;
	for(p=cCheckDelete::GetDebugRoot();p;p=p->GetDebugNext())
	{
		const type_info& ti=typeid(*p);
		ids[ti.name()]++;
	}

	fprintf(f,"-------------------------All object stat\n");
	idmap::iterator it;
	FOR_EACH(ids,it)
	{
		idmap::value_type& v=*it;
		fprintf(f,"%3i - %s\n",v.second,v.first.c_str());
	}

	fprintf(f,"-------------------------cObjectNodeRoot stat\n");
	for(p=cCheckDelete::GetDebugRoot();p;p=p->GetDebugNext())
	{
		cObjectNodeRoot* node=dynamic_cast<cObjectNodeRoot*>(p);
		if(node)
		{
			fprintf(f,"%s\n",node->GetFileName());
		}
	}

	fprintf(f,"-------------------------cTexture stat\n");
	for(p=cCheckDelete::GetDebugRoot();p;p=p->GetDebugNext())
	{
		cTexture* node=dynamic_cast<cTexture*>(p);
		if(node)
		{
			if(node->GetName())
				fprintf(f,"%s\n",node->GetName());
			else
				fprintf(f,"unknown name\n");
		}
	}

	fclose(f);
	VISASSERT(!cVisGeneric::IsAssertEnabled() && "Not all graph object is delete. See obj_notfree.txt");
#endif
}

void cScene::Compact()
{ 
	GetObjLibrary()->Compact();
	GetTexLibrary()->Compact();
}

void cScene::Animate()
{
	float dTime=(float)(CurrentTime-PreviousTime);
	if(dTime==0) return;
	MTEnter enter(lock_draw);

	// анимация объектов
	grid.SetDetachNow(false);
	for(sGrid2d::iterator it=grid.begin();it!=grid.end();)
	{
		cIUnkClass* p=*it;
		if(p && p->GetAttr(ATTRUNKOBJ_IGNORE)==0) 
		{
			p->Animate(dTime);
		}

		grid.DetachAndIterate(it);
	}
	grid.SetDetachNow(true);
	if(TileMap)
		TileMap->Animate(dTime);

	// анимация источников света
	for(int i=0;i<GetNumberLight();i++)
		if(GetLight(i)&&GetLight(i)->GetAttr(ATTRUNKOBJ_IGNORE)==0) 
			GetLight(i)->Animate(dTime);
	BuildTree();
	CaclulateLightAmbient();

	PreviousTime=CurrentTime;
}

void cScene::SetTime(double Time)
{ 
	PreviousTime=CurrentTime; 
	CurrentTime=Time; 
}

void cScene::dSetTime(float dTime)
{ 
	PreviousTime=CurrentTime; 
	CurrentTime+=dTime; 
}

int cScene::GetTime()
{ 
	return CurrentTime; 
}

void cScene::UpdateLists(int cur_quant)
{
	grid.mtUpdate(cur_quant);
	UnkLightArray.mtUpdate(cur_quant);
}

void cScene::PreDraw(cCamera *DrawNode)
{
}

void cScene::Draw(cCamera *DrawNode)
{
	MTEnter enter(lock_draw);
	//PreDraw
	UpdateLists(gb_VisGeneric->GetGraphLogicQuant());
	VISASSERT(DrawNode->GetScene()==this);
	Animate();
	int i;

	{
		DrawNode->PreDrawScene();
		if(Option_ShadowType==SHADOW_STRENCIL && 
			(gb_RenderDevice->GetRenderMode()&RENDERDEVICE_MODE_STRENCIL))
			AddStrencilCamera(DrawNode);
	}

	if(TileMap)
	{
		AddReflectionCamera(DrawNode);
		TileMap->PreDraw(DrawNode);
		TileMap->DrawLightmapShadow(DrawNode);
		if(!disable_tilemap_visible_test)
			DrawNode->EnableGridTest(TileNumber.x,TileNumber.y,tile_size);
	}

	{
		for(i=0;i<grid.size();i++)
		{
			cIUnkClass* obj=grid[i];
			if(obj&&obj->GetAttr(ATTRUNKOBJ_IGNORE)==0) 
			{
				obj->PreDraw(DrawNode);
			}
		}
	}

	{
		for(i=0;i<GetNumberLight();i++)
			if(GetLight(i)&&GetLight(i)->GetAttr(ATTRLIGHT_DIRECTION)==0) 
				GetLight(i)->PreDraw(DrawNode);
	}

	if(TileMap)
		TileMap->FixShadowMapCamera(DrawNode);

	//Draw
	VISASSERT(DrawNode->GetScene()==this);

	DrawNode->DrawScene();

	for(i=0;i<grid.size();i++)
	{
		cIUnkClass* obj=grid[i];
		if(obj && obj->GetKind(KIND_OBJ_NODE_ROOT)) 
		{
			((cObjectNodeRoot*)obj)->OcclusionTest();
		}
	}

	gb_RenderDevice->SetClipRect(0,0,gb_RenderDevice->GetSizeX(),gb_RenderDevice->GetSizeY());
}

void cScene::PostDraw(cCamera *Camera)
{
}

void cScene::SetObjLibrary(cObjLibrary *pObjLibrary)
{
	if(pObjLibrary==0) { ObjLibrary=0; return; }
	ObjLibrary=pObjLibrary;
	VISASSERT(GetObjLibrary()->GetKind(KIND_LIB_OBJECT));
}
void cScene::GetLighting(const Vect3f &pos,sColor4f &diffuse,sColor4f &specular)
{
	for(int i=0;i<GetNumberLight();i++)
	{
		cUnkLight *Light=GetLight(i);
		float intensity=1;
		if(Light->GetAttr(ATTRLIGHT_SPHERICAL))
		{
			float d=pos.distance2(Light->GetPos());
			float r2=Light->GetRadius()*Light->GetRadius();
			if(r2<d)continue;
			intensity=1-d/r2;
		}
		diffuse.r+=Light->GetDiffuse().r*intensity;
		diffuse.g+=Light->GetDiffuse().g*intensity;
		diffuse.b+=Light->GetDiffuse().b*intensity;
		specular.r+=Light->GetDiffuse().r*intensity;
		specular.g+=Light->GetDiffuse().g*intensity;
		specular.b+=Light->GetDiffuse().b*intensity;
	}
}
void cScene::GetLighting(Vect3f *LightDirection)
{
	LightDirection->set(0,0,-1);
	for(int i=0;i<GetNumberLight();i++)
	{
		cUnkLight *Light=GetLight(i);
		if(Light->GetAttr(ATTRLIGHT_DIRECTION))
		{
			*LightDirection=Light->GetDirection();
			break;
		}
	}
	LightDirection->Normalize();
}

bool cScene::GetLighting(sColor4f &Ambient,sColor4f &Diffuse,sColor4f &Specular,Vect3f &LightDirection)
{
	bool found=false;
	for(int i=0;i<GetNumberLight();i++)
	{
		cUnkLight *Light=GetLight(i);
		if(Light->GetAttr(ATTRLIGHT_DIRECTION))
		{
			LightDirection+=Light->GetDirection();
			Ambient+=Light->GetDiffuse();
			Diffuse+=Light->GetDiffuse();
			Specular+=Light->GetDiffuse();
			found=true;
		}
	}

	return found;
}

#define PREC_TRACE							10
#define PREC_TRACE_RAY						18
bool cScene::Trace(const Vect3f& pStart,const Vect3f& pFinish,Vect3f *pTrace)
{ // тест на пересечение луча с объектами сцены, в том числе и с ландшафтом

	TerraInterface* terra=TileMap->GetTerra();

	float xb=pStart.x,yb=pStart.y,zb=pStart.z;
	float xe=pFinish.x,ye=pFinish.y,ze=pFinish.z;
	float dx=xe-xb,dy=ye-yb,dz=ze-zb,dxAbs=ABS(dx),dyAbs=ABS(dy);
	int dx_,dy_,dz_;
	if(dxAbs>dyAbs)
		dy_=round(dy*(1<<PREC_TRACE_RAY)/dxAbs),dz_=round(dz*(1<<PREC_TRACE_RAY)/dxAbs),dx_=((dx>=0)?1:-1)<<PREC_TRACE_RAY;
	else if(dyAbs>FLT_EPS)
		dx_=round(dx*(1<<PREC_TRACE_RAY)/dyAbs),dz_=round(dz*(1<<PREC_TRACE_RAY)/dyAbs),dy_=((dy>=0)?1:-1)<<PREC_TRACE_RAY;
	else
	{
		VISASSERT(0);
		if(pTrace) pTrace->set(xb,yb,terra->GetZ(round(xb),round(yb)));
		return true;
	}

	int x_size=terra->SizeX();
	int y_size=terra->SizeY();

	if(xb<0)
		if(dxAbs) xb=(0-xb)/dxAbs,yb+=xb*dy,zb+=xb*dz,xb=0;
		else return false;
	if(yb<0)
		if(dyAbs) yb=(0-yb)/dyAbs,xb+=yb*dx,zb+=yb*dz,yb=0;
		else return false;
	if(xb>=x_size)
		if(dxAbs) xb=(xb-(x_size-1))/dxAbs,yb+=xb*dy,zb+=xb*dz,xb=x_size-1;
		else return false;
	if(yb>=y_size)
		if(dyAbs) yb=(yb-(y_size-1))/dyAbs,xb+=yb*dx,zb+=yb*dz,yb=y_size-1;
		else return false;

	int xb_=round(xb*(1<<PREC_TRACE_RAY)),yb_=round(yb*(1<<PREC_TRACE_RAY)),zb_=round(zb*(1<<PREC_TRACE_RAY));
	for(;(xb_>>PREC_TRACE_RAY)>=0 && (xb_>>PREC_TRACE_RAY)<x_size && 
		 (yb_>>PREC_TRACE_RAY)>=0 && (yb_>>PREC_TRACE_RAY)<y_size;
				xb_+=dx_,yb_+=dy_,zb_+=dz_)
		if(terra->GetZ((xb_>>PREC_TRACE_RAY),(yb_>>PREC_TRACE_RAY))>(zb_>>PREC_TRACE_RAY))
		{
			if(pTrace) pTrace->set(xb_>>PREC_TRACE_RAY,yb_>>PREC_TRACE_RAY,terra->GetZ((xb_>>PREC_TRACE_RAY),(yb_>>PREC_TRACE_RAY)));
			return true;
		}

	return false;
}

void cScene::AttachObj(cIUnkClass *obj)
{
	xassert(obj->GetKind()!=KIND_LIGHT);
	grid.Attach(obj);
	obj->SetScene(this);
}
void cScene::DetachObj(cIUnkClass *UObj)
{
	if(UObj==TileMap)
		TileMap=NULL;
	else
	if(UObj->GetKind()==KIND_LIGHT)
	{
		DetachLight((cUnkLight*)UObj);
	}else
	{
		grid.Detach((cIUnkObj*)UObj);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
cUnkLight* cScene::CreateLight(int Attribute,cTexture *pTexture)
{
	cUnkLight *Light=new cUnkLight();
	Light->SetAttr(Attribute);
	if(pTexture) 
		Light->SetTexture(0,pTexture);
	AttachLight(Light);
	Light->SetScene(this);
	return Light;
}

cUnkLight* cScene::CreateLight(int Attribute, const char* TextureName)
{ // реализация cUnkLight
	cUnkLight *Light=new cUnkLight();
	Light->SetAttr(Attribute);
	if(TextureName) 
		Light->SetTexture(0,GetTexLibrary()->GetElement(TextureName));
	AttachLight(Light);
	Light->SetScene(this);
	return Light;
}
//////////////////////////////////////////////////////////////////////////////////////////
cTileMap* cScene::CreateMap(TerraInterface* terra,int zeroplastnumber)
{
	VISASSERT(TileMap==NULL);
	MTG();

	cScene::Size=Vect2i(terra->SizeX(),terra->SizeY());
	TileMap=new cTileMap(this,terra);
	TileMap->SetBuffer(cScene::Size,zeroplastnumber);

	TileNumber=TileMap->GetTileNumber();
	tile_size=TileMap->GetTileSize().x;
	return TileMap;
}
//////////////////////////////////////////////////////////////////////////////////////////
cObjectNodeRoot* cScene::CreateObject(const char *fname,const char *TexturePath)
{
	cObjectNodeRoot *UObj=GetObjLibrary()->GetElement(fname,TexturePath);
	if(UObj==0) return 0;
	AttachObj(UObj);
	return UObj;
}

cObject3dx* cScene::CreateObject3dx(const char* fname,const char *TexturePath)
{
	cStatic3dx *pStatic=pLibrary3dx->GetElement(fname,TexturePath,false);
	if(pStatic==0) return 0;

	cObject3dx *pObject=new cObject3dx(pStatic);
	AttachObj(pObject);
	return pObject;
}

cObject3dx* cScene::CreateLogic3dx(const char* fname)
{
	cStatic3dx *pStatic=pLibrary3dx->GetElement(fname,NULL,true);
	if(pStatic==0) return 0;
	cObject3dx *pObject=new cObject3dx(pStatic);
	return pObject;
}

//////////////////////////////////////////////////////////////////////////////////////////
cIUnkClass* cScene::CreateSprite(const char *TexFName)
{
	cSpriteNode *USprite=new cSpriteNode;
	USprite->SetTexture(0,GetTexLibrary()->GetElement(TexFName));
	AttachObj(USprite);
	return USprite;
}

cSpriteManager* cScene::CreateSpriteManager(const char* TexFName)
{
	cSpriteManager *USprite=new cSpriteManager;
	USprite->SetTexture(0,GetTexLibrary()->GetElement(TexFName));
	AttachObj(USprite);
	return USprite;
}

//////////////////////////////////////////////////////////////////////////////////////////

cIUnkClass* cScene::CreateZPlaneObj(const char* Tex0,const char* Tex1,float k0,float k1,int op,
									float v0x,float v0y,float v1x,float v1y)
{
	cZPlane *PlaneObj=new cZPlane;

	cTexture* pTex0=GetTexLibrary()->GetElement(Tex0);
	cTexture* pTex1=GetTexLibrary()->GetElement(Tex1);
	PlaneObj->SetTexture(pTex0,pTex1,k0,k1,op);
	PlaneObj->SetSpeed(0,v0x,v0y);
	PlaneObj->SetSpeed(1,v1x,v1y);

	AttachObj(PlaneObj);
	return PlaneObj; 
}

cChaos* cScene::CreateChaos(Vect2f size,LPCSTR str_tex0,LPCSTR str_tex1,LPCSTR str_bump,int tile,bool enable_bump)
{
	cChaos* p=new cChaos(size,str_tex0,str_tex1,str_bump,tile,enable_bump);

	AttachObj(p);
	return p;
}

cIUnkClass* cScene::CreateBox(Vect3f size,LPCSTR str_cube)
{
	CBox* p=new CBox(size,str_cube);
	AttachObj(p);
	return p;
}

cIUnkClass* cScene::CreateSkySpere(LPCSTR str_name,LPCSTR str_texture,int h_size)
{
	CSkySpere* p=new CSkySpere(GetObjLibrary(),str_name,str_texture,h_size);
	AttachObj(p);
	return p;
}

cPlane* cScene::CreatePlaneObj()
{
	cPlane *PlaneObj=new cPlane;
	AttachObj(PlaneObj);
	return PlaneObj;
}

//////////////////////////////////////////////////////////////////////////////////////////
cTrail* cScene::CreateTrail(const char* TextureName,float TimeLife)
{ 
	cTrail *UObj=new cTrail(TimeLife);
	UObj->SetTexture(0,GetTexLibrary()->GetElement(TextureName));
	AttachObj(UObj);
	return UObj;
}
cParticle* cScene::CreateParticle(const char* TextureName,float TimeLife,Vect2f *vTexSize)
{ 
	cParticle *UObj=new cParticle(TimeLife,*vTexSize);
	UObj->SetTexture(0,GetTexLibrary()->GetElement(TextureName));
	AttachObj(UObj);
	return UObj;
}

cEffect* cScene::CreateEffect(EffectKey& el,cEmitter3dObject* models,float scale,bool auto_delete_after_life)
{
	cEffect* UObj=new cEffect();
	UObj->SetAutoDeleteAfterLife(auto_delete_after_life);
	UObj->SetScene(this);
	UObj->Init(el,models,scale);
	AttachObj(UObj);
	return UObj;
}

cEffect* cScene::CreateScaledEffect(EffectKey& el,cObjectNodeRoot* models,bool auto_delete_after_life)
{
	VISASSERT(models);
	float scale=models->GetScale().x;
	return CreateEffect(el,models,scale,auto_delete_after_life);
}

cEffect* cScene::CreateScaledEffect(EffectKey& el,cObject3dx* models,bool auto_delete_after_life)
{
	VISASSERT(models);
	float scale=models->GetScale();
	return CreateEffect(el,models,scale,auto_delete_after_life);
}

//////////////////////////////////////////////////////////////////////////////////////////
cLine3d* cScene::CreateLine3d(const char* TexFName,const char* TexFNameZ)
{ 
	cLine3d *UObj=new cLine3d;
	UObj->SetTexture(0,GetTexLibrary()->GetElement(TexFName,"MipMapNoBlur"));
	if(TexFNameZ)
		UObj->SetTexture(1,GetTexLibrary()->GetElement(TexFNameZ,"MipMapNoBlur"));
	AttachObj(UObj);
	return UObj;
}
//////////////////////////////////////////////////////////////////////////////////////////

cIUnkClass* cScene::CreateElasticSphere(const char* TextureName1, const char* TextureName2)
{
	ElasticSphere *UObj=new ElasticSphere;
	UObj->SetTexture(0,GetTexLibrary()->GetElement(TextureName1));
	UObj->SetTexture(1,GetTexLibrary()->GetElement(TextureName2));
	AttachObj(UObj);
	return UObj;
}

void cScene::AttachLight(cUnkLight* ULight)
{
	UnkLightArray.Attach(ULight);
}

void cScene::DetachLight(cUnkLight* ULight)
{
	UnkLightArray.Detach(ULight);
}

void cScene::AddStrencilCamera(cCamera *DrawNode)
{
	DrawNode->SetCopy(StrencilShadowDrawNode);
	StrencilShadowDrawNode->SetAttribute(ATTRCAMERA_SHADOW_STRENCIL);
	StrencilShadowDrawNode->SetClip(DrawNode->GetClip());
	DrawNode->AttachChild(StrencilShadowDrawNode);
}

cCamera* cScene::CreateCamera()
{
	return new cCamera(this);
}

void cScene::AddReflectionCamera(cCamera *DrawNode)
{
	if(!TileMap)return;

	if(TileMap->GetAttribute(ATTRUNKOBJ_REFLECTION))
	{ // reflection
		int h_zeroplast=TileMap->GetTerra()->GetHZeroPlast();
		sPlane4f ReflectionPlane(0,0,-1,h_zeroplast+1);

		DrawNode->SetCopy(ReflectionDrawNode);
		DrawNode->AttachChild(ReflectionDrawNode);
		ReflectionDrawNode->SetAttribute(ATTRCAMERA_REFLECTION);
		ReflectionPlane.GetReflectionMatrix(DrawNode->GetMatrix(),ReflectionDrawNode->GetMatrix());
		ReflectionDrawNode->GetPos().z=2*h_zeroplast-ReflectionDrawNode->GetPos().z;
		//ReflectionDrawNode.SetZPlaneTemp(Vect2f(100,10000));

		ReflectionDrawNode->SetHReflection(h_zeroplast+1);
		ReflectionDrawNode->Update();
	}

}

void cScene::BuildTree()
{
	if(TileMap)
	{
		tree.clear();

		bool is_spherical=false;
		for(int i=0;i<GetNumberLight();i++)
		{
			cUnkLight* ULight=GetLight(i);
			if(ULight && ULight->GetAttr(ATTRLIGHT_SPHERICAL))
			{
				is_spherical=true;
			}
		}

		if(is_spherical)
		{
			vector<cIUnkClass*> nodes;
			sGrid2d::iterator it;
			FOR_EACH(grid,it)
			{
				if((*it)->GetKind()==KIND_OBJ_NODE_ROOT)
					nodes.push_back(*it);
			}

			tree.build(0,0,Size.x,Size.y,nodes);
		}
	}
}

struct SceneLightProcParam
{
	cUnkLight* light;
	float radius;
	float inv_radius;
	Vect3f pos;
	sColor4f color;
};

static void SceneLightProc(cIUnkClass* obj,void* param)
{
	SceneLightProcParam& p=*(SceneLightProcParam*)param;
	VISASSERT(obj->GetKind()==KIND_OBJ_NODE_ROOT);
	cObjectNodeRoot* node=(cObjectNodeRoot*)obj;
/*
	float r=(node->GetPosition().trans()-p.pos).norm();
	if(r<p.radius)
	{
		sColor4f c=p.color*((p.radius-r)*p.inv_radius);
		node->AddLightIntensity(c);
	}
/*/
	node->AddLight(p.light);
/**/
}

void cScene::CaclulateLightAmbient()
{
	if(!Option_EnablePointLight)
		return;
	for(int i=0;i<GetNumberLight();i++)
	{
		cUnkLight* ULight=GetLight(i);
		if(ULight && ULight->GetAttr(ATTRLIGHT_SPHERICAL))
		{
			SceneLightProcParam p;
			p.light=ULight;
			p.pos=ULight->GetPos();
			p.radius=ULight->GetRadius();
			if(p.radius<5)
				continue;
			p.inv_radius=1/p.radius;
			p.color=ULight->GetDiffuse();
			p.color*=2;

			tree.find(Vect2i(p.pos.x,p.pos.y),p.radius,SceneLightProc,&p);
		}
	}
}

void cScene::DisableTileMapVisibleTest()
{
	VISASSERT(!TileMap && "This function must called before create tilemap");
	disable_tilemap_visible_test=true;
}

void cScene::DeleteAutoObject()
{
	MTG();
	UpdateLists(INT_MAX);

	for(int i=0;i<grid.size();i++)
	{
		cIUnkClass* obj=grid[i];
		if(obj) 
		{
			cEffect* eff=dynamic_cast<cEffect*>(obj);
			if(eff)
			if(eff->IsAutoDeleteAfterLife())
			{
				if(eff->Release()<=0)
					i--;
			}
		}
	}
	UpdateLists(INT_MAX);
}
