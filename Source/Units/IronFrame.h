#ifndef __IRONFRAME_H__
#define __IRONFRAME_H__

#include "FrameChild.h"
#include "FrameLegion.h"
#include "EnergyConsumer.h"
#include "Installer.h"

const int FRAME_SLOTS_MAX = 5;

struct terFrameSlot
{	
	enum Status
	{
		STATUS_FREE = 0,
		STATUS_PRODUCTION,
		STATUS_COMPLETE,
		STATUS_BUSY
	};

	terFrameSlot();

	void init();
	void quant();

	terUnitAttributeID ProductedUnit() const { return ProductionID; }
	int productionProgress() const { return 1 + 99*productionConsumption_.progress(); }

	bool needInput() const { return UnitPoint && UnitPoint->readyToInput(); }
	bool needOutput() const { return status_ == STATUS_COMPLETE && ProductionID != UNIT_ATTRIBUTE_NONE; }

	Status status_;
	terFrameChild* UnitPoint;
	terUnitAttributeID ProductionID;
	EnergyConsumer productionConsumption_;
};


class AttributeFrame : public AttributeReal
{
public:
	int heightMax;
	float accumulatedEnergyInitial; // 0..1, только для вновь установленного фрейма, иначе - в редакторе миссий

	ConsumptionData antigravConsumption;
	ConsumptionData annihilationConsumption;
	ConsumptionData repairConsumption;

	float repairElementsPerQuant;
	DamageData fallDamage;
	float oneStepMovement;
	
	float energyReserveToChargeSpiral;
	
	float mmpHomePositionRadius;
	
	float normalVelocity;
	float teleportationVelocity;

	AttributeFrame();

	template<class Archive>
	void serialize(Archive& ar) {
		__super::serialize(ar);
		if(ar.openBlock("attributeFrame", "Фрейм")){
			ar & TRANSLATE_OBJECT(heightMax, "heightMax");
			ar & TRANSLATE_OBJECT(accumulatedEnergyInitial, "accumulatedEnergyInitial");

			ar & TRANSLATE_OBJECT(antigravConsumption, "antigravConsumption");
			ar & TRANSLATE_OBJECT(annihilationConsumption, "annihilationConsumption");
			ar & TRANSLATE_OBJECT(repairConsumption, "repairConsumption");
			
			ar & TRANSLATE_OBJECT(repairElementsPerQuant, "repairElementsPerQuant");
			ar & TRANSLATE_OBJECT(fallDamage, "fallDamage");
			ar & TRANSLATE_OBJECT(oneStepMovement, "oneStepMovement");

			ar & TRANSLATE_OBJECT(energyReserveToChargeSpiral, "energyReserveToChargeSpiral");
			ar & TRANSLATE_OBJECT(mmpHomePositionRadius, "mmpHomePositionRadius");
			ar & TRANSLATE_OBJECT(normalVelocity, "normalVelocity");
			ar & TRANSLATE_OBJECT(teleportationVelocity, "teleportationVelocity");
			ar.closeBlock();
		}
 	}
};

class terFrame : public terUnitReal
{
public:
	enum FrameStatus
	{
		ISOLATED,
		MOBILE,
		ATTACHED
	};

	terFrame(const UnitTemplate& data);
	~terFrame();
	
	const AttributeFrame& attr() const {
		return safe_cast_ref<const AttributeFrame&>(terUnitReal::attr());
	}

	void DestroyLink();
	
	void setPose(const Se3f& pose, bool initPose);
	void setRealModel(int modelIndex, float scale);

	void AvatarQuant();
	void Quant();
	void MoveQuant();

	void WayPointStart();
	void WayPointController();

	void Kill();
	void ChangeUnitOwner(terPlayer* player);

	void executeCommand(const UnitCommand& command);

	void MapUpdateHit(float x0,float y0,float x1,float y1);

	terUnitBase* GetProductionPoint(){ return activeSlot_ ? activeSlot_->UnitPoint : 0; };
	terUnitSquad* GetSquadPoint(){ return SquadPoint; };

	void attach();
	void detach();
	void finishIO();

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);

	void GetInterfaceFrameProduction(int* phase,int* id,terUnitBase* unit[FRAME_SLOTS_MAX]);

	terFrameSlot& slot(int index){ xassert(index >= 0 && index < FRAME_SLOTS_MAX); return frameSlots_[index]; }

	FrameStatus frameStatus() const { return ValidStatus; }
	bool attached() const { return ValidStatus == ATTACHED; }
	bool attaching() const { return RequestStatus == ATTACHED; }
	bool powered() const { return powered_; }
	bool isBuildingEnable() const { return powered() || attached(); } //{ return powered_ && pillar_->chainID() == CHAIN_SWITCHED_ON; }
	bool readyToConnect() const;
//	int isSingleSelection(){ return ValidStatus == ATTACHED && RequestStatus == ATTACHED; }
	int isSingleSelection(){ return 0; }
	bool isBuilding() const { return ValidStatus == ATTACHED; }

	void setDamage(const DamageData& damage, terUnitBase* p);

	void setPowered(bool powered);
	void restorePosition();

	int GetInterfaceLegionMode();

	bool canTeleportate() const;
	bool underTeleportation() const { return underTeleportation_; }

	void ShowInfo();
	void showDebugInfo();

	bool analyzeTerrain();

	float spiralLevel() const { return spiralConsumer_.progress(); }

	bool isSlotUnit(int slotNumber, terUnitBase* unit) { return unit == frameSlots_[slotNumber].UnitPoint; }

	Vect2f inputPosition() const { return position2D() + inputPositionOffset_; }

private:
	enum DockStatus
	{
		DOCK_STATUS_NONE,
		DOCK_STATUS_INPUT_PREPARE,
		DOCK_STATUS_INPUT,
		DOCK_STATUS_OUTPUT_PREPARE,
		DOCK_STATUS_OUTPUT
	};

	bool powered_;
	FrameStatus RequestStatus, ValidStatus;	

	terFrameSlot* activeSlot_;
	terFrameSlot frameSlots_[FRAME_SLOTS_MAX];
	
	terUnitSquad* SquadPoint;
	
	DockStatus dockStatus_;
	
	float SpeedFactor;
	terSoundController* MoveSoundPoint;
	terSoundController* productionSound_;
	
	int SoundAttachStatus;
	
	NodeController* pillar_;
	NodeController* animate_;

	bool underTeleportation_;

	EnergyConsumer antigravConsumer_;
	EnergyConsumer spiralConsumer_;
	float spiralLevelPrev_;

	EnergyConsumer movementConsumption_;
	EnergyConsumer repairConsumption_;

	terBuildingInstaller installer_;

	bool catched_;
	DurationTimer poweredTimer_;

	terBelligerent nativeBelligerent_;

	Vect2f inputPositionOffset_;
};

#endif //__IRONFRAME_H__