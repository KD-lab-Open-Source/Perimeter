/**
 *	"Procedural 3D Texture Library"
 *	
 *	Abstract parent for all of "colorizers".
 *	The "colorizer" means class, that compute a vector in the color space
 *	for the random noise in a specified point, that is given as vector.
 *
 *	@version $Id: Colorizer3D.h,v 1.8 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_COLORIZER3D_H
#define _TX3D_COLORIZER3D_H

#include "Vector3D.hpp"
#include "Serializable.hpp"
#include "SharedPointer.hpp"

namespace tx3d {

	class Colorizer3D : public Serializable, public SharedPointer {
		public:
			virtual void computeColor(Vector3D *destClr, const Vector3D &v, float noise) = 0;
			virtual void zoomColor(float degree) {
			};
	};

}

#endif