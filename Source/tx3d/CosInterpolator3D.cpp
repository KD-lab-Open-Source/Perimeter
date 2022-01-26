/**
 *	"Procedural 3D Texture Library"
 *	
 *  This "interpolator" implements simple cosine approximation.
 *	
 *	@version $Id: CosInterpolator3D.cpp,v 1.5 2002/11/05 17:55:26 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "CosInterpolator3D.hpp"
#include "Texture3DUtils.hpp"

using namespace tx3d;

const string CosInterpolator3D::TYPE_NAME = "Cosine";

float CosInterpolator3D::interpolate(const Vector3D &v) {
	//TODO: optimization needed
	xi = xm::floor(v.x);
	yi = xm::floor(v.y);
	zi = xm::floor(v.z);
	xd = v.x - xi;
	yd = v.y - yi;
	zd = v.z - zi;

	vert1 = getNoise(xi, yi, zi);
	vert2 = getNoise(xi + 1, yi, zi);
	vert3 = getNoise(xi, yi + 1, zi);
	vert4 = getNoise(xi + 1, yi + 1, zi);
	vert5 = getNoise(xi, yi, zi + 1);
	vert6 = getNoise(xi + 1, yi, zi + 1);
	vert7 = getNoise(xi, yi + 1, zi + 1);
	vert8 = getNoise(xi + 1, yi + 1, zi + 1);

	return 
		cosInterpolate(
			cosInterpolate(
				cosInterpolate(vert1, vert2, xd),
				cosInterpolate(vert3, vert4, xd), yd),
			cosInterpolate(
				cosInterpolate(vert5, vert6, xd),
				cosInterpolate(vert7, vert8, xd), yd),
			zd
		);


/*	vert1 = getNoise(xi, yi, zi);
	vert2 = getNoise(xi + 1, yi, zi);
	vert3 = getNoise(xi, yi + 1, zi);
	vert4 = getNoise(xi + 1, yi + 1, zi);
	vert5 = getNoise(xi, yi, zi + 1);
	vert6 = getNoise(xi + 1, yi, zi + 1);
	vert7 = getNoise(xi, yi + 1, zi + 1);
	vert8 = getNoise(xi + 1, yi + 1, zi + 1);

	return 
		cosInterpolate(
			cosInterpolate(
				cosInterpolate(vert1, vert2, xd),
				cosInterpolate(vert3, vert4, xd), yd),
			cosInterpolate(
				cosInterpolate(vert5, vert6, xd),
				cosInterpolate(vert7, vert8, xd), yd),
			zd
		);

/*	vert1 = cosInterpolate(vert1, vert2, xd);
	vert3 = cosInterpolate(vert3, vert4, xd);
	vert5 = cosInterpolate(vert5, vert6, xd);
	vert7 = cosInterpolate(vert7, vert8, xd);

	return 
		cosInterpolate(
			cosInterpolate(
				vert1,
				vert3, yd),
			cosInterpolate(
				vert5,
				vert7, yd),
			zd
		);


/*	return 
		cosInterpolate(
			cosInterpolate(
				cosInterpolate(getNoise(xi, yi, zi), getNoise(xi + 1, yi, zi), xd),
				cosInterpolate(getNoise(xi, yi + 1, zi), getNoise(xi + 1, yi + 1, zi), xd), yd),
			cosInterpolate(
				cosInterpolate(getNoise(xi, yi, zi + 1), getNoise(xi + 1, yi, zi + 1), xd),
				cosInterpolate(getNoise(xi, yi + 1, zi + 1), getNoise(xi + 1, yi + 1, zi + 1), xd), yd),
			zd
		);
*/
}

float CosInterpolator3D::cosInterpolate(float h1, float h2, float shift) {
	coss = (1.0 - xm::cos(shift * 3.1415926)) * 0.5;
	return h1 * (1.0 - coss) + h2 * coss;
}

float CosInterpolator3D::getNoise(int x, int y, int z) {
	int a = x + y * 400 + z * 160000;
	a = (a << 13) ^ a;
	return 1.0 - (float) ((a * (a * a * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
}
