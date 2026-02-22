// TreeListTipCtrl.cpp : implementation file
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

static lpfnUpdateLayeredWindow g_lpfnUpdateLayeredWindow = NULL;
static lpfnSetLayeredWindowAttributes g_lpfnSetLayeredWindowAttributes = NULL;

BOOL InitLayeredWindows()
{
	if( g_lpfnUpdateLayeredWindow == NULL || g_lpfnSetLayeredWindowAttributes == NULL )
	{
		HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));

		g_lpfnUpdateLayeredWindow =	(lpfnUpdateLayeredWindow)GetProcAddress( hUser32, _T("UpdateLayeredWindow") );
		g_lpfnSetLayeredWindowAttributes = (lpfnSetLayeredWindowAttributes)GetProcAddress( hUser32, _T("SetLayeredWindowAttributes") );

		if( g_lpfnUpdateLayeredWindow == NULL || g_lpfnSetLayeredWindowAttributes == NULL )
			return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CTreeListTipCtrl

CTreeListTipCtrl::CTreeListTipCtrl() : 
	m_dwFadeStep( 10L ), 
	m_dwFadeInTime( 500L ), 
	m_dwFadeOutTime( 200L ),
	m_bLayeredWindows( FALSE ),
	m_nAlpha( 0 ),
	m_rcItem( 0, 0, 0, 0 ),
	m_rcTip( 0, 0, 0, 0 ),
	m_uFormat( 0 ),
	m_pTreeListCtrl( NULL )
{
	// construction
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if( ::GetClassInfo( hInst, TREELISTTIPCTRL_CLASSNAME, &wndcls ) == FALSE )
	{
		wndcls.style =  CS_DBLCLKS ;
		wndcls.lpfnWndProc		= ::DefWindowProc;
		wndcls.cbClsExtra		= 0;
		wndcls.cbWndExtra		= 0;
		wndcls.hInstance		= hInst;
		wndcls.hIcon			= NULL;
		wndcls.hCursor			= LoadCursor(hInst, _T("IDC_ARROW"));
		wndcls.hbrBackground	= (HBRUSH)(COLOR_INFOBK+1);
		wndcls.lpszMenuName		= NULL;
		wndcls.lpszClassName	= TREELISTTIPCTRL_CLASSNAME;

		if( !AfxRegisterClass(&wndcls) )
			AfxThrowResourceException();
	}

	// for layer windows
	m_bLayeredWindows = InitLayeredWindows();
}

CTreeListTipCtrl::~CTreeListTipCtrl()
{
	// deconstruction
	m_Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CTreeListTipCtrl, CWnd)
	//{{AFX_MSG_MAP(CTreeListTipCtrl)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListTipCtrl message handlers
BOOL CTreeListTipCtrl::Create( CTreeListCtrl* pTreeListCtrl )
{
	// create tip
	ASSERT_VALID( pTreeListCtrl );

	m_pTreeListCtrl = pTreeListCtrl;

	DWORD dwExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT;
	DWORD dwStyle	= WS_BORDER | WS_POPUP;
	CRect rcRect ( 0, 0, 0, 0 );

	if( m_bLayeredWindows )
		dwExStyle |= WS_EX_LAYERED;

	return CreateEx( dwExStyle, TREELISTTIPCTRL_CLASSNAME, NULL, dwStyle, rcRect, pTreeListCtrl, 0, NULL ); 
}

BOOL CTreeListTipCtrl::Show( CRect rcItem, LPCTSTR lpszText, UINT uFormat )
{
	// show tip
	BOOL bResult = FALSE;

	ASSERT(::IsWindow(GetSafeHwnd()));

	if(	rcItem.IsRectEmpty() )// || GetFocus() == NULL ) 
	{
		Hide();
		return FALSE;
	}

	if( m_rcItem != rcItem )// || !IsWindowVisible() )
	{
		m_rcItem	= rcItem;

		CClientDC dc(this);
		CFont *pOldFont = dc.SelectObject( &m_Font );
		CSize size = dc.GetTextExtent(lpszText, static_cast<int>(_tcslen(lpszText)));
		dc.SelectObject(pOldFont);

		if( size.cx + 5 > rcItem.Width() )
		{
			m_strText = lpszText;
			m_uFormat = uFormat;

			m_rcTip		= rcItem;
			m_pTreeListCtrl->ClientToScreen(m_rcTip);
				
			m_rcTip.left -= 1;
			m_rcTip.top -= 1;
			if( uFormat&DT_RIGHT )
			{
				m_rcTip.left = m_rcTip.right - size.cx - 6;
			}
			else if( uFormat&DT_CENTER )
			{
				int nFix = ( size.cx - m_rcItem.Width() ) / 2;
				m_rcTip.left = m_rcTip.left - nFix - 3;
				m_rcTip.right = m_rcTip.right + nFix + 2;
			}
			else // if( uFormat&DT_LEFT )
			{
				m_rcTip.right = m_rcTip.left + size.cx + 5;
			}

			Invalidate();
			UpdateWindow();

			KillTimer( 2 );
			SetWindowPos( &wndTop, m_rcTip.left, m_rcTip.top, m_rcTip.Width(), m_rcTip.Height(),
				SWP_NOOWNERZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE	);

			if(m_bLayeredWindows)
			{
				if(m_nAlpha < 255)
				{
					KillTimer( 2 );
					SetTimer( 1, m_dwFadeInTime/(255/m_dwFadeStep), NULL );
				}
				g_lpfnSetLayeredWindowAttributes( GetSafeHwnd(), RGB(0xFF, 0, 0xFF), m_nAlpha, ULW_ALPHA );
			}

			bResult = TRUE;
		}
		else
		{
			Hide();
			bResult = FALSE;
		}
	}
	else
	{
		bResult = TRUE;
	}

	return bResult;

}

void CTreeListTipCtrl::Hide()
{
	// hide tip
	ASSERT(::IsWindow(GetSafeHwnd()));

	if ( IsWindowVisible() )
	{
		if( m_bLayeredWindows )
		{
			KillTimer( 1 );
			SetTimer( 2, m_dwFadeOutTime/(255/m_dwFadeStep), NULL );
		}
		else
		{
			ShowWindow( SW_HIDE );
		}
	}
}

BOOL CTreeListTipCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// post message of mouse and key to parent window
	DWORD	dwTick = 0;
	BOOL	bDoubleClick = FALSE;

/*
	CWnd*	pWnd;
	int		nHitTest;
	switch( pMsg->message )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:

	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:

	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:

	case WM_MOUSEMOVE:

	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCLBUTTONDBLCLK:

	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_NCRBUTTONDBLCLK:

	case WM_NCMBUTTONDOWN:
	case WM_NCMBUTTONUP:
	case WM_NCMBUTTONDBLCLK:

	case WM_NCMOUSEMOVE:

		POINTS points = MAKEPOINTS( pMsg->lParam );
		POINT pt;
		pt.x = points.x;
		pt.y = points.y;

		ClientToScreen( &pt );
		pWnd = WindowFromPoint( pt );

		if( pWnd == this )
			pWnd = m_pTreeListCtrl;

		if( pWnd != NULL )
		{
			nHitTest = (int)pWnd->SendMessage( WM_NCHITTEST, 0, MAKELONG( pt.x, pt.y ) );

			if( nHitTest == HTCLIENT )
			{
				switch( pMsg->message )
				{
				case WM_NCLBUTTONDOWN:		pMsg->message = WM_LBUTTONDOWN;		break;
				case WM_NCLBUTTONUP:		pMsg->message = WM_LBUTTONUP;		break;
				case WM_NCLBUTTONDBLCLK:	pMsg->message = WM_LBUTTONDBLCLK;	break;

				case WM_NCRBUTTONDOWN:		pMsg->message = WM_RBUTTONDOWN;		break;
				case WM_NCRBUTTONUP:		pMsg->message = WM_RBUTTONUP;		break;
				case WM_NCRBUTTONDBLCLK:	pMsg->message = WM_RBUTTONDBLCLK;	break;

				case WM_NCMBUTTONDOWN:		pMsg->message = WM_MBUTTONDOWN;		break;
				case WM_NCMBUTTONUP:		pMsg->message = WM_MBUTTONUP;		break;
				case WM_NCMBUTTONDBLCLK:	pMsg->message = WM_MBUTTONDBLCLK;	break;

				case WM_NCMOUSEMOVE:		pMsg->message = WM_MOUSEMOVE;		break;

				default:	break;
				}
			}
			else
			{
				switch( pMsg->message )
				{
				case WM_LBUTTONDOWN:		pMsg->message = WM_NCLBUTTONDOWN;	break;
				case WM_LBUTTONUP:			pMsg->message = WM_NCLBUTTONUP;		break;
				case WM_LBUTTONDBLCLK:		pMsg->message = WM_NCLBUTTONDBLCLK;	break;

				case WM_RBUTTONDOWN:		pMsg->message = WM_NCRBUTTONDOWN;	break;
				case WM_RBUTTONUP:			pMsg->message = WM_NCRBUTTONUP;		break;
				case WM_RBUTTONDBLCLK:		pMsg->message = WM_NCRBUTTONDBLCLK;	break;

				case WM_MBUTTONDOWN:		pMsg->message = WM_NCMBUTTONDOWN;	break;
				case WM_MBUTTONUP:			pMsg->message = WM_NCMBUTTONUP;		break;
				case WM_MBUTTONDBLCLK:		pMsg->message = WM_NCMBUTTONDBLCLK;	break;
	
				case WM_MOUSEMOVE:			pMsg->message = WM_NCMOUSEMOVE;		break;
	
				default:	break;
				}
			}

			pWnd->ScreenToClient( &pt );
			pMsg->lParam = MAKELONG( pt.x, pt.y );
			pWnd->SendMessage( pMsg->message, pMsg->wParam, pMsg->lParam );
			return TRUE;
		}
		break;

	default:
		;
	}
*/
	return CWnd::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void CTreeListTipCtrl::OnPaint() 
{
	// paint the control
	CPaintDC dc(this); // device context for painting

	CRect rcText;
	rcText.SetRect( 0, 0, m_rcTip.Width(), m_rcTip.Height() );
	dc.FillSolidRect( rcText, 0xF0FFFF );

	CFont* pOldFont = dc.SelectObject( &m_Font );
	int nBkMode = dc.SetBkMode( TRANSPARENT );
	rcText.DeflateRect( 2, 0, 3, 0 );
	dc.DrawText( m_strText, rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	dc.SetBkMode( nBkMode );
	dc.SelectObject( pOldFont );
}

void CTreeListTipCtrl::OnTimer(UINT nIDEvent) 
{
	// fade in/out timer
	switch( nIDEvent )
	{
	case 1:	// fade in
		ASSERT( m_bLayeredWindows == TRUE );

		if( m_nAlpha < 255 )
		{
			Invalidate();
			UpdateWindow();
			m_nAlpha += m_dwFadeStep;
			if( m_nAlpha >= 255 )
			{
				m_nAlpha = 255;
				KillTimer( 1 );
			}
			g_lpfnSetLayeredWindowAttributes( GetSafeHwnd(), 0xFF00FF, m_nAlpha, ULW_ALPHA );
		}
		else
		{
			m_nAlpha = 255;
			KillTimer( 1 );
		}

		break;
	
	case 2: // fade out
		ASSERT( m_bLayeredWindows == TRUE );

		if( m_nAlpha > 0 )
		{
			Invalidate();
			UpdateWindow();
			m_nAlpha -= m_dwFadeStep;
			if( m_nAlpha <= 0 )
			{
				m_nAlpha = 0;
				KillTimer( 2 );
				if( IsWindowVisible() )
					ShowWindow( SW_HIDE );
			}
			g_lpfnSetLayeredWindowAttributes( GetSafeHwnd(), 0xFF00FF, m_nAlpha, ULW_ALPHA );
		}
		else 
		{
			m_nAlpha = 0;
			KillTimer( 2 );
			if( IsWindowVisible() )
				ShowWindow( SW_HIDE );
			g_lpfnSetLayeredWindowAttributes( GetSafeHwnd(), 0xFF00FF, m_nAlpha, ULW_ALPHA );
		}

		break;

	default:
		break;
	}

	CWnd::OnTimer(nIDEvent);
}

int CTreeListTipCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	// active a window with mouse click
	// * do NOT write into PreTranslateMessage
	// * because MFC call it first.
	return MA_NOACTIVATE;
}

void CTreeListTipCtrl::SetFont()
{
	// set font with font of parent window
	if( m_pTreeListCtrl == NULL )
		return;

	CFont* pFont = m_pTreeListCtrl->GetFont();
	if( pFont == NULL )
		return;

	LOGFONT lf;
	pFont->GetLogFont(&lf);
	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&lf);
}

CFont* CTreeListTipCtrl::GetFont()
{
	// get font
	return &m_Font;
}

