#include "StdAfx.h"

#include "CameraManager.h"
#include "Runtime.h"

#include "Universe.h"
#include "Triggers.h"

#include "Config.h"
#include "UniverseInterface.h"

#include "GameShell.h"
#include "IronPort.h"
#include "AIMain.h"
#include "AIPrm.h"
#include "PerimeterShellUI.h"
#include "qd_textdb.h"
#include "WarBuilding.h"
#include "EditArchive.h"
#include "XPrmArchive.h"
#include "BinaryArchive.h"

//---------------------------------------
bool Condition::compare(int op1, int op2, CompareOperator op)
{
	switch(op){
		case COMPARE_LESS:
			return op1 < op2;
		case COMPARE_LESS_EQ:
			return op1 <= op2;
		case COMPARE_EQ:
			return op1 == op2;
		case COMPARE_NOT_EQ:
			return op1 != op2;
		case COMPARE_GREATER:
			return op1 > op2;
		case COMPARE_GREATER_EQ:
			return op1 >= op2;
		default: 
			xassert(0);
			return false;
	}
}

bool Condition::compare(float op1, float op2, CompareOperator op)
{
	switch(op){
		case COMPARE_LESS:
			return op1 < op2;
		case COMPARE_LESS_EQ:
			return op1 <= op2;
		case COMPARE_EQ:
			return op1 == op2;
		case COMPARE_NOT_EQ:
			return op1 != op2;
		case COMPARE_GREATER:
			return op1 > op2;
		case COMPARE_GREATER_EQ:
			return op1 >= op2;
		default: 
			xassert(0);
			return false;
	}
}

bool Condition::checkPlayer(const terPlayer* player1, const terPlayer* player2, AIPlayerType playerType)
{
	switch(playerType){
	case AI_PLAYER_TYPE_ME:
		return player1 == player2;
	case AI_PLAYER_TYPE_ENEMY:
		return player1 != player2 && !player2->isWorld();
	case AI_PLAYER_TYPE_WORLD:
		return player2->isWorld();
	case AI_PLAYER_TYPE_ANY:
		return true;
	default:
		xassert(0);
		return false;
	}
}

AIPlayer* Condition::getPlayer(AIPlayer& aiPlayer, AIPlayerType playerType)
{
	switch(playerType){
	case AI_PLAYER_TYPE_ME:
		return &aiPlayer;
	case AI_PLAYER_TYPE_ENEMY:
		return safe_cast<AIPlayer*>(aiPlayer.enemyPlayer());
	case AI_PLAYER_TYPE_WORLD:
		return safe_cast<AIPlayer*>(universe()->worldPlayer());
	default:
		xassert(0 && "Недопустимое значение типа игрока - 'Любой игрок'");
		return &aiPlayer;
	}
}

//---------------------------------------
bool ConditionIsPlayerAI::check(AIPlayer& aiPlayer) 
{ 
	return aiPlayer.isAI(); 
}

bool ConditionCheckBelligerent::check(AIPlayer& aiPlayer) 
{ 
	switch(belligerent){
	case EXODUS:
		return aiPlayer.belligerent() == BELLIGERENT_EXODUS0 || aiPlayer.belligerent() == BELLIGERENT_EXODUS1 ||
			aiPlayer.belligerent() == BELLIGERENT_EXODUS2 || aiPlayer.belligerent() == BELLIGERENT_EXODUS3 || aiPlayer.belligerent() == BELLIGERENT_EXODUS4;
	case HARKBACKHOOD:
		return aiPlayer.belligerent() == BELLIGERENT_HARKBACKHOOD0 || aiPlayer.belligerent() == BELLIGERENT_HARKBACKHOOD1;
	case EMPIRE:
		return aiPlayer.belligerent() == BELLIGERENT_EMPIRE0 || aiPlayer.belligerent() == BELLIGERENT_EMPIRE1 ||
			aiPlayer.belligerent() == BELLIGERENT_EMPIRE2 || aiPlayer.belligerent() == BELLIGERENT_EMPIRE_VICE || 
			aiPlayer.belligerent() == BELLIGERENT_EMPIRE3  || aiPlayer.belligerent() == BELLIGERENT_EMPIRE4;
	}
	return false; 
}

bool ConditionObjectExists::check(AIPlayer& aiPlayer) 
{ 
	if(constructedAndConstructing || object < UNIT_ATTRIBUTE_CORE || object > UNIT_ATTRIBUTE_STRUCTURE_MAX){
		switch(playerType){
		case AI_PLAYER_TYPE_ME:
			return aiPlayer.countUnits(object) >= counter;

		case AI_PLAYER_TYPE_ENEMY:{
			PlayerVect::iterator pi;
			FOR_EACH(universe()->Players, pi)
				if(!(*pi)->isWorld() && *pi != &aiPlayer && (*pi)->countUnits(object) >= counter)
					return true;
			}
			break;
		
		case AI_PLAYER_TYPE_WORLD:
			return universe()->worldPlayer()->countUnits(object) >= counter;
		
		case AI_PLAYER_TYPE_ANY:{
			int total = 0;
			PlayerVect::iterator pi;
			FOR_EACH(universe()->Players, pi)
				total += (*pi)->countUnits(object);

			return total >= counter;
		}
		}
	}
	else{
		switch(playerType){
		case AI_PLAYER_TYPE_ME:
			return aiPlayer.countBuildingsConstructed(object) >= counter;

		case AI_PLAYER_TYPE_ENEMY:{
			PlayerVect::iterator pi;
			FOR_EACH(universe()->Players, pi)
				if(!(*pi)->isWorld() && *pi != &aiPlayer && (*pi)->countBuildingsConstructed(object) >= counter)
					return true;
			}
			break;
		
		case AI_PLAYER_TYPE_WORLD:
			return universe()->worldPlayer()->countBuildingsConstructed(object) >= counter;
		
		case AI_PLAYER_TYPE_ANY:{
			int total = 0;
			PlayerVect::iterator pi;
			FOR_EACH(universe()->Players, pi)
				total += (*pi)->countBuildingsConstructed(object);

			return total >= counter;
		}
		}
	}

	return false;
}

void ConditionCreateObject::checkEvent(AIPlayer& aiPlayer, const Event& event) 
{
	if(event.type() == Event::CREATE_OBJECT){
		const EventUnitPlayer& eventUnit = safe_cast_ref<const EventUnitPlayer&>(event);
		if(checkPlayer(&aiPlayer, eventUnit.player(), playerType) && object == eventUnit.unit()->attr().ID)
			++created_;
	}
	else if(event.type() == Event::DESTROY_OBJECT){
		const EventUnitPlayer& eventUnit = safe_cast_ref<const EventUnitPlayer&>(event);
		if(checkPlayer(&aiPlayer, eventUnit.player(), playerType) && object == eventUnit.unit()->attr().ID)
			--created_;
	}
}

void ConditionKillObject::checkEvent(AIPlayer& aiPlayer, const Event& event) 
{
	if(event.type() == Event::ATTACK_OBJECT){
		const EventUnitMyUnitEnemy& eventUnit = safe_cast_ref<const EventUnitMyUnitEnemy&>(event);
		if(checkPlayer(&aiPlayer, eventUnit.unitMy()->Player, playerType)
			&& eventUnit.unitMy()->attr().ID == object
			&& !eventUnit.unitMy()->damageMolecula().isAlive()){
				++killed_;
		}
	}
}

void ConditionCaptureBuilding::checkEvent(AIPlayer& aiPlayer, const Event& event) 
{
	if(event.type() == Event::CAPTURE_BUILDING){
		const EventUnitPlayer& eventUnit = safe_cast_ref<const EventUnitPlayer&>(event);
		if(checkPlayer(&aiPlayer, eventUnit.player(), playerType) && object == eventUnit.unit()->attr().ID)
			setSatisfied(60*60*1000);
	}
}

bool ConditionObjectByLabelExists::check(AIPlayer& aiPlayer) 
{ 
	return universe()->findUnitByLabel(label);
}

void ConditionKillObjectByLabel::checkEvent(AIPlayer& aiPlayer, const Event& event) 
{
	if(event.type() == Event::DESTROY_OBJECT){
		const EventUnitPlayer& eventUnit = safe_cast_ref<const EventUnitPlayer&>(event);
		if(!strcmp(label, eventUnit.unit()->label()))
			setSatisfied();
	}
}

void ConditionTimeMatched::checkEvent(AIPlayer& aiPlayer, const Event& event) 
{
	if(event.type() == Event::TIME){
		const EventTime& eventTime = safe_cast_ref<const EventTime&>(event);
		if(eventTime.time() < time*1000)
			setSatisfied();
	}
}

void ConditionMouseClick::checkEvent(AIPlayer& aiPlayer, const Event& event) 
{
	if(event.type() == Event::MOUSE_CLICK)
		setSatisfied();
}

void ConditionClickOnButton::checkEvent(AIPlayer& aiPlayer, const Event& event) 
{
	if(event.type() == Event::CLICK_ON_BUTTON &&
	  safe_cast_ref<const EventClickOnButton&>(event).controlID() == controlID)
			counter_++;
}

bool ConditionToolzerSelectedNearObjectByLabel::check(AIPlayer& aiPlayer) 
{ 
	terUnitBase* unit = universe()->findUnitByLabel(label);
	if(unit){
		float radius2 = sqr(radius);
		Vect2i center = unit->position2D();
		MetaRegionLock lock(aiPlayer.RegionPoint);
		RegionDispatcher* region = aiPlayer.ZeroRegionPoint.data();
		for(int y = -radius; y <= radius; y += 16)
			for(int x = -radius; x <= radius; x += 16)
				if(sqr(x) + sqr(y) < radius2 && !region->filledRasterized(center + Vect2i(x, y)))
					return false;
		return true;
	}
	return false;
}

bool ConditionTerrainLeveledNearObjectByLabel::check(AIPlayer& aiPlayer) 
{ 
	terUnitBase* unit = universe()->findUnitByLabel(label);
	if(unit){
		if(vMap.IsCyrcleSurfaceLeveled(unit->position2D(), radius))
			return true;
	}
	return false;
}

bool ConditionSetSquadWayPoint::check(AIPlayer& aiPlayer)
{
	terUnitSquad* squad = aiPlayer.chooseSquad(CHOOSE_SQUAD_1);
	if(squad && !squad->noWayPoints())
		return true;
	return false;
}

void ConditionActivateSpot::checkEvent(AIPlayer& aiPlayer, const Event& event)
{
	if(event.type() == Event::ACTIVATE_SPOT){
		if(safe_cast_ref<const EventActivateSpot&>(event).isFilth()){
			if(type & FILTH)
				setSatisfied();
		}
		else{
			if(type & GEO)
				setSatisfied();
		}
	}
}

bool ConditionObjectNearObjectByLabel::check(AIPlayer& aiPlayer)
{
	terUnitBase* unit = universe()->findUnitByLabel(label);
	if(!unit){
		xassert_s(0 && "Объект по метке не найден: ", label);
		return false;
	}												
	else{
		terUnitBase* unit2 = getPlayer(aiPlayer, playerType)->findUnit(object, unit->position2D());
		if(!unit2)
			return false;
		if(objectConstructed && !unit2->isConstructed())
			return false;
		if(unit2->position2D().distance2(unit->position2D()) < sqr(distance))
			return true;
		return false;
	}
}

bool ConditionWeaponIsFiring::check(AIPlayer& aiPlayer)
{
	xassert(isBuilding(gun) && "'Спецоружие стреляет' - указать пушку");
	terBuildingList& list = aiPlayer.buildingList(gun);
	if(list.empty())
		return false;
	return list.front()->isFiring();
}

void ConditionTeleportation::checkEvent(AIPlayer& aiPlayer, const Event& event) 
{ 
	if(event.type() == Event::TELEPORTATION){
		const EventUnitPlayer& eventUnit = safe_cast_ref<const EventUnitPlayer&>(event);
		if(checkPlayer(&aiPlayer, eventUnit.player(), playerType) &&
			(teleportationType == TELEPORTATION_TYPE_ALPHA && eventUnit.unit()->attr().ID == UNIT_ATTRIBUTE_CORRIDOR_ALPHA ||
			teleportationType == TELEPORTATION_TYPE_OMEGA && eventUnit.unit()->attr().ID == UNIT_ATTRIBUTE_CORRIDOR_OMEGA)){
				setSatisfied();
		}
	}
}

bool ConditionEnegyLevelLowerReserve::check(AIPlayer& aiPlayer)
{
	return aiPlayer.energyData().accumulated() < energyReserve;
}

bool ConditionEnegyLevelUpperReserve::check(AIPlayer& aiPlayer)
{
	return aiPlayer.energyData().accumulated() > energyReserve;
}

bool ConditionEnegyLevelBelowMaximum::check(AIPlayer& aiPlayer)
{
	bool log = aiPlayer.energyData().accumulated() < accumulatedMax_ - delta;
	if(accumulatedMax_ < aiPlayer.energyData().accumulated())
		accumulatedMax_ = aiPlayer.energyData().accumulated();
	return log;
}

bool ConditionOutOfEnergyCapacity::check(AIPlayer& aiPlayer)
{
	terEnergyDataType& data = aiPlayer.energyData();
	return data.accumulated() > data.capacity()*chargingPercent/100;
}

bool ConditionNumberOfBuildingByCoresCapacity::check(AIPlayer& aiPlayer)
{
	AIPlayer* player = getPlayer(aiPlayer, playerType);
	return compare(round(player->countUnits(building)*factor), player->countUnits(building2), compareOp);
}

void ConditionUnitClassUnderAttack::checkEvent(AIPlayer& aiPlayer, const Event& event)
{
	if(event.type() == Event::ATTACK_OBJECT){
		const EventUnitMyUnitEnemy& eventUnit = safe_cast_ref<const EventUnitMyUnitEnemy&>(event);
		if((playerType == AI_PLAYER_TYPE_ANY || checkPlayer(&aiPlayer, eventUnit.unitMy()->Player, playerType))
			&& (eventUnit.unitMy()->unitClass() & victimUnitClass)
			&& (!eventUnit.unitEnemy() || (eventUnit.unitEnemy()->unitClass() & agressorUnitClass)) 
			&& (1 - eventUnit.unitMy()->life())*100 >= damagePercent){
				setSatisfied();
		}
	}
}

void ConditionUnitClassIsGoingToBeAttacked::checkEvent(AIPlayer& aiPlayer, const Event& event)
{
	if(event.type() == Event::AIM_AT_OBJECT){
		const EventUnitMyUnitEnemy& eventUnit = safe_cast_ref<const EventUnitMyUnitEnemy&>(event);
		if(&aiPlayer == eventUnit.unitMy()->Player
			&& (eventUnit.unitMy()->unitClass() & victimUnitClass)
			&& (eventUnit.unitEnemy()->unitClass() & agressorUnitClass)){
				setSatisfied();
		}
	}
}

bool ConditionSquadGoingToAttack::check(AIPlayer& aiPlayer)
{
	terUnitSquad* squad = aiPlayer.chooseSquad(chooseSquadID);
	if(squad && !squad->Empty() && !squad->noTargets())
		return true;
	return false;
}

bool ConditionFrameState::check(AIPlayer& aiPlayer)
{
	terFrame* frame = getPlayer(aiPlayer, playerType)->frame();
	
	if(state == AI_FRAME_STATE_EXIST)
		return frame;

	if(!frame)
		return false;

	switch(state){
	case AI_FRAME_STATE_INSTALLED:
		return frame->attached();
	case AI_FRAME_STATE_INSTALLING:
		return frame->attaching();
	case AI_FRAME_STATE_POWERED:
		return frame->powered();
	case AI_FRAME_STATE_MOVING:
		return frame->isMoving();
	case AI_FRAME_STATE_TELEPORTATION_ENABLED:
		return frame->canTeleportate();
	case AI_FRAME_STATE_TELEPORTATION_STARTED:
		return frame->underTeleportation();
	case AI_FRAME_STATE_SPIRAL_CHARGING:
		return frame->spiralLevel() > spiralChargingPercent/100.f - FLT_EPS;
	default:
		xassert(0);
	return false;
	}
}

bool ConditionCorridorOmegaUpgraded::check(AIPlayer& aiPlayer)
{
	terUnitBase* unit = 0;
	PlayerVect::const_iterator pi;
	FOR_EACH(universe()->Players, pi)
		if((unit = (*pi)->findUnit(UNIT_ATTRIBUTE_CORRIDOR_OMEGA)) != 0)
			return safe_cast<terCorridorOmega*>(unit)->upgraded();

	return false;
}

bool ConditionSquadSufficientUnits::check(AIPlayer& aiPlayer)
{
	terPlayer* player = playerType == AI_PLAYER_TYPE_ME ? &aiPlayer : aiPlayer.enemyPlayer();
	if(!player)
		return false;
	terUnitSquad* squad = safe_cast<AIPlayer*>(player)->chooseSquad(chooseSquadID);
	if(squad && squad->currentMutation() == unitType){
		if(unitType != UNIT_ATTRIBUTE_NONE)
			return compare(squad->countUnits(unitType), unitsNumber, compareOperator);
		else
			return compare(squad->countUnits(UNIT_ATTRIBUTE_SOLDIER), soldiers, compareOperator)
				&& compare(squad->countUnits(UNIT_ATTRIBUTE_OFFICER), officers, compareOperator)
				&& compare(squad->countUnits(UNIT_ATTRIBUTE_TECHNIC), technics, compareOperator);
		}
	return false;
}

bool ConditionMutationEnabled::check(AIPlayer& aiPlayer)
{
	if(!aiPlayer.GetMutationElement(unitType).Enabled)
		return false;
	const DamageMolecula& damageMolecula = aiPlayer.unitAttribute(unitType)->damageMolecula;
	if(damageMolecula.elementCount(DAMAGE_FILTER_SOLDIERS) && !aiPlayer.GetEvolutionBuildingData(UNIT_ATTRIBUTE_SOLDIER_PLANT).Worked)
		return false;
	if(damageMolecula.elementCount(DAMAGE_FILTER_OFFICERS) && !aiPlayer.GetEvolutionBuildingData(UNIT_ATTRIBUTE_OFFICER_PLANT).Worked)
		return false;
	if(damageMolecula.elementCount(DAMAGE_FILTER_TECHNICS) && !aiPlayer.GetEvolutionBuildingData(UNIT_ATTRIBUTE_TECHNIC_PLANT).Worked)
		return false;
	return true;
}

bool ConditionBuildingNearBuilding::check(AIPlayer& aiPlayer)
{
	terPlayer* player1 = getPlayer(aiPlayer, playerType1);
	terBuildingList& cores = player1->buildingList(UNIT_ATTRIBUTE_CORE);
	if(cores.empty())
		index_ = 0;
	else{
		if(index_ >= cores.size())
			index_ = 0;
		
		Vect2f position = cores[index_]->position2D();
		terPlayer* player2 = getPlayer(aiPlayer, playerType2);
		terUnitBase* unit2 = player2->findUnitByUnitClass(UNIT_CLASS_STRUCTURE | UNIT_CLASS_STRUCTURE_GUN, position);
		if(unit2 && position.distance2(unit2->position2D()) < sqr(distance))
			return true;
		else
			index_++;
	}

	return false;
}

void ConditionPlayerState::checkEvent(AIPlayer& aiPlayer, const Event& event)
{
	if(event.type() == Event::PLAYER_STATE){
		const EventPlayerState& eventState = safe_cast_ref<const EventPlayerState&>(event);
		if(eventState.playerState() == playerState){
			active_ = eventState.activate();
		}
	}
}

bool ConditionIsFieldOn::check(AIPlayer& aiPlayer)
{
	return aiPlayer.isFieldOn();
}

bool ConditionOnlyMyClan::check(AIPlayer& aiPlayer)
{
	PlayerVect::iterator pi;
	FOR_EACH(universe()->Players, pi)
		if((*pi)->frame() && (*pi)->clan() != aiPlayer.clan()){
			return false;
		}
	return true;
}

bool ConditionSkipCutScene::check(AIPlayer& aiPlayer)
{
	return gameShell->cutSceneSkipped();
}

bool ConditionCutSceneWasSkipped::check(AIPlayer& aiPlayer)
{ 
	return gameShell->cutSceneWasSkipped(timeMax*1000); 
}

bool ConditionDifficultyLevel::check(AIPlayer& aiPlayer)
{
	return aiPlayer.difficulty() == difficulty;
}


//------------------------------------------------------
void ActionDelay::activate(AIPlayer& aiPlayer) 
{ 
	timer.start(scaleByDifficulty ? round(aiPlayer.difficultyPrm().triggerDelayFactor*delay*1000) : delay*1000); 
}

bool ActionDelay::workedOut(AIPlayer& aiPlayer)
{ 
	if(showTimer){
		gameShell->setCountDownTime(timer() ? timer() : -1);
		aiPlayer.checkEvent(EventTime(timer()));
	}
	return !timer(); 
}

void ActionInstallFrame::activate(AIPlayer& aiPlayer) 
{
	aiPlayer.installFrame();
}

bool ActionInstallFrame::workedOut(AIPlayer& aiPlayer)
{
	return aiPlayer.installFrameQuant();
}

bool ActionOrderBuilding::automaticCondition(AIPlayer& aiPlayer) const 
{ 
	if(--waitingCounter_ < 0)
		waitingCounter_ = 0;

	if(!aiPlayer.unitAttribute(building)->enabledByBelligerent(aiPlayer.belligerent()))
		return false;

	if(energyReserve >= aiPlayer.energyData().accumulated())
		return false;

	EnableData& data = aiPlayer.GetEvolutionBuildingData(building);
	if(!data.Enabled)
		return false;

	terBuildingList::iterator bi;
	FOR_EACH(aiPlayer.buildingList(building), bi){
		if((*bi)->buildingStatus() & BUILDING_STATUS_HOLD_CONSTRUCTION){
			(*bi)->executeCommand(UnitCommand(COMMAND_ID_CONTINUE_CONSTRUCTION, 0));
			workedOut_ = true;
			return true;
		}
		if(!(*bi)->isConstructed())
			return false;
	}

	const PlaceOpPrm& prm = aiPrm.find(placementStrategy);
	if(prm.buildingsMax && aiPlayer.GetEvolutionBuildingData(building).Requested >= prm.buildingsMax)
		return false;

	const AttributeBuilding& attr = *safe_cast<const AttributeBuilding*>(aiPlayer.unitAttribute(building));
	if(attr.hasDowngrade()){
		terBuildingList& list = aiPlayer.buildingList(attr.downgrade());
		if(!list.empty()){
			if(list.front()->isBuildingEnable()){
				list.front()->executeCommand(UnitCommand(COMMAND_ID_STRUCTURE_UPGRADE, 0));
				list.front()->setCanselConstructionTime(buildDurationMax);
				workedOut_ = true;
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	else if(building != UNIT_ATTRIBUTE_CORE && aiPlayer.buildingList(UNIT_ATTRIBUTE_CORE).empty() && (!aiPlayer.frame() || !aiPlayer.frame()->attached()))
		return false;

	aiPlayer.orderBuilding((priority + 1)*100000 - (waitingCounter_ += 2), const_cast<ActionOrderBuilding*>(this));

	return accepted_; 
}

bool ActionOrderBuilding::workedOut(AIPlayer& aiPlayer)
{
	if(workedOut_){
		initialize();
		return true;
	}
	return false;
}

void ActionHoldBuilding::activate(AIPlayer& aiPlayer) 
{
	if(building == UNIT_ATTRIBUTE_ANY){
		for(int i = 0;i < UNIT_ATTRIBUTE_STRUCTURE_MAX;i++){
			terBuildingList::iterator bi;
			FOR_EACH(aiPlayer.buildingList(i), bi)
				if(!(*bi)->isConstructed())
					(*bi)->executeCommand(UnitCommand(COMMAND_ID_HOLD_CONSTRUCTION, 0));
		}
	}
	else{
		terBuildingList::iterator bi;
		FOR_EACH(aiPlayer.buildingList(building), bi)
			if(!(*bi)->isConstructed())
				(*bi)->executeCommand(UnitCommand(COMMAND_ID_HOLD_CONSTRUCTION, 0));
	}
}

bool ActionSellBuilding::automaticCondition(AIPlayer& aiPlayer) const
{
	return findBuilding(aiPlayer);
}

terUnitBase* ActionSellBuilding::findBuilding(AIPlayer& aiPlayer) const
{
	if(!aiPlayer.frame())
		return 0;

	terUnitBase* unit = 0;
	if(building != UNIT_ATTRIBUTE_ANY){
		terBuildingList& list = aiPlayer.buildingList(building);
		switch(sellFactor){
		case AI_SELL_CLOSEST_TO_FRAME: { // minimize
			float dist, distBest = FLT_INF;
			terBuildingList::iterator bi;
			FOR_EACH(list, bi)
				if(distBest > (dist = aiPlayer.frame()->position2D().distance2((*bi)->position2D()))){
					unit = *bi;
					distBest = dist;
				}
			}
			break;
		case AI_SELL_FAREST_FROM_FRAME: { // maximize
			float dist, distBest = -FLT_INF;
			terBuildingList::iterator bi;
			FOR_EACH(list, bi)
				if(distBest < (dist = aiPlayer.frame()->position2D().distance2((*bi)->position2D()))){
					unit = *bi;
					distBest = dist;
				}
			}
			break;
		case AI_SELL_IF_DAMAGE_GREATER: {
			terBuildingList::iterator bi;
			FOR_EACH(list, bi)
				if((*bi)->isConstructed() && (*bi)->life() < 1 - damagePercent/100){
					unit = *bi;
					break;
				}
			}
			break;
		case AI_SELL_IF_GUN_CANT_REACH_BUILDINGS: {
			terBuildingList& guns = aiPlayer.buildingList(building);
			if(!guns.empty()){
				if(index_ >= guns.size())
					index_ = 0;
				
				terUnitBase* unit1 = guns[index_];
				Vect2f position = unit1->position2D();
				terUnitBase* unit2 = aiPlayer.enemyPlayer()->findUnitByUnitClass(UNIT_CLASS_STRUCTURE | UNIT_CLASS_STRUCTURE_GUN, position);
				if(unit2 && position.distance2(unit2->position2D()) > sqr(unit1->attr().fireRadius()))
					return unit1;

				index_++;
			}
			}
			break;
		}
	}
	else{
		switch(sellFactor){
		case AI_SELL_CLOSEST_TO_FRAME: { // minimize
			float dist, distBest = FLT_INF;
			const UnitList& unit_list=aiPlayer.units();
			UnitList::const_iterator ui;
			FOR_EACH(unit_list, ui)
				if((*ui)->isBuilding() && distBest > (dist = aiPlayer.frame()->position2D().distance2((*ui)->position2D()))){
					unit = *ui;
					distBest = dist;
				}
			}
			break;
		case AI_SELL_FAREST_FROM_FRAME: { // maximize
			float dist, distBest = -FLT_INF;
			const UnitList& unit_list=aiPlayer.units();
			UnitList::const_iterator ui;
			FOR_EACH(unit_list, ui)
				if((*ui)->isBuilding() && distBest < (dist = aiPlayer.frame()->position2D().distance2((*ui)->position2D()))){
					unit = *ui;
					distBest = dist;
				}
			}
			break;
		case AI_SELL_IF_DAMAGE_GREATER: {
			const UnitList& unit_list=aiPlayer.units();
			UnitList::const_iterator ui;
			FOR_EACH(unit_list, ui)
				if((*ui)->isBuilding() && (*ui)->isConstructed() && (*ui)->life() < 1 - damagePercent/100){
					unit = *ui;
					break;
				}
			}
			break;
		case AI_SELL_IF_GUN_CANT_REACH_BUILDINGS: 
			xassert(0 && "Нельзя в стратегии продажи пушек указывать любое здание");
			break;
		}
	}
	return unit;
}

void ActionSellBuilding::activate(AIPlayer& aiPlayer)
{
	terUnitBase* unit = findBuilding(aiPlayer);
	if(unit)
		unit->executeCommand(UnitCommand(COMMAND_ID_UNINSTALL, 0));
}

bool ActionSwitchGuns::automaticCondition(AIPlayer& aiPlayer) const
{
	xassert(isBuilding(gunID));
	terBuildingList& guns = aiPlayer.buildingList(gunID);
	terBuildingList::iterator gi;
	FOR_EACH(guns, gi)
		if((*gi)->isConstructed() && ((*gi)->isBuildingPowerOn() ? mode == OFF : mode == ON))
			return true;
	return false;
}

void ActionSwitchGuns::activate(AIPlayer& aiPlayer)
{
	xassert(isBuilding(gunID));
	terBuildingList& guns = aiPlayer.buildingList(gunID);
	terBuildingList::iterator gi;
	FOR_EACH(guns, gi)
		(*gi)->executeCommand(UnitCommand(mode == ON ? COMMAND_ID_POWER_ON : COMMAND_ID_POWER_OFF, 0));
}

bool ActionUpgradeOmega::automaticCondition(AIPlayer& aiPlayer) const
{
	return aiPlayer.findUnit(UNIT_ATTRIBUTE_CORRIDOR_OMEGA);
}

void ActionUpgradeOmega::activate(AIPlayer& aiPlayer)
{
	terUnitBase* unit = aiPlayer.findUnit(UNIT_ATTRIBUTE_CORRIDOR_OMEGA);
	if(unit)
		unit->executeCommand(UnitCommand(COMMAND_ID_STRUCTURE_UPGRADE, 0));
}

//-------------------------------------
bool ActionChargeCores::automaticCondition(AIPlayer& aiPlayer) const 
{
	// !!! Расчитано пока только на зарядку всех
	terBuildingList& cores = aiPlayer.buildingList(UNIT_ATTRIBUTE_CORE);
	terBuildingList::iterator ci;
	FOR_EACH(cores, ci)
		if(!safe_cast<terProtector*>(*ci)->enableCharge()) 
			return true;
	return false;
}

void ActionChargeCores::activate(AIPlayer& aiPlayer) 
{
	switch(chargeCoresStrategy){
	case CHARGE_CORES_NONE:
		aiPlayer.dischargeAllCores();
		break;
	case CHARGE_CORES_INNER:
		xassert(0);
		break;
	case CHARGE_CORES_OUTER:
		xassert(0);
		break;
	case CHARGE_CORES_ALL:
		aiPlayer.chargeAllCores();
		break;
	default:
		xassert(0);
	}
}

bool ActionSwitchFieldOn::automaticCondition(AIPlayer& aiPlayer) const
{
	return aiPlayer.energyData().accumulated() > energyReserve && aiPlayer.canStartField();
}

void ActionSwitchFieldOn::activate(AIPlayer& aiPlayer) 
{
	timer.start(duration*1000);
	if(allCores)
		aiPlayer.startField();
	else
		aiPlayer.setAutomaticUsingField(duration*1000, onlyIfCoreDamaged);
}

bool ActionSwitchFieldOn::workedOut(AIPlayer& aiPlayer) 
{
	if(!timer()){
		aiPlayer.stopField();
		return true;
	}
	else 
		return false;
}

//-------------------------------------
bool ActionSquadOrderUnits::automaticCondition(AIPlayer& aiPlayer) const
{
	terUnitSquad* squad = aiPlayer.chooseSquad(chooseSquadID);
	if(squad){
		DamageMolecula total = squad->atomsRequested() + squad->squadMolecula();
		if(soldiers != total.elementCount(DAMAGE_FILTER_SOLDIERS) && aiPlayer.GetEvolutionBuildingData(UNIT_ATTRIBUTE_SOLDIER_PLANT).Worked)
			return true;
		if(officers != total.elementCount(DAMAGE_FILTER_OFFICERS) && aiPlayer.GetEvolutionBuildingData(UNIT_ATTRIBUTE_OFFICER_PLANT).Worked)
			return true;
		if(technics != total.elementCount(DAMAGE_FILTER_TECHNICS) && aiPlayer.GetEvolutionBuildingData(UNIT_ATTRIBUTE_TECHNIC_PLANT).Worked)
			return true;
		}
	return false;
}

void ActionSquadOrderUnits::activate(AIPlayer& aiPlayer)
{
	terUnitSquad* squad = aiPlayer.chooseSquad(chooseSquadID);
	if(squad)
		squad->setOrderUnitsAction(this);
}

bool ActionSquadOrderUnits::workedOut(AIPlayer& aiPlayer)
{
	bool done = true;
	terUnitSquad* squad = aiPlayer.chooseSquad(chooseSquadID);
	if(squad){
		if(squad->orderUnitsAction() != this)
			return true;

		int deltas[] = { 
			soldiers - squad->squadMolecula()[0] - squad->atomsRequested()[0], 
			officers - squad->squadMolecula()[1] - squad->atomsRequested()[1], 
			technics - squad->squadMolecula()[2] - squad->atomsRequested()[2]
		};

		for(int i = 0; i < 3; i++)
			if(deltas[i])
				done = false;

		if(aiPlayer.energyData().accumulated() < energyReserve)
			for(int i = 0; i < 3; i++){
				deltas[i] = min(deltas[i], -squad->atomsRequested()[i]);
				if(squad->atomsPaused()[i])
					deltas[i] = 0;
			}

		int i;
		for(i = 0; i < 3; i++){
			if(deltas[i])
				squad->executeCommand(UnitCommand(deltas[i] > 0 ? COMMAND_ID_PRODUCTION_INC : 
					(squad->atomsRequested()[i] ? COMMAND_ID_PRODUCTION_DEC : COMMAND_ID_BASIC_UNIT_DESTROY), i));
			if(squad->atomsRequested()[i])
				done = false;
		}

		if(showDebugSquad.order && !done){
			XBuffer buf;
			buf < "Need: " <= soldiers < " " <= officers < " " <= technics < "\n";
			buf < "Exist: " <= squad->squadMolecula()[0] < " " <= squad->squadMolecula()[1] < " " <= squad->squadMolecula()[2] < "\n";
			buf < "Requested: " <= squad->atomsRequested()[0] < " " <= squad->atomsRequested()[1] < " " <= squad->atomsRequested()[2] < "\n";
			buf < "Delta: " <= deltas[0] < " " <= deltas[1] < " " <= deltas[2] < "\n";
			show_text(squad->position(), buf, CYAN);
		}
	}

	return done;
}

bool ActionSquadAttack::automaticCondition(AIPlayer& aiPlayer) const
{
	terUnitSquad* squad = aiPlayer.chooseSquad(chooseSquadID);
	if(!squad || squad->Empty() || squad->attackAction() && !squad->attackAction()->interruptable)
		return false;

	if(attackByType != UNIT_ATTRIBUTE_NONE &&
	  !(attackByType == squad->currentMutation() && unitsNumber <= squad->complexUnitsNumber())){

		if(squad->mutationEnergy() < 1 - FLT_EPS)
			return false;

		if(!aiPlayer.GetMutationElement(attackByType).Enabled)
			return false;
	}

	DamageMolecula delta = squad->squadMutationMolecula() - (attackByType != UNIT_ATTRIBUTE_NONE ?
		DamageMolecula(aiPlayer.unitAttribute(attackByType)->damageMolecula)*unitsNumber :
		DamageMolecula(aiPlayer.unitAttribute(UNIT_ATTRIBUTE_SOLDIER)->damageMolecula)*soldiers + 
		DamageMolecula(aiPlayer.unitAttribute(UNIT_ATTRIBUTE_OFFICER)->damageMolecula)*officers + 
		DamageMolecula(aiPlayer.unitAttribute(UNIT_ATTRIBUTE_TECHNIC)->damageMolecula)*technics);

	if(delta.elementCount(DAMAGE_FILTER_SOLDIERS) < 0)
		return false;
	if(delta.elementCount(DAMAGE_FILTER_OFFICERS) < 0)
		return false;
	if(delta.elementCount(DAMAGE_FILTER_TECHNICS) < 0)
		return false;

	if(squadFollowDistance)
		return true;

	terPlayer* enemyPlayer = aiPlayer.enemyPlayer();
	UnitAttributeIDList::const_iterator ti;
	FOR_EACH(unitsToAttack, ti)
		if(enemyPlayer->findUnit(*ti))
			return true;
	
	if(enemyPlayer->findUnitByUnitClass(unitClassToAttack, squad->position2D()))
		return true;

	if(enemyPlayer->findUnitByUnitClass(squad->attackClass(), squad->position2D()))
		return true;

	return false;
}

void ActionSquadAttack::activate(AIPlayer& aiPlayer)
{
	terUnitSquad* squad = aiPlayer.chooseSquad(chooseSquadID);
	if(squad){
		if(squad->attackAction())
			squad->attackAction()->interrupt(squad);
		squad->setAttackAction(this);
		squad->clearOrders();
		if(squad->currentMutation() != attackByType && squad->mutationEnergy() > 1 - FLT_EPS)
			squad->executeCommand(UnitCommand(COMMAND_ID_UNIT_MORPHING, attackByType));
		squad->executeCommand(UnitCommand(COMMAND_ID_UNIT_OFFENSIVE, offensive));
		attackTimer.start(attackTime*1000);
		interrupt_ = false;
		if(squadFollowDistance)
			squad->setSquadToFollow(aiPlayer.chooseSquad(squadToFollowBy));
	}
}

void ActionSquadAttack::interrupt(terUnitSquad* squad) 
{ 
	interrupt_ = true; 

	if(holdProduction)
		squad->unholdProduction();

	squad->executeCommand(UnitCommand(COMMAND_ID_UNIT_OFFENSIVE, 0));
	squad->clearOrders();

	squad->setAttackAction(0);
}

bool ActionSquadAttack::workedOut(AIPlayer& aiPlayer)
{
	if(interrupt_)
		return true;
	
	terUnitSquad* squad = aiPlayer.chooseSquad(chooseSquadID);
	if(!squad)
		return true;

	if(attackTimer() && !squad->Empty()){
		if(holdProduction)
			squad->holdProduction();
		
		if(!squadFollowDistance && squad->currentMutation() == attackByType){
			if(squad->noTargets()){
				Vect2iVect path;
				terUnitBase* target = findTarget(aiPlayer, path);
				if(ignoreLastTarget)
					lastTarget_ = target;
				if(target){
					#ifndef _FINAL_
						//XBuffer buf;
						//buf < "Не назначен класс атаки у " < getEnumNameAlt(squad->currentMutation())
						//	< ", чтобы атаковать " < getEnumNameAlt(target->attr().ID);
						//xassert_s(squad->squadUnits().front()->checkFireClass(target), buf);
					#endif
					if(!path.empty()){
						path.pop_back();
						squad->addTarget(target, path);
					}
					else
						squad->addTarget(target);
					lastTargetPosition_ = squad->getAttackPoint()->position();//target->position2D();
				}
			}
			else if(lastTargetPosition_.distance2(squad->getAttackPoint()->position()) 
			  > sqr(squad->currentAttribute()->fireRadius()/2))
				squad->clearOrders();
		}

		if(attackByType != UNIT_ATTRIBUTE_NONE && squad->mutationFinished() && squad->mutationEnergy() > 1 - FLT_EPS
		  && remutateCounter && squad->countPossibleUnits(attackByType) - squad->complexUnitsNumber() >= remutateCounter)
			squad->executeCommand(UnitCommand(COMMAND_ID_UNIT_MORPHING, attackByType));

		return false;
	}
	else{
		const_cast<ActionSquadAttack&>(*this).interrupt(squad);
		if(returnToBase){
			Vect2iVect path;
			if(!squad->isDigging() && aiPlayer.enemyPlayer()->findPathToPoint(aiPlayer.defenceMap(), squad->position2D(), squad->homePosition(), path)){
				Vect2iVect::iterator i;
				FOR_EACH(path, i)
					squad->addWayPoint(*i);
			}
			else
				squad->addWayPoint(squad->homePosition());
		}
		return true;
	}
}

terUnitBase* ActionSquadAttack::findTarget(AIPlayer& aiPlayer, Vect2iVect& path) const
{
	terUnitSquad* squad = aiPlayer.chooseSquad(chooseSquadID);
	if(!squad || squad->Empty())
		return 0;

	bool ignoreUninstalledFrame = squad->currentAttribute()->ignoreUninstalledFrame;

	terPlayer* enemyPlayer = aiPlayer.enemyPlayer();
	UnitAttributeIDList::const_iterator ti;
	FOR_EACH(unitsToAttack, ti){
		if(ignoreUninstalledFrame && *ti == UNIT_ATTRIBUTE_FRAME && enemyPlayer->frame() && !enemyPlayer->frame()->attached())
			continue;
		terUnitBase* target = enemyPlayer->findPathToTarget(aiPlayer.defenceMap(), *ti, lastTarget_, squad->position2D(), path);
		if(target)
			return target;
	} 
	
	int attackClass = unitClassToAttack;
	if(ignoreUninstalledFrame && (attackClass & UNIT_CLASS_FRAME) && enemyPlayer->frame() && !enemyPlayer->frame()->attached())
		attackClass &= ~UNIT_CLASS_FRAME;
	terUnitBase* target = enemyPlayer->findPathToTarget(aiPlayer.defenceMap(), attackClass, lastTarget_, squad->position2D(), path);
	if(target)
		return target;

	attackClass = squad->attackClass();
	if(ignoreUninstalledFrame && (attackClass & UNIT_CLASS_FRAME) && enemyPlayer->frame() && !enemyPlayer->frame()->attached())
		attackClass &= ~UNIT_CLASS_FRAME;
	return enemyPlayer->findPathToTarget(aiPlayer.defenceMap(), attackClass, lastTarget_, squad->position2D(), path);
}

void ActionSquadMove::activate(AIPlayer& aiPlayer)
{
	terUnitSquad* squad = aiPlayer.chooseSquad(chooseSquadID);
	if(squad){
		terUnitBase* unit = universe()->findUnitByLabel(label);
		if(unit)
			squad->executeCommand(UnitCommand(COMMAND_ID_POINT, unit->position(), 0));
	}
}

bool ActionAttackBySpecialWeapon::automaticCondition(AIPlayer& aiPlayer) const
{
	terUnitBase* unit = aiPlayer.findUnit(weapon);
	if(!unit || !safe_cast<terUnitReal*>(unit)->isWeaponLoaded())
		return false;

	return findTarget(aiPlayer, unit->position2D(), unit->attr().fireRadiusMin());
}

void ActionAttackBySpecialWeapon::activate(AIPlayer& aiPlayer)
{
	terUnitBase* unit = aiPlayer.findUnit(weapon);
	if(unit){
		terBuildingMilitary* warBuilding = safe_cast<terBuildingMilitary*>(unit);
		terUnitBase* target = findTarget(aiPlayer, warBuilding->position2D(), unit->attr().fireRadiusMin());
		#ifndef _FINAL_
			XBuffer buf;
			buf < "Не назначен класс атаки у " < getEnumNameAlt(unit->attr().ID)
				< ", чтобы атаковать " < getEnumNameAlt(target->attr().ID);
			xassert_s(unit->checkFireClass(target), buf);
		#endif
		if(target)
			warBuilding->setAttackTarget(safe_cast<terUnitReal*>(target), true);
	}
}

terUnitBase* ActionAttackBySpecialWeapon::findTarget(AIPlayer& aiPlayer, const Vect2f& position, float radiusMin) const
{
	PlayerVect::iterator pi;
	FOR_EACH(universe()->Players, pi){
		if(aiPlayer.clan() == (*pi)->clan())
			continue;
		UnitAttributeIDList::const_iterator ti;
		FOR_EACH(unitsToAttack, ti){
			terUnitBase* target = (*pi)->findUnit(*ti, position, radiusMin);
			if(target)
				return target;
		} 
	}

	FOR_EACH(universe()->Players, pi){
		if(aiPlayer.clan() == (*pi)->clan())
			continue;
		UnitAttributeIDList::const_iterator ti;
		FOR_EACH(unitsToAttack, ti){
			terUnitBase* target = (*pi)->findUnitByUnitClass(unitClassToAttack, position, radiusMin);
			if(target)
				return target;
		} 
	}
	
	return 0;
}


//-------------------------------------
void ActionSetCamera::activate(AIPlayer& aiPlayer) 
{ 
	if(aiPlayer.active()){
		gameShell->setSkipCutScene(false);
		terCamera->SetCameraFollow(0);
		string camera = cameraSplineName;
		if(camera == "Camera0" || camera == "Camera1" || camera == "Camera2" || camera == "Camera3"){
			XBuffer buffer;
			buffer < "Camera" <= aiPlayer.playerStrategyIndex();
			camera = buffer;
		}
		const SaveCameraSplineData* spline = gameShell->manualData().findCameraSpline(camera.c_str());
		if(spline){
			xassert(!spline->path.empty());
			if(!spline->useAsSpline){
				CameraCoordinate coord;
				coord.load(spline->path.front());
				terCamera->setCoordinate(coord);
			}
			else{
				terCamera->loadPath(*spline, smoothTransition);
				terCamera->startReplayPath(round(stepTime*1000), cycles);
			}
		}
	}
}
 
bool ActionSetCamera::workedOut(AIPlayer& aiPlayer)
{
	if(!aiPlayer.active() || !strlen(cameraSplineName))
		return true;
	if(!terCamera->isPlayingBack()){
		terCamera->erasePath();
		return true;
	}
	return false;
}

void ActionOscillateCamera::activate(AIPlayer& aiPlayer) 
{
	terCamera->startOscillation(duration*1000, factor);
}

void ActionVictory::activate(AIPlayer& aiPlayer) 
{ 
	if(aiPlayer.active()){
		gameShell->setTriggersDisabled();
		_pShellDispatcher->OnInterfaceMessage(UNIVERSE_INTERFACE_MESSAGE_GAME_VICTORY); 
	}
} 

void ActionDefeat::activate(AIPlayer& aiPlayer) 
{ 
	if(aiPlayer.active()){
		gameShell->setTriggersDisabled();
		_pShellDispatcher->OnInterfaceMessage(UNIVERSE_INTERFACE_MESSAGE_GAME_DEFEAT); 
	}
} 

bool ActionTeleportationOut::automaticCondition(AIPlayer& aiPlayer) const
{
	return aiPlayer.frame() && aiPlayer.frame()->canTeleportate();
}

void ActionTeleportationOut::activate(AIPlayer& aiPlayer)
{
	if(aiPlayer.frame())
		aiPlayer.frame()->executeCommand(UnitCommand(COMMAND_ID_TELEPORTATE, 0, COMMAND_SELECTED_MODE_NEGATIVE));
} 

terUnitBase* ActionKillObject::findObject() const
{
	terUnitBase* unit = 0;
	PlayerVect::const_iterator pi;
	FOR_EACH(universe()->Players, pi)
		if((unit = (*pi)->findUnit(object)) != 0)
			return unit;
	return 0;
}

bool ActionKillObject::workedOut(AIPlayer& aiPlayer)
{ 
	terUnitBase* unit = findObject();
	if(unit){
		unit->damageMoleculaKill();
		return false;
	}
	return true;
}

void ActionRepareObjectByLabel::activate(AIPlayer& aiPlayer)
{
	terUnitBase* unit = universe()->findUnitByLabel(label);
	if(!unit){
		xassert_s(0 && "Объект по метке не найден: ", label);
	}
	else{
		unit->setDamageMolecula(unit->attr().damageMolecula);
	}
}


void ActionActivateObjectByLabel::activate(AIPlayer& aiPlayer)
{
	terUnitBase* unit = universe()->findUnitByLabel(label);
	if(!unit){
		xassert_s(0 && "Объект по метке не найден: ", label);
	}
	else{
		unit->setActivity(true);
	}
}

void ActionDeactivateObjectByLabel::activate(AIPlayer& aiPlayer)
{
	terUnitBase* unit = universe()->findUnitByLabel(label);
	if(!unit){
		xassert_s(0 && "Объект по метке не найден: ", label);
	}
	else{
		unit->setActivity(false);
	}
}

bool ActionMessage::workedOut(AIPlayer& aiPlayer)
{ 
	if(!delayTimer()){
		if(!durationTimer()){
			if(started_)
				return true;
			started_ = true;
			float soundDuration = _shellIconManager.playSpeech(messageID);
			int time = (syncroBySound && soundDuration ? round(soundDuration*1000) + speechDurationAddition : duration*1000);
			_shellIconManager.showHint(messageID, time);
			if(duration)
				durationTimer.start(time);
			else
				return true;
		}
	}
	return false;
} 

void ActionTask::activate(AIPlayer& aiPlayer)
{ 
	if(!aiPlayer.active())
		return;

	float soundDuration = _shellIconManager.playSpeech(taskID);
	int time = (syncroBySound && soundDuration ? round(soundDuration*1000) + speechDurationAddition : duration*1000);
	if(showTips)
		_shellIconManager.showHint(taskID, time, type);
	_shellIconManager.setTask(taskID, type);
	durationTimer.start(time);
	switch(type){
	case ASSIGNED:
		aiPlayer.soundEvent(SOUND_VOICE_OBJECTIVES_UPDATED);
		break;
	case COMPLETED:
		aiPlayer.soundEvent(SOUND_VOICE_OBJECTIVE_COMPLETE);
		break;
	}
} 

bool ActionTask::workedOut(AIPlayer& aiPlayer)
{ 
	return !durationTimer();
} 

terUnitBase* ActionSetCameraAtObject::findUnit(AIPlayer& aiPlayer)
{
	terUnitBase* unit = Condition::getPlayer(aiPlayer, playerType)->findUnit(object);
	
	if(!unit){
		if(object == UNIT_ATTRIBUTE_FRAME)
			return 0;
		else
			unit = universe()->findUnit(object);
	}

	xassert(unit && "Самера на объект: объект не найден");

	return unit;
}

void ActionSetCameraAtObject::activate(AIPlayer& aiPlayer)
{
	turnStarted_ = false;

	gameShell->setSkipCutScene(false);

	terCamera->SetCameraFollow(0);

	terUnitBase* unit = findUnit(aiPlayer);
	if(setFollow && unit){
		terCamera->SetCameraFollow(unit, transitionTime*1000);
	}
	else{
		Vect2f position;
		if(unit)
			position = unit->position2D();
		else if(object == UNIT_ATTRIBUTE_FRAME)
			position = Condition::getPlayer(aiPlayer, playerType)->lastFramePosition();
		else
			return;
		CameraCoordinate coord(position, terCamera->coordinate().psi(), terCamera->coordinate().theta(), terCamera->coordinate().distance());
		if(transitionTime){
			SaveCameraSplineData spline;
			spline.path.push_back(SaveCameraData());
			coord.save(spline.path.back());
			terCamera->loadPath(spline, true);
			terCamera->startReplayPath(transitionTime*1000, 1);
		}
		else
			terCamera->setCoordinate(coord);
	}
}

bool ActionSetCameraAtObject::workedOut(AIPlayer& aiPlayer)
{
	if(!terCamera->isPlayingBack()){
		terCamera->erasePath();
		if(!turnStarted_ && turnTime){
			//xassert(!setFollow);
			terUnitBase* unit = findUnit(aiPlayer);
			if(unit){
				SaveCameraSplineData spline;
				spline.path.push_back(SaveCameraData());
				CameraCoordinate coord(unit->position2D(), cycle(terCamera->coordinate().psi(), 2*M_PI) + 2*M_PI, terCamera->coordinate().theta(), terCamera->coordinate().distance());
				coord.save(spline.path.back());
				terCamera->loadPath(spline, true);
				terCamera->startReplayPath(turnTime*1000, 1);
				turnStarted_ = true;
				return false;
			}
		}
		return true;
	}

	return false;
}

void ActionSetControls::activate(AIPlayer& aiPlayer)
{ 
	gameShell->changeControlState(controls);
} 

void ActionSelectUnit::activate(AIPlayer& aiPlayer)
{
	const UnitList& unit_list=aiPlayer.units();
	UnitList::const_iterator ui;
	FOR_EACH(unit_list, ui)
		if((*ui)->attr().ID == unitID){
			universe()->select.allLikeUnitToSelection(*ui);
			break;
		}
}

void ActionProduceBrigadierOrProrab::activate(AIPlayer& aiPlayer)
{
	if(aiPlayer.frame())
		for(int i = 0;i < FRAME_SLOTS_MAX;i++)
			if(aiPlayer.frame()->slot(i).ProductedUnit() == UNIT_ATTRIBUTE_NONE){
				aiPlayer.frame()->executeCommand(UnitCommand(produceBrigadier ? COMMAND_ID_TERRAIN_MASTER_INC : COMMAND_ID_BUILD_MASTER_INC, i));
				return;
			}
}

void ActionFrameMove::activate(AIPlayer& aiPlayer)
{
	if(aiPlayer.frame()){
		terUnitBase* unit = universe()->findUnitByLabel(label);
		if(unit)
			aiPlayer.frame()->executeCommand(UnitCommand(COMMAND_ID_POINT, unit->position(), 0));
	}
}

void ActionFrameDetach::activate(AIPlayer& aiPlayer)
{
	if(aiPlayer.frame())
		aiPlayer.frame()->executeCommand(UnitCommand(COMMAND_ID_FRAME_DETACH, 0, 0));
}

void ActionSetInterface::activate(AIPlayer& aiPlayer)
{
	gameShell->setCutSceneMode(!enableInterface);
}

void ActionActivateAllSpots::activate(AIPlayer& aiPlayer)
{
	terPlayer* world = universe()->worldPlayer();
	const UnitList& unit_list=world->units();
	UnitList::const_iterator ui;
	FOR_EACH(unit_list, ui)
		switch((*ui)->attr().ID){
		case UNIT_ATTRIBUTE_FILTH_SPOT:
		case UNIT_ATTRIBUTE_GEO_INFLUENCE:
		case UNIT_ATTRIBUTE_GEO_BREAK:
		case UNIT_ATTRIBUTE_GEO_FAULT:
		case UNIT_ATTRIBUTE_GEO_HEAD:
			(*ui)->setActivity(true);
			break;
		}
}

void ActionDeactivateAllSpots::activate(AIPlayer& aiPlayer)
{
	terPlayer* world = universe()->worldPlayer();
	const UnitList& unit_list=world->units();
	UnitList::const_iterator ui;
	FOR_EACH(unit_list, ui)
		switch((*ui)->attr().ID){
		case UNIT_ATTRIBUTE_FILTH_SPOT:
		case UNIT_ATTRIBUTE_GEO_INFLUENCE:
		case UNIT_ATTRIBUTE_GEO_BREAK:
		case UNIT_ATTRIBUTE_GEO_FAULT:
		case UNIT_ATTRIBUTE_GEO_HEAD:
			(*ui)->setActivity(false);
			break;
		}
}

void ActionSetControlEnabled::activate(AIPlayer& aiPlayer)
{
	aiPlayer.setControlEnabled(controlEnabled);
}

//------------------------------------------------------
void Trigger::quant(AIPlayer& aiPlayer, TriggerChain& triggerChain)
{
	if((action && action->onlyIfAI() && !aiPlayer.isAI()) || (gameShell->triggersDisabled() && aiPlayer.active()) || gameShell->missionEditor())
		return;

	switch(state_){
	case SLEEPING:
	case DONE:{
		// Входящие стрелки одного цвета - И, разных - ИЛИ
		vector<int> conditions(STRATEGY_COLOR_MAX, 0);
		vector<TriggerLink*>::iterator li;
		FOR_EACH(incomingLinks_, li)
			conditions[(*li)->getType()] |= (*li)->active() ? 1 : 2;

		vector<int>::iterator bi;
		FOR_EACH(conditions, bi)
			if(*bi == 1){
				state_ = CHECKING;
				triggerChain.addLogRecord(*this, (string("П: ") + name()).c_str());
				break;
			}
		
		if(state_ != CHECKING)
			break;
		}

	case CHECKING:
		if((!condition || condition->checkDebug(aiPlayer)) && (!action || action->automaticCondition(aiPlayer))){
			activate(aiPlayer, triggerChain);
			triggerChain.addLogRecord(*this, (string("С: ") + name()).c_str());
		}
		else
			break;

	case WORKING:
		if(!action || action->workedOut(aiPlayer)){
			vector<TriggerLink>::iterator li;
			FOR_EACH(outcomingLinks_, li)
				li->activate(triggerChain);
			state_ = DONE;
			if(!active())
				triggerChain.deactivateTrigger(this);
			triggerChain.addLogRecord(*this, (string("Ф: ") + name()).c_str());
		}
		break;
	}
}

void Trigger::checkEvent(AIPlayer& aiPlayer, const Event& event)
{
	if(state_ == CHECKING && condition)
		condition->checkEvent(aiPlayer, event);
}

void Trigger::activate(AIPlayer& aiPlayer, TriggerChain& triggerChain)
{
	if(condition)
		condition->clear();

	vector<TriggerLink*>::iterator li;
	FOR_EACH(incomingLinks_, li){
		if((*li)->active()){ // Деактивировать связи из родительского триггера других цветов
			Trigger* trigger = (*li)->parent;
			vector<TriggerLink>::iterator lj;
			FOR_EACH(trigger->outcomingLinks_, lj)
				if(lj->getType() != (*li)->getType() && !lj->autoRestarted()){
					if(lj->child && lj->child->state_ == CHECKING) // Выключить другие триггера
						lj->child->state_ = lj->child->executionCounter_ ? DONE : SLEEPING;
					lj->deactivate(triggerChain);
				}
			}
		if(!(*li)->autoRestarted()) // Деактивировать, если тонкая
			(*li)->deactivate(triggerChain);
		}
	
	if(action)
		action->activate(aiPlayer);
	state_ = WORKING;
	executionCounter_++;
}

void TriggerLink::activate(TriggerChain& triggerChain)
{
	active_ = true;
	triggerChain.activateTrigger(child);
}

void TriggerLink::deactivate(TriggerChain& triggerChain)
{
	active_ = false;
	if(!child->active())
		triggerChain.deactivateTrigger(child);
}

//------------------------------------------------------
void TriggerChain::load(const char* fileName) 
{
	XPrmIArchive ia;
	if(ia.open((string("Scripts\\Triggers\\") + fileName).c_str()))
		ia >> WRAP_NAME(*this, "TriggerChain");
	name = fileName;
	initialize();
}

void TriggerChain::save() const 
{
	XBuffer buffer;
	buffer < "Scripts\\Triggers\\" < name;
	XPrmOArchive(buffer) << WRAP_NAME(*this, "TriggerChain");
}

void TriggerChain::initializeTriggersAndLinks()
{
	TriggerList::iterator ti;
	FOR_EACH(triggers, ti){
		ti->state_ = Trigger::SLEEPING;
		ti->executionCounter_ = 0;
		vector<TriggerLink>::iterator li;
		FOR_EACH(ti->outcomingLinks_, li)
			li->active_ = false;
	}
	find("START")->state_ = Trigger::CHECKING;
}

void TriggerChain::initializeCameraTrigger(int playerIndex)
{
	Trigger* trigger = find("Camera");
	trigger->state_ = Trigger::CHECKING;
	if(!trigger->action)
		trigger->action = new ActionSetCamera;
	XBuffer buf;
	buf < "Camera" <= playerIndex;
	safe_cast<ActionSetCamera*>(trigger->action())->cameraSplineName = buf;
	activateTrigger(trigger);
}

void TriggerChain::quant(AIPlayer& aiPlayer)
{
	for(int i = 0; i < activeTriggers_.size(); i++)
		activeTriggers_[i]->quant(aiPlayer, *this);
}

void TriggerChain::checkEvent(AIPlayer& aiPlayer, const Event& event)
{
	ActiveTriggers::iterator ti;
	FOR_EACH(activeTriggers_, ti)
		(*ti)->checkEvent(aiPlayer, event);
}

void TriggerChain::activateTrigger(Trigger* trigger)
{
	if(std::find(activeTriggers_.begin(), activeTriggers_.end(), trigger) == activeTriggers_.end())
		activeTriggers_.push_back(trigger);
}

void TriggerChain::deactivateTrigger(Trigger* trigger)
{
	activeTriggers_.erase(remove(activeTriggers_.begin(), activeTriggers_.end(), trigger), activeTriggers_.end());
}

//------------------------------------------------------

const char* SaveManualData::popupCameraSplineName() const
{
	vector<const char*> items;
	vector<SaveCameraSplineData>::const_iterator ci;
	FOR_EACH(cameras, ci)
		items.push_back(ci->name);

	return popupMenu(items);
}

SaveCameraSplineData* SaveManualData::findCameraSpline(const char* name)
{
	vector<SaveCameraSplineData>::iterator ci;
	FOR_EACH(cameras, ci)
		if(!strcmp(name, ci->name))
			return &*ci;
	return 0;
}

const SaveCameraSplineData* SaveManualData::findCameraSpline(const char* name) const
{
	vector<SaveCameraSplineData>::const_iterator ci;
	FOR_EACH(cameras, ci)
		if(!strcmp(name, ci->name))
			return &*ci;
	return 0;
}

void SaveManualData::saveCamera(int playerID, const char* triggerName)
{
	if(playerID >= players.size())
		return;
	XBuffer name;
	name < triggerName <= playerID;
	SaveCameraSplineData* spline = findCameraSpline(name);
	if(!spline){
		cameras.push_back(SaveCameraSplineData());
		spline = &cameras.back();
	}
	spline->name = name;
	spline->set(terCamera->coordinate().position(), terCamera->coordinate().psi(), terCamera->coordinate().theta(), terCamera->coordinate().distance());
}

//------------------------------------------------------
const char* editTextMultiLine(HWND hwnd, const char* initialString)
{
	static string name;
	name = editTextMultiLine(initialString, hwnd);
	return name.c_str();
}

const char* editTextDbID(HWND hwnd, const char* initialString, const char* topMask)
{
	static string name;
	name = initialString;

	string mask = topMask;
	qdTextDB::IdList idList;
	vector<const char*> items;

	qdTextDB::instance().getIdList(mask.c_str(), idList);
	qdTextDB::IdList::iterator i;
	FOR_EACH(idList, i)
		items.push_back(i->c_str());

	const char* item = popupMenu(items);
	if(item){
		mask += ".";
		mask += item;

		qdTextDB::instance().getIdList(mask.c_str(), idList);
		items.clear();
		qdTextDB::IdList::iterator i;
		FOR_EACH(idList, i){
			string name = mask + "." + *i;
			const char* comment = qdTextDB::instance().getComment(name.c_str());
			if(strlen(comment)){
				*i += " // ";
				*i += comment;
			}
			items.push_back(i->c_str());
		}
	
		const char* item = popupMenu(items);
		if(item){
			name = item;
			size_t pos = name.find(" // ");
			if(pos != string::npos)
				name.erase(pos, name.size() - pos);
			name = mask + "." + name;
		}
	}

	return name.c_str();
}

const char* editMessageID(HWND hwnd, const char* initialString)
{
	return editTextDbID(hwnd, initialString, "Mission Tips");
}

const char* editTaskID(HWND hwnd, const char* initialString)
{
	return editTextDbID(hwnd, initialString, "Mission Tasks");
}

const char* editMissionDescriptionID(HWND hwnd, const char* initialString)
{
	return editTextDbID(hwnd, initialString, "Mission Description");
}

const char* editModelNameDialog(HWND hwnd, const char* initialString)
{
	static string name;
	name = initialString;
	if(openFileDialog(name, "Resourse\\Missions", "m3d", "Mission Name")){
		strlwr((char*)name.c_str());
		int pos = name.find("resource");
		if(pos != string::npos)
			name.erase(0, pos);
		return name.c_str();
	}
	return 0;
}

const char* editTriggerChainNameDialog(HWND hwnd, const char* initialString)
{
	static string name;
	name = initialString;
	if(openFileDialog(name, "Scripts\\Triggers", "scr", "Trigger Chain Name")){
		strlwr((char*)name.c_str());
		int pos = name.rfind("\\");
		if(pos != string::npos)
			name.erase(0, pos + 1);
		return name.c_str();
	}
	return 0;
}

const char* editLabelDialog(HWND hwnd, const char* initialString)
{
	static string name;
	name = initialString;

	vector<const char*> items;
	PlayerVect::iterator pi;
	FOR_EACH(universe()->Players, pi){
		const UnitList& unit_list=(*pi)->units();
		UnitList::const_iterator ui;
		FOR_EACH(unit_list, ui)
			if(strlen((*ui)->label()))
				items.push_back((*ui)->label());
	}

	const char* item = popupMenu(items);
	if(item){
		name = item;
	}
	return name.c_str();
}

const char* editCameraSplineName(HWND hwnd, const char* initialString)
{
	return gameShell->manualData().popupCameraSplineName();
}

//inline void replace(string& s, const char*src, const char* dest)
//{
//	int pos = 0;
//	while(1){
//		pos = s.find(src, pos);
//		if(pos >= s.size())
//			break;
//		s.replace(pos, strlen(src), dest);
//		pos += strlen(dest);
//		}
//}
//
//const char* SavePlayerManualData::findTriggerChainName(const TriggerChain& triggerChain) 
//{
//	const char* fileName = XFindFirst("Scripts\\Triggers\\*.scr");
//	while(fileName){
//		TriggerChain chain;
//		XBuffer buffer;
//		buffer < "Scripts\\Triggers\\" < fileName;
//		XPrmIArchive(buffer) >> WRAP_NAME(chain, "TriggerChain");
//		if(chain == triggerChain)
//			return fileName;
//		fileName = XFindNext();
//	}
//	static int counter;
//	XBuffer buffer;
//
//	static string name;
//	name = gameShell->CurrentMission.saveName();
//	name.erase(name.size() - 4, name.size()); 
//	strlwr((char*)name.c_str());
//	string subString = "resource\\";
//	size_t pos = name.find(subString);
//	xassert(pos != string::npos);
//	name.erase(0, pos + subString.size());
//	replace(name, "\\", "_");
//	buffer < name.c_str();
//	if(counter++)
//		buffer <= (counter - 1);
//	name = buffer;
//	buffer.init();
//	buffer < "Scripts\\Triggers\\" < name.c_str() < ".scr";
//	XPrmOArchive(buffer) << WRAP_NAME(triggerChain, "TriggerChain");
//	return name.c_str();
//}

//--------------------------------------
const SaveUnitLink& SaveUnitLink::operator=(const terUnitBase* unit)
{
	if(unit){
		unitID = unit->unitID();
		playerID = unit->playerID();
	}
	else
		unitID = 0;
	return *this;
}

void SaveUnitLink::setLink(terUnitBase*& unit) const
{
	unitLink_ = &unit;
	universe()->addLinkToResolve(this);
}

void SaveUnitLink::resolveLink() const
{
	if(unitID){
		*unitLink_ = universe()->findUnit(terUnitID(unitID, playerID));
		xassert(*unitLink_);
	}
	else
		*unitLink_ = 0;
}


