#include "StdAfx.h"
#include "DebugPrm.h"
#include "EditArchive.h"
#include "XPrmArchive.h"

int EnableDebugKeyHandlersInitial;
int debug_write_mode;
int debug_show_mode;
int debug_window_sx;
int debug_window_sy;
ShowDebugRigidBody showDebugRigidBody;
int showIncludingCluster;
int show_unit_base_radius;
ShowDebugUnitReal showDebugUnitReal;
ShowDebugLegionary showDebugLegionary;
ShowDebugSquad showDebugSquad;
ShowDebugFrame showDebugFrame;
int show_building_status;
int show_core_generator_status;
int show_node_controller_status;
int show_corridor_alpha_way;
ShowDebugPlayer showDebugPlayer;
int show_field_dispatcher;
int show_filth_debug;
float show_vector_scale;
float show_vector_zmin;
float show_vector_zmax;
int debug_variation;
int mt_interface_quant;
int ht_intf_test;
bool disableHideUnderscore;
int terMaxTimeInterval;
int debug_show_briefing_log;
int debug_show_mouse_position;
int debug_show_energy_consumption;
int debug_show_intf_borders;
int debug_allow_mainmenu_gamespy;
int debug_allow_replay;
int debug_show_lag_stat;

ShowDebugRigidBody::ShowDebugRigidBody()
{
	boundingBox = 0;
	radius = 0;
	inscribedRadius = 0;
	wayPoints = 0;
	contacts = 0;
	movingDirection = 0;
	acceleration = 0;
	accelerationScale = 0.1f;
	angularAccelerationScale = 10;
	obstaclePoint = 0;
	rocketTarget = 0;
	velocityValue = 0;
}

template<class Archive>	
void ShowDebugRigidBody::serialize(Archive& ar)
{
	ar & WRAP_OBJECT(boundingBox);
	ar & WRAP_OBJECT(radius);
	ar & WRAP_OBJECT(inscribedRadius);
	ar & WRAP_OBJECT(wayPoints);
	ar & WRAP_OBJECT(contacts);
	ar & WRAP_OBJECT(movingDirection);
	ar & WRAP_OBJECT(acceleration);
	ar & WRAP_OBJECT(accelerationScale);
	ar & WRAP_OBJECT(angularAccelerationScale);
	ar & WRAP_OBJECT(obstaclePoint);
	ar & WRAP_OBJECT(rocketTarget);
	ar & WRAP_OBJECT(velocityValue);
}

ShowDebugUnitReal::ShowDebugUnitReal()
{
	docking = 0;
	chaosCollidingFactor = 0;
	target = 0;
	fireResponse = 0;
	chain = 0;
	chainID = 0;
	toolzer = 0;
	positionValue = 0;
	unitID = 0;
}

template<class Archive>	
void ShowDebugUnitReal::serialize(Archive& ar)
{
	ar & WRAP_OBJECT(docking);
	ar & WRAP_OBJECT(chaosCollidingFactor);
	ar & WRAP_OBJECT(target);
	ar & WRAP_OBJECT(fireResponse);
	ar & WRAP_OBJECT(chain);
	ar & WRAP_OBJECT(chainID);
	ar & WRAP_OBJECT(toolzer);
	ar & WRAP_OBJECT(positionValue);
	ar & WRAP_OBJECT(unitID);
}


ShowDebugLegionary::ShowDebugLegionary()
{
	way_point = 0;
	attack_point = 0;
	speed_factor = 0;
	invisibility = 0;
}
template<class Archive>	
void ShowDebugLegionary::serialize(Archive& ar)
{
	ar & WRAP_OBJECT(way_point);
	ar & WRAP_OBJECT(attack_point);
	ar & WRAP_OBJECT(speed_factor);
	ar & WRAP_OBJECT(invisibility);
}

ShowDebugSquad::ShowDebugSquad()
{
	position = 0;
	wayPoints = 0;
	attackPoints = 0;
	umbrella = 0;
	fire_radius = 0;
	optimalRadius = 0;
	sight_radius = 0;
	described_radius = 0;
	average_velocity = 0;
	linesNumber = 0;
	attackAction = 0;
	squadToFollow = 0;
	order = 0;
}

template<class Archive>	
void ShowDebugSquad::serialize(Archive& ar)
{
	ar & WRAP_OBJECT(position);
	ar & WRAP_OBJECT(wayPoints);
	ar & WRAP_OBJECT(attackPoints);
	ar & WRAP_OBJECT(umbrella);
	ar & WRAP_OBJECT(fire_radius);
	ar & WRAP_OBJECT(optimalRadius);
	ar & WRAP_OBJECT(sight_radius);
	ar & WRAP_OBJECT(described_radius);
	ar & WRAP_OBJECT(average_velocity);
	ar & WRAP_OBJECT(linesNumber);
	ar & WRAP_OBJECT(attackAction);
	ar & WRAP_OBJECT(squadToFollow);
	ar & WRAP_OBJECT(order);
}

ShowDebugPlayer::ShowDebugPlayer()
{
	field_regions = 0;
	field_region_border = 0;
	energyColumn = 0;
	coresColumn = 0;
	zeroColumn = 0;
	defenceMap = 0;
	trustMap = 0;
	placeOp = 0;
	scanBound = 0;
	enemyPlayer = 0;
}

template<class Archive>	
void ShowDebugPlayer::serialize(Archive& ar)
{
	ar & WRAP_OBJECT(field_regions);
	ar & WRAP_OBJECT(field_region_border);
	ar & WRAP_OBJECT(energyColumn);
	ar & WRAP_OBJECT(coresColumn);
	ar & WRAP_OBJECT(zeroColumn);
	ar & WRAP_OBJECT(defenceMap);
	ar & WRAP_OBJECT(trustMap);
	ar & WRAP_OBJECT(placeOp);
	ar & WRAP_OBJECT(scanBound);
	ar & WRAP_OBJECT(enemyPlayer);
}

ShowDebugFrame::ShowDebugFrame()
{
	status = 0;
	inputPosition = 0;
}

template<class Archive>	
void ShowDebugFrame::serialize(Archive& ar)
{
	ar & WRAP_OBJECT(status);
	ar & WRAP_OBJECT(inputPosition);
}

DebugPrm::DebugPrm()
{
	EnableDebugKeyHandlersInitial = 0;
	debug_write_mode = 0;
	debug_show_mode = 0;
	debug_window_sx = 300;
	debug_window_sy = 400;
	showIncludingCluster = 0;
	show_unit_base_radius = 0;
	show_building_status = 0;
	show_core_generator_status = 0;
	show_node_controller_status = 0;
	show_corridor_alpha_way = 0;
	show_field_dispatcher = 0;
	show_filth_debug = 0;
	show_vector_scale = 30;
	show_vector_zmin = 200;
	show_vector_zmax = 4000;
	debug_variation = 0;
	mt_interface_quant = 1;
	ht_intf_test = 0;
	disableHideUnderscore = true;
	terMaxTimeInterval = 100;
	debug_show_briefing_log = 0;
	debug_show_mouse_position = 0;
	debug_show_energy_consumption = 0;
	debug_show_intf_borders = 0;
	debug_allow_mainmenu_gamespy = 0;
	debug_allow_replay = 1;
	debug_show_lag_stat = 0;

#ifndef _FINAL_VERSION_
	debugPrmName = "Debug.dat";
	XStream ff(0);
	if(ff.open(debugPrmName, XS_IN)){
		ff.close();
		XPrmIArchive(debugPrmName) >> WRAP_NAME(*this, "DebugPrm");
	}
	if(check_command_line("explore")){
		edit();
		ErrH.Exit();
	}
#endif
}

template<class Archive>	
void DebugPrm::serialize(Archive& ar)
{
	ar & WRAP_OBJECT(showDebugRigidBody);
	ar & WRAP_OBJECT(showDebugUnitReal);
	ar & WRAP_OBJECT(showDebugLegionary);
	ar & WRAP_OBJECT(showDebugSquad);
	ar & WRAP_OBJECT(showDebugFrame);
	ar & WRAP_OBJECT(showDebugPlayer);

	ar & WRAP_OBJECT(EnableDebugKeyHandlersInitial);
	ar & WRAP_OBJECT(debug_write_mode);
	ar & WRAP_OBJECT(debug_show_mode);
	ar & WRAP_OBJECT(debug_window_sx);
	ar & WRAP_OBJECT(debug_window_sy);
	ar & WRAP_OBJECT(showIncludingCluster);
	ar & WRAP_OBJECT(show_unit_base_radius);
	ar & WRAP_OBJECT(show_building_status);
	ar & WRAP_OBJECT(show_core_generator_status);
	ar & WRAP_OBJECT(show_node_controller_status);
	ar & WRAP_OBJECT(show_corridor_alpha_way);
	ar & WRAP_OBJECT(show_field_dispatcher);
	ar & WRAP_OBJECT(show_filth_debug);
	ar & WRAP_OBJECT(show_vector_scale);
	ar & WRAP_OBJECT(show_vector_zmin);
	ar & WRAP_OBJECT(show_vector_zmax);
	ar & WRAP_OBJECT(debug_variation);
	ar & WRAP_OBJECT(mt_interface_quant);
	ar & WRAP_OBJECT(ht_intf_test);
	ar & WRAP_OBJECT(disableHideUnderscore);
	ar & WRAP_OBJECT(terMaxTimeInterval);
	ar & WRAP_OBJECT(debug_show_briefing_log);
	ar & WRAP_OBJECT(debug_show_mouse_position);
	ar & WRAP_OBJECT(debug_show_energy_consumption);
	ar & WRAP_OBJECT(debug_show_intf_borders);
	ar & WRAP_OBJECT(debug_allow_mainmenu_gamespy);
	ar & WRAP_OBJECT(debug_allow_replay);
	ar & WRAP_OBJECT(debug_show_lag_stat);
}


void DebugPrm::save()
{
#ifndef _DEMO_
#ifndef _FINAL_VERSION_
	XPrmOArchive(debugPrmName) << WRAP_NAME(*this, "DebugPrm");
#endif
#endif
}
void DebugPrm::edit()
{
	EditArchive ea;
	ea.setTranslatedOnly(false);
	if(ea.edit(*this, "DebugPrm"))
		XPrmOArchive(debugPrmName) << WRAP_NAME(*this, "DebugPrm");
}

DebugPrm debugPrm;