/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: LayingLocation3D.h,v 1.1 2002/11/11 15:37:33 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "LayingLocation3D.hpp"
#include "Texture3DUtils.hpp"
#include "XMLUtils.hpp"
#include <limits>

using namespace tx3d;

const string LayingLocation3D::TYPE_NAME = "Laying";
const string LayingLocation3D::BRICK = "brick";
const string LayingLocation3D::GAP = "gap";
const string LayingLocation3D::IS_BRICK = "isBrick";
//const float LayingLocation3D::FAR = numeric_limits<float>::max()

LayingLocation3D::LayingLocation3D(const string& paramString) {
	brick = XMLUtils::vectorFromTag(XMLUtils::extractTagFromXML(BRICK, 0, paramString));
	gap = XMLUtils::vectorFromTag(XMLUtils::extractTagFromXML(GAP, 0, paramString));
	isBrick = 
		(XMLUtils::extractTagFromXML(IS_BRICK, 0, paramString) != "");
}

float LayingLocation3D::distance(const Vector3D& v) {
	//NOTE-not-optimized-yet
	shifted.y = v.y / brick.y;
	shifted.y -= Texture3DUtils::floor(shifted.y);
	shifted.y *= brick.y;
	if (shifted.y <= gap.y) {
		return isBrick ? std::numeric_limits<float>::max() : -1.0;
	}
	shifted.z = v.z / brick.z;
	shifted.y = Texture3DUtils::floor(shifted.z);
	shifted.z -= shifted.y;
	shifted.z *= brick.z;
	if (shifted.z <= gap.z) {
		return isBrick ? std::numeric_limits<float>::max() : -1.0;
	}
	shifted.x = v.x;
	shifted.y /= 2.0;
	if ( shifted.y == Texture3DUtils::floor(shifted.y) ) {
		shifted.x += brick.x / 2.0;
	}
	shifted.x /= brick.x;
	shifted.x -= Texture3DUtils::floor(shifted.x);
	shifted.x *= brick.x;
	if (shifted.x <= gap.x) {
		return isBrick ? std::numeric_limits<float>::max() : -1.0;
	} else {
		return isBrick ? -1.0 : std::numeric_limits<float>::max();
	}
}

string LayingLocation3D::paramString() const {
	return 
		  XMLUtils::vectorToTag(BRICK, brick)
		+ XMLUtils::vectorToTag(GAP, gap)
		+ (isBrick ? XMLUtils::createTag(IS_BRICK) : "");
}
