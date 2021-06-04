// FindLinkByPoint.cpp: implementation of the FindLinkByPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FindLinkByPoint.h"
//#include "Game/GameInclude.h"
#include "FindLinkInTriggerByPoint.h"
#include "custom_algorithm.h"
#include <algorithm>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FindLinkByPoint::FindLinkByPoint()
{

}

FindLinkByPoint::~FindLinkByPoint()
{

}

std::pair<int, int> FindLinkByPoint::run(TriggerChain const& chain, 
										 CPoint const& point)
{
	typedef TriggerList::const_iterator Iterator;
	TriggerList const& triggers = chain.triggers;
	Iterator i(triggers.begin()), e(triggers.end());

	int linkIndex = -1;
	FindLinkInTriggerByPoint test(point, linkIndex);
	Iterator res = std::find_if( i, e, test);
	if (res == e)
		return std::make_pair(-1, -1);
	return std::make_pair(test.getLinkIndex(), std::distance(i, res));
}
