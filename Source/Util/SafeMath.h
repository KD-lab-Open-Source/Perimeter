#ifndef _PERIMETER_SAFE_MATH_
#define _PERIMETER_SAFE_MATH_

inline float cycle(float f, float size) 
{ 
	return fmod(fmod(f, size) + size, size); 
}

inline float getDist(float v0, float v1, float size) 
{
	float d = fmod(v0 - v1, size);
	float ad = (float)fabs(d);
	float dd = size - ad;
	if(ad <= dd) return d;
	return d < 0 ? d + size : d - size;
}

inline float uncycle(float f1, float f0, float size) 
{	
	return f0 + getDist(f1, f0, size); 
}

inline float getDeltaAngle(float to,float from)
{
	return getDist(to, from, 2*M_PI);
}

inline float cycleAngle(float a)
{
	return cycle(a, 2*M_PI);
}


inline bool isEq(float x, float y)
{ 
	return fabsf(x - y) < FLT_EPS; 
}

inline bool isLess(float x, float y)
{ 
	return x < y + FLT_EPS; 
}

inline bool isGreater(float x, float y)
{ 
	return x > y - FLT_EPS; 
}

#endif

