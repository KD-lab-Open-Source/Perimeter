
#include "StdAfx.h"
#include "Frame.hpp"

Frame::Frame(const string& name, World* parentWorld, int race, int colorIndex) : name(name), race(race), colorIndex(colorIndex) {
	alive = true;
	frame3D = 0;
	goToWorld(parentWorld);
}
Frame::~Frame() {
	destroy();
	if (frame3D) {
		frame3D->released();
	}
}
void Frame::addKnowledge(const Knowledge& newKnowledge) {
	knowledge.addKnowledge(newKnowledge);
}
void Frame::goToWorld(World* destWorld) {
	knowledge.worldVisited(destWorld);
}
void Frame::setFrame3D(Object3D* newFrame3D) {
	if (frame3D) {
		frame3D->released();
	}
	frame3D = newFrame3D;
	if (frame3D) {
		frame3D->referenced();
	}
}
