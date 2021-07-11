#ifndef _DEBUG_PRM_H_
#define _DEBUG_PRM_H_

enum WriteModes {
	DEBUG_WRITE_BODY_STATE = 1,
	DEBUG_SHOW_WATCH
};

struct ShowDebugRigidBody {
	int boundingBox;
	int radius;
	int inscribedRadius;
	int wayPoints;
	int contacts;
	int movingDirection;
	int acceleration;
	float accelerationScale;
	float angularAccelerationScale;
	int obstaclePoint;
	int rocketTarget;
	int velocityValue;
	
	ShowDebugRigidBody();
	template<class Archive>	void serialize(Archive& ar);
};

struct ShowDebugUnitReal {
	int docking;
	int chaosCollidingFactor;
	int target;
	int fireResponse;
	int chain;
	int chainID;
	int toolzer;
	int positionValue;
	int unitID;
	
	ShowDebugUnitReal();
	template<class Archive>	void serialize(Archive& ar);
};

struct ShowDebugLegionary {
	int way_point;
	int attack_point;
	int speed_factor;
	int invisibility;
	
	ShowDebugLegionary();
	template<class Archive>	void serialize(Archive& ar);
};

struct ShowDebugSquad {
	int position;
	int wayPoints;
	int attackPoints;
	int umbrella;
	int fire_radius;
	int optimalRadius;
	int sight_radius;
	int described_radius;
	int average_velocity;
	int linesNumber;
	int attackAction;
	int squadToFollow;
	int order;
	
	ShowDebugSquad();
	template<class Archive>	void serialize(Archive& ar);
};

struct ShowDebugFrame {
	int status;
	int inputPosition;
	
	ShowDebugFrame();
	template<class Archive>	void serialize(Archive& ar);
};

struct ShowDebugPlayer {
	int field_regions;
	int field_region_border;
	int energyColumn;
	int coresColumn;
	int zeroColumn;
	int defenceMap;
	int trustMap;
	int placeOp;
	int scanBound;
	int enemyPlayer;
	
	ShowDebugPlayer();
	template<class Archive>	void serialize(Archive& ar);
};


extern int EnableDebugKeyHandlersInitial;
extern int debug_write_mode;
extern int debug_show_mode;
extern int debug_window_sx;
extern int debug_window_sy;
extern ShowDebugRigidBody showDebugRigidBody;
extern int showIncludingCluster;
extern int show_unit_base_radius;
extern ShowDebugUnitReal showDebugUnitReal;
extern ShowDebugLegionary showDebugLegionary;
extern ShowDebugSquad showDebugSquad;
extern ShowDebugFrame showDebugFrame;
extern int show_building_status;
extern int show_core_generator_status;
extern int show_node_controller_status;
extern int show_corridor_alpha_way;
extern ShowDebugPlayer showDebugPlayer;
extern int show_field_dispatcher;
extern int show_filth_debug;
extern float show_vector_scale;
extern float show_vector_zmin;
extern float show_vector_zmax;
extern int debug_variation;
extern int mt_interface_quant;
extern int ht_intf_test;
extern bool disableHideUnderscore;
extern int terMaxTimeInterval;
extern int debug_show_briefing_log;
extern int debug_show_mouse_position;
extern int debug_show_energy_consumption;
extern int debug_show_intf_borders;
extern int debug_allow_replay;
extern int debug_show_lag_stat;

struct DebugPrm {
	DebugPrm();
	void save();
	void edit();
	template<class Archive>	void serialize(Archive& ar);

private:
	const char* debugPrmName;
};


#endif //_DEBUG_PRM_H_
