/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Sands.h,v 1.1 2002/11/11 15:37:33 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_SANDS_H
#define _TX3D_SANDS_H

#include "Empty.hpp"

namespace tx3d {

	class Sand : public Empty {
		public:
			Sand(const string& paramString = "");

			Sand(const Vector3D &bgColor, const Vector3D &fgColor, float grad, float step);

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);
			
			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const;

			static const string TYPE_NAME;
			static const string GRADATION;
			static const string STEP;
			float grad;
			float step;

		protected:
			float frac;
	};

	class SandSmooth : public Empty {
		public:
			SandSmooth(const string& paramString = "") : Empty(paramString) {}

			SandSmooth(const Vector3D &bgColor, const Vector3D &fgColor) : Empty(bgColor, fgColor) {}

			void computeColor(Vector3D *destClr, const Vector3D &v, float noise);
			
			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;

		protected:
			float frac;
	};
}

#endif