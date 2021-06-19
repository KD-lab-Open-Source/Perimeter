/**
 *	"Procedural 3D Texture Library"
 *	
 *	Implementation of compound texture.
 *
 *	@version $Id: Compound3D.cpp,v 1.11 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Compound3D.hpp"
#include "CachingTurbulator3D.hpp"
#include "Texture3DUtils.hpp"
#include <limits>

using namespace tx3d;

const string Compound3D::TYPE_NAME = "Compound";
const string Compound3D::COMPONENT = "component";
const string Component::TYPE_NAME = "";
const string Component::LOCATION = "location";
const string Component::TEXTURE = "texture";
const string Component::SHOULD_DEGRADATE = "shouldDegradate";

Compound3D::Compound3D(const string& paramString) {
	turbulator = new CachingTurbulator3D();
	int textureCount = 0;
	string component;
	while(
		(component = XMLUtils::extractTagFromXML(COMPONENT, textureCount, paramString)) != ""
	) {
		addComponent(XMLUtils::extractContentFromTag(component));
		textureCount++;
	}
}

Compound3D::Compound3D() {
	turbulator = new CachingTurbulator3D();
}

Compound3D::~Compound3D() {
	if (turbulator) {
		for (int i = 0; i < components.size(); ++i) {
			if (components[i]->getPrimitive() && components[i]->getPrimitive()->getTurbulator() == turbulator) {
				components[i]->getPrimitive()->setTurbulator(0);
			}
		}
	}

	for (int i = 0; i < components.size(); ++i) {
		components[i]->released();
//		delete components[i];
	}

	if (turbulator) {
		delete turbulator;
	}
}

void Compound3D::getColor(Vector3D* destClr, const Vector3D &v) {
/*	if (components.size() == 1) {
		components[0]->getTexture()->getColor(destClr, v);
		return;
	}
*/
	clr.x = 0.0;
	clr.y = 0.0;
	clr.z = 0.0;

	float sumAll = 0.0;
	float sumDegradated = std::numeric_limits<float>::denorm_min();
	int i;
	for (i = 0; i < components.size(); i++) {
		distances[i] = components[i]->getLocation()->distance(v);
		if (distances[i] <= 0.0) {
			components[i]->getTexture()->getColor(destClr, v);
			return;
		}
		distances[i] = 1.0 / distances[i];
		sumAll += distances[i];
		if (components[i]->shouldDegradate) {
			sumDegradated += distances[i];
		}
	}

	sumAll = 1.0 / sumAll;
	sumDegradated = 1.0 / sumDegradated;
	
	float pDegradated = 0.0;
	float oDegradated = 0.0;
	for (i = 0; i < components.size(); i++) {
		if (components[i]->shouldDegradate) {
			t = sumDegradated * distances[i];
			pDegradated += components[i]->getPrimitive()->persistence * t;
			oDegradated += components[i]->getPrimitive()->octaveCount * t;
		}
	}

	o = Texture3DUtils::round( oDegradated);
	for (i = 0; i < components.size(); i++) {
		t = sumAll * distances[i];
		if (components[i]->shouldDegradate) {
			components[i]->getPrimitive()->getColor(destClr, v, pDegradated, o);
		} else {
			components[i]->getTexture()->getColor(destClr, v);
		}
		*destClr *= t;
		clr += *destClr;
	}
	*destClr = clr;
}


/**
 * ...
 *
 * @param		primitive
 */

int Compound3D::getIndexToInsert(const Primitive3D *primitive) {
	int j = 0;
	for (int i = 0; i < components.size(); i++) {
		if (components[i]->getPrimitive() != 0) {
			if (components[i]->getPrimitive()->interpolator == primitive->interpolator &&
				components[i]->getPrimitive()->shift == primitive->shift &&
				components[i]->getPrimitive()->scale == primitive->scale) {
				if (components[i]->shouldDegradate) {
					return i;
				} else {
					j = i;
				}
			}
		}
	}
	return j;
}

void Compound3D::addTexture3D(Texture3D *texture, bool shouldDegradate, Location3D *location) {
	addComponent(new Component(texture, shouldDegradate, location));
}

void Compound3D::addComponent(const string& paramString) {
	addComponent(new Component(paramString));
}

void Compound3D::addComponent(Component* component) {
	if (component) {
		component->referenced();
	}
	if (component->getPrimitive() != 0) {
		component->getPrimitive()->setTurbulator(turbulator);
		if (component->getPrimitive()->octaveCount > turbulator->getMaxOctaveCount()) {
			turbulator->setMaxOctaveCount(component->getPrimitive()->octaveCount);
		}
		components.insert(
			components.begin() + getIndexToInsert(component->getPrimitive()),
			component
		);
		distances.push_back(0.0);
	} else {
		component->shouldDegradate = false;
		components.push_back(component);
		distances.push_back(0.0);
	}
}

string Compound3D::paramString() const {
	string params;
		for (int i = 0; i < components.size(); i++) {
			params += XMLUtils::serializableToTag(COMPONENT, *components[i]);
		}
	return params;
}

void Compound3D::remove(Component* component) {
	int index = getComponentIndex(component);
	if (index >= 0) {
		components.erase(components.begin() + index);
		if (turbulator) {
			if (
				   component->getPrimitive()
				&& component->getPrimitive()->getTurbulator() == turbulator
				) {

				component->getPrimitive()->setTurbulator(0);
			}
		}
		component->released();
	}
}

Component* Compound3D::getComponent(int index) {
	return components[index];
}

int Compound3D::getComponentCount() {
	return components.size();
}

int Compound3D::getComponentIndex(Component* component) {
	for (int i = 0, s = components.size(); i < s; i++) {
		if (component == components[i]) {
			return i;
		}
	}
	return -1;
}

void Compound3D::zoom(float degree) {
	for (int i = 0, s = components.size(); i < s; i++) {
		components[i]->zoom(degree);
	}
}
void Compound3D::zoomColor(float degree) {
	for (int i = 0, s = components.size(); i < s; i++) {
		components[i]->zoomColor(degree);
	}
}

//----------Component Methods-----------------------

Component::Component(const string& paramString) {
	texture = 
		Texture3DFactory::createTexture3D(
			XMLUtils::extractTagFromXML(TEXTURE, 0, paramString)
		);
	if (texture) {
		texture->referenced();
	}
	primitive = dynamic_cast<Primitive3D*>(texture);
	shouldDegradate = 
		(XMLUtils::extractTagFromXML(SHOULD_DEGRADATE, 0, paramString) != "");
	location = 
		Location3DFactory::createLocation3D(
			XMLUtils::extractTagFromXML(LOCATION, 0, paramString)
		);
	if (location) {
		location->referenced();
	}
}

Component::Component(const Component& component) {
	texture = component.texture;
	if (texture) {
		texture->referenced();
	}
	primitive = component.primitive;
	shouldDegradate = component.shouldDegradate;
	location = component.location;
	if (location) {
		location->referenced();
	}
}

Component::Component(Texture3D *texture, bool shouldDegradate,	Location3D *location)
		: texture(texture),	primitive(primitive), shouldDegradate(shouldDegradate),	location(location) {

	if (texture) {
		texture->referenced();
	}
	primitive = dynamic_cast<Primitive3D*>(texture);
	if (location) {
		location->referenced();
	}
}

Component::~Component() {
	if (texture) {
		texture->released();
	}
	if (location) {
		location->released();
	}
}

void Component::setTexture(Texture3D* t) {
	if (texture) {
		texture->released();
	}
	if (t) {
		t->referenced();
	}
	texture = t;
}

void Component::setLocation(Location3D* l) {
	if (location) {
		location->released();
	}
	if (l) {
		l->referenced();
	}
	location = l;
}

string Component::paramString() const {
	return 
		  XMLUtils::serializableToTag(TEXTURE, *texture)
		+ (shouldDegradate ? XMLUtils::createTag(SHOULD_DEGRADATE) : "")
		+ XMLUtils::serializableToTag(LOCATION, *location);
}

void Component::zoom(float degree) {
	texture->zoom(degree);
	location->zoom(degree);
}
void Component::zoomColor(float degree) {
	texture->zoomColor(degree);
}