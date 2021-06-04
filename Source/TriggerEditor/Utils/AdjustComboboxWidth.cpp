// AdjustComboboxWidth.cpp: implementation of the AdjustComboboxWidth class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AdjustComboboxWidth.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void AdjustComboboxWidth::adjust(CComboBox& box)
{
	CString      str;
	CSize      sz;
	int      dx = 0;
	TEXTMETRIC   tm;
	CDC*      pDC = box.GetDC();
	CFont*      pFont = box.GetFont();

	// Select the listbox font, save the old font
	CFont* pOldFont = pDC->SelectObject(pFont);
	// Get the text metrics for avg char width
	pDC->GetTextMetrics(&tm);

	for (int i = 0; i < box.GetCount(); i++)
	{
		box.GetLBText(i, str);
		sz = pDC->GetTextExtent(str);

		// Add the avg width to prevent clipping
		sz.cx += tm.tmAveCharWidth;

		if (sz.cx > dx)
			dx = sz.cx;
	}
	// Select the old font back into the DC
	pDC->SelectObject(pOldFont);
	box.ReleaseDC(pDC);

	// Adjust the width for the vertical scroll bar and the left and right border.
	dx += ::GetSystemMetrics(SM_CXVSCROLL) + 2*::GetSystemMetrics(SM_CXEDGE);

	// Set the width of the list box so that every item is completely visible.
	box.SetDroppedWidth(dx);
}
