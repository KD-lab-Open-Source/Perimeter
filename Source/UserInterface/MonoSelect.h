#pragma once

#ifndef _MONOSELECT_H
#define _MONOSELECT_H

class MonoSelect {

	public:

		MonoSelect(int slotCount, int selectSize);

		void reset();
		int putPrevious(int slotNumber);
		int putNext(int slotNumber);
		void disable(int slotNumber);
		int enable(int slotNumber);
		int getPosition(int slotNumber);

	protected:
		bool hasPosition(int pos, int slotNumber);
		int cycle(int pos) {
			if (pos < 0) {
				pos += selectSize;
			} else if (pos >= selectSize) {
				pos -= selectSize;
			}
			return pos;
		}

		int selectSize;
		std::vector<int> slotPositions;

};

#endif //_MONOSELECT_H