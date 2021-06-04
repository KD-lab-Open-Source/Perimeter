#ifndef __SQUAD_H__
#define __SQUAD_H__

#include "IronLegion.h"
#include "PositionGenerator.h"
#include "Mutation.h"
#include "EnergyConsumer.h"

//////////////////////////////////////////////////////////////////////////////////////////
//		Реализация логики сквада
//////////////////////////////////////////////////////////////////////////////////////////
class AttributeSquad : public AttributeBase
{
public:
	Vect2f homePositionOffsetFactor;
	float formationRadiusBase;
	float followDistanceFactor; // followDistanceFactor*(radius1 + radius2)

	// квадрат радиуса, в котором базовый сквад
	// ремонтирует и защищает другие сквады своими техниками
	float supportRadius;

	ConsumptionData productionConsumption;
    
	AttributeSquad() {
		SelectAble = true;
		homePositionOffsetFactor.set(1.2f, 0);
		formationRadiusBase = 10;
		followDistanceFactor = 1; // followDistanceFactor*(radius1 + radius2)
		supportRadius = 200 * 200;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		__super::serialize(ar);
		if(ar.openBlock("attributeSquad", "Сквад")){
			ar & TRANSLATE_OBJECT(homePositionOffsetFactor, "homePositionOffsetFactor");
			ar & TRANSLATE_OBJECT(formationRadiusBase, "formationRadiusBase");
			ar & TRANSLATE_OBJECT(followDistanceFactor, "followDistanceFactor");
			ar & TRANSLATE_OBJECT(supportRadius, "supportRadius");
			ar & TRANSLATE_OBJECT(productionConsumption, "Энергия на производство");
			ar.closeBlock();
		}
	}
};

class terUnitSquad : public terUnitBase
{
	typedef	list<terUnitLegionary*> SquadUnitList;

public:
	terUnitSquad(const UnitTemplate& data);
	~terUnitSquad();

	const AttributeSquad& attr() const {
		return safe_cast_ref<const AttributeSquad&>(__super::attr());
	}

	void Start();
	void Quant();
	void MoveQuant();
	void AvatarInterpolation();
	void DestroyLink();
	void executeCommand(const UnitCommand& command);
	void setPose(const Se3f& pose, bool initPose);

	void ShowCircles();

	const SquadUnitList& squadUnits() const { return Units; }

	void AvatarQuant();

	void setCommander(terUnitBase* p) { commander_ = p; }
	terUnitBase* commander() { return commander_; }

	// Точка атаки
	class AttackPoint
	{
	public:
		AttackPoint() : unit_(0), squad_(0), positionTarget_(false) {};
		AttackPoint(terUnitBase* unit);
		AttackPoint(const Vect3f& position);
													  
		terUnitBase* target(terUnitLegionary& assailant) const;
		bool obsolete();
		const Vect3f& position() const { 
			return !positionTarget_ ? (!squad_ ? unit_->position() : squad_->position()) : position_; 
		}
		bool positionTarget() const { return positionTarget_; }
		terUnitSquad* squad() const { return squad_; }

		void save(SaveAttackPoint& data) const;
		void load(const SaveAttackPoint& data);

	private:
		terUnitBase* unit_;
		terUnitSquad* squad_;
		Vect3f position_;
		bool positionTarget_;

		friend SaveAttackPoint;
	};

	//-------------------------------------
	int countUnits(terUnitAttributeID id, bool inSquad = true) const;
	int countPossibleUnits(terUnitAttributeID id) const;

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);

	terUnitSquad* GetSquadPoint(){ return this; }

	void setDamage(const DamageData& damage, terUnitBase* p);

	void setHomePosition(const Vect2f& homePosition) { homePosition_ = homePosition; }
	const Vect2f& homePosition() const { return homePosition_; }

	int GetInterfaceOffensiveMode();

	void ChangeUnitOwner(terPlayer* player);

	const terUnitLegionary* RequestTransportPoint(const terUnitBase* transported_unit) const;
	bool isTransport() const;
	bool addToTransport(const terUnitLegionary* unit);

	float mutationEnergy(){ return ((squadMolecula_.elementCount()) ? (mutationEnergy_ / (float)(squadMolecula_.elementCount())) : 0); };
	void clearMutationEnergy(){ mutationEnergy_ = 0; }

	void clearOrders();

	int GetInterfaceLegionMode();

	int isBuildingPowerOn(){ return 1; }
	int CanBuildingEnable(){ return 1; }

	void GetAtomPaused(DamageMolecula& paused);
	void GetAtomProduction(DamageMolecula& atom_request,DamageMolecula& atom_progress,DamageMolecula& atom_enabled);
		
	int loadedTechnicsCount() const;
	int dischargeTechnics(int count);

	void ShowInfo();

	void showPath(const vector<Vect2f>& wayPoints, const vector<Vect2f>& patrolPoints, AttackPoint* attackPoint);
	const vector<Vect2f>& getWayPoints() const {
		return wayPoints_;
	}
	const vector<Vect2f>& getPatrolPoints() const {
		return patrolPoints_;
	}
	AttackPoint* getAttackPoint() {
		return !attack_points.empty() ? &attack_points.front() : 0;
	}
	
	void Select();
	void Deselect();
	
	void UpdateSkinColor();

	//-------------------------------------------------
	void addUnit(terUnitLegionary* unit, bool set_position);
	void removeUnit(terUnitLegionary* unit);
	void addSquad(terUnitSquad* squad);
	void Kill();
	
	bool Empty() const { return Units.empty(); }
	bool emptyForMutation() const { return !squadMutationMolecula().elementCount(); }
	bool isFlying() const { return !Empty() && !isBase() && Units.front()->isFlying(); }
	bool isDigging() const { return !Empty() && Units.front()->attr().LegionType == LEGION_SUBTERRANEAN; }
	terUnitAttributeID currentMutation() const { return currentMutation_; }
	const AttributeLegionary* currentAttribute() const { xassert(!Empty()); return &(Units.front()->attr()); }
	bool isBase() const { return currentMutation() == UNIT_ATTRIBUTE_NONE; }
	float formationRadius() const;
	int attackClass() const;
	int attackEnemyClass() const;

	bool offensiveMode() const { return offensiveMode_; }
	bool patrolMode() const { return patrolPoints_.size() > 1; }
	bool mutationFinished() const { return mutation_process.finished(); }
	
	// Базовые и составные
	const DamageMolecula& squadMolecula() const { return squadMolecula_; } // Всего 
	const DamageMolecula& squadMutationMolecula() const { return squadMutationMolecula_; } // Готовых к мутации
	const DamageMolecula& atomsRequested() const { return atomsRequested_; }
	const DamageMolecula& atomsPaused() const { return atomsPaused_; }
	void addSquadMutationMolecula(const DamageMolecula& molecula) { squadMutationMolecula_ += molecula; }

	int complexUnitsNumber() const { return n_complex_units; }
	
	// Передвижение
	void clearWayPoints();
	void addWayPoint(const Vect2f& way_point);
	void goHome();
	bool noWayPoints() const { return wayPoints_.empty(); }
	int wayPointsNumber() const { return wayPoints_.size(); }
	
	// Стрельба
	void clearTargets();
	void addTarget(terUnitBase* target);
	void addTarget(const Vect3f& v);
	void addTarget(terUnitBase* target, const Vect2iVect& path);
	bool noTargets() const { return attack_points.empty(); }
	void fireStop();
	
	// Coordinate funcs
	const MatX2f& stablePose() const { return stablePose_; }
	void setStablePosition(const Vect2f& position) { stablePose_.trans = position; }
	void setStableOrientation(const Mat2f& orientation) { stablePose_.rot = orientation; }

	Vect2f forwardDirection() const { return stablePose().rot.ycol(); }

	int includingCluster() const { return including_cluster; }

	void holdProduction();
	void unholdProduction();

	float chargeLevel() const;

	struct ActionSquadAttack* attackAction() const { return attackAction_; }
	void setAttackAction(ActionSquadAttack* attackAction) { attackAction_ = attackAction; }

	const struct ActionSquadOrderUnits* orderUnitsAction() const { return orderUnitsAction_; }
	void setOrderUnitsAction(const ActionSquadOrderUnits* orderUnitsAction) { orderUnitsAction_ = orderUnitsAction; }

	void setSquadToFollow(terUnitBase* squadToFollow);

	void showDebugInfo();

	bool checkAttackPosition(const Vect3f& to) const;
	float getAttackRadius() const { return radius() + currentAttribute()->fireRadius(); }

private:
	SquadUnitList Units;
	
	float lastChargeLevel;

	int n_base_units;
	int n_complex_units;
	
	Vect2f average_position;
	Vect2f average_position_offset;
	Vect2f prev_forward_direction;
	Vect2f average_velocity;
	int including_cluster;
	
	MatX2f stablePose_; // Центр сквада, куда он стремится.
	Vect2f homePosition_; 
	bool stablePoseRestarted_;
	
	bool check_readiness_to_move; // ожидать после начала движения, пока включаться цепочки у всех юнитов
	
	DurationTimer patrol_mode_attack_timer;
	DurationTimer patrol_mode_ignore_targets_timer;
	DurationTimer reposition_to_attack_timer;
	DurationTimer targets_scan_timer;
	DurationTimer targets_clean_timer;
	DurationTimer technician_targets_scan_timer;

	//--------------------------------
	terUnitBase* commander_;
	
	terUnitAttributeID currentMutation_;
	
	DamageMolecula squadMolecula_;
	DamageMolecula squadMutationMolecula_;
	DamageMolecula atomsRequested_;
	DamageMolecula atomsPaused_;
	DamageMolecula atomsProgress_;
	
	bool offensiveMode_;
	float optimalRadius_;
		
	float mutationEnergy_;
	EnergyConsumer mutationChargeConsumption_;
		
	MutationProcess	mutation_process;
	
	cObjectNodeRoot* flagModel_;
	float flagModelPhase_;
	
	//---------------------------
	Vect2fVect wayPoints_;
	int unitsWayPoinsSize_;

	Vect2fVect patrolPoints_;
	int patrolIndex_;

	typedef list<AttackPoint> AttackPointList;
	AttackPointList attack_points;
	
	PositionGenerator position_generator;

	ActionSquadAttack* attackAction_;
	const ActionSquadOrderUnits* orderUnitsAction_;

	terUnitBase* squadToFollow_;
	
	/////////////////////////////////////////////////////////////////////
	//			Private Members
	void calcCenter();
	void correctSpeed();
	void recalcWayPoints();
	void repositionFormation(bool forceReposition);
	void repositionToAttack(AttackPoint& attackPoint, bool repeated = false);
	void attackQuant();

	terUnitBase* getBestTarget(terUnitLegionary& assailant);
	terUnitBase* findBestTarget(const Vect2f& pos, float radius);
	
	void SupportQuant();
	void initMutation(SquadUnitList& sources, SquadUnitList& destinations);
	bool mutate(terUnitAttributeID id);
	bool killBaseUnit(const UnitCommand& command);
	/// разваливает юнитов, у которых isDisintegrating() возвращает true на базовых
	void disintegrateUnitsQuant();

	/// назначает юнитам цель
	/** 
	unit_id - юнитам какого типа назначать (задаётся в случае базового сквада)
	for_free_units_only - true, если надо назначить цель только тем юнитам, у которых цели ещё нет

	возвращает false, если цель никому не назначена
	*/
	bool distributeAttackTarget(const AttackPoint& attack_point, terUnitAttributeID unit_id = UNIT_ATTRIBUTE_NONE, bool for_free_units_only = false);

	/// обсчёт техников - поиск и распределение целей для ремонта и т.д.
	void techniciansQuant();

	void followQuant();

	//------------------------------
	friend AttackPoint;
	friend class SquadBestTargetScanOp;
	friend class AIPlayer;
	friend class SquadSearchTargetScanOp;
};


#endif //__SQUAD_H__