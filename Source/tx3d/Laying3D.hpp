/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Laying3D.h,v 1.1 2002/11/11 15:37:33 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_LAYING3D_H
#define _TX3D_LAYING3D_H

#include "Texture3D.hpp"

namespace tx3d {

	class Laying3D : public Texture3D {
		public:
			Laying3D(const string& paramString);

			Laying3D(const Vector3D &brick = Vector3D()) : brick(brick) {}

			void getColor(Vector3D* destClr, const Vector3D &v);

			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const;

			static const string TYPE_NAME;
			static const string BRICK;

			Vector3D brick;

		protected :
			Vector3D shifted;
	};

}

#endif