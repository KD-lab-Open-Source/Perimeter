
#include "StdAfx.h"
#include "Knowledge.hpp"

Knowledge::Knowledge() {
	//reserve
}

Knowledge::Knowledge(const Knowledge& origin) {
	//reserve
	addKnowledge(origin);
}

Knowledge::~Knowledge() {
	set <World*>::iterator sit;
	set <World*>::iterator end = knownWorlds.end();
	for (sit = knownWorlds.begin(); sit != end; sit++) {
		(*sit)->released();
	}
	for (int i = 0, s = path.size(); i < s; i++) {
		path[i]->released();
	}
}

void Knowledge::worldVisited(World* world) {
	if (knownWorlds.find(world) == knownWorlds.end()) {
		world->referenced();
		knownWorlds.insert(world);
	}
	world->referenced();
	path.push_back(world);
}

void Knowledge::addKnowledge(const Knowledge& anotherKnowledge) {
	set <World*>::const_iterator it;
	set <World*>::const_iterator anotherBegin = anotherKnowledge.knownWorlds.begin();
	set <World*>::const_iterator anotherEnd = anotherKnowledge.knownWorlds.end();
	set <World*>::const_iterator end = knownWorlds.end();
	for (it = anotherBegin; it != anotherEnd; it++) {
		if (knownWorlds.find(*it) == end) {
			(*it)->referenced();
		}
	}
	knownWorlds.insert(anotherBegin, anotherEnd);
}
