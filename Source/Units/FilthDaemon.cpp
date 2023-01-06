#include "StdAfx.h"

#include "Universe.h"

#include "Config.h"
#include "ForceField.h"
#include "GenericFilth.h"
#include "Region.h"
#include "IronFrame.h"

#include "IronExplosion.h"
#include "FilthDaemon.h"
#include "filth.hi"

const float DAEMON_ADDZ=40;
const float DAEMON_CREATE_DISTANCE=30;
const float DAEMON_DELTA_START_PHASE=0.1f;
const float DAEMON_SPEED=100;
const float DAEMON_SPEED_ZDOWN=20;
const float DAEMON_MAX_TIME_TO_BERSERK=50;
const float DAEMON_A=1.0f;
const float DAEMON_SPEED_GO_UP=100.0f;
const int DAEMON_BLUR_LEN=6;
const int DAEMON_BLUR_DELTA=2;

terFilthSwarmDaemon::terFilthSwarmDaemon(terFilthSpot* spot,const Vect3f& pos,int attack_period_)
: terFilthSwarm(spot,pos)
{
	attack_pos=pos;
	attack_period=attack_period_;

	berserk_time=-1;
	berserk_mode=false;
	must_init_pos=true;
	GenerationFactor=GenerationCount=GenerationSpeed=0;
	pin=Vect3f::I;
}

void terFilthSwarmDaemon::SetCreatureGeneration(int creature_num,int generation_period)
{
	GenerationFactor = 0;
	GenerationCount = generation_period;
	if(GenerationCount > 0)
		GenerationSpeed = (float)(creature_num) / (float)(GenerationCount);
	else
		GenerationSpeed = 0;
	init_pos.resize(creature_num);
}

terFilthSwarmDaemon::~terFilthSwarmDaemon()
{
	FilthListType::iterator it;
	FOR_EACH(unit_list,it)
		(*it)->Kill();
}

void terFilthSwarmDaemon::SetFreeDestroy()
{
	FilthListType::iterator i_unit;
	FOR_EACH(unit_list,i_unit)
		(*i_unit)->SetFreeDestroy();
}

void terFilthSwarmDaemon::Quant()
{
	if(GenerationCount<=0 && unit_list.empty())
		Alive = 0;
	if(!TargetPoint)
		FindTargetPoint();

	if(TargetPoint)
		attack_pos=TargetPoint->position();
	Generate();

	if(unit_list.empty())
		return;

	if(!berserk_mode)
	{
		FilthListType::iterator it;
		bool on_zeroplast=false,not_on_zeroplast=false;
		FOR_EACH(unit_list,it)
		{
			if((*it)->isDestroyed())
				continue;
			bool b=(*it)->isOnZeroplast();
			if(b)
				on_zeroplast=true;
			else
				not_on_zeroplast=true;
		}

		if(on_zeroplast && not_on_zeroplast && berserk_time==-1)
			berserk_time=DAEMON_MAX_TIME_TO_BERSERK;

		if(berserk_time==0 || (on_zeroplast && !not_on_zeroplast))
		{
			FOR_EACH(unit_list,it)
				(*it)->SetBerserkMode();
		}

		if(attack_period<0)
		{
			FOR_EACH(unit_list,it)
				(*it)->SetFreeDestroy();
		}

		if(berserk_time>0)
			berserk_time--;
	}else
	{
	}

	attack_period--;

}

void terFilthSwarmDaemon::DestroyLink()
{
	terFilthSwarm::DestroyLink();
	DestroyLinkList(unit_list);
}

terUnitAttributeID terFilthSwarmDaemon::GetUnitID()
{
	switch(SpotPoint->GetFilthParamID())
	{
	case FILTH_SPOT_ID_DAEMON:
		return UNIT_ATTRIBUTE_FILTH_DAEMON;
	case FILTH_SPOT_ID_A_DAEMON:
		return UNIT_ATTRIBUTE_FILTH_A_DAEMON;
	}
	xassert(0);
	return UNIT_ATTRIBUTE_FILTH_DAEMON;
}

void terFilthSwarmDaemon::Generate()
{
	if(must_init_pos)
	{
		must_init_pos=false;
		pin=attack_pos-position;
		pin.z=0;
		if(pin.norm2()<1)
			pin.set(0,1,0);
		pin.Normalize();
		Vect3f norm(pin.y,-pin.x,0);

		int num=init_pos.size();
		float r=terFilthDaemonPrm.CreatureGenerationRadius;
		float center=num*0.5f;

		for(int i = 0;i < num;i++)
		{
			Vect3f pos=position+norm*(DAEMON_CREATE_DISTANCE*(i-center));
			pos+=pin*(terLogicRNDfrnd()*30);
			pos=To3D(pos);
			pos.z-=100;
			init_pos[i]=pos;
		}
	}

	if(GenerationCount<=0)
		return;
	GenerationCount--;
	GenerationFactor += GenerationSpeed;
	int num = xm::round(xm::floor(GenerationFactor));


	for(int i = 0;i < num;i++)
	{
		int ip=terLogicRND(init_pos.size());
		if(ip>=init_pos.size())
			break;

		Vect3f pos=init_pos[ip];
		init_pos.erase(init_pos.begin()+ip);

		if(SpotPoint && SpotPoint->terCheckFilthPoint(xm::round(pos.x), xm::round(pos.y)))
		{
			terFilthDaemon* p;
			p = safe_cast<terFilthDaemon*>(player->buildUnit(GetUnitID()));
			p->setPose(Se3f(QuatF::ID, pos), false);
			p->setStartPhase(terLogicRNDfrand());
			p->setDirection(pin);
			p->Start();
			unit_list.push_back(p);

			playDetectedSound();
		}
	}

	GenerationFactor -= (float)num;
}

///////////////////////////////terFilthDaemon/////////////////////
terFilthDaemon::terFilthDaemon(const UnitTemplate& data)
:terFilthGeneric(data)
{
	start_phase=0;
	on_zeroplast=false;
	berserk_mode=false;
	mul_speed=0.0f;
	free_destroy=false;
	geo_effect=NULL;
	go_up=true;

	obj_sight.startPeriod(1,2.0e3f,false);
}

terFilthDaemon::~terFilthDaemon()
{
	std::list<OneShadow>::iterator it;
	FOR_EACH(shadows,it)
	{
		it->model->Release();
	}

	delete geo_effect;
}

void terFilthDaemon::Start()
{
	terFilthGeneric::Start();
	avatar()->Start();
	realAvatar()->setPhase(start_phase);
	realAvatar()->setSight(0);

	if(false)
	{
		geo_effect=new demonToolzer(32);
		geo_effect->start(xm::round(position().x), xm::round(position().y));
	}

	sound.Init("Filth_Move_Daemon");
	sound.SetPos(To3D(position()));
	sound.Play();
}

void terFilthDaemon::AvatarQuant()
{
	terFilthGeneric::AvatarQuant();
	avatar()->setPose(pose());
}

void terFilthDaemon::MoveQuant()
{
	float minimal_h=radius();
	float optimal_h=radius()+terFilthDaemonPrm.DaemonHeight;
	float dt=0.1f;
	Vect3f pos=position();

	float angle=xm::atan2(direction.y,direction.x)-XM_PI*0.5f;
	QuatF rot(angle,Vect3f(0,0,1));

	if(mul_speed<1.0f)
		mul_speed+=DAEMON_A*dt;
	else
		mul_speed=1.0f;
	
	Vect3f pos_z=To3D(pos);

	if(go_up)
	{
		float dz=pos_z.z+optimal_h-pos.z;
		float speed_up=DAEMON_SPEED_GO_UP*dt;
		if(dz>speed_up)
			pos.z+=speed_up;
		else
			go_up=false;

	}else
	{
		if((on_zeroplast && !berserk_mode))
		{
		}else
		{
			pos+=direction*(DAEMON_SPEED*dt*mul_speed);
		}

		if(pos_z.z+minimal_h>pos.z)
			pos.z=pos_z.z+minimal_h;

		float dz=pos_z.z+optimal_h-pos.z;
		float speed_down=DAEMON_SPEED_ZDOWN*dt;
		if(dz<-speed_down)
			pos.z-=speed_down;
		else
		if(dz>speed_down)
			pos.z+=speed_down;
		else
			pos.z=pos_z.z+optimal_h;
	}

	terFilthGeneric::setPose(Se3f(rot,pos), false);

	if(berserk_mode)
	{
		Vect3f back=object_pos.back().pos.trans();
		ShadowInterpolate p;
		p.pos=pose();
		p.phase=realAvatar()->phase();

		Vect3f dd=p.pos.trans()-back;
//		xassert(fabs(dd.x)<10 && xm::abs(dd.y)<10);

		object_pos.push_back(p);
		object_pos.pop_front();
	}

	sound.SetPos(To3D(position()));
}

void terFilthDaemon::Quant()
{
	terFilthGeneric::Quant();
/*
	if(clusterColliding() && !is_berserk())
		SetFreeDestroy();
*/
	if(!damageMolecula().isAlive()){
		SetFreeDestroy();
	}

	if(geo_effect)
	{
		if(!geo_effect->quant())
		{
			delete geo_effect;
			geo_effect=NULL;
		}
	}


	{
		bool zero=terCheckFilthZero(position().x,position().y);
		if(on_zeroplast && !zero)
			SetFreeDestroy();
		on_zeroplast=zero;
	}

	{
		const int r=radius()+5;
		const Vect3f& p=position();
		if(//clusterColliding() || 
			(p.x<r || p.x>vMap.H_SIZE-r || p.y<r || p.y>vMap.V_SIZE-r)
			)
			SetFreeDestroy();
	}

	if(free_destroy && obj_sight.phase()<1e-2f)
	{
		soundEvent(SOUND_EVENT_EXPLOSION);
		Kill();
	}

	obj_sight.Quant();

}


void terFilthDaemon::SetFreeDestroy()
{
	if(free_destroy)
		return;
	free_destroy=true;
//	addShadow(2.0f);
	obj_sight.setEndPhase(0,false);
}

void terFilthDaemon::Collision(terUnitBase* p)
{
	if(isEnemy(p) && berserk_mode && !free_destroy && IsAttackable(p))
	{
		p->setDamage(attr()->unitDamage.mainDamage,this);
	}
}

void terFilthDaemon::setFieldDamage(const DamageData& damage)
{
}

void terFilthDaemon::SetBerserkMode()
{
	if(free_destroy)
		return;
	if(berserk_mode)
		return;
	realAvatar()->requestChain(CHAIN_SWITCHED_ON);
	berserk_mode=true;
	mul_speed=0;

	for(int i=0;i<DAEMON_BLUR_LEN*DAEMON_BLUR_DELTA+2;i++)
	{
		ShadowInterpolate p;
		p.pos=pose();
		p.phase=realAvatar()->phase();
		object_pos.push_back(p);
	}

	for(int i=0;i<DAEMON_BLUR_LEN;i++)
	{
		OneShadow p;
		cObjectNodeRoot* parent_node=avatar()->GetModelPoint();
		const char* fname=parent_node->GetFileName();
		p.model=parent_node->GetScene()->CreateObject(fname);
		p.model->SetPhase(parent_node->GetPhase());
		p.model->SetScale(parent_node->GetScale());
		p.model->SetPosition(parent_node->GetPosition());

		shadows.push_back(p);
	}

	sound.Init("Filth_Attack_Daemon");
	sound.SetPos(To3D(position()));
	sound.Play();
}

void terFilthDaemon::AvatarInterpolation()
{
	terFilthGeneric::AvatarInterpolation();

	float obj_alpha=obj_sight.phase();

	{
		Vect3f pos=To3D(position());
		float dz=position().z-pos.z;
		if(dz<=-attr()->boundRadius)
			obj_alpha=0;
	}

	realAvatar()->setSight(obj_alpha);

	if(!shadows.empty())
	{
		float alpha=0.0;
		float delta=1/(float)(DAEMON_BLUR_LEN+1);
		std::list<ShadowInterpolate>::iterator itp=object_pos.begin();

		std::list<OneShadow>::iterator it;
		FOR_EACH(shadows,it)
		{
			xassert(itp!=object_pos.end());
			ShadowInterpolate* prev_p=&*itp;
			itp++;
			xassert(itp!=object_pos.end());
			ShadowInterpolate* cur_p=&*itp;
			for(int i=1;i<DAEMON_BLUR_DELTA;i++)
				itp++;

			Se3f pos=prev_p->pos;
			float phase=prev_p->phase;

			alpha+=delta;
			OneShadow& p=*it;
			sColor4f diffuse(1,1,1,alpha*obj_alpha);
			p.model->SetColor(NULL,&diffuse);
			p.model->SetPosition(pos);
			p.model->SetPhase(phase);

		}
	}
}

SaveUnitData* terFilthSwarmDaemon::universalSave(SaveUnitData* baseData)
{
	SaveFilthSwarmDaemon* data = castOrCreate<SaveFilthSwarmDaemon>(baseData);
	terFilthSwarm::universalSave(data);

	data->must_init_pos=must_init_pos;
	data->init_pos=init_pos;
	data->attack_pos=attack_pos;

	data->attack_period=attack_period;

	data->berserk_mode=berserk_mode;
	data->berserk_time=berserk_time;

	data->GenerationCount=GenerationCount;
	data->GenerationFactor=GenerationFactor;
	data->GenerationSpeed=GenerationSpeed;

	data->pin=pin;

	std::vector<SaveUnitData*> unitList;
	FilthListType::iterator it;
	FOR_EACH(unit_list,it)
	if(*it)
	{
		data->unit_list.push_back((*it)->universalSave(0));
	}

	return data;
}

void terFilthSwarmDaemon::universalLoad(SaveUnitData* baseData)
{
	terFilthSwarm::universalLoad(baseData);
	SaveFilthSwarmDaemon* data = safe_cast<SaveFilthSwarmDaemon*>(baseData);
	must_init_pos=data->must_init_pos;
	init_pos=data->init_pos;
	attack_pos=data->attack_pos;

	attack_period=data->attack_period;

	berserk_mode=data->berserk_mode;
	berserk_time=data->berserk_time;

	GenerationCount=data->GenerationCount;
	GenerationFactor=data->GenerationFactor;
	GenerationSpeed=data->GenerationSpeed;

	pin=data->pin;

	SaveUnitDataList::const_iterator it;
	FOR_EACH(data->unit_list,it)
	if(*it)
	{
		terFilthDaemon* unit = safe_cast<terFilthDaemon*>(player->loadUnit(*it));
        if (std::find(unit_list.begin(), unit_list.end(), unit) == unit_list.end()) {
            unit_list.push_back(unit);
        }
	}
}

SaveUnitData* terFilthDaemon::universalSave(SaveUnitData* baseData)
{
	SaveFilthDaemon* data = castOrCreate<SaveFilthDaemon>(baseData);
	terFilthGeneric::universalSave(data);
	data->free_destroy=free_destroy;
	data->on_zeroplast=on_zeroplast;
	data->berserk_mode=berserk_mode;
	data->go_up=go_up;
	data->start_phase=start_phase;
	data->mul_speed=mul_speed;
	data->direction=direction;

	return data;	
}

void terFilthDaemon::universalLoad(SaveUnitData* baseData)
{
	terFilthGeneric::universalLoad(baseData);
	SaveFilthDaemon* data = safe_cast<SaveFilthDaemon*>(baseData);

	free_destroy=data->free_destroy;
	on_zeroplast=data->on_zeroplast;
	berserk_mode=data->berserk_mode;
	go_up=data->go_up;
	start_phase=data->start_phase;
	mul_speed=data->mul_speed;
	direction=data->direction;
	if(free_destroy)
		obj_sight.setEndPhase(0,false);
}
