// CheckSetting.h: interface for the CheckSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHECKSETTING_H__02BEBFEB_FEC8_4623_988D_96B2350D75C2__INCLUDED_)
#define AFX_CHECKSETTING_H__02BEBFEB_FEC8_4623_988D_96B2350D75C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SettingBase.h"

class CheckSetting : public SettingBase  
{
public:
	CheckSetting(UINT ctrlId, UINT sectionId, UINT valueId);
	CheckSetting(CheckSetting const& rhs);
	virtual ~CheckSetting();

	void operator=(CheckSetting const& rhs);

	bool readFromStorage();
	bool saveToStorage() const;

	void DDX_Exchange(CDataExchange* pDX);

	bool operator == (CheckSetting const& rhs) const;

	void copyValue(CheckSetting const& rhs);
protected:
	UINT getSectionId() const;
	UINT getValueId() const;
	int getValue() const;
private:
	UINT sectionId_;
	UINT valueId_;
	int value_;
};

#endif // !defined(AFX_CHECKSETTING_H__02BEBFEB_FEC8_4623_988D_96B2350D75C2__INCLUDED_)
