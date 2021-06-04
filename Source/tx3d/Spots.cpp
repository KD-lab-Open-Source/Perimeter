/**
 *	"Procedural 3D Texture Library"
 *	
 *	Spots, like hoar-frost.
 *
 *	@version $Id: Spots.cpp,v 1.1 2002/11/05 17:38:23 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Spots.hpp"

using namespace tx3d;

const string Spots::TYPE_NAME = "Spots";

void Spots::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	noise = 0.1 / (noise + v.y / 20.0);
	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}
	*destClr = diffColor; 
	*destClr *= ((1.0 + noise ) / 2.0);
	*destClr += bgColor;
}

