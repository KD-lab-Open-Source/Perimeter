#include "StdAfx.h"
#include "MonoSelect.h"

MonoSelect::MonoSelect(int slotCount, int selectSize) : selectSize(selectSize) {
	xassert(slotCount <= selectSize);
	slotPositions.resize(slotCount);
	reset();
}

void MonoSelect::reset() {
	for (int i = 0, s = slotPositions.size(); i < s; i++) {
		slotPositions[i] = -1;
	}
}

int MonoSelect::putPrevious(int slotNumber) {
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
	return slotPositions[slotNumber];
}

bool MonoSelect::hasPosition(int pos, int slotNumber) {
	for (int i = 0, s = slotPositions.size(); i < s; i++) {
		if ( i != slotNumber && slotPositions[i] == pos ) {
			return true;
		}
	}
	return false;
}

void MonoSelect::disable(int slotNumber) {
	slotPositions[slotNumber] = -1;
}

int MonoSelect::enable(int slotNumber) {
	return putNext(slotNumber);
}
