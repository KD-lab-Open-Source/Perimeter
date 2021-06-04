#include "StdAfx.h"

#include "umath.h"
#include "IVisGeneric.h"
#include "VisGenericDefine.h"
#include "IRenderDevice.h"
#include "CameraManager.h"

#include "Runtime.h"


#include "GenericControls.h"
#include "Universe.h"

#include "UniverseInterface.h"
#include "RigidBody.h"
#include "ForceField.h"

#include "AiMain.h"

#include "GenericUnit.h"
#include "RealUnit.h"
#include "IronFrame.h"
#include "Squad.h"
#include "AiPrm.h"
#include "Installer.h"
#include "PlaceOperators.h"
#include "GameShell.h"
#include "Triggers.h"

AITileMap* ai_tile_map = 0;
int AIPlayer::IDs;

/////////////////////////////////////////////////////
//				AIPlayer
/////////////////////////////////////////////////////
AIPlayer::AIPlayer(const PlayerData& player_data)
: terPlayer(player_data),
clear_map(vMap.H_SIZE, vMap.V_SIZE),
place_scan_op(0),
aiLog_(0)
{
	m_ID = IDs++;
	ai_tile_map->AddCallBack(this);

	builder_state = BuildingIdle;
	building_installer = new terBuildingInstaller;

	requiredBuilders_ = 2; 
	requiredSoldiers_ = 0; 
	requiredOfficers_ = 0; 
	requiredTechnics_ = 0; 

	installingFrame_ = false;

	onlyIfCoreDamaged_ = true;

	if(player_data.realPlayerType == REAL_PLAYER_TYPE_AI)
		setAI(true);
}

AIPlayer::~AIPlayer()
{
	delete building_installer;
	if(place_scan_op)
		delete place_scan_op;
}

void AIPlayer::addUnit(terUnitBase* p)
{
	terPlayer::addUnit(p);

	switch(p->attr().ID){
	case UNIT_ATTRIBUTE_TERRAIN_MASTER:
		BrigadierList.push_back(p);
		break;

	case UNIT_ATTRIBUTE_SQUAD: {
		SquadList::iterator si;
		FOR_EACH(squads, si){
			if(!*si){
				*si = safe_cast<terUnitSquad*>(p);
				return;
			}
			if((*si)->commander()->attr().ID != UNIT_ATTRIBUTE_FRAME && !(*si)->commander()->isBuildingEnable())
				break;
		}
		squads.insert(si, safe_cast<terUnitSquad*>(p));
		break;
		}
	}
}

void AIPlayer::removeUnit(terUnitBase* p)
{
	terPlayer::removeUnit(p);

	switch(p->attr().ID)
	{
	case UNIT_ATTRIBUTE_TERRAIN_MASTER:
		BrigadierList.erase(remove(BrigadierList.begin(), BrigadierList.end(), p), BrigadierList.end());
		break;

	case UNIT_ATTRIBUTE_SQUAD: {
		SquadList::iterator si = find(squads.begin(), squads.end(), safe_cast<terUnitSquad*>(p));
		if(si != squads.end())
			*si = 0;
		
		while(!squads.empty() && !squads.back())
			squads.pop_back();

		while(squads.size() > 5){
			int i;
			for(i = 0; i < 5; i++)
				if(!squads[i]){
					squads[i] = squads.back();
					squads.pop_back();
					break;
				}
			if(i == 5)
				break;
		}
		}
		break;
	}
}

void AIPlayer::setAI(bool isAiIn) 
{ 
	terPlayer::setAI(isAiIn);

	if(isAI()){
		clearPlaceOp();
	}
}

void AIPlayer::Quant()
{
	terPlayer::Quant();

	start_timer_auto(AiQuant, STATISTICS_GROUP_AI);

	if(!isAI() || gameShell->missionEditor())
		return;

	// All function below assume the Frame exists
	if(!frame())
		return;

	FrameQuant();
	BuildingQuant();

#ifndef _FINAL_VERSION_
	XBuffer _queue(256, 1);
	OrderBuildingActionMap::iterator ai;
	FOR_EACH(orderBuildingActions, ai)
		_queue < getEnumNameAlt(ai->second->building) < ": " < getEnumNameAlt(ai->second->placementStrategy) < ";\n          ";
	watch_i(_queue, playerID());
	watch_i(builder_state, playerID());
	watch_i(BuildingsUnderConstruction(), playerID());
#endif

	orderBuildingActions.clear();
}

////////////////////////////////////////////
// Find where to dig
////////////////////////////////////////////
void AIPlayer::TerrainWorkingComplete(int x1,int y1,int sx,int sy)
{
	int x2 = clear_map.w2m(x1 + sx + clear_map.tileSize - 1);
	int y2 = clear_map.w2m(y1 + sy + clear_map.tileSize - 1);

	x1 = clear_map.w2m(x1);
	y1 = clear_map.w2m(y1);

	if(x1 < 0)
		x1 = 0;
	if(x2 >= clear_map.sizeX())
		x2 = clear_map.sizeX();
	if(y1 < 0)
		y1 = 0;
	if(y2 >= clear_map.sizeY())
		y2 = clear_map.sizeY();

	for(int y = y1;y < y2;y++)
		for(int x = x1;x < x2;x++)
			if((*ai_tile_map)(x,y).update(x,y)) 
				changeTileState(x,y);
}

////////////////////////////////////////////
//		Frame
////////////////////////////////////////////
void AIPlayer::FrameQuant()
{
	if(!frame())
		return;

	int nbuilders = 0;
	for(int i = 0;i < FRAME_SLOTS_MAX;i++)
		if(frame()->slot(i).ProductedUnit() == UNIT_ATTRIBUTE_BUILD_MASTER)
			nbuilders++;

	if(frame()->GetProductionPoint())
		return;

	int i;
	for(i = 0;i < FRAME_SLOTS_MAX;i++)
		if(frame()->slot(i).ProductedUnit() == UNIT_ATTRIBUTE_NONE){
			frame()->executeCommand(UnitCommand(nbuilders < requiredBuilders_ ? COMMAND_ID_BUILD_MASTER_INC : 
					COMMAND_ID_TERRAIN_MASTER_INC, i));
			return;
		}

	if(nbuilders > requiredBuilders_){
		for(int i = 0;i < FRAME_SLOTS_MAX;i++)
			if(frame()->slot(i).ProductedUnit() == UNIT_ATTRIBUTE_BUILD_MASTER)
			{
				frame()->executeCommand(UnitCommand(COMMAND_ID_TERRAIN_MASTER_INC, i)); 
				return;
			}
	}
	else if(nbuilders < requiredBuilders_){
		for(int i = 0;i < FRAME_SLOTS_MAX;i++)
			if(frame()->slot(i).ProductedUnit() == UNIT_ATTRIBUTE_TERRAIN_MASTER)
			{
				frame()->executeCommand(UnitCommand(COMMAND_ID_BUILD_MASTER_INC, i)); 
				return;
			}
	}
}

////////////////////////////////////////////
//		Brigadiers 
////////////////////////////////////////////
void AIPlayer::MoveBrigadiersToPoint(const Vect2f& pos,float radius)
{
//	return;
	//Даёт указание list двигаться в точку не доезжая radius до pos 
	Vect2f dir = pos - GetBrigadierCenter();
	dir /= FLT_EPS + dir.norm();
	Vect2f target = pos - dir*radius;
	Vect2f normal(dir.y, -dir.x);
	float width = (-0.5f - (float)BrigadierList.size()/2)*ai_brigadier_width;
	UnitList::iterator ui;
	FOR_EACH(BrigadierList,ui)
		(*ui)->executeCommand(UnitCommand(COMMAND_ID_POINT, To3D(target + normal*(width += ai_brigadier_width)), 0, COMMAND_SELECTED_MODE_SINGLE));
}

Vect2f AIPlayer::GetBrigadierCenter()
{
	if(BrigadierList.empty())
		return Vect2f::ZERO;

	int counter = 0;
	Vect2f center(0,0);
	UnitList::iterator it;
	FOR_EACH(BrigadierList,it)
	{
		center += (*it)->position2D();
		counter++;
	}
	center /= counter;
	return center;
}

int AIPlayer::BuildingsUnderConstruction()
{
//	terUnitBase* frame = GetFrame();
//	if(!frame)
//		return 0;

	int counter = 0;
	for(int structure = 0;structure < UNIT_ATTRIBUTE_STRUCTURE_MAX;structure++)
	{
		terBuildingList& struct_list = buildingList(structure);
		terBuildingList::iterator it;
		FOR_EACH(struct_list,it)
			if(!(*it)->isConstructed())
				counter++;
	}

	return counter;
}

terUnitSquad* AIPlayer::chooseSquad(ChooseSquadID chooseSquadID)
{
	if(!frame())
		return 0;

	switch(chooseSquadID){
	case CHOOSE_SQUAD_1:
		return squads.size() > 0 ? squads[0] : 0;
	case CHOOSE_SQUAD_2:
		return squads.size() > 1 ? squads[1] : 0;
	case CHOOSE_SQUAD_3:
		return squads.size() > 2 ? squads[2] : 0;
	case CHOOSE_SQUAD_4:
		return squads.size() > 3 ? squads[3] : 0;
	case CHOOSE_SQUAD_5:
		return squads.size() > 4 ? squads[4] : 0;

	case CHOOSE_SQUAD_SMALLEST: {
		SquadList::iterator si;
		terUnitSquad* bestSquad = 0;
		int number, bestAtoms = 10000;
		FOR_EACH(squads, si)
			if(bestAtoms > (number = (*si)->squadMolecula().elementCount())){
				bestAtoms = number;
				bestSquad = *si;
			}
		return bestSquad;
	}

	case CHOOSE_SQUAD_LARGEST: {
		SquadList::iterator si;
		terUnitSquad* bestSquad = 0;
		int number, bestAtoms = -1;
		FOR_EACH(squads, si)
			if(bestAtoms < (number = (*si)->squadMolecula().elementCount())){
				bestAtoms = number;
				bestSquad = *si;
			}
		return bestSquad;
	}

	case CHOOSE_SQUAD_NEAREST_TO_FRAME: {
		SquadList::iterator si;
		terUnitSquad* bestSquad = 0;
		float dist, bestDist = FLT_INF;
		FOR_EACH(squads, si)
			if(bestDist > (dist = (*si)->position2D().distance2(frame()->position2D()))){
				bestDist = dist;
				bestSquad = *si;
			}
		return bestSquad;
	}

	case CHOOSE_SQUAD_FARTHEST_FROM_FRAME: {
		SquadList::iterator si;
		terUnitSquad* bestSquad = 0;
		float dist, bestDist = 0;
		FOR_EACH(squads, si)
			if(bestDist < (dist = (*si)->position2D().distance2(frame()->position2D()))){
				bestDist = dist;
				bestSquad = *si;
			}
		return bestSquad;
	}
	default:
		xassert(0);
		return 0;
	}
}

void AIPlayer::showDebugInfo()
{
	terPlayer::showDebugInfo();

	if(!isAI())
		return;

	if(showDebugPlayer.placeOp && place_scan_op)
		place_scan_op->show(unitColor());

	//RegionPoint->activeLayer()->show();
}

void AIPlayer::triggerQuant() 
{ 
	TriggerChains::iterator i;
	FOR_EACH(triggerChains_, i)
		i->quant(*this);
}

void AIPlayer::checkEvent(const Event& event) 
{ 
	terPlayer::checkEvent(event);

	TriggerChains::iterator i;
	FOR_EACH(triggerChains_, i)
		i->checkEvent(*this, event); 

	if(isAI() && automaticUsingFieldTimer_() && event.type() == Event::ATTACK_OBJECT){
		const EventUnitMyUnitEnemy& eventUnit = safe_cast_ref<const EventUnitMyUnitEnemy&>(event);
		if(eventUnit.unitMy()->Player == this && eventUnit.unitEnemy()){
			if(eventUnit.unitMy()->attr().ID == UNIT_ATTRIBUTE_CORE)
				safe_cast<terProtector*>(eventUnit.unitMy())->startField();
			else if(!onlyIfCoreDamaged_ && eventUnit.unitMy()->isBuilding()){
				terUnitBase* unit = findUnit(UNIT_ATTRIBUTE_CORE, eventUnit.unitMy()->position2D());
				if(unit)
					safe_cast<terProtector*>(unit)->startField();
			}
		}
	}
}

void AIPlayer::checkLevelingAreaQuant()
{
	MetaRegionLock lock(RegionPoint);
	RegionDispatcher* zeroRegion = ZeroRegionPoint.data();
	float tolzerArea = zeroRegion->getRasterizeColumn().area();
	if(tolzerArea < checkLevelingAreaInitial)
		return;
	float factor = energyColumn().area()/tolzerArea;
	//watch_i(factor, playerID());
	if(factor > checkLevelingAreaFactor)
		return;

	start_timer_auto(checkLevelingAreaQuant, STATISTICS_GROUP_AI);
	
	zeroRegion->clear();
	zeroRegion->operateByCircle(frame()->position2D(), frame()->attr().ZeroLayerRadius, 1);
	for(int i = UNIT_ATTRIBUTE_CORE; i < UNIT_ATTRIBUTE_STRUCTURE_MAX; i++){
		terBuildingList::iterator bi;
		FOR_EACH(buildingList(i), bi)
			zeroRegion->operateByCircle((*bi)->position2D(), (*bi)->attr().ZeroLayerRadius ? (*bi)->attr().ZeroLayerRadius : (*bi)->radius(), 1);
	}
}
