// SettingBase.h: interface for the SettingBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGBASE_H__B53D220A_5177_4E33_8ADC_335B540A9AC5__INCLUDED_)
#define AFX_SETTINGBASE_H__B53D220A_5177_4E33_8ADC_335B540A9AC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISetting.h"

class SettingBase : public ISetting  
{
public:
	SettingBase(UINT ctrlId);
	virtual ~SettingBase();

	//! Инициализация котрола относящегося к настройке(например заполнить комбобокс)
	virtual void initControl(CWnd* parent, LocaleMngr const& locMngr) const;

protected:
	UINT getCtrlId() const;
	void setCtrlId();
private:
	UINT ctrlId_;
};

#endif // !defined(AFX_SETTINGBASE_H__B53D220A_5177_4E33_8ADC_335B540A9AC5__INCLUDED_)
