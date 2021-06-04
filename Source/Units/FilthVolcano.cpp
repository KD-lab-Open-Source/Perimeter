#include "StdAfx.h"

#include "Universe.h"

#include "Config.h"
#include "ForceField.h"
#include "GenericFilth.h"
#include "Region.h"
#include "IronFrame.h"
#include "IronExplosion.h"
#include "filth.hi"
#include "Runtime.h"

#include "FilthVolcano.h"

terFilthSwarmVolcano::terFilthSwarmVolcano(terFilthSpot* spot,const Vect3f& pos,bool first_)
: terFilthSwarm(spot,pos)
{
	first=first_;
	BirthProcessPoint=NULL;

	EffectLibrary* lib=gb_VisGeneric->GetEffectLibrary("Volcano");
	EffectKey* key=lib->Get("effect_23");
	xassert(key);
	effect=terScene->CreateEffect(*key,NULL);
	effect->SetPosition(MatXf(Mat3f::ID,To3D(position)));

/*
	lib=gb_VisGeneric->GetEffectLibrary("world");
	key=lib->Get("lava");
	xassert(key);
	lava=terScene->CreateEffect(*key,NULL);
	lava->SetPosition(MatXf(Mat3f::ID,To3D(position)));
/*/
	lava=NULL;
/**/
}

terFilthSwarmVolcano::~terFilthSwarmVolcano()
{
	if(effect)
		effect->StopAndReleaseAfterEnd();
	if(lava)
		lava->StopAndReleaseAfterEnd();

	if(BirthProcessPoint)
	{
		delete BirthProcessPoint;
		BirthProcessPoint=NULL;
	}
	creature_num=0;
	generation_period=0;
}

void terFilthSwarmVolcano::SetPrm(terFilthVolcanoStruct* prm_)
{
	prm=prm_;
	if(first) {
		int size=prm->volcano_size;
		//BirthProcessPoint = new CGeoInfluence(round(position.x)-size/2,round(position.y)-size/2, size,size);
		BirthProcessPoint = new sTVolcano(round(position.x),round(position.y), 128, 128);
	}
}

terUnitAttributeID terFilthSwarmVolcano::GetUnitID()
{
	return prm->unit_id;
}

void terFilthSwarmVolcano::SetCreatureGeneration(int creature_num_,int generation_period_)
{
	creature_num=creature_num_;
	generation_period=generation_period_;
}

void terFilthSwarmVolcano::Quant()
{
	terFilthSwarm::Quant();
	effect->SetPosition(MatXf(Mat3f::ID,To3D(position)));
	if(lava)
		lava->SetPosition(MatXf(Mat3f::ID,To3D(position)));

	if(BirthProcessPoint)
	{
		if(!BirthProcessPoint->quant())
		{
			BirthProcessPoint=NULL;
		}else
			return;
	}

	if(generation_period)
	{
		GenerationProcess();
	}else
	{
		Alive = 0;
	}
}


void terFilthSwarmVolcano::GenerationProcess()
{
	float chance=creature_num/(float)generation_period;
	generation_period--;

	if(terLogicRNDfrand()<chance)
	{
		creature_num--;
		xassert(creature_num>=0);
		Vect3f pos=To3D(position);
		if(SpotPoint && SpotPoint->terCheckFilthPoint(round(pos.x),round(pos.y)))
		{
			
			terFilthVolcano* p = safe_cast<terFilthVolcano*>(player->buildUnit(GetUnitID()));
			float model_scale = prm->size_min+(prm->size_max-prm->size_min)*terLogicRNDfrand();
			p->setRealModel(0, model_scale);
			p->SetPrm(prm);

			{
				float fmin=prm->speed_min_horizontal;
				float fdelta=prm->speed_max_horizontal-prm->speed_min_horizontal;

				float speed=fmin+fdelta*terLogicRNDfrand();
				float angle=terLogicRNDfrand()*2*M_PI;


				float x=speed*cos(angle);
				float y=speed*sin(angle);
				float z=prm->speed_min_vertical+
					(prm->speed_max_vertical-prm->speed_min_vertical)*terLogicRNDfrand();
				
				Vect3f vel(x,y,z);
				Vect3f set_pos=pos;//-vel*0.1f;
				p->setPose(Se3f(QuatF(terLogicRNDfrand()*M_PI, Vect3f::K), set_pos), false);

				p->GetRigidBodyPoint()->setVelocity(vel);
			}

			p->Start();
			unitList.push_back(p);

//			playDetectedSound(true);
		}
	}
}

void terFilthSwarmVolcano::SetFreeDestroy()
{
	generation_period=0;
}

void terFilthSwarmVolcano::DestroyLink()
{
	terFilthSwarm::DestroyLink();
	DestroyLinkList(unitList);
}

///////////////////////////terFilthVolcano//////////////////////////////////
terFilthVolcano::terFilthVolcano(const UnitTemplate& data)
:terFilthGeneric(data)
{
	model_scale=1;
	begin_wait_destroy=3;
	time_from_last_damage=0;
	bool b=sound.Init("Missile_Fly_Vulcan");
	sound.SetPos(position());
	sound.Play();
	xassert(b);

}

terFilthVolcano::~terFilthVolcano()
{
}

void terFilthVolcano::Collision(terUnitBase* p)
{
	if(isEnemy(p) && IsAttackable(p))
	{
		float dr=prm->collision_damage_rate;
		if(time_from_last_damage>=dr)
		{
			p->setDamage(damageData(),this);
			if(time_from_last_damage<2*dr)
				time_from_last_damage-=dr;
			else
				time_from_last_damage=0;
		}
	}
}

void terFilthVolcano::AvatarQuant()
{
	terFilthGeneric::AvatarQuant();
	avatar()->setPose(BodyPoint->pose());
}

void terFilthVolcano::Quant()
{
	time_from_last_damage+=0.1f;
	terFilthGeneric::Quant();

	sound.SetPos(position());
	Vect3f pos=position();
	Vect3f zpos=To3D(pos);
	Vect3f vel=BodyPoint->velocity();
	if(BodyPoint->groundColliding() && vel.z<0)
	{
		EffectLibrary* lib=gb_VisGeneric->GetEffectLibrary("Volcano");
		EffectKey* key=lib->Get("effect_31");
		xassert(key);
		cEffect* effect=terScene->CreateEffect(*key,NULL,Scale,true);
		effect->SetPosition(MatXf(Mat3f::ID,position()));
		
		bool chaos=terCheckFilthChaos(round(position().x),round(position().y));
		if(begin_wait_destroy && !chaos)
		{
			begin_wait_destroy--;
			vel.z=-vel.z;
			vel*=0.4f;
			BodyPoint->setVelocity(vel);
		}else
		{
			sound.Stop();
			if(!chaos)
			{
				SND3DPlaySound("Missile_Explosion_Vulcan",&position());
				terCrater* p = safe_cast<terCrater*>(Player->buildUnit(UNIT_ATTRIBUTE_VOLCANO_CRATER));
				p->setPose(Se3f(QuatF::ID, position()), true);
				p->setToolzerScale(Scale*prm->crater_scale);
				p->Start();
			
				explode();
			}

			Kill();
		}
	}

}

void terFilthVolcano::setRealModel(int modelIndex, float scale)
{
	terFilthGeneric::setRealModel(modelIndex, model_scale = scale);
}


SaveUnitData* terFilthSwarmVolcano::universalSave(SaveUnitData* baseData)
{
	SaveFilthSwarmVolcano* data = castOrCreate<SaveFilthSwarmVolcano>(baseData);
	terFilthSwarm::universalSave(data);
	data->generation_period=generation_period;
	data->creature_num=creature_num;

	FilthListType::iterator it;
	FOR_EACH(unitList,it)
	if(*it)
	{
		data->unitList.push_back((*it)->universalSave(0));
	}

	return data;
}

void terFilthSwarmVolcano::universalLoad(const SaveUnitData* baseData)
{
	terFilthSwarm::universalLoad(baseData);
	const SaveFilthSwarmVolcano* data = safe_cast<const SaveFilthSwarmVolcano*>(baseData);
	generation_period=data->generation_period;
	creature_num=data->creature_num;

	SaveUnitDataList::const_iterator it;
	FOR_EACH(data->unitList,it)
	if(*it)
	{
		const SaveUnitData* one_base_data=*it;
		const SaveFilthVolcano* one_data = safe_cast<const SaveFilthVolcano*>(one_base_data);
		terFilthVolcano* unit = safe_cast<terFilthVolcano*>(player->buildUnit((*it)->attributeID));
		unit->SetPrm(prm);
		unitList.push_back(unit);
		unit->universalLoad(*it);
	}
}

SaveUnitData* terFilthVolcano::universalSave(SaveUnitData* baseData)
{
	SaveFilthVolcano* data = castOrCreate<SaveFilthVolcano>(baseData);
	terFilthGeneric::universalSave(data);
	data->begin_wait_destroy=begin_wait_destroy;
	data->time_from_last_damage=time_from_last_damage;
	return data;
}

void terFilthVolcano::universalLoad(const SaveUnitData* baseData)
{
	const SaveFilthVolcano* data = safe_cast<const SaveFilthVolcano*>(baseData);
	begin_wait_destroy=data->begin_wait_destroy;
	time_from_last_damage=data->time_from_last_damage;

	terFilthGeneric::universalLoad(data);
}
