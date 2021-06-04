/**
 *	"Procedural 3D Texture Library"
 *	
 *	This "turbulator" caches the one sum.
 *  Save computation time when having a several textures in queue.
 *	Precompute and store powers of two.
 *	
 *	@version $Id: SimpleCachingTurbulator3D.h,v 1.4 2002/11/05 17:55:26 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_SIMPLECACHINGTURBULATOR3D_H
#define _TX3D_SIMPLECACHINGTURBULATOR3D_H

#include "Turbulator3D.hpp"

namespace tx3d {

	class SimpleCachingTurbulator3D : public Turbulator3D {
		public:
			SimpleCachingTurbulator3D(int maximumOctaveCount = 1);

			~SimpleCachingTurbulator3D();

			void setMaxOctaveCount(int maximumOctaveCount);

			int getMaxOctaveCount() {
				return maxOctaveCount;
			}

			float turbulate3D(const Vector3D &v, float persistence, int octaveCount, Interpolator3D *interpolator);

		protected:
			int maxOctaveCount;

			float lastSum;
			Vector3D lastV;
			float lastPersistence;
			int lastOctaveCount;
			Interpolator3D *lastInterpolator;

			float *freqs;
			Vector3D freqV;
	};

}

#endif