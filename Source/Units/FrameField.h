#ifndef __FRAME_FIELD_H__
#define __FRAME_FIELD_H__

#include "Region.h"
#include "ForceField.h"
#include "FrameCore.h"

class terUnitMonk;
typedef vector<terUnitMonk*> MonkList;
typedef vector<class terProtector*> ProtectorList;

bool removeNotAliveMonk(MonkList& unitList);

//--------------------------
class terProtector : public terBuildingEnergy
{
public:
	enum FieldState {
		FIELD_STOPPED = 1,
		FIELD_TO_START = 2,
		FIELD_STARTING = 4,
		FIELD_STARTED = 8,
		FIELD_TO_STOP = 16,
		FIELD_STOPPING = 32
	};

	terProtector(const UnitTemplate& data);
	~terProtector();

	void Start();
	void setPose(const Se3f& poseIn, bool initPose);

	void MoveQuant();
	void DestroyLink();

	void Kill();

	const MonkList& monks() { return monks_; }

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);
	void UpdateAvatar();

	void ChangeUnitOwner(terPlayer* player);

	void executeCommand(const UnitCommand& command);

	bool canStartField() const { return isBuildingEnable() && !isFieldOn() && chargeLevel() > 1 - FLT_EPS; }

	FieldState fieldState() const { return fieldState_; }
	void setFieldState(FieldState fieldState) { fieldState_ = fieldState; }
	void startField();
	void stopField();
	void monksGoHome();
	bool isFieldOn() const { return fieldState() & (FIELD_STARTING | FIELD_STARTED); }
	
	bool enableCharge() const { return enableCharge_; }
	bool canStartOrJoinToField() const { 
		return isBuildingEnable() && chargeLevel() > fieldPrm.chargeLevelToStartMin && startWhenCharged_;
	}
	float chargeLevel() const { return float(monks_.size())/fieldPrm.monksPerCore; }

	bool isBuildingEnable() const;

	void showDebugInfo();

private:
	terUnitMonk* createMonk();
	MonkList monks_;
	FieldState fieldState_;
	bool enableCharge_;
	bool startWhenCharged_;
	int reinitZeroCounter_;

	void killMonks();
};

#endif //__FRAME_FIELD_H__