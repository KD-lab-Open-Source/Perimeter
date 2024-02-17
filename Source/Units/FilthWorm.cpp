#include "StdAfx.h"

#include "Universe.h"

#include "Config.h"
#include "ForceField.h"
#include "GenericFilth.h"
#include "Region.h"
#include "IronFrame.h"
#include "IronExplosion.h"
#include "FilthWorm.h"
#include "filth.hi"

#include "GameShell.h"

#include "UniverseInterface.h"

const int WORM_TIME_CHANGE_ANGLE=50;
const int WORM_DTIME_CHANGE_ANGLE=25;
const float WORM_DANGLE=XM_PI*0.3f;

terFilthSwarmWorm::terFilthSwarmWorm(terFilthSpot* spot,const Vect3f& pos,int attack_period_)
: terFilthSwarm(spot,pos)
{
	attack_period=attack_period_;
	pWorm=new CGeoWorm(xm::round(position.x) - WSXSY05, xm::round(position.y) - WSXSY05);

	speed=2.0f;
	angle=terLogicRNDfrand()*2*XM_PI;
	change_angle=WORM_TIME_CHANGE_ANGLE;
	pWormModel=NULL;
	first=true;
	first_create=true;
}

terFilthSwarmWorm::~terFilthSwarmWorm()
{
	delete pWorm;
}

terUnitAttributeID terFilthSwarmWorm::GetUnitID()
{
	switch(SpotPoint->GetFilthParamID())
	{
    default:
        xassert(0);
        [[fallthrough]];
	case FILTH_SPOT_ID_WORM:
		return UNIT_ATTRIBUTE_FILTH_WORM;
	case FILTH_SPOT_ID_A_WORM:
		return UNIT_ATTRIBUTE_FILTH_A_WORM;
	}
}

void terFilthSwarmWorm::SetFreeDestroy()
{
	Alive = 0;
	delete pWorm;
	pWorm=NULL;
	if(pWormModel)
	{
		pWormModel->soundEvent(SOUND_EVENT_EXPLOSION);
		pWormModel->Kill();
	}
}

void terFilthSwarmWorm::Quant()
{
	if(!Alive)
		return;

	if(first && false)
	{
		std::list<terUnitBase*> target_list;
		FindComplexTarget(target_list,5,NULL);
		if(!target_list.empty())
		{
			int cur=terLogicRND(target_list.size());

			std::list<terUnitBase*>::iterator it=target_list.begin();
			for(int i=0;i<cur;i++,it++);

			TargetPoint=*it;
		}
		first=false;
	}

	if(!pWormModel)
	{
		Vect3f pos=To3D(position);
		pWormModel=safe_cast<terFilthWorm*>(player->buildUnit(GetUnitID()));
		pWormModel->setPose(Se3f(QuatF::ID, pos), false);
		pWormModel->Start();
	}

	attack_period--;
	if(attack_period<=0 || isMoveToChaos() || !pWormModel)
	{
		SetFreeDestroy();
		return;
	}

	if(!TargetPoint)
	{
		UnitSet exclude;
		std::list<terUnitBase*>::iterator it;
		FOR_EACH(exclude_list,it)
			exclude.insert(*it);

		FindTargetPoint(&exclude);

		if(TargetPoint)
		{
			const int max_in_exclude_list=6;
			exclude_list.push_back(TargetPoint);
			if(exclude_list.size()>max_in_exclude_list)
				exclude_list.pop_front();

		}else
			exclude_list.clear();
	}

	Move();

	if(TargetPoint && !pWormModel->isAttack())
	{
		Vect3f delta=TargetPoint->position()-position;
		delta.z=0;
		float dist=delta.norm();
		if(dist<TargetPoint->radius())
		{
			pWormModel->StartAttack();
			TargetPoint=NULL;
		}
	}

	pWormModel->ChangePos(position);

	terFilthSwarm::Quant();
}

void terFilthSwarmWorm::DestroyLink()
{
	terFilthSwarm::DestroyLink();
	if(pWormModel && !pWormModel->alive())
	{
//		molecula=pWormModel->damageMolecula();
		first_create=false;

		if(!pWormModel->damageMolecula().isAlive())
			Alive = 0;
		pWormModel=NULL;
		TargetPoint=NULL;
	}
}

void terFilthSwarmWorm::Move()
{
	if(pWormModel->isAttack())return;
	change_angle--;
	if(change_angle<=0)
	{
		angle+=terLogicRNDfrnd()*WORM_DANGLE;
		change_angle= xm::round(WORM_TIME_CHANGE_ANGLE + terLogicRNDfrnd() * WORM_DTIME_CHANGE_ANGLE);
	}

	if(TargetPoint)
	{
		Vect3f delta=TargetPoint->position()-position;
		float a=xm::atan2(delta.y,delta.x);

		a=uncycle(a,angle,2*XM_PI);

		Vect2f p(xm::cos(a), xm::sin(a)),p1(delta.x, delta.y);
		p1.normalize(1);
		float f=p.dot(p1);

		float t=0.1f;
		angle=a*t+angle*(1-t);
	}else
		angle=cycle(angle,2*XM_PI);

	Vect2f dx(xm::cos(angle) * speed, xm::sin(angle) * speed);

	position.x+=dx.x;
	position.y+=dx.y;

	position.x=clamp(position.x,0,vMap.H_SIZE);
	position.y=clamp(position.y,0,vMap.V_SIZE);
	pWorm->step(xm::round(position.x) - WSXSY05, xm::round(position.y) - WSXSY05, angle);
}

bool terFilthSwarmWorm::isMoveToChaos()
{
	float len=WSXSY05;
	Vect2f dx(xm::cos(angle) * len, xm::sin(angle) * len);

	Vect3f pos(position.x+dx.x,position.y+dx.y,0);
	pos=To3D(pos);
	if(show_filth_debug)
		show_vector(pos,2,GREEN);

	return terCheckFilthChaos(xm::round(pos.x), xm::round(pos.y));
}

//////////////////////////////////////////////////////////
terFilthWorm::terFilthWorm(const UnitTemplate& data)
:terFilthGeneric(data)
{
	end_tail=NULL;
	pWormOut=NULL;
	is_death=false;

}

terFilthWorm::~terFilthWorm()
{
	delete pWormOut;
}

void terFilthWorm::Start()
{
	terFilthGeneric::Start();

	bool b=sound.Init("Filth_Move_Worm");
	sound.SetPos(To3D(position()));

	b=attack_sound.Init("Filth_Attack_Worm");
	attack_sound.SetPos(To3D(position()));

	setAttack(false);

	end_tail=LogicObjectPoint->FindObject("end");
	xassert(end_tail);
	realAvatar()->setChain(CHAIN_STOP);
	CalcLogic();
}

void terFilthWorm::StartAttack()
{
	setAttack(true);

	pWormOut=new CWormOut(xm::round(position().x), xm::round(position().y));
	realAvatar()->setChain(terLogicRND(2)?CHAIN_BUILD1:CHAIN_BUILD2);

	Se3f pos=pose();
	pos.rot().set(XM_PI*terLogicRNDfrnd(),Vect3f(0,0,1));
	setPose(pos,false);
}

void terFilthWorm::AvatarQuant()
{
	terFilthGeneric::AvatarQuant();
	avatar()->setPose(pose());
}

void terFilthWorm::MoveQuant()
{
}

struct terSphericalCollisionOperatorWorm
{
	float radius;
	Vect3f position;
 	terFilthWorm* ObjectPoint;

	terSphericalCollisionOperatorWorm(terFilthWorm* p,float radius_)
	{
		ObjectPoint = p;
		position = ObjectPoint->GetEndPos();
		radius = radius_;
	}

	void operator()(terUnitBase* p)
	{
		if(p->alive() )
		{
			if(position.distance2(p->position()) < sqr(radius + p->radius()))
				ObjectPoint->EndCollision(p);
		}
	}
};

void terFilthWorm::ChangePos(Vect3f position)
{

	if(is_attack)
	{
		realAvatar()->setSight(1);
	}else
	{
		Se3f pos=pose();
		pos.trans()=To3D(position);
		setPose(pos,false);
		realAvatar()->setSight(0);
	}
}

void terFilthWorm::Quant()
{
	if(pWormOut)
	{
		if(!pWormOut->quant())
		{
			delete pWormOut;
			pWormOut=NULL;
		}
	}

	{
		Se3f pos=pose();
		pos.trans()=To3D(pos.trans());
		setPose(pos,false);
	}

	if(!damageMolecula().isAlive() && !is_death)
	{
		StartDeath();
	}

	CalcLogic();
	if(realAvatar()->isEnd())
	{
		if(is_death)
		{
			soundEvent(SOUND_EVENT_EXPLOSION);
			Kill();
		}

		if(is_attack)
		{
			setAttack(false);
			realAvatar()->setChain(CHAIN_STOP);
		}
	}

	if(show_filth_debug)
	{
		Vect3f pos=end_tail->GetGlobalPosition().trans();
		show_vector(pos,2,GREEN);
	}

	terFilthGeneric::Quant();

	{
		int end_radius=5;
		terSphericalCollisionOperatorWorm op(this,end_radius);
		Vect3f pos=GetEndPos();
		universe()->UnitGrid.Scan(xm::round(pos.x), xm::round(pos.y), end_radius, op);
	}

	sound.SetPos(To3D(position()));
	attack_sound.SetPos(To3D(position()));
}

void terFilthWorm::Collision(terUnitBase* p)
{
}

void terFilthWorm::CalcLogic()
{
	UpdateLogicPoint();
	LogicObjectPoint->SetChannel("main",realAvatar()->phase());
	LogicObjectPoint->Update();
}

void terFilthWorm::EndCollision(terUnitBase* TargetPoint)
{
	if(isEnemy(TargetPoint))
		TargetPoint->setDamage(damageData(),this);
}

void terFilthWorm::setAttack(bool a)
{
	is_attack=a;
	setUnitClass(is_attack?UNIT_CLASS_GROUND_FILTH:UNIT_CLASS_UNDERGROUND_FILTH);

	if(is_attack)
	{
		sound.Stop();
		attack_sound.Play();
	}else
	{
		sound.Play();
		attack_sound.Stop();
	}
}

void terFilthWorm::SetFreeDestroy()
{
	sound.Stop();
	attack_sound.Stop();
	if(is_attack)
		StartDeath();
	else
	{
		soundEvent(SOUND_EVENT_EXPLOSION);
		Kill();
	}
}

void terFilthWorm::StartDeath()
{
	is_death=true;
	cObjectNodeRoot* parent_node=avatar()->GetModelPoint();
	cObjectNode* node=parent_node->FindObject("group death");
	if(node)
		node->SetChannel("death",true);
}


SaveUnitData* terFilthSwarmWorm::universalSave(SaveUnitData* baseData)
{
	SaveFilthSwarmWorm* data = castOrCreate<SaveFilthSwarmWorm>(baseData);
	terFilthSwarm::universalSave(data);

	data->attack_period=attack_period;
	data->change_angle=change_angle;
	data->angle=angle;
	data->speed=speed;
	data->first=first;
	data->first_create=first_create;

	if(pWormModel)
		data->pWormModel=pWormModel->universalSave(0);

	return data;
}

void terFilthSwarmWorm::universalLoad(SaveUnitData* baseData)
{
	terFilthSwarm::universalLoad(baseData);
	SaveFilthSwarmWorm* data = safe_cast<SaveFilthSwarmWorm*>(baseData);

	attack_period=data->attack_period;
	change_angle=data->change_angle;
	angle=data->angle;
	speed=data->speed;
	first=data->first;
	first_create=data->first_create;

	if(data->pWormModel)
	{
		pWormModel = safe_cast<terFilthWorm*>(player->loadUnit(data->pWormModel, false));
        pWormModel->universalLoad(data->pWormModel);
	}
}

SaveUnitData* terFilthWorm::universalSave(SaveUnitData* baseData)
{
	SaveFilthVorm* data = castOrCreate<SaveFilthVorm>(baseData);
	terFilthGeneric::universalSave(data);

	return data;
}

void terFilthWorm::universalLoad(SaveUnitData* baseData)
{
	terFilthGeneric::universalLoad(baseData);
	SaveFilthVorm* data = safe_cast<SaveFilthVorm*>(baseData);
	Start();//?????
}
