#include "stdafx.h"
#include "mutation.h"
#include "runtime.h"
#include "RigidBody.h"
#include "terra.h"
#include "PerimeterSound.h"
#include "Universe.h"
#include "Scripts\Mutation.hi"

static float volume_power = 3;
static float Volume(float radius) { return 4./3*M_PI*pow(radius/debuScales.spheres, 3); }
static float Radius(float volume) { return pow(volume*0.75/M_PI, 1./3.)*debuScales.spheres; }

MutationProcess::Sphere::Sphere()
{
	geometry = 0;
}

MutationProcess::Sphere::~Sphere()
{
	RELEASE(geometry);
}

void MutationProcess::Sphere::create(float volume, const Vect3f& position_, bool placeOnTheGround)
{
	RELEASE(geometry);
	geometry = terScene->CreateObject("RESOURCE\\MODELS\\FX\\Sphere.m3d", 0);
	sBox6f sbox;
	geometry->GetBoundBox(sbox);
	radius0 = sbox.xmax();
	scale1 = Radius(volume)/radius0;
	radius = radius0*scale1;
	geometry->SetScale(Vect3f::ZERO);
	pose.set(MatXf(Mat3f::ID, position = position_));
	if(placeOnTheGround)
		pose.trans().z = position.z = calcZ();
	displacement = Vect3f::ZERO;
	set(0, 0);
}

void MutationProcess::Sphere::set(float ts, float tp)
{
	float scale = scale1*clamp(ts, 0, 1);
	radius = radius0*scale;
	geometry->SetScale(Vect3f::ID*scale);

	tp = clamp(tp, 0, 1);
	float t2 = tp*tp;
	tp = 3.*t2 - 2.*tp*t2;
	Vect3f& r = pose.trans();
	r.scaleAdd(position, displacement, tp); 

	float z = calcZ();
	r.z += (z - r.z)*(r.z < z ? mutation_upper_stiffness : mutation_lower_stiffness);

	int x = r.xi(); 
	int y = r.yi(); 
	float Dx = height(x + 1, y) - height(x - 1, y);
	float Dy = height(x, y + 1) - height(x, y - 1);
	Vect3f z_axis(-Dx, -Dy, 1 << kmGrid);
	z_axis.Normalize();

	geometry->SetPosition(pose); 
} 

void MutationProcess::Sphere::setTarget(const Vect3f& target, float distance)
{
	position = pose.trans();
	displacement = target - position;
	if(distance > 0)
		displacement.Normalize(distance);
}

float MutationProcess::Sphere::calcZ()
{
	int xi = pose.trans().xi();
	int yi = pose.trans().yi();
	float radius = round(radius0*scale1);
	int delta = round(radius);
	float zh = max(max(max(height(xi - delta, yi), height(xi + delta, yi)), max(height(xi, yi - delta), height(xi, yi + delta))), height(xi, yi));
	return max(zh, GetRealHeight(pose.trans().x, pose.trans().y, radius)) + radius + mutation_sphere_delta_z;
}

void MutationProcess::Sphere::move(const Vect3f& dr)
{
	pose.trans() += dr;
	geometry->SetPosition(pose); 
}

void RigidBody::setScale(const Vect3f& s)
{ 
	geometry->SetScale(scale*s); 
	MatXf m(matrix()); 
	m.trans().z += box_min.z*(1 - s.z); 
	geometry->SetPosition(m); 
} 

void MutationProcess::logic_quant() 
{ 
	if(!logic_timer)
		clear();
}

void MutationProcess::start(const Vect3f& position)
{
	MTL();
	xassert(source.size() && target.size());
	phase = PreMelting;
	started = true;
	phase_time = scale_time();
	
	int time = 0;
	for(int j = 0; j <= MutationPhaseMax; j++) // пауза после процесса
		time += mutation_times[j];
	logic_timer.start(time);

	forward = 1;
	BodyList::iterator bi;
	float vol = 0;				
	FOR_EACH(source, bi){
		vol += dot((*bi)->composition(), Vect3f(mutation_base_volumes));
		if((*bi)->composition().norm2() > 1.1)
			forward = 0;
		}
	FOR_EACH(target, bi){
		(*bi)->geometry->SetAttr(ATTRUNKOBJ_IGNORE);
		}
	center = position;
	main_sphere.create(vol, center, true);
	center = main_sphere.pose.trans();
	SND3DPlaySound("transforming_units_disappear", &center);
}			      

void MutationProcess::show_quant()
{
	start_timer_auto(MutationProcessShow, STATISTICS_GROUP_LOGIC);

	BodyList::iterator bi;
	SphereList::iterator si;

	if(phase == MutationPhaseMax)
		return;
	
	float t = (scale_time() - phase_time)/(float)mutation_times[phase];
	if(t > 1)
		t = 1;
	switch(phase){
		case PreMelting:
			{
				MTAuto lock(getLock());
				FOR_EACH(source, bi)
					(*bi)->geometry->SetColor(0, &((*bi)->diffuse_color*sColor4f(1 - t, 1 - t, 1 - t, 1)), &(sColor4f(mutation_specular_color)*t));
			}
			break;

		case Melting:
			{
				MTAuto lock(getLock());
				FOR_EACH(source, bi)
					(*bi)->setScale(Vect3f::ID*(1 - t));							    
				FOR_EACH(spheres, si)
					si->set(t, 0);
			}
			break;

		case MovingIn:
			if(!sound_started && t > mutation_move_sound_start_phase){
				SND3DPlaySound(forward ? "transforming_small_move" : "transforming_mid_move", &center);
				sound_started = 1;
				}
			FOR_EACH(spheres, si)
				si->set(1, t);
			compact();
			break;

		case Merging:
			FOR_EACH(spheres, si)
				si->set(1 - t, t);
			main_sphere.set(t, 0);
			compact();
			break;

		case Separation:
			FOR_EACH(spheres, si)
				si->set(t, t);
			main_sphere.set(1 - t, 0);
			compact();
			break;

		case MovingOut:
			if(!sound_started && t > mutation_move_sound_start_phase){
				SND3DPlaySound(forward ? "transforming_mid_move" : "transforming_small_move", &center);
				sound_started = 1;
				}
			FOR_EACH(spheres, si)
				si->set(1, t);
			compact();
			break;

		case Freesing:
			{
				MTAuto lock(getLock());
				FOR_EACH(target, bi)
					(*bi)->setScale(Vect3f::ID*t);
				FOR_EACH(spheres, si)
					si->set(1 - t, 1);
			}
			break;

		case PostFreesing:
			{
				MTAuto lock(getLock());
				FOR_EACH(target, bi)
					(*bi)->geometry->SetColor(0, &((*bi)->diffuse_color*sColor4f(t, t, t, 1)), &(sColor4f(mutation_specular_color)*(1 - t)));
			}
			break;
		}

	if(t >= 1){
		phase++;
		t = 0;
		sound_started = 0;
		phase_time = scale_time();
		switch(phase){
			case Melting: {
					MTAuto lock(getLock());
					SND3DPlaySound(forward ? "transforming_base_collapse" : "transforming_units_collapse", &center);
					FOR_EACH(source, bi){
						spheres.push_back(Sphere());
						spheres.back().create(dot((*bi)->composition(), Vect3f(mutation_base_volumes)), (*bi)->position(), false);
					}
					break;
				}

			case MovingIn: 
				{
					MTAuto lock(getLock());
					FOR_EACH(spheres, si){
						si->setTarget(center);
						if(source.size() != 1)
							si->setTarget(center, si->displacement.norm() - si->radius);
						}
					break; 
				}

			case Merging: 
				SND3DPlaySound("transforming_large_appear", &center);
				FOR_EACH(spheres, si){
					si->setTarget(center);
					}
				if(spheres.size() == 1)
					phase++;
				else
					break;

			case Separation: 
				{
					MTAuto lock(getLock());
					SND3DPlaySound(forward ? "transforming_divide" : "transforming_small_appear", &center);
					FOR_EACH(source, bi)
						(*bi)->setScale(Vect3f::ZERO);
					spheres.clear();

					FOR_EACH(target, bi){
						spheres.push_back(Sphere());
						Sphere& s = spheres.back();
						s.create(dot((*bi)->composition(), Vect3f(mutation_base_volumes)), center, false);
						s.setTarget((*bi)->position(), s.radius);
						}
					if(target.size() == 1){
						phase++;
						main_sphere.set(0, 0);
						}
					else
						break;
				}

			case MovingOut: {
					MTAuto lock(getLock());
					//SND3DPlaySound(forward ? "transforming_mid_move" : "transforming_small_move", &center);
					int i = 0;
					FOR_EACH(spheres, si)
						si->setTarget(target[i++]->position());
				}
				break;

			case Freesing:
				{
					MTAuto lock(getLock());
					SND3DPlaySound(forward ? "transforming_units_appear" : "transforming_base_appear", &center);
					FOR_EACH(target, bi){
						(*bi)->geometry->ClearAttr(ATTRUNKOBJ_IGNORE);
						(*bi)->geometry->SetColor(0, &sColor4f(0,0,0,1), &sColor4f(mutation_specular_color));
						}
				}
				break;
	
			case MutationPhaseMax:
				{
					MTAuto lock(getLock());
					FOR_EACH(target, bi){
						(*bi)->setScale(Vect3f::ID);
						(*bi)->geometry->SetColor(0, &(*bi)->diffuse_color, &sColor4f(0,0,0,0));
						}
				}
				break;
			}
		show_quant();
		}
}			

void MutationProcess::compact()
{
	SphereList::iterator si, sj;
	if(phase == Merging || phase == Separation)
		FOR_EACH(spheres, si){
			Vect3f dr = center - si->pose.trans();
			dr.z = 0;
			dr.Normalize(mutation_compact_step);
			si->move(dr);
			}

	int iterations = 0;
	for(;;){ 
		int move = 0;
		FOR_EACH(spheres, si){
			Vect3f dr = si->pose.trans() - center;
			dr.z = 0;
			float d = (si->radius + main_sphere.radius)*mutation_compact_penetration_factor - dr.norm();
			if(main_sphere.radius > 0 && d > 0){
				dr.Normalize(d*mutation_compact_softer);
				si->move(dr);
				move = 1;
				}
			for(sj = si, ++sj; sj != spheres.end(); ++sj){
				Vect3f dr = si->pose.trans() - sj->pose.trans();
				dr.z = 0;
				float d = (si->radius + sj->radius)*mutation_compact_penetration_factor - dr.norm();
				if(d > 0){
					dr.Normalize(d/2*mutation_compact_softer);
					si->move(dr);
					sj->move(-dr);
					move = 1;
					}
				}
			}
		if(!move)
			break;
		if(iterations++ > mutation_compact_iterations)
			break;
		}
}

void MutationProcess::clear()
{
	MTL();
	started = false;

	BodyList::iterator i;
	FOR_EACH(source, i)
		(*i)->setUnderMutation(0);

	FOR_EACH(target, i){
		(*i)->setScale(Vect3f::ID);
		(*i)->setUnderMutation(0);
		}
	
	{
		MTAuto lock(getLock());
		source.clear(); 
		target.clear(); 
	}

	spheres.clear();
}


void MutationProcess::removeSourceOrTarget(RigidBody* body)
{ 
	MTAuto lock(getLock());

	source.erase(remove(source.begin(), source.end(), body), source.end());
	target.erase(remove(target.begin(), target.end(), body), target.end());
}
