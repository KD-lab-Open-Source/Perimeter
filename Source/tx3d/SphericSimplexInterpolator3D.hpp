/**
 *	"Procedural 3D Texture Library"
 *	
 *  
 *	
 *	@version $Id: CosInterpolator3D.h,v 1.5 2002/11/05 17:55:26 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_SPHERICSIMPLEXINTERPOLATOR3D_H
#define _TX3D_SPHERICSIMPLEXINTERPOLATOR3D_H

#include "Interpolator3D.hpp"

namespace tx3d {

	class SphericSimplexInterpolator3D : public Interpolator3D {
		public:
			SphericSimplexInterpolator3D();

			float interpolate(const Vector3D &v);

			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const {
				return "";
			}

			static const string TYPE_NAME;

		protected:
			inline float contribution(char index);

			inline char extractBit(char source, char bitNumber);

			inline int shuffle(int x, int y, int z);

			inline int getBitPattern(int x, int y, int z, char bitNumber);

			//for-performance-purposes

			char simplex[3];

			int bitPatterns[8];
	};

}

#endif