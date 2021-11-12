#include "GameBalance.inl"

//------------------------------------------------------
struct ConsumptionData
{
	float energy = 0;
	int time = 100; // in milliseconds
	int priority = 0;
	string name = "noname";
};

//-------------------------
struct FieldPrm
{
	int monksPerCore = 20; 
	int monksPerCoreMin = 3;
	float monksVelocity = 5; // Скорость при нормальных условиях
	float monksAccelerationFactor = 0.2; 
	float monksPositionFactor = 0.1f;
	int monksWetPlace = 8; // Радиус выравниваемой поверхности при смерти молельщика
	float chargeLevelToStartMin = 0.1; // Ядро с таким уровнем зарядки включится при тотальном режиме
	ConsumptionData fieldConsumption = {
		energy = 1.0;
		priority = 20;
		name = "fieldConsumption";
	};
	float checkFieldRadiusMin = 8;
	DamageData fieldDamage = {
		power = 20;
		width = 40;
	};
	int impulseDuration = 2000; 
	DamageData impulseFieldDamage = {
		power = 20;
		width = 40;
	};
};

FieldPrm fieldPrm = {};

//-------------------------
struct DebugScales
{
	float frame = 1; // фрейм
	float legion = 0.5; // легион
	float buildins = 0.5; // здания
	float spheres = 0.5; // капли
	float other = 0.5; // остальные
};

DebugScales debuScales = {}; // Отладочное масштабирование

//------------------------------------------------------
struct CorridorPrm
{
	int disappearingTime = 2000;
	int disappearingHeight = 200;
	float distanceThreshould = 30;
	int moveFrame = 1;

	float potentialToAcceleration = rigidBodyTable.CorridorAlphaPrm.forward_acceleration;
	float accelerationDenominator = 400;
	float pathVisualizationDelta = 0.05;
};

struct FramePrm
{
	int heightMax = 30;
	float accumulatedEnergyInitial = 1; // 0..1, только для вновь установленного фрейма, иначе - в редакторе миссий
	ConsumptionData antigravConsumption = {
		energy = 0.07;
		priority = 0;
		name = "antigravConsumption";
	};
	ConsumptionData movementConsumption = {
		energy = 2.0;
		priority = 111;
		name = "movementConsumption";
	};
	ConsumptionData annihilationConsumption = {
		energy = 50;
		priority = 150;
		name = "annihilationConsumption";
	};
	ConsumptionData repairConsumption = {
		energy = 1.0;
		priority = 111;
		name = "repairConsumption";
	};
	float repairElementsPerQuant = FrameRepairElementsPerQuant;

	DamageData fallDamage = {
		width = 100;
		power = 100;
	};

	float oneStepMovement = 5;
	
	float energyReserveToChargeSpiral = 1000;
	
	float mmpHomePositionRadius = 200;
	
	float normalVelocity = 10;
	float teleportationVelocity = 30;
};

//--------------------------
struct SquadPrm
{
	int baseUnitsMax = 250;
	float homePositionOffsetFactor[2] = { 1.2, 0 };
	float formationRadiusBase = 10;
	float followDistanceFactor = 1; // followDistanceFactor*(radius1 + radius2)

	// квадрат радиуса, в котором базовый сквад
	// ремонтирует и защищает другие сквады своими техниками
	float supportRadius = 200 * 200;
};

SquadPrm squadPrm = {};

//--------------------------
ConsumptionData buildingBlockConsumption = {
	energy = 2;
	time = 1000; 
	priority = 30;
	name = "buildingBlockConsumption";
};

float sellBuildingEfficiency = 0.5; // Возвращаемая при продаже энергия

//-----------------------------
struct DifficultyPrm
{
	string name = ""; 

	//filthDensity - влияет на количество генерируемой скверны и период генерации.
	//mul=1 - все значения без изменения.
	//При уменьшении - уменьшается сначало количество скверны, когда
	//количество скверны уменьшится до 1, начинает увеличиваиться период генерации.
	float filthDensity = 1;
	
	int aiDelay = 0; // Задержка АИ
	
	float triggerDelayFactor = 1; // Коэффициент триггера задержка
};

DifficultyPrm difficultyPrmArray[DIFFICULTY_MAX] = 
{
	{ // EASY
		name = "AI Easy";
		filthDensity = 0.3;
		aiDelay = 20000;
		triggerDelayFactor = 3;
	},
	{ // NORMAL
		name = "AI Normal";
		filthDensity = 0.6;
		aiDelay = 10000;
		triggerDelayFactor = 2;
	},
	{ // HARD
		name = "AI Hard";
		filthDensity = 1;
		aiDelay = 0;
		triggerDelayFactor = 1;
	}
};


//-----------------------------
//Параметры настраивающие ковшей
struct TrucksIntrumentParameter {
	float kRadius4DigZL=1.2;
	float kHeigh4DigZL=1.2;

	float kRadius4PutZL=1.2;
	float kHeigh4PutZL=1.2;

	float kRadius4DigGarbage=1.2;
	float kHeigh4DigGarbage=1.2;

	float kRadius4PutGarbage=1.2;
	float kHeigh4PutGarbage=1.2;
};
TrucksIntrumentParameter trucksIntrumentParameter={};

//-----------------------------
struct SoundEventsPrm
{
	float collectorEmptyThreshold = 10;
	float energyLosingThreshold = 2;
	float energyNotEnoughThreshold = 10;
	float energyNotEnoughDischargeThreshold = 1.01;
};
SoundEventsPrm soundEventsPrm = {};

//-----------------------------
struct BelligerentProperty
{
	terBelligerent belligerent = BELLIGERENT_NONE;
	int colorIndex = 0;

	struct SoundTrackData
	{
		string trackName = ""; 
		string fileName = ""; 
	};
	SoundTrackData soundTracks[int soundTracksSize = 3] = {};
};

struct BelligerentPropertyTable
{
	BelligerentProperty data[int size] = 
	{
		{
			belligerent = BELLIGERENT_EXODUS0;
			colorIndex = 0;

			soundTracks = 
			{
				{
					trackName = "construction";
					fileName = "RESOURCE\\MUSIC\\Exodus_Primary.ogg";
				},
				{
					trackName = "battle";
					fileName = "RESOURCE\\MUSIC\\Exodus_Military.ogg";
				},
				{
					trackName = "regular";
					fileName = "RESOURCE\\MUSIC\\Exodus_Covered.ogg";
				}
			};
		},

		{
			belligerent = BELLIGERENT_EXODUS1;
			colorIndex = 1;

			soundTracks = 
			{
				{
					trackName = "construction";
					fileName = "RESOURCE\\MUSIC\\Exodus_Primary.ogg";
				},
				{
					trackName = "battle";
					fileName = "RESOURCE\\MUSIC\\Exodus_Military.ogg";
				},
				{
					trackName = "regular";
					fileName = "RESOURCE\\MUSIC\\Exodus_Covered.ogg";
				}
			};
		},

		{
			belligerent = BELLIGERENT_HARKBACKHOOD0;
			colorIndex = 5;

			soundTracks = 
			{
				{
					trackName = "construction";
					fileName = "RESOURCE\\MUSIC\\Harkbackhood_Primary.ogg";
				},
				{
					trackName = "battle";
					fileName = "RESOURCE\\MUSIC\\Harkbackhood_Military.ogg";
				},
				{
					trackName = "regular";
					fileName = "RESOURCE\\MUSIC\\Harkbackhood_Covered.ogg";
				}
			};
		},

		{
			belligerent = BELLIGERENT_HARKBACKHOOD1;
			colorIndex = 4;

			soundTracks = 
			{
				{
					trackName = "construction";
					fileName = "RESOURCE\\MUSIC\\Harkbackhood_Primary.ogg";
				},
				{
					trackName = "battle";
					fileName = "RESOURCE\\MUSIC\\Harkbackhood_Military.ogg";
				},
				{
					trackName = "regular";
					fileName = "RESOURCE\\MUSIC\\Harkbackhood_Covered.ogg";
				}
			};
		},

		{
			belligerent = BELLIGERENT_EMPIRE0;
			colorIndex = 2;

			soundTracks = 
			{
				{
					trackName = "construction";
					fileName = "RESOURCE\\MUSIC\\Empire_Primary.ogg";
				},
				{
					trackName = "battle";
					fileName = "RESOURCE\\MUSIC\\Empire_Military.ogg";
				},
				{
					trackName = "regular";
					fileName = "RESOURCE\\MUSIC\\Empire_Covered.ogg";
				}
			};
		},

		{
			belligerent = BELLIGERENT_EMPIRE1;
			colorIndex = 3;

			soundTracks = 
			{
				{
					trackName = "construction";
					fileName = "RESOURCE\\MUSIC\\Empire_Primary.ogg";
				},
				{
					trackName = "battle";
					fileName = "RESOURCE\\MUSIC\\Empire_Military.ogg";
				},
				{
					trackName = "regular";
					fileName = "RESOURCE\\MUSIC\\Empire_Covered.ogg";
				}
			};
		},

		{
			belligerent = BELLIGERENT_EMPIRE_VICE;
			colorIndex = 2;

			soundTracks = 
			{
				{
					trackName = "construction";
					fileName = "RESOURCE\\MUSIC\\Empire_Primary.ogg";
				},
				{
					trackName = "battle";
					fileName = "RESOURCE\\MUSIC\\Empire_Military.ogg";
				},
				{
					trackName = "regular";
					fileName = "RESOURCE\\MUSIC\\Empire_Covered.ogg";
				}
			};
		},

		// АДДОН

		{
			belligerent = BELLIGERENT_EXODUS2;
			colorIndex = 0;

			soundTracks = 
			{
				{
					trackName = "construction";
					fileName = "RESOURCE\\MUSIC\\Exodus_Primary.ogg";
				},
				{
					trackName = "battle";
					fileName = "RESOURCE\\MUSIC\\Exodus_Military.ogg";
				},
				{
					trackName = "regular";
					fileName = "RESOURCE\\MUSIC\\Exodus_Covered.ogg";
				}
			};
		},

		{
			belligerent = BELLIGERENT_EXODUS3;
			colorIndex = 1;

			soundTracks = 
			{
				{
					trackName = "construction";
					fileName = "RESOURCE\\MUSIC\\Exodus_Primary.ogg";
				},
				{
					trackName = "battle";
					fileName = "RESOURCE\\MUSIC\\Exodus_Military.ogg";
				},
				{
					trackName = "regular";
					fileName = "RESOURCE\\MUSIC\\Exodus_Covered.ogg";
				}
			};
		},

		{
			belligerent = BELLIGERENT_EXODUS4;
			colorIndex = 2;

			soundTracks = 
			{
				{
					trackName = "construction";
					fileName = "RESOURCE\\MUSIC\\Exodus_Primary.ogg";
				},
				{
					trackName = "battle";
					fileName = "RESOURCE\\MUSIC\\Exodus_Military.ogg";
				},
				{
					trackName = "regular";
					fileName = "RESOURCE\\MUSIC\\Exodus_Covered.ogg";
				}
			};
		},

		{
			belligerent = BELLIGERENT_EMPIRE2;
			colorIndex = 2;

			soundTracks = 
			{
				{
					trackName = "construction";
					fileName = "RESOURCE\\MUSIC\\Empire_Primary.ogg";
				},
				{
					trackName = "battle";
					fileName = "RESOURCE\\MUSIC\\Empire_Military.ogg";
				},
				{
					trackName = "regular";
					fileName = "RESOURCE\\MUSIC\\Empire_Covered.ogg";
				}
			};
		},

		{
			belligerent = BELLIGERENT_EMPIRE3;
			colorIndex = 0;

			soundTracks = 
			{
				{
					trackName = "construction";
					fileName = "RESOURCE\\MUSIC\\Empire_Primary.ogg";
				},
				{
					trackName = "battle";
					fileName = "RESOURCE\\MUSIC\\Empire_Military.ogg";
				},
				{
					trackName = "regular";
					fileName = "RESOURCE\\MUSIC\\Empire_Covered.ogg";
				}
			};
		},

		{
			belligerent = BELLIGERENT_EMPIRE4;
			colorIndex = 1;

			soundTracks = 
			{
				{
					trackName = "construction";
					fileName = "RESOURCE\\MUSIC\\Empire_Primary.ogg";
				},
				{
					trackName = "battle";
					fileName = "RESOURCE\\MUSIC\\Empire_Military.ogg";
				},
				{
					trackName = "regular";
					fileName = "RESOURCE\\MUSIC\\Empire_Covered.ogg";
				}
			};
		}
	};

delegate:
	const BelligerentProperty& find(terBelligerent belligerent)
	{
		for(int i = 0; i < size; i++)
			if(data[i].belligerent == belligerent)
				return data[i];
		xassert(0);
		return data[0];
	}
};

BelligerentPropertyTable belligerentPropertyTable = {};

