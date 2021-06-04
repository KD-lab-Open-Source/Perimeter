#include "StdAfx.h"

#include "Universe.h"
#include "Player.h"

#include "IronExplosion.h"
#include "Config.h"

//-----------------------------------------------------------------------------------------------
terBuildingCommandCenter::terBuildingCommandCenter(const UnitTemplate& data) : terBuildingPowered(data)
{
	SquadPoint = safe_cast<terUnitSquad*>(Player->buildUnit(UNIT_ATTRIBUTE_SQUAD));
	SquadPoint->setCommander(this);
}

void terBuildingCommandCenter::Kill()
{
	terBuildingPowered::Kill();
	SquadPoint->Kill();
}

void terBuildingCommandCenter::setPose(const Se3f& pose, bool initPose)
{
	terBuildingPowered::setPose(pose, initPose);
	if(initPose){
		Se3f poseShifted = pose;
		poseShifted.trans() += to3D(SquadPoint->attr().homePositionOffsetFactor*radius(), 0);
		SquadPoint->setPose(poseShifted, initPose);
		SquadPoint->setHomePosition(poseShifted.trans());
	}
}

void terBuildingCommandCenter::ChangeUnitOwner(terPlayer* player)
{
	terBuildingPowered::ChangeUnitOwner(player);
	SquadPoint->ChangeUnitOwner(player);
}

SaveUnitData* terBuildingCommandCenter::universalSave(SaveUnitData* baseData)
{
	SaveUnitCommandCenterData* data = castOrCreate<SaveUnitCommandCenterData>(baseData);
	terBuildingPowered::universalSave(data);
	
	data->squad = SquadPoint->universalSave(0);
	return data;
}

void terBuildingCommandCenter::universalLoad(const SaveUnitData* baseData)
{
	const SaveUnitCommandCenterData* data = safe_cast<const SaveUnitCommandCenterData*>(baseData);
	terBuildingPowered::universalLoad(data);
	
	if(data->squad){
		SquadPoint->universalLoad(data->squad);
		SquadPoint->Start();
	}
}


