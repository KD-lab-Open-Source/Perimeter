// BoundingRectCalculator.h: interface for the BoundingRectCalculator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOUNDINGRECTCALCULATOR_H__C10359BF_7F53_4CCE_BFB1_8257F60FABFC__INCLUDED_)
#define AFX_BOUNDINGRECTCALCULATOR_H__C10359BF_7F53_4CCE_BFB1_8257F60FABFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TEEngine/TEGrid.h"
#include "TriggerExport.h"

class BoundingRectCalculator  
{
	TEGrid const& grid_;
public:
	BoundingRectCalculator(TEGrid const& grid):grid_(grid){}

	static void run(TEGrid const& grid, Trigger& trigger);
	void operator()(Trigger& trigger) const;
};

#endif // !defined(AFX_BOUNDINGRECTCALCULATOR_H__C10359BF_7F53_4CCE_BFB1_8257F60FABFC__INCLUDED_)
