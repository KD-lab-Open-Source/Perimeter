/**
 *	"Procedural 3D Texture Library"
 *	
 *	Stratum in XY-plane location of texture.
 *
 *	@version $Id: StratumZLocation3D.h,v 1.1 2002/11/05 17:41:59 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "StratumZLocation3D.hpp"
#include "XMLUtils.hpp"

using namespace tx3d;

const string StratumZLocation3D::TYPE_NAME = "StratumZ";

float StratumZLocation3D::distance(const Vector3D& v) {
	if (v.z <= nearBound) {
		return (nearBound - v.z) * fading + blanking;
	} else if (v.z >= farBound) {
		return (v.z - farBound) * fading + blanking;
	} else {
		return blanking;
	}
}
