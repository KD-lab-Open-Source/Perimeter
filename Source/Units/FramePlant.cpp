#include "StdAfx.h"

#include "Universe.h"
#include "FramePlant.h"

#include "UniverseInterface.h"

#include "IronExplosion.h"

#include "PerimeterSound.h"
#include "Config.h"

terBuildingPlant::terBuildingPlant(const UnitTemplate& data) : terBuildingPowered(data),
	production_(0),
	waitingCounter_(0), 
	productionProgress_(0),
	productionSound_(NULL)
{
	productionSound_ = realAvatar()->findSound(SOUND_PRODUCTION);
}

void terBuildingPlant::Quant()
{
	terBuilding::Quant();

	if(isConstructed() && production_){
		if(realAvatar()->requestDone()){
			production_->finalizeConstruction();
			productionProgress_ = 0;
			production_ = 0;
		}
	}

	if(productionSound_){
		if(isBuildingEnable() && productionProgress_ > 0){
			if(productionProgress_ < 1 - FLT_EPS){
				if(productionRequestTimer_())
					productionSound_->play();
			}
		}
		else
			productionSound_->stop();
	}
}

void terBuildingPlant::DestroyLink()
{
	terBuilding::DestroyLink();
	if(production_ && !(production_->alive()))
		production_ = 0;
}

void terBuildingPlant::setProduction(terUnitLegionary* p)
{ 
	xassert(!production_);
	production_ = p;
	terDockingSlot* slot = requestDockingSlot();
	xassert(slot);
	production_->SetDockingPoint(this,slot,DOCK_MODE_LOCK);
}

void terBuildingPlant::ChangeUnitOwner(terPlayer* player)
{
	terBuilding::ChangeUnitOwner(player);
	if(production_)
		production_->Kill();
}

ChainID terBuildingPlant::chainRequest() const
{
	if(isBuildingEnable() && productionProgress_ > 0){
		if(productionProgress_ < 1 - FLT_EPS){
			if(productionRequestTimer_())
				return CHAIN_PRODUCTION;
		}
		else
			return CHAIN_OPEN;
	}

	return terBuilding::chainRequest();
}

float terBuildingPlant::requestProductionProgress(terUnitBase* squad)
{
	if(find(shipmentQueue_.begin(), shipmentQueue_.end(), squad) == shipmentQueue_.end())
		shipmentQueue_.push_back(squad);

	if(production_)
		return 0;

	if(shipmentQueue_.front() != squad) {
        if (waitingCounter_++ < 6)
            return 0;
        else
            shipmentQueue_.front() = squad;
    }

	waitingCounter_ = 0;
	productionRequestTimer_.start(500);

	if(productionProgress_ < 1 - FLT_EPS){
		productionProgress_ = productionProgress();
	}
	else if(realAvatar()->chainID() == CHAIN_OPEN){
		shipmentQueue_.pop_front();
		return 1;
	}

	return productionProgress_*0.99;
}

