#if !defined(ISETTING_FACTORY_H)
#define ISETTING_FACTORY_H

class ISetting;

#include <memory>

class ISettingFactory
{
public:
	virtual ~ISettingFactory() = 0;

	virtual ISetting* createLanguageSetting(UINT ctrlId) = 0;
	virtual ISetting* createMultiprocSetting(UINT ctrlId) = 0;
	virtual ISetting* createResolutionSetting(UINT ctrlId) = 0;
	virtual ISetting* createColorDepthSetting(UINT ctrlId) = 0;
	virtual ISetting* createCheckSetting(UINT ctrlId, UINT sectionId, UINT keyId) = 0;
	virtual ISetting* createSliderSetting(UINT ctrlId, UINT sectionId, UINT keyId) = 0;

	virtual ISetting* createMouseLookRateSetting(UINT ctrlId) = 0;
	virtual ISetting* createScrollRateSetting(UINT ctrlId) = 0;
};


#endif
