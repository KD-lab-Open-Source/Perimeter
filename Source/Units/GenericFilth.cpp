#include "StdAfx.h"

#include "Universe.h"

#include "Config.h"
#include "GenericFilth.h"
#include "IronFrame.h"
#include "Runtime.h"

#include "CameraManager.h"
#include "Triggers.h"

#include "UniverseInterface.h"

#include "PerimeterSound.h"
#include "GameShell.h"

#include "FilthAnts.h"
#include "FilthWasp.h"
#include "FilthDragon.h"
#include "FilthGhost.h"
#include "FilthEye.h"
#include "FilthCrow.h"
#include "FilthDaemon.h"
#include "FilthRat.h"
#include "FilthWorm.h"
#include "FilthShark.h"
#include "FilthVolcano.h"
#include "filth.hi"

bool terCheckFilthHardness(int x,int y)
{
	unsigned short p = vMap.GABuf[vMap.offsetGBufWorldC(x,y)];
	return (p & GRIDAT_MASK_HARDNESS) != GRIDAT_MASK_HARDNESS;
}

bool terCheckFilthChaos(int x,int y)
{
	if(x<0 || x>=vMap.H_SIZE)
		return true;
	if(y<0 || y>=vMap.V_SIZE)
		return true;
	x=clamp(x,0,vMap.H_SIZE);
	y=clamp(y,0,vMap.V_SIZE);
	int z=vMap.GVBuf[vMap.offsetGBuf(x>>kmGrid, y>>kmGrid)];
	return z==0;
}

bool terCheckFilthZero(int x,int y)
{
	if(x<0 || y<0 || x>=vMap.H_SIZE || y>= vMap.V_SIZE)
		return false;

	terUniverse* tu=universe();
	for(int player=0;player<tu->Players.size();player++)
	{
		if(tu->Players[player]->energyRegion().getEditColumn().filled(x,y))
			return true;
	}

	return false;
}

bool terFilthSpot::terCheckFilthPoint(int x,int y)
{
	if(!enable_zeroplast_generate)
	if(terCheckFilthZero(x,y))
		return false;
	return true;
}

bool terCheckFilthZero(const Vect3f& pos)
{
	return terCheckFilthZero(round(pos.x),round(pos.y));
}

bool terCheckFilthChaos(const Vect3f& pos)
{
	return terCheckFilthChaos(round(pos.x),round(pos.y));
}

terFilthSpot::terFilthSpot(const UnitTemplate& data) : terUnitBase(data)
{
	radius_ = attr().boundRadius;

	sleep_ = false;
	first_sleep_time=0;
	sleep_period=1000;
	attack_period=100;
	delta_sleep_period=delta_attack_period=0;
	creature_num=creature_generation_period=1;

	FilthParamID = FILTH_SPOT_ID_WASP;
	BaseParam = &terFilthWaspBaseParameter;

	SoundImpulse = 0;
	attack_width=-1;
	attack_direction=0;

	activatingUnit_ = UNIT_ATTRIBUTE_NONE; 
	activatingDistance_ = 100; 
	create_first=true;
	attack_player=FILTH_ATTACK_ALL;

	pExclusiveAttackUnit=NULL;
	enable_zeroplast_generate=false;
	initial_geoprocess=true;
	harkback=false;
	kill_of_end=false;
	kill_after_generate=false;
}

void terFilthSpot::Kill()
{
	terUnitBase::Kill();
	FilthSwarmListType::iterator i_swarm;
	FOR_EACH(swarm_list,i_swarm)
		delete (*i_swarm);
	swarm_list.clear();
}

void terFilthSpot::SetFreeDestroy()
{
	KillOfEnd();
	FilthSwarmListType::iterator i_swarm;
	FOR_EACH(swarm_list,i_swarm)
		(*i_swarm)->SetFreeDestroy();
}

void terFilthSpot::KillOfEnd()
{
	if(!swarm_list.empty())
		DestroyLink();

	kill_of_end=true;
	if(swarm_list.empty())
		terUnitBase::Kill();
}

void terFilthSpot::KillAfterGenerate()
{
	kill_after_generate=true;
}

terFilthSpot::~terFilthSpot()
{
//	xassert(swarm_list.empty());
}

SaveUnitData* terFilthSpot::universalSave(SaveUnitData* baseData)
{
	SaveUnitFilthData* data = castOrCreate<SaveUnitFilthData>(baseData);
	terUnitBase::universalSave(data);
	
	data->radius = radius();
	data->filthType = FilthParamID;
	data->attackWidth=attack_width;
	data->attackDirection=attack_direction;

	data->sleep = sleep_;
	data->firstSleepTime=first_sleep_time;
	data->sleepPeriod=sleep_period;
	data->deltaSleepPeriod=delta_sleep_period;
	data->attackPeriond=attack_period;
	data->deltaAttackPeriond=delta_attack_period;
	data->creatureNum=creature_num;
	
	data->activatingUnit = activatingUnit_;
	data->activatingDistance = activatingDistance_;
	data->attack_player=attack_player;
	data->initial_geoprocess=initial_geoprocess;
	data->kill_of_end=kill_of_end;

	data->killTimer = killTimer_();

	if(gameShell->missionEditor())
	{
		data->sleep_timer=0;
		data->create_first=true;
	}else
	{
		data->sleep_timer=sleep_timer();
		data->create_first=create_first;
	}

	vector<HoleStruct>::iterator ith;
	FOR_EACH(hole_pos,ith)
	{
		data->hole_position.push_back(ith->pos);
		data->hole_position_inited.push_back(ith->inited);
	}

	FilthSwarmListType::iterator it;
	FOR_EACH(swarm_list,it)
	{
		if(*it)
			data->swarmList.push_back((*it)->universalSave(0));
	}

	return data;
}

void terFilthSpot::universalLoad(const SaveUnitData* baseData)
{
	const SaveUnitFilthData* data = safe_cast<const SaveUnitFilthData*>(baseData);
	terUnitBase::universalLoad(data);

	setRadius(data->radius);
	SetFilthParamID(data->filthType);
	attack_width=data->attackWidth;
	attack_direction=data->attackDirection;

	sleep_ = data->sleep;
	first_sleep_time=data->firstSleepTime;
	sleep_period=data->sleepPeriod;
	delta_sleep_period=data->deltaSleepPeriod;
	attack_period=data->attackPeriond;
	delta_attack_period=data->deltaAttackPeriond;
	creature_num=data->creatureNum;

	activatingUnit_ = data->activatingUnit;
	activatingDistance_ = data->activatingDistance;
	attack_player = data->attack_player;
	initial_geoprocess = data->initial_geoprocess;
	kill_of_end=data->kill_of_end;

	if(data->killTimer)
		killTimer_.start(data->killTimer);

	if(data->sleep_timer)
		sleep_timer.start(data->sleep_timer);
	create_first=data->create_first;

	if(activatingUnit_ != UNIT_ATTRIBUTE_NONE)
		setActivity(false);

	{
		int size=min(data->hole_position.size(),data->hole_position_inited.size());
		for(int i=0;i<size;i++)
		{
			HoleStruct s;
			s.pos=data->hole_position[i];
			s.inited=data->hole_position_inited[i];
			hole_pos.push_back(s);
		}
	}

	FilthSwarmListType& new_swarm=swarm_list;
	SaveUnitDataList::const_iterator it;
	FOR_EACH(data->swarmList,it)
	{
		const SaveFilthSwarm* swarm_data=safe_cast<const SaveFilthSwarm*>(it->get());
		switch(FilthParamID){
			case FILTH_SPOT_ID_ANTS:
			case FILTH_SPOT_ID_ANTS2:
			case FILTH_SPOT_ID_A_ANTS:
			case FILTH_SPOT_ID_A_SPIDER:
				new_swarm.push_back(new terFilthSwarmAnt(this,swarm_data->position,0));
				break;
			case FILTH_SPOT_ID_RAT:
			case FILTH_SPOT_ID_SNAKE:
			case FILTH_SPOT_ID_A_RAT:
				new_swarm.push_back(new terFilthSwarmRat(this,swarm_data->position,0));
				break;
			case FILTH_SPOT_ID_WASP:
			case FILTH_SPOT_ID_EYE:
			case FILTH_SPOT_ID_A_WASP:
			case FILTH_SPOT_ID_A_EYE:
				{
					terFilthSwarmWasp* p = new terFilthSwarmWasp(this,swarm_data->position,0);
					new_swarm.push_back(p);
				}
				break;
			case FILTH_SPOT_ID_GHOST:
				new_swarm.push_back(new terFilthSwarmGhost(this,swarm_data->position,0));
				break;
			case FILTH_SPOT_ID_DRAGON:
			case FILTH_SPOT_ID_DRAGON2:
			case FILTH_SPOT_ID_A_DRAGON:
				{
					terFilthSwarmDragon* p = new terFilthSwarmDragon(this,swarm_data->position,0);

					if(FilthParamID==FILTH_SPOT_ID_A_DRAGON)
						p->SetPrm(&terFilthDragonPrmA);
					else
					if(FilthParamID==FILTH_SPOT_ID_DRAGON)
						p->SetPrm(&terFilthDragonPrm);
					else
						p->SetPrm(&terFilthDragonPrm2);

					new_swarm.push_back(p);
				}
				break;
			case FILTH_SPOT_ID_CROW:
			case FILTH_SPOT_ID_A_CROW:
				{
					terFilthSwarmCrow* p = new terFilthSwarmCrow(this,swarm_data->position,0);
					new_swarm.push_back(p);
				}
				break;

			case FILTH_SPOT_ID_DAEMON:
			case FILTH_SPOT_ID_A_DAEMON:
				{
					terFilthSwarmDaemon* p = new terFilthSwarmDaemon(this,swarm_data->position,0);
					new_swarm.push_back(p);
				}
				break;
			case FILTH_SPOT_ID_WORM:
			case FILTH_SPOT_ID_A_WORM:
				{
					terFilthSwarmWorm* p = new terFilthSwarmWorm(this,swarm_data->position,0);
					new_swarm.push_back(p);
				}
				break;

			case FILTH_SPOT_ID_SHARK:
				{
					terFilthSwarmShark* p = new terFilthSwarmShark(this,swarm_data->position,0);
					new_swarm.push_back(p);
				}
				break;
			case FILTH_SPOT_ID_VOLCANO:
			case FILTH_SPOT_ID_VOLCANO_SCUM_DISRUPTOR:
				{
					terFilthSwarmVolcano* p = new terFilthSwarmVolcano(this,swarm_data->position,0);
					if(FilthParamID==FILTH_SPOT_ID_VOLCANO_SCUM_DISRUPTOR)
						p->SetPrm(&terFilthVolcanoScumDisruptorPrm);
					else
						p->SetPrm(&terFilthVolcanoPrm);
					new_swarm.push_back(p);
				}
				break;
			default:
				xassert(0);
		}

		terFilthSwarm* p=new_swarm.back();
		p->SetBaseParam(BaseParam);
		p->SetAttackPlayer(attack_player);
		p->SetAttackWidth(attack_width,attack_direction);
		p->SetAttackTarget(pExclusiveAttackUnit);
		p->SetPlayer(Player);
		p->universalLoad(swarm_data);
	}

}

void terFilthSpot::Start()
{
	terUnitBase::Start();
	if(!sleep_timer.get_start_time())
	{
		sleep_timer.start(first_sleep_time*1e3f);
	}else
	{
		int k=0;
	}
}

void terFilthSpot::DestroyLink()
{
	FilthSwarmListType::iterator i_swarm;

	terUnitBase::DestroyLink();
	i_swarm = swarm_list.begin();
	while(i_swarm != swarm_list.end()){
		if((*i_swarm)->GetAlive()){
			(*i_swarm)->DestroyLink();
			++i_swarm;
		}else{
			delete (*i_swarm);
			i_swarm = swarm_list.erase(i_swarm);
		}
	}

	if(kill_of_end && swarm_list.empty())
		Kill();

	if(pExclusiveAttackUnit)
	{
		if(!pExclusiveAttackUnit->alive())
			pExclusiveAttackUnit=NULL;
	}
}


void terFilthSpot::SoundActivate()
{
	float z = (float)(vMap.GetAlt(vMap.XCYCL(round(position().x)),vMap.YCYCL(round(position().y))) >> VX_FRACTION);
	switch(FilthParamID){
		case FILTH_SPOT_ID_ANTS:
		case FILTH_SPOT_ID_A_ANTS:
			SND3DPlaySound("ant_nexus",&Vect3f(position().x,position().y,z));
			break;
		case FILTH_SPOT_ID_WASP:
		case FILTH_SPOT_ID_A_WASP:
			SND3DPlaySound("wasp_nexus",&Vect3f(position().x,position().y,z));
			break;
	}
	SoundImpulse = 1;
}

void terFilthSpot::StartSleepMode()
{
	sleep_timer.start((sleep_period + terLogicRND(delta_sleep_period))*1e3f*GetDifficitySleepMul());
	SoundImpulse = 0;
}

void terFilthSpot::GenerationProcess()
{
	FilthSwarmListType new_swarm;
	Vect3f v;
	int i;

	int num = BaseParam->SwarmMax;
	int period=round((attack_period+terLogicRND(delta_attack_period))*10.0f);

	switch(FilthParamID){
		case FILTH_SPOT_ID_ANTS:
		case FILTH_SPOT_ID_ANTS2:
		case FILTH_SPOT_ID_A_ANTS:
		case FILTH_SPOT_ID_A_SPIDER:
			for(i = 0;i < num;i++)
			{
				float a = terLogicRNDfrand()*M_PI*2.0f;
				float r = BaseParam->SwarmRadius;
				v.x = cosf(a) * r;
				v.y = sinf(a) * r;
				v += position();
				v.z = 0;
				if(v.x > 0 && v.y > 0 && v.x < vMap.H_SIZE && v.y < vMap.V_SIZE){
					if(terCheckFilthPoint(round(v.x),round(v.y))){
						terFilthSwarmAnt* p = new terFilthSwarmAnt(this,v,period);
						new_swarm.push_back(p);
					}
				}
			}
			break;
		case FILTH_SPOT_ID_RAT:
		case FILTH_SPOT_ID_SNAKE:
		case FILTH_SPOT_ID_A_RAT:
			num = max(GetDifficityCreatureNum() / 3,1);
			for(i = 0;i < num;i++)
			{
				float a = terLogicRNDfrnd()*BaseParam->SwarmRadius;
				float r = BaseParam->SwarmRadius;
				v.x = terLogicRNDfrnd()*r;
				v.y = terLogicRNDfrnd()*r;
				v += position();
				v.z = 0;
				if(v.x > 0 && v.y > 0 && v.x < vMap.H_SIZE && v.y < vMap.V_SIZE){
					if(terCheckFilthPoint(round(v.x),round(v.y))){
						terFilthSwarmRat* p = new terFilthSwarmRat(this,v,period);
						new_swarm.push_back(p);
					}
				}
			}
			break;
		case FILTH_SPOT_ID_WASP:
		case FILTH_SPOT_ID_EYE:
		case FILTH_SPOT_ID_A_EYE:
		case FILTH_SPOT_ID_A_WASP:
			{
				if(hole_pos.empty())
				{
					float a = terLogicRNDfrand()*M_PI*2.0f;
					for(int i=0;i<BaseParam->SwarmMax;i++)
					{
						HoleStruct v;
						v.inited=false;
						float r = BaseParam->SwarmRadius;
						v.pos.x =position().x+terLogicRNDfrnd() * r;
						v.pos.y =position().y+terLogicRNDfrnd() * r;
						hole_pos.push_back(v);
					}
				}

				terFilthSwarmWasp* p = new terFilthSwarmWasp(this,position(),period);
				p->SetHole(hole_pos);
				if(FILTH_SPOT_ID_A_WASP==FilthParamID)
					p->SetPrm(UNIT_ATTRIBUTE_FILTH_A_WASP);
				else
				if(FILTH_SPOT_ID_A_EYE==FilthParamID)
					p->SetPrm(UNIT_ATTRIBUTE_FILTH_A_EYE);
				else
				if(FILTH_SPOT_ID_EYE==FilthParamID)
					p->SetPrm(UNIT_ATTRIBUTE_FILTH_WASP_EYE);
				else
					p->SetPrm(UNIT_ATTRIBUTE_FILTH_WASP);

				new_swarm.push_back(p);
			}
			break;
		case FILTH_SPOT_ID_GHOST:
				v = position();
				v.z = 0;
				if(v.x > 0 && v.y > 0 && v.x < vMap.H_SIZE && v.y < vMap.V_SIZE){
					if(terCheckFilthPoint(round(v.x),round(v.y))){
						terFilthSwarmGhost* p = new terFilthSwarmGhost(this,v,period);
						new_swarm.push_back(p);
					}
				}
			break;
/*
		case FILTH_SPOT_ID_EYE:
			{
				v = position();
				v.z = 0;
				if(v.x > 0 && v.y > 0 && v.x < vMap.H_SIZE && v.y < vMap.V_SIZE){
					if(terCheckFilthPoint(round(v.x),round(v.y))){
						terFilthSwarmEye* p = new terFilthSwarmEye(this,v,period);
						new_swarm.push_back(p);
					}
				}
			}
			break;
*/
		case FILTH_SPOT_ID_DRAGON:
		case FILTH_SPOT_ID_DRAGON2:
		case FILTH_SPOT_ID_A_DRAGON:
			{
				terFilthSwarmDragon* p = new terFilthSwarmDragon(this,position(),period);

				if(FilthParamID==FILTH_SPOT_ID_A_DRAGON)
					p->SetPrm(&terFilthDragonPrmA);
				else
				if(FilthParamID==FILTH_SPOT_ID_DRAGON)
					p->SetPrm(&terFilthDragonPrm);
				else
					p->SetPrm(&terFilthDragonPrm2);

				new_swarm.push_back(p);
			}
			break;
		case FILTH_SPOT_ID_CROW:
		case FILTH_SPOT_ID_A_CROW:
			{
				v = position();
				terFilthSwarmCrow* p = new terFilthSwarmCrow(this,v,period);
				new_swarm.push_back(p);
			}
			break;

		case FILTH_SPOT_ID_DAEMON:
		case FILTH_SPOT_ID_A_DAEMON:
			{
				v = position();
				v.z = 0;
				if(terCheckFilthPoint(round(v.x),round(v.y))){
					terFilthSwarmDaemon* p = new terFilthSwarmDaemon(this,v,period);
					new_swarm.push_back(p);
				}
			}
			break;
		case FILTH_SPOT_ID_WORM:
		case FILTH_SPOT_ID_A_WORM:
			{
				v = position();
				v.z = 0;
				if(terCheckFilthPoint(round(v.x),round(v.y))){
					terFilthSwarmWorm* p = new terFilthSwarmWorm(this,v,period);
					new_swarm.push_back(p);
				}
			}
			break;

		case FILTH_SPOT_ID_SHARK:
			{
				v = position();
				v.z = 0;
				if(terCheckFilthPoint(round(v.x),round(v.y))){
					terFilthSwarmShark* p = new terFilthSwarmShark(this,v,period);
					new_swarm.push_back(p);
				}
			}
			break;
		case FILTH_SPOT_ID_VOLCANO:
		case FILTH_SPOT_ID_VOLCANO_SCUM_DISRUPTOR:
			{
				v = position();
				v.z = 0;
				creature_generation_period=attack_period*10.0f;
				if(terCheckFilthPoint(round(v.x),round(v.y))){
					SND3DPlaySound("Proc_Geo_Vulcan",&To3D(v));
					terFilthSwarmVolcano* p = new terFilthSwarmVolcano(this,v,create_first);
					if(FilthParamID==FILTH_SPOT_ID_VOLCANO_SCUM_DISRUPTOR)
						p->SetPrm(&terFilthVolcanoScumDisruptorPrm);
					else
						p->SetPrm(&terFilthVolcanoPrm);
					new_swarm.push_back(p);
				}
			}
			break;
		default:
			xassert(0);
	}

	if(!new_swarm.empty())
	{
		FilthSwarmListType::iterator it;
		int size=new_swarm.size();
		int creatures=GetDifficityCreatureNum();
		FOR_EACH(new_swarm,it)
		{
			(*it)->SetBaseParam(BaseParam);
			(*it)->SetAttackPlayer(attack_player);
			(*it)->SetAttackWidth(attack_width,attack_direction);
			(*it)->SetAttackTarget(pExclusiveAttackUnit);

			int num=round(creatures/(float)size);

			(*it)->SetCreatureGeneration(num,creature_generation_period);
			(*it)->SetPlayer(Player);

			creatures-=num;
			size--;
		}
	}

	if(!new_swarm.empty() && !SoundImpulse)
		SoundActivate();

	swarm_list.insert(swarm_list.end(),new_swarm.begin(),new_swarm.end());
	create_first=false;

	if(kill_after_generate)
		KillOfEnd();
}

void terFilthSpot::MoveQuant()
{
	terUnitBase::MoveQuant();

#ifndef _FINAL_VERSION_
	if(check_command_line("noFilth") || gameShell->missionEditor())
	{
		return;
	}
#endif

	FilthSwarmListType::iterator i_swarm;
	FOR_EACH(swarm_list,i_swarm){
		(*i_swarm)->Quant();
	}

	if(activity()){
		if(!sleep_timer() && !kill_of_end){
			GenerationProcess();
			StartSleepMode();
		}
	}
	else{
		if(!sleep_timer()){
			sleep_timer.start(1000 + terLogicRND(1000));
			PlayerVect::iterator pi;
			FOR_EACH(universe()->Players, pi){
				terUnitBase* unit = (*pi)->findUnit(activatingUnit_, position2D());
				if(unit && position2D().distance2(unit->position2D()) < sqr(activatingDistance_)){
					setActivity(true);
					break;
				}
			}
		}
	}

	if(alive() && killTimer_.was_started() && !killTimer_())
		SetFreeDestroy();
}

void terFilthSpot::setActivity(bool activate) 
{ 
	sleep_ = !activate; 
	if(activate){
		sleep_timer.start(first_sleep_time*1000); 
		universe()->checkEvent(EventActivateSpot(false));
	}
} 

bool terFilthSpot::IsAttackOtherSwarm(terUnitBase* unit,terFilthSwarm* swarm)
{
	if(FilthParamID==FILTH_SPOT_ID_ANTS || FilthParamID==FILTH_SPOT_ID_A_ANTS)
		return false;

	FilthSwarmListType::iterator it;
	FOR_EACH(swarm_list,it)
	{
		if(swarm==*it)
			continue;
		if((*it)->GetTarget()==unit)
			return true;
	}

	return false;
}

void terFilthSpot::setParameters(const terFilthSpotParameters* prm)
{
	SetFilthParamID(prm->spotID);

	first_sleep_time = prm->firstSleepTime;

	sleep_period = prm->sleepPeriod;
	delta_sleep_period = prm->sleepPeriodDelta;

	attack_period = prm->attackPeriod;
	delta_attack_period = prm->attackPeriodDelta;

	attack_width = prm->attackWidth;
	attack_direction = prm->attackDirection;

	creature_num = prm->creatureNum;
	creature_generation_period = prm->creatureGenerationPeriod;


	if(prm->lifeTime > FLT_EPS)
		killTimer_.start(prm->lifeTime*1e3f);
	attack_player=FILTH_ATTACK_NOSELF_NOWORLD;
	enable_zeroplast_generate=true;
	initial_geoprocess=false;
	harkback=true;
}

void terFilthSpot::GetDifficity(int& creature_num_,float& sleep_mul)
{
	float difficity=1;
	if(!enable_zeroplast_generate)
		difficity=universe()->worldPlayer()->difficultyPrm().filthDensity;
	xassert(creature_num>0);
	xassert(difficity>0 && difficity<10);
	creature_num_=max(round(creature_num*difficity),1);
	sleep_mul=max(creature_num_/(creature_num*difficity),1.0f);
}

int terFilthSpot::GetDifficityCreatureNum()
{
	int creature_n;
	float sleep_mul;
	GetDifficity(creature_n,sleep_mul);
	return creature_n;
}

float terFilthSpot::GetDifficitySleepMul()
{
	int creature_n;
	float sleep_mul;
	GetDifficity(creature_n,sleep_mul);
	return sleep_mul;
}

//----------------------------------------------------------

void terFilthSpot::ShowInfo()
{
	if(gameShell->missionEditor())
	{
		Vect3f e,w;
		terCamera->GetCamera()->ConvertorWorldToViewPort(&position(),&w,&e);
		terRenderDevice->SetFont(gameShell->debugFont());
		if(selected())
		{
			terRenderDevice->OutText(round(e.x),round(e.y),"Filth",sColor4f(1.0f,1.0f,1.0f,1.0f));
			terRenderDevice->DrawRectangle(round(e.x) - 2,round(e.y) - 2,4,4,sColor4c(255,255,255,255),0);

			if(attack_width>0)
			{
				float a=attack_direction*M_PI/180.0f;
				Vect3f direction(cos(a),sin(a),0);
				Vect3f norm(direction.y,-direction.x,0);
				terRenderDevice->DrawLine(To3D(position()),To3D(position()+direction*20),RED);

				for(int i=0;i<2;i++)
				{
					Vect3f pos=position()+norm*((i==0?1:-1)*attack_width);
					terRenderDevice->DrawLine(To3D(pos),To3D(pos+direction*10),RED);
				}
			}
		}else
		{
			terRenderDevice->OutText(round(e.x),round(e.y),"Filth",sColor4f(1.0f,1.0f,1.0f,0.5f));
			terRenderDevice->DrawRectangle(round(e.x) - 2,round(e.y) - 2,4,4,pathColor,0);
		}
		terRenderDevice->SetFont(0);
	}
}


void terFilthSpot::SetFilthParamID(terFilthSpotID id)
{
	FilthParamID = id;
	switch(FilthParamID)
	{
		case FILTH_SPOT_ID_ANTS:
		case FILTH_SPOT_ID_A_ANTS:
			BaseParam = &terFilthAntBaseParameter;
			break;
		case FILTH_SPOT_ID_WASP:
		case FILTH_SPOT_ID_A_WASP:
			BaseParam = &terFilthWaspBaseParameter;
			break;
		case FILTH_SPOT_ID_DRAGON:
		case FILTH_SPOT_ID_A_DRAGON:
			BaseParam = &terFilthDragonBaseParameter;
			break;
		case FILTH_SPOT_ID_GHOST:
			BaseParam = &terFilthGhostBaseParameter;
			break;
		case FILTH_SPOT_ID_EYE:
		case FILTH_SPOT_ID_A_EYE:
			BaseParam = &terFilthEyeBaseParameter;
			break;
		case FILTH_SPOT_ID_CROW:
		case FILTH_SPOT_ID_A_CROW:
			BaseParam = &terFilthCrowBaseParameter;
			break;
		case FILTH_SPOT_ID_DAEMON:
		case FILTH_SPOT_ID_A_DAEMON:
			BaseParam = &terFilthDaemonBaseParameter;
			break;
		case FILTH_SPOT_ID_RAT:
		case FILTH_SPOT_ID_A_RAT:
			BaseParam = &terFilthRatBaseParameter;
			break;
		case FILTH_SPOT_ID_WORM:
		case FILTH_SPOT_ID_A_WORM:
			BaseParam = &terFilthWormBaseParameter;
			break;
		case FILTH_SPOT_ID_SHARK:
			BaseParam = &terFilthSharkBaseParameter;
			break;
		case FILTH_SPOT_ID_VOLCANO_SCUM_DISRUPTOR:
		case FILTH_SPOT_ID_VOLCANO:
			BaseParam = &terFilthVolcanoBaseParameter;
			break;
		case FILTH_SPOT_ID_ANTS2:
		case FILTH_SPOT_ID_A_SPIDER:
			BaseParam = &terFilthAnt2BaseParameter;
			break;
		case FILTH_SPOT_ID_SNAKE:
			BaseParam = &terFilthSnakeBaseParameter;
			break;
		case FILTH_SPOT_ID_DRAGON2:
			BaseParam = &terFilthDragonBaseParameter2;
			break;
		default:
			xassert(0);
	}

	creature_num=BaseParam->CreatureNum;
	creature_generation_period=BaseParam->GenerationPeriod;
}

//---------------------------------------

terFilthGeneric::terFilthGeneric(const UnitTemplate& data) : terUnitReal(data)
{
	is_fitrst_quant=true;
	cluster_colliding=false;
	cur_cluster=0;
	attack_period=INT_MAX;
}

bool terFilthGeneric::clusterColliding()
{
	return cluster_colliding;
}

void terFilthGeneric::SetAttackPeriod(int attack_period_)
{
	attack_period=attack_period_+terLogicRND(50);
}

void terFilthGeneric::Quant()
{
	terUnitReal::Quant();

	int cluster;
	if(position().z<vMap.hZeroPlast)
		cluster=0;
	else
		cluster=field_dispatcher->getIncludingCluster(position());
	
	if(!is_fitrst_quant)
	{
		cluster_colliding=	cur_cluster!=cluster;
	}

	cur_cluster=cluster;

	is_fitrst_quant=false;

	if(attack_period<=0)
		SetFreeDestroy();
	attack_period--;
}

void terFilthGeneric::AvatarQuant()
{
	terUnitReal::AvatarQuant();
	realAvatar()->setFreeze(FreezeFactor);
	realAvatar()->setHot(HotFactor);
}

void terFilthGeneric::Collision(terUnitBase* p)
{
	if(isEnemy(p) /*&& IsAttackable(p)*/)
	{
		p->setDamage(damageData(),this);
		damageMoleculaKill();
	}
}

void terFilthGeneric::FieldEffect()
{
	EffectLibrary* lib=gb_VisGeneric->GetEffectLibrary("filth");
	EffectKey* key=lib->Get("field");
	xassert(key);
	cEffect* effect=terScene->CreateEffect(*key,NULL,1.0f,true);
	effect->SetPosition(pose());
}

///////////////////////////terFilthSwarm//////////////////////////
terFilthSwarm::terFilthSwarm(terFilthSpot* spot, const Vect3f& pos)
{ 
	Alive = 1;
	SpotPoint = spot; 
	position = pos;

	TargetPoint = 0; 
	attack_width=-1;
	attack_direction=0;
	player=NULL;
	attribute=NULL;
	attack_player=FILTH_ATTACK_ALL;
	find_attack_target_point=false;
}

void terFilthSwarm::SetPlayer(terPlayer* player_)
{
	player=player_;
	attribute=player->unitAttribute(GetUnitID());
}

void terFilthSwarm::DestroyLink()
{
	if(SpotPoint && !(SpotPoint->alive()))
		SpotPoint = NULL;
	if(TargetPoint && !(TargetPoint->alive()))
		TargetPoint = NULL;
}

void terFilthSwarm::SetAttackTarget(terUnitBase* AttackUnit)
{
	TargetPoint=AttackUnit;
	find_attack_target_point=true;
}

terUnitBase* terFilthSwarm::CalcDistance(terUnitBase* unit,float& dist,const Vect2f& direction)
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

	if(!IsAttackable(unit) )
		return NULL;

	float d = unit->position().distance2(position);
	if(d < dist)
	{
		if(SpotPoint && SpotPoint->IsAttackOtherSwarm(unit,this))
			return NULL;

		dist = d;
		return unit;
	}

	return NULL;
}

void terFilthSwarm::FindComplexTarget(list<terUnitBase*>& target_list,int max_target,UnitSet* exclude)
{
	if(!BaseParam)
		return;

	int target_list_size=0;
	float a=attack_direction*M_PI/180.0f;
	Vect2f direction(cos(a),sin(a));

	//На фрэйм нападать только, если нет других целей.
	float md = FLT_MAX;
	PlayerVect::iterator pi;

	for(int iteration=0;iteration<2;iteration++)
	{
		DWORD ignore_unit_class=(iteration==0)?UNIT_CLASS_FRAME:~(DWORD)UNIT_CLASS_FRAME;
		FOR_EACH(universe()->Players, pi)
		{
			terPlayer* player=*pi;
			switch(attack_player)
			{
			case FILTH_ATTACK_ALL:
				break;
			case FILTH_ATTACK_PLAYER:
				if(player->isAI())
					continue;
				if(player->isWorld())
					continue;
				break;
			case FILTH_ATTACK_AI:
				if(!player->isAI())
					continue;
				break;
			case FILTH_ATTACK_NOSELF:
				if(player==terFilthSwarm::player)
					continue;
				break;
			case FILTH_ATTACK_NOSELF_NOWORLD:
				if(player==terFilthSwarm::player)
					continue;
				if(player->isWorld())
				{
					ignore_unit_class=~(DWORD)UNIT_CLASS_STRUCTURE_ENVIRONMENT;
				}
				break;
			default:
				xassert(0);
			}

			const UnitList& unit_list=player->units();
			UnitList::const_iterator it_unit;
			FOR_EACH(unit_list,it_unit)
			{
				terUnitBase* unit=*it_unit;
				if(unit->unitClass() & ignore_unit_class)
					continue;
				if(exclude && exclude->find(unit)!=exclude->end())
					continue;


				if(CalcDistance(unit,md,direction))
				{
					target_list.push_front(unit);
					if(target_list_size>=max_target)
						target_list.pop_back();
					else
						target_list_size++;
				}
			}
		}

		if(!target_list.empty())
			break;
	}
}

void terFilthSwarm::FindTargetPoint(UnitSet* exclude)
{
	if(find_attack_target_point && TargetPoint)
	{
		find_attack_target_point=false;
		return;
	}

	list<terUnitBase*> target_list;
	FindComplexTarget(target_list,1,exclude);
	if(!target_list.empty())
		TargetPoint=target_list.front();
	else
		TargetPoint=NULL;
}

void terFilthSwarm::playDetectedSound(bool geo_mode)
{
	if(!soundTimer_){
		if(geo_mode)
			universe()->soundEvent(SOUND_VOICE_GEOACTIVITY_DETECTED);
		else
			universe()->soundEvent(SOUND_VOICE_FILTH_DETECTED);

		soundTimer_.start(10000);
	}
}

////////////////////////////terSplineController/////////////////////
terSplineController::terSplineController()
{
	delta_time=0.05f;
	cur_way_point=0;
	acceleration=Vect3f::ZERO;
	auto_front=auto_up=true;
	damphing=0.1f;
	k_acceleration=4e-2f;
	interpolation_linear=false;
	user_up=Vect3f::ZERO;
	user_front=Vect3f::ZERO;
}

Vect3f terSplineController::getLastDirection2()
{
	Vect3f n;

	int size=way_points.size();
	if(size>=2)
	{
		WayPoints::iterator it=way_points.end();
		it--;
		it--;
		n=way_points.back()-*it;
		n.z=0;
	}
	
	if(size<2 || n.norm()<0.1f)
	{
		Mat3f p(GetPose().rot());
		n=p*Vect3f(0,1,0);
		n.z=0;
	}

	if(n.norm()<0.1f)
		n.set(0,1,0);
	n.Normalize();
	return n;
}

Se3f terSplineController::quantPosition(float dt)
{
	if(way_points.empty())
		return GetPose();
	Se3f pos;
	Vect3f p[4];
	GetSplineParameter(p);

	if(interpolation_linear)
	{
		xassert(!auto_front);
		xassert(!auto_up);
		pos.trans()=p[1]*(1-cur_way_point)+p[2]*cur_way_point;
	}else
	{
		pos.trans()=HermitSpline(cur_way_point,p[0],p[1],p[2],p[3]);
	}

	Vect3f up,front;

	if(auto_front)
	{
		Vect3f line=HermitSplineDerivation(cur_way_point,p[0],p[1],p[2],p[3]);
		if(sqr(line.x)+sqr(line.y)<FLT_EPS)
			line.set(0,1,0);
		front=Normalize(line);
	}else
	{
		front=user_front;
	}

	Mat3f rotate;
	if(auto_up)
	{
		Vect3f line2=HermitSplineDerivation2(cur_way_point,p[0],p[1],p[2],p[3]);
		Vect3f a=(line2-(line2.dot(front))*front)*k_acceleration;
		a.z=0;//Учитывать только горизонтальную составляющую
		acceleration=acceleration*(1-damphing)+a*damphing;

		up=Normalize(Vect3f(0,0,1)+acceleration);
		rotate.setYcol(front);
		rotate.setXcol(Normalize(rotate.ycol()%up));
		rotate.setZcol(rotate.xcol()%rotate.ycol());
	}else
	{
		up=user_up;
		rotate.setZcol(up);
		rotate.setXcol(front%rotate.zcol());
		rotate.setYcol(rotate.zcol()%rotate.xcol());
	}

//	show_vector(pos.trans(),rotate.xcol(),RED);
//	show_vector(pos.trans(),rotate.ycol(),GREEN);
//	show_vector(pos.trans(),rotate.zcol(),CYAN);
	float det=rotate.det();
	xassert(det>0 && det<2);
	pos.rot().set(rotate);

	cur_way_point+=dt;
	if(cur_way_point>1)
	{
		cur_way_point-=1;
		way_points.erase(way_points.begin());
	}

	return pos;
}

void terSplineController::GetSplineParameter(Vect3f p[4])
{
	if(way_points.empty())
	{
		p[0]=p[1]=p[2]=p[3]=GetPose().trans();
		return;
	}

	WayPoints::iterator it=way_points.begin();
	p[0]=*it;
	it++;
	for(int i=1;i<4;i++)
	{
		if(it!=way_points.end())
			p[i]=*it++;
		else
			p[i]=p[i-1];
	}
}

////////////////////////////terFilthSpline//////////////////////////
terFilthSpline::terFilthSpline(const UnitTemplate& data)
:terFilthGeneric(data)
{
}

void terFilthSpline::AvatarQuant()
{
	terFilthGeneric::AvatarQuant();
	avatar()->setPose(pose());
	
	if(way_points.size()<4)
		addWayPoint();

	if(show_filth_debug)
	{
		WayPoints::iterator it;
		FOR_EACH(way_points,it)
		{
			show_vector(*it,2,GREEN);
		}
	}
}

void terFilthSpline::MoveQuant()
{
	Se3f pos=quantPosition(delta_time);
	terFilthGeneric::setPose(pos, false);
}

SaveUnitData* terFilthSpline::universalSave(SaveUnitData* baseData)
{
	SaveFilthSpline* data = castOrCreate<SaveFilthSpline>(baseData);
	terFilthGeneric::universalSave(data);

	data->delta_time=delta_time;
	data->cur_way_point=cur_way_point;
	data->acceleration=acceleration;
	data->k_acceleration=k_acceleration;

	data->damphing=damphing;
	data->auto_up=auto_up;
	data->auto_front=auto_front;
	data->user_up=user_up;
	data->user_front=user_front;
	data->interpolation_linear=interpolation_linear;

	WayPoints::iterator it;
	FOR_EACH(way_points,it)
	{
		data->way_points.push_back(*it);
	}

	return data;
}

void terFilthSpline::universalLoad(const SaveUnitData* baseData)
{
	terFilthGeneric::universalLoad(baseData);
	const SaveFilthSpline* data = safe_cast<const SaveFilthSpline*>(baseData);
	delta_time=data->delta_time;
	cur_way_point=data->cur_way_point;
	acceleration=data->acceleration;
	k_acceleration=data->k_acceleration;

	damphing=data->damphing;
	auto_up=data->auto_up;
	auto_front=data->auto_front;
	user_up=data->user_up;
	user_front=data->user_front;
	interpolation_linear=data->interpolation_linear;

	way_points.clear();
	vector<Vect3f>::const_iterator it;
	FOR_EACH(data->way_points,it)
	{
		way_points.push_back(*it);
	}
}

//////////////////////////////////terGenerate////////////////
void terGenerate::Set(int creature_num,int generation_period)
{
	GenerationCount = generation_period;
	if(GenerationCount > 0)
		GenerationSpeed = (float)(creature_num) / (float)(GenerationCount);
	else
		GenerationSpeed = 0;
	GenerationFactor = 0;
}

int terGenerate::Quant()
{
	if(!InProcess())
		return 0;
	GenerationCount--;
	GenerationFactor += GenerationSpeed;
	int num;
	if(GenerationCount)
		num=round(floor(GenerationFactor));
	else
		num=round(GenerationFactor);
	GenerationFactor -= (float)num;
	return num;
}

void terGenerate::Save(SaveTerGenerate& data)
{
	data.GenerationCount=GenerationCount;
	data.GenerationFactor=GenerationFactor;
	data.GenerationSpeed=GenerationSpeed;
}

void terGenerate::Load(const SaveTerGenerate& data)
{
	GenerationCount=data.GenerationCount;
	GenerationFactor=data.GenerationFactor;
	GenerationSpeed=data.GenerationSpeed;
}

SaveUnitData* terFilthSwarm::universalSave(SaveUnitData* baseData)
{
	SaveFilthSwarm* data = castOrCreate<SaveFilthSwarm>(baseData);
	data->position=position;
	data->attack_width=attack_width;
	data->attack_direction=attack_direction;
	data->attack_player=attack_player;
	return data;
}

void terFilthSwarm::universalLoad(const SaveUnitData* baseData)
{
	const SaveFilthSwarm* data = safe_cast<const SaveFilthSwarm*>(baseData);
	position=data->position;
	attack_width=data->attack_width;
	attack_direction=data->attack_direction;
	attack_player=data->attack_player;
}
