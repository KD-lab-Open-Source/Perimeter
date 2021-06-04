// GraphSettingsSet.h: interface for the GraphSettingsSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHSETTINGSSET_H__996D6516_E1B6_4FFB_8716_0D623E2BE4ED__INCLUDED_)
#define AFX_GRAPHSETTINGSSET_H__996D6516_E1B6_4FFB_8716_0D623E2BE4ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISetting.h"
#include "CheckSetting.h"
#include "SliderSetting.h"
#include "ReflectionSetting.h"
#include "ShadowTypeSetting.h"
#include "ShadowSamples.h"
#include "MapLevelLOD.h"
#include "ShadowQualitySetting.h"

#include <list>

class GraphSettingsSet : public ISetting  
{
public:
	GraphSettingsSet(UINT sectionId);
	GraphSettingsSet(UINT sectionId,
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
		UINT shadowQualityCtrlId);

	GraphSettingsSet(GraphSettingsSet const& rhs);

	void operator =(GraphSettingsSet const& rhs);

	virtual ~GraphSettingsSet();
	bool operator == (GraphSettingsSet const& rhs) const;

	bool readFromStorage();
	void copySettings(GraphSettingsSet const& rhs);

	void DDX_Exchange(CDataExchange* pDX);
	void initControl(CWnd* parent, LocaleMngr const& locMngr) const;

protected:
	UINT getSectionId() const;
	bool saveToStorage() const;


	void initList();
private:
	UINT sectionId_;
	ReflectionSetting reflection_;
	CheckSetting occlustion_;
	CheckSetting pointLight_;
	CheckSetting bumpMapping_;
	CheckSetting bumpChaos_;
	SliderSetting particleRate_;
	CheckSetting compressTextures_;
	ShadowTypeSetting shadowType_;
	ShadowSamples shadowSamples_;
	MapLevelLODSetting mapLevelLOD_; 
	ShadowQualitySetting shadowQuality_;

	std::list<ISetting*> settingList_;
};

#endif // !defined(AFX_GRAPHSETTINGSSET_H__996D6516_E1B6_4FFB_8716_0D623E2BE4ED__INCLUDED_)
