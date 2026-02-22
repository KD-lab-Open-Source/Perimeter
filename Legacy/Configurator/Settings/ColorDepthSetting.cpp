// ColorDepthSetting.cpp: implementation of the ColorDepthSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "ColorDepthSetting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int VALUE_1 = 16;
const int VALUE_2 = 32;

ColorDepthSetting::ColorDepthSetting(UINT ctrlId)
: SettingBase(ctrlId)
, value_(-1)
{
	
}

ColorDepthSetting::~ColorDepthSetting()
{

}

bool ColorDepthSetting::readFromStorage()
{
	CString section;
	section.LoadString(IDS_SECTION_GRAPHICS);
	CString key;
	key.LoadString(IDS_KEY_COLOR_DEPTH);
	value_ = storageValueToWnd(GetPrivateProfileInt(section, key, VALUE_1, iniFileName));
	return true;
}

bool ColorDepthSetting::saveToStorage() const
{
	CString section;
	section.LoadString(IDS_SECTION_GRAPHICS);
	CString key;
	key.LoadString(IDS_KEY_COLOR_DEPTH);
	CString strValue;
	strValue.Format(_T("%d"), wndValueToStorage(value_));
	return (WritePrivateProfileString(section, key, strValue, iniFileName) == TRUE);
}

void ColorDepthSetting::DDX_Exchange(CDataExchange* pDX)
{
	ASSERT(getCtrlId() != -1);
	DDX_Radio(pDX, getCtrlId(), value_);
}

int ColorDepthSetting::storageValueToWnd(int value) const
{
	switch(value) {
	case VALUE_1:
		return 0;
	case VALUE_2:
		return 1;
	default:
		return -1;
	}
}

int ColorDepthSetting::wndValueToStorage(int value) const
{
	switch(value) {
	case 0:
		return VALUE_1;
	case 1:
		return VALUE_2;
	default:
		return VALUE_1;
	}
}
