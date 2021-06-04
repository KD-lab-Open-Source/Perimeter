#include "StdAfx.h"

#include "umath.h"
#include "IVisGeneric.h"
#include "VisGenericDefine.h"
#include "IRenderDevice.h"

#include "terra.h"

#include "Runtime.h"

#include "Region.h"

#include "GenericControls.h"
#include "Universe.h"

#include "AiMain.h"
#include "Installer.h"
#include "RigidBody.h"
#include "AiPrm.h"
#include "PlaceOperators.h"
#include "Triggers.h"


////////////////////////////////////////////
//  AIPlayer building functions
////////////////////////////////////////////
void AIPlayer::orderBuilding(int priority, ActionOrderBuilding* action)
{
	orderBuildingActions.insert(OrderBuildingActionMap::value_type(priority, action));
}

PlaceScanOp* AIPlayer::WhatToBuild()
{
	if(orderBuildingActions.empty())
		return 0;

	currentOrder = orderBuildingActions.begin()->second;
	currentOrder->setAccepted();
	return new PlaceScanOp(currentOrder->building, *this, aiPrm.find(currentOrder->placementStrategy), currentOrder->buildDurationMax*1000);
}

static void log(const char* name) 
{
	static int prevTime;

	if(name)
		fout < name < ": " <= global_time() - prevTime < "\n";
	prevTime = global_time();
}

void AIPlayer::BuildingQuant()
{
	start_timer_auto(BuildingQuant, STATISTICS_GROUP_AI);

	//watch_i(builder_state, ID());
	//watch_i(totalWork(), ID());

	switch(builder_state){
	case BuildingIdle:	
		if(BuildingsUnderConstruction() < 3){
			PlaceScanOp* scanOp = WhatToBuild();
			if(scanOp)// && scanOp->attributeID() != building_installer->attributeID())
				placeBuilding(scanOp);
			else{
				delete scanOp;
				building_pause.start(ai_building_pause);
				builder_state = BuildingPause;
			}
		}
		break;

	case FoundWhereToDig:
		if(!BrigadierList.empty()){
			MetaRegionLock lock(RegionPoint);
			startDigging(ZeroRegionPoint.data());
			applyRegionChanges();
			MoveBrigadiersToPoint(bestPosition(), ai_brigadier_move_to_point_offset);
		}
		break;

	case DiggingCompleted:
		finishPlacement();
		checkEvent(EventPlayerState(PLAYER_STATE_UNABLE_TO_PLACE_CORE, false));
		checkEvent(EventPlayerState(PLAYER_STATE_UNABLE_TO_PLACE_BUILDING, false));
		break;

	case UnableToFindWhereToDig:
		//xassert(0);
		//placeSecond(new PlaceScanOp(UNIT_ATTRIBUTE_CORE, *this, aiPrm.placeCorePrm));
		if(place_scan_op->generateUnableToPlaceEvent())
			checkEvent(EventPlayerState(place_scan_op->attributeID() == UNIT_ATTRIBUTE_CORE ? 
				PLAYER_STATE_UNABLE_TO_PLACE_CORE : PLAYER_STATE_UNABLE_TO_PLACE_BUILDING, true));
		clearPlaceOp();
		break;

	case FindingWhereToDig:
		findWhereToDigQuant();
		break;

	case Digging: {
		Vect2f& position = place_scan_op->bestPosition();
		building_installer->SetBuildPosition(To3D(position), 0, 0);
		if(building_installer->valid())
			builder_state = DiggingCompleted;
		else if(building_installer->buildingInArea())
			builder_state = UnableToFindWhereToDig;
		break;
	}

	case BuildingPause:
		if(!building_pause)
			builder_state = BuildingIdle;
		break;
	}
}	

void AIPlayer::applyRegionChanges()
{
	RasterizeRegion();
	if(active())
		*universe()->activeRegionDispatcher() = *RegionPoint;
}

void AIPlayer::installFrame()
{
	if(!frame())
		return;

	MetaRegionLock lock(RegionPoint);
	ZeroRegionPoint->operateByCircle(frame()->position2D(), frame()->attr().ZeroLayerRadius, 1);
	ZeroRegionPoint->postOperateAnalyze();
	applyRegionChanges();

	MoveBrigadiersToPoint(frame()->position2D() + Vect2f(moveBrigadiersToInstallFrameOffsetFactor)*frame()->radius(), ai_brigadier_move_to_point_offset);
	installingFrame_ = true;
}

bool AIPlayer::installFrameQuant()
{
	if(!frame())
		return true;

	if(installingFrame_ && frame()->basementReady() && frame()->GetRigidBodyPoint()->velocity().norm() < 0.1f){
		frame()->executeCommand(UnitCommand(COMMAND_ID_FRAME_ATTACH, 0));
		installingFrame_ = false;
		return true;
	}
	return false;
}

void AIPlayer::placeBuilding(PlaceScanOp* scan_op)
{
	int offset = vMap.m2w(ai_border_offset);
	startPlace(scan_op, Vect2i(offset, offset), Vect2i((int)vMap.H_SIZE - offset, (int)vMap.V_SIZE - offset), ai_scan_step);
	//log((char*)0);
}

void AIPlayer::startPlace(PlaceScanOp* scan_op, const Vect2i& scanMin, const Vect2i& scanMax, int scanStep)
{	
	if(!scan_op)
		return;
		
	builder_state = FindingWhereToDig;
	place_scan_op = scan_op;
	
	scanStep_ = scanStep;
	scanMin_ = scanMin;
	scanMax_ = scanMax;
	placement_coords = scanMin_ - Vect2i(scanStep_, 0);
}

void AIPlayer::findWhereToDigQuant()
{
	start_timer_auto(findWhereToDigQuant, STATISTICS_GROUP_AI);
	for(int i = 0; i < ai_placement_iterations_per_quant; i++) {
		if((placement_coords.x += scanStep_) >= scanMax_.x) {
			placement_coords.x = scanMin_.x;
			if((placement_coords.y += scanStep_) >= scanMax_.y) {
				if(place_scan_op->found()) {
					if(scanStep_ > ai_scan_step_min) {
						Vect2i pos = place_scan_op->bestPosition(); 
						Vect2i off(round(ai_scan_size_of_step_factor*scanStep_), round(ai_scan_size_of_step_factor*scanStep_));
						startPlace(place_scan_op, pos - off, pos + off, scanStep_/2);
					}
					else{
						builder_state = FoundWhereToDig;
						//log("FoundWhereToDig");
						best_position = place_scan_op->bestPosition();
					}
				}
				else{
					if(scanStep_ > ai_scan_step_unable_to_find){
						startPlace(place_scan_op, scanMin_, scanMax_, scanStep_/2);
						//log("Decrease step");
					}
					else{
						builder_state = UnableToFindWhereToDig;
						//log("UnableToFindWhereToDig");
					}
				}
				return;
			}
		}
		place_scan_op->checkPosition(placement_coords);
	}
}

void AIPlayer::startDigging(RegionDispatcher* region_disp)
{
	place_scan_op->operate(region_disp);
	region_disp->postOperateAnalyze();
	builder_state = Digging;
	if(place_scan_op->attributeID() != UNIT_ATTRIBUTE_NONE)
		building_installer->InitObject(unitAttribute(place_scan_op->attributeID()));
}

void AIPlayer::finishPlacement()
{
	xassert(place_scan_op->found());
	xassert(place_scan_op->attributeID() < UNIT_ATTRIBUTE_STRUCTURE_MAX);

	if(place_scan_op->lastCheck()){
		Vect2f& position = place_scan_op->bestPosition();
		//building_installer->SetBuildPosition(To3D(position), 0, 0);
		//xassert(building_installer->Valid);
		terBuilding* building = BuildStructure(place_scan_op->attributeID(), Vect3f(place_scan_op->bestPosition().x,place_scan_op->bestPosition().y,0));
		if(building){
			building->setCanselConstructionTime(place_scan_op->buildDuration());
			ai_tile_map->placeBuilding(clear_map.w2m(place_scan_op->bestLeftTop()), clear_map.w2m(place_scan_op->structureSize()), true);
		}
	}

	clearPlaceOp();
}

void AIPlayer::clearPlaceOp()
{	
	building_pause.start(ai_building_pause + difficultyPrm().aiDelay);
	builder_state = BuildingPause;

	delete place_scan_op;
	place_scan_op = 0;
	if(currentOrder){
		currentOrder->setWorkedOut();
		currentOrder = 0;
	}
	building_installer->Clear();
}

////////////////////////////////////////////
//		AIPlayer 
////////////////////////////////////////////
void AIPlayer::CircleShape::make(int radius_)
{
	radius = radius_;
	clear();
	reserve(radius*2 + 1);
	for(int y = -radius; y <= radius; y++){
		int x = round(sqrtf(sqr(radius) - sqr(y)));
		push_back(Interval(-x, 1 + x));
	}
}

int AIPlayer::calcWork(Circle& c) // circle in map's scale!
{
	//НЕ копать близко к границе
	if(c.x - c.radius < ai_border_offset || c.y - c.radius < ai_border_offset || 
	  c.x + c.radius >= clear_map.sizeX() - ai_border_offset || c.y + c.radius >= clear_map.sizeY() - ai_border_offset)
		return -1;

	if(circle_shape.radius != c.radius - 2)
		circle_shape.make(c.radius - 2);

	int work = 0;
	int y = c.y - c.radius;
	CircleShape::iterator si;
	FOR_EACH(circle_shape, si)
	{
		for(int x = c.x - si->xl; x <= c.x + si->xr; x++)
		{
			AITile& ai_tile = (*ai_tile_map)(x,y);
			if(ai_tile.dig_less)
				return false;
			work += ai_tile.dig_work;
		}
		y++;
	}

	return work;
}

int AIPlayer::calcWork(const Vect2i& left_top_, const Vect2i& size) 
{
	Vect2i left_top = clear_map.w2mFloor(left_top_);	 
	Vect2i right_bottom = clear_map.w2mCeil(left_top_ + size);

	//НЕ копать близко к границе
	if(left_top.x < ai_border_offset || left_top.y < ai_border_offset || 
	  right_bottom.x >= clear_map.sizeX() - ai_border_offset || right_bottom.y >= clear_map.sizeY() - ai_border_offset)
		return -1;

	int work = 0;
	for(int y = left_top.y; y <= right_bottom.y; y++)
	{
		for(int x = left_top.x; x <= right_bottom.x; x++)
		{
			AITile& ai_tile = (*ai_tile_map)(x,y);
			if(ai_tile.dig_less)
				return -1;
			work += ai_tile.dig_work;
		}
	}
	return work;
}

void AIPlayer::changeTileState(int x,int y)
{
	clear_map(x,y).state = (*ai_tile_map)(x,y).completed() ? ClearRegion::Completed : ClearRegion::Initial;
}


