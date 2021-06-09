#include "StdAfx.h"
#include "ForceField.h"
#include "Runtime.h"
#include "terra.h"
#include "Region.h"
#include "CameraManager.h"
#include "GenericControls.h"
#include "Universe.h"
#include "Config.h"

#include "TerraInterface.inl"
class PerimeterTerraInterface:public StandartTerraInterface
{
public:
	virtual class Column* GetColumn(int player)
	{
		terUniverse* tu=universe();
		xassert(player>=0 && player<tu->Players.size());
		return &tu->Players[player]->energyRegion().getEditColumn();
	}

	virtual void GetBorder(int player,borderCall call,void* data)
	{
		terUniverse* tu=universe();
		xassert(player>=0 && player<tu->Players.size());
		tu->Players[player]->energyRegion().getBorder(call,data,true);
	}

	virtual void LockColumn()
	{
		terUniverse* tu=universe();
		tu->EnergyRegionLocker()->Lock();
	}
	virtual void UnlockColumn()
	{
		terUniverse* tu=universe();
		tu->EnergyRegionLocker()->Unlock();
	}
};

TerraInterface* GreateTerraInterface()
{
	return new PerimeterTerraInterface;
}

void terPlayer::burnZeroplast()
{
	MTEnter enter(lock_burn_zeroplast);
	begin_time_burn_zeroplast.push_back(clockf());
}

void terPlayer::quantZeroplast()
{
	MTEnter enter(lock_burn_zeroplast);
	float multiply=1;
	double cur_time=clockf();
	double fade=zeroplastBurn.fade_time;

	list<double>::iterator it;
	for(it=begin_time_burn_zeroplast.begin();it!=begin_time_burn_zeroplast.end();)
	{
		float dt=cur_time-*it;
		if(dt>fade)
		{
			it=begin_time_burn_zeroplast.erase(it);
			continue;
		}
/*
		float mul=(fade-dt)/fade;
		multiply+=mul;
/*/
		float t=dt/fade;
		multiply+=zeroplastBurn.strength*(cos((t-0.5f)*2*M_PI)+1);
/**/
		it++;
	}

	sColor4f color=zeroLayerColor_;
	color*=multiply;
	color.r=min(2.0f,color.r);
	color.g=min(2.0f,color.g);
	color.b=min(2.0f,color.b);
	color.a=min(1.0f,zeroplastBurn.reflection*
		(1+zeroplastBurn.reflection_strength*(multiply-1))
		);
	terMapPoint->SetZeroplastColor(playerID(), color);
}
