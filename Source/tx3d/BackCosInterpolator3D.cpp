/**
 *	"Procedural 3D Texture Library"
 *	
 *  This "interpolator" implements back-cosine approximation.
 *	
 *	@version $Id: BackCosInterpolator3D.cpp,v 1.6 2002/11/11 15:37:33 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "BackCosInterpolator3D.hpp"

using namespace tx3d;

const string BackCosInterpolator3D::TYPE_NAME = "BackCosine";
const string BackCosInterpolator3D::DEPTH = "depth";

float BackCosInterpolator3D::interpolate(const Vector3D &v) {
	return
		depth / CosInterpolator3D::interpolate(v);
}

BackCosInterpolator3D::BackCosInterpolator3D(const string& paramString) {
	depth = XMLUtils::floatFromTag(XMLUtils::extractTagFromXML(DEPTH, 0, paramString));
}

string BackCosInterpolator3D::paramString() const {
	return 
	  XMLUtils::floatToTag(DEPTH, depth);
}
