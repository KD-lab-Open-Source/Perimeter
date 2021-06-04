/**
 *	"Procedural 3D Texture Library"
 *	
 *  Abstract parent for all of "turbulators".
 *	"Turbulator" should compute fractal sum of octaves with specified amplitudes.
 *	
 *	@version $Id: Turbulator3D.h,v 1.9 2002/11/05 17:55:27 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_TURBULATOR3D_H
#define _TX3D_TURBULATOR3D_H

#include "Interpolator3D.hpp"

namespace tx3d {

	class Turbulator3D {
		public :
			virtual ~Turbulator3D() {} 
			virtual float turbulate3D(const Vector3D &v, float persistence, int octaveCount, Interpolator3D *interpolator) = 0;
	};

}

#endif
