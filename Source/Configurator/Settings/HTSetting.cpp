// HTSetting.cpp: implementation of the HTSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "HTSetting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HTSetting::HTSetting(UINT ctrlId)
: RadioSetting(ctrlId, IDS_SECTION_GAME, IDS_KEY_MULTIPROC)
{

}

HTSetting::~HTSetting()
{

}
