#include "StdAfx.h"

#include "umath.h"
#include "IVisGeneric.h"
#include "VisGenericDefine.h"
#include "IRenderDevice.h"

#include "Runtime.h"
#include "Universe.h"
#include "Player.h"

#include "StreamInterpolation.h"
#include "ht.h"

static float timer;//0..1 - интерполированное время
float timer_;
/*
UpdateVertex - убрать из периметра
*/

StreamInterpolator stream_interpolator;

#define INERROR() xassert(in_avatar);

StreamInterpolator::StreamInterpolator()
{
	MTINIT(lock);
	in_avatar=false;
}

StreamInterpolator::~StreamInterpolator()
{
	MTDONE(lock);
}

StreamInterpolator& StreamInterpolator::set(InterpolateFunction func,cIUnkClass* obj)
{
	INERROR();
	int sz=stream.size();
	xassert(sizeof(func)==4);
	stream.resize(sz+sizeof(func)+sizeof(obj));
	char* pos=&stream[sz];
	(*(InterpolateFunction*)pos)=func;
	pos+=sizeof(func);
	(*(cIUnkClass**)pos)=obj;
	return *this;
}

StreamInterpolator& StreamInterpolator::operator<<(Vect3f f[2])
{
	INERROR();
	int sz=stream.size();
	stream.resize(sz+sizeof(f[0])*2);
	char* pos=&stream[sz];
	((Vect3f*)pos)[0]=f[0];
	((Vect3f*)pos)[1]=f[1];
	return *this;
}

StreamInterpolator& StreamInterpolator::operator<<(Se3f f[2])
{
	INERROR();
	int sz=stream.size();
	stream.resize(sz+sizeof(f[0])*2);
	char* pos=&stream[sz];
	((Se3f*)pos)[0]=f[0];
	((Se3f*)pos)[1]=f[1];
	return *this;
}

StreamInterpolator& StreamInterpolator::operator<<(float f[2])
{
	INERROR();
	int sz=stream.size();
	stream.resize(sz+sizeof(f[0])*2);
	char* pos=&stream[sz];
	((float*)pos)[0]=f[0];
	((float*)pos)[1]=f[1];
	return *this;
}

StreamInterpolator& StreamInterpolator::operator<<(sColor4c f[2])
{
	INERROR();
	int sz=stream.size();
	stream.resize(sz+sizeof(f[0])*2);
	char* pos=&stream[sz];
	((sColor4c*)pos)[0]=f[0];
	((sColor4c*)pos)[1]=f[1];
	return *this;
}

StreamInterpolator& StreamInterpolator::operator<<(sColor4f f[2])
{
	INERROR();
	int sz=stream.size();
	stream.resize(sz+sizeof(f[0])*2);
	char* pos=&stream[sz];
	((sColor4f*)pos)[0]=f[0];
	((sColor4f*)pos)[1]=f[1];
	return *this;
}

StreamInterpolator& StreamInterpolator::operator<<(sColorInterpolate f[2])
{
	INERROR();
	int sz=stream.size();
	stream.resize(sz+sizeof(f[0])*2);
	char* pos=&stream[sz];
	((sColorInterpolate*)pos)[0]=f[0];
	((sColorInterpolate*)pos)[1]=f[1];
	return *this;
}

StreamInterpolator& StreamInterpolator::operator<<(int f)
{
	INERROR();
	int sz=stream.size();
	stream.resize(sz+sizeof(f));
	char* pos=&stream[sz];
	(*(int*)pos)=f;
	return *this;
}

void StreamInterpolator::ClearData()
{
/*	
	Lock();
	timer=HTManager::instance()->interpolationFactor();
	timer_=1-timer;
	char* front=&stream[0];
	char* cur=front;
	int size=stream.size();
	char* end=front+size;
	int sz=0;
	while(cur<end)
	{
		InterpolateFunction func=*(InterpolateFunction*)cur;
		cur+=4;
		cIUnkObj* cur_object=*(cIUnkObj**)cur;
		cur+=4;
		int sz=func(cur_object,cur);
		if(func==fSe3fInterpolation)
		{
			cur_object->ClearAttr(ATTRUNKOBJ_TEMP_USED);
		}

		cur+=sz;
		
	}

	xassert(cur-front==size);
	Unlock();
*/
	stream.clear();
}

void StreamInterpolator::ProcessData()
{
	Lock();
	timer=HTManager::instance()->interpolationFactor();
	timer_=1-timer;
	int size=stream.size();
	if(size)
	{
		char* front=&stream[0];
		char* cur=front;
		char* end=front+size;
		int sz=0;
		while(cur<end)
		{
			InterpolateFunction func=*(InterpolateFunction*)cur;
			cur+=4;
			cIUnkObj* cur_object=*(cIUnkObj**)cur;
			cur+=4;
			int sz=func(cur_object,cur);
			cur+=sz;
			
		}
		xassert(cur-front==size);
	}

	Unlock();
}

/////////////////////////////////////////////////////////////
int fSpriteInterpolation(cIUnkObj* cur,void* data)
{
	Vect3f* p=(Vect3f*)data;
	Vect3f out;
	out.x=p[0].x*timer_+p[1].x*timer;
	out.y=p[0].y*timer_+p[1].y*timer;
	out.z=p[0].z*timer_+p[1].z*timer;

	((cSprite*)cur)->SetPos(out);

	return sizeof(Vect3f)*2;
}

int fLineInterpolation(cIUnkObj* cur,void* data)
{
	Vect3f* p=(Vect3f*)data;

	Vect3f x0 = p[0];
	Vect3f x1 = p[1];

	Vect3f out[2];

	out[0].x=x0.x*timer_+x1.x*timer;
	out[0].y=x0.y*timer_+x1.y*timer;
	out[0].z=x0.z*timer_+x1.z*timer;

	x0 = p[2];
	x1 = p[3];

	out[1].x=x0.x*timer_+x1.x*timer;
	out[1].y=x0.y*timer_+x1.y*timer;
	out[1].z=x0.z*timer_+x1.z*timer;

	((cLine3d*)cur)->UpdateVertexPos(2,out);

	return sizeof(Vect3f)*4;
}

int fParticleRateInterpolation(cIUnkObj* cur,void* data)
{
	float* f=(float*)data;
	cEffect* eff=(cEffect*)cur;
	float rate=f[0]*timer_+f[1]*timer;
	eff->SetParticleRate(rate);
	return sizeof(float)*2;
}

int fSe3fInterpolation(cIUnkObj* cur,void* data)
{
	Se3f* p=(Se3f*)data;
	Se3f s;
	s.trans().interpolate(p[0].trans(), p[1].trans(), timer);
	s.rot().slerp(p[0].rot(), p[1].rot(), timer);
	MatXf m(s);

	cur->SetPosition(m);

	return sizeof(Se3f)*2;
}

int fPhaseInterpolation(cIUnkObj* cur,void* data)
{
	float* p=(float*)data;
	int recursive=*(int*)(p+2);
	cObjectNode* node=(cObjectNode*)cur;
	static float eps1=1+FLT_EPS;
	float phase = cycle(p[0] + getDist(p[1], p[0], eps1)*timer, eps1);
	node->SetPhase(phase,recursive?true:false);
	return sizeof(float)*2+sizeof(int);
}

int fAngleInterpolation(cIUnkObj* cur,void* data)
{
	float* p=(float*)data;
	eAxis axis=(eAxis)*(int*)(p+2);
	cObjectNode* node=(cObjectNode*)cur;

//	float angle=p[0]*timer_+p[1]*timer;
	static float M_PI2=2*M_PI;
	float angle=cycle(p[0] + getDist(p[1], p[0], M_PI2)*timer, M_PI2);
	node->SetRotate(&Mat3f(angle,axis));

	return sizeof(float)*2+sizeof(int);
}

int fFloatInterpolation(cIUnkObj* cur,void* data)
{
	return sizeof(float)*2;
}

int fColorInterpolation(cIUnkObj* cur,void* data)
{
	sColorInterpolate* p=(sColorInterpolate*)data;
	cObjectNode* node=(cObjectNode*)cur;

	sColor4f color,add_color;
	color.interpolate(p[0].color,p[1].color,timer);
	add_color.interpolate(p[0].add_color,p[1].add_color,timer);
	node->SetColor(&add_color,&color,&add_color);

	return sizeof(sColorInterpolate)*2;
}

int fColorDiffuseInterpolation(cIUnkObj* cur,void* data)
{
	sColor4f* p=(sColor4f*)data;
	cObjectNode* node=(cObjectNode*)cur;
	sColor4f color;
	color.interpolate(p[0],p[1],timer);
	node->SetColor(NULL,&color,NULL);

	return sizeof(sColor4f)*2;
}
