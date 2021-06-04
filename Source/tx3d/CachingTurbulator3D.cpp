/**
 *	"Procedural 3D Texture Library"
 *	
 *	This "turbulator" caches one sum, all partial sums and interpolations.
 *  Save computation time when having a several textures in queue.
 *	Precompute and store powers of two.
 *	
 *	@version $Id: CachingTurbulator3D.cpp,v 1.6 2002/11/05 17:55:26 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "CachingTurbulator3D.hpp"

using namespace tx3d;

float CachingTurbulator3D::turbulate3D(const Vector3D &v, float persistence, int octaveCount, Interpolator3D *interpolator) {
	if (lastInterpolator == interpolator &&
		lastV == v) {
		if (lastPersistence == persistence) {
			if (lastOctaveCount == octaveCount) {
				return sum;
			} else if (lastOctaveCount < octaveCount) {
				if (octaveCount <= lastInterpolationCount) {
					for (int i = lastOctaveCount; i < octaveCount; i++) {
						sum += partInterpolations[i] * ampl;
						partSums[i] = sum;
						ampl *= persistence;
					}
				} else {
					int i;
					for (i = lastOctaveCount; i < lastInterpolationCount; i++) {
						sum += partInterpolations[i] * ampl;
						partSums[i] = sum;
						ampl *= persistence;
					}
					for (i = lastInterpolationCount; i < octaveCount; i++) {
						freqV = v;
						freqV *= freqs[i];
						partInterpolations[i] = interpolator->interpolate(freqV);
						sum += partInterpolations[i] * ampl;
						partSums[i] = sum;
						ampl *= persistence;
					}
					lastInterpolationCount = octaveCount;
				}
				lastOctaveCount = octaveCount;
				return sum;
			} else {
				return partSums[octaveCount];
			}
		}

		sum = 0.0;
		ampl = 1.0;
		if (lastOctaveCount < octaveCount) {
			if (octaveCount <= lastInterpolationCount) {
				for (int i = 0; i < octaveCount; i++) {
					sum += partInterpolations[i] * ampl;
					partSums[i] = sum;
					ampl *= persistence;
				}
			} else {
				int i;
				for (i = 0; i < lastInterpolationCount; i++) {
					sum += partInterpolations[i] * ampl;
					partSums[i] = sum;
					ampl *= persistence;
				}
				for (i = lastInterpolationCount; i < octaveCount; i++) {
					freqV = v;
					freqV *= freqs[i];
					partInterpolations[i] = interpolator->interpolate(freqV);
					sum += partInterpolations[i] * ampl;
					partSums[i] = sum;
					ampl *= persistence;
				}
			}
		} else {
			for (int i = 0; i < octaveCount; i++) {
				sum += partInterpolations[i] * ampl;
				partSums[i] = sum;
				ampl *= persistence;
			}
		}
		lastOctaveCount = octaveCount;
		lastPersistence = persistence;
		return sum;
	} else {
		lastInterpolator = interpolator;
		lastV = v;
		lastPersistence = persistence;
		lastOctaveCount = octaveCount;
		lastInterpolationCount = octaveCount;
		sum = 0.0;
		ampl = 1.0;
		for (int i = 0; i < octaveCount; i++) {
			freqV = v;
			freqV *= freqs[i];
			partInterpolations[i] = interpolator->interpolate(freqV);
			sum += partInterpolations[i] * ampl;
			partSums[i] = sum;
			ampl *= persistence;
		}
		return sum;
	}
}

CachingTurbulator3D::CachingTurbulator3D(int maximumOctaveCount) {
	lastPersistence = 0.0;
	lastOctaveCount = 0;
	lastInterpolator = 0;

	freqs = 0;
	partSums = 0;
	partInterpolations = 0;
	setMaxOctaveCount(maximumOctaveCount);
	lastInterpolationCount = 0;
}

void CachingTurbulator3D::setMaxOctaveCount(int newMaxOctaveCount) {
	maxOctaveCount = newMaxOctaveCount;

	clearCache();
	freqs = new float[maxOctaveCount];
	partSums = new float[maxOctaveCount];
	partInterpolations = new float[maxOctaveCount];

	freqs[0] = 1.0;
	for (int i = 1; i < maxOctaveCount; i++) {
		freqs[i] = freqs[i - 1] * 2.0;
	}
}

void CachingTurbulator3D::clearCache() {
	if (freqs) {
		delete[] freqs;
	}
	if (partSums) {
		delete[] partSums;
	}
	if (partInterpolations) {
		delete[] partInterpolations;
	}
}
