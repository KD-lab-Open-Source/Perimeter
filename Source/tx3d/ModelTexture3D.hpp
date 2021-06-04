/**
 *	"Procedural 3D Texture Library"
 *	
 *
 *	@version $Id: PerTexture3D.h,v 1.5 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_MODELTEXTURE3D_H
#define _TX3D_MODELTEXTURE3D_H

#include "Texture3D.hpp"

namespace tx3d {

	class ModelTexture3D : public Texture3D {

		public :
			ModelTexture3D(const string& paramString);

			ModelTexture3D();

			~ModelTexture3D();
			
			void getColor(Vector3D* destClr, const Vector3D &v);

			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const;

			static const string TYPE_NAME;

		protected :
//			inline float lerp(float h1, float h2, float shift1, float shift);

			Vector3D* model;
	};

}

#endif