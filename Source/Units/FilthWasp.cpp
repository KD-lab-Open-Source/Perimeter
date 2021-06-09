#include "StdAfx.h"
#include "Universe.h"

#include "Config.h"
#include "ForceField.h"
#include "GenericFilth.h"
#include "Region.h"
#include "IronFrame.h"

#include "IronExplosion.h"
#include "FilthWasp.h"
#include "filth.hi"

terFilthSwarmWasp::terFilthSwarmWasp(terFilthSpot* spot, const Vect3f& pos,int attack_period_)
: terFilthSwarm(spot,pos)
{
	prm=NULL;
	unit_id=UNIT_ATTRIBUTE_FILTH_WASP;
	TargetPosition = Vect3f::ZERO;

	attack_period = attack_period_;
	attack_count=0;

}

void terFilthSwarmWasp::SetHole(vector<terFilthSpot::HoleStruct>& hole_position_)
{
	for(int i=0;i<hole_position_.size();i++)
	{
		Vect3f v(hole_position_[i].pos.x,hole_position_[i].pos.y,0);

		if(SpotPoint && SpotPoint->terCheckFilthPoint(round(v.x),round(v.y)) && 
			!terCheckFilthChaos(round(v.x),round(v.y)))
		{
			hole_position.push_back(v);

			if(SpotPoint->initialGeoprocess() && !hole_position_[i].inited)
				wasp_hole_point.push_back(new s_WaspBirthGeoAction(round(v.x),round(v.y),4));
			hole_position_[i].inited=true;
		}
	}
}

void terFilthSwarmWasp::SetPrm(terUnitAttributeID id_)
{
	unit_id=id_;

	if(unit_id==FILTH_SPOT_ID_EYE)
		prm=&terFilthWaspEyePrm;
	else
		prm=&terFilthWaspPrm;

	sound.Init(unit_id==UNIT_ATTRIBUTE_FILTH_WASP?"Filth_Move_Wasp":"Filth_Move_Eye");
	sound.SetPos(To3D(position));
	sound.Play();
}

void terFilthSwarmWasp::SetCreatureGeneration(int creature_num,int generation_period)
{
	gen.Set(creature_num,generation_period);
}

void terFilthSwarmWasp::SetFreeDestroy()
{
	FilthListType::iterator i_unit;
	FOR_EACH(unit_list,i_unit){
		(*i_unit)->SetFreeDestroy();
	}
}

terFilthSwarmWasp::~terFilthSwarmWasp()
{
	FilthListType::iterator i_unit;

	for(int i=0;i<wasp_hole_point.size();i++)
		delete wasp_hole_point[i];

	FOR_EACH(unit_list,i_unit){
		(*i_unit)->Kill();
	}
}

void terFilthSwarmWasp::Quant()
{
	terFilthSwarm::Quant();

	if(!gen.InProcess() && unit_list.empty())
		Alive = 0;

	attack_period--;
	FilthListType::iterator it;
	if(attack_period==0)
	{
		FOR_EACH(unit_list,it)
			(*it)->SetFreeDestroy();
	}


	if(!TargetPoint)
		FindTargetPoint();

	FOR_EACH(unit_list,it)
	{
		if(TargetPoint)
		{
			RigidBody* rb=TargetPoint->GetRigidBodyPoint();
			Vect3f ps=rb->matrix()*rb->boxMax();
			Vect3f pos=TargetPoint->position();
			pos.z=ps.z;
			(*it)->SetFilthTarget(TargetPoint->position(),TargetPoint->radius());
		}
		else
			(*it)->SetFilthTarget(position+Vect3f(0,1,0),1);
		(*it)->SetFilthPos(position);
	}

	if(!wasp_hole_point.empty())
	{
		for(int i=0;i<wasp_hole_point.size();i++)
		if(!wasp_hole_point[i]->quant())
		{
			delete wasp_hole_point[i];
			wasp_hole_point.erase(wasp_hole_point.begin()+i);
			i--;
		}
	}else
	{
		if(gen.InProcess())
		{
			GenerationProcess();
		}
	}

	sound.SetPos(To3D(position));

	if(wasp_hole_point.empty() && !gen.InProcess())
	{
		bool dist_far=true;
		float swarm_height=prm->height;
		if(TargetPoint)
		{
			Vect3f dist=TargetPoint->position()-position;
			
			float delta=prm->swarm_speed*0.1f;
			float norm=dist.norm();
			dist_far=norm>5;//FILTH_WASP_RAND_MAX;
			if(dist_far)
			{
				delta=min(delta,norm);
				position+=Normalize(dist)*delta;
			}
		}

		if(dist_far)
			attack_count=0;
		else
			attack_count+=prm->creature_attack_count;

		int num=attack_count;
		attack_count-=num;
		FOR_EACH(unit_list,it)
		{
			terFilthWasp* p=*it;
			if(p->GetAttackMode())
				continue;
			if(num<=0)
				break;
			num--;
			p->SetSwarmHeight(swarm_height);
			p->SetAttackMode(true);
		}
	}

	if(!unit_list.empty())
	{
		float summary_z=0;
		int num=0;
		FOR_EACH(unit_list,it)
		{
			summary_z+=(*it)->GetLastZ();
			num++;
		}

		summary_z/=num;
		FOR_EACH(unit_list,it)
		{
			(*it)->SetSummaryZ(summary_z);
		}
	}
}

void terFilthSwarmWasp::DestroyLink()
{
	terFilthSwarm::DestroyLink();
	DestroyLinkList(unit_list);

	if(TargetPoint && !(TargetPoint->alive()))
		TargetPoint = NULL;
}

void terFilthSwarmWasp::GenerationProcess()
{
	int num = gen.Quant();

	if(!hole_position.empty())
	for(int i = 0;i < num;i++)
	{
		int holei=terLogicRND(hole_position.size());
		Vect3f pos(hole_position[holei].x,hole_position[holei].y,0);

		if(SpotPoint && SpotPoint->terCheckFilthPoint(round(pos.x),round(pos.y)))
		{
			terFilthWasp* p = safe_cast<terFilthWasp*>(player->buildUnit(GetUnitID()));
			p->SetSwarm(this);
			p->setPose(Se3f(QuatF::ID, pos), true);
			p->Start();
			unit_list.push_back(p);

			playDetectedSound();
		}
	}
}

//---------------------------------------------------------

terFilthWasp::terFilthWasp(const UnitTemplate& data) : terFilthSpline(data)
{
	swarm=NULL;
	target_position=Vect3f::ZERO;
	center_position=Vect3f::ZERO;

	free_destroy=false;
	setPointTime(0.3f);
	setUp(Vect3f(0,0,1));
	setFront(Vect3f(0,1,0));
	begin_move=true;
	summary_z=0;
	attack=false;
	target_radius=1;
	no_add_point=false;

	delta_center.x=50;
	delta_center.y=50;
	delta_center.z=50;
	swarm_height=50;
}

void terFilthWasp::Start()
{
	delta_center.x=terLogicRNDfrnd()*swarm->prm->rand_delta;
	delta_center.y=terLogicRNDfrnd()*swarm->prm->rand_delta;
	delta_center.z=terLogicRNDfrnd()*swarm->prm->rand_delta;
	swarm_height=swarm->prm->height;

	terFilthSpline::Start();

	WayPointStart();
	AvatarQuant();
	avatar()->Start();
	realAvatar()->setPhase(terLogicRNDfrand());

}

void terFilthWasp::Quant()
{
	terFilthSpline::Quant();

	if(clusterColliding() || !damageMolecula().isAlive())
	{
		if(clusterColliding())
			FieldEffect();
		SetFreeDestroy();
	}

	if(free_destroy && realAvatar()->requestDone() && realAvatar()->phase()>0.999f && realAvatar()->chainID()==CHAIN_DIE)
	{
		soundEvent(SOUND_EVENT_EXPLOSION);
		Kill();
	}
}

void terFilthWasp::Collision(terUnitBase* p)
{
	if(!damageMolecula().isAlive())
		return;
	if(isEnemy(p))
	{
		p->setDamage(damageData(),this);
		SetFreeDestroy();
	}
}

void terFilthWasp::AvatarQuant()
{
	terFilthSpline::AvatarQuant();
	avatar()->Show();
//	realAvatar()->setSight(DestroyAnimation.phase());
}


float terFilthWasp::GetLastZ()
{
	if(way_points.empty())
		return 0;
	Vect3f p=To3D(way_points.back());
	return p.z;
}

void terFilthWasp::SetFreeDestroy()
{
	if(free_destroy)
		return;
	free_destroy=true;
	realAvatar()->setChain(CHAIN_DIE);
	damageMoleculaKill();
	way_points.clear();
}

void terFilthWasp::addWayPoint()
{
	if(way_points.empty())
	{
		way_points.push_back(position());
		return;
	}
	if(free_destroy)
		return;

	{
		Vect3f p=target_position-way_points.back();
		p.z=0;
		setFront(p);
	}
/*
	if(free_destroy)
	{
		way_points.push_back(way_points.back());
		return;
	}
*/
	if(attack && !free_destroy)
	{
		Vect3f tp=target_position;
		Vect3f delta2d=target_position-way_points.back();
		delta2d.z=0;
		tp-=Normalize(delta2d)*(target_radius*0.6f);

		Vect3f delta=tp-way_points.back();
		float norm=delta.norm();

		Vect3f pos=tp;
		float attack_speed=swarm->prm->attack_speed*getPointTime();
		if(norm>attack_speed)
		{
			pos=way_points.back()+delta*(attack_speed/norm);
		}

		if(norm<FLT_EPS)
		{
			damageMoleculaKill();
		}

		way_points.push_back(pos);
		return;
	}

	float speed=swarm->prm->speed*getPointTime();
	Vect3f prev_pos=way_points.back();
	Vect3f prev_pos_z=To3D(prev_pos);
	prev_pos_z.z+=swarm_height;

	Vect3f cp=To3D(center_position);
	cp.z=summary_z+swarm_height;


	Vect3f delta=cp+delta_center-prev_pos;
	Vect3f n=Normalize(delta);

	if(begin_move)
	{
		float dz=prev_pos_z.z-prev_pos.z;
		if(fabsf(dz)>speed)
		{
			Vect3f pos=prev_pos;
			pos+=n;
			pos.z+=speed*SIGN(dz);

			way_points.push_back(pos);
			return;
		}

		if(delta.norm()>speed)
		{
			Vect3f pos=prev_pos;
			pos+=n*speed;
			way_points.push_back(pos);
			return;
		}
	}

	begin_move=false;


	if(terLogicRND(4)==0)
	{
		if(delta_center.norm()>swarm->prm->rand_max)
		{
			delta_center.x+=-terLogicRNDfrand()*swarm->prm->rand_delta*SIGN(delta_center.x);
			delta_center.y+=-terLogicRNDfrand()*swarm->prm->rand_delta*SIGN(delta_center.y);
			delta_center.z+=-terLogicRNDfrand()*swarm->prm->rand_delta*SIGN(delta_center.z);
			
		}else
		{
			delta_center.x+=terLogicRNDfrnd()*swarm->prm->rand_delta;
			delta_center.y+=terLogicRNDfrnd()*swarm->prm->rand_delta;
			delta_center.z+=terLogicRNDfrnd()*swarm->prm->rand_delta;
		}
	}
	//pos+=delta_center;
	Vect3f pos=cp+delta_center;
	
	way_points.push_back(pos);
}

SaveUnitData* terFilthSwarmWasp::universalSave(SaveUnitData* baseData)
{
	SaveFilthSwarmWasp* data = castOrCreate<SaveFilthSwarmWasp>(baseData);
	terFilthSwarm::universalSave(data);

	data->unit_id=unit_id;
	gen.Save(data->generate);
	data->attack_period=attack_period;
	data->TargetPosition=TargetPosition;
	data->attack_count=attack_count;

	FilthListType::iterator it;
	FOR_EACH(unit_list,it)
	if(*it)
	{
		data->unitList.push_back((*it)->universalSave(0));
	}

	return data;
}

void terFilthSwarmWasp::universalLoad(const SaveUnitData* baseData)
{
	terFilthSwarm::universalLoad(baseData);
	const SaveFilthSwarmWasp* data = safe_cast<const SaveFilthSwarmWasp*>(baseData);

	unit_id=data->unit_id;
	gen.Load(data->generate);
	attack_period=data->attack_period;
	TargetPosition=data->TargetPosition;
	attack_count=data->attack_count;

	SetHole(SpotPoint->GetHolePos());
	SetPrm(unit_id);

	SaveUnitDataList::const_iterator it;
	FOR_EACH(data->unitList,it)
	if(*it)
	{
		terFilthWasp* unit = safe_cast<terFilthWasp*>(player->buildUnit((*it)->attributeID));
		unit->SetSwarm(this);
		unit_list.push_back(unit);
		unit->universalLoad(*it);
	}
}

SaveUnitData* terFilthWasp::universalSave(SaveUnitData* baseData)
{
	SaveFilthWasp* data = castOrCreate<SaveFilthWasp>(baseData);
	terFilthSpline::universalSave(data);

	data->free_destroy=free_destroy;
	data->target_position=target_position;
	data->center_position=center_position;
	data->delta_center=delta_center;
	data->begin_move=begin_move;
	data->summary_z=summary_z;
	data->swarm_height=swarm_height;
	data->attack=attack;
	data->target_radius=target_radius;
	data->no_add_point=no_add_point;

	return data;
}

void terFilthWasp::universalLoad(const SaveUnitData* baseData)
{
	const SaveFilthWasp* data = safe_cast<const SaveFilthWasp*>(baseData);

	target_position=data->target_position;
	center_position=data->center_position;
	delta_center=data->delta_center;
	begin_move=data->begin_move;
	summary_z=data->summary_z;
	swarm_height=data->swarm_height;
	attack=data->attack;
	target_radius=data->target_radius;
	no_add_point=data->no_add_point;

	terFilthSpline::universalLoad(data);
	if(data->free_destroy)
		SetFreeDestroy();
}
