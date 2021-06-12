#include "StdAfx.h"

#include "Runtime.h"

#include "Universe.h"
#include "Player.h"
#include "IronBullet.h"
#include "RealUnit.h"
#include "RealInterpolation.h"

#include "Config.h"

#include "GridTools.h"

#include "PerimeterSound.h"
#include "IronExplosion.h"
#include "IronFrame.h"
#include "IronBuilding.h"
#include "WarBuilding.h"
#include "Nature.h"

#include "UniverseInterface.h"
#include "Triggers.h"

#include "AIPrm.h"

terBuildingMilitary::terBuildingMilitary(const UnitTemplate& data) : terBuildingPowered(data),
	attackTarget_(NULL),
	lastAttackTarget_(NULL)
{
	step_ = 0;
	manualAttackTarget_ = false;
}

void terBuildingMilitary::ShowCircles()
{
	terUnitBase::ShowCircles();
	if(attr().ShowCircles){
		if(attr().fireRadius())
			terCircleShow(position(),position(),attr().fireRadius(), circleColors.fireRadius);
		if(attr().fireRadiusMin())
			terCircleShow(position(),position(),attr().fireRadiusMin(), circleColors.fireRadiusMin);
	}

	if (
			attr().ID == UNIT_ATTRIBUTE_GUN_SCUM_DISRUPTOR
		||	attr().ID == UNIT_ATTRIBUTE_GUN_BALLISTIC
		||	attr().ID == UNIT_ATTRIBUTE_GUN_FILTH_NAVIGATOR ) {

		Vect3f target;
		bool hasTarget = false;
		if (!wayPoints().empty()) {
			target = wayPoints().back();
			hasTarget = true;
		} else if (manualAttackTarget_ && attackTarget_) {
			target = attackTarget_->position();
			hasTarget = true;
		}

		if (hasTarget) {
			step_++;
			if (step_ >= targetCircleStep) {
				step_ = 0;
			}
			float rStep = targetCircleMaxRadius / targetCircleCount;
			float rShift = rStep / targetCircleStep;
			float curShift = step_ * rShift;
			for (int i = 0; i < targetCircleCount; i++) {
				float r = rStep * i - curShift;
				terCircleShow(target, target, r + rShift, r, circleColors.targetRadius);
			}
			terCircleShow(target, target,  targetTrianglesRadius, circleColors.targetTriangles);
			terCircleShow(target, target,  targetTrianglesRadius, circleColors.targetCross);
		}
	}
}

void terBuildingMilitary::setAttackTarget(terUnitReal* target,bool is_manual)
{
	if(attackTarget_){
		attackTarget_->addPossibleDamage(-estimatedDamage());
		if(manualAttackTarget_)
			lastAttackTarget_ = attackTarget_;
	}

	if(target)
		target->addPossibleDamage(estimatedDamage());

	if(is_manual)
		lastAttackTarget_ = NULL;

	manualAttackTarget_ = is_manual;
	attackTarget_ = target;

	targetEventTimer_.stop();
}

void terBuildingMilitary::Quant()
{
	terBuildingPowered::Quant();

	if(attackTarget_ && !targetEventTimer_){
		targetEventTimer_.start(targetEventTime + terLogicRND(targetEventTime));
		universe()->checkEvent(EventUnitMyUnitEnemy(Event::AIM_AT_OBJECT, attackTarget_, this));
	}
}

void terBuildingMilitary::executeCommand(const UnitCommand& command)
{
	terBuildingPowered::executeCommand(command);

	switch(command.commandID()){
	case COMMAND_ID_POINT:
		setAttackTarget(NULL,true);
		xassert(!wayPoints().empty());
		break;
	case COMMAND_ID_OBJECT:
		if(!command.unit() || command.unit()->Player->clan() != Player->clan())
			setAttackTarget(safe_cast<terUnitReal*>(command.unit()),true);
		break;
	case COMMAND_ID_STOP: 
		setAttackTarget(NULL,true);
		break;
	}
}

void terBuildingMilitary::MoveQuant()
{
	start_timer_auto(WarBuilding,STATISTICS_GROUP_UNITS);

	terBuildingPowered::MoveQuant();

	if(isBuildingEnable()){
		if(attackTarget_){
			if((!attackTarget_->GetLegionMorphing() || attackTarget_->includingCluster() != includingCluster() || attackTarget_->isUnseen()))
				setAttackTarget(NULL);
		}
		
		if(needAttackTarget()){
			if(!targetsScanTimer_())
				findTarget();
		}

		int status = 0;

		bool st = wayPoints().empty();

		if(attackTarget_){
			fireRequest(0,attackTarget_,status);
		}
		else {
			if(!wayPoints().empty()) {
                Vect3f v(wayPoints().front());
                fireRequest(&v, 0, status);
            }
		}
		if(status & (LEGION_FIRE_STATUS_GROUND_OBSTACLE | LEGION_FIRE_STATUS_FRIENDLY_FIRE | LEGION_FIRE_STATUS_BORDER_ANGLE | LEGION_FIRE_STATUS_DISTANCE | LEGION_FIRE_STATUS_FIELD_OBSTACLE | LEGION_FIRE_STATUS_BAD_TARGET))
			setAttackTarget(NULL);
		if(status & LEGION_FIRE_STATUS_NOT_ENEMY_TARGET && !manualAttackTarget_)
			setAttackTarget(NULL);

		if(lastAttackTarget_){
			if(fireCheck(lastAttackTarget_->position(),lastAttackTarget_))
				setAttackTarget(lastAttackTarget_,true);
		}
	}
}

bool terBuildingMilitary::findTarget()
{
	terUnitGridTeamOffensiveOperator op(this);
	universe()->UnitGrid.Scan(round(position().x), round(position().y), round(attr().sightRadius()), op);

	targetsScanTimer_.start(static_gun_targets_scan_period);

	if(op.offensivePoint()){
		setAttackTarget(safe_cast<terUnitReal*>(op.offensivePoint()));
		return true;
	}
	else 
		return false;
}

int terBuildingMilitary::GetInterfaceLegionMode()
{
	int stop = 0;
	int attack = 0;

	switch(lastCommand()){
	case COMMAND_ID_POINT:
	case COMMAND_ID_OBJECT:
		attack = 1;
		break;
	case COMMAND_ID_STOP:
		stop = 1;
		break;
	case COMMAND_ID_NONE:
		break;
	//default:
		//xassert(0);
	}

	if(stop)
		return INTERFACE_LEGION_MODE_STOP;
	else{
		if(attack || attackTarget_)
			return INTERFACE_LEGION_MODE_ATTACK;
	}
	return INTERFACE_LEGION_MODE_STOP;
}

void terBuildingMilitary::DestroyLink()
{
	terBuildingPowered::DestroyLink();

	if(attackTarget_ && (!attackTarget_->alive() || !attackTarget_->damageMolecula().isAlive()))
		attackTarget_ = NULL;

	if(lastAttackTarget_ && (!lastAttackTarget_->alive() || !lastAttackTarget_->damageMolecula().isAlive()))
		lastAttackTarget_ = NULL;
}


void terBuildingMilitary::ChangeUnitOwner(terPlayer* player)
{
	terBuildingPowered::ChangeUnitOwner(player);

	manualAttackTarget_ = false;
	setAttackTarget(NULL);
}

SaveUnitData* terBuildingMilitary::universalSave(SaveUnitData* baseData)
{
	SaveUnitBuildingMilitaryData* data = castOrCreate<SaveUnitBuildingMilitaryData>(baseData);
	terBuildingPowered::universalSave(data);

	data->attackTarget = attackTarget_;
	data->lastAttackTarget = lastAttackTarget_;
	data->manualAttackTarget = manualAttackTarget_;

	if(attr().ID == UNIT_ATTRIBUTE_GUN_BALLISTIC && weapon_ && weapon_->missileCount())
		data->weaponChargeLevel = 1;

	return data;
}

void terBuildingMilitary::universalLoad(const SaveUnitData* baseData)
{
	terBuildingPowered::universalLoad(baseData);
	
	const SaveUnitBuildingMilitaryData* data = dynamic_cast<const SaveUnitBuildingMilitaryData*>(baseData);
	if(data){
		data->attackTarget.setLink((terUnitBase*&)attackTarget_);
		data->lastAttackTarget.setLink((terUnitBase*&)lastAttackTarget_);
		manualAttackTarget_ = data->manualAttackTarget;
	}
}
