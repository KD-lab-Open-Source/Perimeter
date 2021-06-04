#include "StdAfx.h"
#include "Runtime.h"

#include "Universe.h"
#include "Player.h"
#include "RealUnit.h"

#include "Config.h"
#include "ForceField.h"
#include "GenericFilth.h"
#include "Region.h"
#include "IronFrame.h"

#include "IronExplosion.h"
#include "PerimeterSound.h"
#include "FilthDragon.h"
#include "Filth.hi"

//--------------------------------------------
const float DragonDiePeriod=400;//ms
const float DragonMinSight=0.6f;
const float DragonMaxSight=0.8f;

terFilthSwarmDragon::terFilthSwarmDragon(terFilthSpot* spot, const Vect3f& pos,int attack_period_)
: terFilthSwarm(spot,pos)
{
	prm=&terFilthDragonPrm;
	HeadPoint=NULL;
	TargetPoint=NULL;
	TargetCount = 0;
	TargetPosition = Vect3f::ZERO;

	AttackCount = attack_period_;

	use_geo=SpotPoint->initialGeoprocess();
	BirthProcessPoint=new s_WaspBirthGeoAction(round(position.x),round(position.y), 16);
}

terFilthSwarmDragon::~terFilthSwarmDragon()
{
	if(HeadPoint)
	{
		HeadPoint->Kill();
	}

	delete BirthProcessPoint;
}

void terFilthSwarmDragon::SetFreeDestroy()
{
	if(HeadPoint)
		HeadPoint->SetFreeDestroy();
}

terUnitBase* terFilthSwarmDragon::GetNextTarget()
{
	if(TargetPoint)
	{
		UnitSet exclude;
		exclude.insert(TargetPoint);
		TargetPoint=NULL;
		FindTargetPoint(&exclude);
		if(!TargetPoint)
			FindTargetPoint();
	}else
	{
		FindTargetPoint();
	}
	return TargetPoint;
}

void terFilthSwarmDragon::Quant()
{

	if(BirthProcessPoint)
	{
		if(!use_geo || !BirthProcessPoint->quant())
		{
			delete BirthProcessPoint;
			BirthProcessPoint=NULL;
			GenerationProcess();
		}
		return;
	}

	Vect3f v1,v;
	Vect3f nv;
	float dist,d;

	terFilthSwarm::Quant();

	if(!HeadPoint)
		Alive = 0;
	else
	{
		AttackCount--;
		if(AttackCount < 0)
		{
			HeadPoint->MustDie();
		}else
		{
			if(!TargetPoint)
			{
				FindTargetPoint();
			}

			if(TargetPoint){
				v1 = v = TargetPoint->position();
				d = TargetPoint->radius() * 2.5f;
			}else{
				d = 1.0f;
				TargetCount--;
				if(TargetCount < 0){
					TargetPosition.x = vMap.H_SIZE / 4 + terLogicRND(vMap.H_SIZE / 2);
					TargetPosition.y = vMap.V_SIZE / 4 + terLogicRND(vMap.V_SIZE / 2);
					TargetPosition.z = (float)(vMap.GetAlt(vMap.XCYCL(round(TargetPosition.x)),vMap.YCYCL(round(TargetPosition.y))) >> VX_FRACTION);
					TargetCount = 100;
				}
				v1 = v = TargetPosition;
			}

			v1 -= HeadPoint->position();
			dist = v1.norm();
			if(dist < d){
				HeadPoint->SetFilthTarget(v);
			}else{
				HeadPoint->SetFilthTarget(v);
			}
		}
	}
}

void terFilthSwarmDragon::DestroyLink()
{
	terFilthSwarm::DestroyLink();
	if(TargetPoint && !(TargetPoint->alive()))
		TargetPoint = NULL;

	if(HeadPoint && !(HeadPoint->alive()))
		HeadPoint = NULL;
}


void terFilthSwarmDragon::GenerationProcess()
{
	if(!SpotPoint)
	{
		Alive=0;
		return;
	}

	if(HeadPoint)
		return;

	Vect3f v,dv;
	float r,dr;

	position=To3D(position);
	position.z+=150;

	v = position;
	float angle = terLogicRNDfrand() * M_PI * 2.0f;
	dv.x = 0;//cosf(angle) * prm->CreatureGenerationRadius;
	dv.y = 0;//sinf(angle) * prm->CreatureGenerationRadius;
	dv.z = -35;

	HeadPoint = safe_cast<terFilthDragonHead*>(player->buildUnit(prm->id_head));
	Se3f pose_tail,pose_head;
	pose_tail.rot().set(M_PI*0.5f,Vect3f(1,0,0));

	pose_head.rot().mult(QuatF::ID,pose_tail.rot());

	pose_head.trans()=position;
	HeadPoint->setPose(pose_head,true);
	HeadPoint->SetSwarm(this);

	terFilthDragon* prev = NULL;

	int num = 2 * prm->BrushNum / 3;

	r = 1.0f;
	dr = (0.1f - r) / (float)(prm->BrushNum);

	float phase = 0;
	float dphase = 5.0f / (float)(prm->BrushNum);

	for(int i = 0;i < num;i++){
		v += dv;
		terFilthDragon* p = safe_cast<terFilthDragon*>(SpotPoint->Player->buildUnit(prm->id_body));
		p->SetSwarm(this);
		p->SetScale(r);
		pose_tail.trans()=v;
		p->setPose(pose_tail,true);
		p->setPhase(phase);

		if(prev)
			prev->SetNextPoint(p);
		else
			HeadPoint->NextPoint=p;
		prev = p;

		r += dr;
		phase = fmod(phase + dphase,1.0f);
	}

	for(int i = 0;i < prm->BrushNum - num;i++){
		v += dv;
		terFilthDragon* p = safe_cast<terFilthDragon*>(SpotPoint->Player->buildUnit(prm->id_tail));
		p->SetSwarm(this);
		p->SetScale(r);
		p->setPosition(v);
		p->setPhase(phase);
		prev->SetNextPoint(p);
		prev = p;

		r += dr;
		phase = fmod(phase + dphase,1.0f);
	}
	prev->SetNextPoint(NULL);

	HeadPoint->Start();
	HeadPoint->LinkingBody();
	terFilthDragon* p = HeadPoint->NextPoint;
	while(p){
		p->Start();
		p->LinkingBody();
		p->DockNow();
		p = p->NextPoint;
	}

	playDetectedSound();
}

void terFilthSwarmDragon::FindTargetPoint(UnitSet* exclude)
{
	distance_direction=HeadPoint->getLastDirection2();
	terFilthSwarm::FindTargetPoint(exclude);
}

terUnitBase* terFilthSwarmDragon::CalcDistance(terUnitBase* unit,float& dist,const Vect2f& direction)
{
	if(attack_width>0 && SpotPoint)
	{
		Vect3f spot_pos=SpotPoint->position();
		Vect2f p(unit->position().x-spot_pos.x,unit->position().y-spot_pos.y);
		float t=p.x*direction.x+p.y*direction.y;
		if(t<0)
			return NULL;
		float n=fabsf(p.x*direction.y-p.y*direction.x);
		if(n>attack_width)
			return NULL;
	}

	if(!(unit->unitClass() & attribute->AttackClass) )
		return NULL;

	Vect3f delta=unit->position()-position;
	delta.z=0;

	float d = /*-0.75f*delta.dot(distance_direction)+*/delta.norm();
	if(d < dist)
	{
		if(SpotPoint && SpotPoint->IsAttackOtherSwarm(unit,this))
			return NULL;

		dist = d;
		return unit;
	}

	return NULL;
}

//-----------------------------------------------------------------

terFilthDragon::terFilthDragon(const UnitTemplate& data) : terFilthGeneric(data)
{
	NextPoint = NULL;

	ObjectAnimation.startPhase(0,0,1);

	AngleDockingEnable = true;
	is_die=false;
	is_die_free=false;
	crater=NULL;
	crater_run=false;
	scale=1.0f;
	swarm=NULL;
	timerDieFree.startPeriod(1,DragonDiePeriod,false);
	timerDieFree.setEndPhase(1,false);
}

terFilthDragon::~terFilthDragon()
{
	delete crater;
}
void terFilthDragon::SetSwarm(terFilthSwarmDragon* swarm_)
{
	swarm=swarm_;
	DockPhase.startPhase(swarm->prm->PositionFactor,0,0);
	DockAnglePhase.startPhase(swarm->prm->AngleFactor,0,0);
}

void terFilthDragon::DockNow()
{
	DockPoint->avatar()->updateLogicObject();
	BodyPoint->docking(DockSlot->position(),1,1);
	setPose(BodyPoint->pose(), false);
	avatar()->setPose(BodyPoint->pose());
}

void terFilthDragon::WayPointStart()
{
	terFilthGeneric::WayPointStart();
	avatar()->SetChain("main");
}

void terFilthDragon::LinkingBody()
{
	if(NextPoint){
		terDockingSlot* slot = requestDockingSlot();
		if(slot){
			UpdateLogicPoint();
			LogicObjectPoint->SetChannel("main",ObjectAnimation.phase());
			LogicObjectPoint->Update();
			NextPoint->SetDockingPoint(this,slot,DOCK_MODE_LEAVE);
		}
	}
}

void terFilthDragonHead::LinkingBody()
{
	if(NextPoint){
		terDockingSlot* slot = requestDockingSlot();
		if(slot){
			UpdateLogicPoint();
			LogicObjectPoint->SetChannel("main",ObjectAnimation.phase());
			LogicObjectPoint->Update();
			NextPoint->SetDockingPoint(this,slot,DOCK_MODE_LEAVE);
		}
	}
}

void terFilthDragon::MoveQuant()
{
	terUnitGeneric::MoveQuant();
	setPose(BodyPoint->pose(), false);
//	terFilthGeneric::MoveQuant();

//	if(!DockPoint)
//		Kill();

	ObjectAnimation.Quant();

	UpdateLogicPoint();
	LogicObjectPoint->SetChannel("main",ObjectAnimation.phase());
	LogicObjectPoint->Update();
}

void terFilthDragon::RefreshAttribute()
{
	terFilthGeneric::RefreshAttribute();

	DockPhase.startPhase(swarm->prm->PositionFactor,0,0);
	DockAnglePhase.startPhase(swarm->prm->AngleFactor,0,0);
}

void terFilthDragon::DestroyLink()
{
	terFilthGeneric::DestroyLink();
	if(NextPoint && !(NextPoint->alive()))
		NextPoint = NULL;
}

void terFilthDragon::AvatarQuant()
{
	terFilthGeneric::AvatarQuant();

	avatar()->Show();
	avatar()->setPose(BodyPoint->pose());
	realAvatar()->setSight(GetSight());
	realAvatar()->setPhase(ObjectAnimation.phase());
}

void terFilthDragon::Collision(terUnitBase* p)
{
	if(is_die)
		return;

	if(GetSight()>DragonMaxSight)
	if(isEnemy(p) && (swarm && swarm->IsAttackable(p)))
	{
		if(p->unitClass()&UNIT_CLASS_FRAME)
			p->setDamage(attr().unitDamage.splashDamage,this);
		else
			p->setDamage(damageData(),this);
	}
}

void terFilthDragon::Quant()
{
	terFilthGeneric::Quant();
	if(HotCount && NextPoint)
		NextPoint->SetHotCount(HotCount);

	if(is_die_free)
	{
	}else
	if(!crater_run && is_die)
	{
		Vect3f pos=To3D(position());
		float dz=position().z-pos.z;
		if(dz<0)
		{
			xassert(!crater);
			if(!terCheckFilthChaos(position()))
			{
				crater=new craterToolzerDestroyZP(scale*0.3f);
				crater->start(round(pos.x),round(pos.y));
			}
			crater_run=true;
		}
	}

	if(crater)
	{
		if(!crater->quant())
		{
			delete crater;
			crater=NULL;
		}
	}
	timerDieFree.Quant();
}

void terFilthDragon::SetScale(float r)
{
	scale=r;
	setRealModel(0, r);
}

void terFilthDragon::MustDie()
{
	is_die=true;
}

void terFilthDragon::MustDieFree(bool must)
{
	is_die_free=must;

	if(is_die_free)
		timerDieFree.setEndPhase(0,false);

	if(NextPoint)
	{
		if(is_die_free)
		{
			if(GetSight()<DragonMinSight)
				NextPoint->MustDieFree(true);
		}else
		{
			NextPoint->MustDieFree(false);
			if(NextPoint->GetSight()>DragonMaxSight)
				timerDieFree.setEndPhase(1,false);
		}
	}else
	{
		if(!is_die_free)
			timerDieFree.setEndPhase(1,false);
	}
}

void terFilthDragonHead::SetFreeDestroy()
{
	MustDie();
}

//-----------------------------------------

terFilthDragonHead::terFilthDragonHead(const UnitTemplate& data) : terFilthSpline(data)
{
	NextPoint=NULL;

	attack=false;
	Height = 300;
	height_flag=true;
	TargetPosition = Vect3f::ZERO;
	k_acceleration=1e-2f;

	swarm=NULL;
	is_die=false;
	is_die_free=false;
	fire_node=NULL;

	timerDieFree.startPeriod(1,DragonDiePeriod,false);
	timerDieFree.setEndPhase(1,false);
}

void terFilthDragonHead::Kill()
{
	soundEvent(SOUND_EVENT_EXPLOSION);
	terFilthSpline::Kill();
	terFilthDragon* p = NextPoint;
	while(p)
	{
		p->Kill();
		p=p->NextPoint;
	}
}

terFilthDragonHead::~terFilthDragonHead()
{
}

void terFilthDragonHead::SetSwarm(terFilthSwarmDragon* swarm_)
{
	swarm=swarm_;
	ObjectAnimation.startPeriod(0,swarm->prm->AnimationHeadPeriod,1);

	sound.Init("Filth_Move_Dragon");
	sound.SetPos(To3D(position()));
	sound.Play();

	attack_sound.Init("Filth_Attack_Dragon");
	sound.SetPos(To3D(position()));
	sound.Play();
}

void terFilthDragonHead::Collision(terUnitBase* p)
{
	if(is_die)
		return;
}

void terFilthDragonHead::RefreshAttribute()
{
	terFilthSpline::RefreshAttribute();
	DockPhase.startPhase(swarm->prm->PositionFactor,0,0);
	DockAnglePhase.startPhase(swarm->prm->AngleFactor,0,0);
	ObjectAnimation.setPeriod(swarm->prm->AnimationHeadPeriod);
}

void terFilthDragonHead::WayPointController()
{
	terFilthGeneric::WayPointController();

	BodyPoint->way_points.clear();
	BodyPoint->setFlyingHeight(Height);
	BodyPoint->setFlyingMode(1);
}

void terFilthDragonHead::MoveQuant()
{
	Se3f pos=quantPosition(delta_time);
	Se3f set_pos;
	float t=0.1f;
	set_pos.trans().interpolate(pose().trans(), pos.trans(), t);
	set_pos.rot().slerp(pose().rot(), pos.rot(), t);

	terFilthGeneric::setPose(set_pos, false);

	if(!damageMolecula().isAlive())
		MustDie();
	sound.SetPos(To3D(position()));
}

void terFilthDragonHead::CalcHeight()
{
	if(height_flag){
		Height = swarm->prm->MaxHeight;
	}else{
		Height = swarm->prm->MinHeight;
	}
	height_flag=!height_flag;
}

void terFilthDragonHead::checkField()
{
}

void terFilthDragonHead::addWayPoint()
{
	if(is_die)
	{
		Kill();
		return;
	}

	bool set_height=false;
	const float move_len=50;
	float len=move_len;
	if(way_points.empty())
	{
		way_points.push_back(position());
		way_points.push_back(position());
		len=0;
	}else
	{
		CalcHeight();
	}

	Vect3f prev_pos=way_points.back();
	Vect3f n=TargetPosition-prev_pos;
	n.z=0;
	float dist=n.norm();

	if(dist<10)
	{
		n=getLastDirection2();

		terUnitBase* target=swarm->GetNextTarget();
		if(target)
		{
			//Лететь по тому-же направлению и поменять цель
			TargetPosition=target->position();

			Vect3f last_pos=prev_pos;
			float aprev=atan2(n.y,n.x);
			for(int i=0;i<5;i++)
			{
				Vect3f p=TargetPosition-last_pos;
				p.z=0;
				p.Normalize();
				float da=M_PI/4;
				float agood=atan2(p.y,p.x);
				float a1=aprev+da,a2=aprev-da;

				Vect3f n0,n1,n2;
				n0.set(cos(aprev),sin(aprev),0);
				n1.set(cos(a1),sin(a1),0);
				n2.set(cos(a2),sin(a2),0);

				float f0=p.dot(n0);
				float f1=p.dot(n1);
				float f2=p.dot(n2);
				float fmax=f0,fi=0;
				if(f1>fmax)
				{
					fmax=f1;
					fi=1;
				}
				if(f2>fmax)
				{
					fmax=f2;
					fi=2;
				}
				if(fi==0)
					break;

				float a=(fi==1)?a1:a2;
				n=(fi==1)?n1:n2;
				Vect3f pos=To3D(last_pos+n*len);
				CalcHeight();
				pos.z+=Height;
				way_points.push_back(pos);
				aprev=a;
				last_pos=pos;
			}

			return;
		}
	}else
	{
		if(dist<len)
		{
			terUnitBase* target=swarm->GetTarget();
			if(target)
			{
				RigidBody* rb=target->GetRigidBodyPoint();
				Vect3f ps=rb->matrix()*rb->boxMax();
				Height=ps.z;
				set_height=true;
			}
		}

		len=min(dist,len);
		n/=dist;
	}

	Vect3f pos=To3D(prev_pos+n*len);
	if(set_height)
		pos.z=Height;
	else
		pos.z+=Height;

	way_points.push_back(pos);
}

void terFilthDragonHead::MustDie()
{
	if(is_die)
		return;
	is_die=true;

	if(way_points.empty())
		return;

	terFilthDragon* p = NextPoint;
	while(p)
	{
		p->MustDie();
		p=p->NextPoint;
	}

	const float move_len=50;
	const float dz=25;

	Vect3f n=getLastDirection2();
	Vect3f prev_pos=way_points.back();
	Vect3f pos=To3D(prev_pos+n*move_len);
	pos.z-=dz;
	way_points.push_back(pos);
	pos=To3D(pos+n*move_len);
	pos.z-=2*dz;
	way_points.push_back(pos);
	pos=To3D(pos+n*move_len);
	pos.z-=3*dz;
	way_points.push_back(pos);
	pos.z-=dz;
	way_points.push_back(pos);
	pos.z-=dz;
	way_points.push_back(pos);
}

void terFilthDragonHead::MustDieFree(bool must)
{
	is_die_free=must;

	if(is_die_free)
		timerDieFree.setEndPhase(0,false);

	if(NextPoint)
	{
		if(is_die_free)
		{
			if(GetSight()<DragonMinSight)
				NextPoint->MustDieFree(true);
		}else
		{
			NextPoint->MustDieFree(false);
			if(NextPoint->GetSight()>DragonMaxSight)
				timerDieFree.setEndPhase(1,false);
		}
	}else
	{
		if(!is_die_free)
			timerDieFree.setEndPhase(1,false);
	}
}

bool terFilthDragonHead::IsEndInvisibly()
{
	float sight=GetSight();
	for(terFilthDragon* p=NextPoint;p;p=p->NextPoint)
	{
		if(!p->NextPoint)
		{
			sight=p->GetSight();
		}
	}

	return sight>0;
}

void terFilthDragonHead::Quant()
{
	terFilthSpline::Quant();
	if(HotCount && NextPoint)
		NextPoint->SetHotCount(HotCount);

	if(!fire_node)
	{
		fire_node=avatar()->GetModelPoint()->FindObject("group fire");
		fire_node->SetChannel("fire off",true);
		xassert(fire_node);
	}

	realAvatar()->setSight(GetSight());
	bool fire=false;
	if(is_die_free)
	{
		if(!IsEndInvisibly())
		{
			Kill();
			for(terFilthDragon* p=NextPoint;p;p=p->NextPoint)
				p->Kill();
		}
	}else
	if(!is_die)
	{
		float dist=position().distance(TargetPosition);
		if(GetSight()>DragonMaxSight)
		if(dist<swarm->prm->fire_distance && dist>10)
		{
			Vect3f n=(TargetPosition-position())/dist;
			Mat3f rot(pose().rot());
			Vect3f n1=rot*Vect3f(0,1,0);
			fire=n.dot(n1)>0.8f;
			if(fire && swarm->GetTarget())
				swarm->GetTarget()->setDamage(damageData(),this);
		}
	}
	fire_node->SetChannel(fire?"fire on":"fire off",true);

	attack_sound.SetPos(position());
	if(fire && !attack_sound.IsPlayed())
		attack_sound.Play(false);

	timerDieFree.Quant();

	int cluster=field_dispatcher->getIncludingCluster(position());
	MustDieFree(cluster || !damageMolecula().isAlive());
}

//-----------------------------------------

terFilthDragonBody::terFilthDragonBody(const UnitTemplate& data) : terFilthDragon(data)
{
}

void terFilthDragon::setPhase(float phase)
{
	ObjectAnimation.startPeriod(phase, swarm->prm->AnimationBodyPeriod, 1);
}

void terFilthDragonBody::RefreshAttribute()
{
	terFilthDragon::RefreshAttribute();
	ObjectAnimation.setPeriod(swarm->prm->AnimationBodyPeriod);
}


SaveUnitData* terFilthSwarmDragon::universalSave(SaveUnitData* baseData)
{
	SaveFilthSwarmDragon* data = castOrCreate<SaveFilthSwarmDragon>(baseData);
	terFilthSwarm::universalSave(data);
	data->use_geo=use_geo;
	data->AttackCount=AttackCount;
	data->TargetPosition=TargetPosition;
	data->TargetCount=TargetCount;
	data->distance_direction=distance_direction;

	if(HeadPoint)
		data->HeadPoint=HeadPoint->universalSave(0);

	return data;
}

void terFilthSwarmDragon::universalLoad(const SaveUnitData* baseData)
{
	terFilthSwarm::universalLoad(baseData);
	const SaveFilthSwarmDragon* data = safe_cast<const SaveFilthSwarmDragon*>(baseData);
	use_geo=data->use_geo;
	AttackCount=data->AttackCount;
	TargetPosition=data->TargetPosition;
	TargetCount=data->TargetCount;
	distance_direction=data->distance_direction;

	if(data->HeadPoint)
	{
		HeadPoint = safe_cast<terFilthDragonHead*>(player->buildUnit(data->HeadPoint->attributeID));
		HeadPoint->SetSwarm(this);
		HeadPoint->universalLoad(data->HeadPoint);

		HeadPoint->Start();
		HeadPoint->LinkingBody();
		terFilthDragon* p = HeadPoint->NextPoint;
		while(p){
			p->Start();
			p->LinkingBody();
			p->DockNow();
			p = p->NextPoint;
		}
	}
}

SaveUnitData* terFilthDragon::universalSave(SaveUnitData* baseData)
{
	SaveFilthDragon* data = castOrCreate<SaveFilthDragon>(baseData);
	terFilthGeneric::universalSave(data);
	data->ObjectAnimationPhase=ObjectAnimation.phase();
	data->timerDieFreePhase=timerDieFree.phase();
	data->scale=scale;
	data->crater_run=crater_run;
	data->is_die=is_die;
	data->is_die_free=is_die_free;

	if(NextPoint)
		data->NextPoint=NextPoint->universalSave(0);
	return data;
}

void terFilthDragon::universalLoad(const SaveUnitData* baseData)
{
	terFilthGeneric::universalLoad(baseData);
	const SaveFilthDragon* data = safe_cast<const SaveFilthDragon*>(baseData);
	scale=data->scale;
	crater_run=data->crater_run;
	is_die=data->is_die;
	is_die_free=data->is_die_free;

	timerDieFree.startPeriod(data->timerDieFreePhase,DragonDiePeriod,false);
	if(is_die_free)
		timerDieFree.setEndPhase(0,false);
	else
		timerDieFree.setEndPhase(1,false);
	SetScale(scale);
	setPhase(data->ObjectAnimationPhase);

	if(data->NextPoint)
	{
		NextPoint = safe_cast<terFilthDragon*>(swarm->player->buildUnit(data->NextPoint->attributeID));
		NextPoint->SetSwarm(swarm);
		NextPoint->universalLoad(data->NextPoint);
	}
}

SaveUnitData* terFilthDragonHead::universalSave(SaveUnitData* baseData)
{
	SaveFilthDragonHead* data = castOrCreate<SaveFilthDragonHead>(baseData);
	terFilthSpline::universalSave(data);

	data->ObjectAnimationPhase=ObjectAnimation.phase();
	data->timerDieFreePhase=timerDieFree.phase();
	data->attack=attack;
	data->Height=Height;
	data->height_flag=height_flag;
	data->TargetPosition=TargetPosition;
	data->is_die=is_die;
	data->is_die_free=is_die_free;

	if(NextPoint)
		data->NextPoint=NextPoint->universalSave(0);
	return data;
}

void terFilthDragonHead::universalLoad(const SaveUnitData* baseData)
{
	terFilthSpline::universalLoad(baseData);
	const SaveFilthDragonHead* data = safe_cast<const SaveFilthDragonHead*>(baseData);

	ObjectAnimation.startPeriod(data->ObjectAnimationPhase,swarm->prm->AnimationHeadPeriod,1);
	attack=data->attack;
	Height=data->Height;
	height_flag=data->height_flag;
	TargetPosition=data->TargetPosition;
	is_die=data->is_die;
	is_die_free=data->is_die_free;

	timerDieFree.startPeriod(data->timerDieFreePhase,DragonDiePeriod,false);
	if(is_die_free)
		timerDieFree.setEndPhase(0,false);
	else
		timerDieFree.setEndPhase(1,false);

	if(data->NextPoint)
	{
		NextPoint = safe_cast<terFilthDragon*>(swarm->player->buildUnit(data->NextPoint->attributeID));
		NextPoint->SetSwarm(swarm);
		NextPoint->universalLoad(data->NextPoint);
	}
}
