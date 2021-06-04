#pragma once

#include "..\saver\saver.h"
#include "NParticleKey.h"
#include "observer.h"
#include "texture.h"
#define EXPORT_TO_GAME 1
#ifndef _FINAL_VERSION_
	#define NEED_TREANGLE_COUNT 
	#define EFFECTTOOL
#endif

class cEffect;

struct cObjectParticleInterface
{
};

struct KeyParticleInt
{
	float dtime;

	float vel;//скорость частицы, абсолютное значение
	float angle_vel;
	sColor4f color;
	float size;//величина частицы
	float gravity;
//protected:
	friend class cEmitterInt;
	friend class cEmitterZ;
	float inv_dtime;//inv_dtime=1/dtime
};

struct KeyParticleSpl
{
	float dtime;

	float angle_vel;
	sColor4f color;
	float size;//величина частицы
//protected:
	friend class cEmitterSpl;
	float inv_dtime;//inv_dtime=1/dtime
};

enum EMITTER_CLASS
{
	EMC_BAD=0,
	EMC_INTEGRAL,
	EMC_INTEGRAL_Z,
	EMC_SPLINE,
	EMC_LIGHT,
};

enum EMITTER_TYPE_POSITION
{
	EMP_BOX=0,
	EMP_CYLINDER,
	EMP_SPHERE,
	EMP_LINE,
	EMP_RING,
	EMP_3DMODEL,
	EMP_3DMODEL_INSIDE,
	EMP_OTHER_EMITTER,
};

enum EMITTER_TYPE_VELOCITY
{
	EMV_BOX=0,
	EMV_CYLINDER=1,
	EMV_SPHERE=2,
	EMV_LINE=3,
	EMV_NORMAL_IN=4,
	EMV_NORMAL_OUT=5,
	EMV_NORMAL_IN_OUT=6,
	EMV_Z=7,
	EMV_NORMAL_3D_MODEL = 8,
	EMV_INVARIABLY,
};

enum EMITTER_TYPE_DIRECTION_SPL
{
	ETDS_ID,
	ETDS_ROTATEZ,
	ETDS_BURST1,
	ETDS_BURST2,
};

enum EMITTER_TYPE_ROTATION_DIRECTION
{
	ETRD_CW=0,
	ETRD_CCW=1,
	ETRD_RANDOM=2,
};

enum EMITTER_BLEND
{
	EMITTER_BLEND_MULTIPLY=0,
	EMITTER_BLEND_ADDING=1,
	EMITTER_BLEND_SUBSTRACT=2,
};

struct EmitterType
{
	EMITTER_TYPE_POSITION type;
	//Последующие значения зависят от типа эмиттера
	Vect3f size;
	bool fix_pos;
	struct CountXYZ{short x,y,z;}num;
	//r=size.x
	float alpha_min,alpha_max,teta_min,teta_max;

	EmitterType()
	{
		type=EMP_BOX;
		size.set(0,0,0);
		alpha_min=-M_PI;
		alpha_max=+M_PI;
		teta_min=-M_PI/2;
		teta_max=+M_PI/2;
		fix_pos = false;
		num.x = num.y = 5;
		num.z =1;
	}

	void Save(CSaver& s,int id);
	void Load(CLoadIterator rd);
};

struct EffectSpeedParam
{
	Vect3f pos;
	Vect3f dpos;
};

struct EffectBeginSpeed:KeyGeneral
{
	string name;
	EMITTER_TYPE_VELOCITY velocity;
	float mul;
	QuatF rotation;

	EffectSpeedParam esp;

	EffectBeginSpeed()
	{
		velocity=EMV_BOX;
		mul=1.0f;
		rotation=QuatF::ID;
		time=0;
		esp.pos.set(1,1,1);
		esp.dpos.set(0,0,0);
	}

	void Save(CSaver& s);
	void Load(CLoadIterator rd);

};

struct EffectBeginSpeedMatrix
{
	EMITTER_TYPE_VELOCITY velocity;
	float mul;
	Mat3f rotation;
	float time_begin;

	EffectSpeedParam esp;
};

inline bool operator <(const EffectBeginSpeedMatrix& s1,const EffectBeginSpeedMatrix& s2)
{
	return s1.time_begin<s2.time_begin;
}

class SpiralData
{
public:
	class Dat
	{
	public:
		MatXf mat;
		float dalpha;
		float r1,r2;
		float h1,h2;
		float compress;
		Dat():mat(MatXf::ID)
		{
			dalpha=0.5;
			r1=0;
			r2 = 100;
			h1= 0;
			h2= 100;
			compress= 1;
		}
	protected:
	friend class SpiralData;
		void Save(CSaver& s)
		{
			s<<dalpha;
			s<<r1;
			s<<r2;
			s<<h1;
			s<<h2;
			s<<compress;
		}
		void Load(CLoadIterator &rd)
		{
			rd>>dalpha;
			rd>>r1;
			rd>>r2;
			rd>>h1;
			rd>>h2;
			rd>>compress;
		}
	};
protected:
	Dat dat[2];
	int active;
public:
	void SetActive(int ix){ xassert((UINT)ix<2); active = ix;}
	Dat& GetActiveData(){return dat[active];}
	Dat& GetData(int ix){xassert((UINT)ix<2); return dat[ix];}
	void Save(CSaver& s, int id)
	{
		if (s.GetData()!=EXPORT_TO_GAME)
		{
			s.push(id);     
			dat[0].Save(s);
			dat[1].Save(s);
			s.pop();
		}
	}
	void Load(CLoadData* ld)
	{
		CLoadIterator rd(ld);
		dat[0].Load(rd);
		dat[1].Load(rd);
	}
};
/////////////////////////////EmitterKey/////////////////////////////////////////

struct EmitterKeyInterface
{
	EmitterKeyInterface();
	virtual ~EmitterKeyInterface(){};
	virtual void Save(CSaver& s)=0;
	virtual void Load(CLoadDirectory rd)=0;
	virtual void RelativeScale(float scale)=0;
	virtual EMITTER_CLASS GetType()=0;
	virtual void BuildKey()=0;

#ifdef EFFECTTOOL
	SpiralData sdat;
#endif
	string name;
	string texture_name;
	bool cycled;//После emitter_life_time эмиттер начинает работать заново
	float emitter_create_time,emitter_life_time;

	float LocalTime(float t);
	float GlobalTime(float t);

	virtual EmitterKeyInterface* Clone()=0;
};

struct EmitterKeyLight:public EmitterKeyInterface
{
public:
	EmitterKeyLight();
	virtual void Save(CSaver& s);
	virtual void Load(CLoadDirectory rd);
	virtual void RelativeScale(float scale);
	virtual EmitterKeyInterface* Clone();
	virtual EMITTER_CLASS GetType(){return EMC_LIGHT;};
	virtual void BuildKey();

	CKeyPos   emitter_position;
	CKey	  emitter_size;
	CKeyColor emitter_color;
};

struct EmitterKeyBase:public EmitterKeyLight//EmitterKeyInterface
{
	EmitterKeyBase();
	virtual ~EmitterKeyBase();
	virtual void Save(CSaver& s)=0;
	virtual void Load(CLoadDirectory rd)=0;
	virtual void RelativeScale(float scale);
	virtual EMITTER_CLASS GetType()=0;
	virtual void BuildKey()=0;

	//Параметры эмиттера
	EMITTER_BLEND sprite_blend;
	bool generate_prolonged;
	float particle_life_time;
	EMITTER_TYPE_ROTATION_DIRECTION rotation_direction;

	CKeyPos    emitter_position;
	CKeyRotate emitter_rotation;
	EmitterType particle_position;

	///Общие параметры частиц
	CKey life_time;
	CKey life_time_delta;
	CKey begin_size;
	CKey begin_size_delta;
	CKey num_particle;
	string other;
	bool relative;

	bool  chFill;
	bool  chPlume;
	int   TraceCount;
	float PlumeInterval;
	bool smooth;

	CVectVect3f begin_position; //Распределение по 3D модели for EMP_3DMODEL_INSIDE
	CVectVect3f normal_position;//Распределение по 3D модели  for EMP_3DMODEL_INSIDE

	//Параметры отдельной частицы
	CKey	  p_size;
	CKeyColor p_color;
	CKeyColor p_alpha;
	CKey	  p_angle_velocity;


	//Возвращает в секундах, принимает в LocalTime
	void GetParticleLifeTime(float t,float& mid_t,float& min_t,float& max_t);

	KeyPos* GetOrCreatePosKey(float t,bool* create);
	KeyRotate* GetOrCreateRotateKey(float t,bool* create);
	void GetPosition(float t,MatXf& m);
	void Load3DModelPos(cEmitter3dObject* model/*LPCTSTR fname*/);
protected:
	virtual void SaveInternal(CSaver& s);
	virtual void LoadInternal(CLoadData* ld);
	void add_sort(vector<float>& xsort,CKey& c);
	void add_sort(vector<float>& xsort,CKeyColor& c);
	void end_sort(vector<float>& xsort);
};

struct EmitterKeyInt:public EmitterKeyBase
{
	EmitterKeyInt();
	~EmitterKeyInt();

	void Save(CSaver& s);
	void Load(CLoadDirectory rd);
	void RelativeScale(float scale);
	EmitterKeyInterface* Clone();
public:
	bool use_light;//Освещение частиц, только при EMP_3DMODEL,EMP_3DMODEL_INSIDE. 
				//Используется из первого попавшегося материала diffuse и ambient.

	//Параметры отдельной частицы
	CKey	  p_velocity;
	CKey	  p_gravity;

	EMITTER_CLASS GetType(){return EMC_INTEGRAL;}
	void BuildKey();
	vector<KeyParticleInt>& GetKey(){return key;};

public:
	CKey velocity_delta;

	vector<EffectBeginSpeed> begin_speed;
protected:
	vector<KeyParticleInt> key;
	virtual void SaveInternal(CSaver& s);
};


struct EmitterKeyZ:public EmitterKeyInt
{
	EmitterKeyZ();

	void Save(CSaver& s);
	void RelativeScale(float scale);
	EmitterKeyInterface* Clone();

	EMITTER_CLASS GetType(){return EMC_INTEGRAL_Z;}

	float add_z;
	bool planar;
	bool angle_by_center;//Не имеет смысла ставить true, если planar=false
	float base_angle;
	bool use_force_field;
protected:
	void LoadInternal(CLoadData* ld);
};

struct EmitterKeySpl:public EmitterKeyBase
{
	EmitterKeySpl();
	~EmitterKeySpl();

	void Save(CSaver& s);
	void Load(CLoadDirectory rd);
	void RelativeScale(float scale);
	EmitterKeyInterface* Clone();
public:	
	bool p_position_auto_time;//Автоматически прределять время для наиболее равномерного движения
	CKeyPosHermit    p_position;

	EMITTER_TYPE_DIRECTION_SPL direction;

	EMITTER_CLASS GetType(){return EMC_SPLINE;}
	void BuildKey();
	vector<KeyParticleSpl>& GetKey(){return key;};
protected:
	vector<KeyParticleSpl> key;
};

class EffectKey
{
public:
	string name;
	vector<EmitterKeyInterface*> key;

	EffectKey();
	~EffectKey();

	void Save(CSaver& s);
	void Load(CLoadDirectory rd);

	void RelativeScale(float scale);

	bool IsNeed3DModel();
	bool IsCycled();

	void operator=(const EffectKey& effect_key);
protected:
	void Clear();
};


class FunctorGetZ:public cUnknownClass
{
public:
	virtual void UpdateInterface(cEffect* parent)=0;
	virtual float GetZ(float x,float y)=0;
};

/////////////////////////////cEmitter/////////////////////////////////////////
class cEmitterInterface:public cUnkObj
{
	friend class cEffect;
public:
	cEmitterInterface();
	~cEmitterInterface();

	virtual bool IsLive()=0;
	virtual bool IsVisible(cCamera *pCamera)=0;
	virtual void SetDummyTime(float t)=0;

	void SetPause(bool b);
	bool GetPause()const{return b_pause;};
	virtual void SetParent(cEffect* parent);
	void SetCycled(bool cycled);
	bool IsCycled()const{return cycled;}

	float GetStartTime()const{return start_time;};
	float GetLiveTime()const{return emitter_life_time;}
	EmitterKeyInterface* GetUnicalID(){return unical_id;};
	void SetShowObjEditor(bool b){no_show_obj_editor=!b;}

	virtual void SetFunctorGetZ(FunctorGetZ* func){}
	virtual void AddZ(float z){}
protected:
	virtual void DisableEmitProlonged(){}
	cEffect* parent;

	float time,old_time;
	bool b_pause;
	bool cycled;
	float start_time;
	float emitter_life_time;
	EmitterKeyInterface* unical_id;
	bool no_show_obj_editor;
};

class cEmitterBase:public cEmitterInterface
{
public:
	cEmitterBase* other;
	cEmitterBase();
	~cEmitterBase();

	virtual void PreDraw(cCamera *pCamera);
	virtual void Animate(float dt);

	virtual bool IsLive()=0;

	void SetMaxTime(float emitter_life,float particle_life);
	void SetDummyTime(float t){dummy_time=t;};

	bool IsVisible(cCamera *pCamera);


protected:

	float dummy_time;
	float inv_emitter_life_time;
	float particle_life_time;
	float inv_particle_life_time;
	eBlendMode sprite_blend;

	sBox6f						Bound;

	bool prolonged;
	bool disable_emit_prolonged;
	float time_emit_prolonged;

	CKeyPos position;
	EMITTER_TYPE_ROTATION_DIRECTION rotation_direction;
	CKeyRotate rotation;

	vector<Vect3f> begin_position; //Распределение по 3D модели for EMP_3DMODEL_INSIDE
	//if (cEmitterInt::use_light || cEmitterSpl::direction == ETDS_BURST1,ETDS_BURST2) 
	vector<Vect3f> normal_position;//Распределение по 3D модели  for EMP_3DMODEL_INSIDE
	//endif
	EmitterType particle_position;
public:
	int cur_one_pos;//Индекс в begin_position
	virtual Vect3f GetVdir(int i)=0;
protected:

	CKey num_particle;
	CKey begin_size;
	CKey begin_size_delta;
	CKey inv_life_time;
	CKey life_time_delta;
	bool relative;
	bool  chFill;
	bool  chPlume;
	int   TraceCount;
	float PlumeInterval;

	template<class nParticle> int PutToBuf(nParticle& p, Vect3f& npos, float& dt,
										cQuadBuffer<sVertexXYZDT1>*& pBuf, 
										const sColor4c& color, const Vect3f& PosCamera,
										const float& size, const cTextureAviScale::RECT& rt,
										const UCHAR planar, MatXf* iGM = NULL);


	enum 
	{
		rotate_angle_size=256,
		rotate_angle_mask=rotate_angle_size-1,
	};
	void InitRotateAngle();

	Vect3f RndBox(float x,float y,float z);
	Vect3f RndSpere(float r);
	Vect3f RndCylinder(float r,float dr,float z);
	Vect3f RndRing(float r,float alpha_min,float alpha_max,float teta_min,float teta_max);

	void CalcPosition();
	virtual void DummyQuant()=0;
	virtual void EmitInstantly(float tmin,float tmax)=0;
	virtual void EmitProlonged(float dt)=0;
	int CalcProlongedNum(float dt);

	void cEmitterBase::OneOrderedPos(int i,Vect3f& pos);
	bool OnePos(int i, Vect3f& pos, Vect3f* norm = NULL);
	virtual bool GetRndPos(Vect3f& pos, Vect3f* norm)=0;
	Vect3f* GetNormal(const int& ix);

	void SetEmitterKey(EmitterKeyBase& k,cEmitter3dObject* models);
	void DisableEmitProlonged(){disable_emit_prolonged=true;}

	bool init_prev_matrix;
	MatXf prev_matrix;
	MatXf next_matrix;
	void BeginInterpolateGlobalMatrix();
	void InterpolateGlobalMatrix(float f);
	void EndInterpolateGlobalMatrix();
public:
	virtual void CalculatePos(bool mode){;}
	virtual int GetParticleCount(){return 0;}
	virtual Vect3f& GetParticlePos(int ix){static Vect3f t;return t;}
	virtual void ResetPlumePos(int i){};
};


class cEmitterInt:public cEmitterBase
{
public:
	struct nParticle
	{
		float time;//текущее время в пределах ключа анимации 0..1
		int key;//номер ключа анимации
		float inv_life_time;

		Vect3f pos0;
		Vect3f vdir;//начальное направление движения
		float angle0,angle_dir;
		float gvel0;
		//color0,size0 - константы
		float begin_size;

		float time_summary;
		int key_begin_time;
		sColor4c begin_color;

		Vect3f normal;
		vector<Vect3f> plume_pos;
/*
		void PutToBuf(const float& dtime_global, const KeyParticleInt& k0, 
								const KeyParticleInt& k1, sBox6f& Bound, 
								const cTextureAviScale::RECT& rt,
								const MatXf& mat, cQuadBuffer<sVertexXYZDT1>*& pBuf, 
								const cEmitterInt* emi, Vect3f& pos, float& dtime, 
								const float& t, const float trace_size);
		void ZPutToBuf(const float& dtime_global, const KeyParticleInt& k0, 
								const KeyParticleInt& k1, sBox6f& Bound, 
								const cTextureAviScale::RECT& rt,
								const MatXf& mat, cQuadBuffer<sVertexXYZDT1>*& pBuf, 
								cEmitterZ* emi, Vect3f& pos, float& dtime, 
								const float& t, const float trace_size);
*/
	};
protected:
	BackVector<nParticle>	Particle;

	vector<KeyParticleInt>	keys;
	vector<EffectBeginSpeedMatrix> begin_speed;

	Vect3f g;	//ускорение

	/////
	CKey velocity_delta;
public:
	cEmitterInt();
	~cEmitterInt();

	virtual void Draw(cCamera *pCamera);

	bool IsLive(){return !Particle.is_empty() || time<emitter_life_time || cycled;}

	void SetEmitterKey(EmitterKeyInt& k,cEmitter3dObject* models);
protected:
	bool use_light;
	cObjMaterial material;
/*	void PutPlumeParticle(const float& dtime_global, sBox6f& Bound, 
						 cTextureAviScale*& texture, cTextureAviScale*& plume_texture, 
						 const MatXf& mat, nParticle& p,
						 cQuadBuffer<sVertexXYZDT1>*& pBuf,
						 Vect3f& pos, float& dtime );
	void ZPutPlumeParticle(const float& dtime_global, sBox6f& Bound, 
						 cTextureAviScale*& texture, cTextureAviScale*& plume_texture, 
						 const MatXf& mat, nParticle& p,
						 cQuadBuffer<sVertexXYZDT1>*& pBuf,
						 Vect3f& pos, float& dtime );
*/
	void SetKeys(vector<KeyParticleInt>& k);

	void EmitInstantly(float tmin,float tmax);
	void EmitProlonged(float dt);
	virtual void EmitOne(int ix_cur/*nParticle& cur*/,float begin_time);
	virtual bool GetRndPos(Vect3f& pos, Vect3f* norm);
	virtual Vect3f GetVdir(int i);
	Vect3f CalcVelocity(const EffectBeginSpeedMatrix& s,const nParticle& cur,float mul);

	virtual void ProcessTime(nParticle& p,float dt,int i,Vect3f& cur_pos);
	void DummyQuant();
	void CalcColor(nParticle& cur);
protected:
	bool calc_pos;
public:
	virtual void CalculatePos(bool mode){calc_pos = mode;}
	virtual int GetParticleCount(){return Particle.size();}
	virtual Vect3f& GetParticlePos(int ix){xassert((UINT)ix<Particle.size());return Particle[ix].pos0;}
	virtual void ResetPlumePos(int ix)
	{
		xassert((UINT)ix<Particle.size());
		nParticle& p = Particle[ix]; 
		vector<Vect3f>::iterator i;
		FOR_EACH(p.plume_pos,i)
			*i = p.pos0;
	};
};

class cEmitterZ:public cEmitterInt
{
	bool planar;
	bool smooth;
	float add_z;
	bool angle_by_center;
	float base_angle;
	bool use_force_field;
	FunctorGetZ* func_getz;
public:
	cEmitterZ();
	~cEmitterZ();
	void Draw(cCamera *pCamera);
	virtual void ProcessTime(nParticle& p,float dt,int i,Vect3f& cur_pos);
	void SetEmitterKey(EmitterKeyZ& k,cEmitter3dObject* models);

	void SetParent(cEffect* parent);
	float CalcZ(float pos_x,float pos_y);
	void SetFunctorGetZ(FunctorGetZ* func){RELEASE(func_getz);func_getz=func;func_getz->IncRef();};
	virtual void AddZ(float z){add_z+=z;}
protected:
	virtual bool GetRndPos(Vect3f& pos, Vect3f* norm);
	virtual void EmitOne(int ix_cur/*nParticle& cur*/,float begin_time);
};

class cEmitterSpl:public cEmitterBase
{
//#define EMITTER_TCONTROL
	struct HeritKey
	{
		float dtime;
		float inv_dtime;//inv_dtime=1/dtime
		Vect3f H0,H1,H2,H3;
#ifdef EMITTER_TCONTROL
		float t0,t1,t2,t3;
		inline void Get(Vect3f& out,float ts)
		{
			float t=t0+t1*ts+t2*ts*ts+t3*ts*ts*ts;
			//VISASSERT(t==ts);
			float tt=t*t,ttt=tt*t;
			out.x=H0.x+H1.x*t+H2.x*tt+H3.x*ttt;
			out.y=H0.y+H1.y*t+H2.y*tt+H3.y*ttt;
			out.z=H0.z+H1.z*t+H2.z*tt+H3.z*ttt;
		}
#else
		inline void Get(Vect3f& out,float t)
		{
			float tt=t*t,ttt=tt*t;
			out.x=H0.x+H1.x*t+H2.x*tt+H3.x*ttt;
			out.y=H0.y+H1.y*t+H2.y*tt+H3.y*ttt;
			out.z=H0.z+H1.z*t+H2.z*tt+H3.z*ttt;
		}
#endif
	};
	struct nParticle
	{
		int   key;//номер ключа анимации
		float time;//текущее время в пределах ключа анимации 0..dtime
		float inv_life_time;
		float time_summary;

		//То-же, но для сплайнов
		int   hkey;
		float htime;
		vector<Vect3f> plume_pos;
		MatXf pos;
		float angle0,angle_dir;
		//color0,size0 - константы
		float begin_size;
/*		void PutToBuf(const float& dtime_global, HeritKey& k, const KeyParticleSpl& k0, 
								const KeyParticleSpl& k1, sBox6f& Bound, 
								const cTextureAviScale::RECT& rt,
								const MatXf& mat, cQuadBuffer<sVertexXYZDT1>*& pBuf, 
								cEmitterSpl* emi, Vect3f& pos, float& dtime, 
								const float& t, const float& trace_size, const float& htime);
*/
	};

	BackVector<nParticle>	Particle;
	vector<KeyParticleSpl>	keys;
	vector<HeritKey>		hkeys;

	EMITTER_TYPE_DIRECTION_SPL direction;
public:
	cEmitterSpl();
	~cEmitterSpl();

	virtual void Draw(cCamera *pCamera);
	bool IsLive(){return !Particle.is_empty() || time<emitter_life_time || cycled;}

	void SetEmitterKey(EmitterKeySpl& k,cEmitter3dObject* models);
protected:
//	friend cEmitterSpl::nParticle;
/*	void PutPlumeParticle(const float& dtime_global, sBox6f& Bound, 
						 cTextureAviScale*& texture, cTextureAviScale*& plume_texture, 
						 const MatXf& mat, nParticle& p,
						 cQuadBuffer<sVertexXYZDT1>*& pBuf,
						 Vect3f& pos, float& dtime);*/
	void SetKeys(vector<KeyParticleSpl>& k);

	void EmitInstantly(float tmin,float tmax);
	void EmitProlonged(float dt);
	void EmitOne(int ix_cur/*nParticle& cur*/,float begin_time);
	virtual bool GetRndPos(Vect3f& pos, Vect3f* norm);
	virtual Vect3f GetVdir(int i);
	void ProcessTime(nParticle& p,float dt,int i);
	void DummyQuant();
};


class cEmitterLight:public cEmitterInterface
{
public:
	cEmitterLight();
	~cEmitterLight();
	void Animate(float dt);

	bool IsLive(){return time<emitter_life_time || cycled;}
	bool IsVisible(cCamera *pCamera){return false;}

	void SetEmitterKey(EmitterKeyLight& k);
	void SetDummyTime(float t){};
protected:
	class cUnkLight* light;

	CKeyPos   emitter_position;
	CKey	  emitter_size;
	CKeyColor emitter_color;
};

class cEffect:public cIUnkObjScale
{
	friend class cEmitterBase;
	vector<cEmitterInterface*> emitters;
	float time;
	bool auto_delete_after_life;
	float particle_rate;

	class EffectObserverLink:protected ObserverLink
	{
		class cObjectNode* node;
		cEffect* effect;
	public:
		EffectObserverLink():node(0),effect(0){}
		void SetParent(cEffect* effect_){effect=effect_;}

		void Link(class cObjectNode* node);
		virtual void Update();
		cObjectNode* GetNode(){return observer?node:NULL;}
	} link;

	class EffectObserverLink3dx:protected ObserverLink
	{
		class cObject3dx* object;
		int node;
		cEffect* effect;
	public:
		EffectObserverLink3dx():object(0),node(-1),effect(0){}
		void SetParent(cEffect* effect_){effect=effect_;}

		void Link(class cObject3dx* object,int inode);
		virtual void Update();

		bool IsInitialized(){return object!=0;}
		const MatXf& GetRootMatrix();
	} link3dx;
public:
	cEffect();
	~cEffect();

	virtual void Animate(float dt);
	virtual void PreDraw(cCamera *pCamera);
	virtual void Draw(cCamera *pCamera);

	bool IsLive();
	void Clear();

	float GetTime()const {return time;};
	float GetSummaryTime();
	void SetPosition(const MatXf& Matrix);
	void AddZ(float z)
	{
		vector<cEmitterInterface*>::iterator it;
		FOR_EACH(emitters,it)
			(*it)->AddZ(z);
	}
	void SetCycled(bool cycled);
	bool IsCycled()const;
	
	void ShowEmitter(EmitterKeyBase* emitter_id,bool show);
	void ShowAllEmitter();
	void HideAllEmitter();
	void SetTime(float time);
	void MoveToTime(float time);

	//Этой функцией следует пользоваться с осторожностью, лучше использовать StopAndReleaseAfterEnd
	void SetAutoDeleteAfterLife(bool auto_delete_after_life_);
	bool IsAutoDeleteAfterLife()const{return auto_delete_after_life;}

	//Остановить генерацию спрайтов и удалить спецэффект после исчезновения спрайтов 
	void StopAndReleaseAfterEnd();

	void SetParticleRate(float rate){particle_rate=rate;};
	inline float GetParticleRate()const{return particle_rate;}

	void LinkToNode(class cObjectNode* node);
	void LinkToNode(class cObject3dx* object,int inode);
	inline float GetParticleRateReal()const;

	vector<Vect3f>& GetPos(){return begin_position;}
	vector<Vect3f>& GetNorm(){return normal_position;}
	cEmitterBase* GetEmitN(int n){xassert((UINT)n<emitters.size()); return (cEmitterBase*)emitters[n];}
	void SetFunctorGetZ(FunctorGetZ* func);//Делается вовремя addref,release
protected:
#ifdef  NEED_TREANGLE_COUNT
	int count_triangle;
	double square_triangle;
#endif
public:
	void AddCountTriangle(int count);
	void AddSquareTriangle(float sq);
	int GetTriangleCount();
	double GetSquareTriangle();

protected:
	friend class cScene;
	void Init(EffectKey& el,cEmitter3dObject* models,float scale=1.0f);
	void Add(cEmitterInterface*);//Предполагается, что эмиттер уже инициализированн

	const MatXf& GetCenter3DModel();
	vector<Vect3f> begin_position;//Распределение по 3D модели
	vector<Vect3f> normal_position;//Распределение по 3D модели


#ifdef _DEBUG
	EffectKey* debug_effect_key;
#endif _DEBUG
	FunctorGetZ* func_getz;
};

class EffectLibrary
{
	vector<EffectKey*> lst;
	string filename;
public:
	EffectLibrary();
	~EffectLibrary();

	bool Load(const char* filename,const char* texture_path=NULL);
	EffectKey* Get (const char* name) const;

	const char* GetFileName()const{return filename.c_str();}
	//Для редактора спецэффектов
	typedef vector<EffectKey*>::iterator iterator;
	inline iterator begin(){return lst.begin();}
	inline iterator end(){return lst.end();}
	void ClearNoDelete();
};
