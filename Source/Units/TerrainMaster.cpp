#include "StdAfx.h"

#include "Universe.h"
#include "Player.h"
#include "TerrainMaster.h"
#include "IronDigger.h"

#include "UniverseInterface.h"

#include "IronExplosion.h"

#include "PerimeterSound.h"
#include "Config.h"

const int TERRAIN_MASTER_TRUCK_MAX = 10;

terUnitTerrainMaster::terUnitTerrainMaster(const UnitTemplate& data) : terFrameChild(data)
{
	productedTrucksNumber_ = TERRAIN_MASTER_TRUCK_MAX;
}

void terUnitTerrainMaster::DestroyLink()
{
	terFrameChild::DestroyLink();

	removeNotAlive(Trucks);
}

void terUnitTerrainMaster::Quant()
{
	terFrameChild::Quant();

	MetaRegionLock lock(Player->RegionPoint);
	if(Trucks.size() < TERRAIN_MASTER_TRUCK_MAX){
		if(Trucks.size() + productedTrucksNumber_ < TERRAIN_MASTER_TRUCK_MAX && productionCompleted())
			productedTrucksNumber_++;

		Region* region = Player->ZeroRegionPoint->locateEdit(position2D());
		int request_digger,request_filler;
		if(region && region->positive()){
			Player->TrustMap->ScanTrustMap(request_digger,request_filler,region->ID());
			if(!request_digger && !request_filler)
				Player->TrustMap->ScanTrustMap(request_digger,request_filler);
		}else
			Player->TrustMap->ScanTrustMap(request_digger,request_filler);
		
		int number = min(request_digger + request_filler, productedTrucksNumber_);
		for(int i = 0; i < number && Trucks.size() < TERRAIN_MASTER_TRUCK_MAX; i++){
			productedTrucksNumber_--;
			terUnitTruck* p = safe_cast<terUnitTruck*>(Player->buildUnit(UNIT_ATTRIBUTE_TRUCK));
			p->setPose(Se3f(QuatF::ID, position()), true);
			p->SetTrustMaster(this);
			p->Start();
			Trucks.push_back(p);
		}
	}

	if(!Trucks.empty())
		Player->BrigadierWorking = 1;
}

void terUnitTerrainMaster::ShowCircles() {
	terUnitBase::ShowCircles();
	TruckList::iterator ui;
	FOR_EACH(Trucks, ui){
		if((*ui)->alive()){
			(*ui)->ShowCircles();
		}
	}

}
