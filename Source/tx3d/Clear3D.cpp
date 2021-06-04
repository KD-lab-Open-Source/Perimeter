/**
 *	"Procedural 3D Texture Library"
 *	
 *	Always returns a specified vector in the color space.
 *
 *	@version $Id: Clear3D.h,v 1.1 2002/11/08 19:05:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Clear3D.hpp"
#include "XMLUtils.hpp"

using namespace tx3d;

const string Clear3D::TYPE_NAME = "Clear";
const string Clear3D::COLOR = "color";

Clear3D::Clear3D(const string& paramString) {
	color =
		XMLUtils::vectorFromTag(
			XMLUtils::extractTagFromXML(COLOR, 0, paramString)
		);
}

string Clear3D::paramString() const {
	return 
		  XMLUtils::vectorToTag(COLOR, color);
}
