
#include "stdafx.h"
#include "EffectTool.h"
#include "EffectStorage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////////

CEmitterData::CEmitterData(EMITTER_CLASS cls)
{
	data = 0;
	data_light = 0;
	bActive = true;
	bDirty = true;

	Reset(cls);
}
CEmitterData::CEmitterData(EmitterKeyInterface* pk)
{
	bActive = true;
	bDirty = true;

	data_light = 0;
	data = 0;
	switch(pk->GetType())
	{
	case EMC_INTEGRAL_Z:
		{
			data = new EmitterKeyZ(*(EmitterKeyZ*)pk);
			*(EmitterKeyZ*)data = *(EmitterKeyZ*)pk;
		}
		break;
	case EMC_LIGHT:
		{
			data_light = new EmitterKeyLight;
			*data_light=*(EmitterKeyLight*)pk;
		}
		break;
	case EMC_INTEGRAL:
		{
			data = new EmitterKeyInt;
			*(EmitterKeyInt*)data = *(EmitterKeyInt*)pk;
		}
		break;

	case EMC_SPLINE:
		{
			data = new EmitterKeySpl;
			*(EmitterKeySpl*)data = *(EmitterKeySpl*)pk;
		}
		break;
	default:
		xassert(0);
	}

}
CEmitterData::~CEmitterData()
{
	if(data)
		delete data;
	if(data_light)
		delete data_light;
}
void CEmitterData::Reset(EMITTER_CLASS cls)
{
	bDirty = true;

	static int _unkn = 1;

	CString name;

	if(data)
		name = data->name.c_str();

	if(data_light)
		name = data_light->name.c_str();

	if(data)
	{
		delete data;
		data = 0;
	}

	if(data_light)
	{
		delete data_light;
		data_light = 0;
	}

	switch(cls)
	{
	case EMC_INTEGRAL_Z:
		data = new EmitterKeyZ;

		GetZ()->begin_speed.push_back(EffectBeginSpeed());
		GetZ()->begin_speed.push_back(EffectBeginSpeed());
		GetZ()->begin_speed.back().time = 1.f;

		GetZ()->p_velocity[0].f = 0;
		GetZ()->p_velocity.push_back(KeyFloat(1, 0));
		GetZ()->p_gravity.push_back(KeyFloat(1, GetInt()->p_gravity[0].f));
		break;
	case EMC_LIGHT:
		data_light = new EmitterKeyLight;
		
//		GetLight()->emitter_position.push_back();
//		GetLight()->emitter_position.push_back();

//		GetLight()->emitter_size[0].f = 10;
//		GetLight()->emitter_size.push_back(KeyFloat(0, 10));
//		GetLight()->emitter_size.push_back(KeyFloat(1, 10));
		break;
	case EMC_INTEGRAL:
		data = new EmitterKeyInt;

		GetInt()->begin_speed.push_back(EffectBeginSpeed());
		GetInt()->begin_speed.push_back(EffectBeginSpeed());
		GetInt()->begin_speed.back().time = 1.f;

		GetInt()->p_velocity[0].f = 0;
		GetInt()->p_velocity.push_back(KeyFloat(1, 0));
		GetInt()->p_gravity.push_back(KeyFloat(1, GetInt()->p_gravity[0].f));
		break;
	case EMC_SPLINE:
		data = new EmitterKeySpl;
		data->num_particle[0].f = 10;
		break;
	default:
		xassert(0);
	}

	if(	cls==EMC_LIGHT){
		data_light->emitter_life_time = 1.0f;
	}else{
		data->emitter_life_time = 1.0f;
	}

	if(	cls!=EMC_LIGHT){
		data->cycled = false;
		data->generate_prolonged = false;
		data->particle_life_time = 1.0f;
		data->p_size.push_back(KeyFloat(1, data->p_size[0].f));
		data->p_angle_velocity.push_back(KeyFloat(1, data->p_angle_velocity[0].f));
	}

	char _cb[100];
	sprintf(_cb, "emitter %d", _unkn++);

	if(name.IsEmpty()){
		if(	cls==EMC_LIGHT){
			data_light->name = _cb;
		}else{
			data->name = _cb;
		}
	}else{
		if(	cls==EMC_LIGHT){
			data_light->name = name;
		}else{
			data->name = name;
		}
	}
/*
	if(data)
		data->texture_name = DEFAULT_TXT_FIRE;
	else
		data_light->texture_name = DEFAULT_TXT_LIGHT;
*/
}

EMITTER_CLASS CEmitterData::Class()
{
	if(data)
		return data->GetType();
	else if (data_light)
		return data_light->GetType();
	return EMC_BAD;
}
EmitterKeyInt* CEmitterData::GetInt()
{
	return dynamic_cast<EmitterKeyInt*>(data);
}
EmitterKeySpl* CEmitterData::GetSpl()
{
	return dynamic_cast<EmitterKeySpl*>(data);
}
EmitterKeyZ* CEmitterData::GetZ()
{
	return dynamic_cast<EmitterKeyZ*>(data);
}
EmitterKeyLight* CEmitterData::GetLight()
{
	return dynamic_cast<EmitterKeyLight*>(data_light);
}

float CEmitterData::Effect2EmitterTime(float fEffectTime)
{
	float f = 0;
	if(data){
		if(fEffectTime < data->emitter_create_time)
			f = 0;
		else if(fEffectTime > data->emitter_life_time + data->emitter_create_time)
			f = 1.0f;
		else
			f = (fEffectTime - data->emitter_create_time)/data->emitter_life_time;
	}else{
		if(fEffectTime < data_light->emitter_create_time)
			f = 0;
		else if(fEffectTime > data_light->emitter_life_time + data_light->emitter_create_time)
			f = 1.0f;
		else
			f = (fEffectTime - data_light->emitter_create_time)/data_light->emitter_life_time;
	}

	return f;
}

float CEmitterData::GenerationLifeTime(int nGeneration)
{
	if(data)
		return data->life_time[nGeneration].f;
	else
		return data_light->emitter_life_time;
}
float CEmitterData::GenerationPointTime(int nGenPoint)
{
	if(data)
		return data->num_particle[nGenPoint].time;
	else
		return data_light->emitter_position[nGenPoint].time;
}
float CEmitterData::GenerationPointGlobalTime(int nGenPoint)
{
	if(data)
		return GenerationPointTime(nGenPoint)*data->emitter_life_time + data->emitter_create_time;
	else
		return data_light->emitter_life_time;
}
float CEmitterData::ParticleLifeTime()
{
	float f, fMax = 0;

	if(Class() == EMC_LIGHT){
		return data_light->emitter_life_time;
	}else{
		int sz = data->num_particle.size();
		for(int i=0; i<sz; i++)
		{
			f = data->life_time[i].f;

			//float f1, f2, f3;
			//data->GetParticleLifeTime(data->num_particle[i].time, f1, f2, f3);

			f += /*it->x*/data->num_particle[i].time*data->emitter_life_time;

			if(fMax < f)
				fMax = f;
		}
	}

	return fMax;
}

////////////////////////////////
void ResizeKey(int nPoints, CKey& key)
{
	float val = key[0].f;

	key.resize(nPoints);
	if(nPoints > 1)
	{
		key[nPoints-1].time = 1; key[nPoints-1].f = val;

		for(int i=1; i<nPoints-1; i++)
		{
			key[i].time = float(i)/(nPoints-1);
			key[i].f = val;
		}
	}
}
/*void ResizePosRotationKey(const CKey& key_ref, CKeyPos& pos, CKeyRotate& rot)
{
	pos.clear(); rot.clear();
	int n = key_ref.size();
	for(int i=0; i<n; i++)
	{
		pos.push_back(KeyPos());
		KeyPos& p = pos.back();
		p.pos.set(0, 0, 0); p.time = key_ref[i].time;

		rot.push_back(KeyRotate());
		KeyRotate& r = rot.back();
		r.pos = QuatF::ID; r.time = key_ref[i].time;
	}
}
*/

template <class T>
void SetPosRotationKeyTime(int nPoint, float tm, float tm_old, T& key)
{
	T::value_type* pp = 0;
	T::iterator i;
	FOR_EACH(key, i)
		if(xm::fabs(i->time - tm_old) < 0.001f)
		{
			pp = &*i;
			break;
		}

	if(pp)
		pp->time = tm;
}

template <class T>
void RemovePosRotationKey(float tm, T& key)
{
	T::iterator i;
	FOR_EACH(key, i)
		if(xm::fabs(i->time - tm) < 0.001f)
		{
			key.erase(i);
			break;
		}
}

template <class T>
void ClearPosRotationKey(T& key)
{
	if(key.size() > 1)
		key.erase(key.begin() + 1, key.end());
}

template <class T>
void InsertKey(int nBefore, T& key)
{
	T::iterator i;
	FOR_EACH(key, i)
	{
		if(nBefore == 0)
		{
			key.insert(i, *i);
			break;
		}
		nBefore--;
	}
}

template <class T>
void RemoveKey(int nPoint, T& key)
{
	ASSERT(nPoint < key.size());

	key.erase(key.begin() + nPoint);
}

template <class T>
void SetKeyTime(int nKey, float tm, T& key)
{
	if(nKey<key.size())
		key[nKey].time = tm;
}

template <class T>
void SetKeyTimes(T& key, vector<float>& vTimes, float tm)
{
	int nPoints = key.size() - 1;
	for(int i=0; i<nPoints; i++)
	{
		key[i].time = vTimes[i]/tm;

		if(key[i].time > 1.0f)
			key[i].time = 1.0f;
	}
}

template <class T>
void SetKeyTimesPosRot(T& key, float tm_old, float tm)
{
	T::iterator i;
	FOR_EACH(key, i)
	{
		float t = i->time*tm_old/tm;
		if(t > 1.0f)
			t = 1.0f;

		i->time = t;
	}
}

////////////////////////////////////////////////

void CEmitterData::SetGenerationPointCount(int nPoints)
{
	if(data)
	{
		ResizeKey(nPoints, data->num_particle);
		ResizeKey(nPoints, data->life_time);
		ResizeKey(nPoints, data->life_time_delta);
		ResizeKey(nPoints, data->begin_size);
		ResizeKey(nPoints, data->begin_size_delta);

		if(Class() == EMC_INTEGRAL || Class() == EMC_INTEGRAL_Z)
			ResizeKey(nPoints, GetInt()->velocity_delta);
/*
		ClearPosRotationKey(data->emitter_position);
		ClearPosRotationKey(data->emitter_rotation);
	/// 
		for(int i=0;i<nPoints;++i)
		{
			float fTime = GenerationPointGlobalTime(i);
			bool bCreate;
			data->GetOrCreatePosKey(fTime, &bCreate);
		}
*/	/// 
	}
//	else
	{
			emitter_pos().resize(nPoints);
			float t=0;
			float dt = 1.0/(nPoints-1);
			CKeyPos::iterator it;
			FOR_EACH(emitter_pos(), it)
			{
				it->time  = t;
				it->pos.set(0,0,0);
				t+=dt;
			}
/*		ClearPosRotationKey(data_light->emitter_position);
		for(int i=0;i<nPoints;++i)
		{
			data_light->emitter_position.resize(nPoints);
			float fTime = GenerationPointGlobalTime(i);
			bool create;
			data_light->emitter_position.GetOrCreateKey(fTime,data_light->emitter_life_time,data_light->emitter_create_time,&create);		
		}*/
	}
	bDirty = true;
}
#define LINER(p1,p2,k) ((p1) + ((p2)-(p1))*(k))
/*void Liner(CKey& key,int ix,float k)
{
	ASSERT(ix>0);
	ASSERT(ix<key.size());
	key[ix].f = key[ix-1].f + (key[ix+1].f - key[ix-1].f)*k;
}
*/
void CEmitterData::InsertGenerationPoint(int nBefore, float tg)
{
	if(data)
	{
		InsertKey(nBefore, data->num_particle);
		InsertKey(nBefore, data->life_time);
		InsertKey(nBefore, data->life_time_delta);
		InsertKey(nBefore, data->begin_size);
		InsertKey(nBefore, data->begin_size_delta);

		data->num_particle[nBefore].f = LINER(data->num_particle[nBefore-1].f,data->num_particle[nBefore+1].f,tg);
		data->life_time[nBefore].f = LINER(data->life_time[nBefore-1].f,data->life_time[nBefore+1].f,tg);
		data->life_time_delta[nBefore].f = LINER(data->life_time_delta[nBefore-1].f,data->life_time_delta[nBefore+1].f,tg);
		data->begin_size[nBefore].f = LINER(data->begin_size[nBefore-1].f,data->begin_size[nBefore+1].f,tg);
		data->begin_size_delta[nBefore].f = LINER(data->begin_size_delta[nBefore-1].f,data->begin_size_delta[nBefore+1].f,tg);

		if(Class() == EMC_INTEGRAL || Class() == EMC_INTEGRAL_Z)
		{
			InsertKey(nBefore, GetInt()->velocity_delta);
			GetInt()->velocity_delta[nBefore].f = LINER(GetInt()->velocity_delta[nBefore-1].f,GetInt()->velocity_delta[nBefore+1].f,tg);
		}

		data->emitter_position.insert(data->emitter_position.begin()+nBefore,data->emitter_position[nBefore]);
		if(nBefore)
			data->emitter_position[nBefore].pos = (data->emitter_position[nBefore-1].pos + 
													data->emitter_position[nBefore+1].pos)/2;
		else
			data->emitter_position[nBefore].pos = Vect3f(0,0,0);
	}else
	{
		data_light->emitter_position.insert(data_light->emitter_position.begin()+nBefore,data_light->emitter_position[nBefore]);
		if(nBefore)
			data_light->emitter_position[nBefore].pos = (data_light->emitter_position[nBefore-1].pos + 
													data_light->emitter_position[nBefore+1].pos)/2;
		else
			data_light->emitter_position[nBefore].pos = Vect3f(0,0,0);
	}
	bDirty = true;
}
void CEmitterData::SetGenerationPointTime(int nPoint, float tm)
{
//		float  tm_old = GenerationPointTime(nPoint);
//		SetPosRotationKeyTime(nPoint, tm, tm_old, emitter_pos());
	ASSERT((UINT)nPoint<emitter_pos().size());
	emitter_pos()[nPoint].time = tm;
		//SetPosRotationKeyTime(nPoint, tm, tm_old, rotation);
	if(data)
	{
		SetKeyTime(nPoint, tm, data->num_particle);
		SetKeyTime(nPoint, tm, data->life_time);
		SetKeyTime(nPoint, tm, data->life_time_delta);
		SetKeyTime(nPoint, tm, data->begin_size);
		SetKeyTime(nPoint, tm, data->begin_size_delta);

		if(Class() == EMC_INTEGRAL || Class() == EMC_INTEGRAL_Z)
			SetKeyTime(nPoint, tm, GetInt()->velocity_delta);
	}
	bDirty = true;
}

/// Calc spline point between two points
void CalcSPPos(CKeyPosHermit& p_pos, int before)
{
	float dt = (p_pos[before+1].time - p_pos[before-1].time);
	float dt1 = (p_pos[before].time - p_pos[before-1].time);
	float k = dt1/dt;

	float dx = (p_pos[before+1].pos.x - p_pos[before-1].pos.x);
	float dy = (p_pos[before+1].pos.y - p_pos[before-1].pos.y);
	float dz = (p_pos[before+1].pos.z - p_pos[before-1].pos.z);

	p_pos[before].pos.x = p_pos[before-1].pos.x + .5*dx;
	p_pos[before].pos.y = p_pos[before-1].pos.y + .5*dy;
	p_pos[before].pos.z = p_pos[before-1].pos.z + .5*dz;
}
///
/// Calc light positio between two positions
void CalcLightPos(CKeyPos& p_pos, int before)
{
	float dt = (p_pos[before+1].time - p_pos[before-1].time);
	float dt1 = (p_pos[before].time - p_pos[before-1].time);
	float k = dt1/dt;

	float dx = (p_pos[before+1].pos.x - p_pos[before-1].pos.x);
	float dy = (p_pos[before+1].pos.y - p_pos[before-1].pos.y);
	float dz = (p_pos[before+1].pos.z - p_pos[before-1].pos.z);

	p_pos[before].pos.x = p_pos[before-1].pos.x + .5*dx;
	p_pos[before].pos.y = p_pos[before-1].pos.y + .5*dy;
	p_pos[before].pos.z = p_pos[before-1].pos.z + .5*dz;
}
///

void CEmitterData::InsertParticleKey(int nBefore,float tg)
{
	switch(Class())
	{
	case EMC_SPLINE:
		InsertKey(nBefore, GetSpl()->p_position);
		CalcSPPos(GetSpl()->p_position, nBefore);
		break;

	case EMC_INTEGRAL:
	case EMC_INTEGRAL_Z:
		InsertKey(nBefore, GetInt()->p_velocity);
		InsertKey(nBefore, GetInt()->p_gravity);
		InsertKey(nBefore, GetInt()->begin_speed);

		GetInt()->p_velocity[nBefore].f = LINER(GetInt()->p_velocity[nBefore-1].f,GetInt()->p_velocity[nBefore+1].f,tg);
		GetInt()->p_gravity[nBefore].f = LINER(GetInt()->p_gravity[nBefore-1].f,GetInt()->p_gravity[nBefore+1].f,tg);
		GetInt()->begin_speed[nBefore].mul = LINER(GetInt()->begin_speed[nBefore-1].mul,GetInt()->begin_speed[nBefore+1].mul,tg);
		GetInt()->begin_speed[nBefore].velocity = EMV_INVARIABLY;
		break;

	case EMC_LIGHT:
//		InsertKey(nBefore, GetLight()->emitter_position);
//		CalcLightPos(GetLight()->emitter_position, nBefore);

		InsertKey(nBefore, GetLight()->emitter_size);
		GetLight()->emitter_size[nBefore].f = LINER(GetLight()->emitter_size[nBefore-1].f,GetLight()->emitter_size[nBefore+1].f,tg);

//		InsertKey(nBefore, GetLight()->emitter_color);
		break;
	}
	if(Class() != EMC_LIGHT){
		InsertKey(nBefore, data->p_size);
		InsertKey(nBefore, data->p_angle_velocity);

		data->p_size[nBefore].f = LINER(data->p_size[nBefore-1].f,data->p_size[nBefore+1].f,tg);
		data->p_angle_velocity[nBefore].f = LINER(data->p_angle_velocity[nBefore-1].f,data->p_angle_velocity[nBefore+1].f,tg);

	}

/*
	InsertKey(nBefore, data->p_size);
	InsertKey(nBefore, data->p_angle_velocity);
//	InsertKey(nBefore, key_dummy);

	if(Class() == EMC_INTEGRAL || Class() == EMC_INTEGRAL_Z)
	{
		InsertKey(nBefore, GetInt()->p_velocity);
		InsertKey(nBefore, GetInt()->p_gravity);
		InsertKey(nBefore, GetInt()->begin_speed);
	}else{// Spline Emitter
		InsertKey(nBefore, GetSpl()->p_position);
		CalcSPPos(GetSpl()->p_position, nBefore);
	}
*/
	bDirty = true;
}
void CEmitterData::SetParticleKeyTime(int nKey, float tm)
{
	switch(Class())
	{
	case EMC_SPLINE:
		SetKeyTime(nKey, tm, GetSpl()->p_position);
		break;

	case EMC_INTEGRAL:
	case EMC_INTEGRAL_Z:
		SetKeyTime(nKey, tm, GetInt()->p_velocity);
		SetKeyTime(nKey, tm, GetInt()->p_gravity);
		SetKeyTime(nKey, tm, GetInt()->begin_speed);
		break;

	case EMC_LIGHT:
//		SetKeyTime(nKey, tm, GetLight()->emitter_position);
		SetKeyTime(nKey, tm, GetLight()->emitter_size);
//		SetKeyTime(nKey, tm, GetLight()->emitter_color);
		break;
	}

	if(Class()!=EMC_LIGHT){
		SetKeyTime(nKey, tm, data->p_size);
		SetKeyTime(nKey, tm, data->p_angle_velocity);
	}
/*
	SetKeyTime(nKey, tm, data->p_size);
	SetKeyTime(nKey, tm, data->p_angle_velocity);
//	SetKeyTime(nKey, tm, key_dummy);

	if(Class() == EMC_INTEGRAL || Class() == EMC_INTEGRAL_Z)
	{
		SetKeyTime(nKey, tm, GetInt()->p_velocity);
		SetKeyTime(nKey, tm, GetInt()->p_gravity);
		SetKeyTime(nKey, tm, GetInt()->begin_speed);
	}else{// Spline Emitter
		SetKeyTime(nKey, tm, GetSpl()->p_position);
	}
*/
	bDirty = true;
}
void CEmitterData::DeleteGenerationPoint(int nPoint)
{
	if (data)
	{
		ASSERT(nPoint<data->emitter_position.size());
		data->emitter_position.erase(data->emitter_position.begin()+nPoint);
	//	float  tm_old = GenerationPointTime(nPoint);

	//	RemovePosRotationKey(tm_old, data->emitter_position);
		//RemovePosRotationKey(tm_old, rotation);

		RemoveKey(nPoint, data->num_particle);
		RemoveKey(nPoint, data->life_time);
		RemoveKey(nPoint, data->life_time_delta);
		RemoveKey(nPoint, data->begin_size);
		RemoveKey(nPoint, data->begin_size_delta);

		if(Class() == EMC_INTEGRAL || Class() == EMC_INTEGRAL_Z)
			RemoveKey(nPoint, GetInt()->velocity_delta);
	}
	else
	{
		ASSERT(nPoint<data_light->emitter_position.size());
		data_light->emitter_position.erase(data_light->emitter_position.begin()+nPoint);
	}
	bDirty = true;
}
void CEmitterData::DeleteParticleKey(int nPoint)
{
	switch(Class())
	{
	case EMC_SPLINE:
		RemoveKey(nPoint, GetSpl()->p_position);
		break;

	case EMC_INTEGRAL:
	case EMC_INTEGRAL_Z:
		RemoveKey(nPoint, GetInt()->p_velocity);
		RemoveKey(nPoint, GetInt()->p_gravity);
		RemoveKey(nPoint, GetInt()->begin_speed);
		break;

	case EMC_LIGHT:
//		RemoveKey(nPoint, GetLight()->emitter_position);
		RemoveKey(nPoint, GetLight()->emitter_size);
//		RemoveKey(nPoint, GetLight()->emitter_color);
		break;
	}
	if(Class() != EMC_LIGHT){
		RemoveKey(nPoint, data->p_size);
		RemoveKey(nPoint, data->p_angle_velocity);
	}
/*
	RemoveKey(nPoint, data->p_size);
	RemoveKey(nPoint, data->p_angle_velocity);
//	RemoveKey(nPoint, key_dummy);

	if(Class() == EMC_INTEGRAL || Class() == EMC_INTEGRAL_Z)
	{
		RemoveKey(nPoint, GetInt()->p_velocity);
		RemoveKey(nPoint, GetInt()->p_gravity);
		RemoveKey(nPoint, GetInt()->begin_speed);
	}else{
		RemoveKey(nPoint, GetSpl()->p_position);
	}
*/
	bDirty = true;
}
void CEmitterData::ChangeLifetime(float tm)
{
/*	CKeyPos& keypos = (data) ? data->emitter_position : 
							GetLight()->emitter_position;
	int nPoints = keypos.size();
	vector<float> vTimes;
	for(int i=0; i<nPoints; i++)
	{
		float f = keypos[i].time*EmitterLifeTime();
		if((f >=tm)&&(i<nPoints-1)){
			DeleteGenerationPoint(i);//f = tm;
			nPoints--;
			i--;
		}else
			vTimes.push_back(f);
	}

	switch(Class())
	{
	case EMC_SPLINE:
		break;

	case EMC_INTEGRAL:
	case EMC_INTEGRAL_Z:
		break;

	case EMC_LIGHT:
		SetKeyTimesPosRot(GetLight()->emitter_position, GetLight()->emitter_life_time, tm);
		SetKeyTimesPosRot(GetLight()->emitter_size, GetLight()->emitter_life_time, tm);
		SetKeyTimesPosRot(GetLight()->emitter_color, GetLight()->emitter_life_time, tm);
////		SetKeyTimes(GetLight()->emitter_position, vTimes, tm);
//		SetKeyTimes(GetLight()->emitter_size, vTimes, tm);
//		SetKeyTimes(GetLight()->emitter_color, vTimes, tm);
		break;
	}

	if(Class() != EMC_LIGHT){
		SetKeyTimesPosRot(data->emitter_position, data->emitter_life_time, tm);
		SetKeyTimesPosRot(data->emitter_rotation, data->emitter_life_time, tm);
	}
tt*/
	if(data)
		data->emitter_life_time = tm;
	else
		data_light->emitter_life_time = tm;
/* tt
	if(Class() != EMC_LIGHT){
		SetKeyTimes(data->num_particle, vTimes, tm);
		SetKeyTimes(data->life_time, vTimes, tm);
		SetKeyTimes(data->life_time_delta, vTimes, tm);
		SetKeyTimes(data->begin_size, vTimes, tm);
		SetKeyTimes(data->begin_size_delta, vTimes, tm);
	}

	if(Class() == EMC_INTEGRAL || Class() == EMC_INTEGRAL_Z)
		SetKeyTimes(GetInt()->velocity_delta, vTimes, tm);
*/
	bDirty = true;
}
void CEmitterData::ChangeGenerationLifetime(int nGeneration, float tm)
{
	ASSERT(data);
	ASSERT(nGeneration<data->life_time.size());
		data->life_time[nGeneration].f = tm;
/*	vector<float> vTimes;
	float fTime;

	if(data){
		fTime = data->life_time[nGeneration].f;

		int n = data->p_size.size();
		for(int i=0; i<n; i++)
		{
			float f = data->p_size[i].time*fTime;
			if(f > tm)
				f = tm;

			vTimes.push_back(f);
		}
		
		data->life_time[nGeneration].f = tm;

	}else{
		fTime = data_light->emitter_life_time;

		int n = data_light->emitter_size.size();
		for(int i=0; i<n; i++)
		{
			float f = GetLight()->emitter_size[i].time*fTime;
			if(f > tm)
				f = tm;

			vTimes.push_back(f);
		}

		data_light->emitter_life_time = tm;
	}

	switch(Class())
	{
	case EMC_SPLINE:
		SetKeyTimes(GetSpl()->p_position, vTimes, tm);
		break;

	case EMC_INTEGRAL:
	case EMC_INTEGRAL_Z:
		SetKeyTimes(GetInt()->p_velocity, vTimes, tm);
		SetKeyTimes(GetInt()->p_gravity, vTimes, tm);
		SetKeyTimes(GetInt()->begin_speed, vTimes, tm);
		break;

	case EMC_LIGHT:
//		SetKeyTimes(GetLight()->emitter_position, vTimes, tm);
		SetKeyTimes(GetLight()->emitter_size, vTimes, tm);
		SetKeyTimes(GetLight()->emitter_color, vTimes, tm);
		break;
	}

	if(Class() != EMC_LIGHT){
		SetKeyTimes(data->p_size, vTimes, tm);
		SetKeyTimes(data->p_angle_velocity, vTimes, tm);
	}

/*
	SetKeyTimes(data->p_size, vTimes, tm);
	SetKeyTimes(data->p_angle_velocity, vTimes, tm);
//	SetKeyTimes(key_dummy, vTimes, tm);

	if(Class() == EMC_INTEGRAL || Class() == EMC_INTEGRAL_Z)
	{
		SetKeyTimes(GetInt()->p_velocity, vTimes, tm);
		SetKeyTimes(GetInt()->p_gravity, vTimes, tm);
		SetKeyTimes(GetInt()->begin_speed, vTimes, tm);
	}else{
		SetKeyTimes(GetSpl()->p_position, vTimes, tm);
	}
*/
	bDirty = true;
}
void CEmitterData::DeletePositionKey(KeyPos* key)
{
	if(data){
		if(data->emitter_position.size() == 1)
			return;

		CKeyPos::iterator i;
		FOR_EACH(data->emitter_position, i)
			if(&*i == key)
			{
				data->emitter_position.erase(i);
				break;
			}
	}else{
		if(data_light->emitter_position.size() == 1)
			return;

		CKeyPos::iterator i;
		FOR_EACH(data_light->emitter_position, i)
			if(&*i == key)
			{
				data_light->emitter_position.erase(i);
				break;
			}
	}

	bDirty = true;
}

void CEmitterData::GetBeginSpeedMatrix(int nGenPoint, int nPoint, MatXf& mat)
{
	if(Class() == EMC_INTEGRAL || Class() == EMC_INTEGRAL_Z)
	{
		float tm = data->num_particle[nGenPoint].time;

		data->GetPosition(tm, mat);
		mat.rot() = GetInt()->begin_speed[nPoint].rotation;
	}

/*	BSKey::iterator i1 = begin_speed.begin();
	BSKey::iterator i2 = i1;
	while((*i2)->time_begin < tm)
	{
		i1 = i2; i2++;
	}
	ASSERT(i1 != begin_speed.end() && i2 != begin_speed.end());

	QuatF q;
	q.slerp((*i1)->rotation, (*i2)->rotation, (tm - (*i1)->time_begin)/((*i2)->time_begin - (*i1)->time_begin));
	pos.rot() = q;
*/
}

void CEmitterData::Save(CSaver &sv)
{
	if(data)
		data->Save(sv);
	else
		data_light->Save(sv);
}

extern void RemoveSplinePoint(CKeyPosHermit& spline, KeyPos *p);

void CEmitterData::Load(CLoadData* rd)
{
	if(data)
		data->Load(rd);
	else
		data_light->Load(rd);

	if(Class() == EMC_SPLINE){ // delete the rest points
//		CKeyPos::iterator i;
		for(int i=0; i<GetSpl()->p_position.size(); i++)
		{
			if(i>(GetSpl()->p_size.size()-1)){
				RemoveSplinePoint(GetSpl()->p_position, &GetSpl()->p_position[i]);
				i--;
			}
		}
	}

	//
}

///////////////////////////////////////////////////////////////////////////

CEffectData::CEffectData()
{
	static int _unkn = 1;

	char _cb[100];
	sprintf(_cb, "effect %d", _unkn++);

	name = _cb;

	bDirty = true;

	Show3Dmodel = true;
	
	bExpand = true;
}
CEffectData::CEffectData(EffectKey* pk)
{
	name = pk->name;

	vector<EmitterKeyInterface*>::iterator i;
	FOR_EACH(pk->key, i)
		emitters.push_back(new CEmitterData((EmitterKeyInterface*)*i));

	bDirty = true;

	Show3Dmodel = true;

	bExpand = true;
}
CEffectData::~CEffectData()
{
	key.clear();
}

CEmitterData* CEffectData::add_emitter(EMITTER_CLASS cls)
{
	CEmitterData* pEmitter = new CEmitterData(cls);

	emitters.push_back(pEmitter);

	bDirty = true;

	return pEmitter;
}

void CEffectData::del_emitter(CEmitterData* p)
{
	EmitterListType::iterator it = find(emitters.begin(), emitters.end(), p);
	ASSERT(it != emitters.end());

	bDirty = true;

	delete *it;
	emitters.erase(it);
}

void CEffectData::swap_emitters(CEmitterData* p1, CEmitterData* p2)
{
	EmitterListType::iterator it1 = find(emitters.begin(), emitters.end(), p1);
	EmitterListType::iterator it2 = find(emitters.begin(), emitters.end(), p2);
	if((it1!=emitters.end())&&(it2!=emitters.end()))
		swap(*it1, *it2);
	bDirty = true;
}

void CEffectData::prepare_effect_data(CEmitterData *pActiveEmitter)
{
	key.clear();

	EmitterListType::iterator it;
	FOR_EACH(emitters, it)
	{
		if(pActiveEmitter)
		{
			if(*it == pActiveEmitter)
			{
				if(pActiveEmitter&&pActiveEmitter->Class()==EMC_LIGHT){
					key.push_back((*it)->data_light);
					(*it)->data_light->BuildKey();
				}else{
					key.push_back((*it)->data);
					(*it)->data->BuildKey();
				}
			}
		}
		else
		{
			if((*it)->bActive)
			{
				if((*it)->data){
					key.push_back((*it)->data);
					(*it)->data->BuildKey();

				}else{
					key.push_back((*it)->data_light);
					(*it)->data_light->BuildKey();
				}
			}
		}
		
	}
}

float CEffectData::GetTotalLifeTime()
{
	float f = 0;

	EmitterListType::iterator it;
	FOR_EACH(emitters, it)
	{
		CEmitterData* p = *it;

		if(p->EmitterEndTime() > f)
			f = p->EmitterEndTime();
	}

	return f;
}

bool CEffectData::CheckName(LPCTSTR lpsz, CEmitterData* p)
{
	EmitterListType::iterator it;
	FOR_EACH(emitters, it)
		if((*it) != p){
			if((*it)->data){
				if((*it)->data->name == lpsz)
					return false;
			}else{
				if((*it)->data_light->name == lpsz)
					return false;
			}
		}

	return true;
}
bool CEffectData::HasModelEmitter()
{
	EmitterListType::iterator it;
	FOR_EACH(emitters, it)
		if((*it)->data && (*it)->data->particle_position.type == EMP_3DMODEL)
			return true;

	return false;
}
