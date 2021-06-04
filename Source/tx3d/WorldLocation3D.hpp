/**
 *	"Procedural 3D Texture Library"
 *	
 *	World location of texture.
 *
 *	@version $Id: SphereLocation3D.h,v 1.1 2002/11/05 17:41:59 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_WORLDLOCATION3D_H
#define _TX3D_WORLDLOCATION3D_H

#include "Location3D.hpp"

namespace tx3d {

	class WorldLocation3D : public Location3D {
		public :
			WorldLocation3D(const string& paramString);

			WorldLocation3D(const WorldLocation3D* worldLocation) {
				blanking = worldLocation->blanking;
			}

			WorldLocation3D(float blanking = 0)
				: blanking(blanking) {
			}

			float distance(const Vector3D& v);

			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const;

			static const string TYPE_NAME;
			static const string BLANKING;
			float blanking;
	};

}

#endif