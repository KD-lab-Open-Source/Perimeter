#ifndef __SAVE_PARSER_H__
#define __SAVE_PARSER_H__

#ifndef _UNIT_ATTRIBUTE_INL_
#define _UNIT_ATTRIBUTE_INL_
#include "../Scripts/UnitAttribute.inl"
#include "../Scripts/SquadShellEnums.inl"
#endif // _UNIT_ATTRIBUTE_INL_

#include "Serialization.h"
#include "Handle.h"
#include "SafeCast.h"
#include "../TriggerEditor/TriggerExport.h"
#include "Timers.h"
#include "../UserInterface/SoundTrack.h"

typedef vector<Vect2i> Vect2iVect;
typedef vector<PrmString> PrmStringList;

const char* editTriggerChainNameDialog(HWND hwnd, const char* initialString);
const char* editModelNameDialog(HWND hwnd, const char* initialString);
const char* editTextMultiLine(HWND hwnd, const char* initialString);
const char* editMessageID(HWND hwnd, const char* initialString);
const char* editTaskID(HWND hwnd, const char* initialString);
const char* editMissionDescriptionID(HWND hwnd, const char* initialString);
const char* editLabelDialog(HWND hwnd, const char* initialString);
const char* editCameraSplineName(HWND hwnd, const char* initialString);

struct SaveDamageMolecula
{
	bool isAlive;
	vector<int> elementsDead;

	SaveDamageMolecula() {
		isAlive = true;
	}

	const SaveDamageMolecula& operator=(const class DamageMolecula& data);

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(isAlive);
		ar & WRAP_OBJECT(elementsDead);
	}
};

struct SaveDurationTimer
{
	int time;

	SaveDurationTimer() {
		time = 0;
	}

	void initialize() {
		if(global_time() > 1)
			timer_.start(time);
		else
			time = 0;
	}

	void start(int duration) { timer_.start(time = duration); }
	int operator()() { return time = timer_(); }
	int operator!() { time = timer_(); return !timer_; } 

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(time);
		void initialize();
	}

private:
	DurationTimer timer_;											 
};

struct SaveUnitLink
{
	int unitID;
	int playerID;

	SaveUnitLink() {
		unitID = 0;
		playerID = 0;
	}

	const SaveUnitLink& operator=(const terUnitBase* unit);
	void setLink(terUnitBase*& unit) const;
	void resolveLink() const;

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(unitID);
		ar & WRAP_OBJECT(playerID);
	}

private:
	mutable terUnitBase** unitLink_;
};

struct SaveNodeController
{
	EnumWrapper<ChainID> currentChain;
	float phase;

	EnumWrapper<ChainID> requestedChain; 
	float requestedPhase;

	SaveNodeController() {
		currentChain = CHAIN_NONE;
		phase = 0;
		requestedChain = CHAIN_NONE; 
		requestedPhase = -1;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(currentChain);
		ar & WRAP_OBJECT(phase);
		ar & WRAP_OBJECT(requestedChain);
		ar & WRAP_OBJECT(requestedPhase);
	}
};

struct SaveInterpolationReal
{
	SaveNodeController node;
	vector<SaveNodeController> nodeControllers;
	
	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(node);
		ar & WRAP_OBJECT(nodeControllers);
	}
};

//---------------------------------

struct SaveWeaponData : ShareHandleBase 
{
	bool isSwitchedOn;
	Vect3f targetPosition;
	SaveUnitLink target;
	SaveUnitLink missile;
	int fireDelayTimer;

	SaveWeaponData() {
		isSwitchedOn = false;
		targetPosition = Vect3f::ZERO;
		fireDelayTimer = 0;
	}
	virtual ~SaveWeaponData() {}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(isSwitchedOn);
		ar & WRAP_OBJECT(targetPosition);
		ar & WRAP_OBJECT(target);
		ar & WRAP_OBJECT(missile);
		ar & WRAP_OBJECT(fireDelayTimer);
	}
};

struct SaveWeaponFilthNavigatorData : SaveWeaponData
{
	int prmIndex;

	SaveWeaponFilthNavigatorData() {
		prmIndex = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(prmIndex);
	}
};

//---------------------------------

struct SaveToolzerControllerData
{
	float scale;
	int currentStep;
	int requestedPhase;
	int currentStepTime;
	bool isFinished;

	Vect2i position;

	SaveToolzerControllerData() {
		scale = 0;
		currentStep = -1;
		requestedPhase = 0;
		currentStepTime = 0;
		isFinished = false;
		position = Vect2i(0,0);
	}
	
	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(scale);
		ar & WRAP_OBJECT(currentStep);
		ar & WRAP_OBJECT(requestedPhase);
		ar & WRAP_OBJECT(currentStepTime);
		ar & WRAP_OBJECT(isFinished);
		ar & WRAP_OBJECT(position);
	}
};

//---------------------------------
struct SaveUnitData : ShareHandleBase 
{
	int unitID;
	EnumWrapper<terUnitAttributeID> attributeID;
	Vect3f position;
	QuatF orientaion;
	float radius;
	PrmString label;
	SaveDamageMolecula damageMolecula;

	SaveUnitData() {
		unitID = 0;
		attributeID = UNIT_ATTRIBUTE_NONE;
		position = Vect3f::ZERO;
		orientaion = QuatF::ID;
		radius = 0;
	}
    virtual ~SaveUnitData() {}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(unitID);
		ar & WRAP_OBJECT(attributeID);
		ar & WRAP_OBJECT(position);
		ar & WRAP_OBJECT(orientaion);
		ar & WRAP_OBJECT(radius);
		ar & TRANSLATE_OBJECT(label, "�����");
		ar & WRAP_OBJECT(damageMolecula);
	}
};

struct SaveCraterData : SaveUnitData
{
	int lifeTimer;
	SaveToolzerControllerData toolzer;

	SaveCraterData() {
		lifeTimer = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitData::serialize(ar);
		ar & WRAP_OBJECT(lifeTimer);
		ar & WRAP_OBJECT(toolzer);
	}
};
															   
struct SaveUnitRealData : SaveUnitData
{
	bool basementInstalled; 
	float accumulatedEnergy; 
	int zeroLayerCounter;
	float weaponChargeLevel;
	vector<Vect3f> wayPoints;
	ShareHandle<SaveWeaponData> weapon;

	SaveUnitRealData() {
		basementInstalled = false;
		accumulatedEnergy = 0; 
		zeroLayerCounter = 0;
		weaponChargeLevel = 1;
	}
	
	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitData::serialize(ar);
		ar & WRAP_OBJECT(basementInstalled);
		ar & TRANSLATE_OBJECT(accumulatedEnergy, "������� ������������ (0..1)");
		ar & WRAP_OBJECT(zeroLayerCounter);
		ar & WRAP_OBJECT(weaponChargeLevel);
		ar & WRAP_OBJECT(wayPoints);
		ar & WRAP_OBJECT(weapon);
	}
};

struct SaveUnitProjectileData : SaveUnitRealData
{
	SaveUnitLink target;
	SaveUnitLink owner;

	Vect3f sourcePosition;
	Vect3f targetPosition;

	int killTimer;

	SaveUnitProjectileData() {
		sourcePosition = Vect3f::ZERO;
		targetPosition = Vect3f::ZERO;
		killTimer = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitRealData::serialize(ar);
		ar & WRAP_OBJECT(target);
		ar & WRAP_OBJECT(owner);
		ar & WRAP_OBJECT(sourcePosition);
		ar & WRAP_OBJECT(targetPosition);
		ar & WRAP_OBJECT(killTimer);
	}	
};
	
struct SaveUnitScumStormData : SaveUnitProjectileData
{
	int freeMovementTimer;
	bool freeMovement;

	SaveUnitScumStormData() {
		freeMovementTimer = 0;
		freeMovement = false;
	}
	
	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitProjectileData::serialize(ar);
		ar & WRAP_OBJECT(freeMovementTimer);
		ar & WRAP_OBJECT(freeMovement);
	}	
};

struct SaveUnitProjectileUndergroundData : SaveUnitProjectileData
{
	Vect2f lastCraterPosition;

	SaveUnitProjectileUndergroundData() {
		lastCraterPosition = Vect2f(0,0);
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitProjectileData::serialize(ar);
		ar & WRAP_OBJECT(lastCraterPosition);
	}	
};

struct SaveAttackPoint
{
	SaveUnitLink unit;
	SaveUnitLink squad;
	Vect3f position;
	bool positionTarget;

	SaveAttackPoint() {
		position = Vect3f::ZERO;
		positionTarget = false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(unit);
		ar & WRAP_OBJECT(squad);
		ar & WRAP_OBJECT(position);
		ar & WRAP_OBJECT(positionTarget);
	}	
};
										
struct SaveUnitSquadData : SaveUnitData						
{
	Vect2f stablePosition;
	EnumWrapper<terUnitAttributeID> currentMutation;
	float curvatureRadius;
	vector<ShareHandle<SaveUnitData> > squadMembers;

	vector<Vect2f> wayPoints;

	vector<Vect2f> patrolPoints;
	int patrolIndex;

	vector<SaveAttackPoint> attackPoints;
	SaveUnitLink squadToFollow;

	bool offensiveMode;

	int atomsRequested[DAMAGE_ELEMENT_TYPE_MAX];
	int atomsPaused[DAMAGE_ELEMENT_TYPE_MAX];

	float mutationEnergy;

	SaveUnitSquadData() {
		stablePosition = Vect2f::ZERO;
		currentMutation = UNIT_ATTRIBUTE_NONE;
		curvatureRadius = 0;
		patrolIndex = 0;
		offensiveMode = false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitData::serialize(ar);
		ar & WRAP_OBJECT(stablePosition);
		ar & WRAP_OBJECT(currentMutation);
		ar & WRAP_OBJECT(curvatureRadius);
		ar & WRAP_OBJECT(squadMembers);
		
		ar & WRAP_OBJECT(wayPoints);
		
		ar & WRAP_OBJECT(patrolPoints);
		ar & WRAP_OBJECT(patrolIndex);
		
		ar & WRAP_OBJECT(attackPoints);
		ar & WRAP_OBJECT(squadToFollow);
		
		ar & WRAP_OBJECT(offensiveMode);
		
		ar & WRAP_OBJECT(atomsRequested);
		ar & WRAP_OBJECT(atomsPaused);
		
		ar & WRAP_OBJECT(mutationEnergy);
	}	
};

struct SaveFrameSlotData
{
	EnumWrapper<terUnitAttributeID> productionID;
	int status;
	float progress;

	SaveFrameSlotData() {
		productionID = UNIT_ATTRIBUTE_NONE;
		status = -1;
		progress = 1;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(productionID);
		ar & WRAP_OBJECT(status);
		ar & WRAP_OBJECT(progress);
	}
};

struct SaveUnitFrameData : SaveUnitRealData
{
	bool attached;
	bool attaching;
	bool powered;
	float spiralLevel; 
	ShareHandle<SaveUnitData> squad;
	ShareHandle<SaveUnitData> frameSlots[5];
	SaveFrameSlotData slotsData[5];
	
	EnumWrapper<terBelligerent> belligerent;
	bool catched;

	SaveUnitFrameData() {
		attached = false;
		attaching = false;
		powered = false;
		spiralLevel = 0; 
		belligerent = BELLIGERENT_NONE;
		catched = false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitRealData::serialize(ar);
		ar & WRAP_OBJECT(attached);
		ar & WRAP_OBJECT(attaching);
		ar & WRAP_OBJECT(powered);
		ar & TRANSLATE_OBJECT(spiralLevel, "������� ������� ������� [0-1]");
		ar & WRAP_OBJECT(squad);
		ar & WRAP_OBJECT(frameSlots);
		ar & WRAP_OBJECT(slotsData);
		
		ar & WRAP_OBJECT(belligerent);
		ar & WRAP_OBJECT(catched);
	}
};																		   

struct SaveUnitFrameChildData : SaveUnitRealData
{
	bool dockReadyStatus;
	bool alarmStatus;

	SaveUnitFrameChildData() {
		dockReadyStatus = false;
		alarmStatus = false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitRealData::serialize(ar);
		ar & WRAP_OBJECT(dockReadyStatus);
		ar & WRAP_OBJECT(alarmStatus);
	}	
};

struct SaveUnitBuildingData : SaveUnitRealData
{
	BitVector<BuildingStatus> buildingStatusBV;
	int fireCount;
	bool visible; 

	SaveUnitBuildingData() {
		fireCount = 0;
		visible = true; 
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitRealData::serialize(ar);
		ar & WRAP_OBJECT(buildingStatusBV);
		ar & WRAP_OBJECT(fireCount);
		ar & TRANSLATE_OBJECT(visible, "�������");
	}	
};

struct SaveUnitLegionaryData : SaveUnitRealData
{
	int transportedSoldiers; 
	int transportedOfficers; 
	int transportedTechnics; 

	bool flyingMode;
	bool diggingMode;

	bool inSquad;
	Vect2f localPosition;
	bool localPositionValid;
	vector<Vect2f> wayPoints;

	SaveUnitLegionaryData() {
		transportedSoldiers = 0; 
		transportedOfficers = 0; 
		transportedTechnics = 0; 

		flyingMode = false;
		diggingMode = false;

		inSquad = true;
		localPosition = Vect3f::ZERO;
		localPositionValid = false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitRealData::serialize(ar);
		ar & WRAP_OBJECT(transportedSoldiers);
		ar & WRAP_OBJECT(transportedOfficers);
		ar & WRAP_OBJECT(transportedTechnics);
		
		ar & WRAP_OBJECT(flyingMode);
		ar & WRAP_OBJECT(diggingMode);
		
		ar & WRAP_OBJECT(inSquad);
		ar & WRAP_OBJECT(localPosition);
		ar & WRAP_OBJECT(localPositionValid);
		ar & WRAP_OBJECT(wayPoints);
	}
};

struct SaveUnitCommandCenterData : SaveUnitBuildingData
{
	ShareHandle<SaveUnitData> squad;

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitBuildingData::serialize(ar);
		ar & WRAP_OBJECT(squad);
	}
};

struct SaveUnitProtectorData : SaveUnitBuildingData
{
	int monksNumber;
	int fieldState;
	bool enableCharge;
	bool startWhenCharged;

	SaveUnitProtectorData() {
		monksNumber = 0;
		fieldState = 1;
		enableCharge = true;
		startWhenCharged = false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitBuildingData::serialize(ar);
		ar & WRAP_OBJECT(monksNumber);
		ar & WRAP_OBJECT(fieldState);
		ar & WRAP_OBJECT(enableCharge);
		ar & WRAP_OBJECT(startWhenCharged);
	}
};

struct SaveUnitBuildingMilitaryData : SaveUnitBuildingData
{
	SaveUnitLink attackTarget;
	SaveUnitLink lastAttackTarget;
	bool manualAttackTarget;
	
	SaveUnitBuildingMilitaryData() {
		manualAttackTarget = false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitBuildingData::serialize(ar);
		ar & WRAP_OBJECT(attackTarget);
		ar & WRAP_OBJECT(lastAttackTarget);
		ar & WRAP_OBJECT(manualAttackTarget);
	}	
};

struct SaveUnitCorridorAlphaData : SaveUnitBuildingData // �������� �����
{
	bool free;
	int passTime; 
	int timeOffset;

	SaveUnitCorridorAlphaData() {
		free = true; 
		passTime = 1200;
		timeOffset = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitBuildingData::serialize(ar);
		ar & TRANSLATE_OBJECT(free, "�������� �������");
		ar & TRANSLATE_OBJECT(passTime, "����� ������, �������");
		ar & WRAP_OBJECT(timeOffset);
	}	
};

struct SaveUnitCorridorOmegaData : SaveUnitBuildingData // �������� �����
{
	bool upgraded;

	SaveUnitCorridorOmegaData() {
		upgraded = false; 
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitBuildingData::serialize(ar);
		ar & TRANSLATE_OBJECT(upgraded, "������������");
	}
};

struct SaveUnitNatureData : SaveUnitData
{
	CustomString modelName;
	bool visible;
	BitVector<terMissionObjectType> natureFlag; 
	PrmString chainName; 
	float chainPhase; 
	float chainPeriod;

	SaveUnitNatureData() :
	modelName(editModelNameDialog)
	{
		visible = true; 
		chainPhase = 0; 
		chainPeriod = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitData::serialize(ar);
		ar & TRANSLATE_OBJECT(modelName, "��� ������");
		ar & TRANSLATE_OBJECT(visible, "�������");
		ar & TRANSLATE_OBJECT(natureFlag, "��������");
		ar & TRANSLATE_OBJECT(chainName, "��� ������� ��������");
		ar & TRANSLATE_OBJECT(chainPhase, "��������� ���� ��������");
		ar & TRANSLATE_OBJECT(chainPeriod, "������ �������� (����������)");
	}
};

struct SaveUnitFilthData : SaveUnitData
{
	EnumWrapper<terFilthSpotID> filthType; 

	float attackWidth;	
	float attackDirection;

	bool sleep; 

	float firstSleepTime;
	float sleepPeriod;			
	float deltaSleepPeriod;	

	float attackPeriond;
	float deltaAttackPeriond;

	int creatureNum;

	EnumWrapper<terUnitAttributeID> activatingUnit;
	float activatingDistance; 
	EnumWrapper<terFilthAttackType> attack_player; 
	bool initial_geoprocess;

	int killTimer;

	int sleep_timer;
	bool create_first;
	vector<Vect2f> hole_position;
	vector<int> hole_position_inited;
	bool kill_of_end;

	vector<ShareHandle<SaveUnitData> > swarmList;

	SaveUnitFilthData() {
		filthType = FILTH_SPOT_ID_NONE; 
		
		attackWidth = -1;	
		attackDirection = 0;

		sleep = false; 

		firstSleepTime = 0;
		sleepPeriod = 20;
		deltaSleepPeriod = 0;	

		attackPeriond = 20;
		deltaAttackPeriond = 0;

		creatureNum =10;

		activatingUnit = UNIT_ATTRIBUTE_NONE;
		activatingDistance = 100; 
		attack_player = FILTH_ATTACK_ALL; 
		initial_geoprocess = true;

		killTimer = 0;

		sleep_timer = 0;
		create_first = true;

		kill_of_end = false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitData::serialize(ar);
		ar & TRANSLATE_OBJECT(filthType, "��� �������");
		
		ar & TRANSLATE_OBJECT(attackWidth, "������ ������, ��� ����� (-1 ���� ���)");
		ar & TRANSLATE_OBJECT(attackDirection, "����������� ����� 0..360 ����");
		
		ar & TRANSLATE_OBJECT(sleep, "����");
		
		ar & TRANSLATE_OBJECT(firstSleepTime, "�������� �� ������� �� ������� ��������� (���)");
		ar & TRANSLATE_OBJECT(sleepPeriod, "����� ������ (���)");
		ar & TRANSLATE_OBJECT(deltaSleepPeriod, "������� ������� ������ (���)");
		
		ar & TRANSLATE_OBJECT(attackPeriond, "����� ���������� ������� (���)");
		ar & TRANSLATE_OBJECT(deltaAttackPeriond, "������� ������� ���������� ������� (���)");
		
		ar & TRANSLATE_OBJECT(creatureNum, "���������� ������� ������������ � ����� �����");
		
		ar & TRANSLATE_OBJECT(activatingUnit, "������������ ������");
		ar & TRANSLATE_OBJECT(activatingDistance, "������ ���������");
		ar & TRANSLATE_OBJECT(attack_player, "���������: ����, ������, AI");
		ar & TRANSLATE_OBJECT(initial_geoprocess, "��������� ����������");
		
		ar & WRAP_OBJECT(killTimer);
		
		ar & WRAP_OBJECT(sleep_timer);
		ar & WRAP_OBJECT(create_first);
		ar & WRAP_OBJECT(hole_position);
		ar & WRAP_OBJECT(hole_position_inited);
		ar & WRAP_OBJECT(kill_of_end);
		
		ar & WRAP_OBJECT(swarmList);
	}
};

struct SaveTerGenerate
{
	int GenerationCount;
	float GenerationFactor;
	float GenerationSpeed;

	SaveTerGenerate() {
		GenerationCount = 0;
		GenerationFactor = 0;
		GenerationSpeed = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(GenerationCount);
		ar & WRAP_OBJECT(GenerationFactor);
		ar & WRAP_OBJECT(GenerationSpeed);
	}
	
};

struct SaveFilthSwarm : SaveUnitData
{
	Vect3f position;
	float attack_width;
	float attack_direction;
	EnumWrapper<terFilthAttackType> attack_player;

	SaveFilthSwarm() {
		position = Vect3f::ZERO;
		attack_width = -1;
		attack_direction = -1;
		attack_player = FILTH_ATTACK_ALL;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitData::serialize(ar);
		ar & WRAP_OBJECT(position);
		ar & WRAP_OBJECT(attack_width);
		ar & WRAP_OBJECT(attack_direction);
		ar & WRAP_OBJECT(attack_player);
	}
};

struct SaveFilthSwarmAnt : SaveFilthSwarm
{
	SaveTerGenerate generate;
	float DeltaAngle;
	float ChangeAngleCount;

	int TargetCount;
	Vect3f TargetPosition;

	int attack_period;

	vector<ShareHandle<SaveUnitData> > unitList;

	SaveFilthSwarmAnt() {
		DeltaAngle = 0;
		ChangeAngleCount = 0;
		TargetCount = 0;
		TargetPosition = Vect3f::ZERO;
		attack_period = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSwarm::serialize(ar);
		ar & WRAP_OBJECT(generate);
		ar & WRAP_OBJECT(DeltaAngle);
		ar & WRAP_OBJECT(ChangeAngleCount);
		
		ar & WRAP_OBJECT(TargetCount);
		ar & WRAP_OBJECT(TargetPosition);
		
		ar & WRAP_OBJECT(attack_period);
		
		ar & WRAP_OBJECT(unitList);
	}
};

struct SaveFilthAnt : SaveUnitRealData
{
	Vect3f TargetPosition;
	float SpeedScale;
	float SpeedFactor;

	bool free_destroy;
	bool death_requested;
	bool initial_geoprocess;

	SaveFilthAnt() {
		TargetPosition = Vect3f::ZERO;
		SpeedScale = 0;
		SpeedFactor = 0;
		free_destroy = false;
		death_requested = false;
		initial_geoprocess = false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitRealData::serialize(ar);
		ar & WRAP_OBJECT(TargetPosition);
		ar & WRAP_OBJECT(SpeedScale);
		ar & WRAP_OBJECT(SpeedFactor);
		
		ar & WRAP_OBJECT(free_destroy);
		ar & WRAP_OBJECT(death_requested);
		ar & WRAP_OBJECT(initial_geoprocess);
	}
};

struct SaveFilthSpline : SaveUnitRealData
{
	float delta_time;
	float cur_way_point;
	Vect3f acceleration;
	float k_acceleration;

	float damphing;
	bool auto_up;
	bool auto_front;
	Vect3f user_up;
	Vect3f user_front;
	bool interpolation_linear;

	vector<Vect3f> way_points;

	SaveFilthSpline() {
		delta_time = 0;
		cur_way_point = 0;
		acceleration = Vect3f::ZERO;
		k_acceleration = 0;
		damphing = 0;
		auto_up = false;
		auto_front = false;
		user_up = Vect3f::ZERO;
		user_front = Vect3f::ZERO;
		interpolation_linear =false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitRealData::serialize(ar);
		ar & WRAP_OBJECT(delta_time);
		ar & WRAP_OBJECT(cur_way_point);
		ar & WRAP_OBJECT(acceleration);
		ar & WRAP_OBJECT(k_acceleration);
		
		ar & WRAP_OBJECT(damphing);
		ar & WRAP_OBJECT(auto_up);
		ar & WRAP_OBJECT(auto_front);
		ar & WRAP_OBJECT(user_up);
		ar & WRAP_OBJECT(user_front);
		ar & WRAP_OBJECT(interpolation_linear);
		
		ar & WRAP_OBJECT(way_points);
	}
};

struct SaveFilthSwarmCrow : SaveFilthSwarm
{
	int generate_creature_num;
	Vect3f attack_pos;
	int attack_period;
	
	vector<ShareHandle<SaveUnitData> > unitList;

	SaveFilthSwarmCrow() {
		generate_creature_num=0;
		attack_pos = Vect3f::ZERO;
		attack_period = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSwarm::serialize(ar);
		ar & WRAP_OBJECT(generate_creature_num);
		ar & WRAP_OBJECT(attack_pos);
		ar & WRAP_OBJECT(attack_period);
		
		ar & WRAP_OBJECT(unitList);
	}
};

struct SaveFilthCrow : SaveFilthSpline
{
	bool free_destroy;
	Vect3f target_position;
	Vect3f delta_target;
	bool go_out;
	bool attack_mode;
	bool must_die;
	bool death_request;

	SaveFilthCrow() {
		free_destroy = false;
		target_position = Vect3f :: ZERO;
		delta_target = Vect3f :: ZERO;
		go_out = false;
		attack_mode = false;
		must_die = false;
		death_request = false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSpline::serialize(ar);
		ar & WRAP_OBJECT(free_destroy);
		ar & WRAP_OBJECT(target_position);
		ar & WRAP_OBJECT(delta_target);
		ar & WRAP_OBJECT(go_out);
		ar & WRAP_OBJECT(attack_mode);
		ar & WRAP_OBJECT(must_die);
		ar & WRAP_OBJECT(death_request);
	}
};

struct SaveFilthSwarmDaemon : SaveFilthSwarm
{
	bool must_init_pos;
	vector<Vect3f> init_pos;
	Vect3f attack_pos;

	int attack_period;

	bool berserk_mode;
	int berserk_time;

	int GenerationCount;
	float GenerationFactor;
	float GenerationSpeed;

	Vect3f pin;
	vector<ShareHandle<SaveUnitData> > unit_list;

	SaveFilthSwarmDaemon() {
		must_init_pos = false;
		attack_pos = Vect3f::ZERO;
		attack_period = 0;
		berserk_mode = false;
		berserk_time = 0;
		GenerationCount = 0;
		GenerationFactor = 0;
		GenerationSpeed = 0;
		pin = Vect3f::ZERO;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSwarm::serialize(ar);
		ar & WRAP_OBJECT(must_init_pos);
		ar & WRAP_OBJECT(init_pos);
		ar & WRAP_OBJECT(attack_pos);
		
		ar & WRAP_OBJECT(attack_period);
		
		ar & WRAP_OBJECT(berserk_mode);
		ar & WRAP_OBJECT(berserk_time);
		
		ar & WRAP_OBJECT(GenerationCount);
		ar & WRAP_OBJECT(GenerationFactor);
		ar & WRAP_OBJECT(GenerationSpeed);
		
		ar & WRAP_OBJECT(pin);
		ar & WRAP_OBJECT(unit_list);
	}
};

struct SaveFilthDaemon: SaveUnitRealData
{
	bool free_destroy;
	bool on_zeroplast;
	bool berserk_mode;
	bool go_up;
	float start_phase;
	float mul_speed;
	Vect3f direction;

	SaveFilthDaemon() {
		free_destroy = false;
		on_zeroplast = false;
		berserk_mode = false;
		go_up = false;
		start_phase = 0;
		mul_speed = 0;
		direction = Vect3f :: ZERO;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitRealData::serialize(ar);
		ar & WRAP_OBJECT(free_destroy);
		ar & WRAP_OBJECT(on_zeroplast);
		ar & WRAP_OBJECT(berserk_mode);
		ar & WRAP_OBJECT(go_up);
		ar & WRAP_OBJECT(start_phase);
		ar & WRAP_OBJECT(mul_speed);
		ar & WRAP_OBJECT(direction);
	}
};

struct SaveFilthSwarmDragon: SaveFilthSwarm
{
	bool use_geo;
	int AttackCount;
	Vect3f TargetPosition;
	int TargetCount;
	Vect3f distance_direction;
	ShareHandle<SaveUnitData> HeadPoint;

	SaveFilthSwarmDragon() {
		use_geo = false;
		AttackCount = 0;
		TargetPosition = Vect3f :: ZERO;
		TargetCount = 0;
		distance_direction = Vect3f :: ZERO;
		HeadPoint = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSwarm::serialize(ar);
		ar & WRAP_OBJECT(use_geo);
		ar & WRAP_OBJECT(AttackCount);
		ar & WRAP_OBJECT(TargetPosition);
		ar & WRAP_OBJECT(TargetCount);
		ar & WRAP_OBJECT(distance_direction);
		ar & WRAP_OBJECT(HeadPoint);
	}	
};

struct SaveFilthDragonHead : SaveFilthSpline
{
	float ObjectAnimationPhase;
	float timerDieFreePhase;
	bool attack;
	float Height;
	bool height_flag;
	Vect3f TargetPosition;
	bool is_die;
	bool is_die_free;

	ShareHandle<SaveUnitData> NextPoint;

	SaveFilthDragonHead() {
		ObjectAnimationPhase = 0;
		timerDieFreePhase = 0;
		attack = false;
		Height = 0;
		height_flag = false;
		TargetPosition = Vect3f :: ZERO;
		is_die = false;
		is_die_free = false;
		NextPoint = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSpline::serialize(ar);
		ar & WRAP_OBJECT(ObjectAnimationPhase);
		ar & WRAP_OBJECT(timerDieFreePhase);
		ar & WRAP_OBJECT(attack);
		ar & WRAP_OBJECT(Height);
		ar & WRAP_OBJECT(height_flag);
		ar & WRAP_OBJECT(TargetPosition);
		ar & WRAP_OBJECT(is_die);
		ar & WRAP_OBJECT(is_die_free);
		
		ar & WRAP_OBJECT(NextPoint);
	}	
};

struct SaveFilthDragon : SaveUnitRealData
{
	float ObjectAnimationPhase;
	float timerDieFreePhase;
	float scale;
	float crater_run;
	bool is_die;
	bool is_die_free;

	ShareHandle<SaveUnitData> NextPoint;

	SaveFilthDragon() {
		ObjectAnimationPhase = 0;
		timerDieFreePhase = 0;
		scale = 0;
		crater_run = 0;
		is_die = false;
		is_die_free = false;
		NextPoint = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitRealData::serialize(ar);
		ar & WRAP_OBJECT(ObjectAnimationPhase);
		ar & WRAP_OBJECT(timerDieFreePhase);
		ar & WRAP_OBJECT(scale);
		ar & WRAP_OBJECT(crater_run);
		ar & WRAP_OBJECT(is_die);
		ar & WRAP_OBJECT(is_die_free);
		
		ar & WRAP_OBJECT(NextPoint);
	}	
};

struct SaveFilthSwarmGhost : SaveFilthSwarm
{
	float angle_z;
	SaveTerGenerate generate;
	int attack_period;
	Vect3f TargetPosition;
	int TargetCount;
	vector<ShareHandle<SaveUnitData> > unitList;

	SaveFilthSwarmGhost() {
		angle_z = 0;
		attack_period = 0;
		TargetPosition = Vect3f::ZERO;
		TargetCount = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSwarm::serialize(ar);
		ar & WRAP_OBJECT(angle_z);
		ar & WRAP_OBJECT(generate);
		ar & WRAP_OBJECT(attack_period);
		ar & WRAP_OBJECT(TargetPosition);
		ar & WRAP_OBJECT(TargetCount);
		ar & WRAP_OBJECT(unitList);
	}
};

struct SaveFilthGhost : SaveUnitRealData
{
	Vect3f target_position;
	Vect3f attack_position;

	bool destruction;

	float ObjectAnimation;
	float DestroyAnimation;
	float BeginAnimation;

	bool last_attack;
	bool request_attack;
	float AttackAnimation;

	bool destroy_flag;

	int zeroPositionTimer;
	float last_deltaz;

	SaveFilthGhost() {
		target_position = Vect3f::ZERO;
		attack_position = Vect3f::ZERO;
		destruction = false;
		ObjectAnimation = 0;
		DestroyAnimation = 0;
		BeginAnimation = 0;
		last_attack = false;
		request_attack = false;
		AttackAnimation = 0;
		destroy_flag = false;
		zeroPositionTimer = 0;
		last_deltaz = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitRealData::serialize(ar);
		ar & WRAP_OBJECT(target_position);
		ar & WRAP_OBJECT(attack_position);
		
		ar & WRAP_OBJECT(destruction);
		
		ar & WRAP_OBJECT(ObjectAnimation);
		ar & WRAP_OBJECT(DestroyAnimation);
		ar & WRAP_OBJECT(BeginAnimation);
		
		ar & WRAP_OBJECT(last_attack);
		ar & WRAP_OBJECT(request_attack);
		ar & WRAP_OBJECT(AttackAnimation);
		
		ar & WRAP_OBJECT(destroy_flag);
		
		ar & WRAP_OBJECT(zeroPositionTimer);
		ar & WRAP_OBJECT(last_deltaz);
	}
};

struct SaveFilthSwarmRat : SaveFilthSwarm
{
	SaveTerGenerate generate;

	float DeltaAngle;
	float ChangeAngleCount;

	int TargetCount;
	Vect3f TargetPosition;

	int attack_period;
	vector<ShareHandle<SaveUnitData> > unitList;

	SaveFilthSwarmRat() {
		DeltaAngle = 0;
		ChangeAngleCount = 0;
		TargetCount = 0;
		TargetPosition = Vect3f::ZERO;
		attack_period = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSwarm::serialize(ar);
		ar & WRAP_OBJECT(generate);

		ar & WRAP_OBJECT(DeltaAngle);
		ar & WRAP_OBJECT(ChangeAngleCount);
		
		ar & WRAP_OBJECT(TargetCount);
		ar & WRAP_OBJECT(TargetPosition);
		
		ar & WRAP_OBJECT(attack_period);
		ar & WRAP_OBJECT(unitList);
	}
};

struct SaveFilthRat : SaveUnitRealData
{
	int FilthStatus;

	Vect3f TargetPosition;
	float SpeedScale;
	float SpeedFactor;

	bool free_destroy;
	int zeroPositionTimer;

	bool first_z_negate;
	float last_x_angle;
	float intrerpolate_x_angle;

	SaveFilthRat() {
		FilthStatus = 0;
		TargetPosition = Vect3f :: ZERO;
		SpeedScale = 0;
		SpeedFactor = 0;
		free_destroy = false;
		zeroPositionTimer = 0;
		first_z_negate = false;
		last_x_angle = 0;
		intrerpolate_x_angle = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitRealData::serialize(ar);
		ar & WRAP_OBJECT(FilthStatus);

		ar & WRAP_OBJECT(TargetPosition);
		ar & WRAP_OBJECT(SpeedScale);
		ar & WRAP_OBJECT(SpeedFactor);
		
		ar & WRAP_OBJECT(free_destroy);
		ar & WRAP_OBJECT(zeroPositionTimer);
		
		ar & WRAP_OBJECT(first_z_negate);
		ar & WRAP_OBJECT(last_x_angle);
		ar & WRAP_OBJECT(intrerpolate_x_angle);
	}
};

struct SaveFilthSwarmShark : SaveFilthSwarm
{
	int attack_period;
	SaveTerGenerate generate;

	vector<ShareHandle<SaveUnitData> > unitList;

	SaveFilthSwarmShark() {
		attack_period = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSwarm::serialize(ar);
		ar & WRAP_OBJECT(attack_period);
		ar & WRAP_OBJECT(generate);

		ar & WRAP_OBJECT(unitList);
	}
};

struct SaveFilthShark : SaveFilthSpline
{
	Vect3f target;
	Vect3f pin;
	bool is_hmax;
	bool last_effect_up;
	bool is_die;
	bool is_die_free;
	int attack_period;
	int timer_die_free;

	SaveFilthShark() {
		target = Vect3f::ZERO;
		pin = Vect3f::ZERO;
		is_hmax = false;
		last_effect_up = false;
		is_die = false;
		is_die_free = false;
		attack_period = 0;
		timer_die_free = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSpline::serialize(ar);
		ar & WRAP_OBJECT(target);
		ar & WRAP_OBJECT(pin);
		ar & WRAP_OBJECT(is_hmax);
		ar & WRAP_OBJECT(last_effect_up);
		ar & WRAP_OBJECT(is_die);
		ar & WRAP_OBJECT(is_die_free);
		ar & WRAP_OBJECT(attack_period);
		ar & WRAP_OBJECT(timer_die_free);
	}	
};

struct SaveFilthSwarmVolcano : SaveFilthSwarm
{
	int generation_period;
	int creature_num;
	vector<ShareHandle<SaveUnitData> > unitList;

	SaveFilthSwarmVolcano() {
		generation_period = 0;
		creature_num = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSwarm::serialize(ar);
		ar & WRAP_OBJECT(generation_period);
		ar & WRAP_OBJECT(creature_num);
		ar & WRAP_OBJECT(unitList);
	}	
};

struct SaveFilthVolcano : SaveUnitRealData
{
	int begin_wait_destroy;
	float time_from_last_damage;

	SaveFilthVolcano() {
		begin_wait_destroy = 0;
		time_from_last_damage = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitRealData::serialize(ar);
		ar & WRAP_OBJECT(begin_wait_destroy);
		ar & WRAP_OBJECT(time_from_last_damage);
	}		
};

struct SaveFilthSwarmWasp : SaveFilthSwarm
{
	EnumWrapper<terUnitAttributeID> unit_id;
	SaveTerGenerate generate;
	int attack_period;
	Vect3f TargetPosition;
	float attack_count;

	vector<ShareHandle<SaveUnitData> > unitList;

	SaveFilthSwarmWasp() {
		unit_id = UNIT_ATTRIBUTE_FILTH_WASP;
		attack_period = 0;
		TargetPosition = Vect3f :: ZERO;
		attack_count = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSwarm::serialize(ar);
		ar & WRAP_OBJECT(unit_id);
		ar & WRAP_OBJECT(generate);
		ar & WRAP_OBJECT(attack_period);
		ar & WRAP_OBJECT(TargetPosition);
		ar & WRAP_OBJECT(attack_count);
		
		ar & WRAP_OBJECT(unitList);
	}	
};

struct SaveFilthWasp : SaveFilthSpline
{
	bool free_destroy;
	Vect3f target_position;
	Vect3f center_position;
	Vect3f delta_center;
	bool begin_move;
	float summary_z;
	float swarm_height;
	bool attack;
	float target_radius;
	bool no_add_point;

	SaveFilthWasp() {
		free_destroy = false;
		target_position = Vect3f::ZERO;
		center_position = Vect3f::ZERO;
		delta_center = Vect3f::ZERO;
		begin_move = false;
		summary_z = 0;
		swarm_height = 0;
		attack = false;
		target_radius = 0;
		no_add_point = false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSpline::serialize(ar);
		ar & WRAP_OBJECT(free_destroy);
		ar & WRAP_OBJECT(target_position);
		ar & WRAP_OBJECT(center_position);
		ar & WRAP_OBJECT(delta_center);
		ar & WRAP_OBJECT(begin_move);
		ar & WRAP_OBJECT(summary_z);
		ar & WRAP_OBJECT(swarm_height);
		ar & WRAP_OBJECT(attack);
		ar & WRAP_OBJECT(target_radius);
		ar & WRAP_OBJECT(no_add_point);
	}	
};

struct SaveFilthSwarmWorm : SaveFilthSwarm
{
	int attack_period;
	int change_angle;
	float angle;
	float speed;
	bool first;
	bool first_create;

	ShareHandle<SaveUnitData> pWormModel;

	SaveFilthSwarmWorm() {
		attack_period = 0;
		change_angle = 0;
		angle = 0;
		speed = 0;
		first = false;
		first_create = false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveFilthSwarm::serialize(ar);
		ar & WRAP_OBJECT(attack_period);
		ar & WRAP_OBJECT(change_angle);
		ar & WRAP_OBJECT(angle);
		ar & WRAP_OBJECT(speed);
		ar & WRAP_OBJECT(first);
		ar & WRAP_OBJECT(first_create);
		
		ar & WRAP_OBJECT(pWormModel);
	}
};

struct SaveFilthVorm : SaveUnitRealData
{
};

//////////////////////////////////////////////////////////
struct SaveGeoControl : SaveUnitData
{
	bool sleep; 

	float firstSleepTime;
	float sleepPeriod;
	float deltaSleepPeriod;		

	float attackPeriond;		
	float deltaAttackPeriond;

	EnumWrapper<terUnitAttributeID> activatingUnit; 
	float activatingDistance; 

	SaveGeoControl() {
		sleep = false;
		firstSleepTime = 0;
		sleepPeriod = 20;
		deltaSleepPeriod = 0;
		attackPeriond = 20;
		deltaAttackPeriond = 0;
		activatingUnit = UNIT_ATTRIBUTE_NONE;
		activatingDistance = 100;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveUnitData::serialize(ar);
		ar & TRANSLATE_OBJECT(sleep, "����");

		ar & TRANSLATE_OBJECT(firstSleepTime, "�������� �� ������� �� ������� ��������� (���)" );
		ar & TRANSLATE_OBJECT(sleepPeriod, "����� ������ (���)");
		ar & TRANSLATE_OBJECT(deltaSleepPeriod, "������� ������� ������ (���)");
		
		ar & TRANSLATE_OBJECT(attackPeriond, "����� ���������� ������� (���)");
		ar & TRANSLATE_OBJECT(deltaAttackPeriond, "������� ������� ���������� ������� (���)");
		
		ar & TRANSLATE_OBJECT(activatingUnit, "������������ ������");
		ar & TRANSLATE_OBJECT(activatingDistance, "������ ���������");
	}
};

struct SaveGeoInfluence : SaveGeoControl
{
	float geoRadius;

	SaveGeoInfluence() {
		geoRadius = 8;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveGeoControl::serialize(ar);
		ar & TRANSLATE_OBJECT(geoRadius, "������");
	}	
};

struct SaveGeoBreak : SaveGeoInfluence
{
	int num_break; 

	SaveGeoBreak() {
		num_break = 0; 
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveGeoInfluence::serialize(ar);
		ar & WRAP_OBJECT(num_break);
	}	
};

struct SaveGeoFault: SaveGeoControl
{
	float length; 
	float angle;

	SaveGeoFault() {
		length = 1000; 
		angle = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		SaveGeoControl::serialize(ar);
		ar & TRANSLATE_OBJECT(length, "�����");
		ar & TRANSLATE_OBJECT(angle, "���� (����)");
	}	
};

//---------------------------------
struct SaveCameraData
{
	Vect2f position;
	float psi;
	float theta;
	float distance;

	SaveCameraData() {
		position = Vect2f::ZERO;
		psi = 0;
		theta = 0;
		distance = 1;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(position);
		ar & WRAP_OBJECT(psi);
		ar & WRAP_OBJECT(theta);
		ar & WRAP_OBJECT(distance);
	}
};

struct SaveCameraSplineData // ������ ������
{
	PrmString name; 
	vector<SaveCameraData> path; 
	bool useAsSpline;
	Vect2f position;
	Vect2f angle;
	float distance;

	SaveCameraSplineData() {
		useAsSpline = true;
		position = Vect2f::ZERO;
		angle = Vect2f::ZERO;
		distance = 0;
		
		initialize();
	}

	void initialize() {
		if(path.empty() && !useAsSpline)
			set(position, angle.y, angle.x, distance);
	}

	void set(const Vect2f& position, float psi, float theta, float distance)
	{
		path.clear();
		path.push_back(SaveCameraData());
		path.back().position = position;
		path.back().psi = psi;
		path.back().theta = theta;
		path.back().distance = distance;
		useAsSpline = false;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(name, "&���");
		ar & TRANSLATE_OBJECT(path, "������");
		ar & WRAP_OBJECT(useAsSpline);
		ar & WRAP_OBJECT(position);
		ar & WRAP_OBJECT(angle);
		ar & WRAP_OBJECT(distance);
		initialize();
	}
};

//---------------------------------
//			Triggers
//---------------------------------
#include "Conditions.h"						
										
//------------------------------------
// ��������							  					
struct ActionForAI : Action // ---------------
{
	bool onlyIfAi; 

	ActionForAI() {
		onlyIfAi = true; 
	}

	bool onlyIfAI() const { return onlyIfAi; }

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(onlyIfAi, "��������� ������ ��� ��");
	}
};
													
struct ActionDelay : Action // �������� �������
{
	int delay; 
	bool showTimer; 
	bool scaleByDifficulty; 
	SaveDurationTimer timer;											 

	ActionDelay() {
		delay = 60;
		showTimer = true;
		scaleByDifficulty = false;
	}

	void activate(AIPlayer& aiPlayer);
	bool workedOut(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(delay, "�����, �������");
		ar & TRANSLATE_OBJECT(showTimer, "���������� ������");
		ar & TRANSLATE_OBJECT(scaleByDifficulty, "������� ������ ���������");
		ar & WRAP_OBJECT(timer);
	}	
};

struct ActionSetCamera : Action // ��������� ������
{
	CustomString cameraSplineName;
	float stepTime; 
	int cycles;
	bool smoothTransition; 

	ActionSetCamera() :
	cameraSplineName(editCameraSplineName)
	{
		stepTime = 4; 
		cycles = 1;
		smoothTransition = false; 
	}

	void activate(AIPlayer& aiPlayer);
	bool workedOut(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(cameraSplineName, "��� ������� ������");
		ar & TRANSLATE_OBJECT(stepTime, "����� ����� ������������ ������� �������, �������");
		ar & TRANSLATE_OBJECT(cycles, "���������� ������");
		ar & TRANSLATE_OBJECT(smoothTransition, "������� �������");
	}	
};

struct ActionOscillateCamera : Action // ������ ������
{
	int duration; 
	float factor; 

	ActionOscillateCamera() {
		duration = 30; 
		factor = 1; 
	}

	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(duration, "������������, �������");
		ar & TRANSLATE_OBJECT(factor, "���������");
	}
};


//-------------------------------------
struct ActionVictory : Action // ������
{
	void activate(AIPlayer& aiPlayer);
};

struct ActionDefeat : Action // ���������
{
	void activate(AIPlayer& aiPlayer);
};

//-------------------------------------
struct ActionTeleportationOut : Action // ��������������� ����� � ����
{
	bool automaticCondition(AIPlayer& aiPlayer) const;
	void activate(AIPlayer& aiPlayer);
};	

struct ActionKillObject : Action // ���������� ������
{
	EnumWrapper<terUnitAttributeID> object; 

	ActionKillObject() {
		object = UNIT_ATTRIBUTE_NONE; 
	}

	bool automaticCondition(AIPlayer& aiPlayer) const { return findObject(); }
	bool workedOut(AIPlayer& aiPlayer);
	terUnitBase* findObject() const;

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(object, "������");
	}
};	

//-------------------------------------
struct ActionInstallFrame : ActionForAI // �������������� �����
{
	void activate(AIPlayer& aiPlayer);
	bool workedOut(AIPlayer& aiPlayer); 
};																													  

struct ActionOrderBuilding : ActionForAI // �������� ������
{
	EnumWrapper<terUnitAttributeID> building; 
	EnumWrapper<PlacementStrategy> placementStrategy; 
	float energyReserve; 
	int buildDurationMax;
	int priority; 

	ActionOrderBuilding() {
		building = UNIT_ATTRIBUTE_CORE;
		placementStrategy = PLACEMENT_STRATEGY_CORE; 
		energyReserve = 50; 
		buildDurationMax = 300;
		priority = 3; 
		initialize();
	}

	void initialize() {
		accepted_ = false;
		workedOut_ = true;
		waitingCounter_ = 0;
	}

	bool automaticCondition(AIPlayer& aiPlayer) const;
	bool workedOut(AIPlayer& aiPlayer); 
	void setAccepted() { workedOut_ = false; accepted_ = true; }
	void setWorkedOut() { workedOut_ = true; }

	template<class Archive>	
	void serialize(Archive& ar) {
		ActionForAI::serialize(ar);
		ar & TRANSLATE_OBJECT(building, "��� ������");
		ar & TRANSLATE_OBJECT(placementStrategy, "��������� ���������");
		ar & TRANSLATE_OBJECT(energyReserve, "������ �������");
		ar & TRANSLATE_OBJECT(buildDurationMax, "������������ ������������ �������������");
		ar & TRANSLATE_OBJECT(priority, "��������� (0 - ����� �������)");
		initialize();
	}

protected:
	bool accepted_;
	mutable bool workedOut_;
	mutable int waitingCounter_;
};																													  

struct ActionHoldBuilding : ActionForAI // ���������� �������������
{
	EnumWrapper<terUnitAttributeID> building; 

	ActionHoldBuilding() {
		building = UNIT_ATTRIBUTE_ANY; 
	}

	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		ActionForAI::serialize(ar);
		ar & TRANSLATE_OBJECT(building, "��� ������");
	}
};

struct ActionSellBuilding : ActionForAI // ������� ������
{
	enum AISellFactor
	{
		AI_SELL_CLOSEST_TO_FRAME, // ��������� � ������
		AI_SELL_FAREST_FROM_FRAME, // ��������� �� ������
		AI_SELL_IF_DAMAGE_GREATER, // ���������, ���� ���� ������ ��������
		AI_SELL_IF_GUN_CANT_REACH_BUILDINGS // ���������, ���� ����� �� ����� ������� �� ������
	};

	EnumWrapper<terUnitAttributeID> building; 
	EnumWrapper<AISellFactor> sellFactor; 
	float damagePercent; 

	ActionSellBuilding() {
		building = UNIT_ATTRIBUTE_CORE; 
		sellFactor = AI_SELL_CLOSEST_TO_FRAME; 
		damagePercent = 0; 
		index_ = 0;
	}

 	bool automaticCondition(AIPlayer& aiPlayer) const;
	void activate(AIPlayer& aiPlayer);
	class terUnitBase* findBuilding(AIPlayer& aiPlayer) const;

	template<class Archive>	
	void serialize(Archive& ar) {
		ActionForAI::serialize(ar);
		ar & TRANSLATE_OBJECT(building, "��� ������");
		ar & TRANSLATE_OBJECT(sellFactor, "������� �������");
		ar & TRANSLATE_OBJECT(damagePercent, "������� �����");
		index_ = 0;
	}

private:
	mutable int index_;
};																													  

struct ActionSwitchGuns : ActionForAI // ��������/��������� �����
{
	enum Mode {
		ON, // ��������
		OFF // ���������
	};
	EnumWrapper<Mode> mode; 
	EnumWrapper<terUnitAttributeID> gunID; 

	ActionSwitchGuns() {
		mode = OFF; 
		gunID = UNIT_ATTRIBUTE_LASER_CANNON; 
	}

	bool automaticCondition(AIPlayer& aiPlayer) const;
	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		ActionForAI::serialize(ar);
		ar & TRANSLATE_OBJECT(mode, "��������");
		ar & TRANSLATE_OBJECT(gunID, "��� �����");
	}
};

struct ActionUpgradeOmega : ActionForAI // �������� �����
{
	bool automaticCondition(AIPlayer& aiPlayer) const;
	void activate(AIPlayer& aiPlayer);
};																													  

//-------------------------------------
struct ActionChargeCores : ActionForAI // �������� ����
{
	enum ChargeCoresStrategy // ��������� ������� ����
	{
		CHARGE_CORES_NONE, // ������� ����
		CHARGE_CORES_INNER, // ���������� ����
		CHARGE_CORES_OUTER, // ������� ����
		CHARGE_CORES_ALL // ��� ����
	};

	EnumWrapper<ChargeCoresStrategy> chargeCoresStrategy; 

	ActionChargeCores() {
		chargeCoresStrategy = CHARGE_CORES_ALL; 
	}

	bool automaticCondition(AIPlayer& aiPlayer) const;
	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		ActionForAI::serialize(ar);
		ar & TRANSLATE_OBJECT(chargeCoresStrategy, "��������� �������");
	}	
};

struct ActionSwitchFieldOn : ActionForAI // �������� ����
{
	int duration;
	float energyReserve; 
	bool allCores; 
	bool onlyIfCoreDamaged; 
	SaveDurationTimer timer;

	ActionSwitchFieldOn() {
		duration = 60;
		energyReserve = 0; 
		allCores = true; 
		onlyIfCoreDamaged = true; 
	}

	bool automaticCondition(AIPlayer& aiPlayer) const;
	void activate(AIPlayer& aiPlayer);
	bool workedOut(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		ActionForAI::serialize(ar);
		ar & TRANSLATE_OBJECT(duration, "����� ������");
		ar & TRANSLATE_OBJECT(energyReserve, "��� ����");
		ar & TRANSLATE_OBJECT(allCores, "��������� �������");
		ar & TRANSLATE_OBJECT(onlyIfCoreDamaged, "������ ����� ������� ����");
		ar & WRAP_OBJECT(timer);
	}	
};

//-------------------------------------
struct ActionSquadOrderUnits : ActionForAI // �������� ������ � �����
{
	EnumWrapper<ChooseSquadID> chooseSquadID; 
	int soldiers; 
	int officers; 
	int technics; 
	float energyReserve; 

	ActionSquadOrderUnits() {
		chooseSquadID = CHOOSE_SQUAD_1; 
		soldiers = 1; 
		officers = 1; 
		technics = 1; 
		energyReserve = 100; 
	}

	bool automaticCondition(AIPlayer& aiPlayer) const;
	void activate(AIPlayer& aiPlayer);
	bool workedOut(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		ActionForAI::serialize(ar);
		ar & TRANSLATE_OBJECT(chooseSquadID, "�����");
		ar & TRANSLATE_OBJECT(soldiers, "�������");
		ar & TRANSLATE_OBJECT(officers, "�������");
		ar & TRANSLATE_OBJECT(technics, "�������");
		ar & TRANSLATE_OBJECT(energyReserve, "������ �������");
	}	
};

struct ActionSquadAttack : ActionForAI // ��������� �������
{
	EnumWrapper<ChooseSquadID> chooseSquadID; 
	EnumWrapper<terUnitAttributeID> attackByType;
	vector<EnumWrapper<terUnitAttributeID> > unitsToAttack; 
	BitVector<terUnitClassType> unitClassToAttack; 
	bool offensive; 
	
	int unitsNumber; 
	int soldiers;
	int officers;
	int technics;
	
	int attackTime;
	int remutateCounter; 
	bool holdProduction;
	
	float squadFollowDistance;
	EnumWrapper<ChooseSquadID> squadToFollowBy;
	bool ignoreLastTarget;
	bool returnToBase; 
	bool interruptable; 
	SaveDurationTimer attackTimer;

	ActionSquadAttack() {
		chooseSquadID = CHOOSE_SQUAD_1;
		attackByType = UNIT_ATTRIBUTE_NONE;
		offensive = true;
		unitsNumber = 0;
		soldiers = 0;
		officers = 0;
		technics = 0;
		attackTime = 180;
		remutateCounter = 1;
		holdProduction = false;
		squadFollowDistance = 0;
		squadToFollowBy = CHOOSE_SQUAD_2;
		ignoreLastTarget = false;
		returnToBase = true;
		interruptable = false;
		initialize();
	}

	void initialize() {
		lastTarget_ = 0;
		lastTargetPosition_ = Vect2f::ZERO;
		interrupt_ = true;
	}

	bool automaticCondition(AIPlayer& aiPlayer) const;
	void activate(AIPlayer& aiPlayer);
	bool workedOut(AIPlayer& aiPlayer);
	void interrupt(class terUnitSquad* squad);

	template<class Archive>	
	void serialize(Archive& ar) {
		ActionForAI::serialize(ar);
		ar & TRANSLATE_OBJECT(chooseSquadID, "�����");
		ar & TRANSLATE_OBJECT(attackByType, "��������� ����� ('�����' - �������)");
		ar & TRANSLATE_OBJECT(unitsToAttack, "��������� �����");
		ar & TRANSLATE_OBJECT(unitClassToAttack, "��������� ������ ������");
		ar & TRANSLATE_OBJECT(offensive, "����������� (��������)");
		
		ar & TRANSLATE_OBJECT(unitsNumber, "���������� �����������");
		ar & TRANSLATE_OBJECT(soldiers, "�������� ��� �������");
		ar & TRANSLATE_OBJECT(officers, "�������� ��� �������");
		ar & TRANSLATE_OBJECT(technics, "�������� ��� �������");
		
		ar & TRANSLATE_OBJECT(attackTime, "����� �����");
		ar & TRANSLATE_OBJECT(remutateCounter, "������������ ��� �����");
		ar & TRANSLATE_OBJECT(holdProduction, "���������� ������������ �� ����� �����");
		
		ar & TRANSLATE_OBJECT(squadFollowDistance, "��������� �� ������� �� ���������� (0 - ���������)");
		ar & TRANSLATE_OBJECT(squadToFollowBy, "�����, �� ������� ���������");
		ar & TRANSLATE_OBJECT(ignoreLastTarget, "������������ ��������� ����");
		ar & TRANSLATE_OBJECT(returnToBase, "������������ �� ����");
		ar & TRANSLATE_OBJECT(interruptable, "�����������");
		ar & WRAP_OBJECT(attackTimer);
		initialize();
	}	

private:
	terUnitBase* lastTarget_;
	Vect2f lastTargetPosition_;
	bool interrupt_;

	terUnitBase* findTarget(AIPlayer& aiPlayer, Vect2iVect& path) const;
};

struct ActionSquadMove : ActionForAI // ������� ����� � ����� ������� �� �����
{
	EnumWrapper<ChooseSquadID> chooseSquadID; 
	CustomString label; 

	ActionSquadMove() :
	label(editLabelDialog)
	{
		chooseSquadID = CHOOSE_SQUAD_1; 
	}

	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		ActionForAI::serialize(ar);
		ar & TRANSLATE_OBJECT(chooseSquadID, "�����");
		ar & TRANSLATE_OBJECT(label, "����� �������");
	}	
};

struct ActionAttackBySpecialWeapon : ActionForAI // ��������� �����������
{
	EnumWrapper<terUnitAttributeID> weapon; 
	vector<EnumWrapper<terUnitAttributeID> > unitsToAttack; 
	BitVector<terUnitClassType> unitClassToAttack; 

	ActionAttackBySpecialWeapon() {
		weapon = UNIT_ATTRIBUTE_GUN_BALLISTIC; 
	}

	bool automaticCondition(AIPlayer& aiPlayer) const;
	void activate(AIPlayer& aiPlayer);
	terUnitBase* findTarget(AIPlayer& aiPlayer, const Vect2f& position, float radiusMin) const;

	template<class Archive>	
	void serialize(Archive& ar) {
		ActionForAI::serialize(ar);
		ar & TRANSLATE_OBJECT(weapon, "����������");
		ar & TRANSLATE_OBJECT(unitsToAttack, "��������� �����");
		ar & TRANSLATE_OBJECT(unitClassToAttack, "��������� ������ ������");
	}
};

struct ActionRepareObjectByLabel : Action // ��������������� ������� �� �����
{
	CustomString label; 

	ActionRepareObjectByLabel() : 
	label(editLabelDialog)
	{}

	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(label, "����� �������");
	}	
};


struct ActionActivateObjectByLabel : Action // ������������ ������ �� �����
{
	CustomString label; 

	ActionActivateObjectByLabel() : 
	label(editLabelDialog)
	{}

	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(label, "����� �������");
	}	
};

struct ActionDeactivateObjectByLabel : Action // �������������� ������ �� �����
{
	CustomString label;

	ActionDeactivateObjectByLabel() : 
	label(editLabelDialog)
	{}

	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(label, "����� �������");
	}	
};

struct ActionActivateAllSpots : Action // ������������ ��� �����
{
	void activate(AIPlayer& aiPlayer);
};

struct ActionDeactivateAllSpots : Action // �������������� ��� �����
{
	void activate(AIPlayer& aiPlayer);
};

struct ActionSetControlEnabled : Action // ���������/��������� ���������� ������
{
	bool controlEnabled; 

	ActionSetControlEnabled() {
		controlEnabled = false; 
	}

	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(controlEnabled, "���������");
	}	
};

//-------------------------------------
struct ActionMessage : Action // C��������
{
	CustomString messageID; 
	CustomString message;
	int delay; 
	int duration;
	bool syncroBySound; 
	SaveDurationTimer delayTimer;											 
	SaveDurationTimer durationTimer;											 

	ActionMessage() :
	messageID(editMessageID),
	message(editTextMultiLine) 
	{
		delay = 0;
		duration = 120;
		syncroBySound = false;
	}

	bool started_;

	void activate(AIPlayer& aiPlayer) { delayTimer.start(delay*1000); started_ = false; }
	bool workedOut(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(messageID, "������������� ���������");
		ar & TRANSLATE_OBJECT(message, "���������");
		ar & TRANSLATE_OBJECT(delay, "��������, �������");
		ar & TRANSLATE_OBJECT(duration, "������������, �������");
		ar & TRANSLATE_OBJECT(syncroBySound, "���������������� �� ������");
		ar & WRAP_OBJECT(delayTimer);
		ar & WRAP_OBJECT(durationTimer);
	}		
};

struct ActionTask : Action // ������
{
	enum Type {
		ASSIGNED, // ���������
		COMPLETED, // ���������
		FAILED, // ���������
		TO_DELETE // �������
	};
	EnumWrapper<Type> type; 
	CustomString taskID; 
	int duration;
	bool syncroBySound;
	bool showTips;
	SaveDurationTimer durationTimer;		

	ActionTask() : 
	taskID(editTaskID) 
	{
		type = ASSIGNED;
		duration = 120;
		syncroBySound = false;
		showTips = true;
	}

	void activate(AIPlayer& aiPlayer);
	bool workedOut(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(type, "��������");
		ar & TRANSLATE_OBJECT(taskID, "������������� ������");
		ar & TRANSLATE_OBJECT(duration, "������������, �������");
		ar & TRANSLATE_OBJECT(syncroBySound, "���������������� �� ������");
		ar & TRANSLATE_OBJECT(showTips, "�������� ���������");
		ar & WRAP_OBJECT(durationTimer);
	}	
};

struct ActionSetCameraAtObject : Action // ���������� ������ �� ������
{
	EnumWrapper<terUnitAttributeID> object; 
	EnumWrapper<AIPlayerType> playerType; 
	int transitionTime; 
	bool setFollow;
	int turnTime; 

	ActionSetCameraAtObject() {
		object = UNIT_ATTRIBUTE_FRAME;
		playerType = AI_PLAYER_TYPE_ME;
		transitionTime = 0;
		setFollow = false;
		turnTime = 0;
	}

	void activate(AIPlayer& aiPlayer);
	bool workedOut(AIPlayer& aiPlayer);
	terUnitBase* findUnit(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(object, "������");
		ar & TRANSLATE_OBJECT(playerType, "�������� �������");
		ar & TRANSLATE_OBJECT(transitionTime, "����� ��������, �������");
		ar & TRANSLATE_OBJECT(setFollow, "���������� ��������");
		ar & TRANSLATE_OBJECT(turnTime, "����� ��������");
	}

private:
	bool turnStarted_;
};

//---------------------------------
struct SaveControlData
{
	EnumWrapper<ShellControlID> controlID; 
	bool enabled; 
	bool visible; 
	bool flashing; 
	int tabNumber; 

	SaveControlData() {
		controlID = SQSH_STATIC_ID;
		enabled = true;
		visible = true;
		flashing = false;
		tabNumber = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(controlID, "&������������� ������");	
		ar & TRANSLATE_OBJECT(enabled, "���������");	
		ar & TRANSLATE_OBJECT(visible, "������");	
		ar & TRANSLATE_OBJECT(flashing, "������");	
		ar & TRANSLATE_OBJECT(tabNumber, "����� ��������");	
	}
};

struct ActionSetControls : Action // ���������� ��������� ������
{
	vector<SaveControlData> controls; 

	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(controls, "������");
	}
};

struct ActionSelectUnit : Action // ������������� �����
{
	EnumWrapper<terUnitAttributeID> unitID; 

	ActionSelectUnit() {
		unitID = UNIT_ATTRIBUTE_SQUAD; 
	}

	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(unitID, "������������� �����");
	}	
};

struct ActionProduceBrigadierOrProrab : Action // ���������� ��������� ��� �������
{
	bool produceBrigadier; 

	ActionProduceBrigadierOrProrab() {
		produceBrigadier = true; 
	}

	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(produceBrigadier, "���������� ���������");
	}	
};

struct ActionFrameMove : Action // ������� ����� � �����
{
	CustomString label; 

	ActionFrameMove() :
	label(editLabelDialog)
	{ }

	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);

		ar & TRANSLATE_OBJECT(label, "����� �������");
	}	
};

struct ActionFrameDetach : Action // ������� �����
{
	ActionFrameDetach(){ }

	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
	}	
};

struct ActionSetInterface : Action // ��������/��������� ���������
{
	bool enableInterface; 

	ActionSetInterface() {
		enableInterface = true; 
	}

	void activate(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Action::serialize(ar);
		ar & TRANSLATE_OBJECT(enableInterface, "�������� ���������");
	}	
};

//---------------------------------
struct SavePlayerManualData
{
	PrmStringList triggerChainNamesOld; 

	struct TriggerChainName : CustomString
	{
		TriggerChainName() : CustomString(editTriggerChainNameDialog) {}
		TriggerChainName(const char* nameIn) : CustomString(editTriggerChainNameDialog, nameIn) {}

		template<class Archive>	
		void serialize(Archive& ar) {
			ar & TRANSLATE_NAME(static_cast<CustomString&>(*this), "name", "&���");
		}	
	};
	typedef vector<TriggerChainName> TriggerChainNames;
	TriggerChainNames triggerChainNames; 
	TriggerChain triggerChainOld; 

	template<class Archive>	
	void serialize(Archive& ar) {
		if(ar.isInput())
			ar & WRAP_NAME(triggerChainOld, "strategy");
		ar & TRANSLATE_NAME(triggerChainNames, "TriggerChainNames", "��������");
		ar & WRAP_NAME(triggerChainNamesOld, "triggerChainNames");
		if(!ar.isOutput()){
			PrmStringList::iterator i;
			FOR_EACH(triggerChainNamesOld, i)
				triggerChainNames.push_back(TriggerChainName(*i));
			triggerChainNamesOld.clear();
		}
	}	
};


struct SavePlayerStats
{
	int maxLeveledArea;
	int maxZeroedArea;
	int scourgeKilled;
	float medEfficiency;
	int effQuants;
	float energy;

	int unitCount;
	int unitKilled; 
	int unitLost;

	int buildings;
	int buildingRazed; 
	int buildingCaptured;
	int buildingLost;

	SavePlayerStats() {
		maxLeveledArea = 0;
		maxZeroedArea = 0;
		scourgeKilled = 0;
		medEfficiency = 0;
		effQuants = 0;
		energy = 0;
		unitCount = 0;
		unitKilled = 0;
		unitLost = 0;
		buildings = 0;
		buildingRazed = 0;
		buildingCaptured = 0;
		buildingLost = 0;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(maxLeveledArea);
		ar & WRAP_OBJECT(maxZeroedArea);
		ar & WRAP_OBJECT(scourgeKilled);
		ar & WRAP_OBJECT(medEfficiency);
		ar & WRAP_OBJECT(effQuants);
		ar & WRAP_OBJECT(energy);
		
		ar & WRAP_OBJECT(unitCount);
		ar & WRAP_OBJECT(unitKilled);
		ar & WRAP_OBJECT(unitLost);
		
		ar & WRAP_OBJECT(buildings);
		ar & WRAP_OBJECT(buildingRazed);
		ar & WRAP_OBJECT(buildingCaptured);
		ar & WRAP_OBJECT(buildingLost);
	}
};

struct SavePlayerData
{
	ShareHandle<SaveUnitData> frame;
	vector<ShareHandle<SaveUnitData> > buildings;
	vector<ShareHandle<SaveUnitData> > catchedFrames;
	int compAndUserID;
	SavePlayerStats playerStats;
	vector<ShareHandle<SaveUnitData> > commonObjects;
	list<TriggerChain> currentTriggerChains;

	SavePlayerData() {
		compAndUserID = 0;
	}
	
	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(frame);
		ar & WRAP_OBJECT(buildings);
		ar & WRAP_OBJECT(catchedFrames);
		ar & WRAP_OBJECT(compAndUserID);
		ar & WRAP_OBJECT(playerStats);
		ar & WRAP_OBJECT(commonObjects);
		ar & WRAP_OBJECT(currentTriggerChains);
	}
};

struct SaveObjectsData
{
	vector<ShareHandle<SaveUnitData> > objects; 

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(objects, "�������");
	}
};

struct SaveWorldObjects
{
	vector<ShareHandle<SaveUnitData> > alphaPotentials;

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(alphaPotentials);
	}
};

struct SaveBuildingInstallerInstruction // ����������� ������������ ������
{
	EnumWrapper<terUnitAttributeID> building; 
	CustomString label;
	float distance; 
	bool labeledObjectActivity;

	SaveBuildingInstallerInstruction() :
	label(editLabelDialog) 
	{
		building = UNIT_ATTRIBUTE_NONE;
		distance = 50;
		labeledObjectActivity = true;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(building, "������");
		ar & TRANSLATE_OBJECT(label, "����� �������");
		ar & TRANSLATE_OBJECT(distance, "������������ ����������");
		ar & TRANSLATE_OBJECT(labeledObjectActivity, "������-����� ������ ���� �������");
	}
};

//---------------------------------
struct SaveTask
{
	PrmString id;
	EnumWrapper<ActionTask::Type> type;

	SaveTask() {
		type = ActionTask::ASSIGNED;
	}

	SaveTask(const char* idIn, ActionTask::Type typeIn) : id(idIn), type(typeIn) {}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(id);
		ar & WRAP_OBJECT(type);
	}
};

//---------------------------------
struct SaveManualData // ������, ������������� ������
{
	SoundTracks soundTracks;
	vector<SavePlayerManualData> players; 
	vector<SaveControlData> controls; 
	
	float spiralChargingEnergy; 
	int spiralChargingTime;
	int spiralChargingPriority;
	int zeroLayerHeight; 
	vector<SaveCameraSplineData> cameras; 
	vector<SaveBuildingInstallerInstruction> buildingInstallerInstructions; 

	enum OmegaMissionType { 
		OMEGA_UPGRADE, // �������������
		OMEGA_MINE, // ������������
		OMEGA_DEMINE // �������������
	};
	EnumWrapper<OmegaMissionType> omegaMissionType; 
	float alphaActivationDistance; 
	float omegaActivationDistance; 
	bool interfaceEnabled;
	bool loadHardness;

	SaveManualData() {
		spiralChargingEnergy = 2;
		spiralChargingTime = 100000;
		spiralChargingPriority = 170;
		zeroLayerHeight = - 1;
		omegaMissionType = OMEGA_UPGRADE;
		alphaActivationDistance = 200;
		omegaActivationDistance = 1000;
		interfaceEnabled = true;
		loadHardness = true;
	}
	
	const char* popupCameraSplineName() const;
	const SaveCameraSplineData* findCameraSpline(const char* name) const;
	void saveCamera(int playerID, const char* triggerName);

	template<class Archive>	
	void serialize(Archive& ar) {
		if(ar.type() & ARCHIVE_EDIT){
			if(ar.openBlock("Sound tracks", "����������� �����")){
				ar & TRANSLATE_OBJECT(soundTracks[0], "Construction");
				ar & TRANSLATE_OBJECT(soundTracks[1], "Battle");
				ar & TRANSLATE_OBJECT(soundTracks[2], "Regular");
				ar.closeBlock();
			}
		}
		else
			ar & TRANSLATE_OBJECT(soundTracks, "����������� �����");

		ar & TRANSLATE_OBJECT(players, "������");
		ar & TRANSLATE_OBJECT(controls, "������");
		
		ar & TRANSLATE_OBJECT(spiralChargingEnergy, "������� ������� �������");
		ar & TRANSLATE_OBJECT(spiralChargingTime, "����� ������� �������");
		ar & TRANSLATE_OBJECT(spiralChargingPriority, "��������� ������� �������");
		ar & TRANSLATE_OBJECT(zeroLayerHeight, "������� ����������");
		ar & TRANSLATE_OBJECT(cameras, "������");
		ar & TRANSLATE_OBJECT(buildingInstallerInstructions, "����������� ������������ ������");
		
		ar & TRANSLATE_OBJECT(omegaMissionType, "������ �����-������");
		ar & TRANSLATE_OBJECT(alphaActivationDistance, "���������� ��������� �����-��������");
		ar & TRANSLATE_OBJECT(omegaActivationDistance, "���������� ��������� �����-��������");
		ar & TRANSLATE_OBJECT(interfaceEnabled, "��������� �������");
		ar & TRANSLATE_OBJECT(loadHardness, "������� ����� ������������");
	}

private:
	SaveCameraSplineData* findCameraSpline(const char* name);
};

//---------------------------------
struct SavePrm {
	vector<SavePlayerData> players; 

	SaveObjectsData environment; 
	SaveObjectsData filth; 
	SaveObjectsData nobodysBuildings; 

	SaveWorldObjects worldObjects; 

	vector<SaveTask> activeTasks;

	SaveManualData manualData; 

	SavePrm() {}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & WRAP_NAME(players, "players");

		ar & TRANSLATE_NAME(environment, "environment", "������� ���������");
		ar & TRANSLATE_NAME(filth, "filth", "�������");
		ar & TRANSLATE_NAME(nobodysBuildings, "nobodysBuildings", "����������� ������");

		ar & WRAP_NAME(worldObjects, "worldObjects");
		ar & WRAP_NAME(activeTasks, "activeTasks");
		ar & TRANSLATE_NAME(manualData, "manualData", "������, ������������� �������");
	}
};

typedef vector<ShareHandle<Condition> > ConditionList;
typedef vector<ShareHandle<SaveUnitData> > SaveUnitDataList;
typedef vector<SavePlayerData> SavePlayerDataList;
typedef vector<SavePlayerManualData> SavePlayerManualDataList;
typedef vector<SaveControlData> SaveControlDataList;

typedef vector<EnumWrapper<terUnitAttributeID> > UnitAttributeIDList;
typedef vector<SaveTask> SaveTaskList;

//--------------------------------------
template<class D, class B>
D* castOrCreate(B* baseData)
{
	return baseData ? safe_cast<D*>(baseData) : new D;
}


#endif //__SAVE_PARSER_H__