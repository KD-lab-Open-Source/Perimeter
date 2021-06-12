#include "StdAfx.h"

#include "Universe.h"

#include "Config.h"
#include "ForceField.h"
#include "GenericFilth.h"
#include "Region.h"
#include "IronFrame.h"
#include "IronExplosion.h"
#include "FilthRat.h"
#include "filth.hi"

extern s3DSGeoParameter geo_ant_death;

terFilthSwarmRat::terFilthSwarmRat(terFilthSpot* spot,const Vect3f& pos,int attack_period_)
: terFilthSwarm(spot,pos)
{
	DeltaAngle = 0;
	ChangeAngleCount = 0;
	TargetCount = 0;
	TargetPosition = Vect3f::ZERO;
	
	attack_period=attack_period_;
	BirthProcessPoint=NULL;
	if(!terCheckFilthChaos(position) && SpotPoint->initialGeoprocess())
		BirthProcessPoint = new s_WaspBirthGeoAction(round(position.x),round(position.y), 4);

	sound.Init("Filth_Move_Rat");
	sound.SetPos(To3D(pos));
	sound.Play();
}

terUnitAttributeID terFilthSwarmRat::GetUnitID()
{
	switch(SpotPoint->GetFilthParamID())
	{
	case FILTH_SPOT_ID_RAT:
		return UNIT_ATTRIBUTE_FILTH_RAT;
	case FILTH_SPOT_ID_SNAKE:
		return UNIT_ATTRIBUTE_FILTH_SNAKE;
	case FILTH_SPOT_ID_A_RAT:
		return UNIT_ATTRIBUTE_FILTH_A_RAT;
	default:
		xassert(0);
	}

	return UNIT_ATTRIBUTE_FILTH_ANTS;
}

void terFilthSwarmRat::SetCreatureGeneration(int creature_num,int generation_period)
{
	gen.Set(creature_num,generation_period);
}

terFilthSwarmRat::~terFilthSwarmRat()
{
	FilthListType::iterator i_unit;
	FOR_EACH(unitList,i_unit)
	{
		terFilthRat* p=*i_unit;
		p->Kill();
	}

	if(BirthProcessPoint)
	{
		delete BirthProcessPoint;
		BirthProcessPoint=NULL;
	}
}

void terFilthSwarmRat::SetFreeDestroy()
{
	FilthListType::iterator i_unit;
	FOR_EACH(unitList,i_unit)
		(*i_unit)->SetFreeDestroy();
}

float terFilthSwarmRat::GetFollow(Vect3f& v1,Vect3f& v2,float a,Vect3f& result)
{
	Vect3f v;
	float d;
	v = v2;
	v -= v1;
	d = v.norm();
	if(d > FLT_EPS)
		v *= terFilthRatPrm.follow_distance / d;
	else
		v = Vect3f(1.0f,0,0);
	v += v1;
	result = v;
	return d;
}

void terFilthSwarmRat::Quant()
{
	Vect3f v1,v2,v;
	Vect3f nv;
	float adist,dist,a,d;
	FilthListType::iterator i_unit;

	terFilthSwarm::Quant();

	if(BirthProcessPoint)
	{
		if(!BirthProcessPoint->quant())
		{
			BirthProcessPoint=NULL;
		}
	}

	if(!gen.InProcess() && unitList.empty())
		Alive = 0;

	if(gen.InProcess())
	{
		GenerationProcess();
	}else
	{
		if(!TargetPoint)
			FindTargetPoint();

		if(!unitList.empty())
		{
			if(TargetPoint)
			{
				v = TargetPoint->position();

				terUnitReal* point=dynamic_cast<terUnitReal*>(TargetPoint);
				if(point)
				{
					if(!point->BodyPoint->way_points.empty())
					{
						v=point->BodyPoint->way_points.back();
					}
				}

				d = TargetPoint->radius() * 5.0f;
			}else
			{
				d = 1.0f;
				TargetCount--;
				if(TargetCount < 0){
					TargetPosition.x = vMap.H_SIZE / 4 + terLogicRND(vMap.H_SIZE / 2);
					TargetPosition.y = vMap.V_SIZE / 4 + terLogicRND(vMap.V_SIZE / 2);
					TargetPosition.z = (float)(vMap.GetAlt(vMap.XCYCL(round(TargetPosition.x)),vMap.YCYCL(round(TargetPosition.y))) >> VX_FRACTION);
					TargetCount = 100;
				}
				v = TargetPosition;
			}
			
			v2=v;

			i_unit = unitList.begin();
			a = (*i_unit)->angleZ();
			v1 = (*i_unit)->position();
			sound.SetPos(To3D(v1));

			v2 -= v1;
			dist = v2.norm();
			if(dist < d)
				(*i_unit)->SetFilthTarget(v);
			else
			{
				ChangeAngleCount--;
				if(ChangeAngleCount <= 0){
					float delta_angle=terFilthRatPrm.noise_angle_delta*(M_PI/180.0f);
					if(DeltaAngle > 0)
						DeltaAngle = -terLogicRNDfrand() * delta_angle;
					else
						DeltaAngle = terLogicRNDfrand() * delta_angle;
					ChangeAngleCount = terFilthRatPrm.noise_change_time;
				}
				v2 *= 256.0f / dist;
				nv.x = -v2.y;
				nv.y = v2.x;
				nv.z = 0;
				nv *= sinf(DeltaAngle);
				v2 *= cosf(DeltaAngle);
				v = v2;
				v += nv;
				v += v1;

				(*i_unit)->SetFilthTarget(v);
			}

			adist = 0;

			i_unit++;
			while(i_unit != unitList.end())
			{
				v2 = (*i_unit)->position();
				dist = GetFollow(v1,v2,a,v);
				adist += dist;
				if(dist > terFilthRatPrm.follow_distance)
					(*i_unit)->SetFilthScale(2.0f);
				else
					(*i_unit)->SetFilthScale(1.0f);
				(*i_unit)->SetFilthTarget(v);
				a = (*i_unit)->angleZ();
				v1 = v2;
				i_unit++;
			}

			if(unitList.size() > 1)
			{
				adist /= (float)(unitList.size() - 1);
				if(adist > terFilthRatPrm.brake_distance)
					unitList.front()->SetFilthScale(0.2f);
				else
					unitList.front()->SetFilthScale(1.0f);
			}
		}

		if(TargetPoint)
		{
/*
			terUnitGeneric* point=dynamic_cast<terUnitGeneric*>(TargetPoint);
			FOR_EACH(unitList,i_unit)
			{
				const Vect3f& enemy_pos=TargetPoint->position();
				float min_dist=(TargetPoint->radius()+(*i_unit)->radius())*terFilthRatPrm.radius_mul;
				float dist=(*i_unit)->position().distance(enemy_pos);
				if(dist<min_dist)
				{
					(*i_unit)->SetAttack(point);
				}
			}
*/
		}
	}
}

void terFilthSwarmRat::DestroyLink()
{
	terFilthSwarm::DestroyLink();
	DestroyLinkList(unitList);
}

void terFilthSwarmRat::GenerationProcess()
{
	int num = gen.Quant();

	for(int i = 0;i < num;i++){
		float a = terLogicRNDfrand()*M_PI*2.0f;
		float r = terFilthRatPrm.CreatureGenerationRadius;
		Vect3f v;
		v = position;
		v.z = 0;
		if(v.x > 0 && v.y > 0 && v.x < vMap.H_SIZE && v.y < vMap.V_SIZE)
		{
			if(SpotPoint && SpotPoint->terCheckFilthPoint(round(v.x),round(v.y)))
			{
				terFilthRat* p = safe_cast<terFilthRat*>(player->buildUnit(GetUnitID()));
				p->setPose(Se3f(QuatF(terLogicRNDfrand()*M_PI, Vect3f::K), v), false);
				p->SetAttackPeriod(attack_period);
				p->Start();
				unitList.push_back(p);

				playDetectedSound();
			}
		}
	}
}

//---------------------------------------------------------

terFilthRat::terFilthRat(const UnitTemplate& data) : terFilthGeneric(data)
{
	TargetPosition = Vect3f::ZERO;
	SpeedScale = 1.0f;
	SpeedFactor = 1.0f;

	FilthStatus=FILTH_RAT_STATUS_RUN;
	zeroPositionTimer_.start(100);
	first_z_negate=true;
	last_x_angle=0;
	intrerpolate_x_angle=last_x_angle;
	DeathProcessPoint=NULL;
	free_destroy=false;
}

terFilthRat::~terFilthRat()
{
	if(DeathProcessPoint)
	{
		delete DeathProcessPoint;
		DeathProcessPoint=NULL;
	}
}

void terFilthRat::setPose(const Se3f& poseIn, bool initPose)
{
	Se3f poseZ = poseIn;
	if(zeroPositionTimer_())
		poseZ.trans().z = -3;	
	terFilthGeneric::setPose(poseZ, initPose);
}

void terFilthRat::WayPointStart()
{
	terFilthGeneric::WayPointStart();
	realAvatar()->setSight(0);
}

const int FILTH_RAT_SIGHT_DEATH=1000;//милисекунд (тфу)

void terFilthRat::WayPointController()
{
	terFilthGeneric::WayPointController();

	BodyPoint->way_points.clear();
	BodyPoint->setVelocityFactor(1);

	average(SpeedFactor, SpeedScale, 0.3f);

	if(last_x_angle<0.1f)
	if(FilthStatus==FILTH_RAT_STATUS_RUN)
	{
		BodyPoint->setVelocityFactor(SpeedFactor);
		BodyPoint->way_points.push_back(TargetPosition);
	}
}

void terFilthRat::Quant()
{
	terFilthGeneric::Quant();

	if(free_destroy && !DeathProcessPoint && FilthStatus==FILTH_RAT_STATUS_RUN)
	{
		FilthStatus=FILTH_RAT_STATUS_STOP;
		DeathProcessPoint = c3DSGeoActionCreator::Build(round(position().x),round(position().y), angleZ(),&geo_ant_death);
	}

	if(DeathProcessPoint)
	{
		if(!DeathProcessPoint->quant())
		{
			delete DeathProcessPoint;
			DeathProcessPoint=NULL;
		//tmp free_destroy	StartDeath();
		}
	}

	if(FilthStatus==FILTH_RAT_STATUS_DEATH && 
		(realAvatar()->requestDone() && realAvatar()->chainID()==CHAIN_DETACH) )
	{
		soundEvent(SOUND_EVENT_EXPLOSION);
		Kill();
	}

	if(FilthStatus!=FILTH_RAT_STATUS_DEATH)
	if(//clusterColliding() || 
		BodyPoint->chaosCollidingFactor()>0.5f //BodyPoint->chaosColliding() 
		|| !damageMolecula().isAlive())
	{
//		if(clusterColliding())
//			FieldEffect();
		StartDeath();
	}

	if(first_z_negate)
	{
		Vect3f pos=To3D(position());
		float dz=position().z-pos.z;
		if(dz<0)
		{
			last_x_angle=M_PI*0.5f;
		}

		if(dz>0)
			first_z_negate=false;
	}else
		last_x_angle*=0.9f;
	intrerpolate_x_angle=last_x_angle;
}

void terFilthRat::AvatarQuant()
{
	terFilthGeneric::AvatarQuant();

	if(FilthStatus == FILTH_RAT_STATUS_DEATH)
	{
		float alpha=(zeroPositionTimer_.get_start_time()-global_time()+FILTH_RAT_SIGHT_DEATH*0.5f)/(float)FILTH_RAT_SIGHT_DEATH;
		realAvatar()->setSight(alpha);
	}
	else
	{
		Vect3f pos=To3D(position());
		float dz=position().z-pos.z;
		if(dz<=-attr().boundRadius)
			realAvatar()->setSight(0);
		else
			realAvatar()->setSight(1);

		avatar()->Show();


		QuatF rpos(intrerpolate_x_angle,Vect3f::I);
		
		Se3f ps=BodyPoint->pose();
		ps.rot().postmult(rpos);
		avatar()->setPose(ps);
	}
}

void terFilthRat::StartDeath()
{
	zeroPositionTimer_.start(FILTH_RAT_SIGHT_DEATH);
	FilthStatus=FILTH_RAT_STATUS_DEATH;
	realAvatar()->requestChain(CHAIN_DETACH);
}

void terFilthRat::SetFreeDestroy()
{
/* //tmp free_destroy
	free_destroy=true;
/*/
	if(FilthStatus!=FILTH_RAT_STATUS_DEATH)
	{
		if(!terCheckFilthChaos(position()))
			DeathProcessPoint = c3DSGeoActionCreator::Build(round(position().x),round(position().y), angleZ(),&geo_ant_death);
			
		StartDeath();
	}
/**/
}

void terFilthRat::SetAttack(terUnitGeneric* enemy_unit)
{
	enemy_unit->explode();
	enemy_unit->Kill();
}

void terFilthRat::setFieldDamage(const DamageData& damage)
{
	if(FilthStatus==FILTH_RAT_STATUS_DEATH)
		return;
	FieldEffect();
	StartDeath();
}

SaveUnitData* terFilthSwarmRat::universalSave(SaveUnitData* baseData)
{
	SaveFilthSwarmRat* data = castOrCreate<SaveFilthSwarmRat>(baseData);
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

void terFilthSwarmRat::universalLoad(const SaveUnitData* baseData)
{
	terFilthSwarm::universalLoad(baseData);
	const SaveFilthSwarmRat* data = safe_cast<const SaveFilthSwarmRat*>(baseData);

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
		terFilthRat* unit = safe_cast<terFilthRat*>(player->buildUnit((*it)->attributeID));
		unitList.push_back(unit);
		unit->universalLoad(*it);
	}
}

SaveUnitData* terFilthRat::universalSave(SaveUnitData* baseData)
{
	SaveFilthRat* data = castOrCreate<SaveFilthRat>(baseData);
	terFilthGeneric::universalSave(data);
	data->FilthStatus=FilthStatus;
	data->TargetPosition=TargetPosition;
	data->SpeedScale=SpeedScale;
	data->SpeedFactor=SpeedFactor;

	data->free_destroy=free_destroy;
	data->zeroPositionTimer=zeroPositionTimer_();

	data->first_z_negate=first_z_negate;
	data->last_x_angle=last_x_angle;
	data->intrerpolate_x_angle=intrerpolate_x_angle;
	return data;
}

void terFilthRat::universalLoad(const SaveUnitData* baseData)
{
	const SaveFilthRat* data = safe_cast<const SaveFilthRat*>(baseData);

	FilthStatus=(terFilthRatStatusType)data->FilthStatus;
	TargetPosition=data->TargetPosition;
	SpeedScale=data->SpeedScale;
	SpeedFactor=data->SpeedFactor;

	free_destroy=data->free_destroy;
	zeroPositionTimer_.start(data->zeroPositionTimer);

	first_z_negate=data->first_z_negate;
	last_x_angle=data->last_x_angle;
	intrerpolate_x_angle=data->intrerpolate_x_angle;

	terFilthGeneric::universalLoad(data);
}
