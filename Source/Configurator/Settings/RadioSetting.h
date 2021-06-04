// RadioSetting.h: interface for the RadioSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADIOSETTING_H__E2F7F244_7186_4F44_8557_DF91BB668752__INCLUDED_)
#define AFX_RADIOSETTING_H__E2F7F244_7186_4F44_8557_DF91BB668752__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SettingBase.h"

class RadioSetting : public SettingBase  
{
public:
	RadioSetting(UINT ctrlId, UINT sectionId, UINT valueId);
	virtual ~RadioSetting();

	bool readFromStorage();
	bool saveToStorage() const;
	void DDX_Exchange(CDataExchange* pDX);

	void copyValue(RadioSetting const& rhs);
	bool operator == (RadioSetting const& rhs) const;
protected:
	UINT getSectionId() const;
	UINT getValueId() const;
	int getValue() const;

	virtual int storedToWnd(int value) const;
	virtual int wndToStored(int value) const;
private:
	UINT sectionId_;
	UINT valueId_;
	int value_;
};

#endif // !defined(AFX_RADIOSETTING_H__E2F7F244_7186_4F44_8557_DF91BB668752__INCLUDED_)
