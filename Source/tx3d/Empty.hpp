/**
 *	"Procedural 3D Texture Library"
 *	
 *	Always returns a specified vector in the color space.
 *
 *	@version $Id: Empty.h,v 1.9 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_EMPTY_H
#define _TX3D_EMPTY_H

#include "Colorizer3D.hpp"

namespace tx3d {

	class Empty : public Colorizer3D {
		public:
			Empty(const string& paramString = "");

			Empty(const Vector3D &bgColor, const Vector3D &fgColor);

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			void zoomColor(float degree) {
				bgColor *= degree;
				diffColor *= degree;
			}

			string paramString() const;

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
			static const string BGCOLOR;
			static const string FGCOLOR;
			Vector3D bgColor;
			Vector3D diffColor;
	};
}

#endif