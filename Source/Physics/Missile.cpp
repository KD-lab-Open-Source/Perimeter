#include "StdAfx.h"
#include "terra.h"
#include "RigidBody.h"
#include "ForceField.h"

RigidBody* GetMissileTest(RigidBody* object,RigidBody* source,RigidBody* target);

void RigidBody::suggestMissileTurnAngels(const RigidBodyPrm& prm, const RigidBody& firing_object, const Vect3f& position, const Vect3f& target, float& psi, float& theta)
{
	Vect3f r;
	firing_object.matrix().invXformPoint(target, r);
	psi = r.psi() - M_PI/2;
	theta = prm.calcTurnTheta(sqrt(sqr(r.x) + sqr(r.y)), r.z, prm.forward_velocity_max);
}

void RigidBody::suggestMissileTurnAngels(const RigidBody& firingObject, const Vect3f& firingPosition, const Vect3f& target, float& psi, float& theta)
{
	Vect3f r;
	MatXf(firingObject.rotation(), firingPosition).invXformPoint(target, r);
	psi = r.psi() - M_PI/2;
	theta = M_PI/2 - r.theta();
}

int RigidBody::testMissileShot(const RigidBodyPrm& prm, const Vect3f box_min, const Vect3f box_max, const RigidBody& firing_object, const Vect3f& position, const Vect3f& direction, RigidBody* target)
{
	RigidBody missile;
	missile.build(prm, 0, box_min, box_max);
	missile.startMissile(firing_object, position, target->position(), direction);
	while(1){
		missile.evolve(0.1f);
		RigidBody* b = GetMissileTest(&missile, (RigidBody*)&firing_object, target);
		if(b)
			return b == target;
		if(missile.ground_colliding)
			return missile.position().distance2(target->position()) < test_missile_shot_distance2;
		}
	return 0;
}

void RigidBody::startMissile(const RigidBody& firing_object, const Vect3f& position, const Vect3f& target_, const Vect3f& direction)
{
	setPosition(position);
	target = target_;
				 
	if(!prm().keep_direction_time || direction == Vect3f::ZERO)
	{
		Vect3f r = target - position;
		float psi = r.psi() - M_PI/2;
		float theta = prm().calcTurnTheta(sqrt(sqr(r.x) + sqr(r.y)), r.z, forwardVelocity());

		setOrientation(Mat3f(psi, Z_AXIS)*Mat3f(theta, X_AXIS));
	}
	else
	{
		//Vect3f cross = Vect3f::J % direction;
		//float len = cross.norm();
		//if(len > FLT_EPS)
		//	setOrientation(QuatF(Acos(dot(Vect3f::J, direction))/(direction.norm() + FLT_EPS), cross));

		keep_direction_timer.start(prm().keep_direction_time);
		restart_missile = true;
	}

	velocity_.scale(rotation().ycol(), forwardVelocity());

	posePrev_ = pose();

	including_cluster = firing_object.including_cluster;// FieldDispatcher->getIncludingCluster(position);
	missile_started = true;
	ground_colliding_timer.start(prm().ground_colliding_delay);
}

void RigidBody::missile_quant(float dt)
{
	if(!missile_started)
	{
		velocity_ = Vect3f::ZERO;
		return;
	}

	set_debug_color(GREEN);
	
	if(prm().analyse_force_field_obstacle){
		int cluster = field_dispatcher->getIncludingCluster(position());
		cluster_colliding = cluster == including_cluster ? 0 : max(cluster, including_cluster);
		including_cluster = cluster;
	}

	ground_colliding = !ground_colliding_timer && height(position().xi(), position().yi()) > position().zi();
	
	if(keep_direction_timer()){
		setVelocity(rotation().ycol()*forwardVelocity());
	}
	else if(restart_missile){
		restart_missile = false;
		startMissile(*this, position(), target, Vect3f::ZERO);
	}
	else
		apply_levelling_torque(rotation().ycol(), velocity());
}

float RigidBodyPrm::calcTurnTheta(float x, float z, float velocity) const 
{
	if(velocity < FLT_EPS)
		return 0;
	x += FLT_EPS;
	z += FLT_EPS;
	x *= distance_correction_factor;
	float v = velocity;
	float t30 = -2.0*z;
	float t16 = x*x;
	float t11 = z*z+t16;
	float t29 = -2.0*t11;
	float t28 = 1.0/t11/2.0;
	float t13 = v*v;
	float t27 = z*gravity-t13;
	float t15 = 1.0/v;
	float t26 = t11*t15;
	float t25 = t28*t30;
	float t9 = t13*t13+(t13*t30-t16*gravity)*gravity;
	if(t9 < 0) {
		//xassert_s(forward_velocity_max < FLT_EPS && "1. Can't reach the target. Increase forward_velocity_max for ", name);
		return M_PI/4;
	}
	t9 = sqrt(t9);
	float t8 = t9+t27;
	float t17 = t8*t29;
	if(t17 < FLT_EPS) {
		//xassert_s(forward_velocity_max < FLT_EPS && "2. Can't reach the target. Increase forward_velocity_max for ", name);
		return M_PI/4;
	}
	t17 = sqrt(t17);
	float t24 = (t8*t25+gravity)/t17*t26;
	float t7 = -t9+t27;
	float t18 = t7*t29;
	if(t18 < FLT_EPS) {
		//xassert_s(forward_velocity_max < FLT_EPS && "3. Can't reach the target. Increase forward_velocity_max for ", name);
		return M_PI/4;
	}
	t18 = sqrt(t18);
	float t23 = (t7*t25+gravity)/t18*t26;
	float t22 = t15*x*t28;
	float t21 = t18*t22;
	float t20 = t17*t22;
	float r[4];
	r[0] = atan2(t23,t21);
	r[1] = atan2(-t23,-t21);
	r[2] = atan2(t24,t20);
	r[3] = atan2(-t24,-t20);
	float lower = G2R(lower_theta);
	float upper = G2R(upper_theta);
	float theta = r[0] > upper ? upper : (r[0] < lower ? lower : r[0]);
	if(minimize_theta){
		for(int i = 1; i < 4; i++)
			if(fabs(theta) > fabs(r[i]) && r[i] < upper && r[i] > lower)
				theta = r[i];
		}
	else
		for(int i = 1; i < 4; i++)
			if(fabs(theta) < fabs(r[i]) && r[i] < upper && r[i] > lower)
				theta = r[i];
	return theta;
}

void RigidBody::startDebris(const Vect3f& position, const Vect3f& velocity)
{
	setVelocity(velocity);
	setAngularVelocity(Vect3f(terLogicRNDfrnd(), terLogicRNDfrnd(), terLogicRNDfrnd())*prm().debris_angular_velocity);

	setPose(Se3f(QuatF::ID, position));
	posePrev_ = Se3f::ID;
	including_cluster = field_dispatcher->getIncludingCluster(position);
}

void RigidBody::debris_quant(float dt)
{
	int cluster = field_dispatcher->getIncludingCluster(position());
	cluster_colliding = cluster == including_cluster ? 0 : max(cluster, including_cluster);
	including_cluster = cluster;

	ground_colliding = height(position().xi(), position().yi()) > position().z;
}