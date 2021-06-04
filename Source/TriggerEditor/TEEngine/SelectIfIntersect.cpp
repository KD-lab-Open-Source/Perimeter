// SelectIfIntersect.cpp: implementation of the SelectIfIntersect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TEEngine/SelectIfIntersect.h"
#include "TEEngine/SelectionManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SelectIfIntersect::SelectIfIntersect(CRect const& rect, 
									 SelectionManager& selectionManager):
  rect_(rect)
, selectionManager_(selectionManager)
{
}

void SelectIfIntersect::operator()(Trigger& trigger) const{
	CRect inters;
	CRect const boundingRect(trigger.boundingRect());
	inters.IntersectRect(rect_, &boundingRect);
	if (!inters.IsRectEmpty())
		selectionManager_.select(selectionManager_.getTriggerChain()->triggerIndex(trigger));

}
