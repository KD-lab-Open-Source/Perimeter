/**
 *	"Procedural 3D Texture Library"
 *	
 *	Implementation of texture primitive.
 *
 *	@version $Id: Primitive3D.h,v 1.10 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Primitive3D.hpp"
#include "Colorizer3DFactory.hpp"
#include "SimpleTurbulator3D.hpp"
#include "XMLUtils.hpp"
#include "Interpolator3DFactory.hpp"

using namespace tx3d;

const string Primitive3D::TYPE_NAME = "Primitive";
const string Primitive3D::PERSISTENCE = "persistence";
const string Primitive3D::OCTAVE_COUNT = "octaveCount";
const string Primitive3D::INTERPOLATOR = "interpolator";
const string Primitive3D::COLORIZER = "colorizer";
const string Primitive3D::SHIFT = "shift";
const string Primitive3D::SCALE = "scale";

Primitive3D::Primitive3D(const string& paramString) {
	persistence =
		XMLUtils::floatFromTag(
			XMLUtils::extractTagFromXML(PERSISTENCE, 0, paramString)
		);
	octaveCount = 
		XMLUtils::intFromTag(
			XMLUtils::extractTagFromXML(OCTAVE_COUNT, 0, paramString)
		);
	shift = 
		XMLUtils::vectorFromTag(
			XMLUtils::extractTagFromXML(SHIFT, 0, paramString)
		);
	scale = 
		XMLUtils::vectorFromTag(
			XMLUtils::extractTagFromXML(SCALE, 0, paramString)
		);
	//
	interpolator = 
		Interpolator3DFactory::getFactory().getInterpolator3D(
			XMLUtils::extractTagFromXML(INTERPOLATOR, 0, paramString)
		);
	colorizer = 
		Colorizer3DFactory::createColorizer3D(
			XMLUtils::extractTagFromXML(COLORIZER, 0, paramString)
		);
	if (colorizer) {
		colorizer->referenced();
	}

	//
	setTurbulator(0);
}

Primitive3D::Primitive3D(
	float persistence,
	int octaveCount,
	const Vector3D& shift,
	const Vector3D& scale,
	Colorizer3D *colorizer,
	Interpolator3D *interpolator,
	Turbulator3D *turbulator
) :
	persistence(persistence),
	octaveCount(octaveCount),
	shift(shift),
	scale(scale),
	colorizer(colorizer),
	interpolator(interpolator),
	turbulator(turbulator) {

		if (colorizer) {
			colorizer->referenced();
		}
}

Primitive3D::~Primitive3D() {
	if (colorizer) {
		colorizer->released();
	}
}


void Primitive3D::setColorizer(Colorizer3D *c) {
	if (colorizer) {
		colorizer->released();
	}
	if (c) {
		c->referenced();
	}
	colorizer = c;
}

void Primitive3D::setTurbulator(Turbulator3D *t) {
	if (t) {
		turbulator = t;
	} else {
		turbulator = SimpleTurbulator3D::getSharedSimpleTurbulator3D();
	}
}

string Primitive3D::paramString() const {
	return 
		  XMLUtils::floatToTag(PERSISTENCE, persistence)
		+ XMLUtils::intToTag(OCTAVE_COUNT, octaveCount)
		+ XMLUtils::vectorToTag(SHIFT, shift)
		+ XMLUtils::vectorToTag(SCALE, scale)
		+ XMLUtils::serializableToTag(COLORIZER, *colorizer)
		+ XMLUtils::serializableToTag(INTERPOLATOR, *interpolator);
}
