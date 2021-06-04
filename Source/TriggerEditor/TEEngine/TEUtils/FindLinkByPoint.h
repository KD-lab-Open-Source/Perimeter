// FindLinkByPoint.h: interface for the FindLinkByPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FindLinkByPoint_H__CB814EE9_52D1_43FD_8588_81150A650753__INCLUDED_)
#define AFX_FindLinkByPoint_H__CB814EE9_52D1_43FD_8588_81150A650753__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <utility>
#include "TriggerExport.h"

class FindLinkByPoint  
{
	FindLinkByPoint();
	~FindLinkByPoint();
public:
	static std::pair<int, int> run(TriggerChain const& chain, 
									CPoint const& point);
};

#endif // !defined(AFX_FindLinkByPoint_H__CB814EE9_52D1_43FD_8588_81150A650753__INCLUDED_)
