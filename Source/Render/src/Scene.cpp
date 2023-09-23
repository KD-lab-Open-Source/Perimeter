#include "StdAfxRD.h"

#include <climits>
#include <typeinfo>

#include "SafeCast.h"
#include "Scene.h"
#include "tilemap/TileMap.h"
#include "ObjNode.h"
#include "SpriteNode.h"
#include "Line3d.h"
#include "ObjLibrary.h"
#include "cPlane.h"
#include "CChaos.h"
#include "../client/Silicon.h"

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
	ObjLibrary = nullptr;
	Size.set(0,0);
	CurrentTime=PreviousTime=0;
    UpdateLists(INT_MAX);
	grid.Release();
    UnkLightArray.Release();
#ifdef PERIMETER_DEBUG_ASSERT
    std::vector<cIUnkClass*> list; //Empty as no object should be present at this point
    CheckPendingObjects(list);
#endif
	VISASSERT(grid.size()==0);
	VISASSERT(UnkLightArray.size()==0);

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
		std::remove("obj_notfree.txt");
		return;
	}

	FILE* f=fopen("obj_notfree.txt","wt");
	typedef std::map<std::string,int> idmap;
	idmap ids;
	cCheckDelete* p;
	for(p=cCheckDelete::GetDebugRoot();p;p=p->GetDebugNext())
	{
		const std::type_info& ti=typeid(*p);
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

    //Iterate objects
    grid.DisableChanges(true);
	for (auto el : grid) {
#ifdef MTGVECTOR_USE_HANDLES
        cIUnkClass* p = safe_cast<cIUnkClass*>(el->Get());
#else
        cIUnkClass* p = el;
#endif
		if (p) {
            //Clear previously assigned lights, we do this always outside Animate() to avoid stale pointers
            //Sometimes an object may skip Animate() and get ATTRUNKOBJ_IGNORE removed, while having stale lights in array
            if (p->GetKind()==KIND_OBJ_NODE_ROOT) {
                cObjectNodeRoot* r = safe_cast<cObjectNodeRoot*>(p);
                r->GetLight().clear();
            }
            if (p->GetAttr(ATTRUNKOBJ_IGNORE) == 0) {
                p->Animate(dTime);
                //p might not be valid after this point!
            }
        }
	}
    grid.DisableChanges(false);
    
	if(TileMap) {
        TileMap->Animate(dTime);
    }

	// анимация источников света
    UnkLightArray.DisableChanges(true);
	for (int i=0;i<GetNumberLight();i++) {
        cUnkLight* light = GetLight(i);
        if (light && light->GetAttr(ATTRUNKOBJ_IGNORE) == 0) {
            light->Animate(dTime);
        }
    }
    UnkLightArray.DisableChanges(false);
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

    grid.DisableChanges(true);
    for (auto el : grid) {
#ifdef MTGVECTOR_USE_HANDLES
        cIUnkClass* obj = safe_cast<cIUnkClass*>(el->Get());
#else
        cIUnkClass* obj = el;
#endif
        if (obj&&obj->GetAttr(ATTRUNKOBJ_IGNORE)==0) {
            obj->PreDraw(DrawNode);
        }
    }
    grid.DisableChanges(false);

    UnkLightArray.DisableChanges(true);
    for (i=0;i<GetNumberLight();i++) {
        cUnkLight* light = GetLight(i);
        if (light && light->GetAttr(ATTRLIGHT_DIRECTION) == 0) {
            light->PreDraw(DrawNode);
        }
    }
    UnkLightArray.DisableChanges(false);

	if(TileMap) {
        TileMap->FixShadowMapCamera(DrawNode);
    }

	//Draw
	VISASSERT(DrawNode->GetScene()==this);

	DrawNode->DrawScene();

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
		if(Light && Light->GetAttr(ATTRLIGHT_SPHERICAL))
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
		if(Light && Light->GetAttr(ATTRLIGHT_DIRECTION))
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
		if(Light && Light->GetAttr(ATTRLIGHT_DIRECTION))
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
        dy_= xm::round(dy * (1 << PREC_TRACE_RAY) / dxAbs), dz_= xm::round(dz * (1 << PREC_TRACE_RAY) / dxAbs), dx_=((dx >= 0) ? 1 : -1) << PREC_TRACE_RAY;
	else if(dyAbs>FLT_EPS)
        dx_= xm::round(dx * (1 << PREC_TRACE_RAY) / dyAbs), dz_= xm::round(dz * (1 << PREC_TRACE_RAY) / dyAbs), dy_=((dy >= 0) ? 1 : -1) << PREC_TRACE_RAY;
	else
	{
		//VISASSERT(0);
		if(pTrace) pTrace->set(xb,yb,terra->GetZ(xm::round(xb), xm::round(yb)));
		return true;
	}

	int x_size=terra->SizeX();
	int y_size=terra->SizeY();

	if(xb<0) {
		if(dxAbs) xb=(0-xb)/dxAbs,yb+=xb*dy,zb+=xb*dz,xb=0;
		else return false;
    }
	if(yb<0) {
		if(dyAbs) yb=(0-yb)/dyAbs,xb+=yb*dx,zb+=yb*dz,yb=0;
		else return false;
    }
	if(xb>=x_size) {
		if(dxAbs) xb=(xb-(x_size-1))/dxAbs,yb+=xb*dy,zb+=xb*dz,xb=x_size-1;
		else return false;
    }
	if(yb>=y_size) {
		if(dyAbs) yb=(yb-(y_size-1))/dyAbs,xb+=yb*dx,zb+=yb*dz,yb=y_size-1;
		else return false;
    }

	int xb_= xm::round(xb * (1 << PREC_TRACE_RAY)),yb_= xm::round(yb * (1 << PREC_TRACE_RAY)),zb_= xm::round(
            zb * (1 << PREC_TRACE_RAY));
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
    obj->SetScene(this);
	grid.Attach(obj);
}
void cScene::DetachObj(cIUnkClass *UObj)
{
	if (UObj==TileMap) {
        TileMap = NULL;
    } else if (UObj->GetKind()==KIND_LIGHT) {
		DetachLight(safe_cast<cUnkLight*>(UObj));
	} else {
		grid.Detach(safe_cast<cIUnkClass*>(UObj));
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
cUnkLight* cScene::CreateLight(int Attribute,cTexture *pTexture)
{
	cUnkLight *Light=new cUnkLight();
	Light->SetAttr(Attribute);
	if(pTexture) {
        Light->SetTexture(0, pTexture);
    }
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

//////////////////////////////////////////////////////////////////////////////////////////

cSpriteManager* cScene::CreateSpriteManager(const char* TexFName)
{
	cSpriteManager *USprite=new cSpriteManager;
	USprite->SetTexture(0,GetTexLibrary()->GetElement(TexFName));
	AttachObj(USprite);
	return USprite;
}

//////////////////////////////////////////////////////////////////////////////////////////

cChaos* cScene::CreateChaos(Vect2f size, const char* str_tex0, const char* str_tex1, const char* str_bump, int tile, bool enable_bump)
{
	cChaos* p=new cChaos(size,str_tex0,str_tex1,str_bump,tile,enable_bump);

	AttachObj(p);
	return p;
}

cIUnkClass* cScene::CreateSkySpere(const char* str_name, const char* str_texture, int h_size)
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

cUnkLight* cScene::GetLight(int number) {
    xassert(0 <= number && number < UnkLightArray.size());
    auto light = UnkLightArray.get(number);
#ifdef MTGVECTOR_USE_HANDLES
    return handle ? safe_cast<cUnkLight*>(handle->Get()) : nullptr;
#else
    return safe_cast<cUnkLight*>(light);
#endif
}

void cScene::AttachLight(cUnkLight* ULight)
{
    xassert(ULight->GetKind()==KIND_LIGHT);
	UnkLightArray.Attach(ULight);
}

void cScene::DetachLight(cUnkLight* ULight)
{
    xassert(ULight->GetKind()==KIND_LIGHT);
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
	if(TileMap) {
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

		if(is_spherical) {
			std::vector<cIUnkClass*> nodes;
			for (auto el : grid) {
#ifdef MTGVECTOR_USE_HANDLES
                cUnknownClass* unk = el->Get();
#else
                cUnknownClass* unk = el;
#endif
				if (unk && unk->GetKind()==KIND_OBJ_NODE_ROOT) {
                    cIUnkClass* obj = safe_cast<cIUnkClass*>(unk);
                    nodes.push_back(obj);
                }
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
	cObjectNodeRoot* node = safe_cast<cObjectNodeRoot*>(obj);
/*
	float r=(node->GetPosition().trans()-p.pos).norm();
	if(r<p.radius)
	{
		sColor4f c=p.color*((p.radius-r)*p.inv_radius);
		node->AddLightIntensity(c);
	}
/*/

    float r = (node->GetPosition().trans()-p.pos).norm();
    if( r <= p.radius * 3.0f)
    {
        node->AddLight(p.light);
    }
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

			tree.find(Vect2i(p.pos.x,p.pos.y),p.radius * 3.5f,SceneLightProc,&p);
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

    std::vector<cEffect*> list;

    for (auto el : grid) {
#ifdef MTGVECTOR_USE_HANDLES
        cIUnkClass* obj = safe_cast<cIUnkClass*>(el->Get());
#else
        cIUnkClass* obj = el;
#endif
        if (obj) {
            cEffect* eff = dynamic_cast<cEffect*>(obj);
            if (eff && eff->IsAutoDeleteAfterLife()) {
                list.push_back(eff);
            }
        }
    }

    for (auto eff : list) {
        eff->Release();
    }

    UpdateLists(INT_MAX);
}


#ifdef PERIMETER_DEBUG_ASSERT
void cScene::AssertNoObject(cIUnkClass* object) {
    if (object->GetKind() == KIND_LIGHT) {
        UnkLightArray.AssertNoObject(object);
    } else {
        grid.AssertNoObject(object);
    }
}

void cScene::CheckPendingObjects(std::vector<cIUnkClass*>& allowed) {
    MTG();
    UpdateLists(INT_MAX);
    for (auto e : grid) {
        if (std::count(allowed.begin(), allowed.end(), e) == 0) {
            fprintf(stderr, "Pending Object: %p refs %ld\n", e, e->GetRef());
            xxassert(0, "Pending Object");
        }
    }
    for (auto e : UnkLightArray) {
        if (std::count(allowed.begin(), allowed.end(), e) == 0) {
            fprintf(stderr, "Pending Light: %p refs %ld\n", e, e->GetRef());
            xxassert(0, "Pending Light");
        }
    }
    xxassert(0 == grid.PendingChanges(), "Pending Object changes!");
    xxassert(0 == UnkLightArray.PendingChanges(), "Pending Light changes!");
}
#endif
