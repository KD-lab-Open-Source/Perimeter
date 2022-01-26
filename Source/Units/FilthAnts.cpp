#include "StdAfx.h"

#include "Universe.h"

#include "Config.h"
#include "ForceField.h"
#include "GenericFilth.h"
#include "Region.h"
#include "IronFrame.h"
#include "IronExplosion.h"
#include "FilthAnts.h"
#include "filth.hi"

struct s3DSGeoParameterAntBirth:public s3DSGeoParameter
{
	s3DSGeoParameterAntBirth()
	{
		Vect3f scaling(.035f,.035f,.015f);
		command.push_back(CommandSet(0, "", 6, scaling, 0, 0));
		command.push_back(CommandSet(1, "ant.M3D", 6, scaling, 0, 0));
		command.push_back(CommandSet(2, "ant.M3D", 4, scaling, 1, 0));
	}
};

struct s3DSGeoParameterAntDeath:public s3DSGeoParameter
{
	s3DSGeoParameterAntDeath()
	{
		Vect3f scaling(.035f,.035f,.015f);
		command.push_back(CommandSet(0, "", 6, scaling, 0, 0));
		command.push_back(CommandSet(1, "ant.M3D", 4, scaling, 0, 0));
	}
};

s3DSGeoParameter geo_ant_birth=s3DSGeoParameterAntBirth();
s3DSGeoParameter geo_ant_death=s3DSGeoParameterAntDeath();


terFilthSwarmAnt::terFilthSwarmAnt(terFilthSpot* spot,const Vect3f& pos,int attack_period_)
: terFilthSwarm(spot,pos)
{
	DeltaAngle = 0;
	ChangeAngleCount = 0;
	TargetCount = 0;
	TargetPosition = Vect3f::ZERO;
	
	attack_period=attack_period_;
	switch(SpotPoint->GetFilthParamID())
	{
	default:
		xassert(0);
	case FILTH_SPOT_ID_ANTS:
	case FILTH_SPOT_ID_A_ANTS:
		prm=&terFilthAntsPrm;
		break;
	case FILTH_SPOT_ID_ANTS2:
	case FILTH_SPOT_ID_A_SPIDER:
		prm=&terFilthAnts2Prm;
		break;

	}
	
	sound.Init("Filth_Move_Ant");
	sound.SetPos(To3D(pos));
	sound.Play();
}

void terFilthSwarmAnt::SetFreeDestroy()
{
	FilthListType::iterator i_unit;
	FOR_EACH(unitList,i_unit)
		(*i_unit)->SetFreeDestroy();
}

void terFilthSwarmAnt::SetCreatureGeneration(int creature_num,int generation_period)
{
	gen.Set(creature_num,generation_period);
}

terFilthSwarmAnt::~terFilthSwarmAnt()
{
	FilthListType::iterator i_unit;
	FOR_EACH(unitList,i_unit)
		(*i_unit)->Kill();
}

float terFilthSwarmAnt::GetFollow(Vect3f& v1,Vect3f& v2,float a,Vect3f& result)
{
	Vect3f v;
	float d;
	v = v2;
	v -= v1;
	d = v.norm();
	if(d > FLT_EPS)
		v *= prm->follow_distance / d;
	else
		v = Vect3f(1.0f,0,0);
	v += v1;
	result = v;
	return d;
}

void terFilthSwarmAnt::Quant()
{
	Vect3f v1,v2,v;
	Vect3f nv;
	float adist,dist,a,d;
	FilthListType::iterator i_unit;

	terFilthSwarm::Quant();

	if(!gen.InProcess() && unitList.empty())
		Alive = 0;

	if(gen.InProcess()){
		GenerationProcess();
	}

	if(!TargetPoint)
		FindTargetPoint();

	if(!unitList.empty()){
		if(TargetPoint){
			v2 = v = TargetPoint->position();
			d = TargetPoint->radius() * 5.0f;
			TargetPosition=v;
		}else{
			d = 1.0f;
			TargetCount--;
			if(TargetCount < 0){
				TargetPosition.x = vMap.H_SIZE / 4 + terLogicRND(vMap.H_SIZE / 2);
				TargetPosition.y = vMap.V_SIZE / 4 + terLogicRND(vMap.V_SIZE / 2);
				TargetPosition.z = (float)(vMap.GetAlt(vMap.XCYCL(xm::round(TargetPosition.x)), vMap.YCYCL(
                        xm::round(TargetPosition.y))) >> VX_FRACTION);
				TargetCount = 100;
			}
			v2 = v = TargetPosition;
		}

		if(show_filth_debug)
		{
			show_vector(v,2,RED);
		}

		i_unit = unitList.begin();
		a = (*i_unit)->angleZ();
		v1 = (*i_unit)->position();

		sound.SetPos(To3D(v1));

		v2 -= v1;
		dist = v2.norm();
		if(dist < d)
			(*i_unit)->SetFilthTarget(v);
		else{
			ChangeAngleCount--;
			if(ChangeAngleCount <= 0){
				if(DeltaAngle > 0)
					DeltaAngle = -terLogicRNDfrand() * prm->noise_angle_delta;
				else
					DeltaAngle = terLogicRNDfrand() * prm->noise_angle_delta;
				ChangeAngleCount = prm->noise_change_time;
			}
			v2 *= 256.0f / dist;
			nv.x = -v2.y;
			nv.y = v2.x;
			nv.z = 0;
			nv *= xm::sin(DeltaAngle);
			v2 *= xm::cos(DeltaAngle);
			v = v2;
			v += nv;
			v += v1;

			(*i_unit)->SetFilthTarget(v);
		}

		adist = 0;

		i_unit++;
		int UnitNum=0;
		while(i_unit != unitList.end()){
			v2 = (*i_unit)->position();
			dist = GetFollow(v1,v2,a,v);
			adist += dist;
			UnitNum++;
			if(dist > prm->follow_distance)
				(*i_unit)->SetFilthScale(2.0f);
			else
				(*i_unit)->SetFilthScale(1.0f);
			(*i_unit)->SetFilthTarget(v);
			a = (*i_unit)->angleZ();
			v1 = v2;
			i_unit++;
		}
		if(UnitNum > 1){
			adist /= (float)(UnitNum - 1);
			if(adist > prm->brake_distance)
				unitList.front()->SetFilthScale(0.2f);
			else
				unitList.front()->SetFilthScale(1.0f);
		}
	}

}

void terFilthSwarmAnt::DestroyLink()
{
	terFilthSwarm::DestroyLink();
	DestroyLinkList(unitList);
}

void terFilthSwarmAnt::AddFilthPoint(terFilthAnt* p)
{
	unitList.push_back(p);
}

void terFilthSwarmAnt::GenerationProcess()
{
	int num = gen.Quant();

	for(int i = 0;i < num;i++){
		float a = terLogicRNDfrand()*XM_PI*2.0f;
		float r = BaseParam->SwarmRadius;
		Vect3f v;
		v.x = xm::cos(a) * r;
		v.y = xm::sin(a) * r;
		v += position;
		v.z = 0;
		if(v.x > 0 && v.y > 0 && v.x < vMap.H_SIZE && v.y < vMap.V_SIZE){
			if(SpotPoint && SpotPoint->terCheckFilthPoint(xm::round(v.x), xm::round(v.y))){
				terFilthAnt* p = safe_cast<terFilthAnt*>(player->buildUnit(GetUnitID()));
				p->setPose(Se3f(QuatF(terLogicRNDfrand()*XM_PI, Vect3f::K), v), true);
				p->SetAttackPeriod(attack_period);
				p->setInitialGeoprocess(SpotPoint->initialGeoprocess());
				p->Start();
				AddFilthPoint(p);

				playDetectedSound();
			}
		}
	}
}

terUnitAttributeID terFilthSwarmAnt::GetUnitID()
{
	switch(SpotPoint->GetFilthParamID())
	{
	case FILTH_SPOT_ID_ANTS:
		return UNIT_ATTRIBUTE_FILTH_ANTS;
	case FILTH_SPOT_ID_ANTS2:
		return UNIT_ATTRIBUTE_FILTH_ANTS2;
	case FILTH_SPOT_ID_A_ANTS:
		return UNIT_ATTRIBUTE_FILTH_A_ANTS;
	case FILTH_SPOT_ID_A_SPIDER:
		return UNIT_ATTRIBUTE_FILTH_A_SPIDER;
	default:
		xassert(0);
	}

	return UNIT_ATTRIBUTE_FILTH_ANTS;
}

//---------------------------------------------------------

terFilthAnt::terFilthAnt(const UnitTemplate& data) : terFilthGeneric(data)
{
	TargetPosition = Vect3f::ZERO;
	SpeedScale = 1.0f;
	SpeedFactor = 1.0f;

	BirthProcessPoint = NULL;
	DestroyProcessPoint = NULL;
	free_destroy=false;
	death_requested=false;
	initial_geoprocess=true;
}

terFilthAnt::~terFilthAnt()
{
	delete BirthProcessPoint;
	delete DestroyProcessPoint;
}

void terFilthAnt::WayPointStart()
{
	terFilthGeneric::WayPointStart();

	if(!BirthProcessPoint && initial_geoprocess)
	{
		BirthProcessPoint=c3DSGeoActionCreator::Build(xm::round(position().x), xm::round(position().y), angleZ(), &geo_ant_birth);
	}
}

void terFilthAnt::WayPointController()
{
	terFilthGeneric::WayPointController();

	BodyPoint->way_points.clear();
	BodyPoint->setVelocityFactor(1);

	if(free_destroy || terCheckFilthZero(xm::round(position().x), xm::round(position().y)) || 
		BodyPoint->chaosCollidingFactor()>=0.5f)
	{
		if(!DestroyProcessPoint)
			DestroyProcessPoint = c3DSGeoActionCreator::Build(xm::round(position().x), xm::round(position().y), angleZ(), &geo_ant_death);
	}

	if(!death_requested)
	if(!damageMolecula().isAlive())
	{
		realAvatar()->requestChain(CHAIN_DETACH);
		death_requested=true;
	}

	if(realAvatar()->requestDone() && realAvatar()->chainID()==CHAIN_DETACH)
	{
		soundEvent(SOUND_EVENT_EXPLOSION);
		Kill();
		return;
	}

	average(SpeedFactor, SpeedScale, 0.3f);

	if(DestroyProcessPoint)
	{
		if(!DestroyProcessPoint->quant())
		{
			delete DestroyProcessPoint;
			DestroyProcessPoint = NULL;
			soundEvent(SOUND_EVENT_EXPLOSION);
			Kill();
			return;
		}
	}

	if(isRun())
	{
		BodyPoint->setVelocityFactor(SpeedFactor);
		BodyPoint->way_points.push_back(TargetPosition);
	}

	if(BirthProcessPoint)
	{
		if(!BirthProcessPoint->quant())
		{
			delete BirthProcessPoint;
			BirthProcessPoint = NULL;
		}
	}

}

void terFilthAnt::AvatarQuant()
{
	terFilthGeneric::AvatarQuant();

	if(!isRun())
		avatar()->Hide();
	else
		avatar()->Show();

	avatar()->setPose(BodyPoint->pose());
}

void terFilthAnt::setFieldDamage(const DamageData& damage)
{
	if(!death_requested)
		return;
	death_requested=true;
	FieldEffect();

	if(!(realAvatar()->currentChain() && realAvatar()->currentChain()->chainID==CHAIN_DETACH))
	{
		realAvatar()->requestChain(CHAIN_DETACH);
	}
}

SaveUnitData* terFilthSwarmAnt::universalSave(SaveUnitData* baseData)
{
	SaveFilthSwarmAnt* data = castOrCreate<SaveFilthSwarmAnt>(baseData);
	terFilthSwarm::universalSave(data);
	gen.Save(data->generate);
	data->DeltaAngle=DeltaAngle;
	data->ChangeAngleCount=ChangeAngleCount;
	data->TargetCount=TargetCount;
	data->TargetPosition=TargetPosition;
	data->attack_period=attack_period;

	FilthListType::iterator it;
	FOR_EACH(unitList,it)
	if(*it)
	{
		data->unitList.push_back((*it)->universalSave(0));
	}

	return data;
}

void terFilthSwarmAnt::universalLoad(SaveUnitData* baseData)
{
	terFilthSwarm::universalLoad(baseData);
	SaveFilthSwarmAnt* data = safe_cast<SaveFilthSwarmAnt*>(baseData);
	gen.Load(data->generate);
	DeltaAngle=data->DeltaAngle;
	ChangeAngleCount=data->ChangeAngleCount;
	TargetCount=data->TargetCount;
	TargetPosition=data->TargetPosition;
	attack_period=data->attack_period;

	SaveUnitDataList::const_iterator it;
	FOR_EACH(data->unitList,it)
	if(*it)
	{
		terFilthAnt* unit = safe_cast<terFilthAnt*>(player->loadUnit(*it));
        if (std::find(unitList.begin(), unitList.end(), unit) == unitList.end()) {
            unitList.push_back(unit);
        }
	}

}

SaveUnitData* terFilthAnt::universalSave(SaveUnitData* baseData)
{
	SaveFilthAnt* data = castOrCreate<SaveFilthAnt>(baseData);
	terUnitReal::universalSave(data);

	data->TargetPosition=TargetPosition;
	data->SpeedScale=SpeedScale;
	data->SpeedFactor=SpeedFactor;

	data->free_destroy=free_destroy;
	data->death_requested=death_requested;
	data->initial_geoprocess=initial_geoprocess;
	return data;
}

void terFilthAnt::universalLoad(SaveUnitData* baseData)
{
	SaveFilthAnt* data = safe_cast<SaveFilthAnt*>(baseData);
	terUnitReal::universalLoad(data);

	TargetPosition=data->TargetPosition;
	SpeedScale=data->SpeedScale;
	SpeedFactor=data->SpeedFactor;

	free_destroy=data->free_destroy;
	death_requested=data->death_requested;
	initial_geoprocess=data->initial_geoprocess;
}
