#ifndef __UNIT_ATTRIBUTE_H__
#define __UNIT_ATTRIBUTE_H__

#include "..\Util\Serialization.h"
#include "..\Util\TypeLibrary.h"

#ifndef _UNIT_ATTRIBUTE_INL_
#define _UNIT_ATTRIBUTE_INL_
#include "..\Scripts\UnitAttribute.inl"
#include "..\Scripts\SquadShellEnums.inl"
#endif // _UNIT_ATTRIBUTE_INL_

#ifndef __TOOLZER_ENUMS_INL__
#define __TOOLZER_ENUMS_INL__
#include "..\Scripts\ToolzerEnums.inl"
#endif // __TOOLZER_ENUMS_INL__

#include "..\Physics\RigidBodyPrm.h"
#include "..\Util\DebugUtil.h"
#include "..\UserInterface\SoundTrack.h"

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

	// = 1 если звук на мире
	bool is3D;

	// = 1 если голосовое сообщение, вызывается только для активного игрока
	bool isVoice;

	// = 1 если событие должно звучать только для активного игрока
	bool activePlayer;

	// время в миллисекундах, в течении которого после 
	// старта звука нельзя запустить его ещё раз
	int pauseTime;

	// задержка перед проигрыванием звука в миллисекундах
	int startDelay;

	// максимальное время, которое событие будет стоять в очереди на проигрывание (для голосов)
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
// Параметры анимационной цепочки
struct AnimationChain
{
	EnumWrapper<ChainID> chainID;
	PrmString chainName;
	float phase;   // 0..1
	int period; // mS
	EnumWrapper<ChainNodeID> begin;
	EnumWrapper<ChainNodeID> end;
	int periodDeviation; // !0: period += rnd(perionDeviation), phase = rnd(1)
	int enableReverse; // Разрешить реверс между смежными цепочками
	int counter; // for path finding
	EnumWrapper<SoundEventID> soundID;

	AnimationChain() {
		chainID = CHAIN_NONE;
		phase = 0;   // 0..1
		period = 2000; // mS
		begin = CHAIN_NODE_NONE;
		end = CHAIN_NODE_NONE;
		periodDeviation = 0; // !0: period += rnd(perionDeviation), phase = rnd(1)
		enableReverse = 0; // Разрешить реверс между смежными цепочками
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

// Набор анимационных цепочек для определенного узла
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

	const AnimationChain* initial() const { return !chains.empty() ? &chains[0] : 0; }	// первая цепочка включается при создании объекта
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
// параметры воздействия тулзера
struct ToolzerActionData
{
	// тип воздействия
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

// параметры воздействий тулзера в определенной фазе
struct ToolzerStepData
{
	EnumWrapper<ToolzerPhaseID> phaseID;

	// время работы шага в логических квантах
	int duration;

	// следующая фаза
	// TOOLZER_PHASE_DEFAULT - переходить к следующей по порядку
	// TOOLZER_PHASE_NONE - остановить работу тулзера
	EnumWrapper<ToolzerPhaseID> nextPhaseID;

	// минимальное изменение координат тулзера для наложнения воздействия
	//
	// сделано, чтобы избежать наваливания куч до небес когда тулзер стоит
	// или движется медленно
	float dr;

	// повреждения зданиям, режим и радиус
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

// параметры тулзера
struct ToolzerSetup
{
	vector<ToolzerStepData> steps;

	// коэфф. масштабирования тулзера
	float scale;

	// = 1 если тулзер рушит собственный зеро-слой
	bool destroyOwnZeroLayer;

	// = 1 если работает на хаосе
	bool workOnChaos;

	// радиус для пересчёта масштаба
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
// параметры повреждений, наносимых юниту
struct DamageData
{
	// ширина атаки
	int width;
	// мощность атаки
	int power;

	// фильтр на элементы, из которых берутся атомы для атаки
	// значения: DAMAGE_FILTER_ALL или комбинация из других значений DamageElementFilter
	BitVector<DamageElementFilter> attackFilter;
	// фильтр на элементы, которые будут гибнуть при успешном попадании
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
		ar & TRANSLATE_OBJECT(width, "ширина атаки");
		ar & TRANSLATE_OBJECT(power, "мощность атаки");
		ar & TRANSLATE_OBJECT(attackFilter, "фильтр на элементы, из которых берутся атомы для атаки");
		ar & TRANSLATE_OBJECT(damageFilter, "фильтр на элементы, которые будут гибнуть при успешном попадании");
	}
};

/////////////////////////////////////////////////////////////////////////////////
// параметры повреждений, наносимых миром (хаос, поврежденный зерослой под зданиями)
struct EnvironmentalDamage
{
	// тип повреждений
	EnumWrapper<EnvironmentalDamageType> damageType;
	// параметры повреждений, [0] - минимум, [1] - максимум
	// если width == -1, то туда
	// подставляется общее количество атомов юнита
	DamageData damageData[2];

	// периодичность нанесения повреждений, все в миллисекундах
	//
	// повреждения наносятся через время t = period + periodDelta * (1 - damage_ratio)
	// damage_ratio - величина, определяющая интенсивность повреждений,
	// отношение площади наносящей повреждение поверхности (хаос или нарушенный зерослой)
	// к общей площади, занимаемой юнитом
	int period;
	int periodDelta;

	// минимальное значение damage_ratio,
	// при котором юниту начинают наноситься повреждения
	float damageRatioMin;

	EnvironmentalDamage() {
		damageType = ENV_DAMAGE_NONE;
		period = 0;
		periodDelta = 0;
		damageRatioMin = 0.01f;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(damageType, "тип повреждений");
		ar & TRANSLATE_OBJECT(damageData, "параметры повреждений, [0] - минимум, [1] - максимум");
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



// параметры наносимых юнитом повреждений
struct UnitDamage
{
	// повреждения, наносимые непосредственно в точке попадания
	DamageData mainDamage;

	int splashDamageRadius;
	// повреждения, наносимые в радиусе splashDamageRadius от точки попадания
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

	// оценка количества элементов, вышибаемых с одного раза
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
	float monksVelocity; // Скорость при нормальных условиях
	float monksAccelerationFactor; 
	float monksPositionFactor;
	int monksWetPlace; // Радиус выравниваемой поверхности при смерти молельщика
	float chargeLevelToStartMin; // Ядро с таким уровнем зарядки включится при тотальном режиме
	ConsumptionData fieldConsumption;
	float checkFieldRadiusMin;
	DamageData fieldDamage;
	int impulseDuration; 
	DamageData impulseFieldDamage;

	FieldPrm() 
	{
		monksPerCore = 20; 
		monksPerCoreMin = 3;
		monksVelocity = 5; // Скорость при нормальных условиях
		monksAccelerationFactor = 0.2f; 
		monksPositionFactor = 0.1f;
		monksWetPlace = 8; // Радиус выравниваемой поверхности при смерти молельщика
		chargeLevelToStartMin = 0.1f; // Ядро с таким уровнем зарядки включится при тотальном режиме
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
		ar & TRANSLATE_OBJECT(monksWetPlace, "Радиус выравниваемой поверхности при смерти молельщика"); 
		ar & TRANSLATE_OBJECT(chargeLevelToStartMin, "Ядро с таким уровнем зарядки включится при тотальном режиме"); 
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
	float frame; // фрейм
	float legion; // легион
	float buildins; // здания
	float spheres; // капли
	float other; // остальные

	DebugScales() {
		frame = 1; // фрейм
		legion = 0.5; // легион
		buildins = 0.5; // здания
		spheres = 0.5; // капли
		other = 0.5; // остальные
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(frame, "фрейм"); 
		ar & TRANSLATE_OBJECT(legion, "легион"); 
		ar & TRANSLATE_OBJECT(buildins, "здания"); 
		ar & TRANSLATE_OBJECT(spheres, "капли"); 
		ar & TRANSLATE_OBJECT(other, "остальные"); 
	}
};

extern DebugScales debuScales; // Отладочное масштабирование

//-----------------------------
struct DifficultyPrm
{
	PrmString name;

	//filthDensity - влияет на количество генерируемой скверны и период генерации.
	//mul=1 - все значения без изменения.
	//При уменьшении - уменьшается сначало количество скверны, когда
	//количество скверны уменьшится до 1, начинает увеличиваиться период генерации.
	float filthDensity;

	int aiDelay; // Задержка АИ

	float triggerDelayFactor; // Коэффициент триггера задержка

	DifficultyPrm() {
		filthDensity = 1;
        aiDelay = 0; 
		triggerDelayFactor = 1; 
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(name, "&name"); 
		ar & TRANSLATE_OBJECT(filthDensity, "filthDensity"); 
		ar & TRANSLATE_OBJECT(aiDelay, "Задержка АИ"); 
		ar & TRANSLATE_OBJECT(triggerDelayFactor, "Коэффициент триггера задержка"); 
	}
};

extern DifficultyPrm difficultyPrmArray[DIFFICULTY_MAX];

//-----------------------------
//Параметры настраивающие ковшей
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
	float sellBuildingEfficiency; // Возвращаемая при продаже энергия
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
		ar & TRANSLATE_OBJECT(debuScales, "Отладочное масштабирование"); 
		ar & TRANSLATE_OBJECT(belligerentPropertyTable, "Параметры рас"); 
		ar & TRANSLATE_OBJECT(buildingBlockConsumption, "Затраты на строительные блоки"); 
		ar & TRANSLATE_OBJECT(fieldPrm, "Параметры поля"); 
		ar & TRANSLATE_OBJECT(difficultyPrmArray, "Уровни сложности"); 
		ar & TRANSLATE_OBJECT(soundEventsPrm, "Звуковые события"); 
		ar & TRANSLATE_OBJECT(playerSound, "Звуки игрока"); 
		ar & TRANSLATE_OBJECT(baseUnitsMax, "Максимальное количество базовых юнитов");
		ar & TRANSLATE_OBJECT(changePlayerDelay, "Задержка потери неподключенного здания");
		ar & TRANSLATE_OBJECT(sellBuildingEfficiency, "Возвращаемая при продаже энергия");
		ar & TRANSLATE_OBJECT(trucksIntrumentParameter, "Параметры инструментов ковшей"); 
		ar & TRANSLATE_OBJECT(FallTreeTime, "FallTreeTime");
	}
};

extern SingletonPrm<GlobalAttributes> globalAttr;

////////////////////////////////////////////////////

// параметры осколков
struct terDebrisData
{
	EnumWrapper<terUnitAttributeID> debrisID;

	int count;
	// если равно -1, то осколков будет count + round(pow(radius / 4,1.5f));
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

// спецэффект
struct terUnitEffectData
{
	// идентификатор эффекта
	EnumWrapper<terEffectID> effectID;
	// имя для поиска в библиотеке
	PrmString effectName;
	// == 1 если эффекту надо передавать ориентацию юнита
	bool needOrientation;

	// (для огня/дыма от горящих зданий)
	// уровень повреждений, при котором включается эффект
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

// параметры спецэффектов юнита/здания - имя файла, имена эффектов и т.д.
struct terUnitEffects
{
	PrmString libraryFileName;
	vector<terUnitEffectData> effects;

	// осколки
	vector<terDebrisData> debrisData;

	// время существования трупа от юнита
	int corpseLifeTime;

	// тип трупа
	// UNIT_ATTRIBUTE_CORPSE_DYNAMIC - для юнитов
	// UNIT_ATTRIBUTE_FALL_STRUCTURE - для зданий
	// UNIT_ATTRIBUTE_NONE - если трупа нету
	EnumWrapper<terUnitAttributeID> corpseID;

	// радиус воронки от взрыва
	// ноль если воронки нету
	float craterRadius;
	// тип кратера
	EnumWrapper<terUnitAttributeID> craterID;

	// задержка перед появлением кратера от взрыва
	int craterDelay;

	// кратер, вызывающийся при движении юнита, для подземных снарядов
	EnumWrapper<terUnitAttributeID> movementCraterID;
	// чарез какое расстояние класть кратер при движении
	// если обнулить, будет вызываться непрерывно при движении
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

// параметры управления наведением оружия
struct terWeaponControllerSetup
{
	// имена дамми для управления моделью башни и ствола
	PrmString objectName[2];
	// имена логических объектов для управления башней и стволом
	PrmString logicObjectName[2];

	// имена логических объектов для прицеливания (центр башни и конец ствола)
	PrmString targetingObjectName;

	// точность прицеливания, в долях PI
	float targetingPrecision[2];

	// скорость поворотов по горизонтали и по ветрикали
	// в долях PI
	float turnSpeed[2];

	// ограничения поворота ствола по горизонтали
	// в долях PI
	float psiLimit[2];

	// ограничения поворота ствола по вертикали
	// в долях PI
	float thetaLimit[2];

	// направление ствола по умолчанию, в долях PI
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

// оружие
struct terWeaponSetup
{
	// тип оружия
	EnumWrapper<WeaponType> weaponType;

	// флаги, комбинация значений enum WeaponFlags
	BitVector<WeaponFlags> flags;

	// ID снаряда
	EnumWrapper<terUnitAttributeID> missileID;
	// максимальное количество одновременно живущих на мире снарядов
	// 0 если нет ограничения
	int missileLimit;

	// время после наведения, по истечении которого
	// оружие разворачивается в направлении по-умолчанию
	//
	// в милиисекундах
	int aimResetDelay;

	// время, в течении которого после выстрела нельзя стрельнуть ещё
	int fireDisableTime;

	// задержка между выстрелами
	// для оружия, которое какое-то время заряжается,
	// потом некоторое время работает
	int fireDelay;

	// задержка перезарядки после выстрела,
	// используется для ракетниц
	int missileLoadDelay;

	// prm для расчета наведения
	RigidBodyPrmLibrary::Reference turnSuggestPrm;

	// управление наведением
	terWeaponControllerSetup controllerSetup;

	float dischargeSpeed;

	// дальность стрельбы, min/max
	float fireRadius[2];

	// радиус видимости цели
	float sightRadius;

	// радиус, в котром ложатся снаряды
	float accuracyRadius;

	// внутренний индекс, назначение зависит от оружия
	int weaponIndex;

	// для лазера - толщина луча
	float laserWidth;

	ConsumptionData reloadConsumption;

	float thetaForced; // Игнорировать theta, возвращаемое suggest 

	// 0 - индивидуальная проверка расстояния до цели
	// 1 - проверка расстояния до цели по скваду
	int squadMode;

	// true если не может автоматом наводиться на голограммы
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

	// Загрузка сейвов
	const DamageMolecula& operator=(const struct SaveDamageMolecula& data);

	bool isAlive() const { return isAlive_; }

	/// проверка, что молекула стабильна, т.е. в ней не выбит полностью какой-либо ти элементов
	bool isStable() const;

	/// Количество элементов типа type без учета мертвых (если тип - DAMAGE_ELEMENT_TYPE_MAX возвращает общее количество).
	int elementCount(int element_filter = DAMAGE_FILTER_ALL) const;
	/// Количество живых элементов типа type.
	int aliveElementCount(int element_filter = DAMAGE_FILTER_ALL) const;
	/// Количество мертвых элементов типа type.
	int deadElementCount(int element_filter = DAMAGE_FILTER_ALL) const;

	/// Количество атомов элементов типа type без учета мертвых (если тип - DAMAGE_ELEMENT_TYPE_MAX возвращает общее количество атомов).
	int atomCount(int element_filter = DAMAGE_FILTER_ALL) const { return elementCount(element_filter) * 4; }

	/// Количество элементов типа i, без учета выбитых.
	int& operator[](int i){ return elements_[i]; }
	int operator[](int i) const { return elements_[i]; }

	/// Покомпонентная сумма и разность.
	DamageMolecula& operator += (const DamageMolecula& rhs) { for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){ elements_[i] += rhs.elements_[i]; elementsDead_[i] += rhs.elementsDead_[i]; } return *this; }
	DamageMolecula& operator -= (const DamageMolecula& rhs) { for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){ elements_[i] -= rhs.elements_[i]; elementsDead_[i] -= rhs.elementsDead_[i]; } return *this; }
	DamageMolecula operator + (const DamageMolecula& rhs) const { return DamageMolecula(*this) += rhs; }
	DamageMolecula operator - (const DamageMolecula& rhs) const { return DamageMolecula(*this) -= rhs; }

	DamageMolecula& operator *= (const DamageMolecula& rhs) { for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){ elements_[i] *= rhs.elements_[i]; elementsDead_[i] *= rhs.elementsDead_[i]; } return *this; }
	DamageMolecula& operator *= (int value) { for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){ elements_[i] *= value; elementsDead_[i] *= value; } return *this; }

	DamageMolecula operator * (int value) { return DamageMolecula(*this) *= value; }

	/// Наименьшее частное - сколько можно произвести юнитов, рассчитывается по первым трём типам элементов.
	int operator / (const DamageMolecula& rhs) const { int n, nmin = 1000; for(int i = 0; i < 3; i++) if(rhs[i] && nmin > (n = (elements_[i] - elementsDead_[i])/rhs[i])) nmin = n; xassert(nmin != 1000); return nmin; }

	/// Количество мягких атомов в элементах молекулы.
	int softAtomCount(int element_filter = DAMAGE_FILTER_ALL) const;
	/// Количество твердых атомов в элементач молекулы.
	int hardAtomCount(int element_filter = DAMAGE_FILTER_ALL) const;

	/// Возвращает true, если в молекуле есть хотя бы один мертвый элемент.
	bool needRepair() const { for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++) if(elementsDead_[i]) return true; return false; }
	/// Восстановление мертвых элементов.
	/**
	Возвращает количество реально восстановленных элементов.
	*/
	int repair(int element_count = 1);
	/// Восстановление мертвых элементов определенного типа.
	void repair(DamageElementType element_type,int element_count);

	/// Атака на молекулу.
	/**
	attack_width - ширина атаки (количество атакуемых атомов)
	attack_power - мощность атаки (количество выстрелов по атомам)
	shield - количество мягких атомов, преобразуемых в твердые на время атаки
	attack_filter - фильтр на элементы, из которых берутся атомы для атаки
	damage_filter - фильтр на элементы, которые будут гибнуть при успешном попадании

	возвращает количество убитых в результате атаки элементов
	*/
	int hit(int attack_width,int attack_power,int attack_filter = DAMAGE_FILTER_ALL,int damage_filter = DAMAGE_FILTER_ALL,int shield = 0);

	int killElements(int element_count = 1,int element_filter = DAMAGE_FILTER_ALL);
	void kill()
	{
		zeroize(); isAlive_ = false;
	}

	/// обнуление количества живых элементов
	void zeroize(){ for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++) elementsDead_[i] = elements_[i]; isAlive_ = true; }

	float phase() const { if(elementCount()) return float(aliveElementCount())/float(elementCount()); return 0.0f; }

	int elementsDead(DamageElementType type) const { return elementsDead_[type]; }
	void addElementsDead(DamageElementType type,int count = 1){ elementsDead_[type] += count; if(elementsDead_[type] < 0) elementsDead_[type] = 0; }

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_NAME(elements_, "elements", "элементы"); 
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
		ar & TRANSLATE_OBJECT(boundScale, "Масштабирование");
		ar & TRANSLATE_OBJECT(boundRadius, "Радиус объекта (0 - использовать масштабирование)");
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
	EnumWrapper<terBelligerent> belligerent; // выделяет специализации по воюющей стороне, массив _общий_

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
	bool saveAsCommonObject; // Записывать объект (нельзя ставить для объектов, которые уже пишутся в составе общей иерархии)
	
	float GroundPlaneSpeed;
	float GroundAngleSpeed;
	
	bool MilitaryUnit;
	BitVector<terUnitClassType> UnitClass;
	BitVector<terUnitClassType> AttackClass;
	bool enemyWorld; // Оъекты, принадлежащие миру, будут атаковаться. Скверна.

	PrmString SoundName;
	bool SoundCycled;

	bool InstallBound;
	bool lockInitialPosition;

	bool showPath;

	int ExcludeCollision;
	BitVector<CollisionGroupID> CollisionGroup;

	float MakeEnergy; // в секунду
	float energyCapacity;

	float ConnectionRadius;
	float ZeroLayerRadius;
	float FieldRadius;

	int LifeTime;

	// коэффициент, на который умножается количество элементов, 
	// приносимых строительным блоком при постройке здания
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
	
	RigidBodyPrmLibrary::Reference rigidBodyPrm; // Параметры физики
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
		ar & TRANSLATE_OBJECT(modelData, "Модель");
		ar & TRANSLATE_OBJECT(additionalModelsData, "Дополнительные модели");
		ar & TRANSLATE_OBJECT(animationDataTable, "Описание анимации");
		ar & TRANSLATE_OBJECT(ConnectionPointNames, "Точки dummy");

		if(ar.openBlock("interaction", "Взаимодействие")){
			ar & TRANSLATE_OBJECT(UnitClass, "UnitClass");
			ar & TRANSLATE_OBJECT(AttackClass, "AttackClass");
			ar & TRANSLATE_OBJECT(MilitaryUnit, "MilitaryUnit");
			ar & TRANSLATE_OBJECT(enemyWorld, "enemyWorld (Оъекты, принадлежащие миру, будут атаковаться. Скверна.)");
			ar & TRANSLATE_OBJECT(ExcludeCollision, "ExcludeCollision");
			ar & TRANSLATE_OBJECT(CollisionGroup, "CollisionGroup");
			ar & TRANSLATE_OBJECT(kill_priority, "kill_priority");
			ar.closeBlock();
		}

		if(ar.openBlock("energyModel", "Энергетическая модель")){
			ar & TRANSLATE_OBJECT(MakeEnergy, "MakeEnergy");
			ar & TRANSLATE_OBJECT(energyCapacity, "energyCapacity");
		
			ar & TRANSLATE_OBJECT(ConnectionRadius, "ConnectionRadius");
			ar & TRANSLATE_OBJECT(ZeroLayerRadius, "ZeroLayerRadius");
			ar & TRANSLATE_OBJECT(FieldRadius, "FieldRadius");
			ar.closeBlock();
		}

		ar & TRANSLATE_OBJECT(damageMolecula, "damageMolecula");
		ar & TRANSLATE_OBJECT(unitDamage, "Наносимые повреждения");
		ar & TRANSLATE_OBJECT(environmentalDamage, "Повреждения от окружающей среды");

		ar & TRANSLATE_OBJECT(weaponSetup, "Оружие");
		ar & TRANSLATE_OBJECT(effectsData, "Эффекты");
		ar & TRANSLATE_OBJECT(toolzerSetup, "Тулзера");
		ar & TRANSLATE_OBJECT(soundSetup, "Звуки");
		ar & TRANSLATE_OBJECT(interfacePrm, "Параметры интерфейса");
		ar & TRANSLATE_OBJECT(interfaceTV, "ИнтерфейсТВ");
   
		if(ar.openBlock("Physics", "Физика")){
			ar & TRANSLATE_OBJECT(rigidBodyPrm, "Параметры");
			ar & TRANSLATE_OBJECT(forwardVelocity, "Скорость движения");
			ar & TRANSLATE_OBJECT(flyingHeight, "Высота полета");
			ar.closeBlock();
		}

		if(ar.openBlock("garbage", "Разное")){
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

			ar & TRANSLATE_OBJECT(saveAsCommonObject, "saveAsCommonObject (Записывать объект (нельзя ставить для объектов, которые уже пишутся в составе общей иерархии)");
	
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
