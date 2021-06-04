// ShadowSamples.cpp: implementation of the ShadowSamples class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "ShadowSamples.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ShadowSamples::ShadowSamples(UINT ctrlId, UINT sectionId)
: RadioSetting(ctrlId, sectionId, IDS_KEY_SHADOW_SAMPLES)
{

}

ShadowSamples::~ShadowSamples()
{

}

int ShadowSamples::wndToStored(int value) const{
	switch(value) {
	case 0: return 4;
	case 1: return 16;
	}
	ASSERT(0);
	return -1;
}

int ShadowSamples::storedToWnd(int value) const{
	switch(value) {
	case 4: return 0;
	case 16: return 1;
	}
	ASSERT(0);
	return -1;
}
