/**
 *	"Procedural 3D Texture Library"
 *	
 *	Always returns a specified vector in the color space.
 *
 *	@version $Id: Empty.h,v 1.9 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Empty.hpp"
#include "XMLUtils.hpp"

using namespace tx3d;

const string Empty::TYPE_NAME = "Empty";
const string Empty::BGCOLOR = "bgColor";
const string Empty::FGCOLOR = "fgColor";

Empty::Empty(const string& paramString) {
	bgColor =
		XMLUtils::vectorFromTag(
			XMLUtils::extractTagFromXML(BGCOLOR, 0, paramString)
		);
	diffColor = 
		XMLUtils::vectorFromTag(
			XMLUtils::extractTagFromXML(FGCOLOR, 0, paramString)
		) - 
		bgColor;
}

Empty::Empty(const Vector3D &bgColor, const Vector3D &fgColor) 
	: bgColor(bgColor), diffColor(fgColor - bgColor) {}


void Empty::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	*destClr = bgColor;
}

string Empty::paramString() const {
	return 
		  XMLUtils::vectorToTag(BGCOLOR, bgColor)
		+ XMLUtils::vectorToTag(FGCOLOR, (bgColor + diffColor));
}
