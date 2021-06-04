/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: LayingLocation3D.h,v 1.1 2002/11/11 15:37:33 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_LAYINGLOCATION3D_H
#define _TX3D_LAYINGLOCATION3D_H

#include "Location3D.hpp"

namespace tx3d {

	class LayingLocation3D : public Location3D {
		public :
			LayingLocation3D(const string& paramString);

			LayingLocation3D(
				const Vector3D& brick = Vector3D(),
				const Vector3D& gap = Vector3D(),
				bool isBrick = true
			) : brick(brick), gap(gap), isBrick(isBrick) {}


			float distance(const Vector3D& v);

			void zoom(float degree) {
				brick *= degree;
				gap *= degree;
			}

			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const;

			static const string TYPE_NAME;
			static const string BRICK;
			static const string GAP;
			static const string IS_BRICK;
//			static const float FAR = numeric_limits<float>::max();
			Vector3D brick;
			Vector3D gap;
			bool isBrick;

		protected :
			Vector3D shifted;
	};

}

#endif