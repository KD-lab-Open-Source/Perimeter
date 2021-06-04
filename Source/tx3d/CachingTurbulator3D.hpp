/**
 *	"Procedural 3D Texture Library"
 *	
 *	This "turbulator" caches one sum, all partial sums and interpolations.
 *  Save computation time when having a several textures in queue.
 *	Precompute and store powers of two.
 *	
 *	@version $Id: CachingTurbulator3D.h,v 1.7 2002/11/05 17:55:26 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_CACHINGTURBULATOR3D_H
#define _TX3D_CACHINGTURBULATOR3D_H

#include "Turbulator3D.hpp"

namespace tx3d {

	class CachingTurbulator3D : public Turbulator3D {

		public :
			CachingTurbulator3D(int maximumOctaveCount = 1);

			~CachingTurbulator3D() {
				clearCache();
			}

			void setMaxOctaveCount(int maximumOctaveCount);

			int getMaxOctaveCount() {
				return maxOctaveCount;
			}

			float turbulate3D(const Vector3D &v, float persistence, int octaveCount, Interpolator3D *interpolator);

		protected :
			void clearCache();

			int maxOctaveCount;

			Vector3D lastV;
			float lastPersistence;
			int lastOctaveCount;
			Interpolator3D *lastInterpolator;

			float *freqs;

			float *partSums;
			float *partInterpolations;

			float sum;
			float ampl;

			int lastInterpolationCount;

			Vector3D freqV;
	};

}

#endif