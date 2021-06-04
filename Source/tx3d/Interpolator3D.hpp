/**
 *	"Procedural 3D Texture Library"
 *	
 *	Abstract parent for all of "interpolators".
 *  "Interpolator" should return a value of some approximation function in the point,
 *	that is given as a vector. "Randomizer" is used for computing of matching points.
 *	
 *	@version $Id: Interpolator3D.h,v 1.5 2002/11/05 17:55:26 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_INTERPOLATOR3D_H
#define _TX3D_INTERPOLATOR3D_H

#include "Vector3D.hpp"
#include "Serializable.hpp"

namespace tx3d {

	class Interpolator3D : public Serializable {
		public:
			virtual ~Interpolator3D() {}
			virtual float interpolate(const Vector3D &v) = 0;
	};

}

#endif