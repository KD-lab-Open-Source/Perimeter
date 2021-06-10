#include "StdAfx.h"

#include "Universe.h"

#include "Config.h"
#include "ForceField.h"
#include "GenericFilth.h"
#include "Region.h"
#include "IronFrame.h"

#include "IronExplosion.h"
#include "FilthEye.h"

#include "CameraManager.h"
#include "FilthEye.h"
#include "filth.hi"

terFilthSwarmEye::terFilthSwarmEye(terFilthSpot* spot, const Vect3f& pos,int attack_period_)
: terFilthSwarm(spot,pos)
{
	TargetPosition = Vect3f::ZERO;

	attack_period = attack_period_;

	center_pos=position;
	destruction=false;
}

void terFilthSwarmEye::SetCreatureGeneration(int creature_num,int generation_period)
{
	gen.Set(creature_num,generation_period);
}

terFilthSwarmEye::~terFilthSwarmEye()
{
	FilthListType::iterator i_unit;

	FOR_EACH(unitList,i_unit){
		(*i_unit)->Kill();
	}
}

void terFilthSwarmEye::Quant()
{
	terFilthSwarm::Quant();

	if(!gen.InProcess() && unitList.empty())
		Alive = 0;

	if(gen.InProcess()){
		GenerationProcess();
	}

	if(!unitList.empty()){
		SnakeMove();
	}

	if(TargetPoint)
	{
		show_vector(TargetPoint->position(),4,RED);
	}
}

void terFilthSwarmEye::SnakeMove()
{
	if(unitList.empty())
		return;
	FilthListType::iterator it;
	Vect3f v=center_pos;
	if(!TargetPoint)
		FindTargetPoint();
	if(TargetPoint)
		v = TargetPoint->position();

	int cur_num=0;
	FOR_EACH(unitList,it)
	{
		(*it)->SetFilthHeight(cur_num*(*it)->BodyPoint->radius());
		cur_num++;
	}


	float maxz=0;
	terFilthEye* maxp=NULL;
	FOR_EACH(unitList,it)
	{
		terFilthEye* p=*it;
		if(p->position().z>maxz)
		{
			maxz=p->position().z;
			maxp=p;
		}

		p->SetFilthTop(false);
	}

	if(maxp)
		maxp->SetFilthTop(true);

	if(!destruction)
	{
		bool des=true;
		FOR_EACH(unitList,it)
			des=des && (*it)->IsEnd();

		if(des && attack_period<0)
		{
			destruction=des;
			FOR_EACH(unitList,it)
			{
				(*it)->SetFilthAttackPosition(v);
				(*it)->SetDestroy();
			}
		}
	}

	attack_period--;
}

void terFilthSwarmEye::DestroyLink()
{
	terFilthSwarm::DestroyLink();
	DestroyLinkList(unitList);
}

void terFilthSwarmEye::GenerationProcess()
{
	int num,i;
	terFilthEye* p;
	Vect3f v;

	num = gen.Quant();
	float r=terFilthEyePrm.CreatureGenerationRadius;

	for(i = 0;i < num;i++)
	{
		Vect3f pos;
		pos.x=center_pos.x+r*terLogicRNDfrnd();
		pos.y=center_pos.y+r*terLogicRNDfrnd();
		pos.z=0;

		if(SpotPoint && SpotPoint->terCheckFilthPoint(round(pos.x),round(pos.y)))
		{
			p = safe_cast<terFilthEye*>(player->buildUnit(GetUnitID()));
			p->setPose(Se3f(QuatF::ID, pos), false);
			p->SetFilthTarget(center_pos);
			p->SetFilthHeight(unitList.size()*p->BodyPoint->radius());
			p->SetFilthSpeedAngle(terFilthEyePrm.speed_angle_z*terLogicRNDfrnd());
			p->Start();
			unitList.push_back(p);

			playDetectedSound();
		}
	}
}

//---------------------------------------------------------

terFilthEye::terFilthEye(const UnitTemplate& data) : terFilthGeneric(data)
{
	target_position = Vect3f::ZERO;
	attack_position= Vect3f::ZERO;
	begin_destruct_terra = Vect3f::I;

	destruction = false;
	last_attack = false;
	request_attack=false;
	AttackAnimation.startPhase(0,0.05f,false);
	destroy_flag = false;

	ObjectAnimation.startPhase(0,0.9f,true);
	BeginAnimation.startPhase(0,0.025f,false);
	zeroPositionTimer_.start(100);
	height_terra=0;

	set_way_point=false;
	bubble=NULL;
	sphere_node=NULL;
	angle_z=0;
	speed_angle_z=0.1f;
	interpolate_angle_z=angle_z;

	is_top=false;
	pre_destruction=false;
	pre_destruction_time=0;

	TopAnimation.startPhase(0,0.025f,false);
	TopAnimation.setEndPhase(0,false);
}

terFilthEye::~terFilthEye()
{
	delete bubble;
}

void terFilthEye::SetFilthTarget(Vect3f& v)
{
	target_position = v;
}

void terFilthEye::SetDestroy()
{
	if(pre_destruction || destruction)
		return;
	pre_destruction=true;
	begin_destruct_pos=pose();
	begin_destruct_terra=To3D(begin_destruct_pos.trans());
}

void terFilthEye::WayPointStart()
{
	const int rb=8;
	if(!terCheckFilthChaos(position()))
		bubble=new sTBubble(round(position().x),round(position().y),rb*2,rb*2);
	terFilthGeneric::WayPointStart();
	avatar()->SetChain("main");
	DestroyAnimation.startPhase(1.0f,-0.1f,false);
	BodyPoint->setFlyingHeight(terFilthEyePrm.height_down);

	sphere_node=avatar()->GetModelPoint()->FindObject("sphere");
}

void terFilthEye::setPose(const Se3f& poseIn, bool initPose)
{
	Se3f poseZ = poseIn;
	if(zeroPositionTimer_())
		poseZ.trans().z = -100;	
	terFilthGeneric::setPose(poseZ, initPose);
}

bool terFilthEye::is_point_reached(const Vect3f& point)
{
/*
	if(BodyPoint->is_point_reached(point))
	{
		Vect3f pnt=To3D(point);
		return fabsf(BodyPoint->flying_height+pnt.z-position().z)<4;
	}
/*/
	Vect3f pnt=To3D(point);
	pnt.z+=BodyPoint->flyingHeight();
	float d2=pnt.distance2(position());
	if(d2<sqr(15))
		return true;
/**/
	return false;
}

void terFilthEye::WayPointController()
{
	terFilthGeneric::WayPointController();

	BodyPoint->setVelocityFactor(1);
	BodyPoint->setFlyingHeight(LinearInterpolate(terFilthEyePrm.height_down,height_terra,BeginAnimation.phase()));
	BodyPoint->setFlyingMode(true);


	if(BodyPoint->way_points.size())
	if(is_point_reached(BodyPoint->way_points[0]))
	{
		BodyPoint->way_points.clear();
	}

}

void terFilthEye::Quant()
{
	terFilthGeneric::Quant();
	if(clusterColliding())
		SetDestroy();
}

void terFilthEye::AvatarQuant()
{
	terFilthGeneric::AvatarQuant();
	RequestAttackQuant();

	Se3f attack_angle(QuatF(M_PI/2*AttackAnimation.phase(),Vect3f(0,0,1)),Vect3f(0,0,0));

	avatar()->Show();
	avatar()->setPose(BodyPoint->pose()*attack_angle);
	realAvatar()->setPhase(ObjectAnimation.phase());
	realAvatar()->setSight(DestroyAnimation.phase());

	ObjectAnimation.Quant();
	BeginAnimation.Quant();
	if(destroy_flag)
		DestroyAnimation.Quant();
	TopAnimation.Quant();

	if(!set_way_point && BeginAnimation.isEnd())
	{
		BodyPoint->way_points.clear();
		BodyPoint->way_points.push_back(target_position);
		set_way_point=true;
	}

	if(bubble)
	if(!bubble->quant())
	{
		delete bubble;
		bubble=NULL;
	}

	if(IsEnd())
	{
		angle_z+=speed_angle_z;
		interpolate_angle_z=angle_z;
	}

	if(pre_destruction)
	{
		pre_destruction_time++;
	}

	if(BodyPoint->clusterColliding())
		damageMoleculaKill();

	if(alive() && (!damageMolecula().isAlive() || destruction)){
		if(!destroy_flag){
			destroy_flag = true;
			setCollisionGroup(0);
			if(destruction)
				explode();
		}else{
			if(DestroyAnimation.isEnd()){
				Kill();
			}
		}
	}
}

void terFilthEye::MoveQuant()
{
	if(pre_destruction || destruction)
	{
		float t=pre_destruction_time*0.1f;
		float pre_destruct_angle=terFilthEyePrm.destruct_angle_acceleration*t*t*0.5f;
		float h=max(begin_destruct_pos.trans().z-begin_destruct_terra.z,10.0f);
		Vect3f direction;
		direction.x=-(attack_position.y-begin_destruct_terra.y);
		direction.y=(attack_position.x-begin_destruct_terra.x);
		direction.z=0;
		direction.Normalize();

		QuatF rot(pre_destruct_angle,direction);
		Se3f sepos,serot,sedown;
		sepos.rot()=QuatF::ID;
		sepos.trans()=begin_destruct_terra;
		serot.rot()=begin_destruct_pos.rot();
		serot.trans()=Vect3f(0,0,h);
		sedown.rot()=rot;
		sedown.trans()=Vect3f::ID;

		setPose(sepos*sedown*serot, false);

		Vect3f terra_pos=To3D(position());
		if(terra_pos.z>position().z)
		{
			destruction=true;
			pre_destruction=false;
		}
	}else
		terFilthGeneric::MoveQuant();
}

void terFilthEye::RequestAttackQuant()
{
	if(request_attack==last_attack)
		return;
	if(request_attack)
	{
		AttackAnimation.setPeriod(2000);
		AttackAnimation.setEndPhase(1,false);
	}else
	{
		AttackAnimation.setPeriod(-2000);
		AttackAnimation.setEndPhase(0,false);
	}
	
	if(AttackAnimation.Quant())
	{
		last_attack=request_attack;
	}

}

void terFilthEye::Collision(terUnitBase* p)
{
	if(isEnemy(p))
	{
		p->setDamage(damageData(),this);

		SetDestroy();
		destruction=true;
	}
}

void terFilthEye::AvatarInterpolation()
{
	terUnitReal::AvatarInterpolation();
	interpolate_angle_z(sphere_node,Z_AXIS);
}


void terFilthEye::SetFilthTop(bool b)
{
	if(is_top!=b)
	{
		TopAnimation.setEndPhase(b?1:0,false);
		TopAnimation.restart();
	}

	is_top=b;
}