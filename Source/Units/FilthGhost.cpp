#include "StdAfx.h"

#include "Runtime.h"
#include "Universe.h"

#include "Config.h"
#include "ForceField.h"
#include "GenericFilth.h"
#include "Region.h"
#include "IronFrame.h"

#include "IronExplosion.h"
#include "FilthGhost.h"
#include "filth.hi"

terFilthSwarmGhost::terFilthSwarmGhost(terFilthSpot* spot, const Vect3f& pos,int attack_period_)
: terFilthSwarm(spot,pos)
{
	TargetPosition = Vect3f::ZERO;

	attack_period = attack_period_;

	angle_z=terLogicRNDfrnd()*2*M_PI;
	sound.Init("Filth_Move_Ghost");
	sound.SetPos(To3D(pos));
	sound.Play();

	sound_attack.Init("Filth_Attack_Ghost");
	sound_attack.SetPos(To3D(pos));
}

void terFilthSwarmGhost::SetCreatureGeneration(int creature_num,int generation_period)
{
	gen.Set(creature_num,generation_period);
}

terFilthSwarmGhost::~terFilthSwarmGhost()
{
	FilthListType::iterator i_unit;
	FOR_EACH(unitList,i_unit){
		(*i_unit)->Kill();
	}
}

void terFilthSwarmGhost::SetFreeDestroy()
{
	FilthListType::iterator i_unit;
	FOR_EACH(unitList,i_unit){
		(*i_unit)->SetFreeDestroy();
	}
}

const float FILTH_GHOST_HEIGHT = 20.0f;
const float FILTH_GHOST_HEIGHT_DOWN = -50.0f;
const float FILTH_GHOST_ANGLE_Z = M_PI * 0.01f;
const float FILTH_GHOST_SPEED = 1.5f;
const float FILTH_GHOST_OBJECT_ANIMATION=0.9f;
const float FILTH_GHOST_BEGIN_ANIMATION=0.05f;
const float FILTH_GHOST_DESTROY_ANIMATION=-0.1f;
const float FILTH_GHOST_ATTACK_ANIMATION=0.05f;

void terFilthSwarmGhost::Quant()
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
		FilthListType::iterator it;
		FOR_EACH(unitList,it)
			(*it)->Attack(TargetPoint);
	}

	angle_z+=FILTH_GHOST_ANGLE_Z;
}

void terFilthSwarmGhost::SnakeMove()
{
	if(unitList.empty())
		return;
	FilthListType::iterator it;
	Vect3f v=position;
	float d=FILTH_GHOST_SPEED*4;

	if(!TargetPoint)
		FindTargetPoint();
	if(TargetPoint)
		v = TargetPoint->position();

	Vect3f n=v-position;
	float dist=n.norm();
	n.Normalize();

	if(!gen.InProcess())
		position+=n*FILTH_GHOST_SPEED;

	if(dist < d && TargetPoint)
	{
		bool is_attack=false;
		FOR_EACH(unitList,it)
		{
			terFilthGhost* p=*it;
			p->SetFilthAttackPosition(v);
			p->SetFilthAttack(true);

			is_attack=is_attack || p->IsAttack();
		}
		
		if(is_attack && !sound_attack.IsPlayed())
			sound_attack.Play();
	}else
	{
		FOR_EACH(unitList,it)
		{
			terFilthGhost* p=*it;
			p->SetFilthAttack(false);
		}
		if(sound_attack.IsPlayed())
			sound_attack.Stop();
	}

	int num=unitList.size();
	float r=terFilthGhostPrm.CreatureGenerationRadius*max(num,1);
	float a=angle_z;
	float da=(2*M_PI)/max(num,1);

	FOR_EACH(unitList,it)
	{
		terFilthGhost* p=*it;

		Vect3f pos;
		pos.x=position.x+r*cos(a);
		pos.y=position.y+r*sin(a);
		pos.z=0;
		pos=To3D(pos);
		pos.z+=FILTH_GHOST_HEIGHT;

		p->SetFilthTarget(pos);
		a+=da;
	}
	sound.SetPos(To3D(position));
	sound_attack.SetPos(To3D(position));
}

void terFilthSwarmGhost::DestroyLink()
{
	terFilthSwarm::DestroyLink();
	DestroyLinkList(unitList);
}

void terFilthSwarmGhost::GenerationProcess()
{
	terFilthGhost* p;
	Vect3f v;

	int num = gen.Quant();

	int prev_unit_num=unitList.size();
	int unit_num=prev_unit_num+num;
	float r=terFilthGhostPrm.CreatureGenerationRadius*max(unit_num,1);
	float a=angle_z;
	float da=(2*M_PI)/max(unit_num,1);
	a+=da*prev_unit_num;

	for(int i = 0;i < num;i++){
		if(SpotPoint && SpotPoint->terCheckFilthPoint(round(position.x),round(position.y))){
			p = safe_cast<terFilthGhost*>(player->buildUnit(GetUnitID()));

			Vect3f pos;
			pos.x=position.x+r*cos(a);
			pos.y=position.y+r*sin(a);
			pos.z=0;
			p->setPose(Se3f(QuatF::ID, pos), false);
			p->SetAttackPeriod(attack_period);
			p->Start();
			unitList.push_back(p);

			playDetectedSound();
			a+=da;
		}
	}
}

//---------------------------------------------------------

terFilthGhost::terFilthGhost(const UnitTemplate& data) : terFilthGeneric(data)
{
	target_position = Vect3f::ZERO;
	attack_position= Vect3f::ZERO;

	destruction = false;
	last_attack = false;
	request_attack=false;
	AttackAnimation.startPhase(0,FILTH_GHOST_ATTACK_ANIMATION,false);

	destroy_flag = false;
	laser= safe_cast<cLine3d*>(terScene->CreateLine3d(terTextureFreezeTrail));
	laser->SetAttr(ATTRUNKOBJ_ADDBLEND | ATTRUNKOBJ_IGNORE);
	laser->SetAnimation(32,0);

	ObjectAnimation.startPhase(0,FILTH_GHOST_OBJECT_ANIMATION,true);
	BeginAnimation.startPhase(0,FILTH_GHOST_BEGIN_ANIMATION,false);

	zeroPositionTimer_.start(1000);

	GhostHolePoint=NULL;
	last_deltaz=-100;
}

terFilthGhost::~terFilthGhost()
{
	laser->Release();
	if(GhostHolePoint)
	{
		delete GhostHolePoint;
		GhostHolePoint=NULL;
	}
}

void terFilthGhost::WayPointStart()
{
	terFilthGeneric::WayPointStart();
	avatar()->SetChain("main");
	DestroyAnimation.startPhase(1.0f,FILTH_GHOST_DESTROY_ANIMATION,false);
	BodyPoint->setFlyingHeight(FILTH_GHOST_HEIGHT_DOWN);
}

void terFilthGhost::setPose(const Se3f& poseIn, bool initPose)
{
	Se3f poseZ = poseIn;
	if(zeroPositionTimer_())
		poseZ.trans().z = -100;	
	terFilthGeneric::setPose(poseZ, initPose);
}

void terFilthGhost::SetFreeDestroy()
{
	if(destruction)
		return;
	destruction=true;
	damageMoleculaKill();
}

void terFilthGhost::WayPointController()
{
	terFilthGeneric::WayPointController();

	BodyPoint->way_points.clear();
	BodyPoint->setVelocityFactor(1);
	BodyPoint->setFlyingHeight(LinearInterpolate(FILTH_GHOST_HEIGHT_DOWN,FILTH_GHOST_HEIGHT,BeginAnimation.phase()));
	BodyPoint->setFlyingMode(true);

	if(clusterColliding()/* || BodyPoint->chaos_colliding*/)
	{
		FieldEffect();
		damageMoleculaKill();
	}

	if(alive() && !damageMolecula().isAlive()){
		if(!destroy_flag){
			destroy_flag = true;
			setCollisionGroup(collisionGroup() & ~(COLLISION_GROUP_ENEMY | COLLISION_GROUP_REAL));
			BodyPoint->way_points.push_back(target_position);
		}else{
			if(DestroyAnimation.isEnd()){
				soundEvent(SOUND_EVENT_EXPLOSION);
				Kill();
			}
		}
	}else{
		BodyPoint->way_points.push_back(target_position);
//		if(destruction)
//			Kill();
	}
}

void terFilthGhost::AvatarQuant()
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
}

void terFilthGhost::RequestAttackQuant()
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

void terFilthGhost::Collision(terUnitBase* p)
{
	if(destruction)
		return;
	if((isEnemy(p) && p->attr().isBuilding()) || p->attr().isLegionary()){
		p->setDamage(attr().unitDamage.splashDamage,this);
	}
}

void terFilthGhost::AvatarInterpolation()
{
	terUnitReal::AvatarInterpolation();
	if(IsAttack())
	{
		sVertexLine l[2];
		l[0].pos=avatar()->matrix().trans();
		l[1].pos=attack_position;
		l[0].color=l[1].color=sColor4c(255,255,255,255);
		float dist=l[0].pos.distance(l[1].pos);
		l[0].v=0;
		l[1].v=dist/128.0f;
		l[0].width=l[1].width=10.0f;

		laser->UpdateVertex(2, l);
		laser->ClearAttr(ATTRUNKOBJ_IGNORE);
	}else
		laser->SetAttr(ATTRUNKOBJ_IGNORE);
}

void terFilthGhost::Attack(terUnitBase* TargetPoint)
{
	if(IsAttack())
		TargetPoint->setDamage(damageData(),this);
}

void terFilthGhost::Quant()
{
	terFilthGeneric::Quant();

	Vect3f pos=To3D(position());
	float dz=position().z-pos.z;
	if(last_deltaz<0 && dz>=0)
	{
		if(!GhostHolePoint 
			&& false
			)
		{
			GhostHolePoint = new ghostToolzer(16);
			GhostHolePoint->start(round(position().x),round(position().y));
		}
	}
	last_deltaz=dz;

	if(GhostHolePoint)
	{
		if(!GhostHolePoint->quant(round(position().x),round(position().y)))
		{
			delete GhostHolePoint;
			GhostHolePoint = NULL;
		}
	}
}


void terFilthGhost::SetFilthAttack(bool attack_)
{
	int cluster=field_dispatcher->getIncludingCluster(position());
	int attack_cluster=field_dispatcher->getIncludingCluster(attack_position);

	if(cluster!=attack_cluster)
	{
		request_attack =false;
		return;
	}
	
	request_attack = attack_;
}

SaveUnitData* terFilthSwarmGhost::universalSave(SaveUnitData* baseData)
{
	SaveFilthSwarmGhost* data = castOrCreate<SaveFilthSwarmGhost>(baseData);
	terFilthSwarm::universalSave(data);
	data->angle_z=angle_z;
	gen.Save(data->generate);
	data->attack_period=attack_period;
	data->TargetPosition=TargetPosition;
	data->TargetCount=TargetCount;

	FilthListType::iterator it;
	FOR_EACH(unitList,it)
	if(*it)
	{
		data->unitList.push_back((*it)->universalSave(0));
	}

	return data;
}

void terFilthSwarmGhost::universalLoad(const SaveUnitData* baseData)
{
	terFilthSwarm::universalLoad(baseData);
	const SaveFilthSwarmGhost* data = safe_cast<const SaveFilthSwarmGhost*>(baseData);
	angle_z=data->angle_z;
	gen.Load(data->generate);
	attack_period=data->attack_period;
	TargetPosition=data->TargetPosition;
	TargetCount=data->TargetCount;

	SaveUnitDataList::const_iterator it;
	FOR_EACH(data->unitList,it)
	if(*it)
	{
		terFilthGhost* unit = safe_cast<terFilthGhost*>(player->buildUnit((*it)->attributeID));
		unitList.push_back(unit);
		unit->universalLoad(*it);
	}
}

SaveUnitData* terFilthGhost::universalSave(SaveUnitData* baseData)
{
	SaveFilthGhost* data = castOrCreate<SaveFilthGhost>(baseData);
	terFilthGeneric::universalSave(data);

	data->target_position=target_position;
	data->attack_position=attack_position;

	data->destruction=destruction;

	data->ObjectAnimation=ObjectAnimation.phase();
	data->DestroyAnimation=DestroyAnimation.phase();
	data->BeginAnimation=BeginAnimation.phase();

	data->last_attack=last_attack;
	data->request_attack=request_attack;
	data->AttackAnimation=AttackAnimation.phase();

	data->destroy_flag=destroy_flag;

	data->zeroPositionTimer=zeroPositionTimer_();
	data->last_deltaz=last_deltaz;

	return data;
}

void terFilthGhost::universalLoad(const SaveUnitData* baseData)
{
	const SaveFilthGhost* data = safe_cast<const SaveFilthGhost*>(baseData);

	target_position=data->target_position;
	attack_position=data->attack_position;

	destruction=data->destruction;

	ObjectAnimation.startPhase(data->ObjectAnimation,FILTH_GHOST_OBJECT_ANIMATION,true);
	DestroyAnimation.startPhase(data->DestroyAnimation,FILTH_GHOST_DESTROY_ANIMATION,false);
	BeginAnimation.startPhase(data->BeginAnimation,FILTH_GHOST_BEGIN_ANIMATION,false);

	last_attack=data->last_attack;
	request_attack=data->request_attack;
	AttackAnimation.startPhase(data->AttackAnimation,FILTH_GHOST_ATTACK_ANIMATION,false);

	destroy_flag=data->destroy_flag;

	zeroPositionTimer_.start(data->zeroPositionTimer);
	last_deltaz=data->last_deltaz;

	terFilthGeneric::universalLoad(data);
}
