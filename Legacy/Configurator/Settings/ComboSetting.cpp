// ComboSetting.cpp: implementation of the ComboSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "ComboSetting.h"
#include "IniReader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ComboSetting::ComboSetting(UINT ctrlId, UINT sectionId, UINT valueId)
: SettingBase(ctrlId)
, sectionId_(sectionId)
, valueId_(valueId)
, value_(-1)
{

}

ComboSetting::~ComboSetting()
{

}

UINT ComboSetting::getSectionId() const{
	return sectionId_;
}
UINT ComboSetting::getValueId() const{
	return valueId_;
}

int ComboSetting::getValue() const{
	return value_;
}

bool ComboSetting::readFromStorage()
{
	value_ = storedToWnd(
		IniReader::readInt(getSectionId(), getValueId(), 0, iniFileName));
	return true;
}
bool ComboSetting::saveToStorage() const
{
	return IniReader::writeInt(getSectionId(), getValueId(), 
		wndToStored(getValue()), iniFileName);
}

void ComboSetting::DDX_Exchange(CDataExchange* pDX)
{
	if (getCtrlId() == UINT(-1))
		return;

	DDX_CBIndex(pDX, getCtrlId(), value_);
}

bool ComboSetting::operator == (ComboSetting const& rhs) const{
	return (value_ == rhs.getValue());
}

void ComboSetting::copyValue(ComboSetting const& rhs){
	value_ = rhs.getValue();
}

int ComboSetting::storedToWnd(int value) const
{
	return value;
}
int ComboSetting::wndToStored(int value) const
{
	return value;
}
