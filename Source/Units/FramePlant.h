#ifndef __FRAMEPLANT_H__
#define __FRAMEPLANT_H__

#include "IronBuilding.h"

class terBuildingPlant : public terBuildingPowered
{
public:
	terBuildingPlant(const UnitTemplate& data);

	void Quant();
	void DestroyLink();

	float requestProductionProgress(terUnitBase* squad);

	void setProduction(terUnitLegionary* p);

	void ChangeUnitOwner(terPlayer* player);

protected:
	ChainID chainRequest() const;

private:
	terUnitLegionary* production_;
	float productionProgress_;
	UnitList shipmentQueue_;
	int waitingCounter_;
	DurationTimer productionRequestTimer_;

	terSoundController* productionSound_;
};

#endif /* __FRAMEPLANT_H__ */