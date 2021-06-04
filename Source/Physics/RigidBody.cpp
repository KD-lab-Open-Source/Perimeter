#include "stdafx.h"
#include "terra.h"
#include "RigidBody.h"
#include "ForceField.h"
#include "Universe.h"
#include "EditArchive.h"
#include "XPrmArchive.h"
#include "BinaryArchive.h"

int RigidBody::IDs;

SINGLETON_PRM(RigidBodyPrmLibrary, "RigidBodyPrmLibrary", "Scripts\\RigidBodyPrmLibrary") rigidBodyPrmLibrary;
REGISTER_CLASS(RigidBodyPrm, RigidBodyPrm, "Базовая физика");

RigidBody::RigidBody()
{
	//ID = ++IDs;
	posePrev_ = Se3f::ID;
	setPose(Se3f::ID);
	composition_ = Vect3f::ZERO;
	diffuse_color.set(1,1,1,1);
	prm_ = 0;
}

RigidBody::~RigidBody() 
{ 
}

void RigidBody::build(const RigidBodyPrm& prmIn, cObjectNodeRoot* geometry_, const Vect3f& box_min_, const Vect3f& box_max_)
{
	prm_ = &prmIn;
	geometry = geometry_;
	if(geometry)
		scale = geometry->GetScale();

	setBound(box_min_, box_max_);

	flying_oscillator_z = prm().flying_oscillator_z;
	gravicap_oscillator_z = prm().gravicap_oscillator_z;
	gravicap_oscillator_pitch = prm().gravicap_oscillator_pitch;
	gravicap_oscillator_roll = prm().gravicap_oscillator_roll;

	flying_oscillator_z.set();
	gravicap_oscillator_z.set();
	gravicap_oscillator_pitch.set();
	gravicap_oscillator_roll.set();

	unmovable_ = prm().unmovable;
	setForwardVelocity(prm().forward_velocity_max);
	setFlyingHeight(prm().flying_height);

	velocity_ = angularVelocity_ = Vect3f::ZERO;
	acceleration = angular_acceleration = Vect3f::ZERO;
	linear_damping = prm().linear_damping;
	angular_damping = prm().angular_damping;
	average_forward_velocity = 0;
	general_velocity_factor = 0;
	including_cluster = 0;
	ground_colliding = 0;
	cluster_colliding = 0;
	chaos_colliding = 0;
	chaosCollidingFactor_ = 0;
	deltaZ_ = 0;
	dz_max_avr = 0;
	kangaroo_height = 0;
	kangaroo_height_prev = 0;
	kangaroo_mode = 0;
	obstacle_turn_avr = 0;
	gravicap_mode = 0;
	flying_mode = 0;
	diggingMode_ = 0;
	velocity_factor = 1;
	underMutation_ = 0;
	obstacle_point_avr.set(0, 0, 0);
	obstacle_point_counter = 0;
	missile_started = false;
	restart_missile = false;
	average_movement = 1;
	set_debug_color(CYAN);
	velocityDeltaSum_ = Vect3f::ZERO;
	velocityDelta_ = Vect3f::ZERO;
	penetrationSum_ = 0;
}

void RigidBody::initPose(const Se3f& pose, bool modify_z)
{
	setPose(pose);
	if(modify_z)
	{
		Vect3f dpx, dpy;
		Vect3f p0 = box_min;
		p0.y += prm().box_delta_y;
  		matrix().xformPoint(p0);
		rotation().xform(Vect3f((box_max.x - box_min.x)/(2*Dx), 0, 0), dpx);
		rotation().xform(Vect3f(0, (box_max.y - box_min.y)/(2*Dy), 0), dpy);

		const int shl = 12, mul = 1 << shl;
		int p0x = round(p0.x*mul), p0y = round(p0.y*mul), p0z = round(p0.z*mul);
		int dpx_x = round(dpx.x*mul), dpx_y = round(dpx.y*mul), dpx_z = round(dpx.z*mul);
		int dpy_x = round(dpy.x*mul), dpy_y = round(dpy.y*mul),	dpy_z = round(dpy.z*mul);
		int z_max = 0;

		for(int y = -Dy; y <= Dy; y++)
		{
			int px = p0x, py = p0y, pz = p0z;
			for(int x = -Dx; x <= Dx; x++)
			{
				int z = vMap.GVBuf[vMap.offsetGBufC(px >> (shl+kmGrid),	py >> (shl+kmGrid))];
				if(z_max < z)
					z_max = z;

				px += dpx_x;
				py += dpx_y;
				pz += dpx_z;
			}
			p0x += dpy_x;
			p0y += dpy_y;
			p0z += dpy_z;
		}

		float positionZ = z_max;
		if(flyingMode()){
			positionZ += flyingHeight();
		}
		else if(diggingMode()){
			positionZ += deltaZ_ = -prm().digging_depth;
		}
		else{
			if(prm().kangaroo_property && !unmovable()){
				float z = GetRealHeight(position().x, position().y, radius()) + prm().kangaroo_delta_z;
				if(positionZ < z){
					kangaroo_height = positionZ = z;
					kangaroo_mode = 1;
					}
				}
		}
		positionZ -= box_min.z;
		setPosition(Vect3f(position().x, position().y, positionZ));
	}

	EulerEvolve(0);
}

void RigidBody::initPose(const Se3f& pose)
{
	setPose(pose);
	EulerEvolve(0);
}

void RigidBody::EulerEvolve(float dt)
{
	posePrev_ = pose();

	Vect3f pos = position();
	pos.scaleAdd(velocity(), dt);
	setPosition(pos);

	QuatF quat = orientation();
	Vect3f wdt;
	wdt.scale(angularVelocity(), 0.5f*dt);
	quat.s() += -quat.x() * wdt.x - quat.y() * wdt.y - quat.z() * wdt.z,
	quat.x() += quat.s() * wdt.x + quat.z() * wdt.y - quat.y() * wdt.z,
	quat.y() += -quat.z() * wdt.x + quat.s() * wdt.y + quat.x() * wdt.z,
	quat.z() += quat.y() * wdt.x - quat.x() * wdt.y + quat.s() * wdt.z;
	quat.normalize();

	// Linear damping anisotropic - apply in local frame
	rotation().invXform(velocity_);
	velocity_.x *= 1.f - linear_damping.x*dt;
	velocity_.y *= 1.f - linear_damping.y*dt;
	velocity_.z *= 1.f - linear_damping.z*dt;
	rotation().xform(velocity_);

	acceleration.z -= prm().gravity;
	velocity_.scaleAdd(acceleration, dt);

	// Apply isotropic angular damping and acceleration
	angularVelocity_.scale(1.f - angular_damping*dt);
	angularVelocity_.scaleAdd(angular_acceleration, dt);

	if(showDebugRigidBody.acceleration){
		show_vector(position(), acceleration*showDebugRigidBody.accelerationScale, GREEN);
		show_vector(position(), angular_acceleration*showDebugRigidBody.angularAccelerationScale, BLUE);
	}

	acceleration.set(0, 0, 0);
	angular_acceleration.set(0, 0, 0);

	// Restore dampings wich can be changed by logic
	linear_damping = prm().linear_damping;
	angular_damping = prm().angular_damping;
	//setForwardVelocity(prm().forward_velocity_max);

	setOrientation(quat);
}

void RigidBody::ground_analysis(float dt)
{
	start_timer_auto(ground_analysis, STATISTICS_GROUP_PHYSICS);

	int Sz = 0, Sxz = 0, Syz = 0, dz_max = 0, z_max = 0, z_min = 100000;
	float kx = (box_max.x - box_min.x)/(2*Dx);
	float ky = (box_max.y - box_min.y)/(2*Dy);
	Vect3f dpx, dpy;
	Vect3f p0 = box_min;
	p0.y += prm().box_delta_y;
  	matrix().xformPoint(p0);
	rotation().xform(Vect3f(kx, 0, 0), dpx);
	rotation().xform(Vect3f(0, ky, 0), dpy);
	int obstacle_x = 0, obstacle_y = 0, obstacle_counter = 0;
	int chaosCollidingCounter = 0;

	const int shl = 12, mul = 1 << shl;
	int p0x=round(p0.x*mul),
		p0y=round(p0.y*mul),
		p0z=round(p0.z*mul);

	int dpx_x=round(dpx.x*mul),
		dpx_y=round(dpx.y*mul),
		dpx_z=round(dpx.z*mul);

	int dpy_x=round(dpy.x*mul),
		dpy_y=round(dpy.y*mul),
		dpy_z=round(dpy.z*mul);

	for(int y = -Dy; y <= Dy; y++)
	{
		int px=p0x, py=p0y, pz=p0z;
		for(int x = -Dx; x <= Dx; x++)
		{
			int zp = pz >> shl;
			int z = vMap.GVBuf[vMap.offsetGBufC(px >> (shl+kmGrid),	py >> (shl+kmGrid))];
			
			if(z == 0){
				//obstacle_x += x;
				//obstacle_y += y;
				//obstacle_counter++;
				chaosCollidingCounter++;
				}
			if(z_max < z)
				z_max = z;
			if(z_min > z)
				z_min = z;

			int dz = z - zp;
			if(dz_max < dz)
				dz_max = dz;

			Sz += z;
			Sxz += x*z;
			Syz += y*z;

			px += dpx_x;
			py += dpx_y;
			pz += dpx_z;
		}
		p0x += dpy_x;
		p0y += dpy_y;
		p0z += dpy_z;
	}

	Vect3f z_axis;
	float dZ = -box_min.z + deltaZ_ - position().z;

	chaos_colliding = chaosCollidingCounter;
	chaosCollidingFactor_ = chaosCollidingCounter/float((2*Dx + 1)*(2*Dy + 1));

	if(flyingMode()){ // Леталка
		gravicap_mode = 1;
		ground_colliding = 0;
		set_debug_color(YELLOW);

		if(on_upper_position()){
			if(kangaroo_height){
				kangaroo_height_prev = kangaroo_height;
				kangaroo_height = 0;
			}
			//z_max = max(z_max, kangaroo_height_prev);
			//dz_max = max(dz_max, kangaroo_height_prev - round(position().z + box_min.z));
		}

		if(prm().flying_height_relative)
			dZ += z_max;
		dZ += flyingHeight();
		acceleration.z += prm().flying_stiffness.force(dZ);
		acceleration.z += flying_oscillator_z(dt, average_forward_velocity);

		z_axis = Vect3f::K - rotation().xcol()*prm().flying_vertical_direction_x_factor*angularVelocity().z;
		z_axis.x += velocity().x*prm().flying_vertical_direction_y_factor;
		z_axis.y += velocity().y*prm().flying_vertical_direction_y_factor;
		z_axis.normalize();
	}
	else{
		// Находим нормаль
		float t14 = Dx*Dy;
		float t13 = 2.0f*Dx+1.0f;
		float t9 = 2.0f*Dy;
		float t8 = 6.0f*t14;
		float N = 4.0f*t14+t9+t13;
		float A = 3.0f*Sxz/Dx/(t9+1.0f+t8+(3.0f+(4.0f*Dy+2.0f)*Dx)*Dx)*kx;
		float B = 3.0f*Syz/Dy/(t8+t13+(3.0f+(4.0f*Dx+2.0f)*Dy)*Dy)*ky;
		float psi = atan2f(rotation()[0][1], rotation()[1][1]);
		float cos_psi = cosf(psi);
		float sin_psi = sinf(psi);
		z_axis.set(-A*cos_psi - B*sin_psi, A*sin_psi - B*cos_psi, 1);
		int zc = round(position().z + box_min.z);

		if(prm().analyse_terraint_obstacle && obstacle_counter){
			Vect3f point((obstacle_x*cos_psi + obstacle_y*sin_psi)/obstacle_counter, (-obstacle_x*sin_psi + obstacle_y*cos_psi)/obstacle_counter, 0);
			point += position();
			add_obstacle_point(point);
		}

		if(underMutation()){
			velocity_.x = velocity_.y = 0;
			if(prm().kangaroo_property){
				float z = GetRealHeight(position().x, position().y, radius()) + prm().kangaroo_delta_z;
				if(z_max < z){
					kangaroo_height = z_max = z;
					kangaroo_mode = 1;
				}
			}
		}
		
		if((prm().gravicap_enabled && !diggingMode() && 
		  (fabs(z_axis.x) > prm().gravicap_pitch_roll_threshould || fabs(z_axis.y) > prm().gravicap_pitch_roll_threshould 
		  || z_max - zc > prm().gravicap_dz_treshould || zc - z_min > prm().gravicap_dz_treshould)) 
		  || on_upper_position()){ // Гравицапа
			gravicap_mode = 1;
			set_debug_color(MAGENTA);
	
			if(on_upper_position()){
				set_debug_color(GREEN);
				if(kangaroo_height){
					kangaroo_height_prev = kangaroo_height;
					kangaroo_height = 0;
				}
				z_max = max(z_max, kangaroo_height_prev);
				dz_max = max(dz_max, kangaroo_height_prev - zc);
			}

			dZ += z_max;
			acceleration.z += prm().gravicap_stiffness.force(dZ);
			
			acceleration.z += gravicap_oscillator_z(dt, average_forward_velocity);
			angular_acceleration.scaleAdd(rotation().xcol(), gravicap_oscillator_pitch(dt, average_forward_velocity));
			angular_acceleration.scaleAdd(rotation().ycol(), gravicap_oscillator_roll(dt, average_forward_velocity));
			z_axis = Vect3f::K;
		}
		else{ // Наземный режим
			gravicap_mode = 0;
			ground_colliding = 1;
			set_debug_color(CYAN);
			average(dz_max_avr, dz_max, dz_max_avr_tau);
			dZ += Sz/N + dz_max_avr;

			float v_norm = velocity().norm() + prm().zero_velocity_z;
			velocity_.z = dZ/dt;
			if(velocity().norm() > v_norm)
				velocity_.normalize(v_norm);
			
			z_axis.Normalize();
		}
	}

	average(kangaroo_mode, 0, kangaroo_mode_avr_tau);

	if(prm().controled_by_points){
		if(!prm().isotropic)
			apply_control_force();
		else
			apply_control_force_isotropic();
	}
	else{
		float v_forward = Vect2f(velocity()).norm();
		average(average_forward_velocity, v_forward, average_forward_velocity_tau);
		general_velocity_factor = 1;
	}

	// Устраняет ступор перед вертикальными стенками
	float norm_xy = sqrtf(sqr(velocity().x) + sqr(velocity().y));
	if((norm_xy < prm().minimal_velocity_xy || gravicap_mode && dz_max > deep_penetration_dz) && norm_xy > FLT_EPS){
		float k = prm().minimal_velocity_xy*general_velocity_factor/norm_xy;
		velocity_.x *= k;
		velocity_.y *= k;
		}

	apply_levelling_torque(rotation().zcol(), z_axis);

	applyDiggingForce();

	obstacle_analysis();
}

bool RigidBody::kangarooEnabled() const 
{
	if(diggingMode() || flyingMode())
		return false;
	return prm().kangaroo_property && !unmovable() && (prm().kangaroo_allow_on_me || controlled());
}

void RigidBody::apply_levelling_torque(const Vect3f& current_axis, const Vect3f& target_axis)
{
	// 1. Axes should be in global frame
	// 2. Parameter orientation_torque_factor is used
	Vect3f cross = current_axis % target_axis;
	float len = cross.norm();
	if(len > FLT_EPS)
		angular_acceleration.scaleAdd(cross, Acos(dot(current_axis, target_axis)/(current_axis.norm()*target_axis.norm() + 1e-5))*prm().orientation_torque_factor/len);
}

void RigidBody::apply_control_force()
{
	float v_local_y = dot(rotation().ycol(), velocity());
	average(average_forward_velocity, v_local_y, average_forward_velocity_tau);

	// Коэффициент скорости - замедление перед последней точкой
	general_velocity_factor = 1;
	switch(way_points.size())
	{
	case 0:
		general_velocity_factor = 0;
		break;
	case 1:
		{
			Vect2f delta = way_points.front() - position();
			float d = delta.norm();
			if(d < prm().point_control_slow_distance){
				d /= prm().point_control_slow_distance;
				general_velocity_factor = prm().point_control_slow_factor*(1 - d) + d;
				}
		}
		break;
	default:
		general_velocity_factor = 1;
	}
	
	linear_damping.y += prm().brake_damping*(1.f - general_velocity_factor);
	general_velocity_factor *= velocity_factor;

	if(!way_points.empty()){
		Vect2f dir = way_points.front() - position();
		dir = Mat2f(angleZ()).invXform(dir);
		dir.x = -dir.x;

		float rudder = fabs(dir.y) > steering_control_min_distance ? atan2(dir.x, dir.y)/M_PI : 0;
		if(rudder < steering_control_negative_rudder_threshould)
			rudder = 1;

		rudder *= 1.f - (float)suppress_steering_timer()/prm().suppress_steering_duration;
	
		float ang_acc = (prm().steering_linear_velocity_min + v_local_y)*prm().rudder_speed*rudder;
		if(fabs(ang_acc) > prm().steering_acceleration_max)
			ang_acc = SIGN(prm().steering_acceleration_max, ang_acc);
		angular_acceleration.z += ang_acc;

		if(v_local_y < forwardVelocity()*general_velocity_factor)
			acceleration.scaleAdd(rotation().ycol(), prm().forward_acceleration); 
	}
}

void RigidBody::apply_control_force_isotropic()
{
	float v_forward = Vect2f(velocity()).norm();
	average(average_forward_velocity, v_forward, average_forward_velocity_tau);

	// Коэффициент скорости - замедление перед последней точкой
	general_velocity_factor = 1;
	switch(way_points.size())
	{
	case 0:
		general_velocity_factor = 0;
		break;
	case 1:
		{
			float d = Vect2f(way_points.front() - position()).norm();
			if(d < prm().point_control_slow_distance){
				d /= prm().point_control_slow_distance;
				general_velocity_factor = prm().point_control_slow_factor*(1 - d) + d;
				}
		}
		break;
	default:
		general_velocity_factor = 1;
	}

	linear_damping.x += prm().brake_damping*(1.f - general_velocity_factor);
	linear_damping.y += prm().brake_damping*(1.f - general_velocity_factor);

	general_velocity_factor *= velocity_factor;
	
	if(general_velocity_factor && v_forward < forwardVelocity()*general_velocity_factor)
	{
		Vect3f dir = To3D(way_points.front()) - position();
		dir.Normalize();
		acceleration.scaleAdd(dir, prm().forward_acceleration); 
	}
}

void RigidBody::obstacle_analysis()
{
	// Force field
	cluster_colliding = 0;
	if(prm().analyse_force_field_obstacle)
	{
		if(including_cluster != field_dispatcher->getIncludingCluster(position())){
			setPose(posePrev());
			including_cluster = field_dispatcher->getIncludingCluster(position());
		}
		Vect3f p0, dpx, dpy, dpz;
  		matrix().xformPoint(box_min, p0);
		rotation().xform(Vect3f((box_max.x - box_min.x)/2, 0, 0), dpx);
		rotation().xform(Vect3f(0, (box_max.y - box_min.y)/2, 0), dpy);
		rotation().xform(Vect3f(0, (box_max.z - box_min.z)/2, 0), dpz);
		for(int y = -1; y <= 1; y++){
			Vect3f p1 = p0;
			for(int x = -1; x <= 1; x++){
				Vect3f p = p1;
				for(int z = 0; z < 2; z++){
					int cluster;
					if((cluster = field_dispatcher->getIncludingCluster(p)) != including_cluster){
						obstacle_point_avr += p;
						obstacle_point_counter++;
						cluster_colliding = max(cluster, including_cluster);
						}
					p += dpz;
					}
				p1 += dpx;
				}
			p0 += dpy;
			}
	}
	else
		including_cluster = field_dispatcher->getIncludingCluster(position());

	if(obstacle_point_counter){
		obstacle_point_avr /= obstacle_point_counter;
		
		if(showDebugRigidBody.obstaclePoint)
			show_vector(obstacle_point_avr, 1, GREEN);

		suppress_steering_timer.start(prm().suppress_steering_duration);

		Vect3f rg = obstacle_point_avr - position();
		Vect3f rl;
		matrix().invXformVect(rg, rl);
		rl.z = 0;
		Vect3f normal = rl;
		normal.x *= -box_max.y;
		normal.y *= -box_max.x;
		matrix().xformVect(normal);
		normal.z = 0;
		normal.Normalize();
		float u_n = dot(velocity() + angularVelocity() % rg, normal) - prm().obstacle_relaxation;
		if(u_n < 0){
			float t12 = rg[1];
			float t11 = rg[0];
			float t10 = normal[0];
			float t9 = normal[1];
			float Azz = normal[2]*normal[2]*mass_inv+(mass_inv+t11*t11*TOI_inv)*t9*t9+(-2.0f*t12*TOI_inv*t11*t9+(mass_inv+t12*t12*TOI_inv)*t10)*t10;
			Azz = fabs(Azz) > FLT_EPS ? 1.f/Azz : 0;
			float j_n = -Azz*(1.f + prm().obstacle_restitution)*u_n;	
			velocity_.scaleAdd(normal, j_n*mass_inv);
			angularVelocity_.z += TOI_inv*j_n*(rg[0]*normal[1] - rg[1]*normal[0]);
			}
		
		if(rl.y > 0)
			average(obstacle_turn_avr, SIGN(rl.x), obstacle_turn_avr_tau);
		}

	obstacle_point_avr.set(0, 0, 0);
	obstacle_point_counter = 0;

	// Apply torque
	average(obstacle_turn_avr, 0, obstacle_turn_avr_tau);
	angular_acceleration.z += obstacle_turn_avr*prm().avoid_obstacle_rudder_speed;
}

void RigidBody::add_obstacle_point(const Vect3f& point)
{
	obstacle_point_avr += point;
	obstacle_point_counter++;
}

void RigidBody::applyDiggingForce()
{
	if(diggingMode()){
		if((deltaZ_ -= prm().digging_depth_velocity) < -prm().digging_depth)
			deltaZ_ = -prm().digging_depth;

		unsigned short p = vMap.GABuf[vMap.offsetGBufWorldC(position().xi(), position().yi())];
		if((p & GRIDAT_MASK_HARDNESS) != GRIDAT_MASK_HARDNESS){
			linear_damping.x += diggingModeDamping;
			linear_damping.y += diggingModeDamping;
			linear_damping.z += diggingModeDamping;
		}
	}
	else if((deltaZ_ += prm().digging_depth_velocity) > 0)
		deltaZ_ = 0;
}

void RigidBody::evolve(float dt)
{
	bool sleep = prm().enable_sleeping && !controlled() && !flying_mode && average_movement < average_movement_threshould;
	if(sleep && !sleep_timer){
		sleep_timer.start(terLogicRND(sleep_time));
		sleep = false;
	}

	if(!unmovable() && !sleep){ //  && !underMutation() 
		EulerEvolve(dt);
		switch(prm().unit_type){
		case RigidBodyPrm::UNIT:
			ground_analysis(dt);
			break;
		case RigidBodyPrm::MISSILE:
			missile_quant(dt);
			break;
		case RigidBodyPrm::ROCKET:
			rocket_analysis(dt);
			break;
		case RigidBodyPrm::DEBRIS:
			debris_quant(dt);
			break;
		}

		float movement = clamp(velocity().norm2() + acceleration.norm2(), 0, 10);
		if(average_movement < movement)
			average_movement = movement;
		else
			average(average_movement, movement, average_movement_tau);
	}
	else{
		including_cluster = field_dispatcher->getIncludingCluster(position());
		posePrev_ = pose();
		velocity_ = angularVelocity_ = acceleration = angular_acceleration = Vect3f::ZERO;
		set_debug_color(WHITE);
	}
}

void RigidBody::docking(const MatXf& pos, float t_pos, float t_dir)
{
	set_debug_color(RED);

	Se3f dockPose = pose();
	Mat3f rot_one=pos.rot();
	Vect3f temp;
	temp=rot_one.xcol();
	temp.Normalize();
	rot_one.setXcol(temp);

	temp=rot_one.ycol();
	temp.Normalize();
	rot_one.setYcol(temp);

	temp=rot_one.zcol();
	temp.Normalize();
	rot_one.setZcol(temp);

	QuatF new_rot(rot_one);

	dockPose.trans().interpolate(position(), pos.trans(), t_pos);
	dockPose.rot().slerp(orientation(), new_rot, t_dir);

	setPose(dockPose);
}

#ifndef _FINAL_VERSION_
void RigidBody::reload_parameters()
{
	build(prm(), geometry, box_min, box_max);
}

void RigidBody::print(XBuffer& buf)
{
	buf < "position: " <= position();
	buf < "quaternion: " <= orientation();
	buf < "velocity: " <= velocity();
	buf < "angular_velocity: " <= angularVelocity();
	buf < "ground_colliding: " <= ground_colliding < "\n";
}

void RigidBody::show()
{
	if(!debug_show_mode || !prm().enable_show)
		return;

	if(showDebugRigidBody.boundingBox){
		Vect3f p0 = position() + rotation()*box_min;
		Vect3f dx = rotation().xcol()*(box_max.x - box_min.x);
		Vect3f dy = rotation().ycol()*(box_max.y - box_min.y);
		Vect3f dz = rotation().zcol()*(box_max.z - box_min.z);
		show_vector(p0, p0 + dz, p0 + dz + dx, p0 + dx, bounding_box_color);
		show_vector(p0 + dy, p0 + dy + dz, p0 + dy + dz + dx, p0 + dy + dx, bounding_box_color);
		show_vector(p0, p0 + dz, p0 + dz + dy, p0 + dy, bounding_box_color);
		show_vector(p0 + dx, p0 + dx + dz, p0 + dx + dz + dy, p0 + dx + dy, bounding_box_color);
		}

	if(showDebugRigidBody.radius)
		show_vector(position(), radius(), GREEN);

	if(showDebugRigidBody.inscribedRadius)
		show_vector(position(), inscribed_radius, YELLOW);

	if(showDebugRigidBody.wayPoints && controlled()){
		Vect3f pos = To3D(position());
		Vect3fList::iterator i;
		FOR_EACH(way_points, i){
			Vect3f p=To3D(*i);
			show_line(pos, p, YELLOW);
			show_vector(p, 1, YELLOW);
			pos = p;
			}
		show_vector(target, 3, YELLOW);
		}

	if(showDebugRigidBody.movingDirection){
		show_vector(position(), velocity()*0.1f, MAGENTA);
		//show_vector(position, rotation().ycol()*2, RED);
		show_vector(position(), velocityDelta_*0.1f, RED);
		}

	if(showDebugRigidBody.velocityValue){
		XBuffer buf;
		buf.SetDigits(2);
		buf <= velocity().norm();
		show_text(position(), buf, MAGENTA);
	}
}
#endif

bool RigidBody::is_point_reached(const Vect2f& point) const
{ 
	Vect2f p0 = posePrev().trans();
	Vect2f p1 = position();
	Vect2f axis = p1 - p0;
	float norm2 = axis.norm2();
	float dist2 = 0;
	if(norm2 < 0.1){
		dist2 = p0.distance2(point);
	}
	else{
		Vect2f dir = point - p0;
		float dotDir = dot(axis, dir);
		if(dotDir < 0)
			dist2 = p0.distance2(point);
		else if(dot(axis, point - p1) > 0)
			dist2 = p1.distance2(point);
		else
			dist2 = (dir - axis*(dotDir/norm2)).norm2();
	}

//	return dist2 < sqr(general_velocity_factor < general_velocity_factor_intermediate ? prm().is_point_reached_radius_max : is_point_reached_radius_max_intermediate);
	return dist2 < sqr(is_point_reached_radius_max_intermediate);
}

void RigidBody::setBound(const Vect3f box_min_, const Vect3f box_max_)
{
	ID = ++IDs; // используется как ключ при хешировании polytree.feature
	box_min = Vect3f(min(box_min_.x, -prm().radius_min), min(box_min_.y, -prm().radius_min), box_min_.z);
	box_max = Vect3f(max(box_max_.x, prm().radius_min), max(box_max_.y, prm().radius_min), box_max_.z);
	radius_ = max(box_max.distance(box_min)/2, prm().radius_min);
	inscribed_radius = box_max.min();
	Dx = (box_max.x - box_min.x)*prm().analyze_points_density;
	if(Dx < prm().Dxy_minimal) Dx = prm().Dxy_minimal;
	Dy = (box_max.y - box_min.y)*prm().analyze_points_density;
	if(Dy < prm().Dxy_minimal) Dy = prm().Dxy_minimal;

	if(false && prm().polyhedral_bound){
		//polytree = universe()->multiBodyDispatcher().polytree_library.create(box_min, box_max);
	}else{
		//polytree = universe()->multiBodyDispatcher().polytree_library.create(radius());
		inscribed_radius = radius();
	}

	float mass = prm().density;
	float TOI = sqr(radius())*mass*prm().TOI_factor;
	mass_inv = 1.f/mass;
	TOI_inv = 1.f/TOI;
}

int height(int x, int y) 
{  
	return vMap.GVBuf[vMap.offsetGBufC(x >> kmGrid, y >> kmGrid)]; 
}

