/**
 *	"Procedural 3D Texture Library"
 *	
 *	Spots, like hoar-frost.
 *
 *	@version $Id: Spots.h,v 1.6 2002/11/05 17:38:23 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_SPOTS_H
#define _TX3D_SPOTS_H

#include "Empty.hpp"

namespace tx3d {

	class Spots : public Empty {
		public:
			Spots(const string& paramString = "") : Empty(paramString) {}

			Spots(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);
			
			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};
}

#endif