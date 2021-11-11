// ScrollRateSetting.cpp: implementation of the ScrollRateSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "ScrollRateSetting.h"
#include <xutil.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ScrollRateSetting::ScrollRateSetting(UINT ctrlId)
: SliderSetting(ctrlId, IDS_SECTION_GAME, IDS_KEY_SCROLL_RATE)
{

}

ScrollRateSetting::~ScrollRateSetting()
{

}

int ScrollRateSetting::storeValueToWnd(float storeValue) const
{
	return xm::round(storeValue * 10.f);
}

float ScrollRateSetting::wndValueToStore(int value) const{
	return value/10.f;
}

void ScrollRateSetting::initControl(CWnd* parent, LocaleMngr const& LocaleMngr) const
{
	if (getCtrlId() == UINT(-1))
		return;
	CSliderCtrl* slider = 
		reinterpret_cast<CSliderCtrl*>(parent->GetDlgItem(getCtrlId()));
	if (!slider)
		return;
	slider->SetRange(10, 200);
}
