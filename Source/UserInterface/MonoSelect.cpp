#include "StdAfx.h"
#include "MonoSelect.h"

MonoSelect::MonoSelect() {
	reset(0, 0);
}

void MonoSelect::reset(int slotCount, int _selectSize) {
    this->selectSize = _selectSize;
    resize(slotCount);
	for (size_t i = 0, s = slotPositions.size(); i < s; i++) {
		slotPositions[i] = -1;
	}
}

void MonoSelect::resize(int slotCount) {
    slotCount = std::min(slotCount,selectSize);
    size_t oldSize = slotPositions.size();
    slotPositions.resize(slotCount);
    for (size_t i = oldSize, s = slotPositions.size(); i < s; i++) {
        slotPositions[i] = -1;
    }
}

int MonoSelect::putPrevious(int slotNumber) {
    if (slotNumber >= slotPositions.size()) return -1;
	int res = slotPositions[slotNumber];

	for (int p = (selectSize - 1); p >= 0; p--) {
		int testPos = cycle(p - selectSize + res);
		if ( !hasPosition(testPos, slotNumber) ) {
			res = testPos;
			break;
		}
	}

	slotPositions[slotNumber] = res;
	return res;
}

int MonoSelect::putNext(int slotNumber) {
    if (slotNumber >= slotPositions.size()) return -1;
	int res = slotPositions[slotNumber];

	for (int p = 0; p < selectSize; p++) {
		int testPos = cycle(p + res + 1);
		if ( !hasPosition(testPos, slotNumber) ) {
			res = testPos;
			break;
		}
	}

	slotPositions[slotNumber] = res;
	return res;
}

int MonoSelect::getPosition(int slotNumber) {
    if (slotNumber >= slotPositions.size()) return -1;
	return slotPositions[slotNumber];
}

bool MonoSelect::setPosition(int slotNumber, int pos) {
    if (slotNumber >= slotPositions.size() || hasPosition(pos, slotNumber)) return false;
    slotPositions[slotNumber] = pos;
    return true;
}

bool MonoSelect::hasPosition(int pos, int slotNumber) {
	for (size_t i = 0, s = slotPositions.size(); i < s; i++) {
		if ( i != slotNumber && slotPositions[i] == pos ) {
			return true;
		}
	}
	return false;
}

void MonoSelect::disable(int slotNumber) {
    if (slotNumber >= slotPositions.size()) return;
	slotPositions[slotNumber] = -1;
}

int MonoSelect::enable(int slotNumber) {
	return putNext(slotNumber);
}
