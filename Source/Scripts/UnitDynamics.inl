////////////////////////////////////////////////////////////////////////////////////
//		UnitDynamics.inl
//
// Описвает динамическое поведение юнитов.
// Все данные будут встраиваться
// в terUnitAttributeData.
////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
//		RigidBodyPrm
/////////////////////////////////////////////////////////////////////////
struct RigidBodyPrm
{
									
	////////////////////////////////////////////////
	//     Small usefull structs
	////////////////////////////////////////////////
	struct Oscillator
	{
		float phase = 0;
		float amplitude = 10;
		float omega = 6;
		float amplitude_decrement = 0.01;
		float omega_increment = 0.01;
		float omega_disperse = 0.05;
	delegate:
		void set() { phase = terLogicRNDfrnd()*XM_PI; omega += terLogicRNDfrnd()*omega*omega_disperse; }
		float operator() (float dt, float velocity){ return amplitude*sin(phase += omega*(1 + omega_increment*velocity)*dt)/(1 + amplitude_decrement*velocity); }
	};

	struct AverageParameter
	{
		float x_avr = 0;
		float avr_tau = 0.05;
		float threshould = 0.1;
	delegate:
		void set(float x) { x_avr = x; }
		void average(float x) { ::average(x_avr, x, avr_tau); }
		operator float () const { return x_avr; }
		int active() const { return fabsf(x_avr) > threshould; }
	};

	struct Stiffness
	{
		float stiffness_up = 10;
		float stiffness_down = 10;
		float dz_max_up = 15;
		float dz_max_down = 15;
	delegate:
		float force(float dz) 
		{ 
			if(dz > 0)
			{
				if(dz > dz_max_up)
					dz = dz_max_up;
				return dz*stiffness_up;
			}
			else
			{
				if(dz < -dz_max_down)
					dz = -dz_max_down;
				return dz*stiffness_down;
			}
		}
	};


	////////////////////////////////////////////////
	//     Type and name
	////////////////////////////////////////////////
	enum RigidBodyType 
	{
		UNIT, 
		MISSILE, 
		ROCKET,
		DEBRIS
	};
	RigidBodyType unit_type = UNIT; 
	string name = "default";

	////////////////////////////////////////////////
	// Base physics properties
	////////////////////////////////////////////////
	// Dampings
	float linear_damping[] = { 6., 0.2, 5 };
	float angular_damping = 6;

	float gravity = 0;

	// Bound
	int polyhedral_bound = 1;
	float density = 1;
	float TOI_factor = 1;
	float radius_min = 3;

	////////////////////////////////////////////////
	//		General Controls
	////////////////////////////////////////////////
	// Не перемещается по x, y. Перемещается по z и ориентация.
	int unmovable = 0; 
	// Разрешается засыпать, если нет точек управления	
	int enable_sleeping = 1;
	// Ненаправленный, двигается в любую сторону
	int isotropic = 0;
	// Управляется точками, иначе свободный объект
	int controled_by_points = 1;
	
	// Obstacle On/Off
	int analyse_force_field_obstacle = 1;
	int analyse_body_obstacle = 1;
	int analyse_terraint_obstacle = 1;

	// Obstacle analytics
	float obstacle_restitution = .0125;
	float obstacle_relaxation = 0.25;

	// Obstacle torque
	AverageParameter obstacle_turn_avr = {};
	float avoid_obstacle_rudder_speed = 10;

	// Остановка 
	float brake_damping = 6;//3.5;
	float point_control_slow_distance = 20;
	float point_control_slow_factor = 0.1;

	//  Steering
	float rudder_speed = 1.5;
	float suppress_steering_duration = 1000;
	float steering_linear_velocity_min = 20;
	float steering_acceleration_max = 15;
	float is_point_reached_radius_max = 4; // Max radius for is_point_reached

	// Traction
	float forward_velocity_max = 20;
	float forward_acceleration = 40;

	// Levelling
	float orientation_torque_factor = 9; 

	// Info
	float average_forward_velocity_tau = 0.7;
	float speed_map_factor = 0.025; // Maps average_velocity -> [0..1]

	////////////////////////////////////////////////
	//     Ground unit parameters
	////////////////////////////////////////////////
	// Terrain analyse
	float analyze_points_density = .4;
	int Dxy_minimal = 6;
	float dz_max_avr_tau = 0.1;
	float minimal_velocity_xy = 4;
	float zero_velocity_z = 5;
	float deep_penetration_dz = 10;
	float box_delta_y = 4;

	// Гравицапа
	int gravicap_enabled = 1;
	float gravicap_pitch_roll_threshould = 0.6;
	int gravicap_dz_treshould = 40;
	Stiffness gravicap_stiffness = { stiffness_up = 7; stiffness_down = 7; dz_max_up = 1000; dz_max_down = 30; }; 
	Oscillator gravicap_oscillator_z = { amplitude = 5; };
	Oscillator gravicap_oscillator_pitch = { amplitude = 0.5; };
	Oscillator gravicap_oscillator_roll = { amplitude = 0.25; };

	// Кенгуру
	int kangaroo_property = 1;
	int kangaroo_allow_on_me = 1; 
	float kangaroo_delta_z = -3;

	// UnderGround
	float digging_depth = 150;
	float digging_depth_velocity = 10;


	////////////////////////////////////////////////
	//		Flying 
	////////////////////////////////////////////////
	int flying_down_without_way_points = 1;
	int flying_height_relative = 1;
	float flying_height = 50;
	Stiffness flying_stiffness = { stiffness_up = 10; stiffness_down = 10; dz_max_up = 15; dz_max_down = 15; };
	Oscillator flying_oscillator_z = { amplitude = 20; omega = 0.005; amplitude_decrement = 0; omega_increment = 0; };
	float flying_vertical_direction_x_factor = 0.30;
	float flying_vertical_direction_y_factor = 0;//0.01;

	////////////////////////////////////////////////
	//      Misssile
	////////////////////////////////////////////////
	float minimize_theta = 0;
	float upper_theta = 90; // degrees
	float lower_theta = -45; // degrees
	float distance_correction_factor = 0.96; // из-за ошибки интегрирования снаряды чуть-чуть перелетают.
	int keep_direction_time = 0;
	int ground_colliding_delay = 100;

	////////////////////////////////////////////////
	//		Rocket
	////////////////////////////////////////////////
	float rocket_vertical_control_distance = 120;
	float rocket_forward_analysis_distance = 80;
	float rocket_target_offset_z = 0;

	////////////////////////////////////////////////
	//      Debris
	////////////////////////////////////////////////
	float debris_angular_velocity = 3.14;

	////////////////////////////////////////////////
	//      Debug
	////////////////////////////////////////////////
	int enable_show = 1;
};


/////////////////////////////////////////////////////////////////////////////////
// Предустановки параметров всех тел
/////////////////////////////////////////////////////////////////////////////////
struct RigidBodyTable
{

	/////////////////////////////////////
	//	RigidBody Templates
	/////////////////////////////////////
	// Обычные наземные юниты
	struct GroundUnitPrm : RigidBodyPrm
	{
		name = "Unit Ground"; 
		unit_type = UNIT; 
		analyse_force_field_obstacle = 0;
	};

	// Статические наземные юниты
	struct StaticGroundUnitPrm : GroundUnitPrm
	{
		name = "Building"; 
		unit_type = UNIT;
		unmovable = 1;
		gravicap_enabled = 0;
		kangaroo_property = 0;
		forward_velocity_max = 0; 
	};

	// Летающие юниты
	struct FlyingUnitPrm : GroundUnitPrm
	{
		name = "Unit Flying"; 
		flying_height = 250;
		forward_velocity_max = 80; 
		forward_acceleration = 60;
		linear_damping = { 3, 0.4, 4 };
		angular_damping = 3;
		orientation_torque_factor = 3;
		rudder_speed = 0.5;
		flying_oscillator_z.amplitude = 50;
		flying_oscillator_z.omega = 1;
		is_point_reached_radius_max = 8;
		point_control_slow_distance = 40;
	};

	struct FlyingMilitaryUnitPrm : FlyingUnitPrm
	{
		name = "Unit Flying Military"; 
		flying_down_without_way_points = 0;
	};

	// Ракеты
	struct RocketPrm : RigidBodyPrm  
	{
		name = "Rocket"; 
		unit_type = ROCKET;
		polyhedral_bound = 0;
		gravicap_enabled = 0;
		kangaroo_property = 0;
		kangaroo_allow_on_me = 0; 

		flying_height = 40;

		point_control_slow_distance = 0;
		suppress_steering_duration = 500;
		
		forward_velocity_max = 300; 
		forward_acceleration = 500;
		linear_damping = { 3, 3, 3 };
		angular_damping = 3;
		orientation_torque_factor = 15;
		rudder_speed = 2;
		is_point_reached_radius_max = 0;
	};

	// Снаряды
	struct MissilePrm : RigidBodyPrm
	{
		name = "Missile"; 
		unit_type = MISSILE; 
		polyhedral_bound = 0;
		gravicap_enabled = 0;
		kangaroo_property = 0;
		kangaroo_allow_on_me = 0; 
		
		forward_velocity_max = 350; 
		gravity = 150;
		linear_damping = { 0, 0, 0 };
		angular_damping = 6;
		orientation_torque_factor = 40;
	};


	/////////////////////////////////////////////////////////////////////////////////
	//			Ground Units
	/////////////////////////////////////////////////////////////////////////////////
	// фрейм
	GroundUnitPrm FramePrm = 
	{
		name = "Unit Frame";
		isotropic = 1;
		forward_velocity_max = 10; 
		//linear_damping = { 5, 1, 4 };
		linear_damping = { 1, 1, 4 };
		angular_damping = 10;

		//gravicap_stiffness = { stiffness_up = 40; stiffness_down = 40; }; 

		flying_height = 40;
		kangaroo_property = 0;
		kangaroo_allow_on_me = 1; 
		flying_down_without_way_points = 0;

		flying_vertical_direction_x_factor = 0;
		flying_vertical_direction_y_factor = 0;
		flying_oscillator_z.amplitude = 0;

		analyse_force_field_obstacle = 0;
		analyse_body_obstacle = 0;
	};

	// Ковш
	GroundUnitPrm TruckPrm = 
	{
		name = "Unit Truck";
		polyhedral_bound = 0;
		forward_velocity_max = 45; 
		rudder_speed = 2.0; 

		kangaroo_property = 0;
		analyse_force_field_obstacle = 0;
		analyse_body_obstacle = 0;
		analyse_terraint_obstacle = 0;
		enable_show = 0;
	};

	// Блок
	GroundUnitPrm BlockPrm = 
	{
		name = "Unit Block"; 
		polyhedral_bound = 0;
		forward_velocity_max = 120; 
		rudder_speed = 2.0; 

		analyse_force_field_obstacle = 0;
		analyse_body_obstacle = 0;
		analyse_terraint_obstacle = 0;
		enable_show = 0;
	};
	
	// Бригадир
	GroundUnitPrm TerrainMasterPrm = 
	{
		//name = "TerrainMaster"; 
		forward_velocity_max = 20;
	};

	// Прораб
	FlyingUnitPrm BuildMasterPrm = 
	{
		//name = "BuildMaster"; 
		forward_velocity_max = 20;
	};


	// солдат
	GroundUnitPrm SoldierPrm = 
	{
		//name = "Soldier"; 
		forward_velocity_max = 160;
	};

	 // техник
	GroundUnitPrm TechnicPrm =
	{
		//name = "Technic"; 
		forward_velocity_max = 160;
	};
																									
	// офицер
	FlyingUnitPrm OfficerPrm = 
	{
		//name = "Officer"; 
		forward_velocity_max = 160;
		flying_height = 50;
	};

	//Перфоратор
	GroundUnitPrm PerforatorPrm = 
	{
		//name = "Perforator"; 
	};

	// Sniper
	GroundUnitPrm SniperPrm = 
	{
		//name = "Sniper";
		forward_velocity_max = 60;
	};

	// Leamo
	GroundUnitPrm LeamoPrm = 
	{
		//name = "Leamo";
		forward_velocity_max = 40;
	};

	// Impaler
	GroundUnitPrm ImpalerPrm = 
	{
		//name = "Impaler";
		forward_velocity_max = 40;
	};

	// Rocker
	GroundUnitPrm RockerPrm = 
	{
		//name = "Rocker";
		forward_velocity_max = 60;
	};

	// Mortar
	GroundUnitPrm MortarPrm = 
	{
		//name = "Mortar";
		forward_velocity_max = 60;
	};

	// Scumer
	GroundUnitPrm ScumerPrm = 
	{
		//name = "Scumer";
		forward_velocity_max = 10;
	};

	// Digger
	GroundUnitPrm DiggerPrm = 
	{
		//name = "Digger";
		forward_velocity_max = 15;
	};

	// Piercer
	GroundUnitPrm PiercerPrm = 
	{
		//name = "Piercer";
		forward_velocity_max = 40;
	};

	// Wargon
	GroundUnitPrm WargonPrm = 
	{
		//name = "Wargon";
		forward_velocity_max = 50;
	};

	//Projector
	GroundUnitPrm ProjectorPrm = 
	{
		//name = "Projector";
		forward_velocity_max = 50;
	};

	GroundUnitPrm MinotaurPrm = 
	{
		//name = "Minotaur";
	};

	GroundUnitPrm UnseenPrm = 
	{
		//name = "Unseen";
//		flying_height = 100;
		forward_velocity_max = 80; 
	};

	GroundUnitPrm LeechPrm = 
	{
		//name = "Leech";
		forward_velocity_max = 60;
	};

	GroundUnitPrm ScumThrowerPrm = 
	{
		//name = "Scum Thrower";
		forward_velocity_max = 40;
	};

	GroundUnitPrm ExtirpatorPrm = 
	{
		//name = "Extirpator";
		forward_velocity_max = 40;
	};

	// Scum Splitter
	GroundUnitPrm ScumSplitterPrm = 
	{
		//name = "Scum Splitter";
		forward_velocity_max = 60;
	};

	// Scum Twister
	GroundUnitPrm ScumTwisterPrm = 
	{
		//name = "Scum Twister";
		forward_velocity_max = 50;
	};

	// Defence Gun
	StaticGroundUnitPrm GunPrm = 
	{
		//name = "Gun"; 
	};

	GroundUnitPrm FilthSpot0Prm = 
	{
		//name = "FilthSpot0";
		forward_velocity_max = 40;
	};
	GroundUnitPrm FilthSpot1Prm = 
	{
		//name = "FilthSpot1";
		forward_velocity_max = 40;
	};
	GroundUnitPrm FilthSpot2Prm = 
	{
		//name = "FilthSpot2";
		forward_velocity_max = 40;
	};
	GroundUnitPrm FilthSpot3Prm = 
	{
		//name = "FilthSpot3";
		forward_velocity_max = 40;
	};

	// GenericBuilding
	StaticGroundUnitPrm GenericBuildingPrm = 
	{
		//name = "GenericBuilding"; 
		analyse_force_field_obstacle = 0;
	};

	// Обект окружения
	GroundUnitPrm EnvironmentPrm = 
	{
		name = "Environment"; 
		unit_type = UNIT;
		unmovable = 1;
		gravicap_enabled = 0;
		analyse_body_obstacle = 0;
		kangaroo_allow_on_me = 0;
	};

	/////////////////////////////////////////////////////////////////////////////////
	//		Flying Units
	/////////////////////////////////////////////////////////////////////////////////
	// Ceptor
	FlyingMilitaryUnitPrm CeptorPrm = 
	{
		//name = "Ceptor";
		flying_height = 360;
		forward_velocity_max = 200; 
	};

	// Gyroid
	FlyingMilitaryUnitPrm GyroidPrm = 
	{
		//name = "Gyroid";
		forward_velocity_max = 360; 
	};

	// Eflair
	FlyingMilitaryUnitPrm EflairPrm = 
	{
		//name = "Eflair";
		forward_velocity_max = 360; 
	};

	// Bomber
	FlyingMilitaryUnitPrm BomberPrm = 
	{
		//name = "Bomber";
		flying_height = 360;
		forward_velocity_max = 70; 
	};

	// Strafer
	FlyingMilitaryUnitPrm StraferPrm = 
	{
		//name = "Strafer";
		forward_velocity_max = 360; 
	};

	// Disintegrator
	FlyingMilitaryUnitPrm DisintegratorPrm = 
	{
		//name = "Disintegrator";
		forward_velocity_max = 360; 
	};

	// Scum Heater
	FlyingMilitaryUnitPrm ScumHeaterPrm = 
	{
		name = "Unit Scum Heater";
		flying_height = 380;
		forward_velocity_max = 70; 
		flying_vertical_direction_x_factor = 0;
	};

	// Conductor
	FlyingMilitaryUnitPrm ConductorPrm = 
	{
		flying_height = 380;
		forward_velocity_max = 70; 
		flying_vertical_direction_x_factor = 0;
	};

	/////////////////////////////////////
	//		Твари
	/////////////////////////////////////
	// Паук
	struct GroundFilth : GroundUnitPrm 
	{
		name = "Filth Ground";
		polyhedral_bound = 0;
		rudder_speed = 2.0;
		gravicap_enabled = 0;
	};
	
	struct FlyingFilth : FlyingUnitPrm 
	{
		name = "Filth Flying";
		polyhedral_bound = 0;
		rudder_speed = 2.0;
		forward_velocity_max = 80;
		gravicap_enabled = 0;
		kangaroo_property = 0;
	};
	
	GroundFilth AntPrm = 
	{
		//name = "Ant"; 
		forward_velocity_max = 20; 
	};

	GroundFilth RatPrm = 
	{
		//name = "Rat"; 
		forward_velocity_max = 20; 
	};

	GroundFilth WormPrm = 
	{
		//name = "Worm"; 
		forward_velocity_max = 20; 
	};

	FlyingFilth WaspPrm = 
	{
		//name = "Filth Wasp"; 
		forward_velocity_max = 80;
	};

	FlyingFilth CrowPrm = 
	{
		name = "Filth Crow"; 
		forward_velocity_max = 80;
		kangaroo_allow_on_me = 0;
	};

	FlyingFilth GhostPrm = 
	{
		name = "Filth Ghost"; 
		forward_velocity_max = 100;
		analyse_body_obstacle = 0;
	};

	FlyingFilth EyePrm = 
	{
		name = "Filth Eye"; 
		forward_velocity_max = 40;
		analyse_body_obstacle = 0;
		analyse_terraint_obstacle = 0;
		flying_down_without_way_points = 0;
	};

	FlyingFilth DragonPrm = 
	{
		name = "Filth Dragon"; 
		rudder_speed = 0.5;
		angular_damping = 4;
		forward_velocity_max = 40;
		forward_acceleration = 100;
		flying_oscillator_z.amplitude = 50;
		flying_oscillator_z.omega = 0.5;

		linear_damping = { 3, 0.4, 0.8 };
	};

	GroundUnitPrm CorridorAlphaPrm = 
	{
		name = "Unit Corridor Alpha";
		unmovable = 1;
		isotropic = 1;
		enable_sleeping = 0;
		controled_by_points = 0;
		flying_down_without_way_points = 0;
		flying_height_relative = 0;
		flying_height = 350;
		linear_damping = { 8, 8, 8 };
		flying_vertical_direction_x_factor = 0;
		flying_vertical_direction_y_factor = 0;
		forward_velocity_max = 100; 
		forward_acceleration = 300;
	};

	/////////////////////////////////////
	//				Missiles
	/////////////////////////////////////
	// Снаряд гаубицы
	MissilePrm HowitzerMissilePrm = 
	{
		name = "Missile Howitzer"; 
		minimize_theta = 0;
		lower_theta = 45;
	};

	// Бомба
	MissilePrm BombMissilePrm = 
	{
		name = "Missile Bomb"; 
		minimize_theta = 1;
//		forward_velocity_max = 250; 
		forward_velocity_max = 0; 
		lower_theta = -90;
	};

	// Баллистическая ракета (снаряд)
	MissilePrm BallisticMissilePrm = 
	{
		name = "Missile GunBallistic";
		minimize_theta = 0;
		forward_velocity_max = 180; 
		gravity = 15;
		keep_direction_time = 2000;
		distance_correction_factor = 1;
		analyse_force_field_obstacle = 0;
	};

	// WargonMissile
	MissilePrm WargonMissilePrm = 
	{
		name = "Missile Wargon"; 
		minimize_theta = 0;
		lower_theta = 45;
	};

	// ScumThrowerMissile
	MissilePrm ScumThrowerMissilePrm = 
	{
		name = "Missile ScumThrower"; 
		minimize_theta = 0;
		lower_theta = 45;
	};

	// MortarMissile
	MissilePrm MortarMissilePrm = 
	{
		name = "Missile Mortar"; 
		minimize_theta = 0;
		gravity = 140;
		lower_theta = 45;
		forward_velocity_max = 300;
	};

	// SoldierMissile
	MissilePrm SoldierMissilePrm = 
	{
		name = "Missile Soldier"; 
		minimize_theta = 1;
		lower_theta = -45;
		gravity = 30;
		forward_velocity_max = 150;
	};

	// StraferMissile
	MissilePrm StraferMissilePrm = 
	{
		name = "Missile Strafer"; 
		minimize_theta = 1;
		lower_theta = -90;
		distance_correction_factor = 1;
		gravity = 30;
		forward_velocity_max = 150;
	};

	// Scum storm
	GroundUnitPrm ScumStormPrm = 
	{
		name = "Missile ScumStorm"; 
		polyhedral_bound = 0;
		forward_velocity_max = 120; 
		rudder_speed = 2.0; 

		analyse_force_field_obstacle = 0;
		analyse_body_obstacle = 0;
		analyse_terraint_obstacle = 0;

		is_point_reached_radius_max = 8;
	};

	/////////////////////////////////////
	//		Rockets
	/////////////////////////////////////
	RocketPrm CeptorRocketPrm = { };
	RocketPrm ProjectorRocketPrm = { };
	RocketPrm RockerRocketPrm = { };
	RocketPrm GunRocketPrm = { };
	
	/////////////////////////////////////
	//		UnderGround
	/////////////////////////////////////
	GroundUnitPrm ExtirpatorMissilePrm = 
	{
		name = "Missile Extirpator"; 
		forward_velocity_max = 60;
		minimal_velocity_xy = forward_velocity_max;
	};

	GroundUnitPrm SubchaserMissilePrm = 
	{
		//name = "Missile Subchaser";
		forward_velocity_max = 60;
	};

	GroundUnitPrm PiercerMissilePrm = 
	{
		name = "Missile Piercer"; 
		forward_velocity_max = 60;
		is_point_reached_radius_max = 8;
	};

	/////////////////////////////////////
	//			Debris
	/////////////////////////////////////
	// default
	RigidBodyPrm debris_prm = 
	{ 
		name = "Debris"; 
		unit_type = DEBRIS; 
		gravity = 100;
		linear_damping = { 0, 0, 0 };
		angular_damping = 0;
		orientation_torque_factor = 0;
	};

	RigidBodyPrm VolcanoPrm = debris_prm
	{
		//name = "Volcano"; 
	};

	RigidBodyPrm CorpseDynamicPrm = debris_prm
	{
		//name = "CorpseDynamic"; 
	};

delegate: // Рассматриваем последовательность расположенных рядом структур как массив
	int size() const { xassert(sizeof(*this) % sizeof(RigidBodyPrm) == 0); return sizeof(*this)/sizeof(RigidBodyPrm); }
	const RigidBodyPrm& operator[](int i) const { xassert(i >= 0 && i < size()); return ((RigidBodyPrm*)this)[i]; }
};


RigidBodyTable rigidBodyTable = {};

/////////////////////////////////////////////////////////////
//		Non-static struct for suggestion.
/////////////////////////////////////////////////////////////
RigidBodyPrm HowitzerMissileSuggestPrm = rigidBodyTable.HowitzerMissilePrm;
RigidBodyPrm WargonMissileSuggestPrm = rigidBodyTable.WargonMissilePrm;
RigidBodyPrm MortarMissileSuggestPrm = rigidBodyTable.MortarMissilePrm;
RigidBodyPrm ScumThrowerMissileSuggestPrm = rigidBodyTable.ScumThrowerMissilePrm;
RigidBodyPrm BallisticMissileSuggestPrm = rigidBodyTable.BallisticMissilePrm;
