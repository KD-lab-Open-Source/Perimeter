// GraphSettingsSet.cpp: implementation of the GraphSettingsSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "GraphSettingsSet.h"
#include "IniReader.h"

#include <algorithm>
#include <functional>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GraphSettingsSet::GraphSettingsSet(UINT sectionId)
: sectionId_(sectionId)
, reflection_(UINT(-1), sectionId)
, occlustion_(UINT(-1), sectionId, IDS_KEY_OCCLUSION)
, pointLight_(UINT(-1), sectionId, IDS_KEY_POINT_LIGHT)
, bumpMapping_(UINT(-1), sectionId, IDS_KEY_BUMP)
, bumpChaos_(UINT(-1), sectionId, IDS_KEY_BUMP_CHAOS)
, particleRate_(UINT(-1), sectionId, IDS_KEY_PARTICLE_RATE)
, compressTextures_(UINT(-1), sectionId, IDS_KEY_COMPRESS_TEXTURES)
, shadowType_(UINT(-1), sectionId)
, shadowSamples_(UINT(-1), sectionId)
, mapLevelLOD_(UINT(-1), sectionId)
, shadowQuality_(UINT(-1), sectionId)
{
	initList();
}

GraphSettingsSet::GraphSettingsSet(UINT sectionId,
		UINT reflectionCtrlId,
		UINT occlustionCtrlId,
		UINT pointLightCtrlId,
		UINT bumpMappingCtrlId,
		UINT bumpChaosCtrlId,
		UINT particleRateCtrlId,
		UINT compressTexturesCtrlId,
		UINT shadowTypeCtrlId,
		UINT shadowSamplesCtrlId,
		UINT mapLevelLODCtrlId,
		UINT shadowQualityCtrlId)
: sectionId_(sectionId)
, reflection_(reflectionCtrlId, sectionId)
, occlustion_(occlustionCtrlId, sectionId, IDS_KEY_OCCLUSION)
, pointLight_(pointLightCtrlId, sectionId, IDS_KEY_POINT_LIGHT)
, bumpMapping_(bumpMappingCtrlId, sectionId, IDS_KEY_BUMP)
, bumpChaos_(bumpChaosCtrlId, sectionId, IDS_KEY_BUMP_CHAOS)
, particleRate_(particleRateCtrlId, sectionId, IDS_KEY_PARTICLE_RATE)
, compressTextures_(compressTexturesCtrlId, sectionId, IDS_KEY_COMPRESS_TEXTURES)
, shadowType_(shadowTypeCtrlId, sectionId)
, shadowSamples_(shadowSamplesCtrlId, sectionId)
, mapLevelLOD_(mapLevelLODCtrlId, sectionId)
, shadowQuality_(shadowQualityCtrlId, sectionId)
{
	initList();
}

GraphSettingsSet::GraphSettingsSet(GraphSettingsSet const& rhs)
: sectionId_(rhs.sectionId_)
, reflection_(rhs.reflection_)
, occlustion_(rhs.occlustion_)
, pointLight_(rhs.pointLight_)
, bumpMapping_(rhs.bumpMapping_)
, bumpChaos_(rhs.bumpChaos_)
, particleRate_(rhs.particleRate_)
, compressTextures_(rhs.compressTextures_)
, shadowType_(rhs.shadowType_)
, shadowSamples_(rhs.shadowSamples_)
, mapLevelLOD_(rhs.mapLevelLOD_)
, shadowQuality_(rhs.shadowQuality_)
{
	initList();
}

void GraphSettingsSet::copySettings(GraphSettingsSet const& rhs)
{
	reflection_.copyValue(rhs.reflection_);
	occlustion_.copyValue(rhs.occlustion_);
	pointLight_.copyValue(rhs.pointLight_);
	bumpMapping_.copyValue(rhs.bumpMapping_);
	bumpChaos_.copyValue(rhs.bumpChaos_);
	particleRate_.copyValue(rhs.particleRate_);
	compressTextures_.copyValue(rhs.compressTextures_);
	shadowType_.copyValue(rhs.shadowType_);
	shadowSamples_.copyValue(rhs.shadowSamples_);
	mapLevelLOD_.copyValue(rhs.mapLevelLOD_);
	shadowQuality_.copyValue(rhs.shadowQuality_);
}

void GraphSettingsSet::operator =(GraphSettingsSet const& rhs) 
{
	sectionId_		= rhs.sectionId_;
	reflection_		= rhs.reflection_;
	occlustion_		= rhs.occlustion_;
	pointLight_		= rhs.pointLight_;
	bumpMapping_	= rhs.bumpMapping_;
	bumpChaos_		= rhs.bumpChaos_;
	particleRate_		= rhs.particleRate_;
	compressTextures_		= rhs.compressTextures_;
	shadowType_		= rhs.shadowType_;
	shadowSamples_	= rhs.shadowSamples_;
	mapLevelLOD_	= rhs.mapLevelLOD_;
	shadowQuality_	= rhs.shadowQuality_;
}

void GraphSettingsSet::initList()
{
	settingList_.push_back(&reflection_);
	settingList_.push_back(&occlustion_);
	settingList_.push_back(&pointLight_);
	settingList_.push_back(&bumpMapping_);
	settingList_.push_back(&bumpChaos_);
	settingList_.push_back(&particleRate_);
	settingList_.push_back(&compressTextures_);
	settingList_.push_back(&shadowType_);
	settingList_.push_back(&shadowSamples_);
	settingList_.push_back(&mapLevelLOD_);
	settingList_.push_back(&shadowQuality_);
}

UINT GraphSettingsSet::getSectionId() const{
	return sectionId_;
}

GraphSettingsSet::~GraphSettingsSet()
{

}

bool GraphSettingsSet::readFromStorage()
{
	std::for_each(settingList_.begin(), settingList_.end(),
		std::mem_fun(&ISetting::readFromStorage));
	return true;
}

bool GraphSettingsSet::saveToStorage() const
{
	std::for_each(settingList_.begin(), settingList_.end(),
		std::mem_fun(&ISetting::saveToStorage));
	return true;
}

void GraphSettingsSet::DDX_Exchange(CDataExchange* pDX)
{
	std::list<ISetting*>::const_iterator i = settingList_.begin(), 
										 e = settingList_.end();
	for(; i != e; ++i)
		(*i)->DDX_Exchange(pDX);
}

void GraphSettingsSet::initControl(CWnd* parent, LocaleMngr const& locMngr) const
{
	std::list<ISetting*>::const_iterator i = settingList_.begin(), 
										e = settingList_.end();
	for(; i != e; ++i)
		(*i)->initControl(parent, locMngr);
}

bool GraphSettingsSet::operator == (GraphSettingsSet const& rhs) const
{
	return (reflection_ == rhs.reflection_
		&& occlustion_ == rhs.occlustion_
		&& pointLight_ == rhs.pointLight_
		&& bumpMapping_ == rhs.bumpMapping_
		&& bumpChaos_ == rhs.bumpChaos_
		&& particleRate_ == rhs.particleRate_
		&& compressTextures_ == rhs.compressTextures_
		&& shadowType_ == rhs.shadowType_
		&&shadowSamples_ == rhs.shadowSamples_
		&&mapLevelLOD_ == rhs.mapLevelLOD_
		&&shadowQuality_ == rhs.shadowQuality_);
}

