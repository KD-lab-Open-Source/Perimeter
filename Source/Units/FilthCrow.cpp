#include "StdAfx.h"

#include "Universe.h"

#include "Config.h"
#include "ForceField.h"
#include "GenericFilth.h"
#include "Region.h"
#include "IronFrame.h"

#include "IronExplosion.h"
#include "FilthCrow.h"
#include "filth.hi"

terFilthSwarmCrow::terFilthSwarmCrow(terFilthSpot* spot, const Vect3f& pos,int attack_period_)
: terFilthSwarm(spot,pos)
{
	generate_creature_num=0;
	attack_pos=pos;
	attack_period=attack_period_;
	sound.Init("Filth_Move_Crow");
	sound.SetPos(To3D(position));
	sound.Play();
}

void terFilthSwarmCrow::SetCreatureGeneration(int creature_num,int generation_period)
{
	generate_creature_num=creature_num;
}

terFilthSwarmCrow::~terFilthSwarmCrow()
{
	FilthListType::iterator it;
	FOR_EACH(unit_list,it)
		(*it)->Kill();
}

void terFilthSwarmCrow::SetFreeDestroy()
{
	FilthListType::iterator it;
	FOR_EACH(unit_list,it)
		(*it)->SetFreeDestroy();
}

void terFilthSwarmCrow::Quant()
{
	Generate();
	if(generate_creature_num==0 && unit_list.empty())
		Alive = 0;

	if(unit_list.empty())
		return;

	if(!TargetPoint)
		FindTargetPoint();

	if(TargetPoint)
		attack_pos=TargetPoint->position();

	float attack_distance=attack_pos.distance2(position);
	if(attack_distance<terFilthCrowPrm.linear_speed && TargetPoint)
	{
		int num_target=0;
		FilthListType::iterator it;
		FOR_EACH(unit_list,it)
		{
			if((*it)->GetTarget())
				num_target++;
		}

		int crow_attack=terFilthCrowPrm.at_one_time_atrack;
		if(num_target==0 || (num_target<crow_attack && unit_list.size()>=crow_attack))
		{
			while(num_target<crow_attack)
			{
				terFilthCrow* find=NULL;
				float dist2=0;

				FilthListType::iterator it;
				FOR_EACH(unit_list,it)
				{
					if((*it)->GetTarget())
						continue;
					float d=attack_pos.distance2((*it)->position());
					if(!find || d<dist2)
					{
						dist2=d;
						find=*it;
					}
				}

				if(!find)
					break;
				find->SetTarget(TargetPoint);
				num_target++;
			}
		}
	}

	if(attack_distance>2*sqr(terFilthCrowPrm.swarm_speed))
	{
		Vect3f v=attack_pos-position;
		v.Normalize();
		v*=terFilthCrowPrm.swarm_speed;
		position+=v;

		FilthListType::iterator it;
		FOR_EACH(unit_list,it)
		{
			(*it)->SetFilthTarget(position);
		}
	}

	sound.SetPos(To3D(position));
}

void terFilthSwarmCrow::DestroyLink()
{
	terFilthSwarm::DestroyLink();
	DestroyLinkList(unit_list);
}

terUnitAttributeID terFilthSwarmCrow::GetUnitID()
{
	switch(SpotPoint->GetFilthParamID())
	{
	case FILTH_SPOT_ID_CROW:
		return UNIT_ATTRIBUTE_FILTH_CROW;
	case FILTH_SPOT_ID_A_CROW:
		return UNIT_ATTRIBUTE_FILTH_A_CROW;
	}
	xassert(0);
	return UNIT_ATTRIBUTE_FILTH_CROW;
}

void terFilthSwarmCrow::Generate()
{
	if(generate_creature_num==0)
		return;

	for(int i=0;i<generate_creature_num;i++)
	{
		Vect3f pos;
		pos.x=position.x+2*terFilthCrowPrm.linear_speed*terLogicRNDfrnd();
		pos.y=position.y+2*terFilthCrowPrm.linear_speed*terLogicRNDfrnd();
		pos.z=0;
		pos=To3D(pos);
		pos.z+=terFilthCrowPrm.z;

		terFilthCrow* p;
		p = safe_cast<terFilthCrow*>(player->buildUnit(GetUnitID()));
		p->setPose(Se3f(QuatF::ID, pos), false);
		p->SetFilthTarget(position);
		Vect3f v = pos-position;
		p->SetDeltaTarget(v);
		p->SetAttackPeriod(attack_period);
		p->Start();
		p->setPose(Se3f(QuatF::ID, pos), false);
		unit_list.push_back(p);

		playDetectedSound();
	}

	generate_creature_num=0;
}


////////////////////////////terFilthCrow/////////////////////////////
terFilthCrow::terFilthCrow(const UnitTemplate& data) : terFilthSpline(data)
{
	target_position = Vect3f::ZERO;
	delta_target=Vect3f::ZERO;
	cur_way_point=0;
	acceleration=Vect3f::ZERO;
	go_out=true;
	TargetPoint=NULL;
	attack_mode=false;
	must_die=false;
	free_destroy=false;
	death_request=false;
}

terFilthCrow::~terFilthCrow()
{
}

void terFilthCrow::Start()
{
	terUnitGeneric::Start();

	WayPointStart();
	AvatarQuant();
	avatar()->Start();
	realAvatar()->setPhase(terLogicRNDfrand());

}

void terFilthCrow::AvatarQuant()
{
	terFilthSpline::AvatarQuant();
	avatar()->Show();

	if(show_filth_debug)
		show_vector(To3D(target()),4,RED);
}

void terFilthCrow::Quant()
{
	terFilthSpline::Quant();

	if(clusterColliding())
		FieldEffect();
	if(clusterColliding() || !damageMolecula().isAlive())
	{
		StartDeath(true);
	}
		

	if(attack_mode)
	{
		if(way_points.size()<4)
		{
			if(!must_die && !death_request)
				realAvatar()->requestChain(CHAIN_CLOSE);
			must_die=true;
		}

		if(way_points.size()<3)
		{
			if(TargetPoint && damageMolecula().isAlive())
			{
				float dist=position().distance(TargetPoint->position());
				float r2=radius()+TargetPoint->radius();
				if(dist<r2)
					TargetPoint->setDamage(damageData(),this);
			}

			StartDeath(false);
		}
	}

	if(realAvatar()->requestDone() && 
		realAvatar()->phase()>0.999f && 
		realAvatar()->chainID()==CHAIN_DIE)
	{
		Kill();
		return;
	}
}

void terFilthCrow::AttackMode()
{
	if(attack_mode)
		return;
	attack_mode=true;
	bool b=attack_sound.Init("Filth_Attack_Crow");
	attack_sound.SetPos(position());
	attack_sound.Play();
}

void terFilthCrow::addWayPoint()
{
	if(attack_mode || death_request)
		return;
	Vect3f p;

	if((TargetPoint || free_destroy) && !way_points.empty())
	{
		Vect3f target_pos=Vect3f::ID;
		if(TargetPoint)
			target_pos=TargetPoint->position();
		else
		{
			if(way_points.size()>=2)
			{
				Vect3f norm=way_points.back()-*(++way_points.rbegin());
				norm.Normalize();
				target_pos=To3D(way_points.back()+norm*terFilthCrowPrm.free_destroy_len);
			}else
			{
				target_pos=way_points.back();
			}

			p=target_pos;
			AttackMode();
			way_points.push_back(p);
			return;
		}

		Vect3f n=target_pos-way_points.back();
		n.z=0;
		float dist=n.norm();
		if(dist<terFilthCrowPrm.linear_speed)
		{
			p=target_pos;
			AttackMode();
			way_points.push_back(p);
			return;
		}else
		{
			n*=terFilthCrowPrm.linear_speed/dist;
			p.x=way_points.back().x+n.x;
			p.y=way_points.back().y+n.y;
		}
	}else
	if(way_points.size()>=2)
	{
		Vect3f p1=way_points.back();
		Vect3f p2=*(++way_points.rbegin());
		Vect3f velocity=Normalize(p1-p2);
		Vect3f to_center=target()-p1;
		float distance=to_center.norm();
		float dot=velocity.dot(to_center);
		if(dot<0.8f)
		{
			if(go_out && distance>terFilthCrowPrm.linear_speed*3)
			{
				go_out=false;
			}

			float dfactor=go_out?0:1;

			float sgn=(velocity%to_center).z;
			float angle=(sgn>0?+1:-1)*dfactor*M_PI/4;
			Vect2f v=Mat2f(angle)*Vect2f(velocity);
			velocity.x=v.x;
			velocity.y=v.y;
		}else
			go_out=true;

		velocity.Normalize();
		velocity*=terFilthCrowPrm.linear_speed;
		p.x=way_points.back().x+velocity.x;
		p.y=way_points.back().y+velocity.y;
	}else
	{
		if(way_points.empty())
		{
			p.x=target().x;
			p.y=target().y;
		}else
		{
			Vect3f velocity(terLogicRNDfrnd(),terLogicRNDfrnd(),0);
			velocity.Normalize();
			p.x=way_points.back().x+velocity.x*terFilthCrowPrm.linear_speed;
			p.y=way_points.back().y+velocity.y*terFilthCrowPrm.linear_speed;
		}
	}

	p.z=0;
	p=To3D(p);
	p.z+=terFilthCrowPrm.z;
	way_points.push_back(p);
}

void terFilthCrow::DestroyLink()
{
	if(TargetPoint && !(TargetPoint->alive()))
		TargetPoint = NULL;
}

void terFilthCrow::Collision(terUnitBase* p)
{
	if(!damageMolecula().isAlive())
		return;
	if(isEnemy(p))
	{
		p->setDamage(damageData(),this);
		StartDeath(true);
	}
}

void terFilthCrow::StartDeath(bool immediate)
{
	if(death_request)
		return;
	death_request=true;
	splashDamage();
	damageMoleculaKill();
	setCollisionGroup(0);
//	explode();
	way_points.clear();
	soundEvent(SOUND_EVENT_EXPLOSION);

	if(immediate)
	{
		realAvatar()->setChain(CHAIN_DIE);
		realAvatar()->setPhase(0);
	}else
	{
		realAvatar()->requestChain(CHAIN_DIE);
	}

}

SaveUnitData* terFilthSwarmCrow::universalSave(SaveUnitData* baseData)
{
	SaveFilthSwarmCrow* data = castOrCreate<SaveFilthSwarmCrow>(baseData);
	terFilthSwarm::universalSave(data);

	data->generate_creature_num=generate_creature_num;
	data->attack_pos=attack_pos;
	data->attack_period=attack_period;
	
	vector<SaveUnitData*> unitList;
	FilthListType::iterator it;
	FOR_EACH(unit_list,it)
	if(*it)
	{
		data->unitList.push_back((*it)->universalSave(0));
	}
	return data;
}

void terFilthSwarmCrow::universalLoad(const SaveUnitData* baseData)
{
	terFilthSwarm::universalLoad(baseData);
	const SaveFilthSwarmCrow* data = safe_cast<const SaveFilthSwarmCrow*>(baseData);
	generate_creature_num=data->generate_creature_num;
	attack_pos=data->attack_pos;
	attack_period=data->attack_period;

	SaveUnitDataList::const_iterator it;
	FOR_EACH(data->unitList,it)
	if(*it)
	{
		terFilthCrow* unit = safe_cast<terFilthCrow*>(player->buildUnit((*it)->attributeID));
		unit_list.push_back(unit);
		unit->universalLoad(*it);
	}
}

SaveUnitData* terFilthCrow::universalSave(SaveUnitData* baseData)
{
	SaveFilthCrow* data = castOrCreate<SaveFilthCrow>(baseData);
	terFilthSpline::universalSave(data);

	data->free_destroy=free_destroy;
	data->target_position=target_position;
	data->delta_target=delta_target;
	data->go_out=go_out;
	data->attack_mode=attack_mode;
	data->must_die=must_die;
	data->death_request=death_request;

	return data;
}

void terFilthCrow::universalLoad(const SaveUnitData* baseData)
{
	terFilthSpline::universalLoad(baseData);
	const SaveFilthCrow* data = safe_cast<const SaveFilthCrow*>(baseData);

	free_destroy=data->free_destroy;
	target_position=data->target_position;
	delta_target=data->delta_target;
	go_out=data->go_out;
	attack_mode=data->attack_mode;
	must_die=data->must_die;

	if(data->death_request)
		StartDeath(true);
}
