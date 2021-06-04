/**
 *	"Procedural 3D Texture Library"
 *	
 *	Stratum in ZY-plane location of texture.
 *
 *	@version $Id: StratumXLocation3D.h,v 1.1 2002/11/05 17:41:59 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "StratumXLocation3D.hpp"
#include "XMLUtils.hpp"

using namespace tx3d;

const string StratumXLocation3D::TYPE_NAME = "StratumX";

float StratumXLocation3D::distance(const Vector3D& v) {
	if (v.x <= nearBound) {
		return (nearBound - v.x) * fading + blanking;
	} else if (v.x >= farBound) {
		return (v.x - farBound) * fading + blanking;
	} else {
		return blanking;
	}
}
