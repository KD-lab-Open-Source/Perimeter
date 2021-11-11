/**
 *	"Procedural 3D Texture Library"
 *	
 *	Sined and cosined remainder "colorizers".
 *	Looks like wood.
 *
 *	@version $Id: Woods.cpp,v 1.1 2002/11/05 17:38:23 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Woods.hpp"
#include "XMLUtils.hpp"
#include "Texture3DUtils.hpp"

using namespace tx3d;

const string Wood::TYPE_NAME = "Wood";
const string Wood::RING_COUNT = "ringCount";
const string WoodSin::TYPE_NAME = "WoodSin";
const string WoodBackSin::TYPE_NAME = "WoodBackSin";
const string WoodCos::TYPE_NAME = "WoodCos";

void Wood::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	noise = noise * ringCount;
	noise = ((1.0 + noise - Texture3DUtils::floor(noise)) / 2.0);

	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < 0.0) {
		noise = 0.0;
	}
	*destClr = diffColor; 
	*destClr *= noise;
	*destClr += bgColor;
}
		
void WoodSin::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	noise = noise * ringCount;
	noise = ((1.0 + xm::sin(noise - Texture3DUtils::floor(noise))) / 2.0);

	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < 0.0) {
		noise = 0.0;
	}
	*destClr = diffColor; 
	*destClr *= noise;
	*destClr += bgColor;
}

void WoodCos::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	noise = noise * ringCount;
	noise = ((1.0 + xm::cos(noise - Texture3DUtils::floor(noise))) / 2.0);

	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < 0.0) {
		noise = 0.0;
	}
	*destClr = diffColor; 
	*destClr *= noise;
	*destClr += bgColor;
}

void WoodBackSin::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	noise = 1.0 / xm::abs(noise) / 100;
	noise = noise * ringCount;
	noise = ((1.0 + xm::sin(noise - Texture3DUtils::floor(noise))) / 2.0);

	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < 0.0) {
		noise = 0.0;
	}
	*destClr = diffColor; 
	*destClr *= noise;
	*destClr += bgColor;
}

Wood::Wood(const string& paramString) : Empty(paramString) {
	ringCount = 
		XMLUtils::floatFromTag(
			XMLUtils::extractTagFromXML(RING_COUNT, 0, paramString)
		);
}

Wood::Wood(const Vector3D &bgColor, const Vector3D &fgColor, float ringCount)
	: Empty(bgColor, fgColor), ringCount(ringCount) {}

string Wood::paramString() const {
	return
		  Empty::paramString()
		+ XMLUtils::floatToTag(RING_COUNT, ringCount);
}
