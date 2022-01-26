/**
 *	"Procedural 3D Texture Library"
 *	
 *	Sined and cosined cloud "colorizers".
 *	Looks like marble.
 *
 *	@version $Id: Marbles.cpp,v 1.1 2002/11/05 17:38:23 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Marbles.hpp"

using namespace tx3d;

const string MarbleSolidX::TYPE_NAME = "MarbleSolidX";
const string MarbleRareX::TYPE_NAME = "MarbleRareX";
const string MarbleChaosX::TYPE_NAME = "MarbleChaosX";
const string MarbleChaos::TYPE_NAME = "MarbleChaos";
const string MarbleLeopard::TYPE_NAME = "MarbleLeopard";
const string MarbleSolidY::TYPE_NAME = "MarbleSolidY";
const string MarbleRareY::TYPE_NAME = "MarbleRareY";
const string MarbleChaosY::TYPE_NAME = "MarbleChaosY";

void MarbleSolidX::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}
	*destClr = diffColor; 
	*destClr *= (( 1.0 + xm::sqrt(0.5 * (xm::cos(v.x + noise) + 1.0)) ) / 2.0);
	*destClr += bgColor;
}

void MarbleRareX::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}
	*destClr = diffColor; 
	*destClr *= (( 1.0 + xm::cos(v.x + noise) ) / 2.0);
	*destClr += bgColor;
}

void MarbleChaosX::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}
	*destClr = diffColor; 
	*destClr *= (( 1.0 + xm::cos(3.1415926 * (v.x + noise)) ) / 2.0);
	*destClr += bgColor;
}

void MarbleChaos::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}
	*destClr = diffColor; 
	*destClr *= (( 1.0 + xm::sin(3.1415926 * noise) ) / 2.0);
	*destClr += bgColor;
}

void MarbleLeopard::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}
	*destClr = diffColor; 
	*destClr *= ((1.0 + 0.5 * xm::cos(5.0 * noise)) / 2.0);
	*destClr += bgColor;
}

void MarbleSolidY::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}
	*destClr = diffColor; 
	*destClr *= (( 1.0 + xm::sqrt(0.5 * (xm::cos(v.y + noise) + 1.0)) ) / 2.0);
	*destClr += bgColor;
}

void MarbleRareY::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}
	*destClr = diffColor; 
	*destClr *= (( 1.0 + xm::cos(v.y + noise) ) / 2.0);
	*destClr += bgColor;
}

void MarbleChaosY::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}
	*destClr = diffColor; 
	*destClr *= (( 1.0 + xm::cos(3.1415926 * (v.y + noise)) ) / 2.0);
	*destClr += bgColor;
}

