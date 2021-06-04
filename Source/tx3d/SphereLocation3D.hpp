/**
 *	"Procedural 3D Texture Library"
 *	
 *	Spherical location of texture.
 *
 *	@version $Id: SphereLocation3D.h,v 1.1 2002/11/05 17:41:59 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_SPHERELOCATION3D_H
#define _TX3D_SPHERELOCATION3D_H

#include "ExtendedWorldLocation3D.hpp"

namespace tx3d {

	class SphereLocation3D : public ExtendedWorldLocation3D {
		public :
			SphereLocation3D(const string& paramString);

			SphereLocation3D(
				const Vector3D& center = Vector3D(),
				float radius = 0,
				float blanking = 0,
				float fading = 1.0f )
				: ExtendedWorldLocation3D(blanking, fading),
				center(center),
				radius(radius) {
			}

			float distance(const Vector3D& v);

			void zoom(float degree) {
				center *= degree;
				radius *= degree;
			}

			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const;

			static const string TYPE_NAME;
			static const string CENTER;
			static const string RADIUS;
			Vector3D center;
			float radius;
	};
}

#endif