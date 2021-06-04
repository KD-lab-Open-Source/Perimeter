#ifndef __RIGID_BODY_H__
#define __RIGID_BODY_H__

#include "RigidBodyPrm.h"
#include "Scripts\RigidBody.hi"

class RigidBody;

class Contact
{
public:

	int set(float penetration_, const Vect3f& cp1_, const Vect3f& cp2_, RigidBody* body1_, RigidBody* body2_);
	float normal_velocity();
	void resolve();
	void resolveSmart();
	void finalizeResolve();
	void clear();
	void show(sColor4c color);

private:
	float penetration;
	Vect3f normal; // points towards body1: cp1 - cp2;
	Vect3f cp1l, cp2l;
	Vect3f cp1g, cp2g;
	RigidBody *body1, *body2; // Never body1->unmovable && !body2->unmovable

	float u_n;
	float Azz;
	float j_n_total;

	char Azz_inited;
	char u_n_calculated;
	char body1_unmovable;
	char body2_unmovable;

	friend class RigidBody;
	friend class MultiBodyDispatcher;
};
typedef list<Contact> ContactList;
typedef vector<Contact*> ContactPtrList;


//------------------------------------

class RigidBody
{
public:
	RigidBody();
	~RigidBody();
	
	const RigidBodyPrm& prm() const { return *prm_; }
	void build(const RigidBodyPrm& prm, cObjectNodeRoot* geometry, const Vect3f& box_min, const Vect3f& box_max);
	
	void evolve(float dt);
	void docking(const MatXf& pos, float t_pos, float t_dir);

	// Geometry
	float radius() const { return radius_; }
	void setRadius(float radius) { radius_ = radius; }

	const Vect3f& boxMin() const { return box_min; }
	const Vect3f& boxMax() const { return box_max; }
	
	// Position
	const Vect3f& position() const { return pose_.trans(); }
	const QuatF& orientation() const { return pose_.rot(); }
	const Se3f& pose() const { return pose_; }
	const Mat3f& rotation() const { return matrix_.rot(); }
	const MatXf& matrix() const { return matrix_; }
	float angleZ() const { return atan2f(rotation()[1][0], rotation()[0][0]); }
	const Se3f& posePrev() const { return posePrev_; }

	void initPose(const Se3f& pose); // Вызывать при первой установке
	void initPose(const Se3f& pose, bool modify_z);

	void setPosition(const Vect3f& position) { pose_.trans() = position; matrix_.trans() = position; } // Для перемещения
	void setOrientation(const QuatF& orientation) { pose_.rot() = orientation; matrix_.rot() = orientation; }
	void setPose(const Se3f& pose) { pose_ = pose; matrix_ = pose; }

	// Velocity
	const Vect3f& velocity() const { return velocity_; }
	void setVelocity(const Vect3f& velocity) { velocity_ = velocity; }
	void addVelocity(const Vect3f& deltaVelocity) { velocity_ += deltaVelocity; }

	const Vect3f& angularVelocity() const { return angularVelocity_; }
	void setAngularVelocity(const Vect3f& angularVelocity) { angularVelocity_ = angularVelocity; }
	void addAngularVelocity(const Vect3f& deltaAngularVelocity) { angularVelocity_ += deltaAngularVelocity; }

	// Acceleration
	void setAcceleration(const Vect3f& accelerationIn) { acceleration = accelerationIn; }
	void addAcceleration(const Vect3f& deltaAcceleration) { acceleration += deltaAcceleration; }
	
	void setAngularAcceleration(const Vect3f& angularAcceleration) { angular_acceleration = angularAcceleration; }
	void addAngularAcceleration(const Vect3f& deltaAngularAcceleration) { angular_acceleration += deltaAngularAcceleration; }
	
	// Ground units
	float forwardVelocity() const { return forwardVelocity_; }
	void setForwardVelocity(float v){ forwardVelocity_ = v; }
	bool is_point_reached(const Vect2f& point) const;
	bool on_upper_position() const { return kangaroo_mode > kangaroo_mode_avr_threshould; } // юнит расположен над другим 
	float speed() const { return fabs(average_forward_velocity); }
	float speedFactor() const { float f = fabs(average_forward_velocity*prm().speed_map_factor); return f < 1 ? f : 1; }
	void stopMovement() { velocity_factor = 0; velocity_ = Vect3f::ZERO; }

	// States
	bool groundColliding() const { return ground_colliding; } // сталкивается, стоит на земле
	bool chaosColliding() const { return chaos_colliding; } // сталкивается с хаосом
	float chaosCollidingFactor() const { return !flyingMode() ? chaosCollidingFactor_ : 0; } // сталкивается с хаосом 0..1
	int clusterColliding() const { return cluster_colliding; } // сталкивается с силовым полем кластера (ID)
	bool gravicapMode() const { return gravicap_mode; } // парит на воздушной подушке

	float velocityFactor() const { return velocity_factor; } // модуляция скорости
	void setVelocityFactor(float factor) { velocity_factor = factor; }
	
	bool unmovable() const { return unmovable_; }
	void makeStatic() { unmovable_ = 1;  }
	void makeDynamic() { unmovable_ = 0; }

	void setFlyingMode(bool mode) {	flying_mode = mode; } // включает полет
	bool flyingMode() const { return flying_mode && (controlled() || !prm().flying_down_without_way_points) && !underMutation(); }
	float flyingHeight() const { return flyingHeight_; }
	void setFlyingHeight(float flyingHeight) { flyingHeight_ = flyingHeight; }

	bool diggingMode() const { return diggingMode_; }
	bool diggingModeLagged() const { return diggingMode_ || diggingModeTimer_(); }
	bool underGround() const { return diggingMode() && deltaZ_ == -prm().digging_depth; }
	bool onGround() const { return !diggingMode() && deltaZ_ == 0; }
	void setDiggingMode(bool mode) { diggingMode_ = mode; diggingModeTimer_.start(diggingModeDelay); } // подземный режим
	
	// Missiles
	void startMissile(const RigidBody& firing_object, const Vect3f& position, const Vect3f& target, const Vect3f& direction); // direction used while keep_direction_time 
	static void suggestMissileTurnAngels(const RigidBodyPrm& prm, const RigidBody& firing_object, const Vect3f& position, const Vect3f& target, float& psi, float& theta);
	static void suggestMissileTurnAngels(const RigidBody& firingObject, const Vect3f& firingPosition, const Vect3f& target, float& psi, float& theta);
	static int testMissileShot(const RigidBodyPrm& prm, const Vect3f box_min, const Vect3f box_max, const RigidBody& firing_object, const Vect3f& position, const Vect3f& direction, RigidBody* target);

	// Rocket, Debris
	void startRocket(RigidBody& owner);
	void startDebris(const Vect3f& position, const Vect3f& velocity);
	void startUnderGroundMissile();
	bool missileStarted() const { return missile_started; }

	// Mutation
	void setComposition(const int array[3]) { composition_.set(array[0], array[1], array[2]); } 
	const Vect3f& composition() const { return composition_; } // soldier, officer, technic
	
	void setUnderMutation(bool underMutation) { underMutation_ = underMutation; }
	bool underMutation() const { return underMutation_; }

	// Utils
	void setScale(const Vect3f& s); 
	void setBound(const Vect3f box_min_,const Vect3f box_max_);

	void finalizeResolve();
	
	// Debug
#ifndef _FINAL_VERSION_
	void show();
	void print(XBuffer& buf);
	void reload_parameters();
#else
	void show(){}
	void print(XBuffer& buf) {}
	void reload_parameters() {}
#endif

	// Control
	Vect3fList way_points;
		
private:
	int ID;
	const RigidBodyPrm* prm_;

	// State
	Se3f pose_; 
	Se3f posePrev_; 
	MatXf matrix_;

	Vect3f velocity_, angularVelocity_;
	Vect3f acceleration, angular_acceleration;

	Vect3f linear_damping;
	float angular_damping;
	
	// Geometry
	float radius_;
	Vect3f box_min, box_max;
	float mass_inv;
	float TOI_inv;	
	float inscribed_radius;
	Vect3f scale;
	int Dx, Dy;
	
	float average_forward_velocity;
	float general_velocity_factor;
	DurationTimer suppress_steering_timer;
	Vect3f obstacle_point_avr;
	int obstacle_point_counter;
	
	float deltaZ_;
	float dz_max_avr;

	// States
	bool ground_colliding; // сталкивается, стоит на земле
	bool chaos_colliding; // сталкивается с хаосом
	float chaosCollidingFactor_; // сталкивается с хаосом 0..1
	int cluster_colliding; // сталкивается с силовым полем кластера (ID)
	int including_cluster; // находится внутри кластера (ID)
	bool gravicap_mode; // парит на воздушной подушке

	float velocity_factor; // модуляция скорости
	float forwardVelocity_;
	float flyingHeight_;
	RigidBodyPrm::Oscillator flying_oscillator_z;
   	RigidBodyPrm::Oscillator gravicap_oscillator_z;
	RigidBodyPrm::Oscillator gravicap_oscillator_pitch;
	RigidBodyPrm::Oscillator gravicap_oscillator_roll;
	float obstacle_turn_avr;
	
	bool unmovable_;
	bool flying_mode; // включает полет
	bool diggingMode_;  // закопанный
	DurationTimer diggingModeTimer_;
	
	// Kangaroo
	float kangaroo_mode;
	int kangaroo_height, kangaroo_height_prev;
	
	// Control
	Vect3f target;
	
	// Missile
	bool missile_started;
	bool restart_missile;
	DurationTimer keep_direction_timer;
	DurationTimer ground_colliding_timer;
	
	// Sleep evolution system
	float average_movement;
	DurationTimer sleep_timer;
	
	// Model
	cObjectNodeRoot* geometry;
	sColor4f diffuse_color;
	
	// Body to Body
	ContactPtrList contacts;
	Vect3f velocityDeltaSum_;
	Vect3f velocityDelta_;
	float penetrationSum_;

	// Mutation
	Vect3f composition_; // soldier, officer, technic
	bool underMutation_;
	
	// Debug
#ifndef _FINAL_VERSION_
	sColor4c bounding_box_color;
#endif

	//-------------------------------
	void EulerEvolve(float dt);

	void apply_control_force();
	void apply_control_force_isotropic();
	void apply_levelling_torque(const Vect3f& current_axis, const Vect3f& target_axis);
	void applyDiggingForce();
	bool controlled() const { return !way_points.empty(); }
	void ground_analysis(float dt);
	void rocket_analysis(float dt);
	void obstacle_analysis();
	void add_obstacle_point(const Vect3f& point);

	void missile_quant(float dt);

	void debris_quant(float dt);

	bool kangarooEnabled() const;

	// Debug
#ifndef _FINAL_VERSION_
	void set_debug_color(sColor4c color) { bounding_box_color = color; }
#else
	void set_debug_color(sColor4c color) {}
#endif

	static int IDs;

	friend class MultiBodyDispatcher;
	friend class MutationProcess;
	friend class Contact;
};

class MultiBodyDispatcher
{
public:
	MultiBodyDispatcher();

	void prepare(); // clears previos contacts information only now, so You can use it for AI
	bool test(RigidBody& b1, RigidBody& b2, const MatXf& X12, bool addContact); // call from Grid operator()
	void resolve(); // call before evolve
	void resolveSmart(); // call before evolve

private:
	ContactList contacts;
	
	friend RigidBody;
};

int height(int x, int y);


#endif // __RIGID_BODY_H__
