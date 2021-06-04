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
#include "ExtendedWorldLocation3D.hpp"
#include "XMLUtils.hpp"

using namespace tx3d;

const string ExtendedWorldLocation3D::FADING = "fading";

ExtendedWorldLocation3D::ExtendedWorldLocation3D(const string& paramString)
		: WorldLocation3D (paramString) {
	fading = XMLUtils::floatFromTag(XMLUtils::extractTagFromXML(FADING, 0, paramString));
}

string ExtendedWorldLocation3D::paramString() const {
	return 
		  WorldLocation3D::paramString()
		+ XMLUtils::floatToTag(FADING, fading);
}
