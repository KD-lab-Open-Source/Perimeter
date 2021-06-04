/**
 *	"Procedural 3D Texture Library"
 *	
 *	Sined and cosined cloud "colorizers".
 *	Looks like marble.
 *
 *	@version $Id: Marbles.h,v 1.5 2002/11/05 17:38:23 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_MARBLES_H
#define _TX3D_MARBLES_H

#include "Empty.hpp"

namespace tx3d {

	class MarbleSolidX : public Empty {
		public :
			MarbleSolidX(const string& paramString = "") : Empty(paramString) {}

			MarbleSolidX(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

	class MarbleRareX : public Empty {
		public:
			MarbleRareX(const string& paramString = "") : Empty(paramString) {}

			MarbleRareX(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

	class MarbleChaosX : public Empty {
		public:
			MarbleChaosX(const string& paramString = "") : Empty(paramString) {}

			MarbleChaosX(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

	class MarbleChaos : public Empty {
		public:
			MarbleChaos(const string& paramString = "") : Empty(paramString) {}

			MarbleChaos(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

	class MarbleLeopard : public Empty {
		public:
			MarbleLeopard(const string& paramString = "") : Empty(paramString) {}

			MarbleLeopard(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

	class MarbleSolidY : public Empty {
		public :
			MarbleSolidY(const string& paramString = "") : Empty(paramString) {}

			MarbleSolidY(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

	class MarbleRareY : public Empty {
		public:
			MarbleRareY(const string& paramString = "") : Empty(paramString) {}

			MarbleRareY(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

	class MarbleChaosY : public Empty {
		public:
			MarbleChaosY(const string& paramString = "") : Empty(paramString) {}

			MarbleChaosY(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);
			
			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};
}

#endif