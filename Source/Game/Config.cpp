#include "StdAfx.h"
#include "PrmEdit.h"

#include "UnitAttribute.h"
#include "InterfaceScript.h"

#include "Scripts/GameShellSq.hi"
#include "Scripts/GameShellSq.cppi"

#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define EXPORT_NS export
#else
// export is a reserved word in modern C++
//TODO maybe we should just replace all 'export' with this instead of macro
#define EXPORT_NS export_new
#endif
namespace EXPORT_NS {
#include "Scripts/InterfaceScriptExport.hi"
#include "Scripts/InterfaceScriptExport.cppi"
}

#include "Scripts/AI.cppi"
#include "Scripts/Mutation.cppi"
#include "Scripts/RigidBody.cppi"
#include "Scripts/Config.cppi"
#include "Scripts/Region.cppi"

#include "Scripts/filth.cppi"

#include "Universe.h"
#include "Player.h"
#include "RealUnit.h"
#include "IronBuilding.h"
#include "IronPort.h"
#include "IronBullet.h"

namespace EXPORT_NS {
#include "Scripts/UnitAttribute.hi"
#include "Scripts/UnitAttribute.cppi"
}

template<class D, class S>
void copy_(D& d, const S& s)
{
	xassert(sizeof(D) == sizeof(S));
	memcpy(&d, &s, sizeof(D));
}

void copy(InterfaceTV& d, const EXPORT_NS::InterfaceTV& s)
{
	d.radius = s.radius;
	d.pos_dx = s.pos_dx;
	d.pos_dy = s.pos_dy;
	d.angle_x = s.angle_x;
	d.angle_y = s.angle_y;
	d.angle_z = s.angle_z;
	d.model = s.model;
	d.channel = s.channel;
	d.channel_power_on = s.channel_power_on;
}


void copy(UnitInterfacePrm& d, const EXPORT_NS::UnitInterfacePrm& s)
{
	copy_(d.actions, s.actions);

	d.upgrade_button_id = s.upgrade_button_id;
	d.upgrading_button_id = s.upgrading_button_id;

	d.format_group = s.format_group;
	d.popup = s.popup;
}

void copy(ConsumptionData& d, const EXPORT_NS::ConsumptionData& s)
{
	d.energy = s.energy;
	d.time = s.time;
	d.priority = s.priority;
	d.name = s.name;
}

void copy(AnimationChain& d, const EXPORT_NS::AnimationChain& s)
{
	d.chainID = s.chainID;
	d.chainName = s.chainName;
	d.phase = s.phase;
	d.period = s.period; 
	d.begin = s.begin;
	d.end = s.end;
	d.periodDeviation = s.periodDeviation; 
	d.enableReverse = s.enableReverse; 
	d.counter = s.counter; 
	d.soundID = s.soundID;
}

void copy(AnimationData& d, const EXPORT_NS::AnimationData& s)
{
	d.groupID = s.groupID;
	d.groupName = s.groupName;
	for(int i = 0; i < s.size; i++){
		d.chains.push_back(AnimationChain());
		copy(d.chains.back(), s.chains[i]);
	}
	d.setPhaseRecursive = s.setPhaseRecursive;
}

void copy(SoundControllerSetup& d, const EXPORT_NS::SoundControllerSetup& s)
{
	d.ID = s.ID;
	d.name = s.name;
	d.cycled = s.cycled;
}

void copy(SoundEventSetup& d, const EXPORT_NS::SoundEventSetup& s)
{
	d.ID = s.ID;
	d.name = s.name;
	d.is3D = s.is3D;
	d.isVoice = s.isVoice;
	d.activePlayer = s.activePlayer;
	d.pauseTime = s.pauseTime;
	d.startDelay = s.startDelay;
	d.queueTime = s.queueTime;
}

void copy(terUnitEffectData& d, const EXPORT_NS::terUnitEffectData& s)
{
	d.effectID = s.effectID;
	d.effectName = s.effectName;
	d.needOrientation = s.needOrientation;
	d.startupDamage = s.startupDamage;
}

void copy(terDebrisData& d, const EXPORT_NS::terDebrisData& s)
{
	d.debrisID = s.debrisID;
	d.count = s.count;
	d.countRnd = s.countRnd;
	d.speed = s.speed;
}

void copy(terUnitEffects& d, const EXPORT_NS::terUnitEffects& s)
{
	d.libraryFileName = s.libraryFileName;
	for(int i = 0; i < s.effectNum; i++){
		d.effects.push_back(terUnitEffectData());
		copy(d.effects.back(), s.effects[i]);
	}
	for(int i = 0; i < s.debrisDataNum; i++){
		d.debrisData.push_back(terDebrisData());
		copy(d.debrisData.back(), s.debrisData[i]);
	}
	d.corpseLifeTime = s.corpseLifeTime;
	d.corpseID = s.corpseID;
	d.craterRadius = s.craterRadius;
	d.craterID = s.craterID;
	d.craterDelay = s.craterDelay;
	d.movementCraterID = s.movementCraterID;
	d.movementCraterDelta = s.movementCraterDelta;
}

void copy(terWeaponControllerSetup& d, const EXPORT_NS::terWeaponControllerSetup& s)
{
	d.objectName[0] = s.objectName[0];
	d.objectName[1] = s.objectName[1];
	d.logicObjectName[0] = s.logicObjectName[0];
	d.logicObjectName[1] = s.logicObjectName[1];
	d.targetingObjectName = s.targetingObjectName;

	d.targetingPrecision[0] = s.targetingPrecision[0];
	d.targetingPrecision[1] = s.targetingPrecision[1];

	d.turnSpeed[0] = s.turnSpeed[0];
	d.turnSpeed[1] = s.turnSpeed[1];

	d.psiLimit[0] = s.psiLimit[0];
	d.psiLimit[1] = s.psiLimit[1];

	d.thetaLimit[0] = s.thetaLimit[0];
	d.thetaLimit[1] = s.thetaLimit[1];

	d.defaultAngles[0] = s.defaultAngles[0];
	d.defaultAngles[1] = s.defaultAngles[1];
}

void copy(terWeaponSetup& d, const EXPORT_NS::terWeaponSetup& s)
{
	d.weaponType = s.weaponType;
	d.flags = s.flags;
	d.missileID = s.missileID;
	d.missileLimit = s.missileLimit;
	d.aimResetDelay = s.aimResetDelay;
	d.fireDisableTime = s.fireDisableTime;
	d.fireDelay = s.fireDelay;
	d.missileLoadDelay = s.missileLoadDelay;
	if(s.turnSuggestPrm)
		d.turnSuggestPrm.setKey(s.turnSuggestPrm->name);
	if(s.controllerSetup)
		copy(d.controllerSetup, *s.controllerSetup);
	d.dischargeSpeed = s.dischargeSpeed;
	d.fireRadius[0] = s.fireRadius[0];
	d.fireRadius[1] = s.fireRadius[1];
	d.sightRadius = s.sightRadius;
	d.accuracyRadius = s.accuracyRadius;
	d.weaponIndex = s.weaponIndex;
	d.laserWidth = s.laserWidth;
	copy(d.reloadConsumption, s.reloadConsumption);
	d.thetaForced = s.thetaForced;
	d.squadMode = s.squadMode;
}

void copy(ToolzerActionData& d, const EXPORT_NS::ToolzerActionData& s)
{
	d.actionID = s.actionID;
	d.rad = s.rad;
	d.dz = s.dz;
	d.smMode = s.smMode;
	d.hAppr = s.hAppr;
}

void copy(ToolzerStepData& d, const EXPORT_NS::ToolzerStepData& s)
{
	d.phaseID = s.phaseID;
	d.duration = s.duration;
	d.nextPhaseID = s.nextPhaseID;
	d.dr = s.dr;
	d.buildingDamageMode = s.buildingDamageMode;
	d.buildingDamageRadius = s.buildingDamageRadius;
	for(int i = 0; i < s.size; i++){
		d.actions.push_back(ToolzerActionData());
		copy(d.actions.back(), s.actions[i]);
	}
}

void copy(ToolzerSetup& d, const EXPORT_NS::ToolzerSetup& s)
{						
	for(int i = 0; i < s.size; i++){
		d.steps.push_back(ToolzerStepData());
		copy(d.steps.back(), s.steps[i]);
	}

	d.scale = s.scale;
	d.destroyOwnZeroLayer = s.destroyOwnZeroLayer;
	d.workOnChaos = s.workOnChaos;
	d.radius = s.radius;
}

void copy(ModelData& d, const EXPORT_NS::SetModelData& s)
{
	d.modelName = s.modelName;
	d.logicName = s.logicName;
	d.boundScale = s.scale;
}

void copy(AttributeBase& d, const EXPORT_NS::terUnitAttributeData& s)
{
	d.ID = s.ID;
	d.ClassID = s.ClassID;
	d.InterpolationType = s.InterpolationType;
	d.belligerent = s.belligerent;

	d.setInterfaceNameTag(s.InterfaceName);
	d.modelData.modelName = s.ModelNameArray[0]; 
	d.modelData.logicName =	s.LogicName;
	d.modelData.boundScale = s.BoundScale;
	d.modelData.boundRadius = s.BoundRadius;
	if(s.ModelNameNum > 1 && s.ModelNameArray[1]){
		d.additionalModelsData.push_back(ModelData());
		d.additionalModelsData.back().modelName = s.ModelNameArray[1];
		d.additionalModelsData.back().logicName = s.LogicName;
		d.additionalModelsData.back().boundScale = s.BoundScale;
		d.additionalModelsData.back().boundRadius = s.BoundRadius;
	}

	if(s.setModelDataArray)
		for(int i = 0; i < s.setModelDataArray->size; i++){
			if(!d.modelData.modelName){
				copy(d.modelData, s.setModelDataArray->table[i]);
				d.modelData.boundScale = s.BoundScale;
				d.modelData.boundRadius = s.BoundRadius;
			}
			else{
				d.additionalModelsData.push_back(ModelData());
				copy(d.additionalModelsData.back(), s.setModelDataArray->table[i]);
				d.additionalModelsData.back().boundScale = s.BoundScale;
				d.additionalModelsData.back().boundRadius = s.BoundRadius;
			}
		}

	for(int i = 0; i < s.ConnectionPointNameNum; i++)
		d.ConnectionPointNames.push_back(s.ConnectionPointNames[i]);
 
	for(int i = 0; i < 5; i++)
		d.damageMolecula[i] = s.damageMolecula[i];

	if(s.damageData)
		copy_(d.unitDamage, *s.damageData);
	if(s.environmentalDamage)
		copy_(d.environmentalDamage, *s.environmentalDamage);

	d.SelectAble = s.SelectAble;
	d.saveAsCommonObject = s.saveAsCommonObject; 
	
	d.GroundPlaneSpeed = s.GroundPlaneSpeed;
	d.GroundAngleSpeed = s.GroundAngleSpeed;
	
	d.MilitaryUnit = s.MilitaryUnit;
	d.UnitClass = s.UnitClass;
	d.AttackClass = s.AttackClass;
	d.enemyWorld = s.enemyWorld; 

	d.SoundName = s.SoundName;
	d.SoundCycled = s.SoundCycled;

	d.InstallBound = s.InstallBound;
	d.lockInitialPosition = s.lockInitialPosition;

	d.showPath = s.showPath;

	d.ExcludeCollision = s.ExcludeCollision;
	d.CollisionGroup = s.CollisionGroup;

	d.MakeEnergy = s.MakeEnergy; 
	d.energyCapacity = s.energyCapacity;

	d.ConnectionRadius = s.ConnectionRadius;
	d.ZeroLayerRadius = s.ZeroLayerRadius;
	d.FieldRadius = s.FieldRadius;

	d.LifeTime = s.LifeTime;

	d.constructionSpeedCoeff = s.constructionSpeedCoeff;

	d.SelectionDistance = s.SelectionDistance;
	d.SelectionSize = s.SelectionSize;

	d.MakingChainNum = s.MakingChainNum;
	d.UpgradeChainName = s.UpgradeChainName;
	d.PrevChainName = s.PrevChainName;
	d.IgnoreTargetTrace = s.IgnoreTargetTrace;

	d.ShowCircles = s.ShowCircles;

	d.enablePathFind = s.enablePathFind;
	d.enableFieldPathFind = s.enableFieldPathFind;
	d.considerInFieldPathFind = s.considerInFieldPathFind;

	d.kill_priority = s.attackablePrm.kill_priority;

	d.isUpgrade = s.isUpgrade;
	d.iconDistanceFactor = s.iconDistanceFactor;

	if(s.rigidBodyPrm){
		d.forwardVelocity = s.rigidBodyPrm->forward_velocity_max;
		d.flyingHeight = s.rigidBodyPrm->flying_height;
		d.rigidBodyPrm.setKey(s.rigidBodyPrm->name);
	}
	
	copy(d.interfacePrm, s.interfacePrm);
	if(s.interfaceTV)
		copy(d.interfaceTV, *s.interfaceTV);
    
	if(s.animationSetup)
		for(int i = 0; i < s.animationSetup->size; i++){
			d.animationDataTable.push_back(AnimationData());
			copy(d.animationDataTable.back(), s.animationSetup->data[i]);
		}
	
	if(s.soundSetup){
		for(int i = 0; i < s.soundSetup->sounds_size; i++){
			d.soundSetup.sounds.push_back(SoundControllerSetup());
			copy(d.soundSetup.sounds.back(), s.soundSetup->sounds[i]);
		}
		for(int i = 0; i < s.soundSetup->events_size; i++){
			d.soundSetup.events.push_back(SoundEventSetup());
			copy(d.soundSetup.events.back(), s.soundSetup->events[i]);
		}
	}

	copy(d.effectsData, s.effectsData);

	if(s.weaponSetup)
		copy(d.weaponSetup, *s.weaponSetup);

	if(s.toolzerSetup)
		copy(d.toolzerSetup, *s.toolzerSetup);
                   
	//---------------------------------------
	AttributeReal* dr = dynamic_cast<AttributeReal*>(&d);
	if(dr){
		if(s.dockingSlots)
			for(int i = 0; i < s.dockingSlots->size; i++)
				dr->dockingSlots.push_back(s.dockingSlots->data[i].position);
		
		if(s.productionConsumption)
			copy(dr->productionConsumption, *s.productionConsumption);
		if(s.movementConsumption)
			copy(dr->movementConsumption, *s.movementConsumption);

		dr->FallDelay = s.FallDelay;
		dr->FallSpeed = s.FallSpeed;
		dr->FallAcceleration = s.FallAcceleration;
	}

	AttributeBuilding* db = dynamic_cast<AttributeBuilding*>(&d);
	if(db){
		for(int i = 0; i < s.enableData.StructureNum; i++)
			db->EnableStructure.push_back(s.enableData.EnableStructure[i]);
		for(int i = 0; i < s.enableData.DowngradesNum; i++)
			db->Downgrades.push_back(s.enableData.Downgrades[i]); 
		db->Upgrade = s.enableData.Upgrade; 

		db->constructionPriority = s.constructionPriority;
		db->disconnectAnimation = s.disconnectAnimation; 
	}

	AttributeLegionary* dl = dynamic_cast<AttributeLegionary*>(&d);
	if(dl){
		dl->is_base_unit = s.legionaryPrm.is_base_unit;
		dl->formationRadiusFactor = s.legionaryPrm.formationRadiusFactor;

		dl->dynamicAttack = s.legionaryPrm.dynamicAttack;
		dl->attackTurnRadius = s.legionaryPrm.attackTurnRadius;
		dl->attackAngleDeviation = s.legionaryPrm.attackAngleDeviation;

		dl->ignoreUninstalledFrame = s.legionaryPrm.ignoreUninstalledFrame;
		dl->dontUseDamageMapPathFind = s.legionaryPrm.dontUseDamageMapPathFind;

		dl->LegionType = s.LegionType;
		dl->destroyZeroLayer = s.destroyZeroLayer;

		for(int i = 0; i < s.enableData.StructureNum; i++)
			dl->EnableStructure.push_back(s.enableData.EnableStructure[i]);
	}

	AttributeFrame* df = dynamic_cast<AttributeFrame*>(&d);
	if(df){
		xassert(s.framePrm);
		
		df->heightMax = s.framePrm->heightMax;
		df->accumulatedEnergyInitial = s.framePrm->accumulatedEnergyInitial;
		
		copy(df->antigravConsumption, s.framePrm->antigravConsumption);
		copy(df->movementConsumption, s.framePrm->movementConsumption);
		copy(df->annihilationConsumption, s.framePrm->annihilationConsumption);
		copy(df->repairConsumption, s.framePrm->repairConsumption);
		
		df->repairElementsPerQuant = s.framePrm->repairElementsPerQuant;

		copy_(df->fallDamage, s.framePrm->fallDamage);

		df->oneStepMovement = s.framePrm->oneStepMovement;
		df->energyReserveToChargeSpiral = s.framePrm->energyReserveToChargeSpiral;
		df->mmpHomePositionRadius = s.framePrm->mmpHomePositionRadius;
		df->normalVelocity = s.framePrm->normalVelocity;
		df->teleportationVelocity = s.framePrm->teleportationVelocity;
	}

	AttributeCorridor* dc = dynamic_cast<AttributeCorridor*>(&d);
	if(dc){
		xassert(s.corridorPrm);

		dc->disappearingTime = s.corridorPrm->disappearingTime;
		dc->disappearingHeight = s.corridorPrm->disappearingHeight;
		dc->distanceThreshould = s.corridorPrm->distanceThreshould;
		dc->moveFrame = s.corridorPrm->moveFrame;

		dc->potentialToAcceleration = s.corridorPrm->potentialToAcceleration;
		dc->accelerationDenominator = s.corridorPrm->accelerationDenominator;
		dc->pathVisualizationDelta = s.corridorPrm->pathVisualizationDelta;
	}
}

void copyAttributes()
{
	for(int i = 0; i < EXPORT_NS::unitAttributeDataTable.size(); i++){
		const EXPORT_NS::terUnitAttributeData& src = EXPORT_NS::unitAttributeDataTable[i];
		ShareHandle<AttributeBase> data = 0;
		if(src.ID == UNIT_ATTRIBUTE_FRAME)
			data = new AttributeFrame;
		else if(src.ID == UNIT_ATTRIBUTE_SQUAD)
			data = new AttributeSquad;
		else if(src.ID == UNIT_ATTRIBUTE_CORRIDOR_ALPHA || src.ID == UNIT_ATTRIBUTE_CORRIDOR_OMEGA)
			data = new AttributeCorridor;
		else if(isBuilding(src.ID))
			data = new AttributeBuilding;
		else if(isLegionary(src.ID))
			data = new AttributeLegionary;
		else if(src.UnitClass == UNIT_CLASS_MISSILE)
			data = new AttributeProjectile;
		else if(src.InterpolationType == INTERPOLATION_ID_REAL)
			data = new AttributeReal;
        else                             
			data = new AttributeBase;

		copy(*data, src);
		data->init();
		attributeLibrary().add(AttributeIDBelligerent(data->ID, data->belligerent), data);
		if(data->belligerent != BELLIGERENT_NONE &&
		  !attributeLibrary().find(AttributeIDBelligerent(data->ID, BELLIGERENT_NONE)))
			attributeLibrary().add(AttributeIDBelligerent(data->ID, BELLIGERENT_NONE), data);
	}
}

void copy(RigidBodyPrm::Oscillator& d, const EXPORT_NS::RigidBodyPrm::Oscillator& s)
{
	d.phase = s.phase;
	d.amplitude = s.amplitude;
	d.omega = s.omega;
	d.amplitude_decrement = s.amplitude_decrement;
	d.omega_increment = s.omega_increment;
	d.omega_disperse = s.omega_disperse;
}

void copy(RigidBodyPrm::Stiffness& d, const EXPORT_NS::RigidBodyPrm::Stiffness& s)
{
	d.stiffness_up = s.stiffness_up;
	d.stiffness_down = s.stiffness_down;
	d.dz_max_up = s.dz_max_up;
	d.dz_max_down = s.dz_max_down;
}

void copy(RigidBodyPrm& d, const EXPORT_NS::RigidBodyPrm& s)
{
	copy_(d.unit_type, s.unit_type);
	d.name = s.name;
	copy_(d.linear_damping, s.linear_damping);
	d.angular_damping = s.angular_damping;
	d.gravity = s.gravity;
	d.polyhedral_bound = s.polyhedral_bound;
	d.density = s.density;
	d.TOI_factor = s.TOI_factor;
	d.radius_min = s.radius_min;
	d.unmovable = s.unmovable;
	d.enable_sleeping = s.enable_sleeping;
	d.isotropic = s.isotropic;
	d.controled_by_points = s.controled_by_points;
	d.analyse_force_field_obstacle = s.analyse_force_field_obstacle;
	d.analyse_body_obstacle = s.analyse_body_obstacle;
	d.analyse_terraint_obstacle = s.analyse_terraint_obstacle;
	d.obstacle_restitution = s.obstacle_restitution;
	d.obstacle_relaxation = s.obstacle_relaxation;
	d.avoid_obstacle_rudder_speed = s.avoid_obstacle_rudder_speed;
	d.brake_damping = s.brake_damping;
	d.point_control_slow_distance = s.point_control_slow_distance;
	d.point_control_slow_factor = s.point_control_slow_factor;
	d.rudder_speed = s.rudder_speed;
	d.suppress_steering_duration = s.suppress_steering_duration;
	d.steering_linear_velocity_min = s.steering_linear_velocity_min;
	d.steering_acceleration_max = s.steering_acceleration_max;
	d.is_point_reached_radius_max = s.is_point_reached_radius_max;
	d.forward_velocity_max = s.forward_velocity_max;
	d.forward_acceleration = s.forward_acceleration;
	d.orientation_torque_factor = s.orientation_torque_factor;
	d.speed_map_factor = s.speed_map_factor;
	d.analyze_points_density = s.analyze_points_density;
	d.Dxy_minimal = s.Dxy_minimal;
	d.minimal_velocity_xy = s.minimal_velocity_xy;
	d.zero_velocity_z = s.zero_velocity_z;
	d.box_delta_y = s.box_delta_y;
	d.gravicap_enabled = s.gravicap_enabled;
	d.gravicap_pitch_roll_threshould = s.gravicap_pitch_roll_threshould;
	d.gravicap_dz_treshould = s.gravicap_dz_treshould;
	copy(d.gravicap_stiffness, s.gravicap_stiffness);
	copy(d.gravicap_oscillator_z, s.gravicap_oscillator_z);
	copy(d.gravicap_oscillator_pitch, s.gravicap_oscillator_pitch);
	copy(d.gravicap_oscillator_roll, s.gravicap_oscillator_roll);
	d.kangaroo_property = s.kangaroo_property;
	d.kangaroo_allow_on_me = s.kangaroo_allow_on_me;
	d.kangaroo_delta_z = s.kangaroo_delta_z;
	d.digging_depth = s.digging_depth;
	d.digging_depth_velocity = s.digging_depth_velocity;
	d.flying_down_without_way_points = s.flying_down_without_way_points;
	d.flying_height_relative = s.flying_height_relative;
	d.flying_height = s.flying_height;
	copy(d.flying_stiffness, s.flying_stiffness);
	copy(d.flying_oscillator_z, s.flying_oscillator_z);
	d.flying_vertical_direction_x_factor = s.flying_vertical_direction_x_factor;
	d.flying_vertical_direction_y_factor = s.flying_vertical_direction_y_factor;
	d.minimize_theta = s.minimize_theta;
	d.upper_theta = s.upper_theta;
	d.lower_theta = s.lower_theta;
	d.distance_correction_factor = s.distance_correction_factor;
	d.keep_direction_time = s.keep_direction_time;
	d.ground_colliding_delay = s.ground_colliding_delay;
	d.rocket_vertical_control_distance = s.rocket_vertical_control_distance;
	d.rocket_forward_analysis_distance = s.rocket_forward_analysis_distance;
	d.rocket_target_offset_z = s.rocket_target_offset_z;
	d.debris_angular_velocity = s.debris_angular_velocity;
	d.enable_show = s.enable_show;
}

void copyRigidBodyTable()
{
	for(int i = 0; i < EXPORT_NS::rigidBodyTable.size(); i++){
		ShareHandle<RigidBodyPrm> data = new RigidBodyPrm;
		copy(*data, EXPORT_NS::rigidBodyTable[i]);
		rigidBodyPrmLibrary().add((const char*)data->name, data);
	}
}

void copy(ControlAction& d, const EXPORT_NS::ControlAction& s)
{
	d.name = s.name;
	d.code = s.code;
	d.param = s.param;
	d.hotKeyPopup = s.hotKeyPopup;
}

void copy(BGObj& d, const EXPORT_NS::BGObj& s)
{
	d.name = s.name;
	d.chainName = s.chainName;
}

void copy(sqshImage& d, const EXPORT_NS::sqshImage& s)
{
	d.texture = s.texture;
	d._ix = s._ix;
	d._iy = s._iy;
	d.ix = s.ix;
	d.iy = s.iy;
	d.dx = s.dx;
	d.dy = s.dy;
	d.dsx = s.dsx;
	d.dsy = s.dsy;
	d.hasResolutionVersion = s.hasResolutionVersion;
	d.hasBelligerentVersion = s.hasBelligerentVersion;
}

void copy(sqshControl& d, const EXPORT_NS::sqshControl& s)
{
	d.type = s.type;
	d.id = s.id;

	d.state = s.state;
	d.x = s.x;
	d.y = s.y;
	d.sx = s.sx;
	d.sy = s.sy;
	d.xstart = s.xstart;	
	d.ystart = s.ystart;	

	copy(d.image,s.image);
	copy(d.image_h,s.image_h);
	copy(d.image_check,s.image_check);
	copy(d.image_disabled,s.image_disabled);

	copy(d.image2,s.image2);
	copy(d.image2_h,s.image2_h);
	copy(d.image2_check,s.image2_check);
	copy(d.image2_disabled,s.image2_disabled);

	d.sound = s.sound;
	d.popup = s.popup;
	d.popupChecked = s.popupChecked;
	d.target = s.target;
	d.parent_id = s.parent_id;

	d.diagram_delta_x = s.diagram_delta_x;
	d.diagram_delta_sx = s.diagram_delta_sx;
	d.diagram_delta_y = s.diagram_delta_y;
	d.diagram_delta_sy = s.diagram_delta_sy;

	copy(d.bgObject,s.bgObject);

	d.text = s.text;
	d.txt_dx = s.txt_dx;
	d.txt_dy = s.txt_dy;
	d.txt_align = s.txt_align;
	d.txt_vert_align = s.txt_vert_align;
	d.font_group = s.font_group;

	d.flashR = s.flashR;
	d.flashG = s.flashG;
	d.flashB = s.flashB;
	d.flashA = s.flashA;

	d.flashBG_R = s.flashBG_R;
	d.flashBG_G = s.flashBG_G;
	d.flashBG_B = s.flashBG_B;
	d.flashBG_A = s.flashBG_A;

	d.tickTime = s.tickTime;

	d.diaR = s.diaR;
	d.diaG = s.diaG;
	d.diaB = s.diaB;
	d.diaA = s.diaA;
	
	d.actions.resize(s.actionCount);
	for(int i = 0; i < s.actionCount; i++)
		copy(d.actions[i], s.actions[i]);

	d.passive = s.passive;
	d.flashWithImage = s.flashWithImage;
	d.alnum = s.alnum;
	
	d.hitTestMode = s.hitTestMode;

	d.canHandleDisabledInput = s.canHandleDisabledInput;
}

void copy(sqshControlContainer& d, const EXPORT_NS::sqshControlContainer& s)
{
	d.type = s.type;
	d.id = s.id;
	d.state = s.state;
	d.load_group = s.load_group;
	d.x = s.x;
	d.y = s.y;
	d.sx = s.sx;
	d.sy = s.sy;
	copy(d.image,s.image);
	copy(d.image2,s.image2);

	d.controls.resize(s.ControlCount);
	for(int i = 0; i < s.ControlCount; i++)
		copy(d.controls[i], s.controls[i]);

	d.bgObjects.resize(s.bgObjCount);
	for(int j = 0; j < s.bgObjCount; j++)
		copy(d.bgObjects[j], s.bgObjects[j]);

	d.defaultEnterBtnID = s.defaultEnterBtnID;
	d.defaultEnterBtnIDAux = s.defaultEnterBtnIDAux;
	d.defaultEscBtnID = s.defaultEscBtnID;
}

void copy(TabIcon& d, const EXPORT_NS::TabIcon& s)
{
	copy(d.image,s.image);
	copy(d.image_h,s.image_h);
	copy(d.image_check,s.image_check);
	copy(d.image_disabled,s.image_disabled);
	copy(d.image_flashing,s.image_flashing);
}

void copy(sqshTabElement& d, const EXPORT_NS::sqshTabElement& s)
{
	d.x = s.x;
	d.y = s.y;
	d.sx = s.sx;
	d.sy = s.sy;

	copy(d.icon,s.icon); 

	d.activeX = s.activeX;
	d.activeY = s.activeY;
	d.activeSX = s.activeSX;
	d.activeSY = s.activeSY;

	copy(d.activeTabImage,s.activeTabImage);

	d.ctrls.resize(s.ctrl_count);
	for(int i = 0; i < s.ctrl_count; i++) d.ctrls[i] = s.ctrls[i];
}

void copy(sqshTabSheet& d, const EXPORT_NS::sqshTabSheet& s)
{
	d.id = s.id;
	d.x = s.x;
	d.y = s.y;
	d.sx = s.sx;
	d.sy = s.sy;

	d.dx = s.dx;
	d.dy = s.dy;
	d.dsx = s.dsx;
	d.dsy = s.dsy;

	d.pages.resize(s.page_count);
	for(int i = 0; i < s.page_count; i++)
		copy(d.pages[i],s.pages[i]);

	d.actions.resize(s.actionCount);
	for(int j = 0; j < s.actionCount; j++)
		copy(d.actions[j],s.actions[j]);

	d.flashR = s.flashR;
	d.flashG = s.flashG;
	d.flashB = s.flashB;
	d.flashA = s.flashA;

	d.flashBG_R = s.flashBG_R;
	d.flashBG_G = s.flashBG_G;
	d.flashBG_B = s.flashBG_B;
	d.flashBG_A = s.flashBG_A;

	d.tickTime = s.tickTime;
}
/*
void copy(sqshCursor& d, const EXPORT_NS::sqshCursor& s)
{
	d.system = s.system;
	d.hotspot_center = s.hotspot_center;
	d.image = s.image;
	d.sx = s.sx;
	d.sy = s.sy;
}

void copy(ShellGalogramType& d, const EXPORT_NS::ShellGalogramType& s)
{
	d.model = s.model;
	d.radius = s.radius;
	d.pos_dx = s.pos_dx;
	d.pos_dy = s.pos_dy;
	d.angle_x = s.angle_x;
	d.angle_y = s.angle_y;
	d.angle_z = s.angle_z;

	d.r1 = s.r1;
	d.g1 = s.g1;
	d.b1 = s.b1;

	d.r2 = s.r2;
	d.g2 = s.g2;
	d.b2 = s.b2;

	d.camera_x = s.camera_x;
	d.camera_y = s.camera_y;
	d.camera_sx = s.camera_sx;
	d.camera_sy = s.camera_sy;
	d.model_rate = s.model_rate;
}

void copy(EventSymbol& d, const EXPORT_NS::EventSymbol& s)
{
	d.image = s.image;
	d.sx = s.sx;
	d.sy = s.sy;
	d.period = s.period;
	d.animTime = s.animTime;
	d.snd = s.snd;
}

void copy(_ProgressBar& d, const EXPORT_NS::_ProgressBar& s)
{
	copy_(d.points,s.points);
}

void copy(unitClassDesc& d, const EXPORT_NS::unitClassDesc& s)
{
	d.unitClass = s.unitClass;
	d.textID = s.textID;
}

void copy(Reel& d, const EXPORT_NS::Reel& s)
{
	d.name = s.name;
	d.abortEnabled = s.abortEnabled;
	d.localized = s.localized;
	d.video = s.video;
	d.time = s.time;
}
*/

void copyInterfaceAttributes()
{
	if(EXPORT_NS::squad_icon_count){
		interfaceAttr().tabIcons.resize(EXPORT_NS::squad_icon_count);
		for(int i = 0; i < EXPORT_NS::squad_icon_count; i++)
			copy(interfaceAttr().tabIcons[i],EXPORT_NS::_squad_icons_table[i]);
	}
	else
		interfaceAttr().tabIcons.clear();

	if(EXPORT_NS::_sqsh_sheet_count){
		interfaceAttr().tabSheets.resize(EXPORT_NS::_sqsh_sheet_count);
		for(int i = 0; i < EXPORT_NS::_sqsh_sheet_count; i++)
			copy(interfaceAttr().tabSheets[i],EXPORT_NS::_sqsh_sheets[i]);
	}
	else
		interfaceAttr().tabSheets.clear();

	if(EXPORT_NS::_sqsh_control_count){
		interfaceAttr().controls.resize(EXPORT_NS::_sqsh_control_count);
		for(int i = 0; i < EXPORT_NS::_sqsh_control_count; i++)
			copy(interfaceAttr().controls[i],EXPORT_NS::_sqsh_controls[i]);
	}
	else
		interfaceAttr().controls.clear();
}

UnitInterfacePrm interface_squad1_prm;
UnitInterfacePrm interface_squad3_prm;
UnitInterfacePrm interface_frame_uninstalled;

void copyInterfaceAttributesIndispensable()
{
	copy(interface_squad1_prm, EXPORT_NS::interface_squad1_prm);
	copy(interface_squad3_prm, EXPORT_NS::interface_squad3_prm);
	copy(interface_frame_uninstalled, EXPORT_NS::interface_frame_uninstalled);
}
