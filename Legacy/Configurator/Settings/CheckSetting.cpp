// CheckSetting.cpp: implementation of the CheckSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "CheckSetting.h"
#include "IniReader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CheckSetting::CheckSetting(UINT ctrlId, UINT sectionId, UINT valueId)
: SettingBase(ctrlId)
, sectionId_(sectionId)
, valueId_(valueId)
, value_(0)
{

}
CheckSetting::CheckSetting(CheckSetting const& rhs)
: SettingBase(rhs)
, value_(rhs.getValue())
{

}

void CheckSetting::operator=(CheckSetting const& rhs)
{
	value_ = rhs.getValue();
}

CheckSetting::~CheckSetting()
{

}

int CheckSetting::getValue() const{
	return value_;
}

UINT CheckSetting::getSectionId() const{
	return sectionId_;
}

UINT CheckSetting::getValueId() const{
	return valueId_;
}

bool CheckSetting::readFromStorage()
{
	value_ = IniReader::readInt(getSectionId(), getValueId(), 0, iniFileName);
	return true;
}

bool CheckSetting::saveToStorage() const
{
	return IniReader::writeInt(getSectionId(), getValueId(), value_, iniFileName);
}

void CheckSetting::DDX_Exchange(CDataExchange* pDX)
{
	if (getCtrlId() == (UINT)-1)
		return;
	DDX_Check(pDX, getCtrlId(), value_);
}

bool CheckSetting::operator == (CheckSetting const& rhs) const{
	return (getValue() == rhs.getValue());
}

void CheckSetting::copyValue(CheckSetting const& rhs)
{
	value_ = rhs.getValue();
}
