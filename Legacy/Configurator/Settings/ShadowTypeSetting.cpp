// ShadowTypeSetting.cpp: implementation of the ShadowTypeSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "ShadowTypeSetting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ShadowTypeSetting::ShadowTypeSetting(UINT ctrlId, UINT sectionId)
: RadioSetting(ctrlId, sectionId, IDS_KEY_SHADOW_TYPE)
{

}

ShadowTypeSetting::~ShadowTypeSetting()
{

}

int ShadowTypeSetting::wndToStored(int value) const
{
	return value + 1;
}

int ShadowTypeSetting::storedToWnd(int value) const
{
	return value - 1;
}
