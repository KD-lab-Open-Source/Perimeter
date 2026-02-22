#include <stdafx.h>
#include <algorithm>
#include <Clear3D.hpp>
#include <SphereLocation3D.hpp>
#include <PerTexture3D.hpp>
#include "SlotManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SlotManager::SlotManager() {
	currentAssembledTexture = 0;
}

SlotManager::~SlotManager() {
	if (currentAssembledTexture) {
		currentAssembledTexture->released();
	}
//	clearSlotList(painters);
//	clearSlotList(mixers);
}

Texture3D* SlotManager::assembleTexture() {
	Texture3D* result = 0;
	if (painters.size() > 0) {
		Texture3D* painterTexture = 0;
		if (painters.size() == 1) {
			painterTexture = painters.front()->getTexture();
		} else {
			Compound3D* painterCompound = new Compound3D();
			slotListToCompound3D(painters, painterCompound);
			painterTexture = painterCompound;
		}
		if (mixers.size() > 0) {
			PerTexture3D* perturbed = new PerTexture3D();
			perturbed->setCore(painterTexture);
			if (mixers.size() == 1) {
				perturbed->setPerturbator(mixers.front()->getTexture());
			} else {
				Compound3D* mixerCompound = new Compound3D();
				slotListToCompound3D(mixers, mixerCompound);
				perturbed->setPerturbator(mixerCompound);
			}
			result = perturbed;
		} else {
			result = painterTexture;
		}

		if (result) {
			result->referenced();
			if (currentAssembledTexture) {
				currentAssembledTexture->released();
			}
			currentAssembledTexture = result;
		}
	}
	return result;
}

bool SlotManager::parseTexture(Texture3D* texture, cScene* scene) {
	if (texture) {
		list<Slot*> newPainters;
		list<Slot*> newMixers;

		if (texture->getType() == Compound3D::TYPE_NAME) {
			Compound3D* compound = dynamic_cast<Compound3D*>(texture);
			if ( !compound3DToSlotList(compound, newPainters, scene) ) {
				return false;
			}
		} else if (texture->getType() == PerTexture3D::TYPE_NAME) {
			PerTexture3D* perturbed = dynamic_cast<PerTexture3D*>(texture);
			if ( !texture3DToSlotList(perturbed->getCore(), newPainters, scene) ) {
				return false;
			}
			if ( !texture3DToSlotList(perturbed->getPerturbator(), newMixers, scene) ) {
				return false;
			}
		} else {
			newPainters.push_back( new Slot(texture, new SphereLocation3D(), scene) );
		}

		clearSlotList(painters);
		clearSlotList(mixers);
		addPainters(newPainters);
		addMixers(newMixers);

		if (currentAssembledTexture) {
			currentAssembledTexture->released();
		}
		texture->referenced();
		currentAssembledTexture = texture;
		return true;
	} else {
		return false;
	}
}

Slot* SlotManager::addNewPainter(cScene* scene) {
	Slot* res = new Slot(new Clear3D(), new WorldLocation3D(10.0f), scene);
	addPainter(res);
	return res;
}

Slot* SlotManager::addNewMixer(cScene* scene) {
	Slot* res = new Slot(new Clear3D(), new WorldLocation3D(10.0f), scene);
	addMixer(res);
	return res;
}

void SlotManager::addPainter(Slot* painter) {
	if (painter) {
		painter->referenced();
		painters.push_back(painter);
	}
}

void SlotManager::addMixer(Slot* mixer) {
	if (mixer) {
		mixer->referenced();
		mixers.push_back(mixer);
	}
}

void SlotManager::removePainter(Slot* painter) {
	if ( find(painters.begin(), painters.end(), painter) != painters.end() ) {
		painter->released();
		painters.remove(painter);
	}
}

void SlotManager::removeMixer(Slot* mixer) {
	if ( find(mixers.begin(), mixers.end(), mixer) != mixers.end() ) {
		mixer->released();
		mixers.remove(mixer);
	}
}

int SlotManager::getPainterCount() {
	return painters.size();
}

int SlotManager::getMixerCount() {
	return mixers.size();
}

void SlotManager::addPainters( list<Slot*>& slotList ) {
	list <Slot*>::iterator it;
	for (it = slotList.begin(); it != slotList.end(); it++) {
		addPainter( (*it) );
	}
}

void SlotManager::addMixers( list<Slot*>& slotList ) {
	list <Slot*>::iterator it;
	for (it = slotList.begin(); it != slotList.end(); it++) {
		addMixer( (*it) );
	}
}

Slot* SlotManager::getPainter(int index) {
	list <Slot*>::iterator it = painters.begin();
	advance(it, index);
	return *(it);
}

Slot* SlotManager::getMixer(int index) {
	list <Slot*>::iterator it = mixers.begin();
	advance(it, index);
	return *(it);
}

void SlotManager::moveToPainters(Slot* mixer) {
	if ( find(mixers.begin(), mixers.end(), mixer) != mixers.end() ) {
		mixers.remove(mixer);
		painters.push_back(mixer);
		//*256
	}
}

void SlotManager::moveToMixers(Slot* painter) {
	if ( find(painters.begin(), painters.end(), painter) != painters.end() ) {
		painters.remove(painter);
		mixers.push_back(painter);
		//*256
	}
}

void SlotManager::clearSlotList( list<Slot*>& slotList ) {
	list <Slot*>::iterator it;
	for (it = slotList.begin(); it != slotList.end(); it++) {
		(*it)->released();
	}
	slotList.clear();
}

void SlotManager::slotListToCompound3D( list<Slot*>& slotList, Compound3D* compound ) {
	list <Slot*>::iterator it;
	for (it = slotList.begin(); it != slotList.end(); it++) {
		compound->addTexture3D( (*it)->getTexture(), false, (*it)->getLocation() );
	}
}

bool SlotManager::compound3DToSlotList( Compound3D* compound, list<Slot*>& slotList, cScene* scene ) {
	int componentCount = compound->getComponentCount();
	for (int i = 0; i < componentCount; i++) {
		Component* component = compound->getComponent(i);
		if ( !isTextureSimple(component->getTexture()) ) {
			return false;
		}
		slotList.push_back( new Slot(component, scene) );
	}
	return true;
}

bool SlotManager::isTextureSimple(Texture3D* texture) {
	return 
		   texture->getType() != Compound3D::TYPE_NAME
		&& texture->getType() != PerTexture3D::TYPE_NAME;
}

bool SlotManager::texture3DToSlotList( Texture3D* texture, list<Slot*>& slotList, cScene* scene ) {
	if (texture->getType() == PerTexture3D::TYPE_NAME) {
		return false;
	} else if (texture->getType() == Compound3D::TYPE_NAME) {
		Compound3D* compound = dynamic_cast<Compound3D*>(texture);
		if ( !compound3DToSlotList(compound, slotList, scene) ) {
			return false;
		}
	} else {
		slotList.push_back( new Slot(texture, new SphereLocation3D(), scene) );
	}
	return true;
}

