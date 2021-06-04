// FindTrigger.cpp: implementation of the FindTrigger class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FindTrigger.h"

#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

class FindTrigger::TestTrigger
{
public:
	TestTrigger(CPoint const& testPoint):
	  testPoint_(testPoint)
	  {}
	bool operator()(Trigger const& trigger) const{
		return PtInRect(&trigger.boundingRect(), testPoint_);
	}
private:
	CPoint const& testPoint_;
};
FindTrigger::FindTrigger()
{

}

FindTrigger::~FindTrigger()
{

}

int FindTrigger::run(TriggerChain const& chain, CPoint const& point)
{
	typedef TriggerList::const_iterator Iterator;

	TriggerList const& triggerList = chain.triggers;
	Iterator i(triggerList.begin()), e(triggerList.end());

	Iterator res = std::find_if(i, e, TestTrigger(point));
	if (res == e)
		return -1;
	return std::distance(i, res);
}
