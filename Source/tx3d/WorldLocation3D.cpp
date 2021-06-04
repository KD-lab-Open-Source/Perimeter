/**
 *	"Procedural 3D Texture Library"
 *	
 *	World location of texture.
 *
 *	@version $Id: SphereLocation3D.h,v 1.1 2002/11/05 17:41:59 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "WorldLocation3D.hpp"
#include "XMLUtils.hpp"

using namespace tx3d;

const string WorldLocation3D::TYPE_NAME = "World";
const string WorldLocation3D::BLANKING = "blanking";

WorldLocation3D::WorldLocation3D(const string& paramString) {
	blanking = XMLUtils::floatFromTag(XMLUtils::extractTagFromXML(BLANKING, 0, paramString));
}

float WorldLocation3D::distance(const Vector3D& v) {
	return blanking;
}

string WorldLocation3D::paramString() const {
	return 
		  XMLUtils::floatToTag(BLANKING, blanking);
}
