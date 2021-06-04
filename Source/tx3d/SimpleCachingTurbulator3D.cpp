/**
 *	"Procedural 3D Texture Library"
 *	
 *	This "turbulator" caches the one sum.
 *  Save computation time when having a several textures in queue.
 *	Precompute and store powers of two.
 *	
 *	@version $Id: SimpleCachingTurbulator3D.cpp,v 1.4 2002/11/05 17:55:26 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "SimpleCachingTurbulator3D.hpp"

using namespace tx3d;

float SimpleCachingTurbulator3D::turbulate3D(const Vector3D &v, float persistence, int octaveCount, Interpolator3D *interpolator) {
	if (
			lastInterpolator == interpolator &&
			lastV == v &&
			lastPersistence == persistence &&
			lastOctaveCount == octaveCount
		) {

		return lastSum;

	} else {
		lastInterpolator = interpolator;
		lastV = v;
		lastPersistence = persistence;
		lastOctaveCount = octaveCount;
		float sum = 0.0;
		float ampl = 1.0;
		for (int i = 0; i < octaveCount; i++) {
			freqV = v;
			freqV *= freqs[i];
			sum += interpolator->interpolate(freqV) * ampl;
			ampl *= persistence;
		}

		lastSum = sum;
		return sum;

	}
}

SimpleCachingTurbulator3D::SimpleCachingTurbulator3D(int maximumOctaveCount) {
	lastSum = lastPersistence = lastOctaveCount = 0;
	lastInterpolator = 0;

	freqs = 0;
	setMaxOctaveCount(maximumOctaveCount);
}

SimpleCachingTurbulator3D::~SimpleCachingTurbulator3D() {
	if (freqs != 0) {
		delete[] freqs;
	}
}

void SimpleCachingTurbulator3D::setMaxOctaveCount(int newMaxOctaveCount) {
	maxOctaveCount = newMaxOctaveCount;
	if (freqs != 0) {
		delete[] freqs;
	}
	freqs = new float[maxOctaveCount];
	freqs[0] = 1.0;
	for (int i = 1; i < maxOctaveCount; i++) {
		freqs[i] = freqs[i - 1] * 2.0;
	}
}
