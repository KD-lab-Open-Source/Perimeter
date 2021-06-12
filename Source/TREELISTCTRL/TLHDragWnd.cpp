// TLHDragWnd.cpp : implementation file
//

#include "stdafx.h"
#include "TreeListHeaderCtrl.h"
#include "TreeListTipCtrl.h"
#include "TreeListStaticCtrl.h"
#include "TreeListEditCtrl.h"
#include "TreeListComboCtrl.h"
#include "TreeListCtrl.h"
#include "TreeListDC.h"
#include "TLHDragWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTLHDragWnd
static lpfnUpdateLayeredWindow g_lpfnUpdateLayeredWindow = NULL;
static lpfnSetLayeredWindowAttributes g_lpfnSetLayeredWindowAttributes = NULL;

CTLHDragWnd::CTLHDragWnd() :
	m_pTreeListHeaderCtrl( NULL )
{
	// register the window class
	WNDCLASS wndclass;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if(!(::GetClassInfo(hInst, TLHDRAGWND_CLASSNAME, &wndclass)))
	{
		wndclass.style = CS_HREDRAW | CS_VREDRAW ; //CS_SAVEBITS ;
		wndclass.lpfnWndProc = ::DefWindowProc;
		wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInst;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor( hInst, _T("IDC_ARROW"));
		wndclass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1); 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = TLHDRAGWND_CLASSNAME;
		if (!AfxRegisterClass(&wndclass))
			AfxThrowResourceException();
	}

	// default color
	m_cr3DFace		= GetSysColor( COLOR_3DFACE );
	m_cr3DLight		= GetSysColor( COLOR_3DHILIGHT );
	m_cr3DShadow	= GetSysColor( COLOR_3DSHADOW );
	m_crText		= GetSysColor( COLOR_BTNTEXT );

	// get layer window
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

CTLHDragWnd::~CTLHDragWnd()
{
}


BEGIN_MESSAGE_MAP(CTLHDragWnd, CWnd)
	//{{AFX_MSG_MAP(CTLHDragWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTLHDragWnd message handlers
BOOL CTLHDragWnd::Create(CRect rcHeader, CTreeListHeaderCtrl* pTreeListHeaderCtrl, int iCol )
{
	ASSERT_VALID( pTreeListHeaderCtrl );
	
	m_pTreeListHeaderCtrl = pTreeListHeaderCtrl;
	m_iCol = iCol;

	DWORD dwStyle	= WS_POPUP | WS_DISABLED;
	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;

	if( m_bLayeredWindows )
		dwExStyle |= WS_EX_LAYERED;

	if( !CreateEx( dwExStyle, TLHDRAGWND_CLASSNAME, NULL, dwStyle, 
		rcHeader.left, rcHeader.top, rcHeader.Width(), rcHeader.Height(), NULL, NULL, NULL ) )
		return FALSE;

	return TRUE;
}

BOOL CTLHDragWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CTLHDragWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

    if( m_pTreeListHeaderCtrl->m_bOffScreenBuffer )
    {
        CTreeListDC MemDC(&dc);
        DrawWnd(&MemDC);
    }
    else
	{
        DrawWnd(&dc);
	}
}

BOOL CTLHDragWnd::DrawWnd( CDC* pDC )
{
	CPtrArray&			Columns = m_pTreeListHeaderCtrl->m_pTreeListCtrl->m_arColumns;
	CArray<int, int>&	Shows	= m_pTreeListHeaderCtrl->m_pTreeListCtrl->m_arShows;
	CArray<int, int>&	Sorts	= m_pTreeListHeaderCtrl->m_pTreeListCtrl->m_arSorts;
	CTreeListColumnInfo* pColumnInfo = (CTreeListColumnInfo*)Columns[m_iCol];

	CImageList&	imgSort		= m_pTreeListHeaderCtrl->m_imgSort;
	CImageList* pImageList	= m_pTreeListHeaderCtrl->m_pImageList ;

	CRect rcClient;
	GetClientRect( rcClient );
	CRect rcHeader;
	rcHeader = rcClient;

	// draw background
	if( !m_bLayeredWindows )
		pDC->FillSolidRect( rcHeader, m_cr3DFace );
	else
		pDC->FillSolidRect( rcHeader, m_cr3DShadow );

	// draw 3d rect;
	if( !m_bLayeredWindows )
		pDC->Draw3dRect( rcHeader, m_cr3DLight, m_cr3DShadow );

	rcHeader.DeflateRect( 2, 0, 3, 0 );
	if( rcHeader.Width() <= 0 )
		return TRUE;

	// draw sort arrow;
	int iSort = m_pTreeListHeaderCtrl->GetSortIndex( m_iCol );
	if( pColumnInfo->m_dwFormat&TLF_CAPTION_SORT && Sorts.GetSize()>0 )
	{
		CRect rcSort;
		rcSort = rcHeader;
		rcSort.left = rcSort.right - TLL_WIDTH/2;

		POINT pt;
		pt.x = rcSort.left;
		pt.y = rcSort.top + ( rcSort.Height() - TLL_HEIGHT )/2;

		pDC->IntersectClipRect( rcHeader );
		pDC->IntersectClipRect( rcSort );

		if( iSort == 0 )
		{
			// first sort column
			ASSERT( pColumnInfo->m_dwFormat&TLF_SORT_MASK );

			if( pColumnInfo->m_dwFormat&TLF_SORT_ASC )
				imgSort.Draw( pDC, TLH_SORT_ASC, pt, ILD_TRANSPARENT );
			else
				imgSort.Draw( pDC, TLH_SORT_DESC, pt, ILD_TRANSPARENT );
		}
		else if( iSort > 0 )
		{
			// other sort column
			ASSERT( pColumnInfo->m_dwFormat&TLF_SORT_MASK );

			if( pColumnInfo->m_dwFormat&TLF_SORT_ASC )
				imgSort.Draw( pDC, TLH_SORT_ASC2, pt, ILD_TRANSPARENT );
			else
				imgSort.Draw( pDC, TLH_SORT_DESC2, pt, ILD_TRANSPARENT );
		}
		else
		{
			// can sort column
			ASSERT( !(pColumnInfo->m_dwFormat&TLF_SORT_MASK) );

			imgSort.Draw( pDC, TLH_SORT_NO, pt, ILD_TRANSPARENT );				
		}

		::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );

		rcHeader.DeflateRect( 0, 0, TLL_WIDTH/2, 0 );
	}

	if( rcHeader.Width() <= 0 )
		return TRUE;

	// draw image
	if( pColumnInfo->m_dwFormat&TLF_CAPTION_IMAGE && pColumnInfo->m_iImage>=0 )
	{
		pDC->IntersectClipRect( rcHeader );

		CRect rcImage;
		if( pColumnInfo->m_dwFormat&TLF_IMAGEALIGN_LEFT )
		{
			rcImage			= rcHeader;
			rcImage.right	= rcHeader.left + TLL_WIDTH;
			rcHeader.left	= rcHeader.left + TLL_WIDTH;
		}
		else if( pColumnInfo->m_dwFormat&TLF_IMAGEALIGN_RIGHT )
		{
			rcImage = rcHeader;
			rcImage.left	= rcHeader.right - TLL_WIDTH;
			rcHeader.right	= rcHeader.right - TLL_WIDTH;
		}
		else if( pColumnInfo->m_dwFormat&TLF_IMAGEALIGN_CENTER )
		{
			rcImage			= rcHeader;
			rcHeader.left	= rcHeader.right;
		}
		else
		{
			rcImage			= rcHeader;
			rcImage.right	= rcHeader.left + TLL_WIDTH;
			rcHeader.left	= rcHeader.left + TLL_WIDTH;
		}

		POINT pt;
		if( !(pColumnInfo->m_dwFormat&TLF_IMAGEALIGN_CENTER) )
		{
			pt.x = rcImage.left;
			pt.y = rcImage.top + ( rcImage.Height() - TLL_HEIGHT )/2;
		}
		else
		{
			pt.x = rcImage.left + ( rcImage.Width() - TLL_WIDTH )/2;
			pt.y = rcImage.top + ( rcImage.Height() - TLL_HEIGHT )/2;
		}

		pDC->IntersectClipRect( rcImage );
		pImageList->Draw( pDC, pColumnInfo->m_iImage, pt, ILD_TRANSPARENT );
		::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
	}

	rcHeader.DeflateRect( 1, 0, 1, 0 );
	if( rcHeader.Width() <= 0 )
		return TRUE;

	// draw text
	if( pColumnInfo->m_dwFormat&TLF_CAPTION_TEXT )
	{
		pDC->IntersectClipRect( rcHeader );

		int nOldMode = pDC->SetBkMode( TRANSPARENT );
		CFont* pOldFont = pDC->SelectObject( m_pTreeListHeaderCtrl->GetHeaderFont() );
		pDC->DrawText( pColumnInfo->m_strCaption, rcHeader, pColumnInfo->GetCaptionFormat() );
		pDC->SelectObject( pOldFont );
		pDC->SetBkMode( nOldMode );

		::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
	}		

	return TRUE;
}

BOOL CTLHDragWnd::Show()
{
	SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE );

	if( m_bLayeredWindows )
		g_lpfnSetLayeredWindowAttributes( GetSafeHwnd(), 0x000000, 160, ULW_ALPHA );

	return TRUE;
}

BOOL CTLHDragWnd::Hide()
{
	SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE );

	return TRUE;
}

