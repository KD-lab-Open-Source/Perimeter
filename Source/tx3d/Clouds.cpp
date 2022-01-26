/**
 *	"Procedural 3D Texture Library"
 *	
 *	Most simple "colorizers".
 *	Looks like cloud.
 *
 *	@version $Id: Clouds.cpp,v 1.1 2002/11/05 17:38:23 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Clouds.hpp"

using namespace tx3d;

const string Cloud::TYPE_NAME = "Cloud";
const string CloudAbs::TYPE_NAME = "CloudAbs";
const string CloudEmboss::TYPE_NAME = "CloudEmboss";
const string CloudSin::TYPE_NAME = "CloudSin";

void Cloud::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}

	*destClr = diffColor; 
	*destClr *= ((1.0 + noise) / 2.0);
	*destClr += bgColor;
}

void CloudAbs::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}
	*destClr = diffColor; 
	*destClr *= xm::abs(noise);
	*destClr += bgColor;
}

void CloudEmboss::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	noise = xm::abs(noise);
	if (noise > 1.0) {
		noise = 1.0;
	}
	*destClr = diffColor; 
	*destClr *= xm::sqrt(1 - (noise - 1.0) * (noise - 1.0));
	*destClr += bgColor;
}

void CloudSin::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}
	*destClr = diffColor; 
	*destClr *= ((1.0 + xm::sin(1.0 / noise / 10)) / 2.0);
	*destClr += bgColor;
}
