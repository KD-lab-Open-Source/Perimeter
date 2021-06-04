/**
 *	"Procedural 3D Texture Library"
 *	
 *	Most simple "colorizers".
 *	Looks like cloud.
 *
 *	@version $Id: Clouds.h,v 1.5 2002/11/05 17:38:23 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_CLOUDS_H
#define _TX3D_CLOUDS_H

#include "Empty.hpp"

namespace tx3d {

	class Cloud : public Empty {
		public:
			Cloud(const string& paramString = "") : Empty(paramString) {}

			Cloud(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

	class CloudAbs : public Empty {
		public:
			CloudAbs(const string& paramString = "") : Empty(paramString) {}

			CloudAbs(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

	class CloudEmboss : public Empty {
		public:
			CloudEmboss(const string& paramString = "") : Empty(paramString) {}

			CloudEmboss(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

	class CloudSin : public Empty {
		public:
			CloudSin(const string& paramString = "") : Empty(paramString) {}

			CloudSin(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};
}

#endif