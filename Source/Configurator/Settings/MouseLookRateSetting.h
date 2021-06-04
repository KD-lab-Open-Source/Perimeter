// MouseLookRateSetting.h: interface for the MouseLookRateSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUSELOOKRATESETTING_H__8B194354_EA11_41B6_8AEB_BBABAE79BB9D__INCLUDED_)
#define AFX_MOUSELOOKRATESETTING_H__8B194354_EA11_41B6_8AEB_BBABAE79BB9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SliderSetting.h"

class MouseLookRateSetting : public SliderSetting  
{
public:
	MouseLookRateSetting(UINT ctrlId);
	virtual ~MouseLookRateSetting();
protected:
	virtual int storeValueToWnd(float storeValue) const;
	virtual float wndValueToStore(int value) const;
	virtual void initControl(CWnd* parent, LocaleMngr const& LocaleMngr) const;

};

#endif // !defined(AFX_MOUSELOOKRATESETTING_H__8B194354_EA11_41B6_8AEB_BBABAE79BB9D__INCLUDED_)
