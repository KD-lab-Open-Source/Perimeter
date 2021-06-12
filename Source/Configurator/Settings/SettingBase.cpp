// SettingBase.cpp: implementation of the SettingBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "SettingBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ISetting::~ISetting(){}

SettingBase::SettingBase(UINT ctrlId)
: ctrlId_(ctrlId)
{

}

SettingBase::~SettingBase()
{

}

void SettingBase::setCtrlId()
{
}

UINT SettingBase::getCtrlId() const{
	return ctrlId_;
}

//! Инициализация котрола относящегося к настройке(например заполнить комбобокс)
void SettingBase::initControl(CWnd* parent, LocaleMngr const& locMngr) const{
}
