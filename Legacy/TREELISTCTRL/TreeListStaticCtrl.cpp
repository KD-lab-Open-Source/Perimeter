// TreeListStaticCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TreeListHeaderCtrl.h"
#include "TreeListTipCtrl.h"
#include "TreeListStaticCtrl.h"
#include "TreeListEditCtrl.h"
#include "TreeListComboCtrl.h"
#include "TreeListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeListStaticCtrl

CTreeListStaticCtrl::CTreeListStaticCtrl()
{
}

CTreeListStaticCtrl::~CTreeListStaticCtrl()
{
}


BEGIN_MESSAGE_MAP(CTreeListStaticCtrl, CStatic)
	//{{AFX_MSG_MAP(CTreeListStaticCtrl)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListStaticCtrl message handlers

int CTreeListStaticCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pTreeListCtrl = (CTreeListCtrl*)GetParent();
	
	return 0;
}

void CTreeListStaticCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CStatic::OnKillFocus(pNewWnd);
	
	m_pTreeListCtrl->SetCtrlFocus( pNewWnd, FALSE );
}

void CTreeListStaticCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CString strText;
	GetWindowText( strText );
	
	CRect rcClient;
	GetClientRect( rcClient );
	dc.FillSolidRect( rcClient, GetSysColor( COLOR_BTNFACE ) );
	
	rcClient.DeflateRect( 3, 1, 4, 0 );
	if( !rcClient.IsRectEmpty() )
	{
		UINT nFormat = DT_VCENTER | DT_SINGLELINE;
		if( GetStyle()&SS_LEFT )
			nFormat |= DT_LEFT;
		else if( GetStyle()&SS_RIGHT )
			nFormat |= DT_RIGHT;
		else if( GetStyle()&SS_CENTER )
			nFormat |= DT_CENTER;
		else
			nFormat |= DT_LEFT;

		CFont* pOldFont = dc.SelectObject( GetFont() );
		int nBkMode = dc.SetBkMode( TRANSPARENT );
		dc.DrawText( strText, rcClient, nFormat );
		dc.SelectObject( pOldFont );
		dc.SetBkMode( nBkMode );
	}

}

BOOL CTreeListStaticCtrl::PreTranslateMessage(MSG* pMsg) 
{
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			if( PreTranslateKeyDownMessage( pMsg ) )
				return TRUE;
		}
		break;

	default:
		break;
	}
	
	return CStatic::PreTranslateMessage(pMsg);
}

BOOL CTreeListStaticCtrl::PreTranslateKeyDownMessage( MSG* pMsg )
{
	switch( pMsg->wParam )
	{
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
		return TRUE;
		break;

	default:
		break;
	}

	return FALSE;
}
