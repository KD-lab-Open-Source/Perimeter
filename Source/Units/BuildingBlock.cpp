#include "StdAfx.h"

#include "Universe.h"
#include "BuildingBlock.h"
#include "IronFrame.h"

#include "UniverseInterface.h"

#include "IronExplosion.h"
#include "PerimeterSound.h"


terUnitBuildingBlock::terUnitBuildingBlock(const UnitTemplate& data) : terUnitReal(data)
{
	masterPoint_ = NULL;
	targetPoint_ = NULL;

	FireSoundPoint = realAvatar()->findSound(SOUND_SHOT);

	fireFactor_ = 0;

	BlockMode = BUILDING_BLOCK_MODE_FIND;
	ConstructionScale = 0;
}

void terUnitBuildingBlock::DestroyLink()
{
	terUnitReal::DestroyLink();

	if(masterPoint_ && !(masterPoint_->alive()))
		masterPoint_ = NULL;

	if(targetPoint_ && !(targetPoint_->alive())){
		targetPoint_ = NULL;
	}
}

void terUnitBuildingBlock::AvatarQuant()
{
	terUnitReal::AvatarQuant();

	avatar()->Show();
	avatar()->setPose(BodyPoint->pose());

	realAvatar()->setBuild(0.5f);
	realAvatar()->setHologram(0.5f + xm::sin(ConstructionScale) * 0.5f);
	realAvatar()->setTexture(Player->HologramPoint);

	switch(ChainMode){
		case BUILDING_BLOCK_CHAIN_MOVE:
			avatar()->SetChain("main");
			break;
		case BUILDING_BLOCK_CHAIN_WORK:
			avatar()->SetChain("sbros");
			break;
	}
	realAvatar()->setPhase(ChainAnimation.phase());

	if(FireSoundPoint && fireFactor_ > 0.0001f){
		FireSoundPoint->setVolume(fireFactor_);
		FireSoundPoint->setFrequency(fireFactor_);
        FireSoundPoint->play();
	}
}

void terUnitBuildingBlock::Quant()
{
	terUnitReal::Quant();

	ConstructionScale += 0.1f * XM_PI;

	average(fireFactor_, (float)(BlockMode == BUILDING_BLOCK_MODE_WORK), 0.2f);
}

void terUnitBuildingBlock::Kill()
{
	if(targetPoint_)
		targetPoint_->changeRepairRequest(-damageMolecula().aliveElementCount());
	
	terUnitReal::Kill();
}

void terUnitBuildingBlock::WayPointController()
{
	terUnitReal::WayPointController();

	BodyPoint->way_points.clear();

	if(!masterPoint_){
		Kill();
		return;
	}

	switch(BlockMode){
		case BUILDING_BLOCK_MODE_FIND:
			MoveChain();
			if(findTarget()){
				targetPoint_->changeRepairRequest(damageMolecula().aliveElementCount());
				BlockMode = BUILDING_BLOCK_MODE_TARGET;
			}
			else
				BlockMode = BUILDING_BLOCK_MODE_MASTER;
			break;
		case BUILDING_BLOCK_MODE_MASTER:
			MoveChain();
			if(BodyPoint->is_point_reached(masterPoint_->position())){
				Kill();
				return;
			}
			else
				BodyPoint->way_points.push_back(masterPoint_->position());
			break;
		case BUILDING_BLOCK_MODE_WORK:
			if(WorkChain()){
				if(targetPoint_ && alive()){
					targetPoint_->damageMoleculaRepair(damageMolecula().aliveElementCount());
					Kill();
				} 
				else {
					if(ChainAnimation.isEnd())
						Kill();
				}
			}
			break;
		case BUILDING_BLOCK_MODE_TARGET:
			MoveChain();
			if(targetPoint_){
				if(BodyPoint->is_point_reached(targetPoint_->position())){
					BlockMode = BUILDING_BLOCK_MODE_WORK;
					if(Player->active())
						SND3DPlaySound("block_put",&position());
				}
				else
					BodyPoint->way_points.push_back(targetPoint_->position());
			}
			else
				BlockMode = BUILDING_BLOCK_MODE_FIND;
			break;
	}
}

void terUnitBuildingBlock::WayPointStart()
{
	terUnitReal::WayPointStart();
	StartMoveChain();
}

bool terUnitBuildingBlock::findTarget()
{
	targetPoint_ = 0;

	terUnitBase* unit = masterPoint_->targetUnit();
	if(unit && unit->needBuildingRepair() && unit->repairRequest()){
		targetPoint_ = safe_cast<terUnitGeneric*>(unit);
		return true;
	}

	float max = 0;
	const UnitList& unit_list=Player->units();
	UnitList::const_iterator it;
	FOR_EACH(unit_list,it){
		if((*it)->needBuildingRepair()){
			terBuilding* p = safe_cast<terBuilding*>(*it);
			if(p->repairRequest()){
				float dist = p->position2D().distance2(position2D());
				float f = p->attr()->constructionPriority + 1.f/(1.f + dist);

				if(f > max || !targetPoint_){
					targetPoint_ = p;
					max = f;
				}
			}
		}
	}

	if(targetPoint_)
		return true;

	return false;
}


int terUnitBuildingBlock::MoveChain()
{
	switch(ChainMode){
		case BUILDING_BLOCK_CHAIN_MOVE:
			ChainAnimation.Quant();
			return 1;
		case BUILDING_BLOCK_CHAIN_WORK:
			ChainAnimation.Quant();
			if(ChainAnimation.isEnd())
				StartMoveChain();
			break;
	}
	return 0;
}

int terUnitBuildingBlock::WorkChain()
{
	switch(ChainMode){
		case BUILDING_BLOCK_CHAIN_MOVE:
			if(ChainAnimation.Quant())
				StartWorkChain();
			break;
		case BUILDING_BLOCK_CHAIN_WORK:
			ChainAnimation.Quant();
			return 1;
	}
	return 0;
}

void terUnitBuildingBlock::StartMoveChain()
{
	ChainAnimation.startPhase(0,0.05f,1);
	ChainMode = BUILDING_BLOCK_CHAIN_MOVE;
}

void terUnitBuildingBlock::StartWorkChain()
{
	ChainAnimation.startPhase(0,0.1f,0);
	ChainMode = BUILDING_BLOCK_CHAIN_WORK;
}

terUnitBase* terUnitBuildingBlock::GetIgnoreUnit()
{
	if(ChainMode == BUILDING_BLOCK_CHAIN_WORK)
		return targetPoint_;
	return NULL;
}


void terUnitBuildingBlock::SoundExplosion()
{
}

void terUnitBuildingBlock::setDamage(const DamageData& damage,terUnitBase* p)
{
	int element_count = damageMolecula().aliveElementCount();

	terUnitReal::setDamage(damage,p);

	if(int element_count_delta = damageMolecula().aliveElementCount() - element_count)
		if(targetPoint_) targetPoint_->changeRepairRequest(element_count_delta);
}
