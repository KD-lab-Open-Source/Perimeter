/**
 *	"Procedural 3D Texture Library"
 *	
 *  
 *	
 *	@version $Id: CosInterpolator3D.cpp,v 1.5 2002/11/05 17:55:26 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "CubicInterpolator3D.hpp"
#include "Texture3DUtils.hpp"

using namespace tx3d;

const string CubicInterpolator3D ::TYPE_NAME = "Cubic";

const float HALF = 0.5;

float CubicInterpolator3D ::interpolate(const Vector3D &v) {
	long xi;
	long yi;
	long zi;
	float xd;
	float yd;
	float zd;

/*
	xi = v.x;
	yi = v.y;
	zi = v.z;

	xd = v.x - xi;
	yd = v.y - yi;
	zd = v.z - zi;
*/
	// fast implementation of C code above
#ifdef _MSC_VER
    _asm{
		mov		esi, v
		fld		[dword ptr HALF]

		fld		[dword ptr esi]
		fld		st(0)
		fsub	st(0), st(2)
	fistp	[dword ptr xi]
	fild	[dword ptr xi]
		fsubp	st(1), st(0)
		fstp	[dword ptr xd]

		fld		[dword ptr esi + 4]
		fld		st(0)
		fsub	st(0), st(2)
	fistp	[dword ptr yi]
	fild	[dword ptr yi]
		fsubp	st(1), st(0)
		fstp	[dword ptr yd]

		fld		[dword ptr esi + 8]
		fld		st(0)
		fsub	st(0), st(2)
	fistp	[dword ptr zi]
	fild	[dword ptr zi]
		fsubp	st(1), st(0)
		fstp	[dword ptr zd]

		fstp	st(0)
	}
#else
    //TODO no idea if this is correct
    xi = v.x;
    yi = v.y;
    zi = v.z;

    xd = v.x - xi;
    yd = v.y - yi;
    zd = v.z - zi;
#endif

	xi += yi * 400 + zi * 160000;

	xd = xd * xd * (3.0 - 2.0 * xd);
	yd = yd * yd * (3.0 - 2.0 * yd);
	zd = zd * zd * (3.0 - 2.0 * zd);

	float xd1 = 1.0 - xd;
	float yd1 = 1.0 - yd;
	float zd1 = 1.0 - zd;

	return 
		Texture3DUtils::lerp(
			Texture3DUtils::lerp(
				Texture3DUtils::lerp(getNoise(xi), getNoise(xi + 1), xd1, xd),
				Texture3DUtils::lerp(getNoise(xi + 400), getNoise(xi + 401), xd1, xd), yd1, yd),
			Texture3DUtils::lerp(
				Texture3DUtils::lerp(getNoise(xi + 160000), getNoise(xi + 160001), xd1, xd),
				Texture3DUtils::lerp(getNoise(xi + 160400), getNoise(xi + 160401), xd1, xd), yd1, yd),
			zd1, zd
		);
/*
	return 
		lerp(
			lerp(
				lerp(getNoise(xi), getNoise(xi + 1), xd),
				lerp(getNoise(xi + 400), getNoise(xi + 401), xd), yd),
			lerp(
				lerp(getNoise(xi + 160000), getNoise(xi + 160001), xd),
				lerp(getNoise(xi + 160400), getNoise(xi + 160401), xd), yd),
			zd
		);


	return 
		(
			(getNoise(xi) * xd1 + getNoise(xi + 1) * xd)
			* yd1
			+ (getNoise(xi + 400) * xd1 + getNoise(xi + 401) * xd)
			* yd
		)
		* (1.0 - zd)
	  + (
			(getNoise(xi + 160000) * xd1 + getNoise(xi + 160001) * xd)
			* yd1
			+ (getNoise(xi + 160400) * xd1 + getNoise(xi + 160401) * xd)
			* yd
		)
		* zd;
*/
}

/*inline float CubicInterpolator3D ::lerp(float h1, float h2, float shift1, float shift) {
	return h1 * shift1 + h2 * shift;
}
/*
inline float CubicInterpolator3D ::lerp(float h1, float h2, float shift) {
	return h1 * (1.0 - shift) + h2 * shift;
}
*/
inline float CubicInterpolator3D::getNoise(long a) {
	a ^= (a << 13);
	return 1.0 - ((a * (a * a * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
//	return 1.0 - ((a * 1664525L + 1013904223L) & 0x7fffffff) / 1073741824.0;
}
