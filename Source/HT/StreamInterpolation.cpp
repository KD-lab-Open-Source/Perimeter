#include "StdAfx.h"

#include "Umath.h"
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

StreamInterpolator::StreamInterpolator()
{
	MTINIT(lock);
	in_avatar=false;
    stream.automatic_realloc = true;
}

StreamInterpolator::~StreamInterpolator()
{
    ClearData();
	MTDONE(lock);
}

bool StreamInterpolator::set(InterpolateFunction func,cUnknownClass* obj)
{
    MTL();
    if (!obj) {
        xassert(0);
        return false;
    }
    InterpolateHeader data;
    data.data_len = 0;
    data.func = func;
#ifdef STREAM_INTERPOLATOR_USE_HANDLES
    data.handle = obj->AcquireHandle();
    if (!data.handle) {
        xassert(0);
        return false;
    }
#else
    data.obj = obj;
#endif
    xassert(in_avatar);
	xassert(sizeof(func)==sizeof(InterpolateFunction));
    last_header = stream.tell();
    stream.write(data);
    headers_count += 1;
	return true;
}

void StreamInterpolator::ClearData()
{
    Lock();

    if (headers_count) {
        size_t size = stream.tell();
        stream.set(0);
        size_t headers = 0;
        while (stream.tell() < size || headers < headers_count) {
            headers += 1;
            InterpolateHeader data;
            stream.read(data);
            stream.set(data.data_len, XB_CUR);
#ifdef STREAM_INTERPOLATOR_USE_HANDLES
            if (data.handle) {
                data.handle->Release();
                data.handle = nullptr;
            }
#else
            data.obj = nullptr;
#endif
        }
        xassert(headers == headers_count);
        xassert(stream.tell() == size);
        stream.set(0);
        headers_count = 0;
        last_header = 0;
    }

    Unlock();
}

void StreamInterpolator::ProcessData()
{
    MTG();
	Lock();
	timer=HTManager::instance()->interpolationFactor();
	timer_=1-timer;
    
    if (headers_count) {
        size_t size = stream.tell();
        stream.set(0);
        size_t headers = 0;
        while (stream.tell() < size || headers < headers_count) {
            headers += 1;
            InterpolateHeader data;
            stream.read(data);
            xassert(data.func);
            size_t pos = stream.tell();
#ifdef STREAM_INTERPOLATOR_USE_HANDLES
            xassert(data.handle);
            //Get object unless it was Release'd
            cUnknownClass* obj = data.handle ? data.handle->Get() : nullptr;
#else
            cUnknownClass* obj = data.obj;
#endif
            if (obj) {
                data.func(obj, &stream);
            } else {
                //Skip this data
                stream.set(data.data_len, XB_CUR);
            }
            xassert(pos + data.data_len == stream.tell());
        }
        xassert(headers == headers_count);
        xassert(stream.tell() == size);
    }

	Unlock();
}

/////////////////////////////////////////////////////////////
void fSpriteInterpolation(cUnknownClass* cur, XBuffer* data)
{
    Vect3f p[2];
    data->read(p);
	Vect3f out;
	out.x=p[0].x*timer_+p[1].x*timer;
	out.y=p[0].y*timer_+p[1].y*timer;
	out.z=p[0].z*timer_+p[1].z*timer;

    reinterpret_cast<cSprite*>(cur)->SetPos(out);
}

void fLineInterpolation(cUnknownClass* cur, XBuffer* data)
{
    Vect3f p[2];
    Vect3f out[2];
    
    data->read(p);

	out[0].x=p[0].x*timer_+p[1].x*timer;
	out[0].y=p[0].y*timer_+p[1].y*timer;
	out[0].z=p[0].z*timer_+p[1].z*timer;

    data->read(p);

	out[1].x=p[0].x*timer_+p[1].x*timer;
	out[1].y=p[0].y*timer_+p[1].y*timer;
	out[1].z=p[0].z*timer_+p[1].z*timer;

	reinterpret_cast<cLine3d*>(cur)->UpdateVertexPos(2, out);
}

void fParticleRateInterpolation(cUnknownClass* cur, XBuffer* data)
{
    float f[2];
    data->read(f);
	float rate=f[0]*timer_+f[1]*timer;
    reinterpret_cast<cEffect*>(cur)->SetParticleRate(rate);
}

void fSe3fInterpolation(cUnknownClass* cur, XBuffer* data)
{
	Se3f p[2];
    data->read(p);
	Se3f s;
	s.trans().interpolate(p[0].trans(), p[1].trans(), timer);
	s.rot().slerp(p[0].rot(), p[1].rot(), timer);
	MatXf m(s);
    reinterpret_cast<cIUnkClass*>(cur)->SetPosition(m);
}

void fPhaseInterpolation(cUnknownClass* cur, XBuffer* data)
{
    float p[2];
    data->read(p);
    int32_t recursive;
    data->read(recursive);
	static float eps1=1+FLT_EPS;
	float phase = cycle(p[0] + getDist(p[1], p[0], eps1)*timer, eps1);
    reinterpret_cast<cObjectNode*>(cur)->SetPhase(phase,recursive?true:false);
}

void fAngleInterpolation(cUnknownClass* cur, XBuffer* data)
{
    float p[2];
    data->read(p);
    uint8_t a;
    data->read(a);
	eAxis axis=static_cast<eAxis>(a);

//	float angle=p[0]*timer_+p[1]*timer;
	static double XM_PI2=2.0f*XM_PI;
	float angle=cycle(p[0] + getDist(p[1], p[0], XM_PI2)*timer, XM_PI2);
    Mat3f m(angle,axis);
    reinterpret_cast<cObjectNode*>(cur)->SetRotate(&m);
}

void fFloatInterpolation(cUnknownClass* cur, XBuffer* data)
{
	//?
}

void fColorInterpolation(cUnknownClass* cur, XBuffer* data)
{
	sColorInterpolate p[2];
    data->read(p);
	sColor4f color,add_color;
	color.interpolate(p[0].color,p[1].color,timer);
	add_color.interpolate(p[0].add_color,p[1].add_color,timer);
	reinterpret_cast<cObjectNode*>(cur)->SetColor(&add_color,&color,&add_color);
}

void fColorDiffuseInterpolation(cUnknownClass* cur, XBuffer* data)
{
    sColor4f p[2];
    data->read(p);
	sColor4f color;
	color.interpolate(p[0],p[1],timer);
	reinterpret_cast<cObjectNode*>(cur)->SetColor(NULL,&color,NULL);
}
