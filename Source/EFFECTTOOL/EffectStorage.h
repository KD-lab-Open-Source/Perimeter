
#pragma once


#define FOLDER_SPRITE "Sprite\\"
#define FOLDER_TEXTURES "Textures\\"

//#define DEFAULT_TXT_FIRE "fire01.tga"
//#define DEFAULT_TXT_LIGHT "lightmap.tga"

///////////////////////////////////////////////////////////////////////////
//
struct CEmitterData 
{
	EmitterKeyBase* data;
	EmitterKeyLight* data_light;
	bool            bActive;
	bool            bDirty;

//	CKey            key_dummy;

	CEmitterData(EMITTER_CLASS cls);
	CEmitterData(EmitterKeyInterface* pk);
	~CEmitterData();

	void Reset(EMITTER_CLASS cls);

	EMITTER_CLASS		Class();

	EmitterKeyInt*		GetInt();
	EmitterKeySpl*		GetSpl();
	EmitterKeyZ*		GetZ();
	EmitterKeyLight*	GetLight();

	float Effect2EmitterTime(float fEffectTime);
	float ParticleLifeTime();
	float GenerationLifeTime(int nGeneration);
	float GenerationPointTime(int nGenPoint);
	float GenerationPointGlobalTime(int nGenPoint);
	float EmitterEndTime(){
		if(data)
			return data->emitter_life_time + data->emitter_create_time;
		else
			return data_light->emitter_life_time + data_light->emitter_create_time;
	}
	float& EmitterLifeTime()
	{
		return (data)? data->emitter_life_time : data_light->emitter_life_time;
	}
	float& EmitterCreateTime()
	{
		return (data)? data->emitter_create_time : data_light->emitter_create_time;
	}
	CKeyPos& emitter_pos()
	{
		return (data) ? data->emitter_position : data_light->emitter_position;
	}
	void SetGenerationPointCount(int nPoints);
	void InsertGenerationPoint(int nBefore, float k =0);
	void SetGenerationPointTime(int nPoint, float tm);
	void InsertParticleKey(int nBefore,float); //temp
	void SetParticleKeyTime(int nKey, float tm);
	void DeleteGenerationPoint(int nPoint);
	void DeleteParticleKey(int nPoint);
	void ChangeLifetime(float tm);
	void ChangeGenerationLifetime(int nGeneration, float tm);
	void DeletePositionKey(KeyPos* key);

	void GetBeginSpeedMatrix(int nGenPoint, int nPoint, MatXf& mat);

	void Save(CSaver &sv);
	void Load(CLoadData* rd);
};										

typedef vector<EffectBeginSpeed> BSKey;

///////////////////////////////////////////////////////////////////////////////////////////
// key wrappers
struct KeyWrapBase
{
	virtual int    size() = 0;
	virtual float& time(int i) = 0;
	virtual float& value(int i) = 0;

	float& operator()(int i){return value(i);}
};

struct KeyDummyWrap : KeyWrapBase
{
	float _f;
	int    size(){return 0;}
	float& time(int i){return _f;}
	float& value(int i){return _f;}
};
struct KeyFltWrap : KeyWrapBase
{
	CKey* k;
	CKeyPosHermit* kh;
	CKeyPos* kl;
	bool hermit;
	bool light;
	float f;

	KeyFltWrap(CKey& _k)			{k = &_k;  kh = NULL;  kl = NULL; hermit = false; light = false; f = 0;}
	KeyFltWrap(CKeyPosHermit& _kh)	{k = NULL; kh = &_kh;  kl = NULL; hermit = true;  light = false; f = 0;}
	KeyFltWrap(CKeyPos& _kl)		{k = NULL; kh = NULL;  kl = &_kl; hermit = false; light = true; f = 0;}

	int size()
	{			
		if(hermit) return (*kh).size();	 
		else{
			if(light)return (*kl).size(); 
			else return (*k).size();
		}
	}
	float& time(int i)
	{	
		if(hermit) return (*kh)[i].time; 
		else{
			if(light)return (*kl)[i].time;
			else return (*k)[i].time;
		}
	}
	float& value(int i) 
	{	
		if(hermit) 
			return f;	
		else 
			return (*k)[i].f;
	}
};
struct KeyBsWrap : KeyWrapBase
{
	BSKey& k;

	KeyBsWrap(BSKey& _k) : k(_k){}

	int    size(){return k.size();}
	float& time(int i){return k[i].time;}
	float& value(int i) {return k[i].mul;}
};

///////////////////////////////////////////////////////////////////////////
//
typedef TAutoReleaseContainer< vector<CEmitterData*> > EmitterListType;

struct CEffectData : public EffectKey
{
	EmitterListType   emitters;

	CString			Path3Dmodel;
	bool			Show3Dmodel;

	bool              bDirty;

	bool			bExpand;

	CEffectData();
	CEffectData(EffectKey* pk);
	~CEffectData();

	CEmitterData* add_emitter(EMITTER_CLASS cls = EMC_INTEGRAL);
	void del_emitter(CEmitterData* p);

	void swap_emitters(CEmitterData* p1, CEmitterData* p2);

	void prepare_effect_data(CEmitterData *pActiveEmitter);

	float GetTotalLifeTime();

	bool CheckName(LPCTSTR lpsz, CEmitterData* p = 0);
	bool HasModelEmitter();
};

typedef  TAutoReleaseContainer< vector<CEffectData*> >  EffectStorageType;

