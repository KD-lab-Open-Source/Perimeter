/**
 *	"Procedural 3D Texture Library"
 *	
 *	Stratum in ZX-plane location of texture.
 *
 *	@version $Id: StratumYLocation3D.h,v 1.1 2002/11/05 17:41:59 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "StratumYLocation3D.hpp"
#include "XMLUtils.hpp"

using namespace tx3d;

const string StratumYLocation3D::TYPE_NAME = "StratumY";

float StratumYLocation3D::distance(const Vector3D& v) {
	if (v.y <= nearBound) {
		return (nearBound - v.y) * fading + blanking;
	} else if (v.y >= farBound) {
		return (v.y - farBound) * fading + blanking;
	} else {
		return blanking;
	}
}
