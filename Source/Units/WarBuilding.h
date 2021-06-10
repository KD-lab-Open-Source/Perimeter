#ifndef __WARBUILDING_H__
#define __WARBUILDING_H__

#include "SecondGun.h"

class terBuildingMilitary : public terBuildingPowered
{
public:
	terBuildingMilitary(const UnitTemplate& data);

//	int CheckStation();	
	void ShowCircles();

	void MoveQuant();
	void Quant();

	void executeCommand(const UnitCommand& command);
	
	int GetInterfaceLegionMode();
	void DestroyLink();

	void setAttackTarget(terUnitReal* target,bool is_manual = false);
	void clearAttackTarget(){ terUnitReal::clearAttackTarget(); manualAttackTarget_ = false; setAttackTarget(NULL); }

	void ChangeUnitOwner(terPlayer* player);

	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);

protected:

	int step_; 

	bool manualAttackTarget_;
	terUnitReal* attackTarget_;
	terUnitReal* lastAttackTarget_;
	DurationTimer targetEventTimer_;

	DurationTimer targetsScanTimer_;

	bool findTarget();
	bool needAttackTarget() const {	
		return !attr().checkWeaponFlag(WEAPON_DISABLE_DEFENCIVE_ATTACK) 
			&& ((!attackTarget_ && wayPoints().empty()) || (!manualAttackTarget_ && attackTarget_ && attackTarget_->possibleDamage() > attackTarget_->damageMolecula().aliveElementCount() + estimatedDamage())); 
	}
};

#endif //__WARBUILDING_H__