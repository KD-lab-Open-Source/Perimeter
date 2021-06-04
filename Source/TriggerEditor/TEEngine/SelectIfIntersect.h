// SelectIfIntersect.h: interface for the SelectIfIntersect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELECTIFINTERSECT_H__F78B2E9E_DD5A_4AED_B4CC_3A1574821819__INCLUDED_)
#define AFX_SELECTIFINTERSECT_H__F78B2E9E_DD5A_4AED_B4CC_3A1574821819__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TriggerExport.h"
class SelectionManager;
class SelectIfIntersect
{
	CRect const& rect_;
	SelectionManager& selectionManager_;
public:
	SelectIfIntersect(CRect const& rect, SelectionManager& selectionManager);
	void operator()(Trigger& trigger) const;
};

#endif // !defined(AFX_SELECTIFINTERSECT_H__F78B2E9E_DD5A_4AED_B4CC_3A1574821819__INCLUDED_)
