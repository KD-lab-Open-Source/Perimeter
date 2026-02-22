// TLCDropWnd.cpp : implementation file
//

#include "stdafx.h"
#include "TreeListHeaderCtrl.h"
#include "TreeListTipCtrl.h"
#include "TreeListStaticCtrl.h"
#include "TreeListEditCtrl.h"
#include "TreeListComboCtrl.h"
#include "TreeListCtrl.h"
#include "TreeListDC.h"
#include "TLCDropWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static lpfnUpdateLayeredWindow g_lpfnUpdateLayeredWindow = NULL;
static lpfnSetLayeredWindowAttributes g_lpfnSetLayeredWindowAttributes = NULL;
/////////////////////////////////////////////////////////////////////////////
// CTLCDropWnd

CTLCDropWnd::CTLCDropWnd() :
	m_pTreeListCtrl( NULL )
{
	// register the window class
	WNDCLASS wndclass;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if(!(::GetClassInfo(hInst, TLCDROPWND_CLASSNAME, &wndclass)))
	{
		wndclass.style = CS_HREDRAW | CS_VREDRAW ; //CS_SAVEBITS ;
		wndclass.lpfnWndProc = ::DefWindowProc;
		wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInst;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor( hInst, _T("IDC_ARROW"));
		wndclass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1); 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = TLCDROPWND_CLASSNAME;
		if (!AfxRegisterClass(&wndclass))
			AfxThrowResourceException();
	}

	if( g_lpfnUpdateLayeredWindow == NULL || g_lpfnSetLayeredWindowAttributes == NULL )
	{
		HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));

		g_lpfnUpdateLayeredWindow =	(lpfnUpdateLayeredWindow)GetProcAddress( hUser32, _T("UpdateLayeredWindow") );
		g_lpfnSetLayeredWindowAttributes = (lpfnSetLayeredWindowAttributes)GetProcAddress( hUser32, _T("SetLayeredWindowAttributes") );

		if( g_lpfnUpdateLayeredWindow == NULL || g_lpfnSetLayeredWindowAttributes == NULL )
			m_bLayeredWindows = FALSE;
		else
			m_bLayeredWindows = TRUE;
	}
}

CTLCDropWnd::~CTLCDropWnd()
{
	m_rgn.DeleteObject();
}


BEGIN_MESSAGE_MAP(CTLCDropWnd, CWnd)
	//{{AFX_MSG_MAP(CTLCDropWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTLCDropWnd message handlers
BOOL CTLCDropWnd::Create( CTreeListCtrl* pTreeListCtrl )
{
	ASSERT_VALID( pTreeListCtrl );
	
	m_pTreeListCtrl = pTreeListCtrl;

	DWORD dwStyle	= WS_POPUP | WS_DISABLED;
	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;

	if( m_bLayeredWindows )
		dwExStyle |= WS_EX_LAYERED;

	if( !CreateEx( dwExStyle, TLCDROPWND_CLASSNAME, NULL, dwStyle, 
		0, 0, 80, 80, NULL, NULL, NULL ) )
		return FALSE;

	POINT pt[7] = {	0, 5,  6, 5,  6, 0,	 15, 8,	 6, 16,  6, 12,  0, 12 };

	m_rgn.CreatePolygonRgn( pt, 7, ALTERNATE );
	SetWindowRgn( m_rgn, FALSE );

	return TRUE;
}

BOOL CTLCDropWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CTLCDropWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

    if( m_pTreeListCtrl->GetState( TLS_OFFSCREENBUFFER ) )
    {
        CTreeListDC MemDC(&dc);
        DrawWnd(&MemDC);
    }
    else
	{
        DrawWnd(&dc);
	}
}

BOOL CTLCDropWnd::DrawWnd( CDC* pDC )
{
	CRect rcClient;
	GetClientRect( rcClient );
	pDC->FillSolidRect( rcClient , 0xFF0000 );
	return TRUE;
}

BOOL CTLCDropWnd::Show( CPoint pt )
{
	pt.Offset( -18, -9 );

	SetWindowPos( &wndTop, pt.x, pt.y, 0, 0, SWP_NOOWNERZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOSIZE );

	if( m_bLayeredWindows )
		g_lpfnSetLayeredWindowAttributes( GetSafeHwnd(), 0x000000, 160, ULW_ALPHA );

	return TRUE;
}

BOOL CTLCDropWnd::Hide()
{
	SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE );

	return TRUE;
}

