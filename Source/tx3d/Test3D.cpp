/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Test3D.h,v 1.10 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Test3D.hpp"
#include <cmath>

using namespace tx3d;

const string Test3D::TYPE_NAME = "Test";

Test3D::Test3D(
	float persistence,
	int octaveCount,
	const Vector3D& shift,
	const Vector3D& scale,
	const Vector3D& bgColor,
	const Vector3D& fgColor
) :
	persistence(persistence),
	octaveCount(octaveCount),
	shift(shift),
	scale(scale),
	bgColor(bgColor),
	diffColor(fgColor - bgColor) {}

Test3D::~Test3D() {}


void Test3D::getColor(Vector3D* destClr, const Vector3D &v) {
	shiftedV = v;
	shiftedV &= scale;
	shiftedV += shift;

//	noise = turbulate3D(shiftedV, persistence, octaveCount);
	noise = 0.0;
	float ampl = 1.0;
	float freqs = 1.0;
	for (int i = 0; i < octaveCount; i++) {
		freqV = shiftedV;
		freqV *= freqs;
		noise += interpolate() * ampl;
		freqs *= 2.0;
		ampl *= persistence;
	}

	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}
	*destClr = diffColor; 
//	*destClr *= (( 1.0 + noise ) / 2.0);
	*destClr *= (( 1.0 + cos(3.1415926 * (shiftedV.x + noise)) ) / 2.0);
	*destClr += bgColor;

}

float Test3D::interpolate() {
	xi = (int) freqV.x;
	yi = (int) freqV.y;
	zi = (int) freqV.z;
	xd = freqV.x - xi;
	yd = freqV.y - yi;
	zd = freqV.z - zi;

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
}

float Test3D::cosInterpolate(float h1, float h2, float shift) {
	coss = (1.0 - cos(shift * 3.1415926)) * 0.5;
	return h1 * (1.0 - coss) + h2 * coss;
}

float Test3D::getNoise(int x, int y, int z) {
	int a = x + y * 400 + z * 160000;
	a = (a << 13) ^ a;
	return 1.0 - (float) ((a * (a * a * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
}

float Test3D::interpolate(const Vector3D &v) {
	xi = (int) v.x;
	yi = (int) v.y;
	zi = (int) v.z;
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
}

float Test3D::turbulate3D(const Vector3D &v, float p, int oct) {
	float sum = 0.0;
	float ampl = 1.0;
	float freqs = 1.0;
	for (int i = 0; i < oct; i++) {
		sum += interpolate(v * freqs) * ampl;
		freqs *= 2.0;
		ampl *= p;
	}
	return sum;
}
