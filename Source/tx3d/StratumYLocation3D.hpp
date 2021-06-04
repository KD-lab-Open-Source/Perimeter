/**
 *	"Procedural 3D Texture Library"
 *	
 *	Stratum in ZX-plane location of texture.
 *
 *	@version $Id: StratumYLocation3D.h,v 1.1 2002/11/05 17:41:59 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_STRATUMYLOCATION3D_H
#define _TX3D_STRATUMYLOCATION3D_H

#include "StratumLocation3D.hpp"

namespace tx3d {

	class StratumYLocation3D : public StratumLocation3D {
		public :
			StratumYLocation3D(const string& paramString) : StratumLocation3D(paramString) {
			}

			StratumYLocation3D(const StratumLocation3D* stratumLoc)
				: StratumLocation3D(stratumLoc) {
			}

			StratumYLocation3D(float nearBound = 0.0, float farBound = 0.0)
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