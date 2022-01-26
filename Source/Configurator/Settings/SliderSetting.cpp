// SliderSetting.cpp: implementation of the SliderSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "SliderSetting.h"
#include <xutil.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SliderSetting::SliderSetting(UINT ctrlId, UINT sectionId, UINT keyId)
: SettingBase(ctrlId)
, sectionId_(sectionId)
, keyId_(keyId)
, value_(-1)
{

}

SliderSetting::~SliderSetting()
{

}

void SliderSetting::copyValue(SliderSetting const& rhs)
{
	value_ = rhs.getValue();
}

int SliderSetting::getValue() const{
	return value_;
}
UINT SliderSetting::getSectionId() const{
	return sectionId_;
}
UINT SliderSetting::getValueId() const{
	return keyId_;
}

int SliderSetting::storeValueToWnd(float storeValue) const
{
	return xm::round(storeValue * 100);
}

float SliderSetting::wndValueToStore(int value) const
{
	return ((float)value)/100;
}

bool SliderSetting::readFromStorage()
{
	CString section;
	section.LoadString(getSectionId());
	CString key;
	key.LoadString(getValueId());
	CString buffer;
	const DWORD BUFFER_LEN = 64;
	GetPrivateProfileString(section, 
							key, 
							_T("0.0"), 
							buffer.GetBuffer(BUFFER_LEN),
							BUFFER_LEN,
							iniFileName);
	buffer.ReleaseBuffer();
	TCHAR* end = NULL;
	float storeValue = static_cast<float>(_tcstod(buffer,&end));
	value_ = storeValueToWnd(storeValue);
	return true;
}

bool SliderSetting::saveToStorage() const
{
	CString section;
	section.LoadString(getSectionId());
	CString key;
	key.LoadString(getValueId());
	CString strValue;
	strValue.Format(_T("%.2f"), wndValueToStore(value_));
	return (WritePrivateProfileString(section, key, strValue, iniFileName) == TRUE);
}

void SliderSetting::DDX_Exchange(CDataExchange* pDX)
{
	if (getCtrlId()== UINT(-1))
		return;
	DDX_Slider(pDX, getCtrlId(), value_);
}

void SliderSetting::initControl(CWnd* parent, LocaleMngr const& locMngr) const
{
	if (getCtrlId() == (UINT)-1)
		return;
	CSliderCtrl* slider = 
		reinterpret_cast<CSliderCtrl*>(parent->GetDlgItem(getCtrlId()));
	slider->SetRange(0, 100);
}

bool SliderSetting::operator == (SliderSetting const& rhs) const{
	return (getValue() == rhs.getValue());
}
