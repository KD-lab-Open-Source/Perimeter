// SlotButton.cpp : implementation file
//

#include "stdafx.h"
#include "GeoTool.h"
#include "SlotButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSlotButton

const COLORREF CSlotButton::PAINTER_BG_COLOR = RGB(128, 0, 0);
const COLORREF CSlotButton::PAINTER_FG_COLOR = RGB(255, 255, 255);
const COLORREF CSlotButton::MIXER_BG_COLOR = RGB(0, 0, 128);
const COLORREF CSlotButton::MIXER_FG_COLOR = RGB(255, 255, 255);
const COLORREF CSlotButton::PAINTER_BG_COLOR_HI = RGB(192, 0, 0);
const COLORREF CSlotButton::PAINTER_FG_COLOR_HI = RGB(255, 255, 255);
const COLORREF CSlotButton::MIXER_BG_COLOR_HI = RGB(0, 0, 192);
const COLORREF CSlotButton::MIXER_FG_COLOR_HI = RGB(255, 255, 255);

CSlotButton::CSlotButton()
{
	slot = 0;
	isMixer = false;
	textureItem = NULL;
	locationItem = NULL;
}

CSlotButton::~CSlotButton()
{
}


BEGIN_MESSAGE_MAP(CSlotButton, CButton)
	//{{AFX_MSG_MAP(CSlotButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlotButton message handlers

bool CSlotButton::isMixerButton() {
	return isMixer;
}

HTREEITEM CSlotButton::getTextureItem() {
	return textureItem;
}

HTREEITEM CSlotButton::getLocationItem() {
	return locationItem;
}

Slot* CSlotButton::getSlot() {
	return slot;
}

void CSlotButton::setMixer(bool isMixerButton) {
	isMixer = isMixerButton;
	updateText();
}

void CSlotButton::setTextureItem(HTREEITEM txItem) {
	textureItem = txItem;
	updateText();
}

void CSlotButton::setLocationItem(HTREEITEM locItem) {
	locationItem = locItem;
}

void CSlotButton::setSlot(Slot* buttonSlot) {
	slot = buttonSlot;
}

void CSlotButton::updateText() {
	if (slot) {
		SetWindowText( theApp.slotPropertiesDialog.treeManager->getItemName(textureItem).c_str() );
	} else {
		SetWindowText( "Empty" );
	}
}

void CSlotButton::OnClicked() 
{
	theApp.slotsDialog.setSelectedButton(this);
}


void CSlotButton::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CButton::OnRButtonDown(nFlags, point);
	theApp.slotsDialog.showPopup(this, point);
}

void CSlotButton::PostNcDestroy() 
{
	CButton::PostNcDestroy();
	delete this;
}

void CSlotButton::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	COLORREF bg;
	COLORREF fg;
	int type = DFCS_BUTTONPUSH;

	if (lpDIS->itemState & ODS_SELECTED) {
		type |= DFCS_PUSHED;
	} 

	if (lpDIS->hwndItem == theApp.slotsDialog.getSelectedButton()->m_hWnd) {
		type |= DFCS_CHECKED;
		if (slot) {
			if (isMixer) {
				fg = MIXER_FG_COLOR_HI;
				bg = MIXER_BG_COLOR_HI;
			} else {
				fg = PAINTER_FG_COLOR_HI;
				bg = PAINTER_BG_COLOR_HI;
			}
		} else {
			fg = GetSysColor(COLOR_BTNTEXT);
			bg = GetSysColor(COLOR_BTNHIGHLIGHT);
		}
	} else {
		if (slot) {
			if (isMixer) {
				fg = MIXER_FG_COLOR;
				bg = MIXER_BG_COLOR;
			} else {
				fg = PAINTER_FG_COLOR;
				bg = PAINTER_BG_COLOR;
			}
		} else {
			fg = GetSysColor(COLOR_BTNTEXT);
			bg = GetSysColor(COLOR_BTNFACE);
		}
	}

	pDC->DrawFrameControl(&lpDIS->rcItem, DFC_BUTTON, type);

	sRect rect(lpDIS->rcItem);
	rect.left += 3;
	rect.right -= 3;
	rect.top += 3;
	rect.bottom -= 3;
	CBrush br(bg);
	pDC->FillRect(&rect, &br);
	

	CString str;
	GetWindowText(str);

	CPoint p(lpDIS->rcItem.left + 4, lpDIS->rcItem.top + 4);
	CSize s(lpDIS->rcItem.right - p.x, lpDIS->rcItem.bottom - p.y);
	pDC->SetTextColor(fg);
	pDC->SetBkColor(bg);
	pDC->DrawState(p, s, str, DST_TEXT | DSS_NORMAL, FALSE, 0, (HBRUSH)NULL);

}
