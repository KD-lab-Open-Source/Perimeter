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

#ifndef _TX3D_CUBICINTERPOLATOR3D_H
#define _TX3D_CUBICINTERPOLATOR3D_H

#include "Interpolator3D.hpp"

namespace tx3d {

	class CubicInterpolator3D : public Interpolator3D {
		public:
			float interpolate(const Vector3D &v);

			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const {
				return "";
			}

//			inline float lerp(float h1, float h2, float shift);
//			inline float lerp(float h1, float h2, float shift1, float shift);

			inline float getNoise(long a);

			static const string TYPE_NAME;
	};

}

#endif