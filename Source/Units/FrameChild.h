#ifndef _FRAME_CHILD_
#define _FRAME_CHILD_

#include "RealUnit.h"

class terFrame;

class terFrameChild : public terUnitReal
{
public:
	terFrameChild(const UnitTemplate& data);
	
	void Quant();
	void DestroyLink();

	void WayPointStart();
	void WayPointController();

	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);

	void executeCommand(const UnitCommand& command);

	void SetFramePoint(terFrame* p){ FramePoint = p; }
	terFrame* GetFramePoint(){ return FramePoint; }

	void constructionStart();
	void ConstructionComplete();

	terUnitBase* GetIgnoreUnit(){ return DockPoint; }

	bool alarmStatus() const { return alarmStatus_; }
	bool readyToInput() const { return readyToInput_; }
	void setAlarm(){ alarmStatus_ = true; }
	void clearAlarm();
	void setDockSlot(terDockingSlot* slot);

	void AvatarQuant();

	float chargeLevel() const { return productionConsumptionProgress(); }

	void setSlotNumber(int slotNumber) { slotNumber_ = slotNumber; }
	int slotNumber() const { return slotNumber_; }

	Vect2f homePosition();

	int GetInterfaceLegionMode();

protected:
	terFrame* FramePoint;

	terSoundController* MoveSoundPoint;
	float SpeedFactor;

	bool alarmStatus_;
	bool readyToInput_;

	int slotNumber_;
};

#endif