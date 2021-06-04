/**
 *	"Procedural 3D Texture Library"
 *	
 *	Spherical location of texture.
 *
 *	@version $Id: SphereLocation3D.h,v 1.1 2002/11/05 17:41:59 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "SphereLocation3D.hpp"
#include "XMLUtils.hpp"

using namespace tx3d;

const string SphereLocation3D::TYPE_NAME = "Sphere";
const string SphereLocation3D::CENTER = "center";
const string SphereLocation3D::RADIUS = "radius";

SphereLocation3D::SphereLocation3D(const string& paramString)
	 : ExtendedWorldLocation3D(paramString) {
	center = XMLUtils::vectorFromTag(XMLUtils::extractTagFromXML(CENTER, 0, paramString));
	radius = XMLUtils::floatFromTag(XMLUtils::extractTagFromXML(RADIUS, 0, paramString));
}

float SphereLocation3D::distance(const Vector3D& v) {
	Vector3D dist = v;
	dist -= center;
	float d = dist.scalar() - radius;
	return d < 0 ? blanking : (d * fading + blanking);
}

string SphereLocation3D::paramString() const {
	return 
		  ExtendedWorldLocation3D::paramString()
		+ XMLUtils::vectorToTag(CENTER, center)
		+ XMLUtils::floatToTag(RADIUS, radius);
}
