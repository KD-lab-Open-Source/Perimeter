#include "StdAfx.h"

#include "Universe.h"
#include "FrameCore.h"

#include "UniverseInterface.h"

#include "IronExplosion.h"

#include "PerimeterSound.h"
#include "Config.h"

void terBuildingEnergy::setRealModel(int modelIndex, float scale)
{
    terBuilding::setRealModel(modelIndex, scale);
	lighting_ = avatar()->animationGroup(ANIMATION_GROUP_LIGHTING);
	realAvatar()->setChain(isBuildingEnable() ? CHAIN_SWITCHED_ON : (isConstructed() ? CHAIN_SWITCHED_OFF : CHAIN_BUILD1));
}

bool terBuildingEnergy::BuildingEnableRequest()
{
	switch(realAvatar()->chainID()){
		case CHAIN_SWITCHED_ON:
		case CHAIN_UPGRADE_PREPARE:
		case CHAIN_UPGRADE:
			return true;
		default:
			return false;
	}
}

ChainID terBuildingEnergy::chainRequest() const
{
	if(isConstructed())
		return isConnected() ? CHAIN_SWITCHED_ON : CHAIN_SWITCHED_OFF;

	return terBuilding::chainRequest();
}


void terBuildingEnergy::Quant()
{
	terBuilding::Quant();

	if(isConstructed() && lighting_){
		if(isBuildingEnable()){
			if(realAvatar()->chainID() == CHAIN_SWITCHED_ON)
				lighting_->requestChainNode(CHAIN_NODE_SWITCHED_ON);
			else
				lighting_->requestChainNode(CHAIN_NODE_SWITCHED_OFF);
		}
		else 
			lighting_->requestChainNode(CHAIN_NODE_SWITCHED_OFF);
	}
}

terEffectID terBuildingEnergy::explosionID() const 
{
	if(isConstructed()){
		if(realAvatar()->chainID() == CHAIN_SWITCHED_ON)
			return EFFECT_ID_EXPLOSION;
	}

	if(attr().getEffectData(EFFECT_ID_EXPLOSION_ALTERNATE))
		return EFFECT_ID_EXPLOSION_ALTERNATE;

	return EFFECT_ID_EXPLOSION;
}
