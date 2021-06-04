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

#ifndef _TX3D_EXTENDEDWORLDLOCATION3D_H
#define _TX3D_EXTENDEDWORLDLOCATION3D_H

#include "WorldLocation3D.hpp"

namespace tx3d {

	class ExtendedWorldLocation3D : public WorldLocation3D {
		public :
			ExtendedWorldLocation3D(const string& paramString);

			ExtendedWorldLocation3D(const ExtendedWorldLocation3D* xWorldLocation)
				: WorldLocation3D(xWorldLocation) {
				fading = xWorldLocation->fading;
			}

			ExtendedWorldLocation3D(
				float blanking = 0.0f,
				float fading = 1.0f)
				: WorldLocation3D(blanking),
				fading(fading) {
			}

			string paramString() const;

			static const string FADING;
			float fading;
	};

}

#endif //_TX3D_EXTENDEDWORLDLOCATION3D_H