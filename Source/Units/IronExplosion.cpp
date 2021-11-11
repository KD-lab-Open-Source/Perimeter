#include "StdAfx.h"

#include "Runtime.h"

#include "Universe.h"
#include "Player.h"

#include "IronBullet.h"
#include "IronExplosion.h"

#include "PerimeterSound.h"


terDestructionCraterType::terDestructionCraterType(const UnitTemplate& data) : terUnitBase(data)
{ 
	radius_ = 32.0f;
}

void terDestructionCraterType::Start()
{
	terUnitBase::Start();

	ToolPoint.setKScale(radius() / 48.0f);
	ToolPoint.start(xm::round(position().x), xm::round(position().y));

/*	unsigned short p;
	p = *(vMap.GABuf + (vMap.YCYCLG(round(Position.y) >> kmGrid) << (vMap.V_SIZE_POWER - kmGrid)) + vMap.XCYCLG(round(Position.x) >> kmGrid));
	if(p & GRIDAT_MASK_HARDNESS){
		if(TerrainToolPoint){
			TerrainToolPoint->start(round(Position.x),round(Position.y));
			ToolWorking = 1;
		}

		if(DestructionToolPoint){
			DestructionToolPoint->start(round(Position.x),round(Position.y));
			ToolWorking = 1;
		}
	}

	if(SootToolPoint)
		SootToolPoint->start(round(Position.x),xm::round(Position.y));*/
}

void terDestructionCraterType::Quant()
{
	terUnitBase::Quant();

	start_timer_auto(CraterQuant,STATISTICS_GROUP_UNITS);

	if(!ToolPoint.quant())
		Kill();
}

void terDestructionCraterType::setPose(const Se3f& poseIn, bool initPose)
{
	Se3f pose = poseIn;
	pose.trans().z = (float)(vMap.GetAlt(vMap.XCYCL(xm::round(position().x)), vMap.YCYCL(xm::round(position().y))) >> VX_FRACTION);
	terUnitBase::setPose(pose, false);
}

//----------------------------------------

terDebrisCraterType::terDebrisCraterType(const UnitTemplate& data) : terUnitBase(data)
{
	radius_ = 32.0f;
}

void terDebrisCraterType::Start()
{
	terUnitBase::Start();

	ToolPoint.setRadius(radius());
	ToolPoint.start(xm::round(position().x), xm::round(position().y));
}
 
void terDebrisCraterType::Quant()
{
	terUnitBase::Quant();

	start_timer_auto(CraterQuant,STATISTICS_GROUP_UNITS);

	if(!ToolPoint.quant())
		Kill();
}

void terDebrisCraterType::setPose(const Se3f& poseIn, bool initPose)
{
	Se3f pose = poseIn;
	pose.trans().z = (float)(vMap.GetAlt(vMap.XCYCL(xm::round(position().x)), vMap.YCYCL(xm::round(position().y))) >> VX_FRACTION);
	terUnitBase::setPose(pose, false);
}

//----------------------------------------

