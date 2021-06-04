// FindTrigger.h: interface for the FindTrigger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDTRIGGER_H__2D0CAF78_B0E8_418B_98B8_D627DFA27EBF__INCLUDED_)
#define AFX_FINDTRIGGER_H__2D0CAF78_B0E8_418B_98B8_D627DFA27EBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TriggerExport.h"

class FindTrigger  
{
	FindTrigger();
	~FindTrigger();

	class TestTrigger;
public:
	static int run(TriggerChain const& chain, CPoint const& point);
};

#endif // !defined(AFX_FINDTRIGGER_H__2D0CAF78_B0E8_418B_98B8_D627DFA27EBF__INCLUDED_)
