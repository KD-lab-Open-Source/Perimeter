#pragma once

#ifndef _LOGICDATA_H
#define _LOGICDATA_H

//#include "../../Game/Runtime.h"
//#include "../PerimeterShellUI.h"
#include "IronFrame.h"

struct ComplexButtonData {
	ComplexButtonData() : phase(0), val(0), value(0), visible(false), enabled(false), checked(false) {
	}
	terUnitAttributeID id;
	bool enabled;
	bool visible;
	bool checked;
	float phase;
	bool paused;
	bool marked;
	bool statusNoEnergy;
	int val;
	int value;
	bool bCanBuildNow;
};

struct TerrainButtonData : public ComplexButtonData {
	TerrainButtonData() : productionPhase(0), unit(0), requestedAttr(-1) {
	}
	float productionPhase;
	bool isBrig;
	bool isInChanging;
	class terUnitReal* unit;
	int requestedAttr;
	vector<Vect3f> wayPoints;
};

struct AtomButtonData : public ComplexButtonData {
	AtomButtonData() : val(0), enabled(0) {
	}
	int enabled;
	bool bNoMutationEnergy;
	int val;
	bool left;
};

struct MutationButtonData : public ComplexButtonData {
	MutationButtonData() : count(0) {
	}
	terUnitAttributeID id;
	int count;
	bool append;
	int enabled;
	bool bS;
	bool bO;
	bool bT;
	bool bNoMutationEnergy;
};

struct SquadPageData {
	SquadPageData() : unitCount(0), mutationEnergy(0), visible(0), enabled(0), squad(0), hasAttackPoint(false) {
	}
	bool enabled;
	bool visible;
	MutationButtonData mutationButtons[UNIT_ATTRIBUTE_LEGIONARY_MAX - UNIT_ATTRIBUTE_SOLDIER - MUTATION_ATOM_MAX];
	AtomButtonData baseUnitsButton;
	AtomButtonData soldierButton;
	AtomButtonData officerButton;
	AtomButtonData technicButton;

	terUnitAttributeID tabIconID;
	int unitCount;

	float mutationEnergy;
	bool empty;
	class terUnitSquad* squad;
	vector<Vect2f> wayPoints;
	vector<Vect2f> patrolPoints;
	terUnitSquad::AttackPoint attackPoint;
	bool hasAttackPoint;
};

struct ProgressEnergyData {
	ProgressEnergyData() : produced(1), requested(1) {
	}
	float produced;
	float requested;
};

struct CollectedEnergyData : public ProgressEnergyData {
	CollectedEnergyData() : capacity(1), accumulated(1) {
	}
	float capacity;
	float accumulated;
};

struct ChargeData {
	ChargeData() : chargeLevel(0) {
	}
	float chargeLevel;
	bool isSpiralCharge;
};

struct SquadMiniMapData {
	Vect2f pos;
	float angle;
	sColor4f color;
};

struct MiniMapEvent {
	MiniMapEvent() : btnID(-1) {
	}
	MiniMapEventCode code;
	Vect2f pos;
	int btnID;
	int tabNumber;
};

struct LogicData {

	LogicData()	:	totalUnitCount(0),
					miniMap(0),
					speed(0),
					spiralLevel(0),
					energyProduced(1),
					energyRequested(1),
					energyCapacity(1),
					energyAccumulated(1),
					miniMapSquadCount(0),
					num_core(0),
					frame(0)
	{

		//memset(m_nProgress, 0, sizeof(int) * UNIT_ATTRIBUTE_STRUCTURE_MAX);//Плохой стиль
		miniMapSquads.resize(30);
		for (int i = 0; i < UNIT_ATTRIBUTE_STRUCTURE_MAX; i++) {
			sProgress& p = progress[i];
			p.progress = 0;
			p.hold = false;
			p.unit = NULL;
		}
	}	

	~LogicData() {
		if (miniMap) {
			delete miniMap;
		}
	}

	string activePlayerName;
	sColor4f activePlayerColor;

	SquadPageData squads[5];

	TerrainButtonData slots[FRAME_SLOTS_MAX];

	ProgressEnergyData progressEnergy;
	CollectedEnergyData collectedEnergy;
	ChargeData commonCharge;

	int totalUnitCount;

	bool soldPlantEnable;
	bool offPlantEnable;
	bool techPlantEnable;

	ComplexButtonData fieldOnBtn;
	ComplexButtonData fieldOffBtn;

	ComplexButtonData zeroToolzBtn;
	ComplexButtonData abyssToolzBtn;

	ComplexButtonData buildings[UNIT_ATTRIBUTE_HARKBACK_STATION3 - UNIT_ATTRIBUTE_CORE + 1];
	bool hasBuilding;
	ComplexButtonData guns[UNIT_ATTRIBUTE_STRUCTURE_MAX - UNIT_ATTRIBUTE_CORRIDOR_ALPHA];
	bool hasGun;

	float speed;

	ComplexButtonData frameInstallBtn;
	ComplexButtonData frameTeleportBtn;

	float spiralLevel;
	float energyProduced;
	float energyRequested;
	float energyCapacity;
	float energyAccumulated;

	struct sProgress
	{
		float progress;
		bool hold;
		class terBuilding* unit;
	};

	sProgress progress[UNIT_ATTRIBUTE_STRUCTURE_MAX];

	vector<SquadMiniMapData> miniMapSquads;
	int miniMapSquadCount;

	list<Vect2f> miniMapLabels;
	list<MiniMapEvent> miniMapEvents;

	list<Vect2f> alphaPath;

	class terFrame* frame;
	vector<Vect3f> frameWayPoints;

	int num_core;

	void createMiniMap(int width, int height) {
		if (miniMap) {
			delete miniMap;
		}
		miniMap = new sColor4c[ width * height ];
		sx = width;
		sy = height;
		clearMiniMap();
	}
	void clearMiniMap() {
		memset(miniMap, 0, sizeof(int) * sx * sy);
	}
	sColor4c* getMiniMap() {
		return miniMap;
	}
	float getWidth() {
		return sx;
	}
	float getHeight() {
		return sy;
	}
protected:
	sColor4c* miniMap;
	int sx;
	int sy;
};

#endif //_LOGICDATA_H
