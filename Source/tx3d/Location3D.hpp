/**
 *	"Procedural 3D Texture Library"
 *	
 *	Defines the location of texture in space.
 *	Should return distance from location bounds to any point, that is given as vector.
 *
 *	@version $Id: Location3D.h,v 1.2 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_LOCATION3D_H
#define _TX3D_LOCATION3D_H

#include "Serializable.hpp"
#include "Vector3D.hpp"
#include "SharedPointer.hpp"

namespace tx3d {

	class Location3D : public Serializable, public SharedPointer {
		public :
			virtual float distance(const Vector3D& v) = 0;
			virtual void zoom(float degree) {
			};
	};

}

#endif