#ifndef __TERRAINMASTER_H__
#define __TERRAINMASTER_H__

#include "FrameChild.h"

class terUnitTerrainMaster : public terFrameChild
{
public:
	terUnitTerrainMaster(const UnitTemplate& data);

	void DestroyLink();

	void Quant();

	void returnTruck() { productedTrucksNumber_++; }

	void ShowCircles();

private:
	typedef list<struct terUnitTruck*> TruckList;
	TruckList Trucks;

	int productedTrucksNumber_;
};

#endif //__TERRAINMASTER_H__