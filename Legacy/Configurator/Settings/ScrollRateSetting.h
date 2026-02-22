// ScrollRateSetting.h: interface for the ScrollRateSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCROLLRATESETTING_H__7C03C8C8_0779_4853_B140_E9B6677911BB__INCLUDED_)
#define AFX_SCROLLRATESETTING_H__7C03C8C8_0779_4853_B140_E9B6677911BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SliderSetting.h"

class ScrollRateSetting : public SliderSetting  
{
public:
	ScrollRateSetting(UINT ctrlId);
	virtual ~ScrollRateSetting();

protected:
	virtual int storeValueToWnd(float storeValue) const;
	virtual float wndValueToStore(int value) const;
	virtual void initControl(CWnd* parent, LocaleMngr const& LocaleMngr) const;
};

#endif // !defined(AFX_SCROLLRATESETTING_H__7C03C8C8_0779_4853_B140_E9B6677911BB__INCLUDED_)
