// FindLinkInTriggerByPoint.cpp: implementation of the FindLinkInTriggerByPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FindLinkInTriggerByPoint.h"
#include "TEEngine/te_helpers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

class FindLinkInTriggerByPoint::TestLink
{
public:
	TestLink(CPoint const& parentCenter, CPoint const& testPoint)
		:parentCenter_(parentCenter)
		, testPoint_(testPoint)
	{}
	bool operator()(TriggerLink const& link){
		Trigger const& childTrigger = *link.child;
		te_h::TRIANGLE linkTriangle;
		te_h::getLinkTriangle(parentCenter_ + link.parentOffset(),
			te_h::get_center(childTrigger.boundingRect()) 
			+ link.childOffset(), linkTriangle);

		return (te_h::pointInTrg(testPoint_, linkTriangle));
	}
private:
	CPoint const& parentCenter_;
	CPoint const& testPoint_;
};

FindLinkInTriggerByPoint::FindLinkInTriggerByPoint(CPoint const& testPoint, 
												   int &linkIndex):
  linkIndex_(linkIndex)
, testPoint_(testPoint)
{

}

FindLinkInTriggerByPoint::~FindLinkInTriggerByPoint()
{

}

int FindLinkInTriggerByPoint::getLinkIndex() const{
	return linkIndex_;
}

bool FindLinkInTriggerByPoint::operator()(argument_type& trigger) 
{
	OutcomingLinksList const& lst = trigger.outcomingLinks();
	CPoint const pt_from(te_h::get_center(trigger.boundingRect()));
	OutcomingLinksList::const_iterator res = std::find_if(lst.begin(), lst.end(), TestLink(pt_from, testPoint_));
	if (res != lst.end())
	{
		linkIndex_ = std::distance(lst.begin(), res);
		return true;
	}
	return false;
}
