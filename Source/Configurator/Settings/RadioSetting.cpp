// RadioSetting.cpp: implementation of the RadioSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "RadioSetting.h"
#include "IniReader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RadioSetting::RadioSetting(UINT ctrlId, UINT sectionId, UINT valueId)
: SettingBase(ctrlId)
, sectionId_(sectionId)
, valueId_(valueId)
{

}

RadioSetting::~RadioSetting()
{

}

UINT RadioSetting::getSectionId() const{
	return sectionId_;
}
UINT RadioSetting::getValueId() const{
	return valueId_;
}
int RadioSetting::getValue() const{
	return value_;
}

int RadioSetting::storedToWnd(int value) const{
	return value;
}

int RadioSetting::wndToStored(int value) const{
	return value;
}

bool RadioSetting::readFromStorage()
{
	value_ = storedToWnd(
		IniReader::readInt(getSectionId(), getValueId(), 0, iniFileName));
	return true;
}

bool RadioSetting::saveToStorage() const
{
	return IniReader::writeInt(getSectionId(), 
		getValueId(), wndToStored(getValue()), iniFileName);
}

void RadioSetting::DDX_Exchange(CDataExchange* pDX)
{
	if (getCtrlId() == (UINT)-1)
		return;
	DDX_Radio(pDX, getCtrlId(), value_);
}

void RadioSetting::copyValue(RadioSetting const& rhs)
{
	value_ = rhs.getValue();
}

bool RadioSetting::operator == (RadioSetting const& rhs) const{
	return (value_ == rhs.getValue());
}
