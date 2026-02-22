// ComboSetting.h: interface for the ComboSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMBOSETTING_H__B8D6D4A3_8724_4D16_BEFD_74D9861D7A60__INCLUDED_)
#define AFX_COMBOSETTING_H__B8D6D4A3_8724_4D16_BEFD_74D9861D7A60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SettingBase.h"

class ComboSetting : public SettingBase  
{
public:
	ComboSetting(UINT ctrlId, UINT sectionId, UINT valueId);
	virtual ~ComboSetting();

	bool readFromStorage();
	bool saveToStorage() const;

	void DDX_Exchange(CDataExchange* pDX);

	bool operator == (ComboSetting const& rhs) const;

	void copyValue(ComboSetting const& rhs);
protected:
	virtual int storedToWnd(int value) const;
	virtual int wndToStored(int value) const;

	UINT getSectionId() const;
	UINT getValueId() const;
	int getValue() const;
private:
	UINT sectionId_;
	UINT valueId_;
	int value_;

};

#endif // !defined(AFX_COMBOSETTING_H__B8D6D4A3_8724_4D16_BEFD_74D9861D7A60__INCLUDED_)
