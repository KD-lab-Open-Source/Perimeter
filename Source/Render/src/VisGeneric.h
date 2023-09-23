#ifndef _VISGENERIC_H_
#define _VISGENERIC_H_
#pragma once

class cScene;
class cObjLibrary;
class cInterfaceRenderDevice;
class cTexture;
class cFont;
class EffectLibrary;

#include "../../PluginMAX/Src/BaseClass.h"
#include "../inc/VisGenericDefine.h"

enum eShadowType
{
	SHADOW_NONE=0,
	SHADOW_STRENCIL=1,
	SHADOW_MAP=2,
	SHADOW_MAP_SELF=4,
	SHADOW_MAP_PERSPECTIVE=3,
};

enum eShowType
{
	SHOW_NORMAL=0,
	SHOW_SHADOW,
	SHOW_REDLECTION,
	SHOW_TILEMAP,
	SHOW_OBJECT,
	SHOW_INFO,
	SHOW_MAX
};

bool GetAllTriangle(const char* filename, std::vector<Vect3f>& point, std::vector<indices_t>& indices);

class cVisGeneric : public cUnknownClass
{
public:
	// инициализационная часть 
	cVisGeneric();
	virtual ~cVisGeneric();
	// общие функции
	virtual void SetData(cInterfaceRenderDevice *pData1);
	virtual void ClearData();
    cInterfaceRenderDevice* GetRenderDevice();
	// функции для работы со сценой
	virtual cScene* CreateScene();

	//Предполагаестя, что все фонты лежат в одной директории,
	//а SetFontDirectory и ReloadAllFont переключает языки 
	void SetFontRootDirectory(const char* dir);//"Resource\fonts\"
	void ReloadAllFont();

	//Создаёт фонт. height - высота фонта в логических единицах.
	//в 1024x768 логическая единица равна пикселю.
    cFont* CreateGameFont(const char *TextureFileName, int height= 20, bool silentErr= false, std::string locale= "");
	virtual cFont* CreateDebugFont();

	// функции для работы со спрайтами
	virtual cTexture* CreateTexture(const char *TextureName);
	virtual cTexture* CreateTexture(int sizex,int sizey,bool alpha);//Всегда 32 битная текстура
	virtual cTexture* CreateRenderTexture(int width,int height,int attr=0,bool enable_assert=true);

	virtual cTexture* CreateTextureDefaultPool(int sizex,int sizey,bool alpha);//Всегда 32 битная текстура

	//Возвращает 32 битную текстуру, в которой хранится изображение экрана.
	virtual cTexture* CreateTextureScreen();
    
	void SetEffectLibraryPath(const char* effect_path,const char* texture_path);
	EffectLibrary* GetEffectLibrary(const char* filename,bool quiet=false);

	// вспомогательные функции
	void SetDrawMeshBound(bool);
	void SetDrawMeshBoundWire(bool);
	void SetMipMapLevel(int);
	void SetMipMapBlur(int);
	void SetDrawNumberPolygon(bool);
	void SetNearDistanceLOD(float);
	void SetFarDistanceLOD(float);
	void SetDrawMeshScreen(bool);
	void SetMapLevel(float);
	void SetShowRenderTextureDBG(bool);
	void SetFavoriteLoadDDS(bool);
	void SetAnisotropic(bool);
	bool GetAnisotropic();

	void SetDebugShowShadowVolume(int);//0,1,2
	void SetShadowType(eShadowType shadow_type,int shadow_size);//shadow_size=0..3
	eShadowType GetShadowType();

	void SetShowType(eShowType type,bool show);
	void XorShowType(eShowType type);
	bool GetShowType(eShowType type);
	void DrawInfo();

	void SetEnableBump(bool enable);
	bool GetEnableBump();
	bool PossibilityBump();

	bool PossibilityShadowMapSelf4x4();
	void SetShadowMapSelf4x4(bool b4x4);

	bool PossibilityBumpChaos();

	class CameraShader* GetShaders(){return shaders;}
	const char* GetEffectPath()const{return effect_path.c_str();};

	void SetLinkEffectToModel(bool link);

	bool PossibilityOcclusion();
	void EnableOcclusion(bool b);
	bool IsEnableOcclusion();
	void SetOcclusionFadeTime(float f);//в секундах
	void EnablePointLight(bool enable);
	bool IsEnablePointLight();

	static void SetAssertEnabled(bool enabled) { assertEnabled_ = enabled; }
	static bool IsAssertEnabled() { return assertEnabled_; }

	//Только в логическом потоке
	void SetLogicQuant(int quant){logic_quant=quant;}
	int GetLogicQuant(){return logic_quant;};

	//Только в графическом потоке
	void SetGraphLogicQuant(int quant){graph_logic_quant=quant;}
	int GetGraphLogicQuant(){return graph_logic_quant;};

	void SetInterpolationFactor(float t){interpolation_factor=t;}
	float GetInterpolationFactor(){return interpolation_factor;};

	void SetLogicTimePeriodInv(float time){logic_time_period_inv=time;};
	float GetLogicTimePeriodInv()const{return logic_time_period_inv;};// 1/ms

	void SetGlobalParticleRate(float r);

	cObjLibrary* GetObjLib(){return ObjLibrary;}

	void SetShadowHint(int hint);
	int GetShadowHint();
private:
	int			logic_quant,graph_logic_quant;
	cObjLibrary				*ObjLibrary;
	std::string font_root_directory;
	std::vector<class cFontInternal*>	fonts;
	friend class cD3DRender;

	class CameraShader* shaders;

	void InitShaders();
	void ReleaseShaders();

	void CalcIsShadowMap();

	MTDECLARE(lock_effect_library);
	std::vector<EffectLibrary*> effect_library;
	std::string effect_path,effect_texture_path;
	float interpolation_factor;
	float logic_time_period_inv;

	static bool assertEnabled_;
};

extern class cVisGeneric		*gb_VisGeneric;

class MTTexObjAutoLock
{
	MTAuto object_lock;
	MTAuto texture_lock;
public:
	MTTexObjAutoLock();
};

#endif // _VISGENERIC_H_
