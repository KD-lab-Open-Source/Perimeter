#include "stdafx.h"
#include "resource.h"
#include "SlotMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSlotMenu::CSlotMenu() {
}

CSlotMenu::~CSlotMenu() {
}

void CSlotMenu::init() {
	VERIFY( parent.LoadMenu(IDR_ACTION_MENU) );
	Attach(parent.GetSubMenu(0)->m_hMenu);

	MENUITEMINFO menuItemInfo;
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = MIIM_TYPE;
	menuItemInfo.fType = MFT_OWNERDRAW;
	for (int i = 0; i < 3; i++) {
		SetMenuItemInfo(m_hMenu, i, true, &menuItemInfo);
	}

}

void CSlotMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS) {
	CString str;
	CDC* pDC = CDC::FromHandle(GetDC(0));
	
	GetMenuString(lpMIS->itemID, str, MF_BYCOMMAND);
	CSize size = pDC->GetOutputTextExtent(str);
	lpMIS->itemWidth = size.cx + 2;
	lpMIS->itemHeight = size.cy + 2;
}
	

void CSlotMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS) {
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	CString str;
	GetMenuString(lpDIS->itemID, str, MF_BYCOMMAND);
	COLORREF bg;
	COLORREF fg;

	switch (lpDIS->itemID) {
		case ID_CLEAR_SLOT:
			bg = GetSysColor(COLOR_MENU);
			fg = GetSysColor(COLOR_MENUTEXT);
			break;
		case ID_PAINTER_SLOT:
			bg = RGB(128, 0, 0);
			fg = RGB(255, 255, 255);
			break;
		case ID_MIXER_SLOT:
			bg = RGB(0, 0, 128);
			fg = RGB(255, 255, 255);
			break;
	}

	if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		CBrush br(bg);
		pDC->FillRect(&lpDIS->rcItem, &br);
	}

	if ((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		// item has been selected - hilite frame
		COLORREF crHilite = RGB( 255-GetRValue(bg), 255-GetGValue(bg), 255-GetBValue(bg) );
		CBrush br(crHilite);
		pDC->FrameRect(&lpDIS->rcItem, &br);
	}

	if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
	{
		// Item has been de-selected -- remove frame
		CBrush br(bg);
		pDC->FrameRect(&lpDIS->rcItem, &br);
	}

	CPoint p(lpDIS->rcItem.left + 2, lpDIS->rcItem.top + 2);
	CSize s(lpDIS->rcItem.right - p.x, lpDIS->rcItem.bottom - p.y);
	pDC->SetTextColor(fg);
	pDC->SetBkColor(bg);
	pDC->DrawState(p, s, str, DST_TEXT | DSS_NORMAL, FALSE, 0, (HBRUSH)NULL);
}
