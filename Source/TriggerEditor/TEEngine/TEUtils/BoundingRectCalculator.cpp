// BoundingRectCalculator.cpp: implementation of the BoundingRectCalculator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BoundingRectCalculator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void BoundingRectCalculator::operator()(Trigger& trigger) const{
	run(grid_, trigger);
}

void BoundingRectCalculator::run(TEGrid const& grid, Trigger& trigger)
{
	int const left = trigger.cellIndex().x*grid.getFullCellWidth();
	int const top = trigger.cellIndex().y*grid.getFullCellHeight();
	trigger.setBoundingRect(
		CRect(left, top, left + grid.getCellWidth(), top + grid.getCellHeight()));
}
