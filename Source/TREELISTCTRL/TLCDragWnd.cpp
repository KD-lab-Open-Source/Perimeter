// TLCDragWnd.cpp : implementation file
//

#include "stdafx.h"
#include "TreeListHeaderCtrl.h"
#include "TreeListTipCtrl.h"
#include "TreeListStaticCtrl.h"
#include "TreeListEditCtrl.h"
#include "TreeListComboCtrl.h"
#include "TreeListCtrl.h"
#include "TreeListDC.h"
#include "TreeListItem.h"
#include "TLCDragWnd.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DRAGWND_SIZE	320
#define MAX_SIZE 320

static lpfnUpdateLayeredWindow g_lpfnUpdateLayeredWindow = NULL;
static lpfnSetLayeredWindowAttributes g_lpfnSetLayeredWindowAttributes = NULL;
/////////////////////////////////////////////////////////////////////////////
// CTLCDragWnd

CTLCDragWnd::CTLCDragWnd() :
	m_pTreeListCtrl( NULL )
{
	// register the window class
	WNDCLASS wndclass;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if(!(::GetClassInfo(hInst, TLCDRAGWND_CLASSNAME, &wndclass)))
	{
		wndclass.style = CS_HREDRAW | CS_VREDRAW;// | CS_SAVEBITS ;
		wndclass.lpfnWndProc = ::DefWindowProc;
		wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInst;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor( hInst, _T("IDC_ARROW"));
		wndclass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1); 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = TLCDRAGWND_CLASSNAME;
		if (!AfxRegisterClass(&wndclass))
			AfxThrowResourceException();
	}

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

	m_pDIB		= NULL;
	m_pBitmap	= NULL;
	m_pBitmap2	= NULL;

	BITMAPINFO bmi;
	HBITMAP hbitmap;

	bmi.bmiHeader.biSize		= sizeof (BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth		= DRAGWND_SIZE;
	bmi.bmiHeader.biHeight		= DRAGWND_SIZE;
	bmi.bmiHeader.biPlanes		= 1;
	bmi.bmiHeader.biBitCount	= 32; 
	bmi.bmiHeader.biCompression	= BI_RGB;
	bmi.bmiHeader.biSizeImage	= DRAGWND_SIZE * DRAGWND_SIZE * 4;

	m_dcMem.CreateCompatibleDC( NULL );
	hbitmap = CreateDIBSection( NULL, &bmi, DIB_RGB_COLORS, (void **)&m_pDIB, NULL, NULL ); 
	m_pBitmap = CBitmap::FromHandle( hbitmap );
	m_pBitmap2 = m_dcMem.SelectObject( m_pBitmap );
}

CTLCDragWnd::~CTLCDragWnd()
{
	m_dcMem.SelectObject( m_pBitmap2 );
	m_dcMem.DeleteDC();
	::DeleteObject( *m_pBitmap );
}


BEGIN_MESSAGE_MAP(CTLCDragWnd, CWnd)
	//{{AFX_MSG_MAP(CTLCDragWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTLCDragWnd message handlers
BOOL CTLCDragWnd::Create( CRect rcWindow, CTreeListCtrl* pTreeListCtrl )
{
	// create window
	ASSERT_VALID( pTreeListCtrl );
	
	m_pTreeListCtrl = pTreeListCtrl;

	DWORD dwStyle	= WS_POPUP | WS_DISABLED;
	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;

	if( m_bLayeredWindows )
	{
		dwExStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT;

		if( !CreateEx( dwExStyle, TLCDRAGWND_CLASSNAME, NULL, dwStyle, 
			rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(), NULL, NULL, NULL ) )
			return FALSE;
	}
	else
	{
		CRect rcClient;
		m_pTreeListCtrl->GetClientRect( &rcClient );

		if( !CreateEx( dwExStyle, TLCDRAGWND_CLASSNAME, NULL, dwStyle,
			rcWindow.left, rcWindow.top, rcClient.Width(), pTreeListCtrl->GetItemHeight(), NULL, NULL, NULL ) )
			return FALSE;
	}

	return TRUE;
}

BOOL CTLCDragWnd::OnEraseBkgnd(CDC* pDC) 
{
	// erase background
	return TRUE;
}

void CTLCDragWnd::OnPaintBMP()
{
	// render off screen bitmap
	ASSERT( m_pTreeListCtrl != NULL );
	ASSERT( m_dcMem.m_hDC != NULL );

	CPaintDC dc(this);

	CRect rcWindow;
	CRect rcClient;
	GetWindowRect( rcWindow );
	GetClientRect( rcClient );

	int i, x, y, z;

	for( i = 0; i < 102400; i++ )
		((UINT32*)m_pDIB)[i] = 0xFFFFFFFF;

	// render control on bitmap
	DrawWnd( &m_dcMem );

	// calculate alpha channel
	int xx, yy, zz;
	for( y = 0; y < 320; y++ )
	{
		yy = 160 * 160 - 2 * 160 * y + y * y;
		for( x = 0; x < 320; x++ )
		{
			xx = 160 * 160 - 2 * 160 * x + x * x;
			zz = xx + yy;
			z = (int)sqrt( (float)zz );
			z = 160 - min( 160, z );

			UINT32 cc, aa, bb, gg, rr;

			cc = ((UINT32 *)m_pDIB)[x + y * DRAGWND_SIZE];
			
			if( cc == 0xFFFFFFFF )
			{
				cc = 0x00000000;
			}
			else
			{
				aa = z;
				bb = ( cc & 0x00FF0000 ) >> 16;
				gg = ( cc & 0x0000FF00 ) >> 8;
				rr = ( cc & 0x000000FF );
				
				bb = bb * aa / 0xFF;
				gg = gg * aa / 0xFF;
				rr = rr * aa / 0xFF;

				cc = ( aa << 24 ) | ( bb << 16 ) | ( gg << 8 ) | rr;
			}

			((UINT32 *)m_pDIB)[x + y * DRAGWND_SIZE] = cc;
		}
	}

	// update window with alpha channel
	BLENDFUNCTION bf;
	bf.BlendOp				= AC_SRC_OVER;
	bf.BlendFlags			= 0;
	bf.SourceConstantAlpha	= 0xFF;
	bf.AlphaFormat			= 0x1;

	g_lpfnUpdateLayeredWindow( 
		GetSafeHwnd(), dc.GetSafeHdc(), &rcWindow.TopLeft(), &CSize( rcWindow.Width(), rcWindow.Height() ),
		m_dcMem.GetSafeHdc(), &CPoint( 0, 0 ), 
		0x00, &bf, 
		ULW_ALPHA );

	g_lpfnSetLayeredWindowAttributes( GetSafeHwnd(), 0x000000, 160, ULW_ALPHA );

	return;
}

void CTLCDragWnd::OnPaint() 
{
	CPaintDC dc(this);

	if( !m_bLayeredWindows )
		DrawDragWnd( &dc );

	return;
}

BOOL CTLCDragWnd::DrawWnd( CDC* pDC )
{
	// draw window
	CRect rcItems;
	CRect rcClient;

	// calculate draw position
	rcClient.SetRect( 0, 0, 320, 320 );
	rcItems.left = m_pTreeListCtrl->m_ptBegin.x + m_pTreeListCtrl->GetScrollPos( SB_HORZ );
	rcItems.top = m_pTreeListCtrl->m_ptBegin.y + m_pTreeListCtrl->GetScrollPos( SB_VERT ) * m_pTreeListCtrl->GetItemHeight();

	CPoint pt;
	pt.x = m_pTreeListCtrl->GetScrollPos( SB_HORZ );
	pt.y = m_pTreeListCtrl->GetScrollPos( SB_VERT ) * m_pTreeListCtrl->GetItemHeight();

	pt.x = m_pTreeListCtrl->m_ptBegin.x + pt.x;
	pt.y = m_pTreeListCtrl->m_ptBegin.y + pt.y;

	pt.y = -m_pTreeListCtrl->GetHeaderCtrl()->GetHeaderHeight() + pt.y;

	pt.x = DRAGWND_SIZE / 2 - pt.x;
	pt.y = DRAGWND_SIZE / 2 - pt.y;

	int nFirstRow = - pt.y / m_pTreeListCtrl->GetItemHeight();
	int nShowRows = DRAGWND_SIZE / m_pTreeListCtrl->GetItemHeight() + 2;

	if( nFirstRow < 0 )
		nFirstRow = 0;

	CRect rcItem;
	rcItem.SetRect( 0, 0, m_pTreeListCtrl->GetWidth(), m_pTreeListCtrl->GetItemHeight() );
	rcItem.OffsetRect( pt.x, pt.y );
	rcItem.OffsetRect( 0, nFirstRow * m_pTreeListCtrl->GetItemHeight() );

	// draw all items
	CPen Pen( PS_SOLID, 1, m_pTreeListCtrl->getGridColor() );
	CPen* pOldPen		= pDC->SelectObject( &Pen );
	CFont* pOldFont		= pDC->SelectObject( &m_pTreeListCtrl->m_Font );
	int nOldMode = pDC->SetBkMode( TRANSPARENT );

	CTreeListItem* pShowItem = m_pTreeListCtrl->GetFirstShowItem( nFirstRow );
	for( int iRow = nFirstRow; iRow < nFirstRow + nShowRows && iRow < m_pTreeListCtrl->GetVisibleCount(); iRow++, pShowItem = m_pTreeListCtrl->GetNextShowItem( pShowItem ) )
	{
		ASSERT( pShowItem != NULL );

		DrawItem( pDC, rcItem, pShowItem );

		if( m_pTreeListCtrl->m_dwStyle&TLC_HGRID_FULL && rcItem.right<rcClient.right )
		{
			pDC->MoveTo( rcItem.right, rcItem.bottom - 1);
			pDC->LineTo( rcClient.right, rcItem.bottom - 1);
		}

		rcItem.OffsetRect( 0, m_pTreeListCtrl->GetItemHeight() );
	}

	if( m_pTreeListCtrl->GetVisibleCount() > 0 )
	{
		if( m_pTreeListCtrl->m_dwStyle&TLC_HGRID 
			&& !(m_pTreeListCtrl->m_dwStyle&TLC_HGRID_EXT) 
			&& !(m_pTreeListCtrl->m_dwStyle&TLC_VGRID_EXT) )
		{
			pDC->MoveTo( rcItem.left, rcItem.top - 1);
			pDC->LineTo( rcItem.right-1, rcItem.top - 1);
		}
	}

/*	for( int iGrid = m_pTreeListCtrl->GetVisibleCount(); 
		iGrid <= nFirstRow + nShowRows; 
		iGrid++ )
	{
		DrawGrid( pDC, rcItem );

		if( m_pTreeListCtrl->m_dwStyle&TLC_HGRID_EXT && m_pTreeListCtrl->m_dwStyle&TLC_HGRID_FULL && rcItem.right<rcClient.right )
		{
			pDC->MoveTo( rcItem.right, rcItem.bottom - 1 );
			pDC->LineTo( rcClient.right, rcItem.bottom - 1 );
		}

		rcItem.OffsetRect( 0, m_pTreeListCtrl->GetItemHeight() );
	}
*/
	pDC->SetBkMode( nOldMode );
	pDC->SelectObject( pOldFont );
	pDC->SelectObject( pOldPen );

	return TRUE;
}

BOOL CTLCDragWnd::DrawDragWnd( CDC* pDC )
{
	// draw drag window
	CRect rcItem;
	GetClientRect( &rcItem );

	pDC->FillSolidRect( &rcItem, m_pTreeListCtrl->getBkColorNonClientArea() );

	CPen Pen( PS_SOLID, 1, m_pTreeListCtrl->getGridColor() );
	CPen* pOldPen		= pDC->SelectObject( &Pen );
	CFont* pOldFont		= pDC->SelectObject( &m_pTreeListCtrl->m_Font );
	int nOldMode = pDC->SetBkMode( TRANSPARENT );

	ASSERT( m_pTreeListCtrl->m_arSelects.GetSize() > 0 );
	CTreeListItem* pShowItem = (CTreeListItem*)m_pTreeListCtrl->m_arSelects[0];

	DrawItem( pDC, rcItem, pShowItem );

	pDC->SetBkMode( nOldMode );
	pDC->SelectObject( pOldFont );
	pDC->SelectObject( pOldPen );
		
	return TRUE;
}

void CTLCDragWnd::DrawItem( CDC* pDC, CRect rcItem, CTreeListItem* pItem )
{
	//! ðèñóåì òîëüêî âûáðàííûå
	if (!pItem->GetSelected()) 
		return;
	// draw item background
	DrawItemBkgnd( pDC, rcItem, pItem );

	// draw all columns
	CTreeListColumnInfo* pColumnInfo;
	int nColPos = 0;
	for( int iShow = 0; 
		iShow < m_pTreeListCtrl->m_arShows.GetSize(); 
		iShow++, nColPos += pColumnInfo->m_nWidth 
		)
	{
		int iCol;

		// do NOT draw zero column;
		iCol = m_pTreeListCtrl->m_arShows[ iShow ];
		pColumnInfo = (CTreeListColumnInfo*)m_pTreeListCtrl->m_arColumns[iCol];
		if( pColumnInfo->m_nWidth == 0 )
			continue;

		CRect rcColumn;
		rcColumn.SetRect( 0, 0, pColumnInfo->m_nWidth, rcItem.Height() );
		rcColumn.OffsetRect( nColPos, 0 );
		rcColumn.OffsetRect( rcItem.left, rcItem.top );

		if( iCol == 0 )
		{
			if( m_pTreeListCtrl->m_dwStyle&TLC_TREELIST )
				DrawItemTree( pDC, rcColumn, pItem, iCol );
			else
				DrawItemMain( pDC, rcColumn, pItem, iCol );
		}
		else
		{
			DrawItemList( pDC, rcColumn, pItem, iCol );
		}
	}

	// draw focus rect
	CRect rcFocus;
	if( pItem == m_pTreeListCtrl->m_pFocusItem )
	{
		CPen pen;
		pen.CreatePen( PS_SOLID, 1, (COLORREF)0xFFFFFF );
		CPen* ppen = pDC->SelectObject( &pen );

		CRect rcFocus;
		rcFocus = rcItem;

		if( GetStyle()&TLC_HGRID )
			rcFocus.DeflateRect( 0, 0, 0, 1 );

		if( GetStyle()&TLC_VGRID )
			rcFocus.DeflateRect( 0, 0, 1, 0 );

		COLORREF crBkgnd, croldBkgnd;
		crBkgnd = m_pTreeListCtrl->getBkColorNonClientArea();
		croldBkgnd = pDC->SetBkColor( crBkgnd );

		pDC->DrawFocusRect( rcFocus );

		pDC->SetBkColor( croldBkgnd );
		pDC->SelectObject( ppen );
	}
}

void CTLCDragWnd::DrawGrid( CDC* pDC, CRect rcItem )
{
	// draw all grids
	CTreeListColumnInfo* pColumnInfo;
	int nColPos = 0;
	for( int iShow = 0; iShow < m_pTreeListCtrl->m_arShows.GetSize(); iShow++, nColPos += pColumnInfo->m_nWidth )
	{
		int iCol;

		// do NOT draw zero column;
		iCol = m_pTreeListCtrl->m_arShows[ iShow ];
		pColumnInfo = (CTreeListColumnInfo*)m_pTreeListCtrl->m_arColumns[iCol];
		if( pColumnInfo->m_nWidth == 0 )
			continue;

		CRect rcColumn;
		rcColumn.SetRect( 0, 0, pColumnInfo->m_nWidth, rcItem.Height() );
		rcColumn.OffsetRect( nColPos, 0 );
		rcColumn.OffsetRect( rcItem.left, rcItem.top );

		if( iCol == 0 )
		{
			// draw vertical grid line of tree
			if( m_pTreeListCtrl->m_dwStyle&TLC_HGRID_EXT && m_pTreeListCtrl->m_dwStyle&TLC_TGRID )
			{
				pDC->MoveTo( rcColumn.left, rcColumn.bottom - 1);
				pDC->LineTo( rcColumn.right-1, rcColumn.bottom - 1);
			}

			if( m_pTreeListCtrl->m_dwStyle&TLC_VGRID_EXT )
			{
				pDC->MoveTo( rcColumn.right-1, rcColumn.bottom - 1);
				pDC->LineTo( rcColumn.right-1, rcColumn.top - 1 );
			}
		}
		else
		{
			// draw vertical grid line and horizonal grid lin
			if( m_pTreeListCtrl->m_dwStyle&TLC_HGRID_EXT )
			{
				pDC->MoveTo( rcColumn.left, rcColumn.bottom - 1);
				pDC->LineTo( rcColumn.right, rcColumn.bottom - 1);
			}

			if( m_pTreeListCtrl->m_dwStyle&TLC_VGRID_EXT )
			{
				pDC->MoveTo( rcColumn.right-1, rcColumn.bottom - 1);
				pDC->LineTo( rcColumn.right-1, rcColumn.top - 1 );
			}
		}
	}
}

void CTLCDragWnd::DrawItemBkgnd( CDC* pDC, CRect rcItem, CTreeListItem* pItem )
{
	// draw selected background
	CRect		rcBkgnd;
	COLORREF	crBkgnd;
	if( pItem->GetSelected() )
	{
		rcBkgnd = rcItem;

		if( GetStyle()&TLC_HGRID )
			rcBkgnd.DeflateRect( 0, 0, 0, 1 );

		if( GetStyle()&TLC_VGRID )
			rcBkgnd.DeflateRect( 0, 0, 1, 0 );

		if( m_pTreeListCtrl->GetState( TLS_FOCUS ) )
		{
			crBkgnd = m_pTreeListCtrl->getBkColorSelectedItem();
		}
		else
		{
			crBkgnd = m_pTreeListCtrl->getBkColorSelectedItemNoFocus();
		}

		DrawItemExclude( pDC, rcItem, pItem );
		pDC->FillSolidRect( rcBkgnd, crBkgnd );
		::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
	}
}

void CTLCDragWnd::DrawItemExclude( CDC* pDC, CRect rcItem, CTreeListItem* pItem )
{
	// exclude rects of selected item
	CTreeListColumnInfo* pColumnInfo;
	int nColPos = 0;

	if( !pItem->GetSelected() )
		return;

	for( int iShow = 0; iShow < m_pTreeListCtrl->m_arShows.GetSize(); iShow++, nColPos += pColumnInfo->m_nWidth )
	{
		int iCol;
		
		iCol = m_pTreeListCtrl->m_arShows[iShow];
		pColumnInfo = (CTreeListColumnInfo*)m_pTreeListCtrl->m_arColumns[iCol];

		CRect rcColumn;
		rcColumn.SetRect( 0, 0, pColumnInfo->m_nWidth, rcItem.Height() );
		rcColumn.OffsetRect( nColPos, 0 );
		rcColumn.OffsetRect( rcItem.left, rcItem.top );

		if( iCol == 0 )
		{
			CRect rcExclude;
			int nPerfix = TLL_BORDER;
			if ( m_pTreeListCtrl->m_dwStyle&TLC_TREELIST )
			{
				nPerfix += TLL_WIDTH * ( pItem->m_nLevel - 1 );

				if( m_pTreeListCtrl->m_dwStyle&(TLC_TREELINE|TLC_BUTTON) )
					nPerfix += TLL_WIDTH;
			}

			if( m_pTreeListCtrl->m_dwStyle&TLC_CHECKBOX )
			{
				if( pItem->GetState()&TLIS_SHOWCHECKBOX )
				{
					CRect rcCheckBox;
					rcCheckBox.SetRect( 0, 0, TLL_WIDTH, TLL_WIDTH );
					rcCheckBox.OffsetRect( rcColumn.left + nPerfix, rcColumn.top + ( rcColumn.Height() - TLL_HEIGHT)/2 );
					rcExclude = rcColumn & rcCheckBox;

					if( !rcExclude.IsRectEmpty() )
						pDC->ExcludeClipRect( rcExclude );

					nPerfix += TLL_HEIGHT;
				}
			}

			if( m_pTreeListCtrl->m_dwStyle&TLC_LOCKBOX )
			{
				if( pItem->GetState()&TLIS_SHOWLOCKBOX )
				{
					CRect rcLockBox;
					rcLockBox.SetRect( 0, 0, TLL_WIDTH / 2, TLL_WIDTH );
					rcLockBox.OffsetRect( rcColumn.left + nPerfix, rcColumn.top + ( rcColumn.Height() - TLL_HEIGHT)/2 );
					rcExclude = rcColumn & rcLockBox;

					if( !rcExclude.IsRectEmpty() )
						pDC->ExcludeClipRect( rcExclude );

					nPerfix += TLL_HEIGHT / 2;
				}
			}
		
			if( m_pTreeListCtrl->m_dwStyle&TLC_IMAGE )
			{
				if( pItem->GetState()&TLIS_SHOWTREEIMAGE )
				{
					CRect rcImage;
					rcImage.SetRect( 0, 0, TLL_WIDTH, TLL_WIDTH );
					rcImage.OffsetRect( rcColumn.left + nPerfix, rcColumn.top + ( rcColumn.Height() - TLL_HEIGHT)/2 );
					rcExclude = rcColumn & rcImage;

					if( !rcExclude.IsRectEmpty() )
						pDC->ExcludeClipRect( rcExclude );

					nPerfix += TLL_HEIGHT;
				}
			}

			break;
		}
	}
}

void CTLCDragWnd::DrawItemTree( CDC* pDC, CRect rcColumn, CTreeListItem* pItem, int iCol )
{
	// draw tree column of treelist control
	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_pTreeListCtrl->m_arColumns[iCol];

	CRect rcGraph;
	rcGraph = rcColumn;
	rcGraph.DeflateRect( TLL_BORDER, 0, TLL_BORDER+1, 0 );
	if( rcGraph.IsRectEmpty() )
		return;

	CRect rcTree;
	rcTree = rcGraph;
	rcTree.right = rcTree.left + TLL_WIDTH;

	int nPerfix = TLL_BORDER;
	// draw prefix vertical line
	for( int iLevel=1; iLevel<pItem->m_nLevel; iLevel++, nPerfix += TLL_WIDTH )
	{
		CTreeListItem* pCheckItem = pItem;
		while( pCheckItem->m_nLevel != iLevel )
		{
			pCheckItem = pCheckItem->m_pParent;
		}

		rcTree.left = rcColumn.left + nPerfix;
		rcTree.right = rcTree.left + TLL_WIDTH;

		if( pCheckItem->m_pNext != NULL )
		{
			if( iLevel != 1 || m_pTreeListCtrl->m_dwStyle&TLC_ROOTLINE )
			{
				m_pTreeListCtrl->DrawItemTreeLine( 
											pDC, 
											rcGraph, 
											rcTree, 
											TLL_TOP | TLL_BOTTOM );				// ©§
			}
		}
	}

	// draw cross line
	if( m_pTreeListCtrl->m_dwStyle&(TLC_TREELINE|TLC_BUTTON) )
	{
		rcTree.left = rcColumn.left + nPerfix;
		rcTree.right = rcTree.left + TLL_WIDTH;
		if( pItem->m_nLevel == 1 )
		{
			// child item of root item
			if( m_pTreeListCtrl->m_dwStyle&TLC_ROOTLINE )
			{
				if( pItem->m_pPrev == NULL && pItem->m_pNext == NULL )
					m_pTreeListCtrl->DrawItemTreeLine( pDC, rcGraph, rcTree, TLL_RIGHT );						// ©¥ ( right part )
				else if( pItem->m_pPrev == NULL )
					m_pTreeListCtrl->DrawItemTreeLine( pDC, 
													rcGraph, 
													rcTree, 
													TLL_RIGHT | TLL_BOTTOM );	// ©³
				else if( pItem->m_pNext == NULL )
					m_pTreeListCtrl->DrawItemTreeLine( 
													pDC, 
													rcGraph, 
													rcTree, 
													TLL_RIGHT | TLL_TOP 
													);				// ©»
				else
					m_pTreeListCtrl->DrawItemTreeLine( 
													pDC, 
													rcGraph, 
													rcTree, 
													TLL_RIGHT | TLL_TOP | TLL_BOTTOM 
													);// ©Ç
			}
			else
			{
				m_pTreeListCtrl->DrawItemTreeLine( pDC, rcGraph, rcTree, TLL_RIGHT );							// ©¥ ( right part )
			}
		}
		else
		{
			// child item of other items ( not root item )
			if( pItem->m_pNext == NULL )
				m_pTreeListCtrl->DrawItemTreeLine( 
												pDC, 
												rcGraph, 
												rcTree, 
												TLL_RIGHT | TLL_TOP 
												);					// ©»
			else
				m_pTreeListCtrl->DrawItemTreeLine( 
												pDC, 
												rcGraph, 
												rcTree, 
												TLL_RIGHT | TLL_TOP | TLL_BOTTOM 
												);	// ©Ç
		}
		
/*
		// draw button of child item of root item
		if( m_pTreeListCtrl->m_dwStyle&TLC_BUTTON )
		{
			if( pItem->GetState()&TLIS_EXPANDEDONCE )
			{
				if( pItem->m_pChild != NULL )
				{
					if( pItem->GetState()&TLIS_EXPANDED )
						m_pTreeListCtrl->DrawItemTreeButton( pDC, rcGraph, rcTree, TLB_MINUS );
					else
						m_pTreeListCtrl->DrawItemTreeButton( pDC, rcGraph, rcTree, TLB_PLUS );
				}				
			}
			else
			{
				m_pTreeListCtrl->DrawItemTreeButton( pDC, rcGraph, rcTree, TLB_UNKNOW );
			}
		}
*/
		nPerfix += TLL_WIDTH;
	}

	// draw check box
	if( m_pTreeListCtrl->m_dwStyle&TLC_CHECKBOX )
	{
		if( pItem->GetState()&TLIS_SHOWCHECKBOX )
		{
			CRect rcCheckBox;
			rcCheckBox = rcGraph;
			rcCheckBox.left = rcColumn.left + nPerfix;
			rcCheckBox.right = rcCheckBox.left + TLL_WIDTH;

			POINT pt;
			pt.x = rcCheckBox.left;
			pt.y = rcCheckBox.top + (rcCheckBox.Height() - TLL_HEIGHT)/2;

			pDC->IntersectClipRect( rcGraph );
			pDC->IntersectClipRect( rcCheckBox );
			m_pTreeListCtrl->m_imgCheck.Draw( 
										pDC, 
										pItem->GetCheck(), 
										pt, 
										ILD_TRANSPARENT 
										);
			::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );

			nPerfix += TLL_WIDTH;
		}
	}

	// draw lock box
	if( m_pTreeListCtrl->m_dwStyle&TLC_LOCKBOX )
	{
		if( pItem->GetState()&TLIS_SHOWLOCKBOX )
		{
			CRect rcLockBox;
			rcLockBox = rcGraph;
			rcLockBox.left = rcColumn.left + nPerfix;
			rcLockBox.right = rcLockBox.left + TLL_WIDTH/2;

			POINT pt;
			pt.x = rcLockBox.left;
			pt.y = rcLockBox.top + (rcLockBox.Height() - TLL_HEIGHT)/2;

			pDC->IntersectClipRect( rcGraph );
			pDC->IntersectClipRect( rcLockBox );
			m_pTreeListCtrl->m_imgLock.Draw( pDC, pItem->GetLock(), pt, ILD_TRANSPARENT );
			::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
			
			nPerfix += TLL_WIDTH/2;
		}
	}

	// draw image box
	if( m_pTreeListCtrl->m_dwStyle&TLC_IMAGE )
	{
		if( pItem->GetState()&TLIS_SHOWTREEIMAGE )
		{
			CRect rcImage;
			rcImage = rcGraph;
			rcImage.left = rcColumn.left + nPerfix;
			rcImage.right = rcImage.left + TLL_WIDTH;

			POINT pt;
			pt.x = rcImage.left;
			pt.y = rcImage.top + (rcImage.Height() - TLL_HEIGHT)/2;

			pDC->IntersectClipRect( rcGraph );
			pDC->IntersectClipRect( rcImage );

			if( pItem->GetState()&TLIS_EXPANDED && pItem->m_pChild != NULL )
			{
				if( m_pTreeListCtrl->m_arSelects.GetSize() > 0 
					&& pItem == m_pTreeListCtrl->GetFocusedItem()
					/*(CTreeListItem*)m_pTreeListCtrl->m_arSelects[0]*/ )
					m_pTreeListCtrl->m_pImageList->Draw( 
														pDC, 
														pItem->m_nExpandSelectedImage, 
														pt, 
														ILD_TRANSPARENT 
														);
				else
					m_pTreeListCtrl->m_pImageList->Draw( 
														pDC, 
														pItem->m_nSelectedImage, 
														pt, 
														ILD_TRANSPARENT 
														);
			}
			else
			{
				if( m_pTreeListCtrl->m_arSelects.GetSize() > 0 
					&& pItem == m_pTreeListCtrl->GetFocusedItem()
					/*(CTreeListItem*)m_pTreeListCtrl->m_arSelects[0]*/ )
					m_pTreeListCtrl->m_pImageList->Draw( 
														pDC, 
														pItem->m_nExpandImage, 
														pt, 
														ILD_TRANSPARENT 
														);
				else
					m_pTreeListCtrl->m_pImageList->Draw( 
														pDC, 
														pItem->m_nImage, 
														pt, 
														ILD_TRANSPARENT 
														);
			}

			::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );

			nPerfix += TLL_WIDTH;
		}
	}

	// add gaps between text and image 
	if( m_pTreeListCtrl->m_dwStyle&TLC_LOCKBOX || m_pTreeListCtrl->m_dwStyle&TLC_IMAGE )
		nPerfix += 2;

	// draw item text
	CRect rcBkgnd;
	rcBkgnd = rcColumn;
	rcBkgnd.left = rcColumn.left + nPerfix;

	DrawItemTreeText( pDC, rcBkgnd, pItem, iCol );

	// draw vertical grid line of tree
	if( m_pTreeListCtrl->m_dwStyle&TLC_TGRID )
	{
		pDC->MoveTo( rcColumn.left, rcColumn.bottom - 1);
		pDC->LineTo( rcColumn.right-1, rcColumn.bottom - 1);
	}

	if( m_pTreeListCtrl->m_dwStyle&TLC_VGRID )
	{
		pDC->MoveTo( rcColumn.right-1, rcColumn.bottom - 1);
		pDC->LineTo( rcColumn.right-1, rcColumn.top - 1 );
	}

	return;
}

void CTLCDragWnd::DrawItemMain( CDC* pDC, CRect rcColumn, CTreeListItem* pItem, int iCol )
{
	// draw main column of list
	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_pTreeListCtrl->m_arColumns[iCol];

	CRect rcGraph;
	rcGraph = rcColumn;
	rcGraph.DeflateRect( TLL_BORDER, 0, TLL_BORDER+1, 0 );
	if( rcGraph.Width() <= 0 )
		return;
	
	CRect rcTree;
	rcTree = rcGraph;
	rcTree.right = rcTree.left + TLL_WIDTH;

	int nPerfix = TLL_BORDER;

	// draw check box
	if( m_pTreeListCtrl->m_dwStyle&TLC_CHECKBOX )
	{
		if( pItem->GetState()&TLIS_SHOWCHECKBOX )
		{
			CRect rcCheckBox;
			rcCheckBox = rcGraph;
			rcCheckBox.left = rcColumn.left + nPerfix;
			rcCheckBox.right = rcCheckBox.left + TLL_WIDTH;

			POINT pt;
			pt.x = rcCheckBox.left;
			pt.y = rcCheckBox.top + (rcCheckBox.Height() - TLL_HEIGHT)/2;

			pDC->IntersectClipRect( rcGraph );
			pDC->IntersectClipRect( rcCheckBox );
			m_pTreeListCtrl->m_imgCheck.Draw( pDC, pItem->GetCheck(), pt, ILD_TRANSPARENT );
			::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );

			nPerfix += TLL_WIDTH;
		}
	}

	// draw lock box
	if( m_pTreeListCtrl->m_dwStyle&TLC_LOCKBOX )
	{
		if( pItem->GetState()&TLIS_SHOWLOCKBOX )
		{
			CRect rcLockBox;
			rcLockBox = rcGraph;
			rcLockBox.left = rcColumn.left + nPerfix;
			rcLockBox.right = rcLockBox.left + TLL_WIDTH/2;

			POINT pt;
			pt.x = rcLockBox.left;
			pt.y = rcLockBox.top + (rcLockBox.Height() - TLL_HEIGHT)/2;

			pDC->IntersectClipRect( rcGraph );
			pDC->IntersectClipRect( rcLockBox );
			m_pTreeListCtrl->m_imgLock.Draw( pDC, pItem->GetLock(), pt, ILD_TRANSPARENT );
			::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );
			
			nPerfix += TLL_WIDTH/2;
		}
	}

	// draw image box
	if( m_pTreeListCtrl->m_dwStyle&TLC_IMAGE )
	{
		if( pItem->GetState()&TLIS_SHOWTREEIMAGE )
		{
			CRect rcImage;
			rcImage = rcGraph;
			rcImage.left = rcColumn.left + nPerfix;
			rcImage.right = rcImage.left + TLL_WIDTH;

			POINT pt;
			pt.x = rcImage.left;
			pt.y = rcImage.top + (rcImage.Height() - TLL_HEIGHT)/2;

			pDC->IntersectClipRect( rcGraph );
			pDC->IntersectClipRect( rcImage );

			if( m_pTreeListCtrl->m_arSelects.GetSize() > 0 
				&& pItem == m_pTreeListCtrl->GetFocusedItem()
				/*(CTreeListItem*)m_pTreeListCtrl->m_arSelects[0]*/ )
				m_pTreeListCtrl->m_pImageList->Draw( pDC, pItem->m_nExpandImage, pt, ILD_TRANSPARENT );
			else
				m_pTreeListCtrl->m_pImageList->Draw( pDC, pItem->m_nImage, pt, ILD_TRANSPARENT );

			::ExtSelectClipRgn( pDC->m_hDC, DEFAULT_HRGN, RGN_COPY );

			nPerfix += TLL_WIDTH;
		}
	}

	// add gaps between text and image 
	if( m_pTreeListCtrl->m_dwStyle&TLC_LOCKBOX || m_pTreeListCtrl->m_dwStyle&TLC_IMAGE )
		nPerfix += 2;

	// draw item text
	CRect rcBkgnd;
	rcBkgnd = rcColumn;
	rcBkgnd.left = rcColumn.left + nPerfix;

	DrawItemTreeText( pDC, rcBkgnd, pItem, iCol );

	// draw vertical grid line of tree
	if( m_pTreeListCtrl->m_dwStyle&TLC_TGRID )
	{
		pDC->MoveTo( rcColumn.left, rcColumn.bottom - 1);
		pDC->LineTo( rcColumn.right-1, rcColumn.bottom - 1);
	}

	if( m_pTreeListCtrl->m_dwStyle&TLC_VGRID )
	{
		pDC->MoveTo( rcColumn.right-1, rcColumn.bottom - 1);
		pDC->LineTo( rcColumn.right-1, rcColumn.top - 1 );
	}
}

void CTLCDragWnd::DrawItemList( CDC* pDC, CRect rcColumn, CTreeListItem* pItem, int iCol )
{
	// draw list
	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_pTreeListCtrl->m_arColumns[iCol];

	CRect rcBkgnd;
	rcBkgnd = rcColumn;
	rcBkgnd.left = rcColumn.left;

	DrawItemListText( pDC, rcBkgnd, pItem, iCol );

	// draw vertical grid line and horizonal grid lin
	if( m_pTreeListCtrl->m_dwStyle&TLC_HGRID )
	{
		pDC->MoveTo( rcColumn.left, rcColumn.bottom - 1);
		pDC->LineTo( rcColumn.right, rcColumn.bottom - 1);
	}

	if( m_pTreeListCtrl->m_dwStyle&TLC_VGRID )
	{
		pDC->MoveTo( rcColumn.right-1, rcColumn.bottom - 1);
		pDC->LineTo( rcColumn.right-1, rcColumn.top - 1 );
	}
}

void CTLCDragWnd::DrawItemTreeText( CDC* pDC, CRect rcBkgnd, CTreeListItem* pItem, int iCol )
{
	// draw text on tree
	CRect rcText;
	rcText = rcBkgnd;
	rcText.DeflateRect( 2, 0, 3, 0 );
	rcBkgnd.bottom = rcBkgnd.bottom - 1;
	DrawItemText( pDC, rcBkgnd, rcText, pItem, iCol );
}

void CTLCDragWnd::DrawItemListText( CDC* pDC, CRect rcBkgnd, CTreeListItem* pItem, int iCol )
{
	// draw text on list
	CRect rcText;
	rcText = rcBkgnd;
	rcText.DeflateRect( 2, 0, 3, 0 );
	rcBkgnd.bottom = rcBkgnd.bottom - 1;
	DrawItemText( pDC, rcBkgnd, rcText, pItem, iCol );
}

void CTLCDragWnd::DrawItemText( CDC* pDC, CRect rcBkgnd, CRect rcText, CTreeListItem* pItem, int iCol )
{
	// draw item text
	COLORREF crBkgnd;
	COLORREF crText = m_pTreeListCtrl->getNormalTextColor();

	CTreeListColumnInfo* pColumnInfo;
	pColumnInfo = (CTreeListColumnInfo*)m_pTreeListCtrl->m_arColumns[iCol];

	// do not draw zero width
	if( rcBkgnd.IsRectEmpty() )
		return;

	// do not draw not selected item
	BOOL bSelected = FALSE;
	CTreeListItem* pTempItem = pItem;
	while( pTempItem != NULL )
	{
		if( pTempItem->GetSelected() )
		{
			bSelected = TRUE;
			break;
		}
		pTempItem = pTempItem->m_pParent;
	}
	
	if( !bSelected )
		return;

	// select text and background color
	if( pItem->GetSelected() )
	{
		if( m_pTreeListCtrl->GetState( TLS_FOCUS ) )
		{
			if( !(m_pTreeListCtrl->m_dwStyle&TLC_MULTIPLESELECT) )
			{
				// single select
				if( iCol == m_pTreeListCtrl->m_iSelCol 
					&& m_pTreeListCtrl->m_dwStyle&TLC_SHOWSELACTIVE )
				{
					crBkgnd = m_pTreeListCtrl->getBkColorSelectedColumn();
					crText	= m_pTreeListCtrl->getSelectedColumnTextColor();
				}
				else
				{
					crBkgnd = m_pTreeListCtrl->getBkColorSelectedRow();
					crText	= m_pTreeListCtrl->getSelectedRowTextColor();
				}
			}
			else
			{
				// multiple select
				if( m_pTreeListCtrl->m_arSelects.GetSize() > 0 
					&& pItem == m_pTreeListCtrl->GetFocusedItem()
				  )
				{
					if( iCol == m_pTreeListCtrl->m_iSelCol 
						&& m_pTreeListCtrl->m_dwStyle&TLC_SHOWSELACTIVE )
					{
						crBkgnd = m_pTreeListCtrl->getBkColorSelectedColumn();
						crText	= m_pTreeListCtrl->getSelectedColumnTextColor();
					}
					else
					{
						crBkgnd = m_pTreeListCtrl->getBkColorSelectedRow();
						crText	= m_pTreeListCtrl->getSelectedRowTextColor();
					}
				}
				else
				{
					crBkgnd = m_pTreeListCtrl->getBkColorSelectedItem();
					crText	= m_pTreeListCtrl->getSelectedTextColor();
				}
			}
		
			pDC->FillSolidRect( rcBkgnd, crBkgnd );
		}
		else
		{
			crBkgnd = m_pTreeListCtrl->getBkColorSelectedItemNoFocus();
			crText  = m_pTreeListCtrl->getSelectedTextNoFocusColor();

			if( !(GetStyle()&TLC_SHOWSELFULLROWS) )
				pDC->FillSolidRect( rcBkgnd, crBkgnd );
		}
	}

	// draw text
	COLORREF crOldText;
	UINT dwFormat = pColumnInfo->GetTextFormat();

	crOldText = pDC->SetTextColor( crText );
	pDC->DrawText( pItem->GetText(iCol), rcText, dwFormat );
	pDC->SetTextColor( crOldText );
}

BOOL CTLCDragWnd::Show()
{
	SetWindowPos( &wndTop, 
		0, 0, 0, 0, 
		SWP_NOOWNERZORDER 
		| SWP_SHOWWINDOW 
		| SWP_NOACTIVATE 
		| SWP_NOSIZE 
		| SWP_NOMOVE );

	if( m_bLayeredWindows )
		OnPaintBMP();

	return TRUE;
}

BOOL CTLCDragWnd::Hide()
{
	SetWindowPos( &wndTop, 
		0, 0, 0, 0, 
		SWP_NOOWNERZORDER 
		| SWP_HIDEWINDOW 
		| SWP_NOACTIVATE 
		| SWP_NOSIZE 
		| SWP_NOMOVE );

	return TRUE;
}
