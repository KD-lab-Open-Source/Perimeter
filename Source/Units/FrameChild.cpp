#include "StdAfx.h"

#include "Universe.h"
#include "FrameChild.h"

#include "UniverseInterface.h"

#include "IronExplosion.h"

#include "PerimeterSound.h"
#include "Config.h"
#include "IronFrame.h"

terFrameChild::terFrameChild(const UnitTemplate& data) : terUnitReal(data)
{
	MoveSoundPoint = realAvatar()->findSound(SOUND_MOVE);

	FramePoint = 0;
	alarmStatus_ = 0;
	readyToInput_ = false;

	slotNumber_ = 0;

	setCollisionGroup(collisionGroup() | COLLISION_GROUP_REAL);
}

void terFrameChild::DestroyLink()
{
	terUnitReal::DestroyLink();

	if(FramePoint && !(FramePoint->alive()))
		FramePoint = 0;
}

void terFrameChild::WayPointStart()
{
	terUnitReal::WayPointStart();
	SpeedFactor = 0;
}

void terFrameChild::WayPointController()
{
	terUnitReal::WayPointController();

	BodyPoint->way_points.clear();

	if(DockPoint){
		switch(dockMode()){
            default:
                break;
			case DOCK_MODE_COMING:
				DockPhase.Quant();
				if(DockPhase.isEnd())
					setDockMode(DOCK_MODE_LOCK);
				break;
			case DOCK_MODE_NONE:{
				Vect2f target;
				if(FramePoint && position2D().distance2(target = FramePoint->dockingSlot(0) ? 
					FramePoint->dockingSlot(0)->position().trans() : FramePoint->position2D()) > sqr(radius()))
						moveToPoint(To3D(target));
				else{
					setDockMode(DOCK_MODE_COMING);
					DockPhase.startPhase(0,0.025f,0);
					setCollisionGroup(collisionGroup() & ~(COLLISION_GROUP_REAL | COLLISION_GROUP_ENEMY));
				}
				break;
			}
		}
	}
	else{
		if(!wayPoints().empty()){
			if(BodyPoint->is_point_reached(wayPoints().front()))
				wayPoints_.erase(wayPoints_.begin());
			else
				moveToPoint(wayPoints().front());
		}

		if(FramePoint && alarmStatus_){
			if(FramePoint->inputPosition().distance2(position2D()) > 100)
				moveToPoint(To3D(FramePoint->inputPosition()));
			else
				readyToInput_ = true;
		}
	}
}

void terFrameChild::Quant()
{
	terUnitReal::Quant();

	average(SpeedFactor, BodyPoint->speedFactor(), 0.2f);
}

Vect2f terFrameChild::homePosition() 
{
	float a = (2*XM_PI*slotNumber_)/5;
	float r = FramePoint->attr()->mmpHomePositionRadius;
	return FramePoint->position2D() + Vect2f(r*xm::cos(a), r*xm::sin(a));
}

void terFrameChild::executeCommand(const UnitCommand& command)
{
	terUnitReal::executeCommand(command);

	clearAlarm();
	
	switch(command.commandID()){
	case COMMAND_ID_OBJECT:
		if(command.unit() == FramePoint){
			wayPoints_.clear();
			alarmStatus_ = true;
			setTargetUnit(0);
		}
		else if(targetUnit() && (targetUnit()->Player != Player || !targetUnit()->attr()->isBuilding()))
			setTargetUnit(0);
		break;

	case COMMAND_ID_STOP:
		clearAlarm();
		break;
		
    default:
        break;
	}
}

void terFrameChild::clearAlarm()
{
	alarmStatus_ = 0;
	readyToInput_ = false;
	if(DockSlot){
		if(dockMode() == DOCK_MODE_NONE)
			clearDockingPoint();
	}
}

void terFrameChild::AvatarQuant()
{
	terUnitReal::AvatarQuant();

	realAvatar()->setSight(SightFactor);
	realAvatar()->setHot(HotFactor);
	avatar()->setPose(BodyPoint->pose());

	if(MoveSoundPoint){
		MoveSoundPoint->setVolume(SpeedFactor);
		MoveSoundPoint->setFrequency(SpeedFactor);
		MoveSoundPoint->play();
	}
}

void terFrameChild::constructionStart()
{ 
	setCollisionGroup(collisionGroup() & ~(COLLISION_GROUP_REAL | COLLISION_GROUP_ENEMY));
}

void terFrameChild::ConstructionComplete()
{ 
	setCollisionGroup(collisionGroup() | COLLISION_GROUP_REAL | COLLISION_GROUP_ENEMY);
	if(DockSlot)
		DockSlot->UnitPoint = 0;
}

SaveUnitData* terFrameChild::universalSave(SaveUnitData* baseData)
{
	SaveUnitFrameChildData* data = castOrCreate<SaveUnitFrameChildData>(baseData);
	terUnitReal::universalSave(data);

	data->alarmStatus = alarmStatus_;

	return data;
}

void terFrameChild::universalLoad(SaveUnitData* baseData)
{
	terUnitReal::universalLoad(baseData);

	SaveUnitFrameChildData* data = dynamic_cast<SaveUnitFrameChildData*>(baseData);
	if(data){
		alarmStatus_ = data->alarmStatus;
	}
}

void terFrameChild::setDockSlot(terDockingSlot* slot)
{
	DockSlot = slot;
	DockPoint = FramePoint;
	DockSlot->UnitPoint = this;
}

int terFrameChild::GetInterfaceLegionMode()
{
	if(alarmStatus() || DockSlot)
		return attr()->ID == FramePoint->slot(slotNumber()).ProductionID ?
			INTERFACE_LEGION_MODE_BACK_TO_FRAME : INTERFACE_LEGION_MODE_EXCHANGE;

	if(!wayPoints().empty())
		return INTERFACE_LEGION_MODE_MOVE;

	return INTERFACE_LEGION_MODE_STOP;
}
