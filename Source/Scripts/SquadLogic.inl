////////////////////////////////////////////////////////////////////////////////////
//		SquadLogic.inl
//
// Описвает логику сквада.
// Все данные определяются как static (не будут
// транслироваться в cpp/h) и будут встраиваться
// в terUnitAttributeData.
////////////////////////////////////////////////////////////////////////////////////

struct LegionaryPrm
{
	int is_base_unit = 0;

	float formationRadiusFactor = 1.2;

	int dynamicAttack = 0;
	float attackTurnRadius = 150;
	float attackAngleDeviation = 0;

	int ignoreUninstalledFrame = 0;
	int dontUseDamageMapPathFind = 0;
};

/////////////////////////////////////////////////////////////////////////////////
//			Base Units
/////////////////////////////////////////////////////////////////////////////////
// солдат
static LegionaryPrm soldierSquadPrm = 
{
	is_base_unit = 1;
};

 // техник
static LegionaryPrm technicSquadPrm = 
{
	is_base_unit = 1;
};

// офицер
static LegionaryPrm officierSquadPrm = 
{
	is_base_unit = 1;
};


/////////////////////////////////////////////////////////////////////////////////
//		Missile Units
/////////////////////////////////////////////////////////////////////////////////
static LegionaryPrm wargonSquadPrm = 
{
};

static LegionaryPrm mortarSquadPrm = 
{
};


/////////////////////////////////////////////////////////////////////////////////
//		Flying Units
/////////////////////////////////////////////////////////////////////////////////
static LegionaryPrm ceptorSquadPrm = 
{
	//dynamicAttack = 1;
	//formationRadiusFactor = 2;
};

static LegionaryPrm gyroidSquadPrm = 
{
	//dynamicAttack = 1;
	//formationRadiusFactor = 2;
};

static LegionaryPrm eflairSquadPrm = 
{
	//dynamicAttack = 1;
	//formationRadiusFactor = 2;
};

static LegionaryPrm bomberSquadPrm = 
{
	dynamicAttack = 1;
	//formationRadiusFactor = 2;
};

static LegionaryPrm straferSquadPrm = 
{
	//dynamicAttack = 1;
	//formationRadiusFactor = 2;
};

static LegionaryPrm disintegratorSquadPrm = 
{
	dynamicAttack = 1;
};

static LegionaryPrm unseenSquadPrm = 
{
};

/////////////////////////////////////////////////////////////////////////////////
//			Laser Units
/////////////////////////////////////////////////////////////////////////////////
static LegionaryPrm rockerSquadPrm = 
{
};

static LegionaryPrm projectorSquadPrm = 
{
};

static LegionaryPrm sniperSquadPrm = 
{
};

static LegionaryPrm leamoSquadPrm = 
{
};

static LegionaryPrm impalerSquadPrm = 
{
};

static LegionaryPrm scumerSquadPrm = 
{
	dynamicAttack = 1;
	ignoreUninstalledFrame = 1;
	dontUseDamageMapPathFind = 1;
	attackAngleDeviation = 0.5;
	attackTurnRadius = 100;
};

static LegionaryPrm diggerSquadPrm = 
{
	dynamicAttack = 1;
	ignoreUninstalledFrame = 1;
	dontUseDamageMapPathFind = 1;
	attackAngleDeviation = 0.5;
	attackTurnRadius = 100;
};

static LegionaryPrm piercerSquadPrm = 
{
	ignoreUninstalledFrame = 1;
	dontUseDamageMapPathFind = 1;
};

static LegionaryPrm minotaurSquadPrm = 
{
};

static LegionaryPrm leechSquadPrm = 
{
};

static LegionaryPrm scumThrowerSquadPrm = 
{
};

static LegionaryPrm extirpatorSquadPrm = 
{
	ignoreUninstalledFrame = 1;
};

static LegionaryPrm scumSplitterSquadPrm = 
{
	ignoreUninstalledFrame = 1;
};

static LegionaryPrm scumHeaterSquadPrm = 
{
	dynamicAttack = 1;
};

static LegionaryPrm conductorSquadPrm = 
{
	dynamicAttack = 1;
};

static LegionaryPrm scumTwisterSquadPrm = 
{
};

static LegionaryPrm filthSpot0SquadPrm = 
{
	ignoreUninstalledFrame = 1;
};

static LegionaryPrm filthSpot1SquadPrm = 
{
	ignoreUninstalledFrame = 1;
};

static LegionaryPrm filthSpot2SquadPrm = 
{
};

static LegionaryPrm filthSpot3SquadPrm = 
{
};

/////////////////////////////////////////////////////////////////////////////////
//		Атакуемость: приоритеты
/////////////////////////////////////////////////////////////////////////////////
struct AttackablePrm 
{
	float kill_priority = 1;
};


static struct AttackableData
{
	enum {
		_Brigadier = 2, 
		_BuildMaster = 2, 
		_Truck = 1,
		_BuildingBlock = 1,

		//  Строения
		_Frame = 1,
		_SoldierPlant,
		_OfficerPlant,
		_TechnicPlant,
		_Laboratory_1,
		_Laboratory_2,
		_Transmitter,
		_Laboratory_3,
		_EnergyApplifier,
		_Commander,	 // 10
		_Core,
		_StaticBomb,

		// Простые юниты сквада
		_Soldier,
		_Officer,
		_Technic,

		_Perforator,
		_Sniper,
		_Rocker,
		_Mortar,

		_R_Projector,
		_Chaser, 
		_Minotaur,

		_Gyroid,
		_Strafer,
		_Ceptor,

		_Scumer,
		_Digger,

		_Leamo,
		_Extirpator,
		
		//  Скверна
		_FilthFly,
		_FilthGround,
		_FilthUnderGround,
		
		//  Стационарные орудия
		_LaserGun, 
		_RocketGun,
		_Howitzer,
		_Subchaser,

		// Продвинутые юниты
		_Bombie,

		_Wargon,

		_Leech,
		_Sucker,
		_ScumSplitter,
		_Scourge0,

		_Unseen,
		_Disintegrator,
		_ScumTwister,
		_ScumHeater,
		_Scourge1,
		_Scourge2,

		_ScumThrower,
		_Piercer,
		_Scourge3,

		_ScumDisruptor,
		_BallisticMissile,
		_FilthNavigator
	};


	AttackablePrm Default = {};

	//////////////////////////////////////////////////
	//		Служебные юниты
	//////////////////////////////////////////////////
	AttackablePrm Brigadier = { kill_priority = _Brigadier; };
	AttackablePrm BuildMaster = { kill_priority = _BuildMaster; };
	AttackablePrm Truck = { kill_priority = _Truck; };
	AttackablePrm BuildingBlock = { kill_priority = _BuildingBlock; };

	//////////////////////////////////////////////////
	//		Строения
	//////////////////////////////////////////////////
	AttackablePrm Frame = { kill_priority = _Frame; };
	AttackablePrm PlantSoldier = { kill_priority = _SoldierPlant; };
	AttackablePrm PlantOfficer = { kill_priority = _OfficerPlant; };
	AttackablePrm PlantTechnician = { kill_priority = _TechnicPlant; };
	AttackablePrm Laboratory_1 = { kill_priority = _Laboratory_1; };
	AttackablePrm Core = { kill_priority = _Core; };
	AttackablePrm Laboratory_2 = { kill_priority = _Laboratory_2; };
	AttackablePrm Transmitter = { kill_priority = _Transmitter; };
	AttackablePrm Laboratory_3 = { kill_priority = _Laboratory_3; };
	AttackablePrm EnergyApplifier = { kill_priority = _EnergyApplifier; };
	AttackablePrm CommandCenter = { kill_priority = _Commander; };
	AttackablePrm StaticBomb = { kill_priority = _StaticBomb; };

	//////////////////////////////////////////////////
	//		Простые юниты сквада
	//////////////////////////////////////////////////
	AttackablePrm Soldier = { kill_priority = _Soldier; };
	AttackablePrm Officer = { kill_priority = _Officer; };
	AttackablePrm Technic = { kill_priority = _Technic; };

	AttackablePrm Perforator = { kill_priority = _Perforator; };
	AttackablePrm Sniper = { kill_priority = _Sniper; };
	AttackablePrm Rocker = { kill_priority = _Rocker; };
	AttackablePrm Mortar = { kill_priority = _Mortar; };

	AttackablePrm RProjector = { kill_priority = _R_Projector; };
	AttackablePrm Chaser = { kill_priority = _Chaser; };
	AttackablePrm Minotaur = { kill_priority = _Minotaur; };

	AttackablePrm Gyroid = { kill_priority = _Gyroid; };
	AttackablePrm Strafer = { kill_priority = _Strafer; };
	AttackablePrm Ceptor = { kill_priority = _Ceptor; };

	AttackablePrm Leamo = { kill_priority = _Leamo; };
	AttackablePrm Unseen = { kill_priority = _Unseen; };

	AttackablePrm ScumSplitter = { kill_priority = _ScumSplitter; };
	AttackablePrm ScumTwister = { kill_priority = _ScumTwister; };
	AttackablePrm ScumHeater = { kill_priority = _ScumHeater; };
	AttackablePrm ScumThrower = { kill_priority = _ScumThrower; };
	AttackablePrm Extirpator = { kill_priority = _Extirpator; };

	AttackablePrm Scumer = { kill_priority = _Scumer; };
	AttackablePrm Digger = { kill_priority = _Digger; };
	AttackablePrm Piercer = { kill_priority = _Piercer; };

	///////////////////////////////////////////////
	//		Скверна
	///////////////////////////////////////////////
	AttackablePrm AntsFilth = { kill_priority = _FilthFly; };
	AttackablePrm RatFilth = { kill_priority = _FilthFly; };
	AttackablePrm WormFilth = { kill_priority = _FilthGround; };
	AttackablePrm WaspFilth = { kill_priority = _FilthGround; };

	
	//////////////////////////////////////////////////		 
	//		Стационарные орудия
	//////////////////////////////////////////////////
	AttackablePrm LaserGun = { kill_priority = _LaserGun; };
	AttackablePrm GunRocket = { kill_priority = _RocketGun; };
	AttackablePrm GunHowitzer = { kill_priority = _Howitzer; };
	AttackablePrm GunSubchaser = { kill_priority = _Subchaser; };


	//////////////////////////////////////////////////
	//		Продвинутые юниты
	//////////////////////////////////////////////////
	AttackablePrm Bombie = { kill_priority = _Bombie; };
	AttackablePrm Disintegrator = { kill_priority = _Disintegrator; };

	AttackablePrm Wargon = { kill_priority = _Wargon; };

	AttackablePrm Leech = { kill_priority = _Leech; };

	AttackablePrm ScumDisruptor = { kill_priority = _ScumDisruptor; };
	AttackablePrm GunBallistic = { kill_priority = _BallisticMissile; };
	AttackablePrm GunFilthNavigator = { kill_priority = _FilthNavigator; };

	AttackablePrm Scourge0 = { kill_priority = _Scourge0; };
	AttackablePrm Scourge1 = { kill_priority = _Scourge1; };
	AttackablePrm Scourge2 = { kill_priority = _Scourge2; };
	AttackablePrm Scourge3 = { kill_priority = _Scourge3; };

};

static AttackableData attackable_data = {};
