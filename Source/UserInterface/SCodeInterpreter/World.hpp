#pragma once

#ifndef _WORLD_HPP
#define _WORLD_HPP

#include "SharedPointer.hpp"
#include "Object3D.hpp"

class World : public tx3d::SharedPointer {
	public:
		World(const string& sysName, const Vect3f& coords, int size, int filthLevel, float timeFactor);
		virtual ~World();
		void createAlphaPortal(World* worldWherefromPerforation);
		void createOmegaPortal();
		void destroyAlphaPortal();
		void destroyOmegaPortal();

		bool hasLinkWithWorld(World* world) const;
		static void linkWorlds(World* world1, World* world2);
		static void unlinkWorlds(World* world1, World* world2);

		void destroy();

		World* clone();

		int getConnectedToAlphaWorldCount();
		World* getConnectedToAlphaWorldByIndex(int i);
		int getConnectedToOmegaWorldCount();
		World* getConnectedToOmegaWorldByIndex(int i);

		void setWorld3D(Object3D* newWorld3D);
		Object3D* getWorld3D() {
			return world3D;
		}
		void setConnectedToOmegaLink3D(int index, Object3D* link3D);
		void setLastConnectedToOmegaLink3D(Object3D* link3D);
		Object3D* getConnectedToOmegaLink3DByIndex(int index);

		const Vect3f& getCoords() const {
			return coords;
		}
		int getSize() {
			return size;
		}
		bool isAlive() const {
			return alive;
		}
		bool hasAlphaPortal() const {
			return alphaPortal;
		}
		bool hasOmegaPortal() const {
			return omegaPortal;
		}
		float getTimeFactor() const {
			return timeFactor;
		}
		void setNewName(const string& newname) {
			name = newname;
		}
		const string& getName() const {
			return name;
		}
		const string& getSystemName() const {
			return sysName;
		}
		int getTestamentLevel() const {
			return testamentLevel;
		}
		void setTestamentLevel(int level) {
			testamentLevel = level;
		}

	protected:

		class Portal : public tx3d::SharedPointer {
			public:
				Portal(bool omegaPortal, World* ownerWorld);
				virtual ~Portal();
				void destroy();
				static void linkPortals(Portal* portal1, Portal* portal2);
				static void unlinkPortals(Portal* portal1, Portal* portal2);

				bool hasLinkWithPortal(Portal* portal) {
					return find(connectedPortals.begin(), connectedPortals.end(), portal) != connectedPortals.end();
				}
				bool isOmegaPortal() {
					return omegaPortal;
				}
				World* getOwnerWorld() {
					return ownerWorld;
				}
				int getConnectedPortalCount() {
					return connectedPortals.size();
				}
				Portal* getConnectedPortalByIndex(int i) {
					return connectedPortals[i];
				}
				void setLink3D(int i, Object3D* link) {
					if (links[i]) {
						links[i]->released();
					}
					links[i] = link;
					if (links[i]) {
						links[i]->referenced();
					}
				}
				void setLastLink3D(Object3D* link) {
					setLink3D(links.size() - 1, link);
				}
				Object3D* getLink3DByIndex(int i) {
					return links[i];
				}
			protected:
				void addPortal(Portal* portal);
				void removePortal(Portal* portal);
				bool omegaPortal;
				World* ownerWorld;
				vector<Portal*> connectedPortals;
				vector<Object3D*> links;
		};

		Object3D* world3D;

		bool alive;
		Portal* alphaPortal;
		Portal* omegaPortal;
		Vect3f coords;
		int size;
		int filthLevel;
		float timeFactor;
		string name;
		string sysName;
		int testamentLevel;

};




#endif //_WORLD_HPP
