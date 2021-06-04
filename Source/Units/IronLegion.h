#ifndef __IRONLEGION_H__
#define __IRONLEGION_H__

#include "RealUnit.h"
#include "terra.h"
#include "Squad.h"

////////////////////////////////////////////////////////////////////////////////
//	Интерфейс с юнитами
////////////////////////////////////////////////////////////////////////////////
enum LegionFireStatus
{
	LEGION_FIRE_STATUS_GROUND_OBSTACLE = 1,
	LEGION_FIRE_STATUS_FRIENDLY_FIRE = 2,
	LEGION_FIRE_STATUS_RELOAD_AMMO = 4,
	LEGION_FIRE_STATUS_DISTANCE = 8,
	LEGION_FIRE_STATUS_FIELD_OBSTACLE = 32,
	LEGION_FIRE_STATUS_HORIZONTAL_ANGLE = 64,
	LEGION_FIRE_STATUS_VERTICAL_ANGLE = 128,
	LEGION_FIRE_STATUS_BORDER_ANGLE = 256,
	LEGION_FIRE_STATUS_BAD_TARGET = 512,
	LEGION_FIRE_STATUS_NOT_ENEMY_TARGET = 1024
};

enum LegionActionStatus
{
	LEGION_ACTION_STATUS_MOVE = 1,
	LEGION_ACTION_STATUS_ATTACK = 2,
	LEGION_ACTION_STATUS_DISINTEGRATE = 4,
	LEGION_ACTION_STATUS_INTEGRATE = 8,
	LEGION_ACTION_STATUS_FREEZE = 32
};

class AttributeLegionary : public AttributeReal
{
public:
	int is_base_unit;
	float formationRadiusFactor;

	int dynamicAttack;
	float attackTurnRadius;
	float attackAngleDeviation;

	int ignoreUninstalledFrame;
	int dontUseDamageMapPathFind;

	EnumWrapper<terLegionType> LegionType;

	bool destroyZeroLayer;
    
	vector<EnumWrapper<terUnitAttributeID> > EnableStructure;
    	
	AttributeLegionary();

	template<class Archive>
	void serialize(Archive& ar) {
		__super::serialize(ar);
		if(ar.openBlock("attributeLegionary", "Легионер")){
			ar & TRANSLATE_OBJECT(is_base_unit, "is_base_unit");
			ar & TRANSLATE_OBJECT(formationRadiusFactor, "formationRadiusFactor");
			ar & TRANSLATE_OBJECT(dynamicAttack, "dynamicAttack");
			ar & TRANSLATE_OBJECT(attackTurnRadius, "attackTurnRadius");
			ar & TRANSLATE_OBJECT(attackAngleDeviation, "attackAngleDeviation");
			ar & TRANSLATE_OBJECT(ignoreUninstalledFrame, "ignoreUninstalledFrame");
			ar & TRANSLATE_OBJECT(dontUseDamageMapPathFind, "dontUseDamageMapPathFind");
			ar & TRANSLATE_OBJECT(LegionType, "LegionType");
			ar & TRANSLATE_OBJECT(destroyZeroLayer, "Разрушает зерослой");
			ar & TRANSLATE_OBJECT(EnableStructure, "EnableStructure");
			ar.closeBlock();
		}
	}
};

class terUnitLegionary : public terUnitReal
{
public:
	terUnitLegionary(const UnitTemplate& data);
	~terUnitLegionary() {}

	const AttributeLegionary& attr() const {
		return safe_cast_ref<const AttributeLegionary&>(terUnitReal::attr());
	}

	void Start();
	void finalizeConstruction();

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);
	
	void DestroyLink();

	void Quant();
	void Kill();

	void WayPointController();

	void setRequestStatus(int request);
	int requestStatus() const { return requestStatus_; }
	int fireStatus() const { return fireStatus_; }
	int responseStatus() const { return responseStatus_; }

	int RequestScriptMove();
	int RequestScriptDisintegrate(){ return !BodyPoint->underMutation(); }
	int RequestScriptIntegrate(){ return !BodyPoint->underMutation(); }
	int RequestScriptFreeze();

	void SetSquadPoint(class terUnitSquad* p);
	terUnitSquad* GetSquadPoint() { return SquadPoint; }

	bool selectAble() const;

	int GetLegionMorphing() const { return !(requestStatus() & (LEGION_ACTION_STATUS_DISINTEGRATE | LEGION_ACTION_STATUS_INTEGRATE)); }
	int GetLegionMutable() const { return !(requestStatus() & (LEGION_ACTION_STATUS_DISINTEGRATE | LEGION_ACTION_STATUS_INTEGRATE | LEGION_ACTION_STATUS_FREEZE)); }

	void UnitDamageCheck();
	void SetGenericDamage(float damage,int damage_type,terUnitBase* p);
	void setDamage(const DamageData& damage,terUnitBase* p = NULL);

	//----------------------------
	terUnitSquad* getSquad() const { return SquadPoint; }
	
	const Vect2f& localPosition() const { return localPosition_; }
	void setLocalPosition(const Vect2f& p);
	void addWayPoint(const Vect2f& p);
	void setAttackPoint(terUnitBase* attack_target,bool is_manual = false);
	void setAttackPosition(const Vect3f& attack_position,bool is_manual = false);
	void clearAttackTarget(){ terUnitReal::clearAttackTarget(); manualAttackTarget_ = false; setAttackPoint(NULL); }
	void quant();
	void stop();

	float formationRadius() const;
	void correctSpeed(float averagePath);
	float deltaPath() const;
	
	bool isUnseen() const { return (invisibilityTimer_()) ? true : false; }
	void setInvisibility(int time);
	bool isFlying() const { return unitClass() & (UNIT_CLASS_AIR | UNIT_CLASS_AIR_HEAVY); }

	void setInSquad();
	bool inSquad() const { return inSquad_;	}
	
	void showDebugInfo();
	
	//----------------------------
	void AvatarQuant();
	
	//-----------------------------
	int GetFireControlCheck(terUnitBase* target){ return (target->unitClass() & attr().AttackClass); };

	const DamageMolecula& transportAtom() const { return transportAtom_; }
	void addToTransport(int atom_id, int numAtoms = 1);

	bool needAttackTarget() const 
	{	
		if(!hasAttackPosition_)
			return (!attackTarget_ || (!manualAttackTarget_ && attackTarget_->possibleDamage() > attackTarget_->damageMolecula().aliveElementCount() + estimatedDamage()));
		else
			return !manualAttackTarget_;
	}

	bool hasAttackTarget() const
	{
		return (attackTarget_ != NULL);
	}

	bool isDisintegrating() const { return isDisintegrating_; }
	void toggleDisintegrate(){ isDisintegrating_ = true; }

	bool effectsEnabled() const { return (!isUnseen() && !BodyPoint->underMutation()); }

protected:
	/// Возвращает ID желаемой цепочки анимации.
	ChainID chainRequest() const;

private:
	class terUnitSquad* SquadPoint;

	int requestStatus_;
	int fireStatus_;
	int responseStatus_;
		
	bool inSquad_;

	/// бкдет развален на базовые в ближайшее время
	bool isDisintegrating_;
	
	Vect2f localPosition_;
	float speed_factor;
	
	/// true если цель для атаки выбрана
	bool manualAttackTarget_;
	terUnitBase* attackTarget_;
	DurationTimer targetEventTimer_;

	/// true если цель для атаки - точка на мире
	bool hasAttackPosition_;
	Vect3f attackPosition_;

	DurationTimer invisibilityTimer_;

	vector<Vect2f> out_path; // Путь к цели, персонально для этого юнита
	mutable float deltaPath_;
	
	//-------------------
	float SpeedFactor;
	terSoundController* MoveSoundPoint;

	const terUnitLegionary* transportPoint_;
	DamageMolecula transportAtom_;

	PtrHandle<EnergyConsumer> movementConsumption_;
	
	friend terUnitSquad;
	friend class terUnitSquad;
};

#endif //__IRONLEGION_H__