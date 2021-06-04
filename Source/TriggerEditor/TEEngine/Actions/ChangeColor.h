// ChangeColor.h: interface for the ChangeColor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANGECOLOR_H__7B735CF2_33DF_4DB0_9F98_0C4BE73F225C__INCLUDED_)
#define AFX_CHANGECOLOR_H__7B735CF2_33DF_4DB0_9F98_0C4BE73F225C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TriggerExport.h"
class ChangeColor  
{
public:
	ChangeColor(TriggerChain& chain, int triggerIndex, COLORREF color);
	~ChangeColor();
	bool operator()();
	static bool run(TriggerChain& chain, 
					int triggerIndex, 
					COLORREF color);
private:
	COLORREF color_;
	int		triggerIndex_;
	TriggerChain& chain_;
};

#endif // !defined(AFX_CHANGECOLOR_H__7B735CF2_33DF_4DB0_9F98_0C4BE73F225C__INCLUDED_)
