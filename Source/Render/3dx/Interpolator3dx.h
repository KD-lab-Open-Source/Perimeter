#pragma once

#include "..\..\IGameExporter\3dx.h"
#include "..\saver\saver.h"

template<int template_size>
struct sInerpolate3dx
{
	ITPL type;
	float tbegin;
	float inv_tsize;
	float a[template_size*4];
};

template<int tsize>
class Interpolator3dx
{
public:
	typedef vector<sInerpolate3dx<tsize> > darray;
	darray data;

	void Interpolate(float globalt,float* out,int index);
	int FindIndex(float globalt);
	int FindIndexRelative(float globalt,int cur);
	
	//cur - значение, которое вернул FindIndex или Next.
	// предполагается, что globalt постоянно растёт.
	int Next(float globalt,int cur);

	// предполагается, что globalt постоянно уменьшается.
	int Prev(float globalt,int cur);

	void Load(CLoadIterator ld);
};

inline
void InterpolateX(float t,float* out,sInerpolate3dx<1>& in)
{
	switch(in.type)
	{
	case 0:
		out[0]=in.a[0];
		break;
	case 1:
		out[0]=in.a[0]+in.a[1]*t;
		break;
	case 2:
		{
			float t2=t*t;
			float t3=t2*t;
			out[0]=in.a[0]+in.a[1]*t+in.a[2]*t2+in.a[3]*t3;
		}
		break;
	default:
		xassert(0);
	}
}

inline
void InterpolateX(float t,float* out,sInerpolate3dx<2>& in)
{
	switch(in.type)
	{
	case 0:
		out[0]=in.a[0];
		out[1]=in.a[1];
		break;
	case 1:
		out[0]=in.a[0]+in.a[1]*t;
		out[1]=in.a[2]+in.a[3]*t;
		break;
	case 2:
		{
			float t2=t*t;
			float t3=t2*t;
			out[0]=in.a[0]+in.a[1]*t+in.a[2]*t2+in.a[3]*t3;
			out[1]=in.a[4]+in.a[5]*t+in.a[6]*t2+in.a[7]*t3;
		}
		break;
	default:
		xassert(0);
	}
}

inline
void InterpolateX(float t,float* out,sInerpolate3dx<3>& in)
{
	switch(in.type)
	{
	case 0:
		out[0]=in.a[0];
		out[1]=in.a[1];
		out[2]=in.a[2];
		break;
	case 1:
		out[0]=in.a[0]+in.a[1]*t;
		out[1]=in.a[2]+in.a[3]*t;
		out[2]=in.a[4]+in.a[5]*t;
		break;
	case 2:
		{
			float t2=t*t;
			float t3=t2*t;
			out[0]=in.a[0]+in.a[1]*t+in.a[2]*t2+in.a[3]*t3;
			out[1]=in.a[4]+in.a[5]*t+in.a[6]*t2+in.a[7]*t3;
			out[2]=in.a[8]+in.a[9]*t+in.a[10]*t2+in.a[11]*t3;
		}
		break;
	default:
		xassert(0);
	}
}

inline
void InterpolateX(float t,float* out,sInerpolate3dx<4>& in)
{
	switch(in.type)
	{
	case 0:
		out[0]=in.a[0];
		out[1]=in.a[1];
		out[2]=in.a[2];
		out[3]=in.a[3];
		break;
	case 1:
		out[0]=in.a[0]+in.a[1]*t;
		out[1]=in.a[2]+in.a[3]*t;
		out[2]=in.a[4]+in.a[5]*t;
		out[3]=in.a[6]+in.a[7]*t;
		break;
	case 2:
		{
			float t2=t*t;
			float t3=t2*t;
			out[0]=in.a[0]+in.a[1]*t+in.a[2]*t2+in.a[3]*t3;
			out[1]=in.a[4]+in.a[5]*t+in.a[6]*t2+in.a[7]*t3;
			out[2]=in.a[8]+in.a[9]*t+in.a[10]*t2+in.a[11]*t3;
			out[3]=in.a[12]+in.a[13]*t+in.a[14]*t2+in.a[15]*t3;
		}
		break;
	default:
		xassert(0);
	}
}

template<int tsize>
void Interpolator3dx<tsize>::Interpolate(float globalt,float* out,int index)
{
	xassert(index>=0 && index<data.size());
	sInerpolate3dx<tsize>& in=data[index];
	float localt=(globalt-in.tbegin)*in.inv_tsize-0.5f;
	InterpolateX(localt,out,in);
}

template<int tsize>
int Interpolator3dx<tsize>::FindIndex(float globalt)
{
	//потом возможно заменить на binary find
	return Next(globalt,0);
}

template<int tsize>
int Interpolator3dx<tsize>::FindIndexRelative(float globalt,int cur)
{
	xassert(cur>=0 && cur<data.size());
	sInerpolate3dx<tsize>& p=data[cur];
	if(globalt>=p.tbegin)
		return Next(globalt,cur);
	return Prev(globalt,cur);
}

template<int tsize>
int Interpolator3dx<tsize>::Next(float globalt,int cur)
{
	int size=data.size();
	for(int i=cur;i<size;i++)
	{
		sInerpolate3dx<tsize>& p=data[i];
		float t=(globalt-p.tbegin)*p.inv_tsize;
		if(t>=0 && t<=1)
		{
			return i;
		}
	}

	return size-1;
}


template<int tsize>
int Interpolator3dx<tsize>::Prev(float globalt,int cur)
{
	int size=data.size();
	for(int i=cur;i>=0;i--)
	{
		sInerpolate3dx<tsize>& p=data[i];
		float t=(globalt-p.tbegin)*p.inv_tsize;
		if(t>=0 && t<=1)
		{
			return i;
		}
	}

	return 0;
}

template<int tsize>
void Interpolator3dx<tsize>::Load(CLoadIterator ld)
{
	int size=0;
	ld>>size;
	xassert(size>0 && size<255);
	data.resize(size);
	for(int i=0;i<size;i++)
	{
		sInerpolate3dx<tsize>& one=data[i];
		int type=0;
		ld>>type;
		one.type=(ITPL)type;
		float time_size=0;
		ld>>one.tbegin;
		ld>>time_size;
		xassert(time_size>1e-5f);
		one.inv_tsize=1/time_size;

		int one_size=0;
		switch(type)
		{
		case ITPL_CONSTANT:
			one_size=1;
			break;
		case ITPL_LINEAR:
			one_size=2;
			break;
		case ITPL_SPLINE:
			one_size=4;
			break;
		default:
			assert(0);
		}

		for(int j=0;j<one_size*tsize;j++)
		{
			ld>>one.a[j];
		}

	}

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct sInerpolate3dxBool
{
	float tbegin;
	float inv_tsize;
	bool a;
};

class Interpolator3dxBool
{
public:
	typedef vector<sInerpolate3dxBool > darray;
	darray data;

	inline void Interpolate(float globalt,bool* out,int index);
	inline int FindIndex(float globalt) {return Next(globalt,0);};
	inline int FindIndexRelative(float globalt,int cur);
	
	//cur - значение, которое вернул FindIndex или Next.
	// предполагается, что globalt постоянно растёт.
	inline int Next(float globalt,int cur);

	// предполагается, что globalt постоянно уменьшается.
	inline int Prev(float globalt,int cur);

	inline void Load(CLoadIterator ld);
};


void Interpolator3dxBool::Interpolate(float globalt,bool* out,int index)
{
	xassert(index>=0 && index<data.size());
	sInerpolate3dxBool& in=data[index];
	float localt=(globalt-in.tbegin)*in.inv_tsize-0.5f;
	*out=in.a;
}

int Interpolator3dxBool::FindIndexRelative(float globalt,int cur)
{
	xassert(cur>=0 && cur<data.size());
	sInerpolate3dxBool& p=data[cur];
	if(globalt>=p.tbegin)
		return Next(globalt,cur);
	return Prev(globalt,cur);
}

int Interpolator3dxBool::Next(float globalt,int cur)
{
	int size=data.size();
	for(int i=cur;i<size;i++)
	{
		sInerpolate3dxBool& p=data[i];
		float t=(globalt-p.tbegin)*p.inv_tsize;
		if(t>=0 && t<=1)
		{
			return i;
		}
	}

	return size-1;
}


int Interpolator3dxBool::Prev(float globalt,int cur)
{
	int size=data.size();
	for(int i=cur;i>=0;i--)
	{
		sInerpolate3dxBool& p=data[i];
		float t=(globalt-p.tbegin)*p.inv_tsize;
		if(t>=0 && t<=1)
		{
			return i;
		}
	}

	return 0;
}

void Interpolator3dxBool::Load(CLoadIterator ld)
{
	int size=0;
	ld>>size;
	xassert(size>0 && size<255);
	data.resize(size);
	for(int i=0;i<size;i++)
	{
		sInerpolate3dxBool& one=data[i];
		ld>>one.tbegin;
		float tsize=0;
		ld>>tsize;
		xassert(tsize>1e-5f);
		one.inv_tsize=1/tsize;

		ld>>one.a;

	}

}