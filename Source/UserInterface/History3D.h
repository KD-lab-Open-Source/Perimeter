#pragma once

#ifndef _HISTORY3D_H
#define _HISTORY3D_H

#include "SCodeInterpreter/Object3D.hpp"
#include "Silicon.h"

class Frame3D : public Object3D {
	public:
		Frame3D(cScene* scene, int race) : race(race), frameObj(0), flareEffectTime(0), hasPlumeEffect(false), shouldStartFlareEffect(false), alive(true) {
			createObj(scene);
		}
		~Frame3D() {
			if (frameObj) {
				frameObj->Release();
				frameObj = 0;
			}
		}
		int getState() const {
			return alive ? 1 : 0;
		}
		bool setState(int newState) {
			alive = (newState == 1);
			return false;
		}
		void quant(float dt);
		void updateColor(const Vect3f& cameraPos);
		cObjectNodeRoot* getFrameObj() {
			return frameObj;
		}

		void setRace(cScene* scene, int newRace);

		void tryFlareEffect();
		void setPlumeEffect(bool on);

		bool hasPlumeEffect;
		bool shouldStartFlareEffect;

	protected:
		void createObj(cScene* scene);

		cObjectNodeRoot* frameObj;
		float flareEffectTime;
		bool alive;

		int race;

};

class WorldSphere : public Object3D {
	public:
		enum Type {
			UNKNOWN = 0,
			FULL,
			ALPHA_EXPEDITION,
			DEAD_END,
			VISITED,
			MISSIONED,
			MAX_TYPE
		};
		WorldSphere(ElasticSphere* elasticSphere, bool alive) : elasticSphere(elasticSphere) {
			elasticSphere->IncRef();
			state = -1;
			timeEvolvingElapsed = 0;
			timeElapsed = 0;
			sphereColor = sColor4f(1, 1, 1, 1);
			frameCount = 0;
			timeTestamentRemaining = 0;
			timeDissolveRemaining = alive ? -1 : 0;
		}
		~WorldSphere() {
			elasticSphere->Release();
		}
		ElasticSphere* getElasticSphere() {
			return elasticSphere;
		}
		int getState() const {
			return state;
		}
		void quant(float dt);
		bool setState(int newState);
		void updateColor(const Vect3f& cameraPos);
		sColor4f& color() {
			return sphereColor;
		}
		void resetFrameCount() {
			frameCount = 0;
		}
		void setupFrame3D(Frame3D* frame3D);
		void startTestamentEffect(int level);

		void setDead();

	protected:
		ElasticSphere* elasticSphere;
		int state;
		float timeEvolvingElapsed;
		sColor4f sphereColor;
		int frameCount;
		float timeElapsed;
		float timeTestamentRemaining;
		float timeDissolveRemaining;
		float radius;
};

class LinkTubule : public Object3D {
	public:
		enum Type {
			SELECTED = 0,
			NORMAL, 
			BLINKING
		};
		LinkTubule(ElasticLink* elasticLink) : elasticLink(elasticLink) {
			elasticLink->IncRef();
			state = -1;
			timeEvolvingElapsed = 0;
			timeElapsed = 0;
		}
		~LinkTubule() {
			elasticLink->Release();
		}
		ElasticLink* getElasticLink() {
			return elasticLink;
		}
		int getState() const {
			return state;
		}
		void quant(float dt);
		bool setState(int newState);
		void updateColor(const Vect3f& cameraPos);
	
	protected:
		ElasticLink* elasticLink;
		int state;
		float timeEvolvingElapsed;
		float timeElapsed;
};


#endif //_HISTORY3D_H