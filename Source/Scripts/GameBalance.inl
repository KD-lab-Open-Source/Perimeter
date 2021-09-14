
int SPECIAL_UNITS_EL_MUL = 3;
int SPECIAL_ELECTRO_UNITS_EL_MUL = 2;

int SHIELD_LASER_MUL = 1;
int SHIELD_ROCKET_MUL = 2;
int SHIELD_BOMB_MUL = 3;
int SHIELD_SPECIAL_MUL = 4;

int RADIUS_SHORT = 200;
int RADIUS_MIDDLE = 300;
int RADIUS_FAR = 400;
int RADIUS_SUPER_FAR = 1500;
int RADIUS_ADD = 100;

int FrameRepairElementsPerQuant = 10;

static struct Balance
{
	struct Unit // Basic structure
	{
		// Target classes
		int		AttackClass = UNIT_CLASS_IGNORE;
		// Main Attack parameters
		int		maPower = 0;
		int		maWidth = 0;
		// Splash Attack parameters
		int		saPower = 0;
		int		saWidth = 0;
		int		saRadius = 0;
		// Energy Consumption
		float	ReloadEnergy = 0.1;
		int		ReloadTime = 1000;
		int		ReloadPriority = 1000;
		// Range
		int		RangeFire = 300;
		int		RangeSight = 500;

		int		AccuracyRadius = 0;

		float	DischargeSpeed = 0.3;

		string	Name = "";
	};

	struct MobileUnit : Unit // Mobile units
	{
		// Elements formula
		int elSoldiers = 0;
		int elOfficers = 0;
		int elTechnicians = 0;
		// Shield elements
		int elShield = 0;
	};

	struct StaticUnit : Unit // Static weapon buildings
	{
		// Structure Elements 
		int elSoft = 0;
		int elHard = 0;
	};

	struct ScourgeUnit : MobileUnit // Scourge units
	{
		terFilthSpotID Type = FILTH_SPOT_ID_NONE;
		int Number = 0;
		int LifeTime = 0;
	};

	struct SpecialUnit : MobileUnit
	{
		int StealEnergy = 25;
	};

	MobileUnit Soldier = {
		elSoldiers = 1;

//		elShield = elSoldiers + elOfficers + elTechnicians;

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_GROUND_FILTH|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL;

		maPower = 2;
		maWidth = 8;

		ReloadEnergy = 0.04;
		ReloadTime = 6000;
		ReloadPriority = 10;

		RangeFire = RADIUS_SHORT;
		RangeSight = RADIUS_SHORT +RADIUS_ADD;
	};
	MobileUnit Officer = {
		elOfficers = 1;

//		elShield = elSoldiers + elOfficers + elTechnicians;

		AttackClass = UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY;

		ReloadEnergy = 0.05;
		ReloadTime = 6000;
		ReloadPriority = 10;

		RangeFire = RADIUS_SHORT;
		RangeSight = RADIUS_SHORT +RADIUS_ADD;
	};
	MobileUnit Technician = {
		elTechnicians = 1;

//		elShield = elSoldiers + elOfficers + elTechnicians;

		AttackClass = UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY;

		ReloadEnergy = 0.1;
		ReloadTime = 6000;
		ReloadPriority = 10;

		RangeFire = RADIUS_SHORT;
		RangeSight = RADIUS_SHORT +RADIUS_ADD;
	};
	MobileUnit Digger = {
		elOfficers = 2;
		elTechnicians = 2;

		elShield = elSoldiers + elOfficers + elTechnicians;

		AttackClass = UNIT_CLASS_GROUND|UNIT_CLASS_FRAME|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL;

		ReloadEnergy = 0.06;
		ReloadPriority = 90;
	};
	MobileUnit Strafer = {
		elSoldiers = 4;
		elOfficers = 4;

		elShield = elSoldiers + elOfficers + elTechnicians;

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_GROUND_FILTH|UNIT_CLASS_AIR_FILTH|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_UNDERGROUND|UNIT_CLASS_UNDERGROUND_FILTH;

		maPower = 8;
		maWidth = 20;

		ReloadEnergy = 0.06;
		ReloadTime = 150;
		ReloadPriority = 10;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;
	};

	// Laser Weapon
	MobileUnit Sniper = {
		elOfficers = 3;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_LASER_MUL);

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_BLOCK|UNIT_CLASS_GROUND_FILTH|UNIT_CLASS_AIR_FILTH|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY;

		maPower = 3;
		maWidth = 6;

		ReloadEnergy = 0.12;
		ReloadTime = 1000;
		ReloadPriority = 10;

		DischargeSpeed = 1;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;
	};
	MobileUnit Scumer = {
		elSoldiers = 0;
		elOfficers = 7;
		elTechnicians = 3;

		AttackClass = UNIT_CLASS_GROUND|UNIT_CLASS_FRAME|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_LASER_MUL);

		ReloadEnergy = 0.2;
		ReloadPriority = 90;
	};
	MobileUnit Gyroid = {
		elOfficers = 4;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_LASER_MUL)*2;

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_BLOCK|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_GROUND_FILTH|UNIT_CLASS_AIR_FILTH;

		maPower = 10;
		maWidth = 6;

		ReloadEnergy = 0.09;
		ReloadTime = 1000;
		ReloadPriority = 10;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;
	};

	MobileUnit Leamo = {
		elSoldiers = 1;
		elOfficers = 12;
		elTechnicians = 7;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_LASER_MUL);

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_FRAME;

		maPower = 50;
		maWidth = 50;

		ReloadEnergy = 0.65;
		ReloadTime = 1000;
		ReloadPriority = 10;

		DischargeSpeed = 1;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;
	};

	// Static gun
	StaticUnit GunLaser = {
		elHard = 50;

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_BLOCK|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_GROUND_FILTH|UNIT_CLASS_AIR_FILTH|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY;

		ReloadEnergy = 0.16;
		ReloadTime = 500;
		ReloadPriority = 10;

		DischargeSpeed = 0.2;

		maPower = 15;
		maWidth = 15;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;
	};

	// Rocket weapon
	MobileUnit Rocker = {
		elSoldiers = 3;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_ROCKET_MUL);

		AttackClass = UNIT_CLASS_BASE|UNIT_CLASS_BUILDER|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_AIR_FILTH|UNIT_CLASS_GROUND_FILTH|UNIT_CLASS_HEAVY|UNIT_CLASS_MEDIUM|UNIT_CLASS_LIGHT|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_FRAME|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		maPower = 8;
		maWidth = 20;
		
		saPower = 4;
		saWidth = 10;
		saRadius = 10;

		ReloadEnergy = 0.13;
		ReloadTime = 2000;
		ReloadPriority = 11;

		DischargeSpeed = 0.15;

		RangeFire = RADIUS_SHORT;
		RangeSight = RADIUS_SHORT +RADIUS_ADD;
	};
	MobileUnit RProjector = {
		elSoldiers = 6;
		elTechnicians = 3;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_ROCKET_MUL);

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_AIR_FILTH|UNIT_CLASS_GROUND_FILTH|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_FRAME|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		ReloadEnergy = 0.29;
		ReloadTime = 1000;
		ReloadPriority = 11;

		maPower = 25;
		maWidth = 20;
		
		saPower = 15;
		saWidth = 10;
		saRadius = 10;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;
	};
	MobileUnit Ceptor = {
		elSoldiers = 6;
		elOfficers = 4;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_ROCKET_MUL);

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_BASE|UNIT_CLASS_GROUND_FILTH|UNIT_CLASS_AIR_FILTH|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_HEAVY|UNIT_CLASS_MEDIUM|UNIT_CLASS_LIGHT|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_CORE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_FRAME|UNIT_CLASS_STRUCTURE_SPECIAL;

		maPower = 25;
		maWidth = 20;
		
		saPower = 15;
		saWidth = 10;
		saRadius = 10;

		ReloadEnergy = 0.29;
		ReloadTime = 1000;
		ReloadPriority = 11;

		RangeFire = RADIUS_SHORT;
		RangeSight = RADIUS_SHORT +RADIUS_ADD;
	};
	MobileUnit Extirpator = {
		elSoldiers = 7;
		elOfficers = 6;
		elTechnicians = 7;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_ROCKET_MUL);

		AttackClass = UNIT_CLASS_GROUND|UNIT_CLASS_UNDERGROUND|UNIT_CLASS_UNDERGROUND_FILTH|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL;

		ReloadEnergy = 1.15;
		ReloadTime = 4000;
		ReloadPriority = 12;

		DischargeSpeed = 1;

		maPower = 80;
		maWidth = 80;
		saPower = 80;
		saWidth = 80;

		saRadius = 40;

		RangeFire = RADIUS_SHORT;
		RangeSight = RADIUS_SHORT +RADIUS_ADD;
	};

	// Static guns
	StaticUnit GunRocket = {
		elHard = 75;

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_AIR_FILTH|UNIT_CLASS_GROUND_FILTH;

		ReloadEnergy = 0.29;
		ReloadTime = 500;
		ReloadPriority = 11;

		DischargeSpeed = 0.3;

		maPower = 10;
		maWidth = 20;
		
		saPower = 5;
		saWidth = 10;
		saRadius = 10;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;
	};
	StaticUnit GunSubchaser = {
		elHard = 100;

		AttackClass = UNIT_CLASS_GROUND|UNIT_CLASS_UNDERGROUND|UNIT_CLASS_UNDERGROUND_FILTH;

		ReloadEnergy = 0.29;
		ReloadTime = 500;
		ReloadPriority = 12;

		DischargeSpeed = 0.3;

		maPower = 28;
		maWidth = 30;
		saPower = 10;
		saWidth = 20;
		saRadius = 20;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;
	};

	// Bomb weapon
	MobileUnit Mortar = {
		elTechnicians = 4;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_BOMB_MUL);

		AttackClass = UNIT_CLASS_STRUCTURE_ENVIRONMENT|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_FRAME;

		saPower = 16;
		saWidth = 20;
		saRadius = 5;

		ReloadEnergy = 0.31;
		ReloadTime = 3000;
		ReloadPriority = 12;

		DischargeSpeed = 1;

		RangeFire = RADIUS_SHORT;
		RangeSight = RADIUS_SHORT +RADIUS_ADD;

		AccuracyRadius = 30;
	};
	MobileUnit Wargon = {
		elSoldiers = 2;
		elTechnicians = 8;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_BOMB_MUL);

		AttackClass = UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_FRAME|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		ReloadEnergy = 0.43;
		ReloadTime = 600;
		ReloadPriority = 12;

		DischargeSpeed = 0.5;

		saPower = 25;
		saWidth = 20;
		saRadius = 10;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;

		AccuracyRadius = 50;
	};
	MobileUnit Bombie = {
		elSoldiers = 2;
		elOfficers = 6;
		elTechnicians = 12;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_BOMB_MUL -20);

		AttackClass = UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_LIGHT|UNIT_CLASS_HEAVY|UNIT_CLASS_MEDIUM|UNIT_CLASS_FRAME;

		ReloadEnergy = 0.25;
		ReloadTime = 600;
		ReloadPriority = 12;

		saPower = 25;
		saWidth = 20;
		saRadius = 15;

		RangeFire = RADIUS_SHORT;
		RangeSight = RADIUS_SHORT +RADIUS_ADD;

		AccuracyRadius = 100;
	};

	// Static gun
	StaticUnit GunHowitzer = {
		elHard = 100;

		AttackClass = UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_FRAME|UNIT_CLASS_GROUND;

		ReloadEnergy = 0.96;
		ReloadTime = 1500;
		ReloadPriority = 12;

		DischargeSpeed = 1;

		maPower = 40;
		maWidth = 40;

		saPower = 40;
		saWidth = 40;
		saRadius = 15;

		RangeFire = RADIUS_FAR;
		RangeSight = RADIUS_FAR +RADIUS_ADD;

		AccuracyRadius = 70;
	};

	// Empire special units
	SpecialUnit Leech = {
		elTechnicians = 4 * SPECIAL_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		AttackClass = UNIT_CLASS_STRUCTURE_CORE;

		ReloadEnergy = 0.16;
		ReloadTime = 5000;
		ReloadPriority = 12;

		DischargeSpeed = 0.15;

		RangeFire = RADIUS_FAR;
		RangeSight = RADIUS_FAR +RADIUS_ADD;

		StealEnergy = 5;
	};
	MobileUnit Unseen = {
		elSoldiers = 3 * SPECIAL_UNITS_EL_MUL;
		elTechnicians = 6 * SPECIAL_UNITS_EL_MUL;
	
		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		ReloadEnergy = 0.2;
		ReloadTime = 6000;
		ReloadPriority = 12;

		DischargeSpeed = 0.1;

		RangeFire = RADIUS_SHORT;
		RangeSight = RADIUS_SHORT;

		AccuracyRadius = RADIUS_SHORT;
	};
	MobileUnit Disintegrator = {
		elOfficers = 4 * SPECIAL_UNITS_EL_MUL;
		elTechnicians = 5 * SPECIAL_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		AttackClass = UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_HEAVY|UNIT_CLASS_UNDERGROUND|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_AIR;

		ReloadEnergy = 0.58;
		ReloadTime = 5000;
		ReloadPriority = 12;

		DischargeSpeed = 0.2;

		RangeFire = RADIUS_SHORT;
		RangeSight = RADIUS_SHORT +RADIUS_ADD;

		AccuracyRadius = 150;
	};
	MobileUnit Piercer = {
		elSoldiers = 1 * SPECIAL_UNITS_EL_MUL;
		elOfficers = 7 * SPECIAL_UNITS_EL_MUL;
		elTechnicians = 13 * SPECIAL_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL)/2;

		AttackClass = UNIT_CLASS_FRAME|UNIT_CLASS_GROUND|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		saPower = 750;
		saWidth = 60;
		saRadius = 70;

		ReloadEnergy = 1.16;
		ReloadTime = 3000;
		ReloadPriority = 12;

		RangeFire = RADIUS_SHORT;
		RangeSight = RADIUS_SHORT +RADIUS_ADD;

		AccuracyRadius = 500;
	};
	// Exodus special units
	MobileUnit ScumSplitter = {
		elTechnicians = 4 * SPECIAL_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		AttackClass = UNIT_CLASS_FRAME|UNIT_CLASS_GROUND|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		ReloadEnergy = 0.16;
		ReloadTime = 20000;
		ReloadPriority = 12;

		DischargeSpeed = 0.5;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;

		AccuracyRadius = 50;
	};
	MobileUnit ScumTwister = {
		elSoldiers = 2 * SPECIAL_UNITS_EL_MUL;
		elTechnicians = 7 * SPECIAL_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_GROUND|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_GROUND_FILTH|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_AIR_FILTH|UNIT_CLASS_FRAME;
//		AttackClass = UNIT_CLASS_ALL;

		ReloadEnergy = 0.21;
		ReloadTime = 10000;
		ReloadPriority = 12;

		DischargeSpeed = 0.05;

		maPower = 40;
		maWidth = 40;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;

		AccuracyRadius = 50;
	};
	MobileUnit ScumHeater = {
		elOfficers = 7 * SPECIAL_UNITS_EL_MUL;
		elTechnicians = 4 * SPECIAL_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_GROUND|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_GROUND_FILTH|UNIT_CLASS_UNDERGROUND|UNIT_CLASS_UNDERGROUND_FILTH|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_AIR_FILTH|UNIT_CLASS_FRAME;

		ReloadEnergy = 0.32;
		ReloadTime = 3000;
		ReloadPriority = 12;

		DischargeSpeed = 0.2;

		maPower = 50;
		maWidth = 50;

		RangeFire = RADIUS_SHORT;
		RangeSight = RADIUS_SHORT +RADIUS_ADD;

		AccuracyRadius = 100;
	};

	MobileUnit ScumThrower = {
		elSoldiers = 1 * SPECIAL_UNITS_EL_MUL;
		elOfficers = 6 * SPECIAL_UNITS_EL_MUL;
		elTechnicians = 13 * SPECIAL_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		AttackClass = UNIT_CLASS_GROUND|UNIT_CLASS_BUILDER|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_HEAVY|UNIT_CLASS_FRAME|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		ReloadEnergy = 0.94;
		ReloadTime = 2500;
		ReloadPriority = 12;

		DischargeSpeed = 10;

		maPower = 200;
		maWidth = 200;
		saPower = 200;
		saWidth = 200;
		saRadius = 50;

		RangeFire = RADIUS_FAR;
		RangeSight = RADIUS_FAR +RADIUS_ADD;

		AccuracyRadius = 50;
	};

	// Electro special
	
	MobileUnit Impaler = {
		elOfficers = 4 * SPECIAL_ELECTRO_UNITS_EL_MUL;
		elTechnicians = 2 * SPECIAL_ELECTRO_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_FRAME;

		maPower = 40;
		maWidth = 40;

		ReloadEnergy = 0.2;
		ReloadTime = 2000;
		ReloadPriority = 10;

		DischargeSpeed = 1;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;
	};

	MobileUnit Eflair = {
		elOfficers = 4 * SPECIAL_ELECTRO_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_BLOCK|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_GROUND_FILTH|UNIT_CLASS_AIR_FILTH|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_FRAME;

		maPower = 25;
		maWidth = 25;

		ReloadEnergy = 0.1;
		ReloadTime = 1000;
		ReloadPriority = 10;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;
	};

	MobileUnit Conductor = {
		elSoldiers = 4 * SPECIAL_ELECTRO_UNITS_EL_MUL;
		elOfficers = 8 * SPECIAL_ELECTRO_UNITS_EL_MUL;
		elTechnicians = 2 * SPECIAL_ELECTRO_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_GROUND|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_GROUND_FILTH|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_AIR_FILTH|UNIT_CLASS_FRAME;

		ReloadEnergy = 0.3;
		ReloadTime = 4000;
		ReloadPriority = 10;

		DischargeSpeed = 0.5;

		maPower = 80;
		maWidth = 80;

		RangeFire = RADIUS_SHORT;
		RangeSight = RADIUS_SHORT +RADIUS_ADD;

		AccuracyRadius = 100;
	};

	StaticUnit GunElectro = {
		elHard = 100;

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_BLOCK|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_GROUND_FILTH|UNIT_CLASS_AIR_FILTH|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY;

		ReloadEnergy = 0.1;
		ReloadTime = 500;
		ReloadPriority = 10;

		DischargeSpeed = 0.2;

		maPower = 25;
		maWidth = 25;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;
	};

	// Harkbakchood special units

	ScourgeUnit Scourge0 = {
		elTechnicians = 4 * SPECIAL_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		AttackClass = UNIT_CLASS_GROUND|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		ReloadEnergy = 0.31;
		ReloadTime = 10000;
		ReloadPriority = 12;

		DischargeSpeed = 2;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;

		Type = FILTH_SPOT_ID_SNAKE;
		
		Name = "Scourge Spider";

		Number = 5;
		LifeTime = 20;

		AccuracyRadius = 50;
	};
	ScourgeUnit Scourge1 = {
		elOfficers = 3 * SPECIAL_UNITS_EL_MUL;
		elTechnicians = 5 * SPECIAL_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		AttackClass = UNIT_CLASS_FRAME|UNIT_CLASS_GROUND|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		ReloadEnergy = 0.31;
		ReloadTime = 10000;
		ReloadPriority = 12;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;

		Type = FILTH_SPOT_ID_SHARK;

		Name = "Scourge Shark";

		Number = 2;
		LifeTime = 10;

		AccuracyRadius = 50;
	};
	ScourgeUnit Scourge2 = {
		elOfficers = 4 * SPECIAL_UNITS_EL_MUL;
		elTechnicians = 5 * SPECIAL_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		AttackClass = UNIT_CLASS_FRAME|UNIT_CLASS_GROUND|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		ReloadEnergy = 0.31;
		ReloadTime = 20000;
		ReloadPriority = 12;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;

		Type = FILTH_SPOT_ID_DAEMON;
		
		Name = "Scourge Daemon";

		Number = 3;
		LifeTime = 20;

		AccuracyRadius = 50;
	};
	ScourgeUnit Scourge3 = {
		elSoldiers = 1 * SPECIAL_UNITS_EL_MUL;
		elOfficers = 7 * SPECIAL_UNITS_EL_MUL;
		elTechnicians = 13 * SPECIAL_UNITS_EL_MUL;

		elShield = ((elSoldiers + elOfficers + elTechnicians)*SHIELD_SPECIAL_MUL);

		AttackClass = UNIT_CLASS_FRAME|UNIT_CLASS_GROUND|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		ReloadEnergy = 0.31;
		ReloadTime = 20000;
		ReloadPriority = 12;

		RangeFire = RADIUS_MIDDLE;
		RangeSight = RADIUS_MIDDLE +RADIUS_ADD;

		Type = FILTH_SPOT_ID_DRAGON;
		
		Name = "Scourge Dragon";

		Number = 1;
		LifeTime = 20;

		AccuracyRadius = 100;
	};

	// Scourge units
	MobileUnit Ants = {
		//FILTH_SPOT_ID_ANTS

		elTechnicians	= 1;

		AttackClass = UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL;

		maPower = 15;
		maWidth = 20;
	};
	MobileUnit Ants2 = {
		//FILTH_SPOT_ID_ANTS2

		elTechnicians	= 15;

		AttackClass = UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		maPower = 15;
		maWidth = 20;
	};
	MobileUnit Rat = {
		//FILTH_SPOT_ID_RAT

		elTechnicians = 5;

		AttackClass = UNIT_CLASS_TRUCK|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL;

		maPower = 100;
		maWidth = 100;
	};
	MobileUnit Snake = {
		elTechnicians = 100;

		AttackClass = UNIT_CLASS_BUILDER|UNIT_CLASS_TRUCK|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		maPower = 50;
		maWidth = 50;
	};
	MobileUnit Worm = {
		//FILTH_SPOT_ID_WORM
		elTechnicians = 100;

		AttackClass = UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL;

		maPower = 50;
		maWidth = 100;
	};
	MobileUnit Wasp = {
		//FILTH_SPOT_ID_WASP

		elTechnicians = 15;

		AttackClass = UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_FRAME|UNIT_CLASS_STRUCTURE_CORE;

		maPower = 130;
		maWidth = 130;
	};
	MobileUnit Ghost = {
		//FILTH_SPOT_ID_GHOST

		elTechnicians = 5;

		AttackClass = UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_STRUCTURE_ENVIRONMENT|UNIT_CLASS_FRAME;

		maPower = 10;
		maWidth = 10;
		saPower = 5;
		saWidth = 10;
		saRadius = 10;
	};
	MobileUnit Eye = {
		//FILTH_SPOT_ID_EYE

		elTechnicians = 20;

		AttackClass = UNIT_CLASS_FRAME|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL;

		maPower = 200;
		maWidth = 200;
		saPower = 200;
		saWidth = 200;
		saRadius = 20;
	};
	MobileUnit Crow = {
		//FILTH_SPOT_ID_CROW

		elTechnicians = 20;

		AttackClass = UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_STRUCTURE_CORE;

		maPower = 50;
		maWidth = 50;
		saPower = 20;
		saWidth = 50;
		saRadius = 20;
	};
	MobileUnit Daemon = {
		//FILTH_SPOT_ID_DAEMON

		elTechnicians = 20;

		AttackClass = UNIT_CLASS_STRUCTURE_CORE|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_FRAME;

		maPower = 50;
		maWidth = 50;
	};
	MobileUnit DragonHead = {
		//FILTH_SPOT_ID_DRAGON

		elTechnicians = 200;

		AttackClass = UNIT_CLASS_STRUCTURE_CORE|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_FRAME|UNIT_CLASS_AIR|UNIT_CLASS_BUILDER|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		maPower = 200;
		maWidth = 200;
	};
	MobileUnit DragonBody = {
		elTechnicians = 100;

		maPower = 50;
		maWidth = 50;
		
		saPower = 3;
		saWidth = 10;
	};
	MobileUnit DragonTail = {
		elTechnicians = 100;

		maPower = 80;
		maWidth = 80;
		
		saPower = 3;
		saWidth = 10;
	};
	MobileUnit Dragon2Head = {
		//FILTH_SPOT_ID_DRAGON

		elTechnicians = 50;

		AttackClass = UNIT_CLASS_STRUCTURE_CORE|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_FRAME|UNIT_CLASS_AIR|UNIT_CLASS_BUILDER|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		maPower = 10;
		maWidth = 10;
	};
	MobileUnit Dragon2Body = {
		elTechnicians = 50;

		maPower = 25;
		maWidth = 25;
		
		saPower = 3;
		saWidth = 15;
	};
	MobileUnit Dragon2Tail = {
		elTechnicians = 50;

		maPower = 20;
		maWidth = 20;

		saPower = 3;
		saWidth = 15;
	};
	MobileUnit Shark = {
		//FILTH_SPOT_ID_SHARK

		elTechnicians = 10;

		AttackClass = UNIT_CLASS_FRAME|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_STRUCTURE_ENVIRONMENT;

		maPower = 20;
		maWidth = 20;
		saPower = 10;
		saWidth = 20;
		saRadius = 20;
	};
	MobileUnit Volcano = {
		//FILTH_SPOT_ID_VOLCANO

//		AttackClass = UNIT_CLASS_BASE|UNIT_CLASS_LIGHT|UNIT_CLASS_MEDIUM|UNIT_CLASS_HEAVY|UNIT_CLASS_AIR|UNIT_CLASS_AIR_HEAVY|UNIT_CLASS_UNDERGROUND|UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_STRUCTURE_CORE|UNIT_CLASS_FRAME|UNIT_CLASS_BUILDER|UNIT_CLASS_TRUCK|UNIT_CLASS_BLOCK;
		AttackClass = UNIT_CLASS_ALL;

		elTechnicians = 5;

		maPower = 200;
		maWidth = 200;
//		saPower = 150;
//		saWidth = 200;
//		saRadius = 30;
	};

	MobileUnit VolcanoScumDisruptor = {
		AttackClass = UNIT_CLASS_ALL;

		elTechnicians = 5;

		maPower = 150;
		maWidth = 200;
		saPower = 150;
		saWidth = 200;
		saRadius = 30;
	};

	StaticUnit GunBallistic = {
		elHard = 1000;

		AttackClass = UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_GROUND|UNIT_CLASS_FRAME|UNIT_CLASS_STRUCTURE_ENVIRONMENT;
//		AttackClass = UNIT_CLASS_ALL;

		ReloadEnergy = 10;
		ReloadTime = 100000;
		ReloadPriority = 200;

		DischargeSpeed = 10000;

		maPower = 4000;
		maWidth = 4000;
		saPower = 3000;
		saWidth = 3000;
		saRadius = 200;

		RangeFire = RADIUS_SUPER_FAR;
		RangeSight = RADIUS_SUPER_FAR;
	};
	StaticUnit GunScumDisruptor = {
		elHard = 1000;

		AttackClass = UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_GROUND|UNIT_CLASS_FRAME;
//		AttackClass = UNIT_CLASS_ALL;

		ReloadEnergy = 10;
		ReloadTime = 100000;
		ReloadPriority = 200;

		DischargeSpeed = 50;

//		maPower = 2;
//		maWidth = 10;
//		saPower = 2;
//		saWidth = 20;
//		saRadius = 30;

		RangeFire = RADIUS_SUPER_FAR;
		RangeSight = RADIUS_SUPER_FAR;
	};
	StaticUnit GunScourgeNavigator = {
		elHard = 1000;

		AttackClass = UNIT_CLASS_STRUCTURE|UNIT_CLASS_STRUCTURE_GUN|UNIT_CLASS_STRUCTURE_SPECIAL|UNIT_CLASS_GROUND|UNIT_CLASS_FRAME;
//		AttackClass = UNIT_CLASS_ALL;

		ReloadEnergy = 10;
		ReloadTime = 100000;
		ReloadPriority = 200;

		DischargeSpeed = 20;

		RangeFire = RADIUS_SUPER_FAR;
		RangeSight = RADIUS_SUPER_FAR;
	};

	ScourgeUnit ScourgeNavigatorPhase0 = {
		Type = FILTH_SPOT_ID_RAT;
		Number = 50;
		LifeTime = 20;
	};
	ScourgeUnit ScourgeNavigatorPhase1 = {
		Type = FILTH_SPOT_ID_EYE;
		Number = 25;
		LifeTime = 20;
	};
	ScourgeUnit ScourgeNavigatorPhase2 = {
		Type = FILTH_SPOT_ID_DAEMON;
		Number = 12;
		LifeTime = 20;
	};
	ScourgeUnit ScourgeNavigatorPhase3 = {
		Type = FILTH_SPOT_ID_WORM;
		Number = 1;
		LifeTime = 40;
	};
};

static Balance balance = {};

//static float aaaa = balance.unit1.damage; // использование
