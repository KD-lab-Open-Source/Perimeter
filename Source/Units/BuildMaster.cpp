#include "StdAfx.h"

#include "terra.h"

#include "Runtime.h"

#include "Universe.h"
#include "GenericUnit.h"
#include "TrustMap.h"
#include "RealUnit.h"
#include "RealInterpolation.h"
#include "BuildMaster.h"
#include "BuildingBlock.h"

#include "UniverseInterface.h"

#include "IronExplosion.h"

#include "PerimeterSound.h"
#include "Config.h"
#include "IronFrame.h"

terUnitBuildMaster::terUnitBuildMaster(const UnitTemplate& data) : terFrameChild(data)
{
}

void terUnitBuildMaster::Start()
{
	terFrameChild::Start();
	BodyPoint->setFlyingMode(1);
}

void terUnitBuildMaster::Quant()
{
	terFrameChild::Quant();

	if((targetUnit() && targetUnit()->repairRequest()) || Player->buildingBlockRequest()){
		if(productionCompleted()){
			terUnitBuildingBlock* p = safe_cast<terUnitBuildingBlock*>(Player->buildUnit(UNIT_ATTRIBUTE_BUILDING_BLOCK));
			p->setPose(Se3f(QuatF::ID, position()), true);
			p->SetMasterPoint(this);
			p->Start();
		}
	}
}

int terUnitBuildMaster::GetInterfaceLegionMode()
{
	return terFrameChild::GetInterfaceLegionMode() | (targetUnit() ? INTERFACE_LEGION_MODE_TARGET_BUILD : 0);
}
