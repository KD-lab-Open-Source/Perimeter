#include "StdAfx.h"
#include "Runtime.h"
#include "RigidBody.h"

MultiBodyDispatcher::MultiBodyDispatcher()
{
	RigidBody::IDs = 0;
}

bool MultiBodyDispatcher::test(RigidBody& b1, RigidBody& b2, const MatXf& Xr1r2, bool addContact)
{
	Vect3f cp1, cp2;
	float dist = Xr1r2.trans().norm();
	float penetration = b1.inscribed_radius + b2.inscribed_radius - dist;
	if(penetration > 0){
		if(dist > FLT_EPS){	
			cp1 = Xr1r2.trans();
			cp1.Normalize();
			Xr1r2.xformVect(cp1, cp2);
			cp1 *= b1.inscribed_radius;
			cp2 *= -b2.inscribed_radius;
		}
		else{
			cp1.set(0.01f, 0, 0);
			cp2.set(-0.01f, 0, 0);
		}
	}
	else if(false && (b1.prm().polyhedral_bound || b2.prm().polyhedral_bound)){ 
		penetration = collision_detection_epsilon;
		if(!b1.prm().polyhedral_bound){
			penetration += b1.radius();
			Vect3f delta;
			Xr1r2.invXformPoint(cp2, delta);
			delta -= cp1;
			delta.Normalize(b1.radius());
			cp1 += delta;
		}
		if(!b2.prm().polyhedral_bound){
			penetration += b2.radius();
			Vect3f delta;
			Xr1r2.xformPoint(cp1, delta);
			delta -= cp2;
			delta.Normalize(b2.radius());
			cp2 += delta;
		}
	}

	if(penetration > 0){
		if(addContact){
			contacts.push_back(Contact());
			if(!contacts.back().set(penetration, cp1, cp2, &b1, &b2))
				contacts.pop_back();
		}

		//Vect3f p1, p2;
		//b1.matrix().xformPoint(cp1, p1);
		//b2.matrix().xformPoint(cp2, p2);
		//show_line(p1, p2, MAGENTA);
		return true;
		}

	//show_line(b1.position(), b2.position(), GREEN);
	return false;
}

void MultiBodyDispatcher::resolve()
{
	if(contacts.empty())
		return;

	for(int i = 0; i < contacts.size()*collision_resolve_iterations_per_contact; i++){
		float u_n, u_n_min = FLT_INF;
		Contact* c_min;
		ContactList::iterator ci;
		FOR_EACH(contacts, ci)
			if(u_n_min > (u_n = ci->normal_velocity())){
				u_n_min = u_n;
				c_min = &*ci;
				}
		if(u_n_min > collision_resolve_velocity_tolerance)
			break;

		c_min->resolve();
		}

	//xassert("Unable to resolve collision" && i < contacts.size()*collision_resolve_iterations_per_contact);
	prepare();
}

void MultiBodyDispatcher::resolveSmart()
{
	if(contacts.empty())
		return;

	ContactList::iterator ci;
	FOR_EACH(contacts, ci)
		ci->resolveSmart();

	FOR_EACH(contacts, ci)
		ci->finalizeResolve();
	
	//xassert("Unable to resolve collision" && i < contacts.size()*collision_resolve_iterations_per_contact);
	prepare();
}


void MultiBodyDispatcher::prepare()
{
//	if(closest_features_ht.size() > closestFeaturesHTsizeMax)
//		closest_features_ht.clear();

	ContactList::iterator ci;
	FOR_EACH(contacts, ci)
		ci->clear();

	contacts.clear();
}
  
//////////////////////////////////////////////////////////////
//	Contact
//////////////////////////////////////////////////////////////
int Contact::set(float penetration_, const Vect3f& cp1_, const Vect3f& cp2_, RigidBody* body1_, RigidBody* body2_)
{
	penetration = penetration_;
	cp1l = cp1_;
	cp2l = cp2_;
	body1 = body1_;
	body2 = body2_;
	body1->matrix().xformVect(cp1l, cp1g);
	body2->matrix().xformVect(cp2l, cp2g);
	
	normal.sub(cp1g, cp2g);
	Vect3f r21 = body1->position() - body2->position();
	normal.add(r21);
	if(dot(r21, normal) < -FLT_EPS)
		normal.negate();

	Azz_inited = 0;
	u_n_calculated = 0;
	j_n_total = 0;

	body1_unmovable = body1->unmovable();
	body2_unmovable = body2->unmovable();

	int k1 = body1->kangarooEnabled() && body2->prm().kangaroo_allow_on_me;
	int k2 = body2->kangarooEnabled() && body1->prm().kangaroo_allow_on_me;
	if(k1 || k2){
		float dh1 = (body1->rotation()*Vect3f(cp1l.x, cp1l.y, body1->box_max.z)).z + body1->prm().kangaroo_delta_z;
		float dh2 = (body2->rotation()*Vect3f(cp2l.x, cp2l.y, body2->box_max.z)).z + body2->prm().kangaroo_delta_z;
		//float dh1 = (body1->rotation.zrow()*body1->box_max).sumAbs() + body1->kangaroo_delta_z;
		//float dh2 = (body2->rotation.zrow()*body2->box_max).sumAbs() + body2->kangaroo_delta_z;
		float h1 = body1->position().z + dh1;
		float h2 = body2->position().z + dh2;
		if(k1 && k2){
			if(fabs(h2 - h1) < kangaroo_max_height_switch && fabs(body1->mass_inv - body2->mass_inv) > kangaroo_min_mass_switch)
				if(body2->mass_inv < body1->mass_inv)
					k2 = 0;
				else 
					k1 = 0;
			else
				if(h1 > h2)
					k2 = 0;
				else 
					k1 = 0;
			}
		if(k1){
			body1->kangaroo_height = max(body1->kangaroo_height, (int)round(h2));
			body1->kangaroo_mode = 1;
			if(!body1->controlled()){
				Vect3f delta = body1->position() - body2->position();
				delta.z = 0;
				delta += body1->rotation().ycol()*kangaroo_avoid_stoopor_epsilon;
				delta.Normalize(kangaroo_avoid_stoopor_acceleration);
				body1->acceleration += delta;
				}
			}
		else{
			body2->kangaroo_height = max(body2->kangaroo_height, (int)round(h1));
			body2->kangaroo_mode = 1;
			if(!body2->controlled()){
				Vect3f delta = body2->position() - body1->position();
				delta.z = 0;
				delta += body2->rotation().ycol()*kangaroo_avoid_stoopor_epsilon;
				delta.Normalize(kangaroo_avoid_stoopor_acceleration);
				body2->acceleration += delta;
				}
			}

		show(MAGENTA);
		return 0;
		}

	if(!body1->prm().analyse_body_obstacle || !body2->prm().analyse_body_obstacle)
		return 0;

	if(body1->controlled())
		body1->add_obstacle_point(body1->position() + cp1g);
	if(body2->controlled())
		body2->add_obstacle_point(body2->position() + cp2g);

	show(RED);
	normal.z = 0;
	normal.Normalize();

	body1->contacts.push_back(this);
	body2->contacts.push_back(this);

	return 1;
}

float Contact::normal_velocity()
{
	if(!u_n_calculated){
		u_n = collision_resolve_penetration_relaxation_factor*penetration;
		if(u_n < collision_resolve_penetration_relaxation_min)
			u_n = collision_resolve_penetration_relaxation_min;
		u_n += dot(body1->velocity() + body1->angularVelocity() % cp1g - body2->velocity() - body2->angularVelocity() % cp2g, normal) - collision_resolve_velocity_relaxation;
		u_n_calculated = 1;
		}
	return u_n;
}

void Contact::resolve()
{	
	if(!Azz_inited){	
		Azz = 0;
		if(!body1_unmovable){
			float t4 = cp1g[1];
			float t3 = cp1g[0];
			float t2 = normal[0];
			float t1 = normal[1];
			Azz += (body1->mass_inv+t4*t4*body1->TOI_inv)*t2*t2+normal[2]*normal[2]*body1->mass_inv+(-2.0*t4*body1->TOI_inv*t3*t2+(body1->mass_inv+t3*t3*body1->TOI_inv)*t1)*t1;
			}
		if(!body2_unmovable){
			float t8 = cp2g[1];
			float t7 = cp2g[0];
			float t6 = normal[0];
			float t5 = normal[1];
			Azz += (body2->mass_inv+t8*t8*body2->TOI_inv)*t6*t6+normal[2]*normal[2]*body2->mass_inv+(-2.0*t8*body2->TOI_inv*t7*t6+(body2->mass_inv+t7*t7*body2->TOI_inv)*t5)*t5;
			}
		Azz = fabs(Azz) > FLT_EPS ? 1.f/Azz : 0;
		Azz_inited = 1;
		}

	float j_n = -Azz*collision_resolve_restitution_plus_one*u_n;	
	j_n_total += j_n;

	if(!body1_unmovable){
		body1->velocity_.scaleAdd(normal, j_n*body1->mass_inv);
		body1->angularVelocity_.z += body1->TOI_inv*j_n*(cp1g[0]*normal[1] - cp1g[1]*normal[0]);
		ContactPtrList::iterator cpi;
		FOR_EACH(body1->contacts, cpi)
			(*cpi)->u_n_calculated = 0;
		}

	if(!body2_unmovable){
		body2->velocity_.scaleAdd(normal, -j_n*body2->mass_inv);
		body2->angularVelocity_.z -= body2->TOI_inv*j_n*(cp2g[0]*normal[1] - cp2g[1]*normal[0]);
		ContactPtrList::iterator cpi;
		FOR_EACH(body2->contacts, cpi)
			(*cpi)->u_n_calculated = 0;
		}
}

void Contact::resolveSmart()
{	
	Vect3f v_n, v_t, delta = body2->position() - body1->position();

	if(!body1_unmovable){
		float velocityNorm = body1->velocity().norm();
		if(velocityNorm > 1){
			decomposition(body1->rotation().ycol(), delta, v_n, v_t);
			v_t.Normalize();
			body1->velocityDeltaSum_ -= v_t*(penetration*velocityNorm);
			body1->penetrationSum_ += penetration;
		}
	}

	if(!body2_unmovable){
		float velocityNorm = body2->velocity().norm();
		if(velocityNorm > 1){
			decomposition(body2->rotation().ycol(), delta, v_n, v_t);
			v_t.Normalize();
			body2->velocityDeltaSum_ += v_t*(penetration*velocityNorm);
			body2->penetrationSum_ += penetration;
		}
	}
}

void RigidBody::finalizeResolve()
{
	if(penetrationSum_ > 0){
		velocityDelta_ = velocityDeltaSum_ *= fieldPathFindFactor/penetrationSum_;
		show_vector(position(), velocityDeltaSum_, RED);
		velocity_ += velocityDeltaSum_;
		velocityDeltaSum_ = Vect3f::ZERO;
		penetrationSum_ = 0;
	}
}

void Contact::finalizeResolve()
{
	body1->finalizeResolve();
	body2->finalizeResolve();
}

void Contact::clear()
{
	body1->contacts.clear();
	body2->contacts.clear();
}

void Contact::show(sColor4c color)
{
	if(showDebugRigidBody.contacts){
		Vect3f cp = cp1g + body1->position();
		show_vector(cp, 1, color);
		show_vector(cp, normal, color);
		}
}



