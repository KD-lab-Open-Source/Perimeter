// TESelfColoredEleDrawer.cpp: implementation of the TESelfColoredEleDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TESelfColoredEleDrawer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TESelfColoredEleDrawer::TESelfColoredEleDrawer()
{

}

TESelfColoredEleDrawer::~TESelfColoredEleDrawer()
{

}

HBRUSH	TESelfColoredEleDrawer::SelectTriggerBackBrush(
								Trigger const& trigger) const
{
	if (!trigger.color())
		return static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	static CBrush br;
	br.DeleteObject();
	br.CreateSolidBrush(trigger.color());
	return br;
}
