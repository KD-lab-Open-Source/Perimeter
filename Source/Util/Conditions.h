#ifndef __CONDITIONS_H__
#define __CONDITIONS_H__

enum AIPlayerType
{
	AI_PLAYER_TYPE_ME, // Я
	AI_PLAYER_TYPE_ENEMY, // Другой
	AI_PLAYER_TYPE_WORLD, // Мир
	AI_PLAYER_TYPE_ANY // Любой
};

enum CompareOperator
{
	COMPARE_LESS,	// Меньше
	COMPARE_LESS_EQ, // Меньше либо равно
	COMPARE_EQ, // Равно
	COMPARE_NOT_EQ, // Не равно
	COMPARE_GREATER, // Больше
	COMPARE_GREATER_EQ // Больше либо равно		 
};

struct ConditionOneTime : Condition // --------------------
{
	bool check(AIPlayer& aiPlayer) { return satisfiedTimer_(); }
	void setSatisfied(int time = 3000) { satisfiedTimer_.start(time); }
	void clear() { satisfiedTimer_.stop(); }

private:
	DurationTimer satisfiedTimer_;
};

struct ConditionIsPlayerAI : Condition // АИ ли Игрок
{
	bool check(AIPlayer& aiPlayer);
};

struct ConditionCheckBelligerent : Condition // Порверка воюющей стороны
{
	enum Belligerent {
		EXODUS, // Исходники
		HARKBACKHOOD, // Возвратники
		EMPIRE // Империя
	};

	EnumWrapper<Belligerent> belligerent; 

	ConditionCheckBelligerent() {
		belligerent = EXODUS; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(belligerent, "Воюющая сторона");
	}
};

//---------------------------------------
struct ConditionCreateObject : Condition // Объект создан
{
	EnumWrapper<terUnitAttributeID> object; 
	int counter; 
	EnumWrapper<AIPlayerType> playerType; 

	ConditionCreateObject() {
		object = UNIT_ATTRIBUTE_FRAME; 
		counter = 1; 
		playerType = AI_PLAYER_TYPE_ME; 
		created_ = 0;
	}

	bool check(AIPlayer& aiPlayer) { return created_ >= counter; }
	void checkEvent(AIPlayer& aiPlayer, const Event& event);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(object, "Объект");
		ar & TRANSLATE_OBJECT(counter, "Количество");
		ar & TRANSLATE_OBJECT(playerType, "Владелец объекта");
		created_ = 0;
	}

protected:
	int created_;
};

struct ConditionKillObject : Condition // Объект уничтожен
{
	EnumWrapper<terUnitAttributeID> object; 
	int counter; 
	EnumWrapper<AIPlayerType> playerType;

	ConditionKillObject() {
		object = UNIT_ATTRIBUTE_FRAME; 
		counter = 1; 
		playerType = AI_PLAYER_TYPE_ME;
		killed_ = 0;
	}

	bool check(AIPlayer& aiPlayer) { return killed_ >= counter; }
	void checkEvent(AIPlayer& aiPlayer, const Event& event);

	template<class Archive>	
	void serialize(Archive& ar){
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(object, "Объект");
		ar & TRANSLATE_OBJECT(counter, "Количество");
		ar & TRANSLATE_OBJECT(playerType, "Владелец объекта");
		killed_ = 0;
	}

private:
	int killed_;
};

struct ConditionObjectExists : Condition // Объект существует
{
	EnumWrapper<terUnitAttributeID> object; 
	int counter; 
	EnumWrapper<AIPlayerType> playerType;
	bool constructedAndConstructing; 

	ConditionObjectExists() {
		object = UNIT_ATTRIBUTE_FRAME; 
		counter = 1; 
		playerType = AI_PLAYER_TYPE_ME;
		constructedAndConstructing = true; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(object, "Объект");
		ar & TRANSLATE_OBJECT(counter, "Количество");
		ar & TRANSLATE_OBJECT(playerType, "Владелец объекта");
		ar & TRANSLATE_OBJECT(constructedAndConstructing, "Построенных и еще строящихся");
	}
};	

struct ConditionCaptureBuilding : ConditionOneTime // Захват здания
{
	EnumWrapper<terUnitAttributeID> object; 
	EnumWrapper<AIPlayerType> playerType;

	ConditionCaptureBuilding() {
		object = UNIT_ATTRIBUTE_COLLECTOR; 
		playerType = AI_PLAYER_TYPE_ME;
	}

	void checkEvent(AIPlayer& aiPlayer, const Event& event);

	template<class Archive>	
	void serialize(Archive& ar) {
		ConditionOneTime::serialize(ar);
		ar & TRANSLATE_OBJECT(object, "Здание");
		ar & TRANSLATE_OBJECT(playerType, "Захвативший Игрок");
	}
};

//---------------------------------------
enum TeleportationType
{
	TELEPORTATION_TYPE_ALPHA, // Телепортация с помощью Альфы
	TELEPORTATION_TYPE_OMEGA // Телепортация с помощью Омеги
};
struct ConditionTeleportation : ConditionOneTime // Произошла телепортация
{
	EnumWrapper<TeleportationType> teleportationType; 
	EnumWrapper<AIPlayerType> playerType; 

	ConditionTeleportation() {
		teleportationType = TELEPORTATION_TYPE_ALPHA; 
		playerType = AI_PLAYER_TYPE_ME; 
	}

	void checkEvent(AIPlayer& aiPlayer, const Event& event);

	template<class Archive>	
	void serialize(Archive& ar) {
		ConditionOneTime::serialize(ar);
		ar & TRANSLATE_OBJECT(teleportationType, "Тип телепорта");
		ar & TRANSLATE_OBJECT(playerType, "Игрок");
	}
};	

//---------------------------------------
struct ConditionEnegyLevelLowerReserve : Condition // Уровень энергии ниже предела
{
	float energyReserve; 

	ConditionEnegyLevelLowerReserve() {
		energyReserve = 300; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(energyReserve, "Резерв энергии");
	}
};

struct ConditionEnegyLevelUpperReserve : Condition // Уровень энергии выше предела
{
	float energyReserve; 

	ConditionEnegyLevelUpperReserve() {
		energyReserve = 300; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(energyReserve, "Резерв энергии");
	}
};

struct ConditionEnegyLevelBelowMaximum : Condition // Уровень энергии ниже максимума
{
	float delta; 

	ConditionEnegyLevelBelowMaximum() {
		delta = 50; 
		accumulatedMax_ = 0;
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(delta, "Погрешность: текущий < максимальный - погрешность");
		accumulatedMax_ = 0;
	}

private:
	float accumulatedMax_;
};

struct ConditionOutOfEnergyCapacity : Condition // Уровень энергии больше процента емкости
{
	float chargingPercent; 

	ConditionOutOfEnergyCapacity() {
		chargingPercent = 99; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(chargingPercent, "Процент зарядки");
	}
};

struct ConditionNumberOfBuildingByCoresCapacity : Condition // Зданий_1*коэффициент < Зданий_2
{
	EnumWrapper<terUnitAttributeID> building; 
	float factor; 
	EnumWrapper<CompareOperator> compareOp; 
	EnumWrapper<terUnitAttributeID> building2; 
	EnumWrapper<AIPlayerType> playerType; 

	ConditionNumberOfBuildingByCoresCapacity() {
		building = UNIT_ATTRIBUTE_LASER_CANNON; 
		factor = 1; 
		compareOp = COMPARE_LESS; 
		building2 = UNIT_ATTRIBUTE_CORE; 
		playerType = AI_PLAYER_TYPE_ME; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(building, "Тип здания_1");
		ar & TRANSLATE_OBJECT(factor, "Коэффициент");
		ar & TRANSLATE_OBJECT(compareOp, "Операция сравнения");
		ar & TRANSLATE_OBJECT(building2, "Тип здания_2");
		ar & TRANSLATE_OBJECT(playerType, "Игрок");
	}
};


//---------------------------------------
struct ConditionUnitClassUnderAttack : ConditionOneTime // Объект атакуют
{
	BitVector<terUnitClassType> victimUnitClass; 
	int damagePercent; 
	BitVector<terUnitClassType> agressorUnitClass; 
	EnumWrapper<AIPlayerType> playerType; 

	ConditionUnitClassUnderAttack() {
		damagePercent = 0; 
		playerType = AI_PLAYER_TYPE_ME; 
	}

	void checkEvent(AIPlayer& aiPlayer, const Event& event);

	template<class Archive>	
	void serialize(Archive& ar) {
		ConditionOneTime::serialize(ar);
		ar & TRANSLATE_OBJECT(victimUnitClass, "атакуемый класс юнитов");
		ar & TRANSLATE_OBJECT(damagePercent, "Процент урона");
		ar & TRANSLATE_OBJECT(agressorUnitClass, "атакующий класс юнитов");
		ar & TRANSLATE_OBJECT(playerType, "Владелец объекта");
	}
};

struct ConditionUnitClassIsGoingToBeAttacked : ConditionOneTime // Объект собираются атаковать
{
	BitVector<terUnitClassType> victimUnitClass; 
	BitVector<terUnitClassType> agressorUnitClass; 

	void checkEvent(AIPlayer& aiPlayer, const Event& event);

	template<class Archive>	
	void serialize(Archive& ar) {
		ConditionOneTime::serialize(ar);
		ar & TRANSLATE_OBJECT(victimUnitClass, "атакуемый класс юнитов");
		ar & TRANSLATE_OBJECT(agressorUnitClass, "атакующий класс юнитов");
	}
};

struct ConditionSquadGoingToAttack : Condition // Сквад собирается атаковать
{
	EnumWrapper<ChooseSquadID> chooseSquadID; 

	ConditionSquadGoingToAttack() {
		chooseSquadID = CHOOSE_SQUAD_1; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(chooseSquadID, "Сквад");
	}
};

//---------------------------------------
struct ConditionFrameState : Condition // Состояние фрейма
{
	enum State // Состояния фрейма
	{
		AI_FRAME_STATE_EXIST, // Существует
		AI_FRAME_STATE_INSTALLED, // Инсталлирован
		AI_FRAME_STATE_INSTALLING, // Инсталлируется в данный момент
		AI_FRAME_STATE_POWERED, // Подключен
		AI_FRAME_STATE_MOVING, // Двигается
		AI_FRAME_STATE_TELEPORTATION_ENABLED, // Телепортация разрешена
		AI_FRAME_STATE_TELEPORTATION_STARTED, // Телепортация началась
		AI_FRAME_STATE_SPIRAL_CHARGING // Спираль заряжена на X % и более
	};
	EnumWrapper<State> state; 
	EnumWrapper<AIPlayerType> playerType; 
	int spiralChargingPercent; 

	ConditionFrameState() {
		state = AI_FRAME_STATE_INSTALLED; 
		playerType = AI_PLAYER_TYPE_ME; 
		spiralChargingPercent = 100; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(state, "Интересующее состояние");
		ar & TRANSLATE_OBJECT(playerType, "Игрок");
		ar & TRANSLATE_OBJECT(spiralChargingPercent, "Процент зарядки спирали");
	}
};

struct ConditionCorridorOmegaUpgraded : Condition // Коридор Омега проапгрейжен
{
	bool check(AIPlayer& aiPlayer);
};

//---------------------------------------
struct ConditionSquadSufficientUnits : Condition // Cквад состоит из юнитов в указанном количестве
{
	EnumWrapper<AIPlayerType> playerType;
	EnumWrapper<ChooseSquadID> chooseSquadID; 
	EnumWrapper<terUnitAttributeID> unitType; 
	EnumWrapper<CompareOperator> compareOperator; 
	int unitsNumber;
	int soldiers; 
	int officers; 
	int technics; 

	ConditionSquadSufficientUnits() {
		playerType = AI_PLAYER_TYPE_ME;
		chooseSquadID = CHOOSE_SQUAD_1;
		unitType = UNIT_ATTRIBUTE_NONE;
		compareOperator = COMPARE_EQ;
		unitsNumber = 0;
		soldiers = 0;
		officers = 0;
		technics = 0;
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(playerType, "Владелец");
		ar & TRANSLATE_OBJECT(chooseSquadID, "Сквад");
		ar & TRANSLATE_OBJECT(unitType, "Тип юнитов ('никто' - базовые)");
		ar & TRANSLATE_OBJECT(compareOperator, "Операция сравнения");
		ar & TRANSLATE_OBJECT(unitsNumber, "Производные");
		ar & TRANSLATE_OBJECT(soldiers, "Солдатов при базовой");
		ar & TRANSLATE_OBJECT(officers, "Офицеров при базовой");
		ar & TRANSLATE_OBJECT(technics, "Техников при базовой");
	}
};

struct ConditionMutationEnabled : Condition // Мутация разрешена
{
	EnumWrapper<terUnitAttributeID> unitType; 

	ConditionMutationEnabled() {
		unitType = UNIT_ATTRIBUTE_NONE; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(unitType, "Тип юнитов");
	}
};

struct ConditionBuildingNearBuilding : Condition // Расстояние от ядра Игрока1 до любого здания Игрока2 меньше определенного
{
	float distance; 
	EnumWrapper<AIPlayerType> playerType1; 
	EnumWrapper<AIPlayerType> playerType2; 

	ConditionBuildingNearBuilding() {
		distance = 500;
		playerType1 = AI_PLAYER_TYPE_ME;
		playerType2 = AI_PLAYER_TYPE_ENEMY;
		index_ = 0;
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(distance, "Расстояние");
		ar & TRANSLATE_OBJECT(playerType1, "Игрок1");
		ar & TRANSLATE_OBJECT(playerType2, "Игрок2");
		index_ = 0;
	}

private:
	int index_;
};

enum PlayerState // Состояние игрока 
{
	PLAYER_STATE_UNABLE_TO_PLACE_BUILDING, // Не могу установить здание
	PLAYER_STATE_UNABLE_TO_PLACE_CORE // Не могу установить ядро
};

struct ConditionPlayerState : Condition // Проверка состояния игрока
{
	EnumWrapper<PlayerState> playerState; 

	ConditionPlayerState() {
		playerState = PLAYER_STATE_UNABLE_TO_PLACE_BUILDING; 
		active_ = false;
	}

	bool check(AIPlayer& aiPlayer) { return active_; }
	void checkEvent(AIPlayer& aiPlayer, const Event& event);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(playerState, "Интересующее состояние");
		active_ = false;
	}

private:
	bool active_;
};

struct ConditionIsFieldOn : Condition // Поле включено
{
	bool check(AIPlayer& aiPlayer);
};

//---------------------------------------
struct ConditionObjectByLabelExists : Condition // Объект по метке существует
{
	CustomString label; 

	ConditionObjectByLabelExists() :
	label(editLabelDialog)
	{}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(label, "Метка объекта");
	}
};	

struct ConditionKillObjectByLabel : ConditionOneTime // Объект по метке уничтожен
{
	CustomString label; 

	ConditionKillObjectByLabel() :
	label(editLabelDialog)
	{}

	void checkEvent(AIPlayer& aiPlayer, const Event& event);

	template<class Archive>	
	void serialize(Archive& ar) { 
		ConditionOneTime::serialize(ar);
		ar & TRANSLATE_OBJECT(label, "Метка объекта");
	}
};

struct ConditionObjectNearObjectByLabel : Condition // Возле объекта по метке находится объект указанного типа
{
	CustomString label; 
	EnumWrapper<terUnitAttributeID> object; 
	bool objectConstructed; 
	EnumWrapper<AIPlayerType> playerType; 
	float distance; 

	ConditionObjectNearObjectByLabel() :
	label(editLabelDialog)
	{
		object = UNIT_ATTRIBUTE_CORE; 
		objectConstructed = false; 
		playerType = AI_PLAYER_TYPE_ME; 
		distance = 100; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(label, "Метка объекта");
		ar & TRANSLATE_OBJECT(object, "Указанный объект");
		ar & TRANSLATE_OBJECT(objectConstructed, "Только если объект достроен");
		ar & TRANSLATE_OBJECT(playerType, "Владелец объекта");
		ar & TRANSLATE_OBJECT(distance, "Максимальное расстояние");
	}
};

struct ConditionWeaponIsFiring : Condition // Спецоружие стреляет
{
	EnumWrapper<terUnitAttributeID> gun; 

	ConditionWeaponIsFiring() {
		gun = UNIT_ATTRIBUTE_GUN_SCUM_DISRUPTOR; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(gun, "Спецоружие");
	}
};

struct ConditionTimeMatched : ConditionOneTime // Осталось времени меньше, чем указано
{
	int time; 
	
	ConditionTimeMatched() {
		time = 60; 
	}

	void checkEvent(AIPlayer& aiPlayer, const Event& event);

	template<class Archive>	
	void serialize(Archive& ar) {
		ConditionOneTime::serialize(ar);
		ar & TRANSLATE_OBJECT(time, "Время, секунды");
	}
};

struct ConditionMouseClick : ConditionOneTime // Клик мыши
{
	void checkEvent(AIPlayer& aiPlayer, const Event& event);
};

struct ConditionClickOnButton : Condition // Клик по кнопке
{
	EnumWrapper<ShellControlID> controlID; 
	int counter; 

	ConditionClickOnButton() {
		controlID = SQSH_STATIC_ID; 
		counter = 1; 
		counter_ = 0;
	}

	bool check(AIPlayer& aiPlayer) { return counter_ >= counter; }
	void checkEvent(AIPlayer& aiPlayer, const Event& event);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(controlID, "Идентификатор кнопки");
		ar & TRANSLATE_OBJECT(counter, "Количество кликов");
		counter_ = 0;
	}

private:
	int counter_;
};

struct ConditionToolzerSelectedNearObjectByLabel : Condition // Поверхность возле объекта по метке выделена для выравнивания
{
	CustomString label;
	int radius; 

	ConditionToolzerSelectedNearObjectByLabel() :
	label(editLabelDialog)
	{
		radius = 100; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(label, "Метка объекта");
		ar & TRANSLATE_OBJECT(radius, "Радиус");
	}
};	

struct ConditionTerrainLeveledNearObjectByLabel : Condition // Поверхность возле объекта по метке выровнена
{
	CustomString label; 
	int radius;

	ConditionTerrainLeveledNearObjectByLabel() :
	label(editLabelDialog)
	{
		radius = 100;
	}
	
	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(label, "Метка объекта");
		ar & TRANSLATE_OBJECT(radius, "Радиус");
	}
};	

struct ConditionSetSquadWayPoint : Condition // Флажок скваду установлен
{
	bool check(AIPlayer& aiPlayer);
};	

struct ConditionActivateSpot : ConditionOneTime // Активировался спот
{
	enum Type {
		FILTH = 1, // Скверна
		GEO = 2 // Гео
	};
	BitVector<Type> type; 

	ConditionActivateSpot() {
		type = FILTH | GEO; 
	}

	void checkEvent(AIPlayer& aiPlayer, const Event& event);

	template<class Archive>	
	void serialize(Archive& ar) {
		ConditionOneTime::serialize(ar);
		ar & TRANSLATE_OBJECT(type, "тип спота");
	}
};


struct ConditionOnlyMyClan : ConditionOneTime // Остался только мой клан
{
	bool check(AIPlayer& aiPlayer);
};

struct ConditionSkipCutScene : Condition // Пропустить кат-сцену
{
	bool check(AIPlayer& aiPlayer);
};

struct ConditionCutSceneWasSkipped : ConditionSkipCutScene // Кат-сцена была пропущена
{
	int timeMax; 

	ConditionCutSceneWasSkipped() {
		timeMax = 5; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		ConditionSkipCutScene::serialize(ar);
		ar & TRANSLATE_OBJECT(timeMax, "Максимальное время");
	}
};

struct ConditionDifficultyLevel : Condition // Уровень сложности
{
	EnumWrapper<Difficulty> difficulty; 

	ConditionDifficultyLevel() {
		difficulty = DIFFICULTY_HARD; 
	}

	bool check(AIPlayer& aiPlayer);

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(difficulty, "Уровень");
	}
};


#endif //__CONDITIONS_H__