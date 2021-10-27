
struct SoundControllerSetup
{
	SoundID ID = SOUND_NONE;

	string name = "";

	int cycled = 0;
};

struct SoundEventSetup
{
	SoundEventID ID = SOUND_EVENT_NONE;
	string name = "";

	// = 1 если звук на мире
	int is3D = 0;

	// = 1 если голосовое сообщение, вызывается только для активного игрока
	int isVoice = 0;

	// = 1 если событие должно звучать только для активного игрока
	int activePlayer = 0;

	// время в миллисекундах, в течении которого после 
	// старта звука нельзя запустить его ещё раз
	int pauseTime = 0;

	// задержка перед проигрыванием звука в миллисекундах
	int startDelay = 0;

	// максимальное время, которое событие будет стоять в очереди на проигрывание (для голосов)
	int queueTime = 2000;
};

struct SoundSetup
{
	SoundControllerSetup* sounds = new SoundControllerSetup[int sounds_size] {};
	SoundEventSetup* events = new SoundEventSetup[int events_size] {};

delegate:

	const SoundEventSetup* event(int idx) const {
		xassert(idx >= 0 && idx < events_size);
		return &events[idx];
	}

	int eventIndex(SoundEventID event_id) const {
		for(int i = 0; i < events_size; i++){
			if(events[i].ID == event_id)
				return i;
		}

		return -1;
	}

	const SoundEventSetup* findEvent(SoundEventID event_id) const 
	{
		for(int i = 0; i < events_size; i++){
			if(events[i].ID == event_id)
				return &events[i];
		}

		return NULL;
	}
};

SoundSetup rprojectorSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_RProjector"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_RProjector"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Missile_Explosion_Rocket_RProjector"; is3D = 1; }
	};
};

SoundSetup subchaserGunSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_RProjector"; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_SubterraGun"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_SubterraGun"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "SubchaserGun_Ready"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "SubchaserGun_Destroyed"; isVoice = 1; pauseTime = 1000; }				
	};
};

SoundSetup  ballisticLauncherSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Struct_Proc_Shot_Ballistic"; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_Ballistic"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_Ballistic"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "BallisticGun_Ready"; isVoice = 1; },
		{ ID = SOUND_VOICE_WEAPON_CHARGED; name = "BallisticGun_Charged"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "BallisticGun_Destroyed"; isVoice = 1; }
	};
};

SoundSetup filthNavigatorSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Struct_Proc_Shot_Navigator"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_Navigator"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_Navigator"; is3D = 1; },
		{ ID = SOUND_VOICE_WEAPON_CHARGED; name = "ScourgeNavigator_Charged"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "ScourgeNavigator_Ready"; isVoice = 1; pauseTime = 1000;},
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "ScourgeNavigator_Destroyed"; isVoice = 1; }
	};
};

SoundSetup scumDisruptorSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Struct_Proc_Shot_Disruptor"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_Disruptor"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_Disruptor"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "ScumDisruptor_Ready"; isVoice = 1; pauseTime = 1000;},
		{ ID = SOUND_VOICE_WEAPON_CHARGED; name = "ScumDisruptor_Charged"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "ScumDisruptor_Destroyed"; isVoice = 1; }
	};
};

SoundSetup howitzerGunSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Struct_Proc_Shot_HowitzerGun"; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_HowitzerGun"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_HowitzerGun"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "HowitzerGun_Ready"; isVoice = 1; pauseTime = 5000;},
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "HowitzerGun_Destroyed"; isVoice = 1; }
	};
};

SoundSetup rocketGunSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Struct_Proc_Shot_RocketGun"; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_RocketGun"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_RocketGun"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "RocketGun_Ready"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "RocketGun_Destroyed"; isVoice = 1; }
	};
};

SoundSetup laserGunSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Struct_Proc_Shot_LaserGun"; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_LaserGun"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_LaserGun"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "LaserGun_Ready"; isVoice = 1;},
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "LaserGun_Destroyed"; isVoice = 1; }
	};
};

SoundSetup electroGunSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Struct_Proc_Shot_LaserGun"; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_LaserGun"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_LaserGun"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "ElectroGun_Ready"; isVoice = 1;},
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "ElectroGun_Destroyed"; isVoice = 1; }
	};
};

SoundSetup bombieSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Bombie"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Bombie"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Missile_Explosion_Bomb_Bombie"; is3D = 1; }
	};
};

SoundSetup wargonSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Wargon"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Wargon"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_heavy"; is3D = 1; }
	};
};

SoundSetup extirpatorSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Extirpator"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Extirpator"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_heavy"; is3D = 1; }
	};
};

SoundSetup leechSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Leech"; cycled = 1; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Leech"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup scumThrowerSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Thrower"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Thrower"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_heavy"; is3D = 1; }
	};
};

SoundSetup straferSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Strafer"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Strafer"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup gyroidSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Gyroid"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Gyroid"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup eflairSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Gyroid"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Gyroid"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup ceptorSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Ceptor"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Ceptor"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup mortarSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Mortar"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Mortar"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_lite"; is3D = 1; }
	};
};

SoundSetup rockerSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Rocker"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Rocker"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_lite"; is3D = 1; }
	};
};

SoundSetup leamoSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Leamo"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Leamo"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_heavy"; is3D = 1; }
	};
};

SoundSetup impalerSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Leamo"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Leamo"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_heavy"; is3D = 1; }
	};
};

SoundSetup sniperSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Sniper"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Sniper"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_lite"; is3D = 1; }
	};
};

SoundSetup officerSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Officer"; },
		{ ID = SOUND_MOVE; name = "light_unit_move"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_lite"; is3D = 1; }
	};
};

SoundSetup technicSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Technician"; },
		{ ID = SOUND_MOVE; name = "light_unit_move"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_lite"; is3D = 1; }
	};
};

SoundSetup soldierSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "soldier_fire"; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Soldier"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_lite"; is3D = 1; }
	};
};

SoundSetup terrainMasterSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "move_brigadier"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_heavy"; is3D = 1; },
		{ ID = SOUND_VOICE_UNIT_READY; name = "Frame_Brigadier_Ready"; isVoice = 1; pauseTime = 5000;},
		{ ID = SOUND_VOICE_UNIT_SELECTED; name = "Brigadier_Selected"; isVoice = 1; pauseTime = 1000;},
		{ ID = SOUND_VOICE_UNIT_MOVE; name = "Brigadier_Moves"; isVoice = 1; pauseTime = 1000; },
		{ ID = SOUND_VOICE_MMP_BACK_TO_FRAME; name = "Brigadier_Back2Frame"; isVoice = 1; pauseTime = 1000;},
		{ ID = SOUND_VOICE_UNIT_UNDER_ATTACK; name = "Building_Under_Attack"; isVoice = 1; pauseTime = 20000;}
	};
};

SoundSetup buildMasterSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "move_build_master"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_heavy"; is3D = 1; },
		{ ID = SOUND_VOICE_UNIT_READY; name = "Frame_BMaster_Ready"; isVoice = 1; pauseTime = 5000;},
		{ ID = SOUND_VOICE_UNIT_SELECTED; name = "BMaster_Selected"; isVoice = 1; pauseTime = 1000;},
		{ ID = SOUND_VOICE_UNIT_MOVE; name = "BMaster_Moves"; isVoice = 1; pauseTime = 1000; },
		{ ID = SOUND_VOICE_MMP_BACK_TO_FRAME; name = "BMaster_Back2Frame"; isVoice = 1; pauseTime = 1000;},
		{ ID = SOUND_VOICE_UNIT_UNDER_ATTACK; name = "Building_Under_Attack"; isVoice = 1; pauseTime = 20000;}
	};
};

SoundSetup coreSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_Core"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_EnergyCore"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "ECore_Ready"; isVoice = 1;},
		{ ID = SOUND_VOICE_BUILDING_UNDER_ATTACK; name = "ECore_Under_Attack"; isVoice = 1; pauseTime = 20000; },
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "ECore_Destroyed"; isVoice = 1; pauseTime = 5000; }
	};
};

SoundSetup transmitterSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_Transmitter"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_Transmitter"; is3D = 1; }
	};
};

SoundSetup collectorSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_Collector"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_EnergyCollector"; is3D = 1; }
	};
};

SoundSetup soldierPlantSound = {
//	sounds = new SoundControllerSetup[]
//	{
//		{ ID = SOUND_PRODUCTION; name = "Struct_Proc_Prod_SoldierPlant"; cycled = 1; }
//	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_heavy"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_SoldierPlant"; is3D = 1; },
		{ ID = SOUND_VOICE_UNIT_UNDER_ATTACK; name = "Building_Under_Attack"; isVoice = 1; pauseTime = 20000;},
		{ ID = SOUND_VOICE_UNIT_DESTROYED; name = "Building_Destroyed"; isVoice = 1; pauseTime = 5000;}
	};
};

SoundSetup technicianPlantSound = {
//	sounds = new SoundControllerSetup[]
//	{
//		{ ID = SOUND_PRODUCTION; name = "Struct_Proc_Prod_TechnicPlant"; cycled = 1; }
//	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_heavy"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_TechnicPlant"; is3D = 1; },
		{ ID = SOUND_VOICE_UNIT_UNDER_ATTACK; name = "Building_Under_Attack"; isVoice = 1; pauseTime = 20000;},
		{ ID = SOUND_VOICE_UNIT_DESTROYED; name = "Building_Destroyed"; isVoice = 1; pauseTime = 5000;}
	};
};

SoundSetup officerPlantSound = {
//	sounds = new SoundControllerSetup[]
//	{
//		{ ID = SOUND_PRODUCTION; name = "Struct_Proc_Prod_OfficerPlant"; cycled = 1; }
//	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_heavy"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_OfficerPlant"; is3D = 1; },
		{ ID = SOUND_VOICE_UNIT_UNDER_ATTACK; name = "Building_Under_Attack"; isVoice = 1; pauseTime = 20000;},
		{ ID = SOUND_VOICE_UNIT_DESTROYED; name = "Building_Destroyed"; isVoice = 1; pauseTime = 5000;}
	};
};

SoundSetup commandCenterSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_CommandCenter"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_CommandCenter"; is3D = 1; }
	};
};

SoundSetup labRocketSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_RocketLab"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_RocketLab"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "Labor_Ready"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_UNDER_ATTACK; name = "Labor_Under_Attack"; isVoice = 1; pauseTime = 20000;},
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "Labor_Destroyed"; isVoice = 1; pauseTime = 5000;}
	};
};

SoundSetup labLaserSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_LaserLab"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_LaserLab"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "Labor_Ready"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_UNDER_ATTACK; name = "Labor_Under_Attack"; isVoice = 1; pauseTime = 20000;},
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "Labor_Destroyed"; isVoice = 1; pauseTime = 5000;}
	};
};

SoundSetup labElectroSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_LaserLab"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_LaserLab"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "Labor_Ready"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_UNDER_ATTACK; name = "Labor_Under_Attack"; isVoice = 1; pauseTime = 20000;},
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "Labor_Destroyed"; isVoice = 1; pauseTime = 5000;}
	};
};

SoundSetup labBombSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_BombLab"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_BombLab"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "Labor_Ready"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_UNDER_ATTACK; name = "Labor_Under_Attack"; isVoice = 1; pauseTime = 20000;},
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "Labor_Destroyed"; isVoice = 1; pauseTime = 5000;}
	};
};

SoundSetup labAntigravSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_FlyLab"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_FlyLab"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "Labor_Ready"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_UNDER_ATTACK; name = "Labor_Under_Attack"; isVoice = 1; pauseTime = 20000;},
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "Labor_Destroyed"; isVoice = 1; pauseTime = 5000;}
	};
};

SoundSetup labSubterraSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_SubLab"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_SubLab"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "Labor_Ready"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_UNDER_ATTACK; name = "Labor_Under_Attack"; isVoice = 1; pauseTime = 20000;},
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "Labor_Destroyed"; isVoice = 1; pauseTime = 5000;}
	};
};

SoundSetup labExodusSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_ExodusLab"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_ExodusLab"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "Labor_Ready"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_UNDER_ATTACK; name = "Labor_Under_Attack"; isVoice = 1; pauseTime = 20000;},
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "Labor_Destroyed"; isVoice = 1; pauseTime = 5000;}
	};
};

SoundSetup labEmpireSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_EmpireLab"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_EmpireLab"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "Labor_Ready"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_UNDER_ATTACK; name = "Labor_Under_Attack"; isVoice = 1; pauseTime = 20000;},
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "Labor_Destroyed"; isVoice = 1; pauseTime = 5000;}
	};
};

SoundSetup labHarkbarkhoodSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_HarkbackLab"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_HarkbackLab"; is3D = 1; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "Labor_Ready"; isVoice = 1; },
		{ ID = SOUND_VOICE_BUILDING_UNDER_ATTACK; name = "Labor_Under_Attack"; isVoice = 1; pauseTime = 20000;},
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "Labor_Destroyed"; isVoice = 1; pauseTime = 5000;}
	};
};

SoundSetup truckSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "digger_active"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_lite"; is3D = 1; }
	};
};

SoundSetup buildingBlockSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Unit_Explosion_Base"; is3D = 1; }
	};
};

SoundSetup scumerSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Unit_Move_Scumer"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup diggerSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Unit_Move_Digger"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup piercerSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Unit_Move_Piercer"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup piercerMissileSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Unit_Shot_Piercer"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup disintegratorSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Disintegrator"; cycled = 1; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Disintegrator"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup scumSplitterSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Splitter"; cycled = 1; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Splitter"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup scumTwisterSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Twister"; cycled = 1; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Twister"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup scumHeaterSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Heater"; cycled = 1; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Heater"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup conductorSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_SHOT; name = "Unit_Shot_Heater"; cycled = 1; },
		{ ID = SOUND_MOVE; name = "Unit_Move_Heater"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup unseenSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Unit_Move_Unseen"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_mid"; is3D = 1; }
	};
};

SoundSetup rockerRocketSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Missile_Fly_Rocket_Rocker"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Missile_Explosion_Rocket_Rocker"; is3D = 1; }
	};
};

SoundSetup rprojectorRocketSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Missile_Fly_Rocket_RProjector"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Missile_Explosion_Rocket_RProjector"; is3D = 1; }
	};
};

SoundSetup ceptorRocketSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Missile_Fly_Rocket_Ceptor"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Missile_Explosion_Rocket_Ceptor"; is3D = 1; }
	};
};

SoundSetup ballisticMissileSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Missile_Fly_Rocket_Ballistic"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Missile_Explosion_Rocket_Ballistic"; is3D = 1; }
	};
};

SoundSetup wargonMissileSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Missile_Fly_Bomb_Wargon"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Missile_Explosion_Bomb_Wargon"; is3D = 1; }
	};
};

SoundSetup subchaserMissileSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "subterra_missile_move"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Missile_Explosion_Bomb_SubterraGun"; is3D = 1; }
	};
};

SoundSetup extirpatorMissileSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "subterra_missile_move"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Missile_Explosion_Bomb_Extirpator"; is3D = 1; }
	};
};

SoundSetup scumThrowerMissileSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Missile_Fly_Scum_Thrower"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Missile_Explosion_Scum_Thrower"; is3D = 1; }
	};
};

SoundSetup gunHowitzerMissileSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Missile_Fly_Bomb_Howitzer"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Missile_Explosion_Bomb_Howitzer"; is3D = 1; }
	};
};

SoundSetup bombieMissileSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Missile_Fly_Bomb_Bombie"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Missile_Explosion_Bomb_Bombie"; is3D = 1; }
	};
};

SoundSetup mortarMissileSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Missile_Fly_Bomb_Mortar"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Missile_Explosion_Bomb_Mortar"; is3D = 1; }
	};
};

SoundSetup frameSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "move_frame"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Struct_Explosion_Frame"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_Frame_Closed"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT_ALTERNATE; name = "Struct_Proc_Work_Frame_Opened"; is3D = 1; },
		{ ID = SOUND_VOICE_UNIT_MOVE; name = "Frame_Move"; isVoice = 1; pauseTime = 3000;},
		{ ID = SOUND_VOICE_UNIT_SELECTED; name = "Frame_Selected"; isVoice = 1; pauseTime = 3000;},
		{ ID = SOUND_VOICE_UNIT_UNDER_ATTACK; name = "Frame_Under_Attack"; isVoice = 1; pauseTime = 30000;},
		{ ID = SOUND_VOICE_UNIT_PRODUCTION_STARTED; name = "Frame_Prod_Started"; isVoice = 1; pauseTime = 3000;}
	};
};

SoundSetup harkbackSound = {
	sounds = new SoundControllerSetup[]
	{
		{ ID = SOUND_MOVE; name = "Unit_Move_Harkback"; cycled = 1; }
	};

	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "explosion_heavy"; is3D = 1; }
	};
};

SoundSetup corridorAlphaSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_OPEN; name = "Struct_Proc_Open_Alpha"; is3D = 1; }
	};
};

SoundSetup corridorOmegaSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_OPEN; name = "Struct_Proc_Open_Omega"; is3D = 1; },
		{ ID = SOUND_EVENT_SELECT; name = "Struct_Proc_Work_Omega"; is3D = 1; }
	};
};

SoundSetup filthAntSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Filth_Die_Ant"; is3D = 1; }
	};
};

SoundSetup filthRatSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Filth_Die_Rat"; is3D = 1; }
	};
};

SoundSetup filthWormSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Filth_Die_Worm"; is3D = 1; }
	};
};

SoundSetup filthWaspSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Filth_Die_Wasp"; is3D = 1; }
	};
};

SoundSetup filthGhostSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Filth_Die_Ghost"; is3D = 1; }
	};
};

SoundSetup filthEyeSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Filth_Die_Eye"; is3D = 1; }
	};
};

SoundSetup filthCrowSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Filth_Die_Crow"; is3D = 1; }
	};
};

SoundSetup filthDaemonSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Filth_Die_Daemon"; is3D = 1; }
	};
};

SoundSetup filthDragonSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Filth_Die_Dragon"; is3D = 1; }
	};
};

SoundSetup filthSharkSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_EVENT_EXPLOSION; name = "Filth_Die_Shark"; is3D = 1; }
	};
};


SoundSetup playerSound = {
	events = new SoundEventSetup[]
	{
		{ ID = SOUND_VOICE_UNIT_READY; name = "Unit_Ready"; isVoice = 1; pauseTime = 30000;},
		{ ID = SOUND_VOICE_SQUAD_SELECTED; name = "Squad_Selected";  isVoice = 1; pauseTime = 3000;},
		{ ID = SOUND_VOICE_SQUAD_MOVES; name = "Squad_Moves"; isVoice = 1; pauseTime = 3000; },
		{ ID = SOUND_VOICE_SQUAD_ATTACKS; name = "Squad_Attacks"; isVoice = 1; pauseTime = 1000; },
		{ ID = SOUND_VOICE_SQUAD_UNDER_ATTACK; name = "Squad_Under_Attack"; isVoice = 1; pauseTime = 10000; },
		{ ID = SOUND_VOICE_SQUAD_TRANSFORM_STARTED; name = "Squad_Transform_Start"; isVoice = 1; pauseTime = 10000; },
		{ ID = SOUND_VOICE_SQUAD_TRANSFORM_FINISHED; name = "Squad_Transform_Finished"; isVoice = 1; pauseTime = 10000; },
		{ ID = SOUND_VOICE_MMP_CONVERT_COMMAND; name = "MMP_Convert_Command"; isVoice = 1; pauseTime = 10000; },
		{ ID = SOUND_VOICE_TERRAFORMING_STARTED; name = "Terraforming_Started"; isVoice = 1; pauseTime = 10000; startDelay = 10000; },
		{ ID = SOUND_VOICE_TERRAFORMING_FINISHED; name = "Terraforming_Finished"; isVoice = 1; pauseTime = 10000; },
		{ ID = SOUND_VOICE_ENERGY_COLLECTOR_EMPTY; name = "Energy_Collector_Empty"; isVoice = 1; pauseTime = 30000; },
//		{ ID = SOUND_VOICE_ENERGY_COLLECTOR_FULL; name = "Energy_Collector_Full"; isVoice = 1; pauseTime = 30000; },
		{ ID = SOUND_VOICE_ENERGY_NOT_ENOUGH; name = "Energy_Not_Enough"; isVoice = 1; pauseTime = 30000; },
//		{ ID = SOUND_VOICE_ENERGY_LOSING; name = "Energy_Losing"; isVoice = 1; pauseTime = 30000; },
//		{ ID = SOUND_VOICE_ENERGY_RESUPPLY; name = "Energy_Resupply"; isVoice = 1; pauseTime = 30000; },
		{ ID = SOUND_VOICE_PSHIELD_GLOBAL_ACTIVE; name = "PShield_Global_Active"; isVoice = 1; pauseTime = 1000; },
		{ ID = SOUND_VOICE_PSHIELD_GLOBAL_OFF; name = "PShield_Global_Off"; isVoice = 1; pauseTime = 1000; },
		{ ID = SOUND_VOICE_PSHIELD_LOCAL_ACTIVE; name = "PShield_Local_Active"; isVoice = 1; pauseTime = 1000; },
		{ ID = SOUND_VOICE_PSHIELD_LOCAL_OFF; name = "PShield_Local_Off"; isVoice = 1; pauseTime = 1000; },
		{ ID = SOUND_VOICE_FRAME_ALARM; name = "Frame_Allarm"; isVoice = 1; pauseTime = 3000; },
		{ ID = SOUND_VOICE_FRAME_INSTALLED; name = "Frame_Installed"; isVoice = 1; pauseTime = 1000; },				
		{ ID = SOUND_VOICE_FRAME_DEINSTALLED; name = "Frame_Deinstalled"; isVoice = 1; pauseTime = 1000; },				
		{ ID = SOUND_VOICE_FRAME_TELEPORTATION; name = "Frame_Teleportation"; isVoice = 1; pauseTime = 1000; },
		{ ID = SOUND_VOICE_BUILDING_DISASSEMBLED; name = "Building_Disassembled"; isVoice = 1; pauseTime = 1000; },				
		{ ID = SOUND_VOICE_BUILDING_UPGRADE_FINISHED; name = "Labor_Upgrade_Finished"; isVoice = 1; pauseTime = 3000; },
		{ ID = SOUND_VOICE_BUILDING_DESTROYED; name = "Building_Destroyed"; isVoice = 1; pauseTime = 5000; },
		{ ID = SOUND_VOICE_BUILDING_READY; name = "Building_Ready"; isVoice = 1; pauseTime = 5000; },
		{ ID = SOUND_VOICE_BUILDING_UNDER_ATTACK; name = "Building_Under_Attack"; isVoice = 1; pauseTime = 20000;},
		{ ID = SOUND_VOICE_FILTH_DETECTED; name = "Scourge_activity_detected"; isVoice = 1; pauseTime = 60000;},
		{ ID = SOUND_VOICE_GEOACTIVITY_DETECTED; name = "Geological_activity_detected"; isVoice = 1; pauseTime = 60000;},
//		{ ID = SOUND_EVENT_BUILDING_READY; name = "Struct_Event_Just_Build"; activePlayer = 1; },
		{ ID = SOUND_EVENT_BUILDING_HOLD_CONSTRUCTION; name = "Struct_Event_Just_Stoped"; activePlayer = 1; }
	};
};

