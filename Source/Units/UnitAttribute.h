#ifndef __UNIT_ATTRIBUTE_H__
#define __UNIT_ATTRIBUTE_H__

#include "../Util/Serialization.h"
#include "../Util/TypeLibrary.h"

#ifndef _UNIT_ATTRIBUTE_INL_
#define _UNIT_ATTRIBUTE_INL_
#include "../Scripts/UnitAttribute.inl"
#include "../Scripts/SquadShellEnums.inl"
#endif // _UNIT_ATTRIBUTE_INL_

#ifndef __TOOLZER_ENUMS_INL__
#define __TOOLZER_ENUMS_INL__
#include "../Scripts/ToolzerEnums.inl"
#endif // __TOOLZER_ENUMS_INL__

#include "../Physics/RigidBodyPrm.h"
#include "../Util/DebugUtil.h"
#include "../UserInterface/SoundTrack.h"

typedef vector<Vect2f> Vect2fVect;
typedef vector<Vect2i> Vect2iVect;
typedef vector<Vect3f> Vect3fVect;
typedef vector<Vect3f> Vect3fList;

class AttributeBase;
class EffectKey;
class EffectLibrary;

inline bool isBuilding(terUnitAttributeID ID) { return ID >= UNIT_ATTRIBUTE_CORE && ID < UNIT_ATTRIBUTE_STRUCTURE_MAX; }
inline bool isLegionary(terUnitAttributeID ID) { return ID >= UNIT_ATTRIBUTE_SOLDIER && ID < UNIT_ATTRIBUTE_LEGIONARY_MAX; }


/////////////////////////////////////////
struct SoundControllerSetup
{
	EnumWrapper<SoundID> ID;
	PrmString name;
	bool cycled;

	SoundControllerSetup() {
		ID = SOUND_NONE;
		cycled = false;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(ID, "&ID");
		ar & TRANSLATE_OBJECT(name, "&name");
		ar & TRANSLATE_OBJECT(cycled, "cycled");
	}
};

struct SoundEventSetup
{
	EnumWrapper<SoundEventID> ID;
	PrmString name;

	// = 1 ���� ���� �� ����
	bool is3D;

	// = 1 ���� ��������� ���������, ���������� ������ ��� ��������� ������
	bool isVoice;

	// = 1 ���� ������� ������ ������� ������ ��� ��������� ������
	bool activePlayer;

	// ����� � �������������, � ������� �������� ����� 
	// ������ ����� ������ ��������� ��� ��� ���
	int pauseTime;

	// �������� ����� ������������� ����� � �������������
	int startDelay;

	// ������������ �����, ������� ������� ����� ������ � ������� �� ������������ (��� �������)
	int queueTime;

	SoundEventSetup() {
		ID = SOUND_EVENT_NONE;
		is3D = 0;
		isVoice = 0;
		activePlayer = 0;
		pauseTime = 0;
		startDelay = 0;
		queueTime = 2000;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(ID, "&ID");
		ar & TRANSLATE_OBJECT(name, "&name");
		ar & TRANSLATE_OBJECT(is3D, "is3D");
		ar & TRANSLATE_OBJECT(isVoice, "isVoice");
		ar & TRANSLATE_OBJECT(activePlayer, "activePlayer");
		ar & TRANSLATE_OBJECT(pauseTime, "pauseTime");
		ar & TRANSLATE_OBJECT(startDelay, "startDelay");
		ar & TRANSLATE_OBJECT(queueTime, "queueTime");
	}
};

struct SoundSetup
{
	vector<SoundControllerSetup> sounds;
	vector<SoundEventSetup> events;

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(sounds, "sounds");
		ar & TRANSLATE_OBJECT(events, "events");
	}

	bool enabled() const {
		return !sounds.empty() || !events.empty();
	}

	const SoundEventSetup* event(int idx) const {
		xassert(!events.empty());
		return &events[idx];
	}

	int eventIndex(SoundEventID event_id) const {
		for(int i = 0; i < events.size(); i++){
			if(events[i].ID == event_id)
				return i;
		}

		return -1;
	}

	const SoundEventSetup* findEvent(SoundEventID event_id) const 
	{
		for(int i = 0; i < events.size(); i++){
			if(events[i].ID == event_id)
				return &events[i];
		}

		return NULL;
	}
};

extern SoundSetup playerSound;

/////////////////////////////////////////////
// ��������� ������������ �������
struct AnimationChain
{
	EnumWrapper<ChainID> chainID;
	PrmString chainName;
	float phase;   // 0..1
	int period; // mS
	EnumWrapper<ChainNodeID> begin;
	EnumWrapper<ChainNodeID> end;
	int periodDeviation; // !0: period += rnd(perionDeviation), phase = rnd(1)
	int enableReverse; // ��������� ������ ����� �������� ���������
	int counter; // for path finding
	EnumWrapper<SoundEventID> soundID;

	AnimationChain() {
		chainID = CHAIN_NONE;
		phase = 0;   // 0..1
		period = 2000; // mS
		begin = CHAIN_NODE_NONE;
		end = CHAIN_NODE_NONE;
		periodDeviation = 0; // !0: period += rnd(perionDeviation), phase = rnd(1)
		enableReverse = 0; // ��������� ������ ����� �������� ���������
		counter = 0; // for path finding
		soundID = SOUND_EVENT_NONE;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(chainID, "&chainID");
		ar & TRANSLATE_OBJECT(chainName, "&chainName");
		ar & TRANSLATE_OBJECT(phase, "phase");
		ar & TRANSLATE_OBJECT(period, "period");
		ar & TRANSLATE_OBJECT(begin, "begin");
		ar & TRANSLATE_OBJECT(end, "end");
		ar & TRANSLATE_OBJECT(periodDeviation, "periodDeviation");
		ar & TRANSLATE_OBJECT(enableReverse, "enableReverse");
		ar & TRANSLATE_OBJECT(counter, "counter");
		ar & TRANSLATE_OBJECT(soundID, "soundID");
	}

	bool cycled() const { return begin == end; }
	float phaseEnd() const { return period > 0 ? 1 : (period < 0 ? 0 : phase); }
};

// ����� ������������ ������� ��� ������������� ����
struct AnimationData
{
	EnumWrapper<AnimationGroupID> groupID;
	PrmString groupName;
	vector<AnimationChain> chains;
	bool setPhaseRecursive;

	AnimationData() {
		groupID = ANIMATION_GROUP_ROOT;
		groupName = 0;
		setPhaseRecursive = false;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(groupID, "&groupID");
		ar & TRANSLATE_OBJECT(groupName, "&groupName");
		ar & TRANSLATE_OBJECT(chains, "chains");
		ar & TRANSLATE_OBJECT(setPhaseRecursive, "setPhaseRecursive");
	}

	const AnimationChain* initial() const { return !chains.empty() ? &chains[0] : 0; }	// ������ ������� ���������� ��� �������� �������
	const AnimationChain* find(ChainID chainID) const  { for(int i = 0; i < chains.size(); i++) if(chains[i].chainID == chainID) return &chains[i]; return 0; }
};

////////////////////////////////////////
struct InterfaceTV
{
	float radius;
	float pos_dx;
	float pos_dy;
	float angle_x;
	float angle_y;
	float angle_z;
	PrmString model;
	PrmString channel;
	PrmString channel_power_on;

	InterfaceTV() {
		radius = 80;
		pos_dx = 0;
		pos_dy = 0;
		angle_x = 70;
		angle_y = 0;
		angle_z = 0;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(radius, "radius");
		ar & TRANSLATE_OBJECT(pos_dx, "pos_dx");
		ar & TRANSLATE_OBJECT(pos_dy, "pos_dy");
		ar & TRANSLATE_OBJECT(angle_x, "angle_x");
		ar & TRANSLATE_OBJECT(angle_y, "angle_y");
		ar & TRANSLATE_OBJECT(angle_z, "angle_z");
		ar & TRANSLATE_OBJECT(model, "model");
		ar & TRANSLATE_OBJECT(channel, "channel");
		ar & TRANSLATE_OBJECT(channel_power_on, "channel_power_on");
	}
};

////////////////////////////////////////
struct UnitInterfaceActions
{
	int op_move;
	int op_stop;
	int op_stop2;
	int op_attack;
	int op_back;
	int op_offdeff;
	int op_patrol;
	int op_power_onoff;
	int op_sell;
	int op_upgrade;
	int op_field;
	int op_install;
	int op_teleportate;
	int op_alarm;
	int op_brig_back;
	int op_brig_change;
	int op_brig_build;
	int op_charging;
	int op_charge_control;

	UnitInterfaceActions() {
		op_move = 0;
		op_stop = 0;
		op_stop2 = 0;
		op_attack = 0;
		op_back = 0;
		op_offdeff = 0;
		op_patrol = 0;
		op_power_onoff = 0;
		op_sell = 0;
		op_upgrade = 0;
		op_field = 0;
		op_install = 0;
		op_teleportate = 0;
		op_alarm = 0;
		op_brig_back = 0;
		op_brig_change = 0;
		op_brig_build = 0;
		op_charging = 0;
		op_charge_control = 0;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(op_move, "op_move");
		ar & TRANSLATE_OBJECT(op_stop, "op_stop");
		ar & TRANSLATE_OBJECT(op_stop2, "op_stop2");
		ar & TRANSLATE_OBJECT(op_attack, "op_attack");
		ar & TRANSLATE_OBJECT(op_back, "op_back");
		ar & TRANSLATE_OBJECT(op_offdeff, "op_offdeff");
		ar & TRANSLATE_OBJECT(op_patrol, "op_patrol");
		ar & TRANSLATE_OBJECT(op_power_onoff, "op_power_onoff");
		ar & TRANSLATE_OBJECT(op_sell, "op_sell");
		ar & TRANSLATE_OBJECT(op_upgrade, "op_upgrade");
		ar & TRANSLATE_OBJECT(op_field, "op_field");
		ar & TRANSLATE_OBJECT(op_install, "op_install");
		ar & TRANSLATE_OBJECT(op_teleportate, "op_teleportate");
		ar & TRANSLATE_OBJECT(op_alarm, "op_alarm");
		ar & TRANSLATE_OBJECT(op_brig_back, "op_brig_back");
		ar & TRANSLATE_OBJECT(op_brig_change, "op_brig_change");
		ar & TRANSLATE_OBJECT(op_brig_build, "op_brig_build");
		ar & TRANSLATE_OBJECT(op_charging, "op_charging");
		ar & TRANSLATE_OBJECT(op_charge_control, "op_charge_control");
	}

	void set(){
		op_move = op_stop = op_stop2 = op_attack = op_back = op_patrol = op_offdeff = 0;
		op_power_onoff = op_sell = op_upgrade = 
		op_field = op_install = op_teleportate = op_alarm = op_brig_back = op_brig_change = op_brig_build = op_charging = op_charge_control = 1;
	}

	void clear(){
		op_move = op_stop = op_stop2 = op_attack = op_back = op_offdeff = 
		op_patrol = op_power_onoff = op_sell = op_upgrade = 
		op_field = op_install = op_teleportate = op_alarm = op_brig_back = op_brig_change = op_brig_build = op_charging = op_charge_control = 0;
	}

	UnitInterfaceActions& operator&=(const UnitInterfaceActions& act){
		op_move |= act.op_move;
		op_stop |= act.op_stop;
		op_stop2 |= act.op_stop2;
		op_attack |= act.op_attack;
		op_back |= act.op_back;
		op_offdeff |= act.op_offdeff;
		op_patrol |= act.op_patrol;
		op_power_onoff &= act.op_power_onoff;
		op_sell &= act.op_sell;
		op_upgrade &= act.op_upgrade;
		op_field &= act.op_field;
		op_install &= act.op_install;
		op_teleportate &= act.op_teleportate;
		op_alarm &= act.op_alarm;
		op_brig_back &= act.op_brig_back;
		op_brig_change &= act.op_brig_change;
		op_brig_build &= act.op_brig_build;
		op_charging &= act.op_charging;
		op_charge_control &= act.op_charge_control;

		return *this;
	}

};

struct UnitInterfacePrm
{
	UnitInterfaceActions actions;

	EnumWrapper<ShellControlID> upgrade_button_id;
	EnumWrapper<ShellControlID> upgrading_button_id;

	EnumWrapper<PopupFormatGroup> format_group;
	PrmString popup;

	UnitInterfacePrm() {
		upgrade_button_id = SQSH_SELPANEL_UPGRADE_ID;
		upgrading_button_id = SQSH_SELPANEL_UPGRADE_ID;
		format_group = POPUP_FORMAT_NONE;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(actions, "actions");
		ar & TRANSLATE_OBJECT(upgrade_button_id, "upgrade_button_id");
		ar & TRANSLATE_OBJECT(upgrading_button_id, "upgrading_button_id");
		ar & TRANSLATE_OBJECT(format_group, "format_group");
		ar & TRANSLATE_OBJECT(popup, "popup");
	}
};

////////////////////////////////////////
// ��������� ����������� �������
struct ToolzerActionData
{
	// ��� �����������
	EnumWrapper<ToolzerActionID> actionID;

	// int influenceDZ(int x, int y, int rad, short dh, int smMode);
	// dh = dz * (1 << VX_FRACTION)

	int rad;
	float dz;

	int smMode;

	// hAppr < 0: individualToolzer::hAppr = vMap.hZeroPlast << VX_FRACTION
	// hAppr >= 0: individualToolzer::hAppr = hAppr << VX_FRACTION
	int hAppr;

	ToolzerActionData() {
		actionID = TOOLZER_4ZP;
		rad = 10;
		dz = 1;
		smMode = 5;
    	hAppr = -1;
 	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(actionID, "actionID");
		ar & TRANSLATE_OBJECT(rad, "rad");
		ar & TRANSLATE_OBJECT(dz, "dz");
		ar & TRANSLATE_OBJECT(smMode, "smMode");
		ar & TRANSLATE_OBJECT(hAppr, "hAppr");
	}
};

// ��������� ����������� ������� � ������������ ����
struct ToolzerStepData
{
	EnumWrapper<ToolzerPhaseID> phaseID;

	// ����� ������ ���� � ���������� �������
	int duration;

	// ��������� ����
	// TOOLZER_PHASE_DEFAULT - ���������� � ��������� �� �������
	// TOOLZER_PHASE_NONE - ���������� ������ �������
	EnumWrapper<ToolzerPhaseID> nextPhaseID;

	// ����������� ��������� ��������� ������� ��� ���������� �����������
	//
	// �������, ����� �������� ����������� ��� �� ����� ����� ������ �����
	// ��� �������� ��������
	float dr;

	// ����������� �������, ����� � ������
	EnumWrapper<ToolzerBuildingDamageMode> buildingDamageMode;
	int buildingDamageRadius;
	
	vector<ToolzerActionData> actions;

	ToolzerStepData() {
		phaseID = TOOLZER_PHASE_DEFAULT;
		duration = 1;
		nextPhaseID = TOOLZER_PHASE_DEFAULT;
		dr = 0;
		buildingDamageMode = BUILDING_DAMAGE_SQUARE;
		buildingDamageRadius = 0;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(phaseID, "phaseID");
		ar & TRANSLATE_OBJECT(duration, "duration");
		ar & TRANSLATE_OBJECT(nextPhaseID, "nextPhaseID");
		ar & TRANSLATE_OBJECT(dr, "dr");
		ar & TRANSLATE_OBJECT(buildingDamageMode, "buildingDamageMode");
		ar & TRANSLATE_OBJECT(buildingDamageRadius, "buildingDamageRadius");
		ar & TRANSLATE_OBJECT(actions, "actions");
	}

	const ToolzerActionData& operator[](int idx) const { xassert(!actions.empty()); return actions[idx]; }
};

// ��������� �������
struct ToolzerSetup
{
	vector<ToolzerStepData> steps;

	// �����. ��������������� �������
	float scale;

	// = 1 ���� ������ ����� ����������� ����-����
	bool destroyOwnZeroLayer;

	// = 1 ���� �������� �� �����
	bool workOnChaos;

	// ������ ��� ��������� ��������
	float radius;

	ToolzerSetup() {
		scale = 1.0f;
		destroyOwnZeroLayer = 0;
		workOnChaos = 1;
		radius = 48;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(steps, "steps");
		ar & TRANSLATE_OBJECT(scale, "scale");
		ar & TRANSLATE_OBJECT(destroyOwnZeroLayer, "destroyOwnZeroLayer");
		ar & TRANSLATE_OBJECT(workOnChaos, "workOnChaos");
		ar & TRANSLATE_OBJECT(radius, "radius");
	}

	int stepIndex(ToolzerPhaseID phase) const
	{
		for(int i = 0; i < steps.size(); i++){
			if(steps[i].phaseID == phase)
				return i;
		}

		return -1;
	}

	bool enabled() const {
		return !steps.empty();
	}

	const ToolzerStepData& step(int idx) const { 
		xassert(!steps.empty()); 
		return steps[idx]; 
	}
};

////////////////////////////////////////
// ��������� �����������, ��������� �����
struct DamageData
{
	// ������ �����
	int width;
	// �������� �����
	int power;

	// ������ �� ��������, �� ������� ������� ����� ��� �����
	// ��������: DAMAGE_FILTER_ALL ��� ���������� �� ������ �������� DamageElementFilter
	BitVector<DamageElementFilter> attackFilter;
	// ������ �� ��������, ������� ����� ������� ��� �������� ���������
	BitVector<DamageElementFilter> damageFilter;
		
	DamageData() {
		clear();
	}

	void clear() { 
		width = power = 0; 
		attackFilter = damageFilter = DAMAGE_FILTER_ALL; 
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(width, "������ �����");
		ar & TRANSLATE_OBJECT(power, "�������� �����");
		ar & TRANSLATE_OBJECT(attackFilter, "������ �� ��������, �� ������� ������� ����� ��� �����");
		ar & TRANSLATE_OBJECT(damageFilter, "������ �� ��������, ������� ����� ������� ��� �������� ���������");
	}
};

/////////////////////////////////////////////////////////////////////////////////
// ��������� �����������, ��������� ����� (����, ������������ �������� ��� ��������)
struct EnvironmentalDamage
{
	// ��� �����������
	EnumWrapper<EnvironmentalDamageType> damageType;
	// ��������� �����������, [0] - �������, [1] - ��������
	// ���� width == -1, �� ����
	// ������������� ����� ���������� ������ �����
	DamageData damageData[2];

	// ������������� ��������� �����������, ��� � �������������
	//
	// ����������� ��������� ����� ����� t = period + periodDelta * (1 - damage_ratio)
	// damage_ratio - ��������, ������������ ������������� �����������,
	// ��������� ������� ��������� ����������� ����������� (���� ��� ���������� ��������)
	// � ����� �������, ���������� ������
	int period;
	int periodDelta;

	// ����������� �������� damage_ratio,
	// ��� ������� ����� �������� ���������� �����������
	float damageRatioMin;

	EnvironmentalDamage() {
		damageType = ENV_DAMAGE_NONE;
		period = 0;
		periodDelta = 0;
		damageRatioMin = 0.01f;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(damageType, "��� �����������");
		ar & TRANSLATE_OBJECT(damageData, "��������� �����������, [0] - �������, [1] - ��������");
		ar & TRANSLATE_OBJECT(period, "period");
		ar & TRANSLATE_OBJECT(periodDelta, "periodDelta");
		ar & TRANSLATE_OBJECT(damageRatioMin, "damageRatioMin");
	}

	const DamageData damage(float damage_ratio) const 
	{
		DamageData data = damageData[0];

		if(data.width != -1)
			data.width += round(float(damageData[1].width - damageData[0].width) * damage_ratio);

		data.power += round(float(damageData[1].power - damageData[0].power) * damage_ratio);

		return data;
	}

	bool enabled() const {
		return damageType != ENV_DAMAGE_NONE;
	}
};



// ��������� ��������� ������ �����������
struct UnitDamage
{
	// �����������, ��������� ��������������� � ����� ���������
	DamageData mainDamage;

	int splashDamageRadius;
	// �����������, ��������� � ������� splashDamageRadius �� ����� ���������
	DamageData splashDamage;

	UnitDamage() {
		splashDamageRadius = 0;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(mainDamage, "mainDamage");
		ar & TRANSLATE_OBJECT(splashDamageRadius, "splashDamageRadius");
		ar & TRANSLATE_OBJECT(splashDamage, "splashDamage");
	}

	// ������ ���������� ���������, ���������� � ������ ����
	int estimatedDamage() const { 
		return ((mainDamage.power + splashDamage.power + 3)/8 + 1); 
	}
};

////////////////////////////////////////
struct ConsumptionData
{
	float energy;
	int time; // in milliseconds
	int priority;
	PrmString name;

	ConsumptionData() {
		energy = 0;
		time = 100; // in milliseconds
		priority = 0;
		name = "noname";
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(energy, "energy");
		ar & TRANSLATE_OBJECT(time, "time (in milliseconds)");
		ar & TRANSLATE_OBJECT(priority, "priority");
		ar & TRANSLATE_OBJECT(name, "name");
	}

	bool enabled() const {
		return energy;
	}
};

extern ConsumptionData buildingBlockConsumption;

struct FieldPrm
{
	int monksPerCore; 
	int monksPerCoreMin;
	float monksVelocity; // �������� ��� ���������� ��������
	float monksAccelerationFactor; 
	float monksPositionFactor;
	int monksWetPlace; // ������ ������������� ����������� ��� ������ ����������
	float chargeLevelToStartMin; // ���� � ����� ������� ������� ��������� ��� ��������� ������
	ConsumptionData fieldConsumption;
	float checkFieldRadiusMin;
	DamageData fieldDamage;
	int impulseDuration; 
	DamageData impulseFieldDamage;

	FieldPrm() 
	{
		monksPerCore = 20; 
		monksPerCoreMin = 3;
		monksVelocity = 5; // �������� ��� ���������� ��������
		monksAccelerationFactor = 0.2f; 
		monksPositionFactor = 0.1f;
		monksWetPlace = 8; // ������ ������������� ����������� ��� ������ ����������
		chargeLevelToStartMin = 0.1f; // ���� � ����� ������� ������� ��������� ��� ��������� ������
		checkFieldRadiusMin = 8;
		impulseDuration = 2000; 
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(monksPerCore, "monksPerCore"); 
		ar & TRANSLATE_OBJECT(monksPerCoreMin, "monksPerCoreMin"); 
		ar & TRANSLATE_OBJECT(monksVelocity, "monksVelocity"); 
		ar & TRANSLATE_OBJECT(monksAccelerationFactor, "monksAccelerationFactor"); 
		ar & TRANSLATE_OBJECT(monksPositionFactor, "monksPositionFactor"); 
		ar & TRANSLATE_OBJECT(monksWetPlace, "������ ������������� ����������� ��� ������ ����������"); 
		ar & TRANSLATE_OBJECT(chargeLevelToStartMin, "���� � ����� ������� ������� ��������� ��� ��������� ������"); 
		ar & TRANSLATE_OBJECT(fieldConsumption, "fieldConsumption"); 
		ar & TRANSLATE_OBJECT(checkFieldRadiusMin, "checkFieldRadiusMin"); 
		ar & TRANSLATE_OBJECT(fieldDamage, "fieldDamage"); 
		ar & TRANSLATE_OBJECT(impulseDuration, "impulseDuration"); 
		ar & TRANSLATE_OBJECT(impulseFieldDamage, "impulseFieldDamage"); 
	}
};

extern FieldPrm fieldPrm;

//-------------------------
struct DebugScales
{
	float frame; // �����
	float legion; // ������
	float buildins; // ������
	float spheres; // �����
	float other; // ���������

	DebugScales() {
		frame = 1; // �����
		legion = 0.5; // ������
		buildins = 0.5; // ������
		spheres = 0.5; // �����
		other = 0.5; // ���������
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(frame, "�����"); 
		ar & TRANSLATE_OBJECT(legion, "������"); 
		ar & TRANSLATE_OBJECT(buildins, "������"); 
		ar & TRANSLATE_OBJECT(spheres, "�����"); 
		ar & TRANSLATE_OBJECT(other, "���������"); 
	}
};

extern DebugScales debuScales; // ���������� ���������������

//-----------------------------
struct DifficultyPrm
{
	PrmString name;

	//filthDensity - ������ �� ���������� ������������ ������� � ������ ���������.
	//mul=1 - ��� �������� ��� ���������.
	//��� ���������� - ����������� ������� ���������� �������, �����
	//���������� ������� ���������� �� 1, �������� �������������� ������ ���������.
	float filthDensity;

	int aiDelay; // �������� ��

	float triggerDelayFactor; // ����������� �������� ��������

	DifficultyPrm() {
		filthDensity = 1;
        aiDelay = 0; 
		triggerDelayFactor = 1; 
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(name, "&name"); 
		ar & TRANSLATE_OBJECT(filthDensity, "filthDensity"); 
		ar & TRANSLATE_OBJECT(aiDelay, "�������� ��"); 
		ar & TRANSLATE_OBJECT(triggerDelayFactor, "����������� �������� ��������"); 
	}
};

extern DifficultyPrm difficultyPrmArray[DIFFICULTY_MAX];

//-----------------------------
//��������� ������������� ������
struct TrucksIntrumentParameter 
{
	float kRadius4DigZL;
	float kHeigh4DigZL;

	float kRadius4PutZL;
	float kHeigh4PutZL;

	float kRadius4DigGarbage;
	float kHeigh4DigGarbage;

	float kRadius4PutGarbage;
	float kHeigh4PutGarbage;

	TrucksIntrumentParameter() {
		kRadius4DigZL = 1.2f;
		kHeigh4DigZL = 1.2f;

		kRadius4PutZL = 1.2f;
		kHeigh4PutZL = 1.2f;

		kRadius4DigGarbage = 1.2f;
		kHeigh4DigGarbage = 1.2f;

		kRadius4PutGarbage = 1.2f;
		kHeigh4PutGarbage = 1.2f;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(kRadius4DigZL, "kRadius4DigZL"); 
		ar & TRANSLATE_OBJECT(kHeigh4DigZL, "kHeigh4DigZL"); 

		ar & TRANSLATE_OBJECT(kRadius4PutZL, "kRadius4PutZL"); 
		ar & TRANSLATE_OBJECT(kHeigh4PutZL, "kHeigh4PutZL"); 

		ar & TRANSLATE_OBJECT(kRadius4DigGarbage, "kRadius4DigGarbage"); 
		ar & TRANSLATE_OBJECT(kHeigh4DigGarbage, "kHeigh4DigGarbage"); 

		ar & TRANSLATE_OBJECT(kRadius4PutGarbage, "kRadius4PutGarbage"); 
		ar & TRANSLATE_OBJECT(kHeigh4PutGarbage, "kHeigh4PutGarbage"); 
	}
};

extern TrucksIntrumentParameter trucksIntrumentParameter;

//-----------------------------
struct SoundEventsPrm
{
	float collectorEmptyThreshold;
	float energyLosingThreshold;
	float energyNotEnoughThreshold;
	float energyNotEnoughDischargeThreshold;

	SoundEventsPrm() {
		collectorEmptyThreshold = 10;
		energyLosingThreshold = 2;
		energyNotEnoughThreshold = 10;
		energyNotEnoughDischargeThreshold = 1.01f;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(collectorEmptyThreshold, "collectorEmptyThreshold"); 
		ar & TRANSLATE_OBJECT(energyLosingThreshold, "energyLosingThreshold"); 
		ar & TRANSLATE_OBJECT(energyNotEnoughThreshold, "energyNotEnoughThreshold"); 
		ar & TRANSLATE_OBJECT(energyNotEnoughDischargeThreshold, "energyNotEnoughDischargeThreshold"); 
	}
};

extern SoundEventsPrm soundEventsPrm;

//-----------------------------
struct BelligerentProperty
{
	EnumWrapper<terBelligerent> belligerent;
	int colorIndex;

	SoundTracks soundTracks;

	BelligerentProperty() {
		belligerent = BELLIGERENT_NONE;
		colorIndex = 0;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(belligerent, "&belligerent"); 
		ar & TRANSLATE_OBJECT(colorIndex, "colorIndex"); 
		ar & TRANSLATE_OBJECT(soundTracks, "soundTracks"); 
	}
};

struct BelligerentPropertyTable
{
	const BelligerentProperty& find(terBelligerent belligerent)
	{
		for(int i = 0; i < data.size(); i++)
			if(data[i].belligerent == belligerent)
				return data[i];
		xassert(0);
		return data[0];
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(data, "data"); 
	}

private:
	vector<BelligerentProperty> data;
};

extern BelligerentPropertyTable belligerentPropertyTable;

struct GlobalAttributes
{
	int baseUnitsMax;
	int changePlayerDelay;
	float sellBuildingEfficiency; // ������������ ��� ������� �������
    float FallTreeTime;

	GlobalAttributes() 
	{
		baseUnitsMax = 250;
		changePlayerDelay = 3000;
		sellBuildingEfficiency = 0.5; 
		FallTreeTime = 3;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(debuScales, "���������� ���������������"); 
		ar & TRANSLATE_OBJECT(belligerentPropertyTable, "��������� ���"); 
		ar & TRANSLATE_OBJECT(buildingBlockConsumption, "������� �� ������������ �����"); 
		ar & TRANSLATE_OBJECT(fieldPrm, "��������� ����"); 
		ar & TRANSLATE_OBJECT(difficultyPrmArray, "������ ���������"); 
		ar & TRANSLATE_OBJECT(soundEventsPrm, "�������� �������"); 
		ar & TRANSLATE_OBJECT(playerSound, "����� ������"); 
		ar & TRANSLATE_OBJECT(baseUnitsMax, "������������ ���������� ������� ������");
		ar & TRANSLATE_OBJECT(changePlayerDelay, "�������� ������ ��������������� ������");
		ar & TRANSLATE_OBJECT(sellBuildingEfficiency, "������������ ��� ������� �������");
		ar & TRANSLATE_OBJECT(trucksIntrumentParameter, "��������� ������������ ������"); 
		ar & TRANSLATE_OBJECT(FallTreeTime, "FallTreeTime");
	}
};

extern SingletonPrm<GlobalAttributes> globalAttr;

////////////////////////////////////////////////////

// ��������� ��������
struct terDebrisData
{
	EnumWrapper<terUnitAttributeID> debrisID;

	int count;
	// ���� ����� -1, �� �������� ����� count + round(pow(radius / 4,1.5f));
	int countRnd;

	float speed;

	terDebrisData() {
		debrisID = UNIT_ATTRIBUTE_NONE;
        count = 10;
		countRnd = 10;
		speed = 150.0f;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(debrisID, "debrisID"); 
		ar & TRANSLATE_OBJECT(count, "count"); 
		ar & TRANSLATE_OBJECT(countRnd, "countRnd"); 
		ar & TRANSLATE_OBJECT(speed, "speed"); 
	}
};

// ����������
struct terUnitEffectData
{
	// ������������� �������
	EnumWrapper<terEffectID> effectID;
	// ��� ��� ������ � ����������
	PrmString effectName;
	// == 1 ���� ������� ���� ���������� ���������� �����
	bool needOrientation;

	// (��� ����/���� �� ������� ������)
	// ������� �����������, ��� ������� ���������� ������
	// [0, 1]
	float startupDamage;

	terUnitEffectData() {
		effectID = EFFECT_ID_EXPLOSION;
        needOrientation = 1;
		startupDamage = 0;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(effectID, "&effectID"); 
		ar & TRANSLATE_OBJECT(effectName, "&effectName"); 
		ar & TRANSLATE_OBJECT(needOrientation, "needOrientation"); 
		ar & TRANSLATE_OBJECT(startupDamage, "startupDamage"); 
	}
};

// ��������� ������������ �����/������ - ��� �����, ����� �������� � �.�.
struct terUnitEffects
{
	PrmString libraryFileName;
	vector<terUnitEffectData> effects;

	// �������
	vector<terDebrisData> debrisData;

	// ����� ������������� ����� �� �����
	int corpseLifeTime;

	// ��� �����
	// UNIT_ATTRIBUTE_CORPSE_DYNAMIC - ��� ������
	// UNIT_ATTRIBUTE_FALL_STRUCTURE - ��� ������
	// UNIT_ATTRIBUTE_NONE - ���� ����� ����
	EnumWrapper<terUnitAttributeID> corpseID;

	// ������ ������� �� ������
	// ���� ���� ������� ����
	float craterRadius;
	// ��� �������
	EnumWrapper<terUnitAttributeID> craterID;

	// �������� ����� ���������� ������� �� ������
	int craterDelay;

	// ������, ������������ ��� �������� �����, ��� ��������� ��������
	EnumWrapper<terUnitAttributeID> movementCraterID;
	// ����� ����� ���������� ������ ������ ��� ��������
	// ���� ��������, ����� ���������� ���������� ��� ��������
	float movementCraterDelta;

	terUnitEffects() 
	{
		corpseLifeTime = 0;
		corpseID = UNIT_ATTRIBUTE_NONE;
		craterRadius = 0;
		craterID = UNIT_ATTRIBUTE_DESTRUCTION_CRATER;
		craterDelay = 0;
		movementCraterID = UNIT_ATTRIBUTE_NONE;
		movementCraterDelta = 1;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(libraryFileName, "&libraryFileName"); 
		ar & TRANSLATE_OBJECT(effects, "effects"); 
		ar & TRANSLATE_OBJECT(debrisData, "debrisData"); 
		ar & TRANSLATE_OBJECT(corpseLifeTime, "corpseLifeTime"); 
		ar & TRANSLATE_OBJECT(corpseID, "corpseID"); 
		ar & TRANSLATE_OBJECT(craterRadius, "craterRadius"); 
		ar & TRANSLATE_OBJECT(craterID, "craterID"); 
		ar & TRANSLATE_OBJECT(craterDelay, "craterDelay"); 
		ar & TRANSLATE_OBJECT(movementCraterID, "movementCraterID"); 
		ar & TRANSLATE_OBJECT(movementCraterDelta, "movementCraterDelta"); 
	}
};

// ��������� ���������� ���������� ������
struct terWeaponControllerSetup
{
	// ����� ����� ��� ���������� ������� ����� � ������
	PrmString objectName[2];
	// ����� ���������� �������� ��� ���������� ������ � �������
	PrmString logicObjectName[2];

	// ����� ���������� �������� ��� ������������ (����� ����� � ����� ������)
	PrmString targetingObjectName;

	// �������� ������������, � ����� PI
	float targetingPrecision[2];

	// �������� ��������� �� ����������� � �� ���������
	// � ����� PI
	float turnSpeed[2];

	// ����������� �������� ������ �� �����������
	// � ����� PI
	float psiLimit[2];

	// ����������� �������� ������ �� ���������
	// � ����� PI
	float thetaLimit[2];

	// ����������� ������ �� ���������, � ����� PI
	float defaultAngles[2];

	terWeaponControllerSetup()
	{
		targetingObjectName = 0;

		targetingPrecision[0] = targetingPrecision[1] = 0.001f;

		turnSpeed[0] = turnSpeed[1] = 0.2f;

		psiLimit[0] = 0;
		psiLimit[1] = 2;

		thetaLimit[0] = -0.5;
		thetaLimit[1] = 0.5;

		defaultAngles[0] = defaultAngles[1] = 0;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(objectName, "objectName"); 
		ar & TRANSLATE_OBJECT(logicObjectName, "logicObjectName"); 
		ar & TRANSLATE_OBJECT(targetingObjectName, "targetingObjectName"); 
		ar & TRANSLATE_OBJECT(targetingPrecision, "targetingPrecision"); 
		ar & TRANSLATE_OBJECT(turnSpeed, "turnSpeed"); 
		ar & TRANSLATE_OBJECT(psiLimit, "psiLimit"); 
		ar & TRANSLATE_OBJECT(thetaLimit, "thetaLimit"); 
		ar & TRANSLATE_OBJECT(defaultAngles, "defaultAngles"); 
	}
};

// ������
struct terWeaponSetup
{
	// ��� ������
	EnumWrapper<WeaponType> weaponType;

	// �����, ���������� �������� enum WeaponFlags
	BitVector<WeaponFlags> flags;

	// ID �������
	EnumWrapper<terUnitAttributeID> missileID;
	// ������������ ���������� ������������ ������� �� ���� ��������
	// 0 ���� ��� �����������
	int missileLimit;

	// ����� ����� ���������, �� ��������� ��������
	// ������ ��������������� � ����������� ��-���������
	//
	// � �������������
	int aimResetDelay;

	// �����, � ������� �������� ����� �������� ������ ���������� ���
	int fireDisableTime;

	// �������� ����� ����������
	// ��� ������, ������� �����-�� ����� ����������,
	// ����� ��������� ����� ��������
	int fireDelay;

	// �������� ����������� ����� ��������,
	// ������������ ��� ��������
	int missileLoadDelay;

	// prm ��� ������� ���������
	RigidBodyPrmLibrary::Reference turnSuggestPrm;

	// ���������� ����������
	terWeaponControllerSetup controllerSetup;

	float dischargeSpeed;

	// ��������� ��������, min/max
	float fireRadius[2];

	// ������ ��������� ����
	float sightRadius;

	// ������, � ������ ������� �������
	float accuracyRadius;

	// ���������� ������, ���������� ������� �� ������
	int weaponIndex;

	// ��� ������ - ������� ����
	float laserWidth;

	ConsumptionData reloadConsumption;

	float thetaForced; // ������������ theta, ������������ suggest 

	// 0 - �������������� �������� ���������� �� ����
	// 1 - �������� ���������� �� ���� �� ������
	int squadMode;

	// true ���� �� ����� ��������� ���������� �� ����������
	bool excludeHolograms;

	terWeaponSetup()
	{
		weaponType = WEAPON_NONE;
		missileID = UNIT_ATTRIBUTE_NONE;
	
		missileLimit = 0;
		aimResetDelay = 1500;
		fireDisableTime = 0;
		fireDelay = 1000;
		missileLoadDelay = 300;

		dischargeSpeed = 0.3f;

		fireRadius[0] = 0;
		fireRadius[1] = 100;

		sightRadius = 100;

		accuracyRadius = 0;

		weaponIndex = 0;

		laserWidth = 2;

		excludeHolograms = true;

		reloadConsumption.name = "reloadConsumption";

		thetaForced = -360; 

		squadMode = 1;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(weaponType, "weaponType"); 
		ar & TRANSLATE_OBJECT(flags, "flags"); 
		ar & TRANSLATE_OBJECT(missileID, "missileID"); 
		ar & TRANSLATE_OBJECT(missileLimit, "missileLimit"); 
		ar & TRANSLATE_OBJECT(aimResetDelay, "aimResetDelay"); 
		ar & TRANSLATE_OBJECT(fireDisableTime, "fireDisableTime"); 
		ar & TRANSLATE_OBJECT(fireDelay, "fireDelay"); 
		ar & TRANSLATE_OBJECT(missileLoadDelay, "missileLoadDelay"); 
		ar & TRANSLATE_OBJECT(turnSuggestPrm, "turnSuggestPrm"); 
		ar & TRANSLATE_OBJECT(controllerSetup, "controllerSetup"); 
		ar & TRANSLATE_OBJECT(dischargeSpeed, "dischargeSpeed"); 
		ar & TRANSLATE_OBJECT(fireRadius, "fireRadius"); 
		ar & TRANSLATE_OBJECT(sightRadius, "sightRadius"); 
		ar & TRANSLATE_OBJECT(accuracyRadius, "accuracyRadius"); 
		ar & TRANSLATE_OBJECT(weaponIndex, "weaponIndex"); 
		ar & TRANSLATE_OBJECT(laserWidth, "laserWidth"); 
		ar & TRANSLATE_OBJECT(reloadConsumption, "reloadConsumption"); 
		ar & TRANSLATE_OBJECT(thetaForced, "thetaForced"); 
		ar & TRANSLATE_OBJECT(squadMode, "squadMode"); 
		ar & TRANSLATE_OBJECT(excludeHolograms, "excludeHolograms"); 
	}

	bool enabled() const { return weaponType != WEAPON_NONE; }
	bool checkFlag(int fl) const { if(flags & fl) return true; else return false; }

	bool hasFireController() const { return checkFlag(WEAPON_HAS_FIRE_CONTROLLER); }
	bool rootFireAnimation() const { return checkFlag(WEAPON_ROOT_FIRE_ANIMATION); }
	bool cycleFireAnimation() const { return checkFlag(WEAPON_CYCLE_FIRE_ANIMATION); }
	bool needCharge() const { return checkFlag(WEAPON_NEED_CHARGE); }
	bool cleanTargetAfterFire() const { return checkFlag(WEAPON_CLEAN_TARGET_AFTER_FIRE); }
};


//#define _DAMAGE_MODEL_DEBUG_

class DamageMolecula
{
public:
	DamageMolecula();
	DamageMolecula(int s,int o,int t);

	// �������� ������
	const DamageMolecula& operator=(const struct SaveDamageMolecula& data);

	bool isAlive() const { return isAlive_; }

	/// ��������, ��� �������� ���������, �.�. � ��� �� ����� ��������� �����-���� �� ���������
	bool isStable() const;

	/// ���������� ��������� ���� type ��� ����� ������� (���� ��� - DAMAGE_ELEMENT_TYPE_MAX ���������� ����� ����������).
	int elementCount(int element_filter = DAMAGE_FILTER_ALL) const;
	/// ���������� ����� ��������� ���� type.
	int aliveElementCount(int element_filter = DAMAGE_FILTER_ALL) const;
	/// ���������� ������� ��������� ���� type.
	int deadElementCount(int element_filter = DAMAGE_FILTER_ALL) const;

	/// ���������� ������ ��������� ���� type ��� ����� ������� (���� ��� - DAMAGE_ELEMENT_TYPE_MAX ���������� ����� ���������� ������).
	int atomCount(int element_filter = DAMAGE_FILTER_ALL) const { return elementCount(element_filter) * 4; }

	/// ���������� ��������� ���� i, ��� ����� �������.
	int& operator[](int i){ return elements_[i]; }
	int operator[](int i) const { return elements_[i]; }

	/// �������������� ����� � ��������.
	DamageMolecula& operator += (const DamageMolecula& rhs) { for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){ elements_[i] += rhs.elements_[i]; elementsDead_[i] += rhs.elementsDead_[i]; } return *this; }
	DamageMolecula& operator -= (const DamageMolecula& rhs) { for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){ elements_[i] -= rhs.elements_[i]; elementsDead_[i] -= rhs.elementsDead_[i]; } return *this; }
	DamageMolecula operator + (const DamageMolecula& rhs) const { return DamageMolecula(*this) += rhs; }
	DamageMolecula operator - (const DamageMolecula& rhs) const { return DamageMolecula(*this) -= rhs; }

	DamageMolecula& operator *= (const DamageMolecula& rhs) { for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){ elements_[i] *= rhs.elements_[i]; elementsDead_[i] *= rhs.elementsDead_[i]; } return *this; }
	DamageMolecula& operator *= (int value) { for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){ elements_[i] *= value; elementsDead_[i] *= value; } return *this; }

	DamageMolecula operator * (int value) { return DamageMolecula(*this) *= value; }

	/// ���������� ������� - ������� ����� ���������� ������, �������������� �� ������ ��� ����� ���������.
	int operator / (const DamageMolecula& rhs) const { int n, nmin = 1000; for(int i = 0; i < 3; i++) if(rhs[i] && nmin > (n = (elements_[i] - elementsDead_[i])/rhs[i])) nmin = n; xassert(nmin != 1000); return nmin; }

	/// ���������� ������ ������ � ��������� ��������.
	int softAtomCount(int element_filter = DAMAGE_FILTER_ALL) const;
	/// ���������� ������� ������ � ��������� ��������.
	int hardAtomCount(int element_filter = DAMAGE_FILTER_ALL) const;

	/// ���������� true, ���� � �������� ���� ���� �� ���� ������� �������.
	bool needRepair() const { for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++) if(elementsDead_[i]) return true; return false; }
	/// �������������� ������� ���������.
	/**
	���������� ���������� ������� ��������������� ���������.
	*/
	int repair(int element_count = 1);
	/// �������������� ������� ��������� ������������� ����.
	void repair(DamageElementType element_type,int element_count);

	/// ����� �� ��������.
	/**
	attack_width - ������ ����� (���������� ��������� ������)
	attack_power - �������� ����� (���������� ��������� �� ������)
	shield - ���������� ������ ������, ������������� � ������� �� ����� �����
	attack_filter - ������ �� ��������, �� ������� ������� ����� ��� �����
	damage_filter - ������ �� ��������, ������� ����� ������� ��� �������� ���������

	���������� ���������� ������ � ���������� ����� ���������
	*/
	int hit(int attack_width,int attack_power,int attack_filter = DAMAGE_FILTER_ALL,int damage_filter = DAMAGE_FILTER_ALL,int shield = 0);

	int killElements(int element_count = 1,int element_filter = DAMAGE_FILTER_ALL);
	void kill()
	{
		zeroize(); isAlive_ = false;
	}

	/// ��������� ���������� ����� ���������
	void zeroize(){ for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++) elementsDead_[i] = elements_[i]; isAlive_ = true; }

	float phase() const { if(elementCount()) return float(aliveElementCount())/float(elementCount()); return 0.0f; }

	int elementsDead(DamageElementType type) const { return elementsDead_[type]; }
	void addElementsDead(DamageElementType type,int count = 1){ elementsDead_[type] += count; if(elementsDead_[type] < 0) elementsDead_[type] = 0; }

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_NAME(elements_, "elements", "��������"); 
	}

	//friend ostream& operator << (ostream& os, const DamageMolecula& m) { os << m.elements_[DAMAGE_ELEMENT_SOLDIER] << " \t"  << m.elements_[DAMAGE_ELEMENT_OFFICER] << " \t" << m.elements_[DAMAGE_ELEMENT_TECHNIC]; return os; }

private:
	int elements_[DAMAGE_ELEMENT_TYPE_MAX];
	int elementsDead_[DAMAGE_ELEMENT_TYPE_MAX];

	static int softAtomCount_[DAMAGE_ELEMENT_TYPE_MAX];

	bool isAlive_;

#ifdef _DAMAGE_MODEL_DEBUG_
	static LogStream log_;
#endif
};

struct IntfBalanceData {
	IntfBalanceData() : power(0), width(0), radius(0), armor(0) {
	}
	int power;
	int width;
	int radius;
	float armor;
};

struct ModelData
{
	PrmString modelName;
	PrmString logicName;
	float boundScale;
	float boundRadius;

	ModelData() {
		modelName = 0;
		logicName = 0;
		boundScale = 1;
		boundRadius = 0;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(modelName, "&modelName");
		ar & TRANSLATE_OBJECT(logicName, "logicName");
		ar & TRANSLATE_OBJECT(boundScale, "���������������");
		ar & TRANSLATE_OBJECT(boundRadius, "������ ������� (0 - ������������ ���������������)");
	}
};

class FileTime : _FILETIME
{
public:
	FileTime() {
		dwLowDateTime = dwHighDateTime = 0;
	}

	FileTime(const char* fname);

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_NAME(dwLowDateTime, "LowDateTime", "LowDateTime");
		ar & TRANSLATE_NAME(dwHighDateTime, "HighDateTime", "HighDateTime");
	}

	bool operator==(const FileTime& rhs) const {
		return dwLowDateTime == rhs.dwLowDateTime && dwHighDateTime == rhs.dwHighDateTime;
	}
};

struct GeometryAttribute
{
	Vect2f BasementMin;
	Vect2f BasementMax;
	Vect2fVect BasementPoints;
	float BasementInscribedRadius;
	
	float modelScale;
	sBox6f logicObjectBound;
	sBox6f logicObjectBoundOriginal;
	float boundRadiusOriginal;
	float boundRadius;

	FileTime modelTime;
	FileTime logicTime;

	GeometryAttribute();

	void initGeometryAttribute(const ModelData& modelData, const AttributeBase& attribute);

	template<class Archive>
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(BasementMin);
		ar & WRAP_OBJECT(BasementMax);
		ar & WRAP_OBJECT(BasementPoints);
		ar & WRAP_OBJECT(BasementInscribedRadius);

		ar & WRAP_OBJECT(modelScale);
		ar & WRAP_NAME(logicObjectBound.min, "logicObjectBoundMin");
		ar & WRAP_NAME(logicObjectBound.max, "logicObjectBoundMax");
		ar & WRAP_OBJECT(boundRadius);
		ar & WRAP_NAME(logicObjectBoundOriginal.min, "logicObjectBoundOriginalMin");
		ar & WRAP_NAME(logicObjectBoundOriginal.max, "logicObjectBoundOriginalMax");
		ar & WRAP_OBJECT(boundRadiusOriginal);

		ar & WRAP_OBJECT(modelTime);
		ar & WRAP_OBJECT(logicTime);
	}
};

class AttributeBase : public GeometryAttribute, public ShareHandleBase
{
public:
	EnumWrapper<terUnitAttributeID> ID;
	EnumWrapper<terUnitClassID> ClassID;
	EnumWrapper<terInterpolationID> InterpolationType;
	EnumWrapper<terBelligerent> belligerent; // �������� ������������� �� ������� �������, ������ _�����_

	ModelData modelData;
	vector<ModelData> additionalModelsData;
	vector<PrmString> ConnectionPointNames;
 
	DamageMolecula damageMolecula; 
	UnitDamage unitDamage;
	EnvironmentalDamage environmentalDamage;

	terUnitEffects effectsData;
	terWeaponSetup weaponSetup;
	ToolzerSetup toolzerSetup;

	bool SelectAble;
	bool saveAsCommonObject; // ���������� ������ (������ ������� ��� ��������, ������� ��� ������� � ������� ����� ��������)
	
	float GroundPlaneSpeed;
	float GroundAngleSpeed;
	
	bool MilitaryUnit;
	BitVector<terUnitClassType> UnitClass;
	BitVector<terUnitClassType> AttackClass;
	bool enemyWorld; // ������, ������������� ����, ����� �����������. �������.

	PrmString SoundName;
	bool SoundCycled;

	bool InstallBound;
	bool lockInitialPosition;

	bool showPath;

	int ExcludeCollision;
	BitVector<CollisionGroupID> CollisionGroup;

	float MakeEnergy; // � �������
	float energyCapacity;

	float ConnectionRadius;
	float ZeroLayerRadius;
	float FieldRadius;

	int LifeTime;

	// �����������, �� ������� ���������� ���������� ���������, 
	// ���������� ������������ ������ ��� ��������� ������
	float constructionSpeedCoeff;

	float SelectionDistance;
	float SelectionSize;

	int MakingChainNum;
	PrmString UpgradeChainName;
	PrmString PrevChainName;
	bool IgnoreTargetTrace;

	bool ShowCircles;

	bool enablePathFind;
	bool enableFieldPathFind;
	bool considerInFieldPathFind;

	float kill_priority;

	bool isUpgrade;
	float iconDistanceFactor;

	float forwardVelocity;
	float flyingHeight;
	
	RigidBodyPrmLibrary::Reference rigidBodyPrm; // ��������� ������
	UnitInterfacePrm interfacePrm;
	InterfaceTV interfaceTV;
    vector<AnimationData> animationDataTable;
	SoundSetup soundSetup;
                               
	//---------------------------------------
	const EffectLibrary* EffectLib;

	AttributeBase();
	virtual ~AttributeBase(){}
	void init();

	EffectKey* getEffect(terEffectID effect_id) const;
	EffectKey* getEffect(const char* effect_name) const;
	const terUnitEffectData* getEffectData(terEffectID effect_id) const;

	int debrisNumber() const { return effectsData.debrisData.size(); }
	const terDebrisData& debrisData(int idx = 0) const { return effectsData.debrisData[idx]; }

	bool hasCorpse() const { return (effectsData.corpseID != UNIT_ATTRIBUTE_NONE); }
	terUnitAttributeID corpseID() const { return effectsData.corpseID; }
	int corpseLifeTime() const { return effectsData.corpseLifeTime;	}

	float craterRadius() const { return effectsData.craterRadius; }
	int craterDelay() const { return effectsData.craterDelay; }
	terUnitAttributeID craterID() const { return effectsData.craterID; }

	float fireRadiusMin() const { return weaponSetup.fireRadius[0]; }
	float fireRadius() const { return weaponSetup.fireRadius[1]; }
	float sightRadius() const { return weaponSetup.sightRadius; }

	static void setBuildCost(float energyPerElement) { energyPerElement_ = energyPerElement; }
	float buildEnergy() const { return energyPerElement_*damageMolecula.elementCount()/constructionSpeedCoeff; }

	bool isBuilding() const { return ::isBuilding(ID); }
	bool isLegionary() const { return ::isLegionary(ID); }
	bool enabledByBelligerent(terBelligerent belligerent) const;

	bool hasAnimationSetup() const { return !animationDataTable.empty(); }
	bool hasSoundSetup() const { return soundSetup.enabled(); }

	int estimatedDamage() const { return unitDamage.estimatedDamage(); }

	bool isDefenciveAttackEnabled() const { if(weaponSetup.checkFlag(WEAPON_DISABLE_DEFENCIVE_ATTACK)) return false; else return true; }
	bool checkWeaponFlag(int flag) const { return weaponSetup.checkFlag(flag); }

	const char* internalName() const;
	const char* interfaceName() const { return InterfaceName.c_str(); }
	const char* interfaceNameTag() const { return InterfaceNameTag; }
	void setInterfaceName(const char* name) { InterfaceName = name; }
	void setInterfaceNameTag(const char* name) { InterfaceNameTag = name; }

	void initIntfBalanceData(const AttributeBase* missile);
	IntfBalanceData intfBalanceData;

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(ID, "ID");
		ar & TRANSLATE_OBJECT(belligerent, "belligerent");
		ar & TRANSLATE_OBJECT(ClassID, "ClassID");
		ar & TRANSLATE_OBJECT(InterpolationType, "InterpolationType");

		ar & TRANSLATE_OBJECT(InterfaceNameTag, "InterfaceName");
		ar & TRANSLATE_OBJECT(modelData, "������");
		ar & TRANSLATE_OBJECT(additionalModelsData, "�������������� ������");
		ar & TRANSLATE_OBJECT(animationDataTable, "�������� ��������");
		ar & TRANSLATE_OBJECT(ConnectionPointNames, "����� dummy");

		if(ar.openBlock("interaction", "��������������")){
			ar & TRANSLATE_OBJECT(UnitClass, "UnitClass");
			ar & TRANSLATE_OBJECT(AttackClass, "AttackClass");
			ar & TRANSLATE_OBJECT(MilitaryUnit, "MilitaryUnit");
			ar & TRANSLATE_OBJECT(enemyWorld, "enemyWorld (������, ������������� ����, ����� �����������. �������.)");
			ar & TRANSLATE_OBJECT(ExcludeCollision, "ExcludeCollision");
			ar & TRANSLATE_OBJECT(CollisionGroup, "CollisionGroup");
			ar & TRANSLATE_OBJECT(kill_priority, "kill_priority");
			ar.closeBlock();
		}

		if(ar.openBlock("energyModel", "�������������� ������")){
			ar & TRANSLATE_OBJECT(MakeEnergy, "MakeEnergy");
			ar & TRANSLATE_OBJECT(energyCapacity, "energyCapacity");
		
			ar & TRANSLATE_OBJECT(ConnectionRadius, "ConnectionRadius");
			ar & TRANSLATE_OBJECT(ZeroLayerRadius, "ZeroLayerRadius");
			ar & TRANSLATE_OBJECT(FieldRadius, "FieldRadius");
			ar.closeBlock();
		}

		ar & TRANSLATE_OBJECT(damageMolecula, "damageMolecula");
		ar & TRANSLATE_OBJECT(unitDamage, "��������� �����������");
		ar & TRANSLATE_OBJECT(environmentalDamage, "����������� �� ���������� �����");

		ar & TRANSLATE_OBJECT(weaponSetup, "������");
		ar & TRANSLATE_OBJECT(effectsData, "�������");
		ar & TRANSLATE_OBJECT(toolzerSetup, "�������");
		ar & TRANSLATE_OBJECT(soundSetup, "�����");
		ar & TRANSLATE_OBJECT(interfacePrm, "��������� ����������");
		ar & TRANSLATE_OBJECT(interfaceTV, "�����������");
   
		if(ar.openBlock("Physics", "������")){
			ar & TRANSLATE_OBJECT(rigidBodyPrm, "���������");
			ar & TRANSLATE_OBJECT(forwardVelocity, "�������� ��������");
			ar & TRANSLATE_OBJECT(flyingHeight, "������ ������");
			ar.closeBlock();
		}

		if(ar.openBlock("garbage", "������")){
			ar & TRANSLATE_OBJECT(LifeTime, "LifeTime");
			ar & TRANSLATE_OBJECT(SoundName, "SoundName");
			ar & TRANSLATE_OBJECT(SoundCycled, "SoundCycled");
		
			ar & TRANSLATE_OBJECT(lockInitialPosition, "lockInitialPosition");

			ar & TRANSLATE_OBJECT(enablePathFind, "enablePathFind");
			ar & TRANSLATE_OBJECT(enableFieldPathFind, "enableFieldPathFind");
			ar & TRANSLATE_OBJECT(considerInFieldPathFind, "considerInFieldPathFind");
		
			ar & TRANSLATE_OBJECT(MakingChainNum, "MakingChainNum");
			ar & TRANSLATE_OBJECT(UpgradeChainName, "UpgradeChainName");
			ar & TRANSLATE_OBJECT(PrevChainName, "PrevChainName");

			ar & TRANSLATE_OBJECT(SelectionDistance, "SelectionDistance");
			ar & TRANSLATE_OBJECT(SelectionSize, "SelectionSize");
			ar & TRANSLATE_OBJECT(showPath, "showPath");
			ar & TRANSLATE_OBJECT(ShowCircles, "ShowCircles");

			ar & TRANSLATE_OBJECT(saveAsCommonObject, "saveAsCommonObject (���������� ������ (������ ������� ��� ��������, ������� ��� ������� � ������� ����� ��������)");
	
			ar & TRANSLATE_OBJECT(GroundPlaneSpeed, "GroundPlaneSpeed");
			ar & TRANSLATE_OBJECT(GroundAngleSpeed, "GroundAngleSpeed");
			ar.closeBlock();
		}

		GeometryAttribute::serialize(ar);

		if(!ar.isOutput())
			init();
	}

private:
	PrmString InterfaceNameTag;
	string InterfaceName;

	static float energyPerElement_;
};

class AttributeIDBelligerent
{
public:
	AttributeIDBelligerent(terUnitAttributeID attributeID = UNIT_ATTRIBUTE_NONE, terBelligerent belligerent = BELLIGERENT_NONE) :
		attributeID_(attributeID), belligerent_(belligerent)
		{}

	terUnitAttributeID attributeID() const {
		return attributeID_;
	}

	terBelligerent belligerent() const {
		return belligerent_;
	}

	bool operator<(const AttributeIDBelligerent& rhs) const {
		return attributeID_ != rhs.attributeID_ ? attributeID_ < rhs.attributeID_ : 
			belligerent_ < rhs.belligerent_;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_NAME(attributeID_, "attributeID", "&attributeID");
		ar & TRANSLATE_NAME(belligerent_, "belligerent", "&belligerent");
	}

private:
	EnumWrapper<terUnitAttributeID> attributeID_;
	EnumWrapper<terBelligerent> belligerent_;
};

inline const string key2String(const AttributeIDBelligerent& data) {
	return string(getEnumNameAlt(data.attributeID())) + ", " + getEnumNameAlt(data.belligerent());
}

inline void setKey(AttributeIDBelligerent& data, const char* str) {
	string aName = str;
	int pos = aName.find(",");
	if(pos == string::npos)
		return;
	string bName(&aName[pos], aName.size() - pos - 2);
	aName.erase(pos, aName.size());
	data = AttributeIDBelligerent(
		getEnumDescriptor(UNIT_ATTRIBUTE_NONE).keyByNameAlt(aName.c_str()),
		getEnumDescriptor(BELLIGERENT_NONE).keyByNameAlt(bName.c_str()));
}

typedef TypeLibrary<AttributeIDBelligerent, AttributeBase> AttributeLibrary;
extern SingletonPrm<AttributeLibrary> attributeLibrary;
void initAttributes();

class cObjectNodeRoot* createObject(const char* name, terBelligerent belligerent);

#endif
