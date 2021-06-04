// SettingFabric.cpp: implementation of the SettingFabric class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "SettingFactory.h"

#include "Settings/LanguageSetting.h"
#include "Settings/HTSetting.h"
#include "Settings/ResolutionSetting.h"
#include "Settings/ColorDepthSetting.h"
#include "Settings/CheckSetting.h"
#include "Settings/SliderSetting.h"
#include "Settings/ComboSetting.h"
#include "Settings/ScrollRateSetting.h"
#include "Settings/MouseLookRateSetting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

ISettingFactory::~ISettingFactory(){}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SettingFactory::SettingFactory()
{

}

SettingFactory::~SettingFactory()
{

}

ISettingFactory* SettingFactory::instance()
{
	static SettingFactory inst;
	return &inst;
}

ISetting* SettingFactory::createLanguageSetting(UINT ctrlId)
{
	return new LanguageSetting(ctrlId);
}

ISetting* SettingFactory::createMultiprocSetting(UINT ctrlId)
{
	return new HTSetting(ctrlId);
}

ISetting* SettingFactory::createResolutionSetting(UINT ctrlId)
{
	return new ResolutionSetting(ctrlId);
}

ISetting* SettingFactory::createColorDepthSetting(UINT ctrlId)
{
	return new ColorDepthSetting(ctrlId);
}

ISetting* SettingFactory::createCheckSetting(UINT ctrlId, UINT sectionId, UINT keyId)
{
	return new CheckSetting(ctrlId, sectionId, keyId);
}

ISetting* SettingFactory::createSliderSetting(UINT ctrlId, UINT sectionId, UINT keyId)
{
	return new SliderSetting(ctrlId, sectionId, keyId);
}

ISetting* SettingFactory::createMouseLookRateSetting(UINT ctrlId)
{
	return new MouseLookRateSetting(ctrlId);
}

ISetting* SettingFactory::createScrollRateSetting(UINT ctrlId)
{
	return new ScrollRateSetting(ctrlId);
}
