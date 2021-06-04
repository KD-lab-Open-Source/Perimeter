/**
 *	"Procedural 3D Texture Library"
 *	
 *	Abstract parent for all of procedural 3D textures.
 *	Texture3D should return a vector in the color space for specified point,
 *	that is given as a vector too.
 *
 *	@version $Id: Texture3D.h,v 1.9 2002/11/07 13:50:54 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_TEXTURE3D_H
#define _TX3D_TEXTURE3D_H

#include "Vector3D.hpp"
#include "Serializable.hpp"
#include "SharedPointer.hpp"

namespace tx3d {

	class Texture3D : public Serializable, public SharedPointer {
		public:
			virtual void getColor(Vector3D* destClr, const Vector3D &v) = 0;
			virtual void zoom(float degree) {
			};
			virtual void zoomColor(float degree) {
			};
	};

}

#endif