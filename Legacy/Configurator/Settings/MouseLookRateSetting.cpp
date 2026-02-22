// MouseLookRateSetting.cpp: implementation of the MouseLookRateSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "MouseLookRateSetting.h"
#include <xutil.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MouseLookRateSetting::MouseLookRateSetting(UINT ctrlId)
:SliderSetting(ctrlId, IDS_SECTION_GAME, IDS_KEY_MOUSE_LOOK_RATE)
{

}

MouseLookRateSetting::~MouseLookRateSetting()
{

}

int MouseLookRateSetting::storeValueToWnd(float storeValue) const
{
//	if (storeValue < 10e-5)
//		storeValue = .0f;
	return xm::round(storeValue * 100.f);
}

float MouseLookRateSetting::wndValueToStore(int value) const{

	return (value/100.f );
}

void MouseLookRateSetting::initControl(CWnd* parent, LocaleMngr const& LocaleMngr) const
{
	if (getCtrlId() == UINT(-1))
		return;
	CSliderCtrl* slider = 
		reinterpret_cast<CSliderCtrl*>(parent->GetDlgItem(getCtrlId()));
	if (!slider)
		return;
	slider->SetRange(0, 600);
}
