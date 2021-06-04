#ifndef __RIGID_BODY_PRM_H__
#define __RIGID_BODY_PRM_H__


////////////////////////////////////////
struct RigidBodyPrm : ShareHandleBase
{
	////////////////////////////////////////////////
	//     Small usefull structs
	////////////////////////////////////////////////
	struct Oscillator
	{
		float phase;
		float amplitude;
		float omega;
		float amplitude_decrement;
		float omega_increment;
		float omega_disperse;

		Oscillator()
		{
			phase = 0;
			amplitude = 10;
			omega = 6;
			amplitude_decrement = 0.01f;
			omega_increment = 0.01f;
			omega_disperse = 0.05f;
			set();
		}

		template<class Archive>
		void serialize(Archive& ar) {
			ar & TRANSLATE_OBJECT(phase, "phase");
			ar & TRANSLATE_OBJECT(amplitude, "amplitude");
			ar & TRANSLATE_OBJECT(omega, "omega");
			ar & TRANSLATE_OBJECT(amplitude_decrement, "amplitude_decrement");
			ar & TRANSLATE_OBJECT(omega_increment, "omega_increment");
			ar & TRANSLATE_OBJECT(omega_disperse, "omega_disperse");
			if(!ar.isOutput())
				set();
		}

		void set() { phase = terLogicRNDfrnd()*M_PI; omega += terLogicRNDfrnd()*omega*omega_disperse; }
		float operator() (float dt, float velocity){ return amplitude*sin(phase += omega*(1 + omega_increment*velocity)*dt)/(1 + amplitude_decrement*velocity); }
	};

	struct Stiffness
	{
		float stiffness_up;
		float stiffness_down;
		float dz_max_up;
		float dz_max_down;
	
		Stiffness() {
			stiffness_up = 10;
			stiffness_down = 10;
			dz_max_up = 15;
			dz_max_down = 15;
		}

		template<class Archive>
		void serialize(Archive& ar) {
			ar & TRANSLATE_OBJECT(stiffness_up, "stiffness_up");
			ar & TRANSLATE_OBJECT(stiffness_down, "stiffness_down");
			ar & TRANSLATE_OBJECT(dz_max_up, "dz_max_up");
			ar & TRANSLATE_OBJECT(dz_max_down, "dz_max_down");
		}

		float force(float dz) const 
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
	EnumWrapper<RigidBodyType> unit_type; 
	PrmString name;

	////////////////////////////////////////////////
	// Base physics properties
	////////////////////////////////////////////////
	// Dampings
	Vect3f linear_damping;
	float angular_damping;

	float gravity;

	// Bound
	bool polyhedral_bound;
	float density;
	float TOI_factor;
	float radius_min;

	////////////////////////////////////////////////
	//		General Controls
	////////////////////////////////////////////////
	// Не перемещается по x, y. Перемещается по z и ориентация.
	bool unmovable; 
	// Разрешается засыпать, если нет точек управления	
	bool enable_sleeping;
	// Ненаправленный, двигается в любую сторону
	bool isotropic;
	// Управляется точками, иначе свободный объект
	bool controled_by_points;
	
	// Obstacle On/Off
	bool analyse_force_field_obstacle;
	bool analyse_body_obstacle;
	bool analyse_terraint_obstacle;

	// Obstacle analytics
	float obstacle_restitution;
	float obstacle_relaxation;

	// Obstacle torque
	float avoid_obstacle_rudder_speed;

	// Остановка 
	float brake_damping;//3.5;
	float point_control_slow_distance;
	float point_control_slow_factor;

	//  Steering
	float rudder_speed;
	float suppress_steering_duration;
	float steering_linear_velocity_min;
	float steering_acceleration_max;
	float is_point_reached_radius_max; // Max radius for is_point_reached

	// Traction
	float forward_velocity_max;
	float forward_acceleration;

	// Levelling
	float orientation_torque_factor; 

	// Info
	float speed_map_factor; // Maps average_velocity -> [0..1]

	////////////////////////////////////////////////
	//     Ground unit parameters
	////////////////////////////////////////////////
	// Terrain analyse
	float analyze_points_density;
	int Dxy_minimal;
	float minimal_velocity_xy;
	float zero_velocity_z;
	float box_delta_y;

	// Гравицапа
	bool gravicap_enabled;
	float gravicap_pitch_roll_threshould;
	int gravicap_dz_treshould;
	Stiffness gravicap_stiffness;
	Oscillator gravicap_oscillator_z;
	Oscillator gravicap_oscillator_pitch;
	Oscillator gravicap_oscillator_roll;

	// Кенгуру
	bool kangaroo_property;
	bool kangaroo_allow_on_me; 
	float kangaroo_delta_z;

	// UnderGround
	float digging_depth;
	float digging_depth_velocity;


	////////////////////////////////////////////////
	//		Flying 
	////////////////////////////////////////////////
	bool flying_down_without_way_points;
	bool flying_height_relative;
	float flying_height;
	Stiffness flying_stiffness;
	Oscillator flying_oscillator_z;
	float flying_vertical_direction_x_factor;
	float flying_vertical_direction_y_factor;

	////////////////////////////////////////////////
	//      Misssile
	////////////////////////////////////////////////
	bool minimize_theta;
	float upper_theta; // degrees
	float lower_theta; // degrees
	float distance_correction_factor; // из-за ошибки интегрирования снаряды чуть-чуть перелетают.
	int keep_direction_time;
	int ground_colliding_delay;

	////////////////////////////////////////////////
	//		Rocket
	////////////////////////////////////////////////
	float rocket_vertical_control_distance;
	float rocket_forward_analysis_distance;
	float rocket_target_offset_z;

	////////////////////////////////////////////////
	//      Debris
	////////////////////////////////////////////////
	float debris_angular_velocity;

	////////////////////////////////////////////////
	//      Debug
	////////////////////////////////////////////////
	bool enable_show;

	RigidBodyPrm() {
		unit_type = UNIT; 
		name = "default";

		linear_damping.set(6, 0.2f, 5);
		angular_damping = 6;
		gravity = 0;

		polyhedral_bound = true;
		density = 1;
		TOI_factor = 1;
		radius_min = 3;

		unmovable = false; 
		enable_sleeping = true;
		isotropic = false;
		controled_by_points = true;
	
		analyse_force_field_obstacle = true;
		analyse_body_obstacle = true;
		analyse_terraint_obstacle = true;

		obstacle_restitution = .0125f;
		obstacle_relaxation = 0.25f;

		avoid_obstacle_rudder_speed = 10;

		brake_damping = 6;//3.5;
		point_control_slow_distance = 20;
		point_control_slow_factor = 0.1f;

		rudder_speed = 1.5;
		suppress_steering_duration = 1000;
		steering_linear_velocity_min = 20;
		steering_acceleration_max = 15;
		is_point_reached_radius_max = 4; // Max radius for is_point_reached

		forward_velocity_max = 20;
		forward_acceleration = 40;

		orientation_torque_factor = 9; 

		speed_map_factor = 0.025f; // Maps average_velocity -> [0..1]

		analyze_points_density = .4f;
		Dxy_minimal = 6;
		minimal_velocity_xy = 4;
		zero_velocity_z = 5;
		box_delta_y = 4;

		gravicap_enabled = true;
		gravicap_pitch_roll_threshould = 0.6f;
		gravicap_dz_treshould = 40;

		kangaroo_property = true;
		kangaroo_allow_on_me = true; 
		kangaroo_delta_z = -3;

		digging_depth = 150;
		digging_depth_velocity = 10;

		flying_down_without_way_points = true;
		flying_height_relative = true;
		flying_height = 50;
		flying_vertical_direction_x_factor = 0.30f;
		flying_vertical_direction_y_factor = 0;//0.01;

		minimize_theta = 0;
		upper_theta = 90; // degrees
		lower_theta = -45; // degrees
		distance_correction_factor = 0.96f; // из-за ошибки интегрирования снаряды чуть-чуть перелетают.
		keep_direction_time = 0;
		ground_colliding_delay = 100;

		rocket_vertical_control_distance = 120;
		rocket_forward_analysis_distance = 80;
		rocket_target_offset_z = 0;

		debris_angular_velocity = 3.14f;

		enable_show = true;
	}
	virtual ~RigidBodyPrm() {}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(unit_type, "unit_type");
		ar & TRANSLATE_OBJECT(name, "name");
		
		ar & TRANSLATE_OBJECT(linear_damping, "linear_damping");
		ar & TRANSLATE_OBJECT(angular_damping, "angular_damping");
		ar & TRANSLATE_OBJECT(gravity, "gravity");
		
		ar & TRANSLATE_OBJECT(polyhedral_bound, "polyhedral_bound");
		ar & TRANSLATE_OBJECT(density, "density");
		ar & TRANSLATE_OBJECT(TOI_factor, "TOI_factor");
		ar & TRANSLATE_OBJECT(radius_min, "radius_min");

		ar & TRANSLATE_OBJECT(unmovable, "unmovable");
		ar & TRANSLATE_OBJECT(enable_sleeping, "enable_sleeping");
		ar & TRANSLATE_OBJECT(isotropic, "isotropic");
		ar & TRANSLATE_OBJECT(controled_by_points, "controled_by_points");
		
		ar & TRANSLATE_OBJECT(analyse_force_field_obstacle, "analyse_force_field_obstacle");
		ar & TRANSLATE_OBJECT(analyse_body_obstacle, "analyse_body_obstacle");
		ar & TRANSLATE_OBJECT(analyse_terraint_obstacle, "analyse_terraint_obstacle");
		
		ar & TRANSLATE_OBJECT(obstacle_restitution, "obstacle_restitution");
		ar & TRANSLATE_OBJECT(obstacle_relaxation, "obstacle_relaxation");
		ar & TRANSLATE_OBJECT(avoid_obstacle_rudder_speed, "avoid_obstacle_rudder_speed");

		ar & TRANSLATE_OBJECT(brake_damping, "brake_damping");
		ar & TRANSLATE_OBJECT(point_control_slow_distance, "point_control_slow_distance");
		ar & TRANSLATE_OBJECT(point_control_slow_factor, "point_control_slow_factor");

		ar & TRANSLATE_OBJECT(rudder_speed, "rudder_speed");
		ar & TRANSLATE_OBJECT(suppress_steering_duration, "suppress_steering_duration");
		ar & TRANSLATE_OBJECT(steering_linear_velocity_min, "steering_linear_velocity_min");
		ar & TRANSLATE_OBJECT(steering_acceleration_max, "steering_acceleration_max");
		ar & TRANSLATE_OBJECT(is_point_reached_radius_max, "is_point_reached_radius_max");

		ar & TRANSLATE_OBJECT(forward_velocity_max, "forward_velocity_max");
		ar & TRANSLATE_OBJECT(forward_acceleration, "forward_acceleration");
		ar & TRANSLATE_OBJECT(orientation_torque_factor, "orientation_torque_factor");

		ar & TRANSLATE_OBJECT(speed_map_factor, "speed_map_factor");

		ar & TRANSLATE_OBJECT(analyze_points_density, "analyze_points_density");
		ar & TRANSLATE_OBJECT(Dxy_minimal, "Dxy_minimal");
		ar & TRANSLATE_OBJECT(minimal_velocity_xy, "minimal_velocity_xy");
		ar & TRANSLATE_OBJECT(zero_velocity_z, "zero_velocity_z");
		ar & TRANSLATE_OBJECT(box_delta_y, "box_delta_y");

		ar & TRANSLATE_OBJECT(gravicap_enabled, "gravicap_enabled");
		ar & TRANSLATE_OBJECT(gravicap_pitch_roll_threshould, "gravicap_pitch_roll_threshould");
		ar & TRANSLATE_OBJECT(gravicap_dz_treshould, "gravicap_dz_treshould");
		ar & TRANSLATE_OBJECT(gravicap_stiffness, "gravicap_stiffness");
		ar & TRANSLATE_OBJECT(gravicap_oscillator_z, "gravicap_oscillator_z");
		ar & TRANSLATE_OBJECT(gravicap_oscillator_pitch, "gravicap_oscillator_pitch");
		ar & TRANSLATE_OBJECT(gravicap_oscillator_roll, "gravicap_oscillator_roll");

		ar & TRANSLATE_OBJECT(kangaroo_property, "kangaroo_property");
		ar & TRANSLATE_OBJECT(kangaroo_allow_on_me, "kangaroo_allow_on_me");
		ar & TRANSLATE_OBJECT(kangaroo_delta_z, "kangaroo_delta_z");
		
		ar & TRANSLATE_OBJECT(digging_depth, "digging_depth");
		ar & TRANSLATE_OBJECT(digging_depth_velocity, "digging_depth_velocity");

		ar & TRANSLATE_OBJECT(flying_down_without_way_points, "flying_down_without_way_points");
		ar & TRANSLATE_OBJECT(flying_height_relative, "flying_height_relative");
		ar & TRANSLATE_OBJECT(flying_height, "flying_height");
		ar & TRANSLATE_OBJECT(flying_stiffness, "flying_stiffness");
		ar & TRANSLATE_OBJECT(flying_oscillator_z, "flying_oscillator_z");
		ar & TRANSLATE_OBJECT(flying_vertical_direction_x_factor, "flying_vertical_direction_x_factor");
		ar & TRANSLATE_OBJECT(flying_vertical_direction_y_factor, "flying_vertical_direction_y_factor");

		ar & TRANSLATE_OBJECT(minimize_theta, "minimize_theta");
		ar & TRANSLATE_OBJECT(upper_theta, "upper_theta");
		ar & TRANSLATE_OBJECT(lower_theta, "lower_theta");
		ar & TRANSLATE_OBJECT(distance_correction_factor, "distance_correction_factor");
		ar & TRANSLATE_OBJECT(keep_direction_time, "keep_direction_time");
		ar & TRANSLATE_OBJECT(ground_colliding_delay, "ground_colliding_delay");

		ar & TRANSLATE_OBJECT(rocket_vertical_control_distance, "rocket_vertical_control_distance");
		ar & TRANSLATE_OBJECT(rocket_forward_analysis_distance, "rocket_forward_analysis_distance");
		ar & TRANSLATE_OBJECT(rocket_target_offset_z, "rocket_target_offset_z");

		ar & TRANSLATE_OBJECT(debris_angular_velocity, "debris_angular_velocity");
		ar & TRANSLATE_OBJECT(enable_show, "enable_show");
	}

	float calcTurnTheta(float x, float z, float velocity) const;
};

typedef TypeLibrary<string, RigidBodyPrm> RigidBodyPrmLibrary;
extern SingletonPrm<RigidBodyPrmLibrary> rigidBodyPrmLibrary;


#endif //__RIGID_BODY_PRM_H__