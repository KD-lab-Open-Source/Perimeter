/**
 *	"Procedural 3D Texture Library"
 *	
 *	Always returns a specified vector in the color space.
 *
 *	@version $Id: Clear3D.h,v 1.1 2002/11/08 19:05:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_CLEAR3D_H
#define _TX3D_CLEAR3D_H

#include "Texture3D.hpp"

namespace tx3d {

	class Clear3D : public Texture3D {
		public:
			Clear3D(const string& paramString);

			Clear3D(const Vector3D &color = Vector3D()) : color(color) {}

			void getColor(Vector3D* destClr, const Vector3D &v) {
				*destClr = color;
			}

			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const;

			static const string TYPE_NAME;
			static const string COLOR;

			Vector3D color;
	};

}

#endif