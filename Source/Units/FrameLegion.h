#ifndef __FRAMELEGION_H__
#define __FRAMELEGION_H__

#include "Squad.h"
#include "FramePlant.h"

class terBuildingCommandCenter : public terBuildingPowered
{
public:
	terBuildingCommandCenter(const UnitTemplate& data);

	void Kill();

	void setPose(const Se3f& pose, bool initPose);
	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);

	terUnitSquad* GetSquadPoint(){ return SquadPoint; };

	void ChangeUnitOwner(terPlayer* player);

private:
	terUnitSquad* SquadPoint;
};

#endif //__FRAMELEGION_H__