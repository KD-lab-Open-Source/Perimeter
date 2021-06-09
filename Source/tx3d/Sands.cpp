/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Sands.cpp,v 1.1 2002/11/11 15:37:33 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Sands.hpp"
#include "XMLUtils.hpp"
#include "Texture3DUtils.hpp"

using namespace tx3d;

const string Sand::TYPE_NAME = "Sand";
const string Sand::GRADATION = "grad";
const string Sand::STEP = "step";
const string SandSmooth::TYPE_NAME = "SandSmooth";

void Sand::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	noise *= grad;
	if ((Texture3DUtils::round(noise) & 1) != 0) {
		noise -=step;
	}
	noise /= grad;

	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}

	*destClr = diffColor; 
	*destClr *= ((1.0 + noise) / 2.0);
	*destClr += bgColor;
}

void SandSmooth::computeColor(Vector3D *destClr, const Vector3D &v, float noise) {
	noise *= 3.0;
	if ((Texture3DUtils::round(noise) & 1) != 0) {
		noise -=2.0;
	}
	noise /= 3.0;

	if (noise > 1.0) {
		noise = 1.0;
	} else if (noise < -1.0) {
		noise = -1.0;
	}
	if (noise < -0.2) {
		noise += 0.66f;
	}


	*destClr = diffColor; 
	*destClr *= ((1.0 + noise) / 2.0);
	*destClr += bgColor;
}

Sand::Sand(const string& paramString) : Empty(paramString) {
	grad = 
		XMLUtils::floatFromTag(
			XMLUtils::extractTagFromXML(GRADATION, 0, paramString)
		);
	step = 
		XMLUtils::floatFromTag(
			XMLUtils::extractTagFromXML(STEP, 0, paramString)
		);
	if (grad == 0.0) {
		grad = 1.0;
	}
}

Sand::Sand(const Vector3D &bgColor, const Vector3D &fgColor, float grad, float step) 
	: Empty(bgColor, fgColor), grad(grad), step(step) {}

string Sand::paramString() const {
	return
		  Empty::paramString()
		+ XMLUtils::floatToTag(GRADATION, grad)
		+ XMLUtils::floatToTag(STEP, step);
}
