// ShadowQualitySetting.cpp: implementation of the ShadowQualitySetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "ShadowQualitySetting.h"
#include "Localisation/LocaleMngr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int SHADOW_OFF	= 0;
const int SHADOW_LOW = 2;
const int SHADOW_MEDIUM = 3;
const int SHADOW_HIGH	= 4;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ShadowQualitySetting::ShadowQualitySetting(UINT ctrlId, UINT sectionId)
: ComboSetting(ctrlId, sectionId, IDS_KEY_SHADOW_QUALITY)
{

}

ShadowQualitySetting::~ShadowQualitySetting()
{

}

void ShadowQualitySetting::initControl(CWnd* parent, LocaleMngr const& locMngr) const
{
	if (getCtrlId() == (UINT)-1)
		return;

	TCHAR const* const value[] ={
		_T("Off"),
		_T("Low"),
		_T("Medium"),
		_T("High")
	};
	int count = sizeof(value)/sizeof(TCHAR*);

	CComboBox* combo = reinterpret_cast<CComboBox*>(parent->GetDlgItem(getCtrlId()));
	if (!combo)
		return;

	combo->ResetContent();
	for(int i = 0; i < count; ++i)
		combo->AddString(locMngr.getText(locMngr.makeComboItemsKey(value[i])));
}

int ShadowQualitySetting::storedToWnd(int value) const{
	switch(value) {
	case SHADOW_OFF: return 0;
	case SHADOW_LOW: return 1;
	case SHADOW_MEDIUM: return 2;
	case SHADOW_HIGH: return 3;
	}
	ASSERT(0);
	return -1;
}

int ShadowQualitySetting::wndToStored(int value) const{
	switch(value) {
	case 0: return SHADOW_OFF;
	case 1: return SHADOW_LOW;
	case 2: return SHADOW_MEDIUM;
	case 3: return SHADOW_HIGH;
	}
	ASSERT(0);
	return -1;
}
