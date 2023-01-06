#include "StdAfx.h"

#include "Runtime.h"
#include "Universe.h"

#include "Config.h"
#include "ForceField.h"
#include "GenericFilth.h"
#include "Region.h"
#include "IronFrame.h"

#include "CameraManager.h"
#include "Triggers.h"

#include "PerimeterSound.h"
#include "GameShell.h"

#include "GeoControl.h"
#include "codepages/codepages.h"

terGeoControl::terGeoControl(const UnitTemplate& data):terUnitBase(data)
{
	sleep_ = false;
	first_sleep_time=0;
	sleep_period=30;
	delta_sleep_period=0;
	attack_period=10;
	delta_attack_periond=0;
	activatingUnit_ = UNIT_ATTRIBUTE_NONE;
	activatingDistance_ = 100;
	attack_on=false;
}

terGeoControl::~terGeoControl()
{
}


SaveUnitData* terGeoControl::universalSave(SaveUnitData* baseData)
{
	SaveGeoControl* data = castOrCreate<SaveGeoControl>(baseData);
	terUnitBase::universalSave(data);
	data->sleep = sleep_;
	data->firstSleepTime=first_sleep_time;
	data->sleepPeriod=sleep_period;
	data->deltaSleepPeriod=delta_sleep_period;
	data->attackPeriond=attack_period;
	data->deltaAttackPeriond=delta_attack_periond;
	data->activatingUnit = activatingUnit_;
	data->activatingDistance = activatingDistance_;
	return data;
}

void terGeoControl::universalLoad(SaveUnitData* baseData)
{
	SaveGeoControl* data = safe_cast<SaveGeoControl*>(baseData);
	terUnitBase::universalLoad(data);

	sleep_ = data->sleep;
	first_sleep_time=data->firstSleepTime;
	sleep_period=data->sleepPeriod;
	delta_sleep_period=data->deltaSleepPeriod;
	attack_period=data->attackPeriond;
	delta_attack_periond=data->deltaAttackPeriond;
	activatingUnit_ = data->activatingUnit;
	activatingDistance_ = data->activatingDistance;
	if(activatingUnit_ != UNIT_ATTRIBUTE_NONE)
		setActivity(false);
}

void terGeoControl::Start()
{
	terUnitBase::Start();
	sleep_timer.start(first_sleep_time*1e3f);
	if(attr()->SoundName[0] && attr()->SoundCycled)
	{
		sound.Init(attr()->SoundName);
		sound.SetPos(To3D(position()));
	}
}

void terGeoControl::Stop()
{
	sound.Stop();
}

void terGeoControl::Quant()
{
	if(gameShell->missionEditor())
		return;
	terUnitBase::Quant();

	if(attack_on){
		if(!attack_timer()){
			attack_on=false;
			Stop();
		}
	}
	else{
		if(activity()){
			if(!sleep_timer()){
				float attack_time=(attack_period+terLogicRND(delta_attack_periond))*1e3f;

				if(attr()->SoundCycled)
				{
					sound.Play();
				}else
				{
					if(attr()->SoundName[0]) {
                        Vect3f p = To3D(position());
                        SND3DPlaySound(attr()->SoundName, &p);
                    }
				}

				Generate(attack_time);
				attack_on=true;
				sleep_timer.start(attack_time+(sleep_period + terLogicRND(delta_sleep_period))*1e3f);
				attack_timer.start(attack_time);
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
	}
}

void terGeoControl::setActivity(bool activate) 
{ 
	sleep_ = !activate; 
	if(activate){
		sleep_timer.start(first_sleep_time*1000);
        EventActivateSpot ev(false);
		universe()->checkEvent(&ev);
	}
} 

void terGeoControl::DestroyLink()
{
	terUnitBase::DestroyLink();
}


void terGeoControl::ShowInfo()
{
	if(gameShell->missionEditor())
	{
		Vect3f e,w;
		terCamera->GetCamera()->ConvertorWorldToViewPort(&position(),&w,&e);
		terRenderDevice->SetFont(gameShell->debugFont());
        std::string text;
        const std::string& locale = getLocale();
        if (startsWith(locale, "russian")) {
            text = convertToCodepage("Гео: ", locale);
            text += convertToCodepage(getEnumDescriptor(UNIT_ATTRIBUTE_NONE).nameAlt(attr()->ID), locale);
            text += "\n";
        } else {
            text = "Geo:\n";
        }
        text += getEnumDescriptor(UNIT_ATTRIBUTE_NONE).name(attr()->ID);
		if(selected())
		{
			terRenderDevice->OutText(xm::round(e.x), xm::round(e.y), text.c_str(), sColor4f(1.0f, 1.0f, 1.0f, 1.0f));
			terRenderDevice->DrawRectangle(xm::round(e.x) - 2, xm::round(e.y) - 2, 4, 4, sColor4c(255, 255, 255, 255), 0);
		}else
		{
			terRenderDevice->OutText(xm::round(e.x), xm::round(e.y), text.c_str(), sColor4f(1.0f, 1.0f, 1.0f, 0.5f));
			terRenderDevice->DrawRectangle(xm::round(e.x) - 2, xm::round(e.y) - 2, 4, 4, pathColor, 0);
		}
		terRenderDevice->SetFont(0);
	}
}

////////////////////////terGeoInfluence////////////////////
terGeoInfluence::terGeoInfluence(const UnitTemplate& data)
:terGeoControl(data)
{
	mount=NULL;
	setRadius(8);
}

terGeoInfluence::~terGeoInfluence()
{
	delete mount;
}

SaveUnitData* terGeoInfluence::universalSave(SaveUnitData* baseData)
{
	SaveGeoInfluence* data = castOrCreate<SaveGeoInfluence>(baseData);
	terGeoControl::universalSave(data);
	data->geoRadius = radius();
	return data;
}

void terGeoInfluence::universalLoad(SaveUnitData* baseData)
{
	SaveGeoInfluence* data = safe_cast<SaveGeoInfluence*>(baseData);
	terGeoControl::universalLoad(data);
	setRadius(data->geoRadius);
}


void terGeoInfluence::Quant()
{
	if(gameShell->missionEditor())
		return;
	terGeoControl::Quant();
	if(mount)
	{
		mount->quant();
	}
}

void terGeoInfluence::Generate(float time)
{
	if(mount)
		return;

	mount=new CGeoInfluence(xm::round(position().x - radius()), xm::round(position().y - radius()), radius() * 2, radius() * 2);
}

void terGeoInfluence::Stop()
{
	terGeoControl::Stop();
	if(mount)
	{
		delete mount;
		mount=NULL;
	}
}

//////////////////////////terGeoBreak///////////////////////////
terGeoBreak::terGeoBreak(const UnitTemplate& data)
:terGeoControl(data)
{
	mount=0;
	setRadius(8);
	num_break=0;
}

terGeoBreak::~terGeoBreak()
{
	delete mount;
}

SaveUnitData* terGeoBreak::universalSave(SaveUnitData* baseData)
{
	SaveGeoBreak* data = castOrCreate<SaveGeoBreak>(baseData);
	terGeoControl::universalSave(data);
	data->geoRadius = radius();
	data->num_break = num_break;
	return data;
}

void terGeoBreak::universalLoad(SaveUnitData* baseData)
{
	SaveGeoBreak* data = safe_cast<SaveGeoBreak*>(baseData);
	terGeoControl::universalLoad(data);
	setRadius(data->geoRadius);
	num_break = data->num_break;
}

void terGeoBreak::Quant()
{
	if(gameShell->missionEditor())
		return;
	terGeoControl::Quant();
	if(mount)
	{
		mount->quant();
	}
}

void terGeoBreak::Generate(float time)
{
	if(mount)
		return;

	mount=new geoBreak1(xm::round(position().x - radius()), xm::round(position().y - radius()), radius(), num_break);
}

void terGeoBreak::Stop()
{
	terGeoControl::Stop();
	if(mount)
	{
		delete mount;
		mount=NULL;
	}
}


//////////////////////////terGeoFault///////////////////////////
terGeoFault::terGeoFault(const UnitTemplate& data)
:terGeoControl(data)
{
	mount=0;
	length=1000;
	angle=0;
}

terGeoFault::~terGeoFault()
{
	delete mount;
}

SaveUnitData* terGeoFault::universalSave(SaveUnitData* baseData)
{
	SaveGeoFault* data = castOrCreate<SaveGeoFault>(baseData);
	terGeoControl::universalSave(data);
	data->length=length;
	data->angle=angle;
	return data;
}

void terGeoFault::universalLoad(SaveUnitData* baseData)
{
	SaveGeoFault* data = safe_cast<SaveGeoFault*>(baseData);
	terGeoControl::universalLoad(data);
	length=data->length;
	angle=data->angle;
}

void terGeoFault::Quant()
{
	if(gameShell->missionEditor())
		return;
	terGeoControl::Quant();
	if(mount)
	{
		mount->quant();
	}
}

void terGeoFault::Generate(float time)
{
	if(mount)
		return;

	Vect2f p(xm::round(position().x), xm::round(position().y));
	mount=new sGeoFault(p,angle*XM_PI/180.0f,length);
}

void terGeoFault::Stop()
{
	terGeoControl::Stop();
	if(mount)
	{
		delete mount;
		mount=NULL;
	}
}

void terGeoFault::ShowInfo()
{
	terGeoControl::ShowInfo();
	if(gameShell->missionEditor())
	{
		if(selected())
		{
			float a=angle*XM_PI/180.0f;
			Mat3f mat(a,Z_AXIS);

			const float len=30;
			Vect3f p0=To3D(position());
			Vect3f p1=p0+mat*Vect3f(len,0,0);
			Vect3f pp=p0+mat*Vect3f(len*0.7f,len*0.2f,0);
			Vect3f pm=p0+mat*Vect3f(len*0.7f,-len*0.2f,0);
			terRenderDevice->DrawLine(p0,p1,RED);
			terRenderDevice->DrawLine(p1,pp,RED);
			terRenderDevice->DrawLine(p1,pm,RED);
		}
	}
}

////////////////////s_HeadGeoAction////////////////
terGeoHead::terGeoHead(const UnitTemplate& data)
:terGeoControl(data)
{
	mount=0;
	setRadius(1);
}

terGeoHead::~terGeoHead()
{
	delete mount;
}

SaveUnitData* terGeoHead::universalSave(SaveUnitData* baseData)
{
	SaveGeoBreak* data = castOrCreate<SaveGeoBreak>(baseData);
	terGeoControl::universalSave(data);
	data->geoRadius = radius();
	return data;
}

void terGeoHead::universalLoad(SaveUnitData* baseData)
{
	SaveGeoBreak* data = safe_cast<SaveGeoBreak*>(baseData);
	terGeoControl::universalLoad(data);
	setRadius(data->geoRadius);
}

void terGeoHead::Quant()
{
	if(gameShell->missionEditor())
		return;
	terGeoControl::Quant();
	if(mount)
	{
		mount->quant(xm::round(position().x), xm::round(position().y));
	}
}

void terGeoHead::Generate(float time)
{
	if(mount)
		return;

	mount=new s_HeadGeoAction;
	mount->init(xm::round(position().x), xm::round(position().y), radius());
}

void terGeoHead::Stop()
{
	terGeoControl::Stop();
	if(mount)
	{
		delete mount;
		mount=NULL;
	}
}
