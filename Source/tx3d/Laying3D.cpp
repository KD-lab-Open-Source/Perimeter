/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Laying3D.h,v 1.1 2002/11/11 15:37:33 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Laying3D.hpp"
#include "XMLUtils.hpp"

using namespace tx3d;

const string Laying3D::TYPE_NAME = "Laying";
const string Laying3D::BRICK = "brick";

Laying3D::Laying3D(const string& paramString) {
	brick =
		XMLUtils::vectorFromTag(
			XMLUtils::extractTagFromXML(BRICK, 0, paramString)
		);
}

void Laying3D::getColor(Vector3D* destClr, const Vector3D &v) {
	//TODO: optimization wanted :)
	shifted.x = int(v.x / brick.x) / 2.0;
	shifted.y = int(v.y / brick.y) / 2.0;
	shifted.z = int(v.z / brick.z) / 2.0;
	if ( shifted.z == (int)shifted.z ) {
		shifted.x = int((v.x + brick.x / 2.0) / brick.x) / 2.0;
	} else {
		shifted.x = int(v.x / brick.x) / 2.0;
	}
	shifted.x = shifted.x + shifted.y + shifted.z;
	if ( shifted.x == (int)shifted.x ) {
		*destClr = brick;
	} else {
		destClr->x = 0;
		destClr->y = 0;
		destClr->z = 0;
	}
	if ( shifted.z == (int)shifted.z ) {
		destClr->x += (brick.x / 2.0);
	}
}

string Laying3D::paramString() const {
	return 
		  XMLUtils::vectorToTag(BRICK, brick);
}
