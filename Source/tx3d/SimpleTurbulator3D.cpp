/**
 *	"Procedural 3D Texture Library"
 *	
 *	Clear implementation of "turbulence".
 *	
 *	@version $Id: SimpleTurbulator3D.cpp,v 1.7 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "SimpleTurbulator3D.hpp"

using namespace tx3d;

std::unique_ptr<SimpleTurbulator3D> SimpleTurbulator3D::sharedInstance;

float SimpleTurbulator3D::turbulate3D(const Vector3D &v, float persistence, int octaveCount, Interpolator3D *interpolator) {
	float sum = 0.0;
	float ampl = 1.0;
	freqV = v;
	for (int i = 0; i < octaveCount; i++) {
		sum += interpolator->interpolate(freqV) * ampl;
		freqV *= 2.0;
		ampl *= persistence;
	}
	return sum;
}

