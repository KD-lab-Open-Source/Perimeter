#ifndef __REALUNIT_H__
#define __REALUNIT_H__

#include "GenericUnit.h"
#include "RigidBody.h"
#include "ToolzerController.h"
#include "Interpolation.h"
#include "EnergyConsumer.h"
#include "SafeCast.h"

class terUnitReal;

class terDockingSlot
{
public:
	void init(cLogicObject* logicObject, const char* position_control);
	void DestroyLink();

	const MatXf& position() const { 
		return PositionControl->GetGlobalMatrix(); 
	}

	int timer() const { return timer_(); }
	void startTimer(int time){ timer_.start(time); }
	void stopTimer(){ timer_.stop(); }

	terUnitReal* UnitPoint;

private:
	cLogicTile* PositionControl;

	DelayTimer timer_;
};

enum terDockMode
{
	DOCK_MODE_NONE = 0,
	DOCK_MODE_LOCK,
	DOCK_MODE_COMING,
	DOCK_MODE_LEAVE
};

//--------------------------------

class terUnitReal;

class terEffectController
{
public:
	terEffectController(const terUnitEffectData* setup,cEffect* effect);
	~terEffectController();

	bool operator == (terEffectID id) const { xassert(setup_); return (setup_->effectID == id); }

	void quant(terUnitReal* owner);
	void avatarInterpolation();
	void avatarQuant(terUnitReal* owner);

	void kill();

	bool createEffect(terUnitReal* owner);

private:

	cEffect* effect_;
	InterpolatorParticleRate effectRate_;
	InterpolatorPose effectPose_;

	const terUnitEffectData* setup_;
};

//--------------------------------
class AttributeReal : public AttributeBase
{
public:
	vector<PrmString> dockingSlots;
	ConsumptionData productionConsumption;
	ConsumptionData movementConsumption;

	int FallDelay;
	float FallSpeed;
	float FallAcceleration;

	AttributeReal();

	template<class Archive>
	void serialize(Archive& ar) {
		__super::serialize(ar);
		if(ar.openBlock("attributeReal", "Real unit")){
			ar & TRANSLATE_OBJECT(dockingSlots, "Слоты");
			ar & TRANSLATE_OBJECT(productionConsumption, "Энергия на производство");
			ar & TRANSLATE_OBJECT(movementConsumption, "Энергия на перемещение");

			ar & TRANSLATE_OBJECT(FallDelay, "FallDelay");
			ar & TRANSLATE_OBJECT(FallSpeed, "FallSpeed");
			ar & TRANSLATE_OBJECT(FallAcceleration, "FallAcceleration");

			ar.closeBlock();
		}
	}
};

//--------------------------------
class terUnitReal : public terUnitGeneric
{
public:
	float Scale;

	class RigidBody* BodyPoint;

	float SightFactor;

	int HotCount;
	float FreezeFactor;
	float HotFactor;
	float HealFactor;

	terUnitReal(const UnitTemplate& data);
	virtual ~terUnitReal();

	const AttributeReal& attr() const {
		return safe_cast_ref<const AttributeReal&>(__super::attr());
	}

	void Start();
	void Kill();

	void MoveQuant();
	void Quant();

	void DestroyLink();

	void executeCommand(const UnitCommand& command);

	float GetScale(){ return Scale; };

	void setPose(const Se3f& pose, bool initPose);

	float freezeFactor() const { return FreezeFactor; }

	virtual void WayPointStart(){}
	virtual void WayPointController(){}

//Logic Object
	cLogicObject* LogicObjectPoint;

	virtual void UpdateLogicPoint();

	RigidBody* GetRigidBodyPoint() const { return BodyPoint; };

	class terInterpolationReal* realAvatar() {return (terInterpolationReal*)avatar();}
	const class terInterpolationReal* realAvatar() const {return (terInterpolationReal*)avatar();};

	void AvatarInterpolation();
	void AvatarQuant();

	void setDamage(const DamageData& damage,terUnitBase* p = 0);

//--------------------
	void RefreshAttribute();

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);

	void showDebugInfo();

	void showPath(const vector<Vect3f>& wayPoints);
	const vector<Vect3f>& getWayPoints() {
		return wayPoints_;
	}

	// Basement
	float basementDamage();
	bool basementReady();
	bool basementInstalled() const { return basementInstalled_; }
	void installBasement();
	void uninstallBasement();

	// Docking for Parent
	terDockingSlot* requestDockingSlot();
	terDockingSlot* requestDockingSlot(int offset);
	terDockingSlot* dockingSlot(int index){ xassert(index >= 0 && index < DockList.size()); return &*(DockList.begin() + index); }
	bool isDockReady(int index) const;
	int docksCount() const { return DockList.size(); }

	// Docking for Child
	void SetDockingPoint(terUnitReal* unit, terDockingSlot* slot, terDockMode mode);
	void clearDockingPoint();
	terDockMode dockMode() const { return dockMode_; }
	void setDockMode(terDockMode mode) { dockMode_ = mode; }

	void SetFreezeCount(int count){ freezeTimer_.start(count); }
	void SetHotCount(int count){ HotCount = count; handleLaserHit(); }
	void setHealTimer(int time){ healTimer_.start(time); }

	void setRealModel(int modelIndex, float scale);
	
	void moveToPoint(const Vect3f& v);

	Vect3fList& wayPoints() { return wayPoints_; }
	const Vect3fList& wayPoints() const { return wayPoints_; }

	bool isMoving() const { return !BodyPoint->way_points.empty();	}
	
	terUnitBase* targetUnit() const { return targetUnit_; }
	void setTargetUnit(terUnitBase* targetUnit) { targetUnit_ = targetUnit; }

	const Vect3f& interpolatedPosition();

	void ShowInfo();
	
	bool fireRequest(const Vect3f* position,terUnitBase* target,int& fire_status);
	bool fireCheck(const Vect3f& position,terUnitBase* target) const;
	bool fireDistanceCheck(const Vect3f& position) const;
	void fireStop();
	virtual void clearAttackTarget(){ wayPoints().clear(); }

	bool isWeaponLoaded() const;
	bool isWeaponReady() const;
	bool weaponReload();
	void removeMissileReference(terUnitBase* p);

	void explode();
	void splashDamage();

	float accumulatedEnergy() const { return accumulatedEnergy_; }
	float addAccumulatedEnergy(float delta); // Возвращает, сколько реально принял/отдал

	float productionProgress();	// Запрашивает энергию на производство, восвращает прогресс производства
	bool productionCompleted(){ return productionProgress() > 1 - FLT_EPS; }

	virtual void checkField();

	int estimatedDamage() const;

	float chargeLevel() const;
	bool isFiring() const;
	void ChangeUnitOwner(terPlayer* player);

	bool isDocked() const { return DockPoint; }//dockMode_ != DOCK_MODE_NONE; }

	/// Юнит, по которому стреляет в данный момент.
	terUnitBase* weaponTarget();

	virtual bool effectsEnabled() const { return true; }

	int zeroLayerPlaced() const { return zeroLayerCounter_; }
	void placeZeroLayer(bool restore);
	void freeZeroLayer();

protected:
	float productionConsumptionProgress() const { return productionConsumption_.progress(); }

	terDockMode dockMode_;
	terUnitReal* DockPoint;
	terDockingSlot* DockSlot;
	typedef vector<terDockingSlot> DockingSlotList;
	DockingSlotList DockList;
	terAnimationPhaseIteratorType DockPhase;
	terAnimationPhaseIteratorType DockAnglePhase;
	bool AngleDockingEnable;

	bool basementInstalled_;

	Vect3fList wayPoints_;
	terUnitBase* targetUnit_;

	terUnitReal* ownerUnit_;

	class terWeapon* weapon_;
	ToolzerController toolzerController_;

	/// Возвращает ID желаемой цепочки анимации.
	virtual ChainID chainRequest() const;
	virtual bool needWeaponDisable() const { return (!isConstructed() || BodyPoint->underMutation() || freezeTimer_()); }

	void weaponQuant();

private:
	DurationTimer recalcPathTimer_;
	bool pathFindSucceeded_;
	Vect2i pathFindTarget_;
	vector<Vect2i> pathFindList_;

	/// таймер для расчета повреждений от хаоса и нарушенного зерослоя
	DelayTimer envDamageTimer_;

	DurationTimer freezeTimer_;
	DurationTimer healTimer_;

	float accumulatedEnergy_;
	EnergyConsumer productionConsumption_;

	int checkFieldCluster_;
	DurationTimer checkFieldTimer_;

	int zeroLayerCounter_;
	static int zeroLayerCounterCurrent_;

	void handleLaserHit();
protected:
	typedef list<terEffectController> terEffectControllerList;
	terEffectControllerList effectControllers_;
};

#endif //__REALUNIT_H__