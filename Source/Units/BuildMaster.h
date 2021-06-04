#ifndef _BUILD_MASTER_
#define _BUILD_MASTER_

#include "FrameChild.h"

struct terUnitBuildMaster : terFrameChild
{
	terUnitBuildMaster(const UnitTemplate& data);
		
	void Start();
	void Quant();

	int GetInterfaceLegionMode();
};

#endif
