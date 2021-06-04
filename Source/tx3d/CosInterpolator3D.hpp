/**
 *	"Procedural 3D Texture Library"
 *	
 *  This "interpolator" implements simple cosine approximation.
 *	
 *	@version $Id: CosInterpolator3D.h,v 1.5 2002/11/05 17:55:26 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_COSINTERPOLATOR3D_H
#define _TX3D_COSINTERPOLATOR3D_H

#include "Interpolator3D.hpp"

namespace tx3d {

	class CosInterpolator3D : public Interpolator3D {
		public:
			float interpolate(const Vector3D &v);

			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const {
				return "";
			}

			inline float cosInterpolate(float h1, float h2, float shift);

			static float getNoise(int x, int y, int z);

			static const string TYPE_NAME;

		protected:
			//for-performance-purposes
			int xi;
			int yi;
			int zi;
			float xd;
			float yd;
			float zd;
			float vert1;
			float vert2;
			float vert3;
			float vert4;
			float vert5;
			float vert6;
			float vert7;
			float vert8;
			float coss;
};

}

#endif