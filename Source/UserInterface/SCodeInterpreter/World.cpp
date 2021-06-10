
#include "StdAfx.h"
#include "World.hpp"

World::World(const string& sysName, const Vect3f& coords, int size, int filthLevel, float timeFactor)
	: sysName(sysName),
	coords(coords),
	size(size),
	filthLevel(filthLevel),
	timeFactor(timeFactor) {

	alive = true;
	name = sysName;
	alphaPortal = 0;
	omegaPortal = 0;
	world3D = 0;
	testamentLevel = 0;
}
World::~World() {
	destroy();
	if (world3D) {
		world3D->released();
	}
}

World* World::clone() {
	World* w = new World(sysName, coords, size, filthLevel, timeFactor);
	w->alive = alive;
	if (hasAlphaPortal()) {
		w->createAlphaPortal(0);
	}
	if (hasOmegaPortal()) {
		w->createOmegaPortal();
	}
	return w;
}

void World::createAlphaPortal(World* worldWherefromPerforation) {
	if (!hasAlphaPortal()) {
		alphaPortal = new Portal(false, this);
		alphaPortal->referenced();
	}
	if (worldWherefromPerforation) {
		linkWorlds(this, worldWherefromPerforation);
	}
}
void World::createOmegaPortal() {
	if (!hasOmegaPortal()) {
		omegaPortal = new Portal(true, this);
		omegaPortal->referenced();
	}
}
void World::destroyAlphaPortal() {
	if (hasAlphaPortal()) {
		alphaPortal->destroy();
		alphaPortal->released();
//		cout << endl << (alphaPortal->isOmegaPortal() ? "omega " : "alpha ") << alphaPortal->getOwnerWorld()->getSystemName() << endl;
		alphaPortal = 0;
	}
}
void World::destroyOmegaPortal() {
	if (hasOmegaPortal()) {
		omegaPortal->destroy();
		omegaPortal->released();
//		cout << endl << (omegaPortal->isOmegaPortal() ? "omega " : "alpha ") << omegaPortal->getOwnerWorld()->getSystemName() << endl;
		omegaPortal = 0;
	}
}
void World::destroy() {
	destroyAlphaPortal();
	destroyOmegaPortal();
	alive = false;
}

bool World::hasLinkWithWorld(World* world) const {
	return 
		(		hasAlphaPortal() 
			&&	world->hasOmegaPortal()
			&&	alphaPortal->hasLinkWithPortal(world->omegaPortal)	)
		|| (	hasOmegaPortal() 
			&&	world->hasAlphaPortal()
			&&	omegaPortal->hasLinkWithPortal(world->alphaPortal)	);
}

void World::linkWorlds(World* world1, World* world2) {
	if (world1->hasAlphaPortal() && world2->hasOmegaPortal()) {
		Portal::linkPortals(world1->alphaPortal, world2->omegaPortal);
	} else if (world2->hasAlphaPortal() && world1->hasOmegaPortal()) {
		Portal::linkPortals(world2->alphaPortal, world1->omegaPortal);
	}
}
void World::unlinkWorlds(World* world1, World* world2) {
	if (
			world1->hasAlphaPortal() 
		&&	world2->hasOmegaPortal()
		&&	world1->alphaPortal->hasLinkWithPortal(world2->omegaPortal) ) {

		Portal::unlinkPortals(world1->alphaPortal, world2->omegaPortal);
	}
	if (
			world2->hasAlphaPortal() 
		&&	world1->hasOmegaPortal()
		&&	world2->alphaPortal->hasLinkWithPortal(world1->omegaPortal) ) {
		Portal::unlinkPortals(world2->alphaPortal, world1->omegaPortal);
	}
}

int World::getConnectedToAlphaWorldCount() {
	if (hasAlphaPortal()) {
		return alphaPortal->getConnectedPortalCount();
	} else {
		return 0;
	}
}
World* World::getConnectedToAlphaWorldByIndex(int i) {
	return alphaPortal->getConnectedPortalByIndex(i)->getOwnerWorld();
}
int World::getConnectedToOmegaWorldCount() {
	if (hasOmegaPortal()) {
		return omegaPortal->getConnectedPortalCount();
	} else {
		return 0;
	}
}
World* World::getConnectedToOmegaWorldByIndex(int i) {
	return omegaPortal->getConnectedPortalByIndex(i)->getOwnerWorld();
}

void World::setWorld3D(Object3D* newWorld3D) {
	if (world3D) {
		world3D->released();
	}
	world3D = newWorld3D;
	if (world3D) {
		world3D->referenced();
	}
}
void World::setConnectedToOmegaLink3D(int index, Object3D* newLink3D) {
	omegaPortal->setLink3D(index, newLink3D);
}
void World::setLastConnectedToOmegaLink3D(Object3D* newLink3D) {
	omegaPortal->setLastLink3D(newLink3D);
}

Object3D* World::getConnectedToOmegaLink3DByIndex(int index) {
	return omegaPortal->getLink3DByIndex(index);
}

//----------------Portal------------------

World::Portal::Portal(bool isOmegaPortal, World* ownerWorld)
	 : omegaPortal(isOmegaPortal), ownerWorld(ownerWorld) {
	//reserve
}
World::Portal::~Portal() {
	destroy();
}
void World::Portal::linkPortals(Portal* portal1, Portal* portal2) {
	portal1->addPortal(portal2);
	portal2->addPortal(portal1);
}
void World::Portal::unlinkPortals(Portal* portal1, Portal* portal2) {
	portal1->removePortal(portal2);
	portal2->removePortal(portal1);
}
void World::Portal::addPortal(Portal* portal) {
	connectedPortals.push_back(portal);
	portal->referenced();
	links.push_back(0);
}
void World::Portal::removePortal(Portal* portal) {
	vector <Portal*>::iterator it = connectedPortals.begin();
	vector <Object3D*>::iterator itLink = links.begin();
	for (; it != connectedPortals.end(); it++, itLink++) {
		if ((*it) == portal) {
			break;
		}
	}
//	vector <Portal*>::iterator it = find(connectedPortals.begin(), connectedPortals.end(), portal);
	if ( it	!=	connectedPortals.end() ) {
		if ((*itLink)) {
			(*itLink)->released();
		}
		links.erase(itLink);
		connectedPortals.erase(it);
		portal->released();
//		cout << endl << "remove " << (portal->isOmegaPortal() ? "omega " : "alpha ") << portal->getOwnerWorld()->getSystemName() << endl;
	}
}
void World::Portal::destroy() {
//	for (int j = 0, sj = connectedPortals.size(); j < sj; j++) {
//		cout << endl << (connectedPortals[j]->isOmegaPortal() ? "omega " : "alpha ") << connectedPortals[j]->getOwnerWorld()->getSystemName() << endl;
//	}
	for (int i = 0, s = connectedPortals.size(); i < s; i++) {
		connectedPortals[i]->removePortal(this);
		connectedPortals[i]->released();
		if (links[i]) {
			links[i]->released();
		}
	}
	connectedPortals.clear();
	links.clear();
}
