/**
 *	"Procedural 3D Texture Library"
 *	
 *	Stratum in ZY-plane location of texture.
 *
 *	@version $Id: StratumXLocation3D.h,v 1.1 2002/11/05 17:41:59 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_STRATUMXLOCATION3D_H
#define _TX3D_STRATUMXLOCATION3D_H

#include "StratumLocation3D.hpp"

namespace tx3d {

	class StratumXLocation3D : public StratumLocation3D {
		public :
			StratumXLocation3D(const string& paramString) : StratumLocation3D(paramString) {
			}

			StratumXLocation3D(const StratumLocation3D* stratumLoc)
				: StratumLocation3D(stratumLoc) {
			}

			StratumXLocation3D(float nearBound = 0.0, float farBound = 0.0)
				: StratumLocation3D(nearBound, farBound) {
			}

			float distance(const Vector3D& v);

			const string& getType() const {
				return TYPE_NAME;
			}

			static const string TYPE_NAME;
	};

}

#endif