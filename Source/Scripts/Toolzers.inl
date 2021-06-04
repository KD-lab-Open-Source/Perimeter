
// параметры воздействия тулзера
struct ToolzerActionData
{
	// тип воздействия
	ToolzerActionID actionID = TOOLZER_4ZP;

	// int influenceDZ(int x, int y, int rad, short dh, int smMode);
	// dh = dz * (1 << VX_FRACTION)

	int rad = 10;
	float dz = 1;

	int smMode = 5;

	// hAppr < 0: individualToolzer::hAppr = vMap.hZeroPlast << VX_FRACTION
	// hAppr >= 0: individualToolzer::hAppr = hAppr << VX_FRACTION
	int hAppr = -1;
};

// параметры воздействий тулзера в определенной фазе
struct ToolzerStepData
{
	ToolzerPhaseID phaseID = TOOLZER_PHASE_DEFAULT;

	// время работы шага в логических квантах
	int duration = 1;

	// следующая фаза
	// TOOLZER_PHASE_DEFAULT - переходить к следующей по порядку
	// TOOLZER_PHASE_NONE - остановить работу тулзера
	ToolzerPhaseID nextPhaseID = TOOLZER_PHASE_DEFAULT;

	// минимальное изменение координат тулзера для наложнения воздействия
	//
	// сделано, чтобы избежать наваливания куч до небес когда тулзер стоит
	// или движется медленно
	float dr = 0;

	// повреждения зданиям, режим и радиус
	ToolzerBuildingDamageMode buildingDamageMode = BUILDING_DAMAGE_SQUARE;
	int buildingDamageRadius = 0;
	
	ToolzerActionData* actions = new ToolzerActionData[int size] {};

delegate:

	const ToolzerActionData& operator[](int idx) const { xassert(idx >= 0 && idx < size); return actions[idx]; }
};

// параметры тулзера
struct ToolzerSetup
{
	ToolzerStepData* steps = new ToolzerStepData[int size] {};

	// коэфф. масштабирования тулзера
	float scale = 1.0f;

	// = 1 если тулзер рушит собственный зеро-слой
	int destroyOwnZeroLayer = 0;

	// = 1 если работает на хаосе
	int workOnChaos = 1;

	// радиус для пересчёта масштаба
	float radius = 48;

delegate:

	int stepIndex(ToolzerPhaseID phase) const
	{
		for(int i = 0; i < size; i++){
			if(steps[i].phaseID == phase)
				return i;
		}

		return -1;
	}

	const ToolzerStepData& step(int idx) const { xassert(idx >= 0 && idx < size); return steps[idx]; }
};

ToolzerSetup scumerToolzer = 
{
	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_START_MOVE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 1; dz = -2; smMode = 2; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 2; dz = -2; smMode = 2; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 3; dz = -2; smMode = 2; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 4; dz = -2; smMode = 2; } };
		},
		{ 
			phaseID = TOOLZER_PHASE_MOVE; nextPhaseID = TOOLZER_PHASE_MOVE; dr = 5;
			buildingDamageRadius = 3;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 3; dz = 0.1; smMode = 2; } };
		},
		{ 
			duration = 10; phaseID = TOOLZER_PHASE_END_MOVE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NONE; } };
		},
		{ 
			duration = 2; nextPhaseID = TOOLZER_PHASE_NONE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 8; dz = -1; smMode = 5; } };
		}
	};
};

ToolzerSetup diggerToolzer = 
{
	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_START_MOVE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 2; dz = -1; smMode = 2; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 3; dz = -1; smMode = 2; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 4; dz = -1; smMode = 2; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 5; dz = -1; smMode = 2; } };
		},
		{ 
			phaseID = TOOLZER_PHASE_MOVE; nextPhaseID = TOOLZER_PHASE_MOVE; dr = 5;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 2; dz = -0.1; smMode = 5; } };
		},
		{ 
			duration = 10; phaseID = TOOLZER_PHASE_END_MOVE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NONE; } };
		},
		{ 
			duration = 2; nextPhaseID = TOOLZER_PHASE_NONE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 4; dz = -1; smMode = 5; } };
		}
	};
};

ToolzerSetup HeavyUnitTrackToolzer = 
{
	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_START_MOVE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 2; dz = -2; smMode = 2; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 6; dz = -2; smMode = 2; } };
		},
		{ 
			phaseID = TOOLZER_PHASE_MOVE; dr = 5;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 6; dz = -1; smMode = 5; } };
		},
		{ 
			nextPhaseID = TOOLZER_PHASE_MOVE; dr = 5;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 8; dz = 1; smMode = 3; } };
		},
		{ 
			duration = 10; phaseID = TOOLZER_PHASE_END_MOVE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NONE; } };
		},
		{ 
			duration = 2; nextPhaseID = TOOLZER_PHASE_NONE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 8; dz = -1; smMode = 5; } };
		}
	};
};

ToolzerSetup MediumUnitTrackToolzer = 
{
	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_START_MOVE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 2; dz = -1; smMode = 2; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 3; dz = -1; smMode = 2; } };
		},
		{ 
			phaseID = TOOLZER_PHASE_MOVE; dr = 5;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 4; dz = -1; smMode = 5; } };
		},
		{ 
			nextPhaseID = TOOLZER_PHASE_MOVE; dr = 5;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 5; dz = 1; smMode = 3; } };
		},
		{ 
			duration = 10; phaseID = TOOLZER_PHASE_END_MOVE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NONE; } };
		},
		{ 
			duration = 2; nextPhaseID = TOOLZER_PHASE_NONE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 5; dz = -1; smMode = 5; } };
		}
	};
};

ToolzerSetup piercerToolzer = 
{
	workOnChaos = 0;

	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_START_MOVE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 6; dz = -3; smMode = 2; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 7; dz = -3; smMode = 2; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 8; dz = -3; smMode = 2; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 10; dz = -3; smMode = 2; } };
		},
		{ 
			phaseID = TOOLZER_PHASE_MOVE; nextPhaseID = TOOLZER_PHASE_MOVE; dr = 5;
			buildingDamageRadius = 8;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 8; dz = 0.5; smMode = 3; } };
		},
		{ 
			duration = 10; phaseID = TOOLZER_PHASE_END_MOVE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NONE; } };
		},
		{ 
			duration = 2; nextPhaseID = TOOLZER_PHASE_NONE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 16; dz = 1; smMode = 5; } };
		}
	};
};

ToolzerSetup scumMissileCraterToolzer = 
{
	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_START_MOVE;
			buildingDamageRadius = 25;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 25; dz = 5; smMode = 5; } };
		}
	};
};

ToolzerSetup bombMissileCraterToolzer = 
{
	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_START_MOVE;
			buildingDamageRadius = 5;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 48; dz = -1; smMode = 2; } };
		}
	};
};

ToolzerSetup bombBombieMissileCraterToolzer = 
{
	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_START_MOVE;
			buildingDamageRadius = 25;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 48; dz = -1; smMode = 2; } };
		}
	};
};

ToolzerSetup rocketMissileCraterToolzer = 
{
	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_START_MOVE;
			buildingDamageRadius = 8;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 48; dz = -0.1; smMode = 2; } };
		}
	};
};

ToolzerSetup volcanoCraterToolzer = 
{
	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_START_MOVE;
			buildingDamageRadius = 15;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 8; dz = 1; smMode = 5; } };
		}
	};

	scale=1;
};

ToolzerSetup extirpatorMissileToolzer = 
{
	destroyOwnZeroLayer = 0;

	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_MOVE; dr = 5;
			buildingDamageRadius = 2;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 3; dz = -0.1; smMode = 3; } };
		},
		{ 
			nextPhaseID = TOOLZER_PHASE_MOVE; dr = 5;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 3; dz = 0.1; smMode = 3; } };
		}
	};
};

ToolzerSetup subchaserMissileToolzer = 
{
	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_MOVE; nextPhaseID = TOOLZER_PHASE_MOVE; dr = 5;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 4; dz = 0.1; smMode = 3; } };
		}
	};
};

ToolzerSetup scumTwisterCraterToolzer = 
{
	steps = new ToolzerStepData[]
	{
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 12; dz = -1; smMode = 3; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 8; dz = -1; smMode = 3; } };
		},
		{ 
			buildingDamageRadius = 12;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 12; dz = 0; smMode = 2; } };
		}
	};
};

ToolzerSetup scumHeaterCraterToolzer = 
{
	workOnChaos = 0;

	steps = new ToolzerStepData[]
	{
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 20; dz = -1; smMode = 6; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 18; dz = 1; smMode = 3; } };
		},
		{ 
			buildingDamageRadius = 20; nextPhaseID = TOOLZER_PHASE_NONE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 20; dz = 0; smMode = 2; } };
		}
	};
};

ToolzerSetup scumPiercerCraterToolzer = 
{
	workOnChaos = 0;
	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_MOVE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 40; dz = -0.1; smMode = 6; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 50; dz = 0.1; smMode = 3; } };
		},
		{ 
			buildingDamageRadius = 50; phaseID = TOOLZER_PHASE_MOVE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 50; dz = 0; smMode = 6; } };
		}
	};
};

ToolzerSetup FrameKaputCraterToolzer = 
{
	destroyOwnZeroLayer = 1;
	steps = new ToolzerStepData[]
	{
/*
		{ 
			phaseID = TOOLZER_PHASE_MOVE;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 20; dz = -0.5; smMode = 6; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 40; dz = 0.1; smMode = 3; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 50; dz = 0.1; smMode = 2; } };
		},
		{ 
			buildingDamageRadius = 50; 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 50; dz = 0; smMode = 6; } };

			nextPhaseID = TOOLZER_PHASE_MOVE;
			duration = 5;
		}
*/
		{ 
			buildingDamageRadius = 50; 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 50; dz = -5; smMode = 6; } };

			nextPhaseID = TOOLZER_PHASE_NONE;
		}
	};
};

ToolzerSetup piercerMissileToolzer = 
{
	steps = new ToolzerStepData[]
	{
		{ 
			phaseID = TOOLZER_PHASE_MOVE; nextPhaseID = TOOLZER_PHASE_MOVE; dr = 2;
			buildingDamageRadius = 50;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 50; dz = 0.3; smMode = 5; } };
		}
	};
};

ToolzerSetup scumDisruptorCraterToolzer = 
{
	steps = new ToolzerStepData[]
	{
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 32; dz = -2; smMode = 3; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 28; dz = -2; smMode = 3; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 24; dz = -2; smMode = 4; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 20; dz = -2; smMode = 3; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 16; dz = -2; smMode = 4; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 12; dz = -2; smMode = 3; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 8; dz = -2; smMode = 4; } };
		},
		{ 
			buildingDamageRadius = 50;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 32; dz = 0; smMode = 2; } };
		}
	};
};

ToolzerStepData* craterToolzerSteps = new ToolzerStepData[] {
	{ 
		actions = new ToolzerActionData[] {
			{ actionID = TOOLZER_H; rad = 32; dz = 2; smMode = 3; } };
	},
	{ 
		actions = new ToolzerActionData[] {
			{ actionID = TOOLZER_H; rad = 24; dz = -4; smMode = 3; } };
	},
	{ 
		actions = new ToolzerActionData[] {
			{ actionID = TOOLZER_H; rad = 16; dz = -4; smMode = 4; } };
	},
	{ 
		actions = new ToolzerActionData[] {
			{ actionID = TOOLZER_H; rad = 32; dz = 0; smMode = 2; } };
	},
	{ 
		actions = new ToolzerActionData[] {
			{ actionID = TOOLZER_S; rad = 27; dz = 1; smMode = 10; } };
	},
	{ 
		buildingDamageRadius = 32;
		actions = new ToolzerActionData[] {
			{ actionID = TOOLZER_S; rad = 38; dz = 1; smMode = 7; } };
	}
};


ToolzerSetup destructionCraterToolzer = 
{
	destroyOwnZeroLayer = 1;

	steps = new ToolzerStepData[]
	{
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 32; dz = 2; smMode = 3; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 24; dz = -4; smMode = 3; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 16; dz = -4; smMode = 4; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_H; rad = 32; dz = 0; smMode = 2; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_S; rad = 27; dz = 1; smMode = 10; } };
		},
		{ 
			buildingDamageRadius = 32;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_S; rad = 38; dz = 1; smMode = 7; } };
		}
	};
};

// кратер, не разрушающий свой зеро-слой
ToolzerSetup destructionCraterToolzer_NOZP = destructionCraterToolzer 
{ 
	destroyOwnZeroLayer = 0;
};


ToolzerSetup destructionCraterToolzerNoDestructAllZeroplast = 
{
	destroyOwnZeroLayer = 1;

	steps = new ToolzerStepData[]
	{
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 32; dz = 2; smMode = 3; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 24; dz = -4; smMode = 3; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 16; dz = -4; smMode = 4; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_NZP; rad = 32; dz = 0; smMode = 2; } };
		},
		{ 
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_S; rad = 27; dz = 1; smMode = 10; } };
		},
		{ 
		//	buildingDamageRadius = 32;
			actions = new ToolzerActionData[] {
				{ actionID = TOOLZER_S; rad = 38; dz = 1; smMode = 7; } };
		}
	};
};
