/**
 *	"Procedural 3D Texture Library"
 *	
 *  
 *	
 *	@version $Id: BackCosInterpolator3D.cpp,v 1.6 2002/11/11 15:37:33 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "BackCubicInterpolator3D.hpp"

using namespace tx3d;

const string BackCubicInterpolator3D::TYPE_NAME = "BackCubic";
const string BackCubicInterpolator3D::DEPTH = "depth";

float BackCubicInterpolator3D::interpolate(const Vector3D &v) {
	return
		depth / CubicInterpolator3D::interpolate(v);
}

BackCubicInterpolator3D::BackCubicInterpolator3D(const string& paramString) {
	depth = XMLUtils::floatFromTag(XMLUtils::extractTagFromXML(DEPTH, 0, paramString));
}

string BackCubicInterpolator3D::paramString() const {
	return 
	  XMLUtils::floatToTag(DEPTH, depth);
}
