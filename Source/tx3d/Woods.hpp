/**
 *	"Procedural 3D Texture Library"
 *	
 *	Sined and cosined remainder "colorizers".
 *	Looks like wood.
 *
 *	@version $Id: Woods.h,v 1.5 2002/11/05 17:38:23 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_WOODS_H
#define _TX3D_WOODS_H

#include "Empty.hpp"

namespace tx3d {

	class Wood : public Empty {
		public:
			Wood(const string& paramString = "");

			Wood(const Vector3D &bgColor, const Vector3D &fgColor, float ringCount);

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);
			
			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const;

			static const string TYPE_NAME;
			static const string RING_COUNT;
			float ringCount;
	};

	class WoodSin : public Wood {
		public:
			WoodSin(const string& paramString = "") : Wood(paramString) {}

			WoodSin(const Vector3D &bgColor, const Vector3D &fgColor, float ringCount)
				: Wood(bgColor, fgColor, ringCount) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return WoodSin::TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

	class WoodCos : public Wood {
		public:
			WoodCos(const string& paramString = "") : Wood(paramString) {}

			WoodCos(const Vector3D &bgColor, const Vector3D &fgColor, float ringCount)
				: Wood(bgColor, fgColor, ringCount) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return WoodCos::TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

	class WoodBackSin : public Wood {
		public:
			WoodBackSin(const string& paramString = "") : Wood(paramString) {}

			WoodBackSin(const Vector3D &bgColor, const Vector3D &fgColor, float ringCount)
				: Wood(bgColor, fgColor, ringCount) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return WoodBackSin::TYPE_NAME;
			}

			static const string TYPE_NAME;
	};
}

#endif