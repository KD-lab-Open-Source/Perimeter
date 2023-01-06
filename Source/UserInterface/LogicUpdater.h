#pragma once

#ifndef _LOGICUPDATER_H
#define _LOGICUPDATER_H

#include "Triggers.h"
//#include "../../Game/Runtime.h"
//#include "../PerimeterShellUI.h"

#include "LogicData.h"

class LogicUpdater {
	public:
		LogicUpdater();
		~LogicUpdater();

		void update();

		void exchange();

		void updateIconsData();

		void updateMiniMap();
		void updateMiscData();
		void updateBuildingsData();
		void updateGunsData();
		void updateSlotsData();
		void updateSquadsData();

		void checkEvent(const Event* event);

		void lock();
		void unlock();

		void reset();

		LogicData* getLogicData() {
			return oldLogicData;
		}

	protected:

		void terMapClusterLine(int yMap, int x_from, int x_to, const sColor4f& color);
		void terMapClusterPoint(int yMap, int xMap, const sColor4c& color);
		void terMapClusterRect(int xMap1, int yMap1, int xMap2, int yMap2, const sColor4c& color);

		MTDECLARE(lockSect);
		LogicData* logicData;
		LogicData* oldLogicData;
};

#endif //_LOGICUPDATER_H
