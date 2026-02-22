// SettingFabric.h: interface for the SettingFabric class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGFABRIC_H__3B3B1362_9EEF_4080_94A3_FF44C0CE22CA__INCLUDED_)
#define AFX_SETTINGFABRIC_H__3B3B1362_9EEF_4080_94A3_FF44C0CE22CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISettingFactory.h"

class SettingFactory : public ISettingFactory  
{
protected:
	SettingFactory();
public:
	virtual ~SettingFactory();

	static ISettingFactory* instance();

	virtual ISetting* createLanguageSetting(UINT ctrlId);
	virtual ISetting* createMultiprocSetting(UINT ctrlId);
	virtual ISetting* createResolutionSetting(UINT ctrlId);
	virtual ISetting* createColorDepthSetting(UINT ctrlId);
	virtual ISetting* createCheckSetting(UINT ctrlId, UINT sectionId, UINT keyId);
	virtual ISetting* createSliderSetting(UINT ctrlId, UINT sectionId, UINT keyId);

	virtual ISetting* createMouseLookRateSetting(UINT ctrlId);
	virtual ISetting* createScrollRateSetting(UINT ctrlId);
protected:
};

#endif // !defined(AFX_SETTINGFactory_H__3B3B1362_9EEF_4080_94A3_FF44C0CE22CA__INCLUDED_)
