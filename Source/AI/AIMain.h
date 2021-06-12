#ifndef __AIMAIN_H__
#define __AIMAIN_H__

#include "GenericControls.h"
#include "AITileMap.h"
#include "Universe.h"
#include "Player.h"

class PlaceScanOp;

class AIPlayer : public terPlayer
{
public:
	AIPlayer(const PlayerData& player_data);
	~AIPlayer();
	
	int ID() const { return m_ID; }
	void setAI(bool isAI);

	void addUnit(terUnitBase* p);
	void removeUnit(terUnitBase* p);
	void Quant();
	void triggerQuant();
	void checkEvent(const Event& event);

	void TerrainWorkingComplete(int x,int y,int sx,int sy);

	Vect2f GetBrigadierCenter();

	void orderBuilding(int priority, ActionOrderBuilding* action);
	void applyRegionChanges();

	void installFrame();
	bool installFrameQuant();

	terUnitSquad* chooseSquad(ChooseSquadID chooseSquadID);

	void setAutomaticUsingField(int duration, bool onlyIfCoreDamaged) { automaticUsingFieldTimer_.start(duration); onlyIfCoreDamaged_ = onlyIfCoreDamaged; }

	void checkLevelingAreaQuant();

protected:
	enum BuilderState
	{
		BuildingIdle,
		FindingWhereToDig, 
		FoundWhereToDig, 
		FailedToFoundWhereToDig,
		Digging, 
		DiggingCompleted,
		FailedToDig,
		BuildingPause,
		UnableToFindWhereToDig
	};

	struct Interval
	{
		short xl, xr;
		Interval(){}
		Interval(short xl_, short xr_) : xl(xl_), xr(xr_) {}
	};
	
	struct CircleShape : vector<Interval>
	{
		int radius;
		CircleShape() : radius(0) {}
		void make(int radius);
	};

	struct Circle : Vect2i
	{
		int radius;
		float effectiveness; 

		Circle(){ invalidate(); }
		Circle(const Vect2i& v, int r) : Vect2i(v), radius(r) { invalidate(); }
		void invalidate() { effectiveness = 0; }
		bool invalid() const { return effectiveness == 0; }
	};
	
	struct ClearRegion
	{
		enum State
		{
			Initial,
			Completed
		};
		
		State state;
		
		ClearRegion() : state(Initial) {}
	};
	typedef Map2D<ClearRegion, AITile::tile_size> ClearMap;
	ClearMap clear_map;

	UnitList BrigadierList;
	
	typedef map<int, ShareHandle<ActionOrderBuilding> > OrderBuildingActionMap;
	OrderBuildingActionMap orderBuildingActions;
	ShareHandle<ActionOrderBuilding> currentOrder;

	int requiredBuilders_; // Количество строителей
	int requiredSoldiers_; // Количество солдат
	int requiredOfficers_; // Количество офицеров
	int requiredTechnics_; // Количество техников
	
	BuilderState builder_state;
	CircleShape circle_shape;

	DamageMolecula required_atoms;

	// Buildings placement
	Vect2i placement_coords;
	Vect2i scanMin_, scanMax_; // world's scale
	int scanStep_;
	class terBuildingInstaller* building_installer;
	PlaceScanOp* place_scan_op;
	Vect2f best_position;
	DurationTimer building_pause;

	bool installingFrame_;
	DurationTimer automaticUsingFieldTimer_;
	bool onlyIfCoreDamaged_;

	LogStream* aiLog_;

	int m_ID;
	static int IDs;
	
	////////////////////////
	// Members
	int BuildingsUnderConstruction();
	PlaceScanOp* WhatToBuild();
	void BuildingQuant();

	void placeBuilding(PlaceScanOp* scan_op);
	void startDigging(RegionDispatcher* region_disp);
	void finishPlacement();
	void clearPlaceOp();

	const Vect2f& bestPosition() const { return best_position; }

	int calcWork(const Vect2i& left_top, const Vect2i& size);
	void changeTileState(int x,int y);//x,y в масштабе сетки

	//----------------------
	int calcWork(Circle& c);
	void startPlace(PlaceScanOp* scan_op, const Vect2i& scan_min_, const Vect2i& scan_max_, int scan_step_);
	void findWhereToDigQuant();

	void MoveBrigadiersToPoint(const Vect2f& pos,float radius);

	void FrameQuant();

	void showDebugInfo();

	////////////////////////
	//		Friends
	friend class AITileMap;
	friend PlaceScanOp;
	friend CircleShape;
	friend struct AIRegionOrderOperation;
};

#endif //__AIMAIN_H__