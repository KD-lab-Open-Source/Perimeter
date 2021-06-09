#include "StdAfx.h"
#include "EnergyConsumer.h"
#include "Player.h"

EnergyConsumer::EnergyConsumer() : 
player_(0), 
delta_(0), 
energy_(1), 
total_(1), 
requested_(false), 
accumulative_(false),
priority_(0),
name_("")
{}

EnergyConsumer::~EnergyConsumer()
{
	if(player_)
		detach();
}

void EnergyConsumer::attach(terPlayer* player, const ConsumptionData& data)
{
	xassert(!player_);
	player_ = player;
	player_->attachEnergyUser(*this, priority_ = data.priority);
	start(data);
}

void EnergyConsumer::changePlayer(terPlayer* player)
{
	if(player_){
		player_->detachEnergyUser(*this);
		player_ = player;
		player_->attachEnergyUser(*this, priority_);
	}
}

void EnergyConsumer::detach()
{
	xassert(player_);
	if(player_)
		player_->detachEnergyUser(*this);
	player_ = 0;
}

void EnergyConsumer::start(const ConsumptionData& data)
{
	xassert(player_);
	delta_ = data.energy;
	if(delta_ < 0.0001f)
		delta_ = 0.0001f;
	accumulative_ = data.time > 100;
	if(accumulative_)
		total_ = energy_ = delta_*data.time/100;
	else{
		total_ = delta_;
		energy_ = 0;
	}
	name_ = data.name;
}

float EnergyConsumer::requestEnergy(terEnergyDataType& energyData)
{
	if(!requested_)
		return -1;
	requested_ = false;

	float delta;
	if(accumulative_){
		energy_ -= delta = energyData.addUsed(min(energy_, delta_));
	}
	else{
		energy_ = delta_;
		energy_ -= delta = energyData.addUsed(delta_);
	}
	return delta;
}

void EnergyConsumer::setProgress(float progress)
{
	energy_ = total_*(1 - progress);
}
