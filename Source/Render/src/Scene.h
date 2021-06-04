#pragma once
#include "UnkLight.h"
#include "czplane.h"
#include "NParticle.h"
#include "SpriteNode.h"
#include "..\3dx\Node3dx.h"

class cObjectNodeRoot;
class terUnitBase;
#include "VisGrid2d.h"

class cScene : public cUnknownClass
{
public:
	cScene();
	~cScene();
	// отрисовка
	virtual void Compact();
	virtual void Draw(cCamera *UCamera);		// отрисовка указанной части мира
	virtual void PreDraw(cCamera *UCamera);		// подготовка к прорисовке, определение видимости, выставление анимации и т.д. 
	virtual void PostDraw(cCamera *UCamera);	// действия после прорисовки, освобождение временных буферов и т.д. 
	virtual void SetTime(double Time);
	virtual void dSetTime(float dTime);
	virtual int GetTime();
	// функции для работы с объектами
	virtual cObjectNodeRoot* CreateObject(const char* fname,const char *TexturePath=NULL);
	virtual cObject3dx* CreateObject3dx(const char* fname,const char *TexturePath=NULL);
	virtual cObject3dx* CreateLogic3dx(const char* fname);
	// функции для работы с объектами
	virtual cIUnkClass* CreateSprite(const char* TexFName);
	virtual cSpriteManager* CreateSpriteManager(const char* TexFName);
	// функции для работы с источниками света, влияющими на освещение объектов текущей сцены
	virtual cUnkLight* CreateLight(int Attribute=0, const char* TextureName = 0);
	virtual cUnkLight* CreateLight(int Attribute,cTexture *pTexture);
	// функции для работы с ломанными
	//TexFName - основная текстура
	//TexFNameZ - текстура которая видна после закрытия ландшафтом
	virtual class cLine3d* CreateLine3d(const char* TexFName,
							const char* TexFNameZ=NULL);
	// функции для работы со следами
	virtual class cTrail* CreateTrail(const char* TextureName,float TimeLife=1000.f);
	// функции для работы с системой частиц
	virtual class cParticle* CreateParticle(const char* TextureName,float TimeLife=1000.f,Vect2f *vTexSize=&Vect2f(1,1));
	virtual cEffect* CreateEffect(EffectKey& el,cEmitter3dObject* models,float scale=1.0f,bool auto_delete_after_life=false);

	//Берет размеры по модели
	virtual cEffect* CreateScaledEffect(EffectKey& el,cObjectNodeRoot* models,bool auto_delete_after_life=false);
	virtual cEffect* CreateScaledEffect(EffectKey& el,cObject3dx* models,bool auto_delete_after_life=false);

	// функции для работы с полигональным объектом
	virtual cPlane* CreatePlaneObj();

	virtual cIUnkClass* CreateZPlaneObj(const char* Tex0,const char* Tex1,float k0,float k1,int op,float v0x,float v0y,float v1x,float v1y);
	virtual class cChaos* CreateChaos(Vect2f size,LPCSTR str_tex0,LPCSTR str_tex1,LPCSTR str_bump,int tile,bool enable_bump);
	virtual cIUnkClass* CreateBox(Vect3f size,LPCSTR str_cube);
	virtual cIUnkClass* CreateSkySpere(LPCSTR str_name,LPCSTR str_texture,int h_size);

	// функции для работы с диспетчером регионов
	virtual class FieldDispatcher* CreateForceFieldDispatcher(int xmax,int ymax, int zeroLayerHeight, const char* TextureName1=0, const char* TextureName2=0);
	virtual class cExternalObj* CreateExternalObj(ExternalObjFunction func, const char* TextureName1=0);

	// функции для работы с ElasticSphere
	virtual cIUnkClass* CreateElasticSphere(const char* TextureName1=0, const char* TextureName2=0);

	// функции для работы с картой мира
	virtual class cTileMap* CreateMap(class TerraInterface* terra,int zeroplastnumber=0);
	virtual bool Trace(const Vect3f& pStart,const Vect3f& pFinish,Vect3f *pTrace=0);

	// эффекты верхнего уровня
	virtual void AttachObj(cIUnkClass *UObj);
	virtual void DetachObj(cIUnkClass *UObj);

	// работа с анимацией сцены
	inline double GetCurrentTime()									{ return CurrentTime; }

	// для работы с библиотекой объектов
	cObjLibrary* GetObjLibrary()									{ return ObjLibrary; }
	virtual void SetObjLibrary(cObjLibrary *pObjLibrary);
	// доступ к переменным
	inline int GetNumberLight()									{ return UnkLightArray.size(); }
	inline cUnkLight* GetLight(int number)						{ return (cUnkLight*)UnkLightArray[number]; }
	void AttachLight(cUnkLight* ULight);
	void DetachLight(cUnkLight* ULight);
	void GetLighting(const Vect3f &pos,sColor4f &diffuse,sColor4f &specular);
	bool GetLighting(sColor4f &Ambient,sColor4f &Diffuse,sColor4f &Specular,Vect3f &LightDirection);
	void GetLighting(Vect3f *LightDirection);

	class cTileMap *GetTileMap(){return TileMap;}
	// функции для работы с камерами
	virtual cCamera* CreateCamera();

	inline Vect2i GetWorldSize(){return Size;}
	void DisableTileMapVisibleTest();
	void DeleteAutoObject();

	template<class Caller>
	void ScanUnit(Caller& call)
	{
		MTG();
		for(int i=0;i<grid.size();i++)
		{
			cIUnkObj* obj=grid[i];
			if(obj && obj->GetKind(KIND_OBJ_NODE_ROOT)) 
			{
				cObjectNodeRoot* node=(cObjectNodeRoot*)obj;
				terUnitBase* unit=node->GetUnit();
				if(unit)
					call(unit);
			}
		}
	}

	CRITICAL_SECTION& GetLockDraw(){return lock_draw;}
private:
	void Animate();
	cObjLibrary			*ObjLibrary;				// библиотека 3d-объектов
	double				CurrentTime,PreviousTime;	// текущее и предыдущее время
	Vect2i				Size;						// размер мира
	MTGVector			UnkLightArray;				// массив источников света сцены
	sGrid2d				grid;
	QuatTree			tree;

	class cTileMap *TileMap;

	Vect2i TileNumber;
	int tile_size;
	MTDECLARE(lock_draw);

	cCamera*		StrencilShadowDrawNode;
	cCamera*		ReflectionDrawNode;

	bool disable_tilemap_visible_test;

	void AddStrencilCamera(cCamera *DrawNode);
	void AddReflectionCamera(cCamera *DrawNode);

	void BuildTree();
	void CaclulateLightAmbient();
	void UpdateLists(int cur_quant);
};

void SaveKindObjNotFree();

