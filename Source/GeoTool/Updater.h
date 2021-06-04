#ifndef _UPDATER_H
#define _UPDATER_H

#include "Rect4i.h"

class Updater {
	public:
		virtual ~Updater() {
		}
		virtual void updateRect(const Rect4i& rect) = 0;
		virtual void updateRect(int xMin, int yMin, int xMax, int yMax) = 0;
};

#endif // _UPDATER_H