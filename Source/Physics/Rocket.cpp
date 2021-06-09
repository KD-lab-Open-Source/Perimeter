#include "StdAfx.h"
#include "terra.h"
#include "RigidBody.h"
#include "ForceField.h"

void RigidBody::startRocket(RigidBody& owner)
{
	velocity_ = owner.velocity();
	set_debug_color(WHITE);
	suppress_steering_timer.start(prm().suppress_steering_duration);
}

void RigidBody::rocket_analysis(float dt)
{
	set_debug_color(GREEN);
	int D = (int)round(radius()) >> kmGrid;
	if(!D)
		D = 1;
	int x0 = (int)round(position().x) >> kmGrid;
	int y0 = (int)round(position().y) >> kmGrid;
	int z_max = 0;
	for(int y = -D; y <= D; y++)
		for(int x = -D; x <= D; x++){
			int z = vMap.GVBuf[vMap.offsetGBufC(x + x0, y + y0)]; 
			if(z_max < z)
				z_max = z;
			}

	ground_colliding = z_max > position().z;

	Vect2f delta = controlled() ? way_points.front() - position() : rotation().ycol();
	delta *= prm().rocket_forward_analysis_distance/(delta.norm() + 0.01);
	x0 = (int)round(position().x + delta.x) >> kmGrid;
	y0 = (int)round(position().y + delta.y) >> kmGrid;
	int z_max_forward = 0;
	for(int y = -D; y <= D; y++)
		for(int x = -D; x <= D; x++){
			int z = vMap.GVBuf[vMap.offsetGBufC(x + x0, y + y0)]; 
			if(z_max_forward < z)
				z_max_forward = z;
			}

	z_max_forward += flyingHeight();

	Vect3f y_axis;
	if(controlled()){
		Vect3f target = way_points.front();
		Vect2f direction = target - position();
		if(direction.norm2() < sqr(prm().rocket_vertical_control_distance) || z_max_forward < target.z){
			delta = direction;
			z_max_forward = target.z + prm().rocket_target_offset_z;
		} 
		y_axis = Vect3f(delta.x, delta.y, z_max_forward - position().z);
		if(showDebugRigidBody.rocketTarget)
			show_line(position(), position() + y_axis, RED);
		y_axis.normalize();
	}
	else
		y_axis = rotation().ycol();

	if(suppress_steering_timer()){
		y_axis = rotation().ycol();
		set_debug_color(BLUE);
		ground_colliding = 0;
	}

	apply_levelling_torque(rotation().ycol(), y_axis);
	if(velocity().norm() < forwardVelocity())
		acceleration.scaleAdd(y_axis, prm().forward_acceleration); 

	int cluster = field_dispatcher->getIncludingCluster(position());
	cluster_colliding = cluster == including_cluster ? 0 : max(cluster, including_cluster);
	including_cluster = cluster;
}

void RigidBody::startUnderGroundMissile()
{
	setDiggingMode(true);
	deltaZ_ = -prm().digging_depth;
}