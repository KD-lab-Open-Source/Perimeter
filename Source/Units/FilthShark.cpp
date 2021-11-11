#include "StdAfx.h"

#include "Universe.h"
#include "Player.h"

#include "Runtime.h"
#include "Config.h"
#include "ForceField.h"
#include "GenericFilth.h"
#include "Region.h"
#include "IronFrame.h"
#include "IronExplosion.h"
#include "FilthShark.h"
#include "filth.hi"

terFilthSwarmShark::terFilthSwarmShark(terFilthSpot* spot, const Vect3f& pos,int attack_period_)
: terFilthSwarm(spot,pos)
{
	attack_period=attack_period_;
}

terFilthSwarmShark::~terFilthSwarmShark()
{
}

void terFilthSwarmShark::Quant()
{
	terFilthSwarm::Quant();
	if(!gen.InProcess() && units.empty())
		Alive = 0;

	if(!TargetPoint)
		FindTargetPoint();

	if(gen.InProcess())
	{
		GenerationProcess();
	}
		
/*
	else
	{
		if(TargetPoint)
		{
			vector<terFilthShark*>::iterator it;
			FOR_EACH(units,it)
				(*it)->SetTarget(TargetPoint->position());
		}
	}
*/
}

void terFilthSwarmShark::SetFreeDestroy()
{
	std::vector<terFilthShark*>::iterator i_unit;
	FOR_EACH(units,i_unit)
		(*i_unit)->MustDieFree();
}

void terFilthSwarmShark::DestroyLink()
{
	terFilthSwarm::DestroyLink();
	DestroyLinkList(units);
}

void terFilthSwarmShark::GenerationProcess()
{
	int num = gen.Quant();

	for(int i = 0;i < num;i++)
	{
		float a = terLogicRNDfrand()*M_PI*2.0f;
		float r = terFilthSharkPrm.CreatureGenerationRadius;
		Vect3f v;
		v = position;
		v.x+=terLogicRNDfrnd()*r;
		v.y+=terLogicRNDfrnd()*r;
		v.z = -50;
		if(v.x > 0 && v.y > 0 && v.x < vMap.H_SIZE && v.y < vMap.V_SIZE)
		{
			if(SpotPoint && SpotPoint->terCheckFilthPoint(xm::round(v.x), xm::round(v.y)))
			{
				terFilthShark* p = safe_cast<terFilthShark*>(player->buildUnit(GetUnitID()));
				p->setPose(Se3f(QuatF(0, Vect3f::K), v), false);
				p->SetAttackPeriod(attack_period);
				if(TargetPoint)
					p->SetTargetDirection(TargetPoint->position()-position);
				p->Start();
				units.push_back(p);

				playDetectedSound();
			}
		}
	}
}

void terFilthSwarmShark::SetCreatureGeneration(int creature_num,int generation_period)
{
	gen.Set(creature_num,generation_period);
}


///////////////////////////terFilthShark//////////////////////////
terFilthShark::terFilthShark(const UnitTemplate& data)
: terFilthSpline(data)
{
	is_hmax=false;
	last_effect_up=false;
	GhostHolePoint=NULL;
	target.set(0,0,0);
	pin.set(0,1,0);
	is_die=false;
	is_die_free=false;
	attack_period=1;
	k_acceleration=0.5e-2f;
}

terFilthShark::~terFilthShark()
{
	delete GhostHolePoint;
}

void terFilthShark::SetTargetDirection(Vect3f pin_)
{
	pin=pin_;
	pin.z=0;
	pin.Normalize();
}

void terFilthShark::Quant()
{
	terFilthSpline::Quant();

	if(GhostHolePoint)
	{
		if(!GhostHolePoint->quant())
		{
			delete GhostHolePoint;
			GhostHolePoint=NULL;
		}
	}

	EffectRun();

	if(attack_period)
	{
		attack_period--;
		if(attack_period==0)
			MustDie();
	}

	if(!damageMolecula().isAlive())
		MustDieFree();

	if(is_die_free)
	{
		float sight=timer_die_free()/(terFilthSharkPrm.free_die_time*1e3f);
		if(sight<=0)
		{
			sight=0;
//			if(!GhostHolePoint)
			{
				Kill();
			}
		}

		realAvatar()->setSight(sight);
	}else
	{
		Vect3f pos=To3D(position());
		float dz=position().z-pos.z;
		if(dz>terFilthSharkPrm.show_z)
			realAvatar()->setSight(1);
		else
			realAvatar()->setSight(0);
	}


	if(clusterColliding() && TestUp(position(),false))
	{
		FieldEffect();
		MustDieFree();
	}
}

void terFilthShark::addWayPoint()
{
	if(is_die && !is_hmax)
	{
		Kill();
	}

	const int hmin=-30,hmax=+30;
	const int len_up=50,len=100;
	if(way_points.empty())
	{
		Vect3f p=To3D(position());
		p.z+=hmin;
		way_points.push_back(p-pin*len);
		way_points.push_back(p);
	}

	Vect3f prev=way_points.back();
	
	Vect3f pos=To3D(prev+pin*len_up);
	pos.z+=is_hmax?hmin:hmax;
	way_points.push_back(pos);

	pos=To3D(pos+pin*len);
	pos.z+=is_hmax?hmin:hmax;
	way_points.push_back(pos);


	is_hmax=!is_hmax;
}

bool terFilthShark::TestUp(const Vect3f& pos,bool last_up)
{
	Vect3f pm=To3D(pos);
	float add=last_up?terFilthSharkPrm.test_z_up:terFilthSharkPrm.test_z_down;

	return (pos.z+add)>pm.z;
}

void terFilthShark::EffectRun()
{
	if(GhostHolePoint || is_die_free)
		return;
	Vect3f p[4];
	bool up[4];
	GetSplineParameter(p);
	for(int i=0;i<4;i++)
	{
		Vect3f pm=To3D(p[i]);
		up[i]=p[i].z>pm.z;
	}

	if(up[1]==up[2])
		return;

	float begin_phase=GetSplinePhase();
	Vect3f cur_pos=HermitSpline(begin_phase,p[0],p[1],p[2],p[3]);
	bool cur_up=TestUp(cur_pos,up[1]);

	if(cur_up==last_effect_up)
		return;
		
	delete GhostHolePoint;
	GhostHolePoint=NULL;

	{
		int geo_radius=terFilthSharkPrm.geo_radius;
		int x= xm::round(cur_pos.x),y= xm::round(cur_pos.y);
		if(x-geo_radius>0 && y-geo_radius>0 &&
			x+geo_radius<vMap.H_SIZE && y+geo_radius<vMap.V_SIZE &&
			!terCheckFilthChaos(position()))
		{
			GhostHolePoint=new sGeoWave(x,y,geo_radius);
		}else
		{
			MustDie();
		}
	}

	if(terCheckFilthZero(xm::round(cur_pos.x), xm::round(cur_pos.y)) ||
       terCheckFilthChaos(xm::round(cur_pos.x), xm::round(cur_pos.y)))
	{
		MustDie();
	}

	bool b=SND3DPlaySound(cur_up?"Filth_Move_Shark1":"Filth_Move_Shark2",&position());
	xassert(b);
	

	last_effect_up=cur_up;
}

void terFilthShark::MustDie()
{
	if(is_die)
		return;

	soundEvent(SOUND_EVENT_EXPLOSION);
	is_die=true;

}

void terFilthShark::MustDieFree()
{
	if(is_die_free)
		return;

	soundEvent(SOUND_EVENT_EXPLOSION);

	{
		EffectLibrary* lib=gb_VisGeneric->GetEffectLibrary("filth");
		xassert(lib);
		if(lib)
		{
			EffectKey* key=lib->Get("shark");
			xassert(key);

			if(key)
			{
				cEffect* effect=terScene->CreateScaledEffect(*key,avatar()->GetModelPoint(),true);
				effect->SetPosition(pose());
				effect->LinkToNode(avatar()->GetModelPoint());
			}
		}
	}

	is_die_free=true;
	timer_die_free.start(terFilthDragonPrm.free_die_time*1e3f);
}


SaveUnitData* terFilthSwarmShark::universalSave(SaveUnitData* baseData)
{
	SaveFilthSwarmShark* data = castOrCreate<SaveFilthSwarmShark>(baseData);
	terFilthSwarm::universalSave(data);
	data->attack_period=attack_period;
	gen.Save(data->generate);

	FilthListType::iterator it;
	FOR_EACH(units,it)
	if(*it)
	{
		data->unitList.push_back((*it)->universalSave(0));
	}
	return data;
}

void terFilthSwarmShark::universalLoad(const SaveUnitData* baseData)
{
	terFilthSwarm::universalLoad(baseData);
	const SaveFilthSwarmShark* data = safe_cast<const SaveFilthSwarmShark*>(baseData);
	attack_period=data->attack_period;
	gen.Load(data->generate);

	SaveUnitDataList::const_iterator it;
	FOR_EACH(data->unitList,it)
	if(*it)
	{
		terFilthShark* unit = safe_cast<terFilthShark*>(player->buildUnit((*it)->attributeID));
		units.push_back(unit);
		unit->universalLoad(*it);
	}
}

SaveUnitData* terFilthShark::universalSave(SaveUnitData* baseData)
{
	SaveFilthShark* data = castOrCreate<SaveFilthShark>(baseData);
	terFilthSpline::universalSave(data);

	data->target=target;
	data->pin=pin;
	data->is_hmax=is_hmax;
	data->last_effect_up=last_effect_up;
	data->is_die=is_die;
	data->is_die_free=is_die_free;
	data->attack_period=attack_period;
	data->timer_die_free=timer_die_free();

	return data;
}

void terFilthShark::universalLoad(const SaveUnitData* baseData)
{
	const SaveFilthShark* data = safe_cast<const SaveFilthShark*>(baseData);

	target=data->target;
	pin=data->pin;
	is_hmax=data->is_hmax;
	last_effect_up=data->last_effect_up;
	is_die=data->is_die;
	is_die_free=data->is_die_free;
	attack_period=data->attack_period;
	timer_die_free.start(data->timer_die_free);

	terFilthSpline::universalLoad(data);
}
