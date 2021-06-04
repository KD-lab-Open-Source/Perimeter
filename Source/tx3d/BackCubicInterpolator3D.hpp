/**
 *	"Procedural 3D Texture Library"
 *	
 *  
 *	
 *	@version $Id: BackCosInterpolator3D.h,v 1.5 2002/11/05 17:55:26 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_BACKCUBICINTERPOLATOR3D_H
#define _TX3D_BACKCUBICINTERPOLATOR3D_H

#include "CubicInterpolator3D.hpp"
#include "XMLUtils.hpp"

namespace tx3d {

	class BackCubicInterpolator3D : public CubicInterpolator3D {
		public :
			BackCubicInterpolator3D(const string& paramString);

			BackCubicInterpolator3D(float depth = 0.02) : depth(depth) {}

			float interpolate(const Vector3D &v);

			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const;

			static const string TYPE_NAME;
			static const string DEPTH;
			float depth;
	};

}

#endif