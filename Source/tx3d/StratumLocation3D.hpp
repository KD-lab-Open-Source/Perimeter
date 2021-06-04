/**
 *	"Procedural 3D Texture Library"
 *	
 *	Stratum in plane location of texture.
 *
 *	@version $Id: StratumXLocation3D.h,v 1.1 2002/11/05 17:41:59 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_STRATUMLOCATION3D_H
#define _TX3D_STRATUMLOCATION3D_H

#include "ExtendedWorldLocation3D.hpp"

namespace tx3d {

	class StratumLocation3D : public ExtendedWorldLocation3D {
		public :
			StratumLocation3D(const string& paramString);

			StratumLocation3D(const StratumLocation3D* stratumLocation);

			StratumLocation3D(
				float nearBound = 0.0,
				float farBound = 0.0,
				float blanking = 0,
				float fading = 1.0f)
				: ExtendedWorldLocation3D(blanking, fading),
				nearBound(nearBound),
				farBound(farBound) {
			}

			void zoom(float degree) {
				nearBound *= degree;
				farBound *= degree;
			}

			string paramString() const;

			static const string NEAR_BOUND;
			static const string FAR_BOUND;
			float nearBound;
			float farBound;
	};

}

#endif