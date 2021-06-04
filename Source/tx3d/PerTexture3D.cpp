/**
 *	"Procedural 3D Texture Library"
 *	
 *	PerTexture3D implements a Texture3D, that is perturbed by another Texture3D.
 *
 *	@version $Id: PerTexture3D.h,v 1.5 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "PerTexture3D.hpp"
#include "Texture3DFactory.hpp"
#include "XMLUtils.hpp"

using namespace tx3d;

const string PerTexture3D::TYPE_NAME = "Perturbed";
const string PerTexture3D::CORE = "core";
const string PerTexture3D::PERTURBATOR = "perturbator";

PerTexture3D::PerTexture3D(const string& paramString) {
	core = 
		Texture3DFactory::createTexture3D(
			XMLUtils::extractTagFromXML(CORE, 0, paramString)
		);
	if (core) {
		core->referenced();
	}
	perturbator = 
		Texture3DFactory::createTexture3D(
			XMLUtils::extractTagFromXML(PERTURBATOR, 0, paramString)
		);
	if (perturbator) {
		perturbator->referenced();
	}
}

PerTexture3D::PerTexture3D(Texture3D *core, Texture3D *perturbator)
		: core(core), perturbator(perturbator) {

	if (core) {
		core->referenced();
	}

	if (perturbator) {
		perturbator->referenced();
	}
}

PerTexture3D::~PerTexture3D() {
	if (core) {
		core->released();
	}
	if (perturbator) {
		perturbator->released();
	}
}

void PerTexture3D::setCore(Texture3D* t) {
	if (core) {
		core->released();
	}
	if (t) {
		t->referenced();
	}
	core = t;
}

void PerTexture3D::setPerturbator(Texture3D* t) {
	if (perturbator) {
		perturbator->released();
	}
	if (t) {
		t->referenced();
	}
	perturbator = t;
}

string PerTexture3D::paramString() const {
	return 
		  XMLUtils::serializableToTag(CORE, *core)
		+ XMLUtils::serializableToTag(PERTURBATOR, *perturbator);
}
