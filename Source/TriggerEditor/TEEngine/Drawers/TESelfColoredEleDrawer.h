// TESelfColoredEleDrawer.h: interface for the TESelfColoredEleDrawer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESELFCOLOREDELEDRAWER_H__17E5615E_1674_4F02_B6C8_900CC24AB308__INCLUDED_)
#define AFX_TESELFCOLOREDELEDRAWER_H__17E5615E_1674_4F02_B6C8_900CC24AB308__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TEConditionColoredEleDrawer.h"

class TESelfColoredEleDrawer : public TEConditionColoredEleDrawer  
{
public:
	TESelfColoredEleDrawer();
	virtual ~TESelfColoredEleDrawer();
protected:
	virtual HBRUSH	SelectTriggerBackBrush(Trigger const& trigger) const;
};

#endif // !defined(AFX_TESELFCOLOREDELEDRAWER_H__17E5615E_1674_4F02_B6C8_900CC24AB308__INCLUDED_)
