
#include "StdAfx.h"
#include "Containers.hpp"

Worlds::~Worlds() {
	clear();
}
void Worlds::addNewWorld(World* world) {
	world->referenced();
	worlds[world->getSystemName()] = world;
}
World* Worlds::getWorldBySysName(const std::string& sysName) {
	std::map<std::string, World*>::iterator it = worlds.find(sysName);
	return it != worlds.end() ? it->second : 0;
}
void Worlds::clear() {
	std::map <std::string, World*>::iterator it;
	std::map <std::string, World*>::iterator end = worlds.end();
	for (it = worlds.begin(); it != end; it++) {
		it->second->released();
	}
	worlds.clear();
}

Frames::~Frames() {
	clear();
}
void Frames::addNewFrame(Frame* frame) {
	frame->referenced();
	frames[frame->getName()] = frame;
}
Frame* Frames::getFrameByName(const std::string& name) {
	return frames.find(name)->second;
}
void Frames::clear() {
	std::map <std::string, Frame*>::iterator it;
	std::map <std::string, Frame*>::iterator end = frames.end();
	for (it = frames.begin(); it != end; it++) {
		it->second->released();
	}
	frames.clear();
}

int Frames::knowledgeArrivedToWorld(const Knowledge& knowledge, World* world) {
	int res = 0;
	std::map <std::string, Frame*>::iterator it;
	std::map <std::string, Frame*>::iterator end = frames.end();
	for (it = frames.begin(); it != end; it++) {
		if (it->second->getCurrentWorld() == world) {
			it->second->addKnowledge(knowledge);
			res++;
		}
	}
	return res;
}
