#include <stdafx.h>
#include "LocationNode.h"
#include "Slot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Slot::Slot(Texture3D* texture, Location3D* lc, cScene* scene)
					: texture(texture) {

	texture->referenced();
	location = 0;
	locationObject = 0;
	setLocation(lc, scene);
}

Slot::Slot(Component* component, cScene* scene) {
	texture = component->getTexture();
	texture->referenced();
	location = 0;
	locationObject = 0;
	setLocation(component->getLocation(), scene);
}

Slot::~Slot() {
	if (texture) {
		texture->released();
	}
	if (location) {
		location->released();
	}
	RELEASE(locationObject);
}

void Slot::setTexture(Texture3D* tx) {
	if (texture) {
		texture->released();
	}
	if (tx) {
		tx->referenced();
	}
	texture = tx;
}

void Slot::setLocation(Location3D* lc, cScene* scene) {
	if (location) {
		location->released();
	}
	if (lc) {
		lc->referenced();
	}
	location = lc;
	updateLocationObject(scene);
}

void Slot::updateLocationObject(cScene* scene) {
	RELEASE(locationObject);
	if (location) {
		locationObject = LocationNode::createLocationObject(location, scene);
	}
}
