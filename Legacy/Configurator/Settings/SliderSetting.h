// SliderSetting.h: interface for the SliderSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLIDERSETTING_H__404EBA7F_F911_4BFB_AECD_550B23AEEB23__INCLUDED_)
#define AFX_SLIDERSETTING_H__404EBA7F_F911_4BFB_AECD_550B23AEEB23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SettingBase.h"

class SliderSetting : public SettingBase  
{
public:
	SliderSetting(UINT ctrlId, UINT sectionId, UINT keyId);
	virtual ~SliderSetting();
	bool readFromStorage();
	bool saveToStorage() const;

	void DDX_Exchange(CDataExchange* pDX);
	void initControl(CWnd* parent, LocaleMngr const& locMngr) const;

	bool operator == (SliderSetting const& rhs) const;
	void copyValue(SliderSetting const& rhs);
protected:
	UINT getSectionId() const;
	UINT getValueId() const;
	int getValue() const;

	virtual int storeValueToWnd(float storeValue) const;
	virtual float wndValueToStore(int value) const;
private:
	UINT sectionId_;
	UINT keyId_;
	int value_;
};

#endif // !defined(AFX_SLIDERSETTING_H__404EBA7F_F911_4BFB_AECD_550B23AEEB23__INCLUDED_)
