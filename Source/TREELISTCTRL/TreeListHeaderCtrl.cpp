// TreeListHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TreeListColumnInfo.h"
#include "TreeListDC.h"
#include "TLHDragWnd.h"
#include "TLHDropWnd.h"
#include "TreeListHeaderCtrl.h"
#include "TreeListTipCtrl.h"
#include "TreeListStaticCtrl.h"
#include "TreeListEditCtrl.h"
#include "TreeListComboCtrl.h"
#include "TreeListCtrl.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace {
	int GetMouseBtn(UINT nFlags)
	{
		if (nFlags&MK_LBUTTON)
			return 0;
		if (nFlags&MK_RBUTTON)
			return 1;
		if (nFlags&MK_MBUTTON)
			return 2;
		ASSERT(0);
		return -1;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CTreeListHeaderCtrl

CTreeListHeaderCtrl::CTreeListHeaderCtrl() :
	m_dwStyle( 0 ),
	m_bSubclassFromCreate( FALSE ),
	m_bOffScreenBuffer( TRUE ),
	m_pImageList( NULL ),
	m_pTreeListCtrl( NULL ),
	m_hSize( NULL ),
	m_hSplit( NULL ),
	m_hArrow( NULL ),
	m_bSplit( FALSE ),
	m_bSizing( FALSE ),
	m_bDraging( FALSE ),
	m_nHeaderHeight( 0 ),
	m_pDragWnd( NULL ),
	m_pDropWnd( NULL )
{
	// register window class
	WNDCLASS wndclass;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if( !::GetClassInfo(hInst, TREELISTHEADERCTRL_CLASSNAME, &wndclass) )
	{
        wndclass.style			= CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc	= ::DefWindowProc;
		wndclass.cbClsExtra		= 0;
		wndclass.cbWndExtra		= 0;
		wndclass.hInstance		= hInst;
		wndclass.hIcon			= NULL;
		wndclass.hCursor		=  AfxGetApp()->LoadStandardCursor( IDC_ARROW );
		wndclass.hbrBackground	= (HBRUSH)COLOR_WINDOW;
		wndclass.lpszMenuName	= NULL;
		wndclass.lpszClassName	= TREELISTHEADERCTRL_CLASSNAME;

		if (!AfxRegisterClass(&wndclass))
			AfxThrowResourceException();
	}

	// default color
	m_cr3DBkgnd		= 0x808080;
	m_cr3DFace		= GetSysColor( COLOR_3DFACE );
	m_cr3DLight		= GetSysColor( COLOR_3DHILIGHT );
	m_cr3DShadow	= GetSysColor( COLOR_3DSHADOW );
	m_crText		= GetSysColor( COLOR_BTNTEXT );

	// default font
	m_Font.CreateStockObject( SYSTEM_FIXED_FONT );

	// load cursors
	m_hSize		= AfxGetApp()->LoadCursor( _T("IDC_CURSOR_SIZE") );
	m_hSplit	= AfxGetApp()->LoadCursor( _T("IDC_CURSOR_SPLIT") );
	m_hArrow	= AfxGetApp()->LoadStandardCursor( IDC_ARROW );
}

CTreeListHeaderCtrl::~CTreeListHeaderCtrl()
{
	// free font
	if( m_Font.m_hObject != NULL )
		m_Font.DeleteObject();

	// free cursors
	if( m_hSize != NULL )
		DestroyCursor( m_hSize );

	if( m_hSplit != NULL )
		DestroyCursor( m_hSplit );

	if( m_hArrow != NULL )
		DestroyCursor( m_hArrow );
}

BEGIN_MESSAGE_MAP(CTreeListHeaderCtrl, CWnd)
	//{{AFX_MSG_MAP(CTreeListHeaderCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CANCELMODE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_MESSAGE(HDM_LAYOUT, OnLayout)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListHeaderCtrl message handlers
BOOL CTreeListHeaderCtrl::Create()
{
	// create sort image
	if( !m_imgSort.Create( _T("IDB_TREELIST_SORT"), 8, 4, 0xFF00FF ) )
		return FALSE;

	// create header image
	if( !m_imgHeader.Create( _T("IDB_TREELIST_HEADER"), 16, 4, 0xFF00FF ) )
		return FALSE;

	m_pImageList = &m_imgHeader;

	return TRUE;
}

BOOL CTreeListHeaderCtrl::Create( DWORD dwStyle, const RECT& rect, CTreeListCtrl* pParentWnd, UINT nID )
{
	// create window
	ASSERT_VALID( pParentWnd );

	m_dwStyle = dwStyle;
	m_pTreeListCtrl = pParentWnd;
	m_bSubclassFromCreate = TRUE;

	dwStyle |= WS_CHILD | WS_VISIBLE | WS_EX_TRANSPARENT;

	if( !CWnd::Create( TREELISTHEADERCTRL_CLASSNAME, NULL, dwStyle, rect, pParentWnd, nID ) )
		return FALSE;

	if( !Create() )
		return FALSE;

	return TRUE;
}

void CTreeListHeaderCtrl::SetHeaderFont()
{
	// set font
	CWnd* pWnd = GetParent();
	CTreeListCtrl* pTreeListCtrl = (CTreeListCtrl*)pWnd;

	if( pTreeListCtrl != NULL )
	{
		CFont* pFont = pTreeListCtrl->GetFont();
		if( pFont != NULL )
		{
			LOGFONT lf;
			pFont->GetLogFont( &lf );

			m_Font.DeleteObject();
			m_Font.CreateFontIndirect( &lf );
		}
	}
}

CFont* CTreeListHeaderCtrl::GetHeaderFont()
{
	// get font
	return &m_Font;
}


int CTreeListHeaderCtrl::SetHeaderHeight( int nHeight )
{
	// set header height
	int nOldHeight = m_nHeaderHeight;

	if( nHeight == -1 )
	{
		ASSERT( m_Font.m_hObject != NULL );
		
		LOGFONT lf;
		m_Font.GetLogFont( &lf );

		if( lf.lfHeight > 0 )
		{
			m_nHeaderHeight = lf.lfHeight + 6;
		}
		else if( lf.lfHeight == 0 )
		{
			m_nHeaderHeight = DEFAULT_HEIGHT;
		}
		else
		{
			m_nHeaderHeight = -lf.lfHeight + 6;
		}
	}
	else
	{
		m_nHeaderHeight = nHeight;
	}

	return nOldHeight;
}

int CTreeListHeaderCtrl::GetHeaderHeight()
{
	// get header height
	if( !(GetStyle()&TLHS_SHOWHEAD) )
		return 0;

	return m_nHeaderHeight;
}

int CTreeListHeaderCtrl::GetHeaderWidth()
{
	// get header width
	ASSERT( m_pTreeListCtrl != NULL );
	return m_pTreeListCtrl->GetWidth();
}

BOOL CTreeListHeaderCtrl::GetHeaderRect( CRect& rcHeader, int iCol )
{
	// get header rect
	CPtrArray&			arColumns	= m_pTreeListCtrl->m_arColumns;
	CArray<int,int>&	arShows		= m_pTreeListCtrl->m_arShows;

	BOOL bFound = FALSE;
	int nPerfix = 0;
	for( int iShow=0; iShow<arShows.GetSize(); iShow++ )
	{
		CTreeListColumnInfo* pColumnInfo;

		int iColumn = arShows[iShow];
		pColumnInfo = (CTreeListColumnInfo*)arColumns[iColumn];
		if( iColumn == iCol )
		{
			bFound = TRUE;
			rcHeader.SetRect( 0, 0, pColumnInfo->m_nWidth, GetHeaderHeight() );
			rcHeader.OffsetRect( nPerfix, 0 );
			break;
		}

		nPerfix += pColumnInfo->m_nWidth;
	}

	return bFound;
}

BOOL CTreeListHeaderCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// do not erase bkgnd
	return TRUE;
}

void CTreeListHeaderCtrl::OnPaint() 
{
	// paint header control
	CPaintDC dc(this);

	if( m_bOffScreenBuffer )
	{
		CTreeListDC TreeListDC( &dc );
		DrawCtrl( &TreeListDC );
	}
	else
	{
		DrawCtrl( &dc );
	}
}

BOOL CTreeListHeaderCtrl::DrawCtrl( CDC* pDC )
{
	// draw control
	ASSERT_VALID( pDC );

	CRect rcClip;
	if( pDC->GetClipBox( &rcClip ) == ERROR )
		return FALSE;

	CRect rcClient;
	GetClientRect( &rcClient );

	if( m_pTreeListCtrl->GetStyle()&TLC_BKGNDCOLOR )
	{
		pDC->FillSolidRect( &rcClient, m_cr3DBkgnd );	
	}
	else
	{
		pDC->FillSolidRect( &rcClient, m_cr3DFace );
		pDC->Draw3dRect( &rcClient, m_cr3DLight, m_cr3DShadow );
	}

	DrawHeaders( pDC );

	return TRUE;
}

BOOL CTreeListHeaderCtrl::DrawHeaders( CDC* pDC )
{
	// draw every column headers
	CPtrArray&			Columns = m_pTreeListCtrl->m_arColumns;
	CArray<int, int>&	Shows	= m_pTreeListCtrl->m_arShows;

	// no columns or zero size
	if( m_pTreeListCtrl->GetWidth() == 0 )
		return TRUE;

	int nWinPos = GetParent()->GetScrollPos( SB_HORZ );

	// get client area
	CRect rcClient;
	GetClientRect( &rcClient );
	
	// draw background
	CRect rcFace;
	rcFace.SetRect( 0, rcClient.top, GetHeaderWidth(), rcClient.bottom );
	rcFace.OffsetRect( -nWinPos, 0 );

	pDC->FillSolidRect( &rcFace, m_cr3DFace );
	pDC->Draw3dRect( &rcFace, m_cr3DLight, m_cr3DShadow );
	
	CTreeListColumnInfo* pColumnInfo;
	int nHeaderPos = 0;
	for( int iShow=0; iShow<Shows.GetSize(); iShow++, nHeaderPos += pColumnInfo->m_nWidth )
	{
		int iCol;
		
		// do NOT draw zero column
		iCol = Shows[iShow];  //*m_pShow[iShow];
		pColumnInfo = (CTreeListColumnInfo*)Columns[iCol];
		if( pColumnInfo->m_nWidth == 0 )
			continue;

		CRect rcHeader;
		rcHeader.SetRect( 0, 0, pColumnInfo->m_nWidth, GetHeaderHeight() );
		rcHeader.OffsetRect( -nWinPos, 0 );
		rcHeader.OffsetRect( nHeaderPos, 0 );

		// do NOT draw out of screen column
		if( rcHeader.right < rcClient.left || rcHeader.left > rcClient.right )
			continue;

		DrawHeader( pDC, rcHeader, iCol );
	}

	return TRUE;
}

BOOL CTreeListHeaderCtrl::DrawHeader( CDC* pDC, CRect rcHeader, int iCol )
{
	// draw one column header
	CPtrArray&			Columns = m_pTreeListCtrl->m_arColumns;
	CArray<int, int>&	Shows	= m_pTreeListCtrl->m_arShows;
	CArray<int, int>&	Sorts	= m_pTreeListCtrl->m_arSorts;
	CTreeListColumnInfo* pColumn = (CTreeListColumnInfo*)Columns[iCol];

	ASSERT( Shows.GetSize() > 0 );

	// draw drag of columns
	CRect rcBkgnd;
	rcBkgnd = rcHeader;
	if( rcBkgnd.Height() > 0 )
	{
		if( m_bDraging && iCol == m_iCol )
			pDC->Draw3dRect( rcBkgnd, 0xFF0000, 0xFF0000 );
	}

	// draw gaps between columns
	CRect rcGaps;
	rcGaps = rcHeader;
	rcGaps.DeflateRect( 0, 2, 0, 2 );
	if( rcGaps.Height() > 0 )
	{
		pDC->IntersectClipRect( rcHeader );
		pDC->IntersectClipRect( rcGaps );

		if( !m_bDraging || iCol != m_iCol )
		{
			if( iCol != Shows[0] )
				pDC->DrawEdge( rcGaps, BDR_RAISEDINNER, BF_LEFT );

			if( iCol != Shows[Shows.GetUpperBound()] )
				pDC->DrawEdge( rcGaps, BDR_RAISEDINNER, BF_RIGHT );
		}

		::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
	}

	if( !m_bDraging ||iCol != m_iCol)
		rcHeader.DeflateRect( 0, 0, 1, 1 );
	else
		rcHeader.DeflateRect( 1, 1, 0, 0 );

	rcHeader.DeflateRect( 2, 0, 3, 0 );
	if( rcHeader.Width() <= 0 || rcHeader.Height() <= 0 )
		return TRUE;

	// draw sort arrow
	int iSort = GetSortIndex( iCol );
	if( pColumn->m_dwFormat&TLF_CAPTION_SORT && Sorts.GetSize()>0 )
	{
		CRect rcSort;
		rcSort = rcHeader;
		rcSort.left = rcSort.right - TLL_WIDTH/2;

		POINT pt;
		pt.x = rcSort.left;
		pt.y = rcSort.top + ( rcSort.Height() - TLL_HEIGHT)/2;

		pDC->IntersectClipRect( rcHeader );
		pDC->IntersectClipRect( rcSort );

		if( iSort == 0 )
		{
			// first sort column
			ASSERT( pColumn->m_dwFormat&TLF_SORT_MASK );

			if( pColumn->m_dwFormat&TLF_SORT_ASC )
				m_imgSort.Draw( pDC, TLH_SORT_ASC, pt, ILD_TRANSPARENT );
			else
				m_imgSort.Draw( pDC, TLH_SORT_DESC, pt, ILD_TRANSPARENT );
		}
		else if( iSort > 0 )
		{
			// other sort column
			ASSERT( pColumn->m_dwFormat&TLF_SORT_MASK );

			if( pColumn->m_dwFormat&TLF_SORT_ASC )
				m_imgSort.Draw( pDC, TLH_SORT_ASC2, pt, ILD_TRANSPARENT );
			else
				m_imgSort.Draw( pDC, TLH_SORT_DESC2, pt, ILD_TRANSPARENT );
		}
		else
		{
			// can sort column
			ASSERT( !(pColumn->m_dwFormat&TLF_SORT_MASK) );

			m_imgSort.Draw( pDC, TLH_SORT_NO, pt, ILD_TRANSPARENT );				
		}

		::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );

		rcHeader.DeflateRect( 0, 0, TLL_WIDTH/2, 0 );
	}

	if( rcHeader.Width() <= 0 )
		return TRUE;

	// draw image
	if( pColumn->m_dwFormat&TLF_CAPTION_IMAGE && pColumn->m_iImage>=0 )
	{
		pDC->IntersectClipRect( rcHeader );

		CRect rcImage;
		if( pColumn->m_dwFormat&TLF_IMAGEALIGN_LEFT )
		{
			rcImage			= rcHeader;
			rcImage.right	= rcHeader.left + TLL_WIDTH;
			rcHeader.left	= rcHeader.left + TLL_WIDTH;
		}
		else if( pColumn->m_dwFormat&TLF_IMAGEALIGN_RIGHT )
		{
			rcImage = rcHeader;
			rcImage.left	= rcHeader.right - TLL_WIDTH;
			rcHeader.right	= rcHeader.right - TLL_WIDTH;
		}
		else if( pColumn->m_dwFormat&TLF_IMAGEALIGN_CENTER )
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
		if( !(pColumn->m_dwFormat&TLF_IMAGEALIGN_CENTER) )
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
		m_pImageList->Draw( pDC, pColumn->m_iImage, pt, ILD_TRANSPARENT );
		::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
	}

	rcHeader.DeflateRect( 1, 0, 1, 0 );
	if( rcHeader.Width() <= 0 )
		return TRUE;

	// draw text
	if( pColumn->m_dwFormat&TLF_CAPTION_TEXT )
	{
		pDC->IntersectClipRect( rcHeader );

		int nOldMode = pDC->SetBkMode( TRANSPARENT );
		CFont* pOldFont = pDC->SelectObject( GetHeaderFont() );
		pDC->DrawText( pColumn->m_strCaption, rcHeader, pColumn->GetCaptionFormat() );
		pDC->SelectObject( pOldFont );
		pDC->SetBkMode( nOldMode );

		::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
	}		
	return TRUE;
}

void CTreeListHeaderCtrl::PreSubclassWindow() 
{
	// create from subclass
	CWnd::PreSubclassWindow();

	if( m_bSubclassFromCreate == FALSE )
	{
		if( !Create() )
		{
			AfxThrowMemoryException();
			return;
		}
	}

	return;
}

LRESULT CTreeListHeaderCtrl::OnLayout(WPARAM wParam, LPARAM lParam)
{
	// releayout
	CRect* pRect= (CRect*)lParam;
	
	if( pRect->Height() >= GetHeaderHeight() )
	{
		pRect->bottom = pRect->top + GetHeaderHeight();
	}

	MoveWindow( pRect, TRUE );

	return TRUE;
}

void CTreeListHeaderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// mouse moving
	if( !m_bSizing && !m_bDraging )
	{
		int iFlag;
		int iCol;

		if( HitTestSize( point, &iFlag, &iCol ) )
		{
			if( iFlag & TLHT_ONHEADERSIZE )
			{
				m_bSplit = FALSE;
				SetCursor( m_hSize );
			}
			else
			{
				m_bSplit = TRUE;
				SetCursor( m_hSplit );
			}
		}
	}
	else if( m_bSizing )
	{
		DoSizing( point );
	}
	else if( m_bDraging )
	{
		if (DoDraging( point ))
			DoChecking( point );
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CTreeListHeaderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// left button down
	m_pTreeListCtrl->SetFocus();

	CWnd::OnLButtonDown(nFlags, point);
	
	ASSERT( m_bSizing == FALSE );

	if( !m_bSizing && !m_bDraging )
	{
		int iFlag;
		int iCol;
		
		SetCapture();
		if( HitTestSize( point, &iFlag, &iCol ) )
		{
			m_iCol = iCol;
			if (!BeginSizing( point ))
				Notify(TLHDN_ITEMCLICK, GetMouseBtn(nFlags), iCol, NULL);
		}
		else if( HitTest( point, &iFlag, &iCol ) )
		{
			if( iFlag&TLHT_ONHEADER )
			{
				m_iCol = iCol;
				if (BeginDraging( point ))
				{
					if (DoDraging(point))
						DoChecking(point);
				}
				else
					Notify(TLHDN_ITEMCLICK, GetMouseBtn(nFlags), iCol, NULL);
			}
		}
	}
}

void CTreeListHeaderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// left button up
	if( m_bSizing )
	{
		EndSizing( point );
	}

	if( m_bDraging )
	{
		EndDraging( point );
	}
	Invalidate();
	CWnd::OnLButtonUp(nFlags, point);
}

void CTreeListHeaderCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDblClk(nFlags, point);

	if( !m_bSizing && !m_bDraging )
	{
		int iFlag;
		int iCol;
		
		if( HitTestSize( point, &iFlag, &iCol ) )
		{
			Notify(TLHDN_DIVIDERDBLCLICK, GetMouseBtn(nFlags), iCol, NULL);
			m_pTreeListCtrl->SetColumnWidth( iCol, TLSCW_AUTOSIZE_USEHEADER );
		}
		else if (HitTest(point, &iFlag, &iCol))
		{
			Notify(TLHDN_ITEMDBLCLICK, GetMouseBtn(nFlags), iCol, NULL);
		}
	}
}

void CTreeListHeaderCtrl::OnCancelMode() 
{
	// cancel internal event
	if( m_bSizing )
	{
		EndSizing();
	}

	if( m_bDraging )
	{
		EndDraging();
	}

	CWnd::OnCancelMode();
}

BOOL CTreeListHeaderCtrl::BeginSizing( CPoint point )
{
	// begin a sizing
	CPtrArray&			arColumns	= m_pTreeListCtrl->m_arColumns;
	CArray<int, int>&	arShows		= m_pTreeListCtrl->m_arShows;

	ASSERT( m_bSizing == FALSE );
	ASSERT( m_bDraging == FALSE );

	m_bSizing = TRUE;
	SetCapture();

	if( !m_bSplit )
		SetCursor( m_hSize );
	else
		SetCursor( m_hSplit );

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)arColumns[m_iCol];

	int nTrackPos = GetParent()->GetScrollPos( SB_HORZ ) + point.x;
	pColumnInfo->BeginTrack( nTrackPos );

	return TRUE;
}

BOOL CTreeListHeaderCtrl::EndSizing( CPoint point )
{
	// end a sizing
	CPtrArray&			arColumns	= m_pTreeListCtrl->m_arColumns;
	CArray<int, int>&	arShows		= m_pTreeListCtrl->m_arShows;

	m_bSizing = FALSE;

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)arColumns[m_iCol];
		
	int nTrackPos = GetParent()->GetScrollPos( SB_HORZ ) + point.x;
	pColumnInfo->EndTrack( nTrackPos );

	m_pTreeListCtrl->SetAllScroll();
	m_pTreeListCtrl->RedrawWindow();

	ReleaseCapture();
	return TRUE;
}

BOOL CTreeListHeaderCtrl::DoSizing( CPoint point )
{
	// do a sizing
	CPtrArray&			arColumns	= m_pTreeListCtrl->m_arColumns;
	CArray<int, int>&	arShows		= m_pTreeListCtrl->m_arShows;

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)arColumns[m_iCol];
		
	int nTrackPos = GetParent()->GetScrollPos( SB_HORZ ) + point.x;
	pColumnInfo->DoTrack( nTrackPos );

	m_pTreeListCtrl->SetAllScroll();
	m_pTreeListCtrl->RedrawWindow();

	return TRUE;
}

BOOL CTreeListHeaderCtrl::BeginDraging( CPoint point )
{
	// begin a draging
	CPtrArray&			arColumns	= m_pTreeListCtrl->m_arColumns;
	CArray<int, int>&	arShows		= m_pTreeListCtrl->m_arShows;

	ASSERT( m_bSizing == FALSE );
	ASSERT( m_bDraging == FALSE );


	CTreeListColumnInfo* pColumnInfo;
	int nPerfix = 0;
	for( int iShow=0; iShow<arShows.GetSize(); iShow++ )
	{
		int iCol = arShows[iShow];
		pColumnInfo = (CTreeListColumnInfo*)arColumns[iCol];
		
		if( iCol == m_iCol )
			break;

		nPerfix += pColumnInfo->m_nWidth;
	}

	CRect rcHeader;
	rcHeader.SetRect( 0, 0, pColumnInfo->m_nWidth, GetHeaderHeight() );
	rcHeader.OffsetRect( -m_pTreeListCtrl->GetScrollPos( SB_HORZ ), 0 );

	//m_ptBegin = point;
	m_ptDelta.x = rcHeader.left - point.x ;
	m_ptDelta.y = rcHeader.top - point.y;

	m_bDraging = TRUE;
	Invalidate();
	UpdateWindow();

	if( !(GetStyle()&TLHS_MASKDRAG) )
		return FALSE;

	CRect rcDragTest(point, point);
	rcDragTest.InflateRect(GetSystemMetrics(SM_CXDRAG), GetSystemMetrics(SM_CYDRAG));
	if (!CTreeListCtrl::TestForDrag(this, rcDragTest, CTreeListCtrl::DRAG_DELAY))
	{
		m_bDraging = FALSE;
		return FALSE;
	}
	SetCapture();
	return TRUE;
}

BOOL CTreeListHeaderCtrl::EndDraging( CPoint point )
{
	ASSERT( m_bDraging == TRUE );
	CPtrArray&			 arColumns	= m_pTreeListCtrl->m_arColumns;
	CArray<int, int>&	 arShows	= m_pTreeListCtrl->m_arShows;

	m_bDraging = FALSE;
	ReleaseCapture();

	if( m_pDragWnd != NULL )
	{
		m_pDragWnd->Hide();
		m_pDragWnd->DestroyWindow();
		delete m_pDragWnd;
		m_pDragWnd = NULL;
	}

	if( m_pDropWnd != NULL )
	{
		m_pDropWnd->Hide();
		m_pDropWnd->DestroyWindow();
		delete m_pDropWnd;
		m_pDropWnd = NULL;
	}

	int nFlag;
	int nCol;
	if( HitTestGaps( point, &nFlag, &nCol ) )
	{
		if (Notify(TLHDN_ENDDRAG, 0, m_iCol, NULL) == 0)
		{
			if( nCol != m_iCol )
			{
				int iShow;
				int iDelShow = -1;
				for( iShow = 0; iShow<arShows.GetSize(); iShow++ )
				{
					if( m_iCol == arShows[iShow] )
					{
						iDelShow = iShow;
						break;
					}
				}

				if( iDelShow >= 0 )
				{
					arShows.RemoveAt( iDelShow );
				}

				int iInsShow = -1;
				for( iShow = 0; iShow<arShows.GetSize(); iShow++ )
				{
					if( nCol == arShows[iShow] )
					{
						iInsShow = iShow;
						break;
					}
				}

				if( nFlag&TLHT_ONHEADERRIGHT )
					iInsShow += 1;

				if( iInsShow >= 0 )
				{
					arShows.InsertAt( iInsShow, m_iCol );
				}
			}
		}
	}

	Invalidate();

	m_pTreeListCtrl->Invalidate();

	return TRUE;
}

BOOL CTreeListHeaderCtrl::DoDraging( CPoint point )
{
	CPtrArray&			arColumns	= m_pTreeListCtrl->m_arColumns;
	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)arColumns[m_iCol];

	if( m_pDragWnd == NULL )
	{
		if (Notify(TLHDN_BEGINDRAG, 0, m_iCol, NULL) != 0)
			return FALSE;

		CRect rcDragWnd;
		GetHeaderRect( rcDragWnd, m_iCol );
		if( GetStyle()&TLHS_FULLDRAG )
		{
			rcDragWnd.OffsetRect( point );
			rcDragWnd.OffsetRect( m_ptDelta );
		}
		else
		{
			rcDragWnd.OffsetRect( point.x, 0 );
			rcDragWnd.OffsetRect( m_ptDelta.x, 0 );
		}
		ClientToScreen( rcDragWnd );
		m_pDragWnd = new CTLHDragWnd;
		m_pDragWnd->Create( rcDragWnd, this, m_iCol );
		m_pDragWnd->Show();
	}
	else
	{
		CRect rcDragWnd;
		GetHeaderRect( rcDragWnd, m_iCol );
		if( GetStyle()&TLHS_FULLDRAG )
		{
			rcDragWnd.OffsetRect( point );
			rcDragWnd.OffsetRect( m_ptDelta );
		}
		else
		{
			rcDragWnd.OffsetRect( point.x, 0 );
			rcDragWnd.OffsetRect( m_ptDelta.x, 0 );
		}
		ClientToScreen( rcDragWnd );

		m_pDragWnd->MoveWindow( rcDragWnd );
	}

	return TRUE;
}

BOOL CTreeListHeaderCtrl::DoChecking( CPoint point )
{
	CPtrArray&			arColumns	= m_pTreeListCtrl->m_arColumns;
	CArray<int, int>&	arShows		= m_pTreeListCtrl->m_arShows;
	int nWinPos	= m_pTreeListCtrl->GetScrollPos( SB_HORZ );

	int nFlag;
	int nCol;
	int nPos;

	BOOL bShow;

	bShow = HitTestGaps( point, &nFlag, &nCol );

	if( bShow )
	{
		if( m_pDropWnd == NULL )
		{
			m_pDropWnd = new CTLHDropWnd;
			m_pDropWnd->Create( this );
		}

		nPos = 0;
		if( nFlag&TLHT_ONHEADERRIGHT )
		{
			for( int iShow = 0; iShow < arShows.GetSize(); iShow++ )
			{
				int iCol = arShows[iShow];

				CTreeListColumnInfo* pColumnInfo;
				pColumnInfo = (CTreeListColumnInfo*)arColumns[iCol];

				nPos = nPos + pColumnInfo->m_nWidth;

				if( iCol == nCol )
					break;
			}
		}

		nPos = nPos - nWinPos;

		CPoint pt;
		pt.x = nPos;
		pt.y = 0;

		ClientToScreen( &pt );
		m_pDropWnd->Show( pt );
	}
	else
	{
		if( m_pDropWnd != NULL )
		{
			m_pDropWnd->Hide();
		}
	}

	return TRUE;
	
	/*
		m_pDropWnd = new CTLHDropWnd;
		m_pDropWnd->Create( this );
		m_pDropWnd->Show();
	*/
}

BOOL CTreeListHeaderCtrl::EndSizing()
{
	// end sizing
	CPoint	point;

	GetCursorPos( &point );
	ScreenToClient( &point );

	return EndSizing( point );
}

BOOL CTreeListHeaderCtrl::EndDraging()
{
	// end draging
	CPoint point;

	GetCursorPos( &point );
	ScreenToClient( &point );

	return EndDraging( point );
}


BOOL CTreeListHeaderCtrl::HitTestSize( CPoint pt, int* pFlag, int* pCol )
{
	// hit test for sizing
	ASSERT( pFlag != NULL );
	ASSERT( pCol != NULL );

	CPtrArray&			 arColumns	= m_pTreeListCtrl->m_arColumns;
	CArray<int, int>&	 arShows	= m_pTreeListCtrl->m_arShows;
	int nWinPos = m_pTreeListCtrl->GetScrollPos( SB_HORZ );

	*pFlag	= 0;
	*pCol	= 0;

	CRect rcClient;
	rcClient.SetRect( 0, 0, GetHeaderWidth(), GetHeaderHeight() );
	rcClient.right += 2;	// fix right border for resize
	rcClient.OffsetRect( -nWinPos, 0 );

	// check if in client rect
	if( !rcClient.PtInRect(pt) )
		return FALSE;

	// check if on gaps
	int nPos = pt.x + nWinPos;
	int nHeaderPos = 0;
	int nSpaceMin  = 0;
	BOOL bOnSize   = FALSE;
	for( int iShow = 0; iShow<arShows.GetSize(); iShow++ )
	{
		CTreeListColumnInfo* pColumnInfo;
		int iCol;

		iCol = arShows[iShow];
		pColumnInfo = (CTreeListColumnInfo*)arColumns[iCol];

		// calculate header right border position;
		nHeaderPos = nHeaderPos + pColumnInfo->m_nWidth;

		// filter nosize column
		if( pColumnInfo->m_dwFormat&TLF_NOSIZE )
			continue;

		// is close right border enough from left
		if( nHeaderPos-3 <= nPos && nPos < nHeaderPos )
		{
			int nGap = nHeaderPos - nPos;
			if( !bOnSize )
			{
				// find first closed right border from left
				*pCol		= iCol;
				nSpaceMin	= nGap;
				bOnSize		= TRUE;
			}
			else
			{
				// find more closed right border from left
				if( nGap < nSpaceMin )
				{
					*pCol	  = iCol;
					nSpaceMin = nGap;
				}
			}
		}
		
		// is close right border enough from right
		if( nHeaderPos <= nPos && nPos <= nHeaderPos+2 )
		{
			int nGap = nPos - nHeaderPos;
			if( !bOnSize )
			{
				// find first closed right border form right
				*pCol		= iCol;
				nSpaceMin	= nGap;
				bOnSize		= TRUE;
			}
			else
			{
				// find more closed right broder from right
				if( nGap <= nSpaceMin )
				{
					*pCol	  = iCol;
					nSpaceMin = nGap;
				}
			}
		}
	} // end for

	if( bOnSize )
	{
		CTreeListColumnInfo* pSizeColumn = ( CTreeListColumnInfo* )arColumns[*pCol];
		if( pSizeColumn->m_nWidth != 0 )
			*pFlag = TLHT_ONHEADERSIZE;
		else
			*pFlag = TLHT_ONHEADERSPLIT;
	}

	return bOnSize;
}

BOOL CTreeListHeaderCtrl::HitTestGaps( CPoint pt, int* pFlag, int* pCol )
{
	// hit test for show
	ASSERT( pFlag != NULL );
	ASSERT( pCol != NULL );

	CPtrArray&			arColumns = m_pTreeListCtrl->m_arColumns;
	CArray<int, int>&	arShows	  = m_pTreeListCtrl->m_arShows;
	int nWinPos = m_pTreeListCtrl->GetScrollPos( SB_HORZ );

	*pFlag	= 0;
	*pCol	= 0;

	CRect rcClient;
	rcClient.SetRect( 0, 0, GetHeaderWidth(), GetHeaderHeight() );
	rcClient.left -= 16;		// fix left border for reorder
	rcClient.right += 16;	// fix right border for reorder
	rcClient.OffsetRect( -nWinPos, 0 );

	// check if in client rect
	if( !rcClient.PtInRect( pt ) )
		return FALSE;

	// check on which column
	int nPos = pt.x + nWinPos;
	int nHeaderPos	= 0;
	int nSpaceMin	= 0;
	BOOL bOnGaps	= FALSE;
	CTreeListColumnInfo* pColumnInfo;
	int iCol;
	int nDelta;

	// check first left border;
	if( arShows.GetSize() > 0 )
	{
		iCol = arShows[0];
		nDelta = 16;
		if( nHeaderPos-nDelta <= nPos && nPos< nHeaderPos )
		{
			// find first closed first left border from left
			ASSERT( !bOnGaps );
			int nGap	= nHeaderPos - nPos;
			*pCol		= iCol;
			*pFlag		= TLHT_ONHEADERLEFT;
			nSpaceMin	= nGap;
			bOnGaps		= TRUE;
		}

		// is close right border enough from right
		CTreeListColumnInfo* pFirstColumnInfo;
		pFirstColumnInfo = (CTreeListColumnInfo*)arColumns[arShows[0]];
		nDelta = max( pFirstColumnInfo->m_nWidth/2, 16 );

		if( nHeaderPos <= nPos && nPos <= nHeaderPos+nDelta )
		{
			// find first closed first left border from right
			ASSERT( !bOnGaps );
			int nGap	= nPos - nHeaderPos;
			*pCol		= iCol;
			*pFlag		= TLHT_ONHEADERLEFT;
			nSpaceMin	= nGap;
			bOnGaps		= TRUE;
		}
	}

	// check all right border;
	for( int iShow = 0; iShow<arShows.GetSize(); iShow++ )
	{
		iCol = arShows[iShow];
		pColumnInfo = (CTreeListColumnInfo*)arColumns[iCol];

		// calculate header right border position;
		nHeaderPos = nHeaderPos + pColumnInfo->m_nWidth;

		// is close right border enough from left
		nDelta = max(pColumnInfo->m_nWidth / 2, 16 );
		if( nHeaderPos-nDelta <= nPos && nPos < nHeaderPos )
		{
			int nGap = nHeaderPos - nPos;
			if( !bOnGaps )
			{
				// find first closed right border from left
				*pCol		= iCol;
				*pFlag		= TLHT_ONHEADERRIGHT;
				nSpaceMin	= nGap;
				bOnGaps		= TRUE;
			}
			else
			{
				// find more closed right border from left
				if( nGap < nSpaceMin )
				{
					*pCol		= iCol;
					*pFlag		= TLHT_ONHEADERRIGHT;
					nSpaceMin	= nGap;
				}
			}
		}

		// is close right border enough from right
		if( iShow < arShows.GetUpperBound() )
		{
			CTreeListColumnInfo* pNextColumnInfo;
			pNextColumnInfo = (CTreeListColumnInfo*)arColumns[arShows[iShow+1]];
			nDelta = max( pNextColumnInfo->m_nWidth/2, 16 );
		}
		else
		{
			nDelta = 16;
		}

		if( nHeaderPos <= nPos && nPos <= nHeaderPos+nDelta )
		{
			int nGap = nPos - nHeaderPos;
			if( !bOnGaps )
			{
				// find first closed right border from right
				*pCol		= iCol;
				*pFlag		= TLHT_ONHEADERRIGHT;
				nSpaceMin	= nGap;
				bOnGaps		= TRUE;
			}
			else
			{
				// fine more closed right border from right
				if( nGap <= nSpaceMin )
				{
					*pCol		= iCol;
					*pFlag		= TLHT_ONHEADERRIGHT;
					nSpaceMin	= nGap;
				}
			}
		}
	} // end for
				
	return bOnGaps;
}

BOOL CTreeListHeaderCtrl::HitTest( CPoint pt, int* pFlag, int* pCol )
{
	// hit test for header
	ASSERT( pFlag != NULL );
	ASSERT( pCol != NULL );

	CPtrArray&			 arColumns	= m_pTreeListCtrl->m_arColumns;
	CArray<int, int>&	 arShows	= m_pTreeListCtrl->m_arShows;
	CTreeListColumnInfo* pColumnInfo= NULL;
	int nWinPos = m_pTreeListCtrl->GetScrollPos( SB_HORZ );

	*pFlag = 0;
	*pCol = 0;

	CRect rcClient;
	rcClient.SetRect( 0, 0, GetHeaderWidth(), GetHeaderHeight() );
	rcClient.OffsetRect( -nWinPos, 0 );

	// check if in client rect
	if( !rcClient.PtInRect(pt) )
		return NULL;

	// check if on header
	int nPerfix = 0;
	for( int iShow = 0; iShow<arShows.GetSize(); iShow++ )
	{
		int iCol = arShows[iShow];
		pColumnInfo	 = (CTreeListColumnInfo*)arColumns[iCol];

		CRect rcColumn;
		rcColumn.SetRect( 0, 0, pColumnInfo->m_nWidth, GetHeaderHeight() );
		rcColumn.OffsetRect( nPerfix, 0 );
		rcColumn.OffsetRect( -nWinPos, 0 );

		if( rcColumn.PtInRect( pt ) )
		{
			CRect rcHeader;
			
			// if on border
			rcHeader = rcColumn;
			rcHeader.DeflateRect( 2, 0, 3, 0 );
			if( rcHeader.Width()>0 )
			{
				if( !rcHeader.PtInRect( pt ) )
					*pFlag |= TLHT_ONHEADERGAPS;
			}

			// if on sort arrow
			if( pColumnInfo->m_dwFormat&TLF_CAPTION_SORT )
			{
				CRect rcSort;
				rcSort = rcHeader;
				rcSort.left = rcSort.right - TLL_WIDTH/2;
				
				if( rcSort.PtInRect( pt ) )
					*pFlag |= TLHT_ONHEADERSORT;

				rcHeader.DeflateRect( 0, 0, TLL_WIDTH/2, 0 );
			}

			// if on image
			if( rcHeader.Width()>0 && pColumnInfo->m_dwFormat&TLF_CAPTION_IMAGE )
			{
				CRect rcImage;
				if( pColumnInfo->m_dwFormat&TLF_IMAGEALIGN_LEFT )
				{
					rcImage			= rcHeader;
					rcImage.right	= rcHeader.left + TLL_WIDTH;
					rcHeader.left	= rcHeader.left + TLL_WIDTH;
				}
				else if( pColumnInfo->m_dwFormat&TLF_IMAGEALIGN_RIGHT )
				{
					rcImage			= rcHeader;
					rcImage.left	= rcHeader.right - TLL_WIDTH;
					rcImage.right	= rcHeader.right - TLL_WIDTH;
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

				if( rcImage.PtInRect( pt ) )
					*pFlag |= TLHT_ONHEADERIMAGE;
			}
			
			// if on text
			if( rcHeader.Width()>0 && pColumnInfo->m_dwFormat&TLF_CAPTION_TEXT )
			{
				if( rcHeader.PtInRect( pt ) )
					*pFlag |= TLHT_ONHEADERTEXT;
			}

			*pCol = iCol;

			return TRUE;
		}

		nPerfix += rcColumn.Width();
	}

	return FALSE;
}

int CTreeListHeaderCtrl::GetSortIndex( int iCol )
{
	CArray<int, int>&	Sorts	= m_pTreeListCtrl->m_arSorts;

	int iRet = -1;

	for( int iSort = 0; iSort<Sorts.GetSize(); iSort++ )
	{
		if( iCol == Sorts[iSort] )
		{
			iRet = iSort;
			break;
		}
	}

	return iRet;
}

DWORD CTreeListHeaderCtrl::GetStyle()
{
	return m_dwStyle;
}

DWORD CTreeListHeaderCtrl::SetStyle( DWORD dwStyle )
{
	DWORD dwoldStyle = m_dwStyle;
	m_dwStyle = dwStyle;
	return dwoldStyle;
}

void CTreeListHeaderCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_pTreeListCtrl->SetFocus();
	CWnd::OnRButtonDown(nFlags, point);
	Notify(TLHDN_RCLICK, GetMouseBtn(nFlags), -1, NULL);	
}

void CTreeListHeaderCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnRButtonUp(nFlags, point);
}

void CTreeListHeaderCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	CWnd::OnRButtonDblClk(nFlags, point);

	if( !m_bSizing && !m_bDraging )
	{
		int iFlag;
		int iCol;
		
		if( HitTestSize( point, &iFlag, &iCol ) )
		{
			m_pTreeListCtrl->SetColumnWidth( iCol, TLSCW_AUTOSIZE );
		}
	}
}

void	CTreeListHeaderCtrl::FillNMHDR(NMHDR& hdr, UINT uiMsg)
{
	hdr.hwndFrom = m_hWnd;
	hdr.code = uiMsg;
	hdr.idFrom = 0;
}

LRESULT CTreeListHeaderCtrl::Notify(UINT uiMsg, int iMouseButton, 
									int iItem, TLHDITEM* pitem)
{
	NMTLHEADER hdr;
	FillNMHDR(hdr.hdr, uiMsg);
	hdr.iButton = iMouseButton;
	hdr.iItem = iItem;
	hdr.pitem = pitem;
	CWnd* p = GetParent();
	if (p)
		p = p->GetParent();
	if (p)
		return p->SendMessage(WM_NOTIFY, hdr.hdr.idFrom, reinterpret_cast<LPARAM>(&hdr));
	else
		return 0l;
}
