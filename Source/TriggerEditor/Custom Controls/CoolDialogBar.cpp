// CoolDialogBar.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"    // for CDockContext
#include "CoolDialogBar.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCoolDialogBar

CCoolDialogBar::CCoolDialogBar() : 
m_clrBtnHilight(::GetSysColor(COLOR_BTNHILIGHT)),
m_clrBtnShadow(::GetSysColor(COLOR_BTNSHADOW))
{
    m_sizeMin = CSize(32, 32);
    m_sizeHorz = CSize(200, 200);
    m_sizeVert = CSize(200, 200);
    m_sizeFloat = CSize(200, 200);
    m_bTracking = FALSE;
    m_bInRecalcNC = FALSE;
    m_cxEdge = 6;//0  // \|/ Border за который изменяют размер
	m_cxBorder = 3;//0
	m_cxGripper = 20;
	m_cDialog = NULL;
	m_brushBkgd.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

	// Rob Wolpov 10/15/98 Added support for diagonal resizing
	m_cyBorder	 = 3;		
	m_cCaptionSize = GetSystemMetrics(SM_CYSMCAPTION);
	m_cMinWidth	 = GetSystemMetrics(SM_CXMIN);
	m_cMinHeight	 = GetSystemMetrics(SM_CYMIN);

	m_nDockBarID=0;
}

CCoolDialogBar::~CCoolDialogBar()
{
}

BEGIN_MESSAGE_MAP(CCoolDialogBar, CControlBar)
    //{{AFX_MSG_MAP(CCoolDialogBar)
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_SETCURSOR()
    ON_WM_WINDOWPOSCHANGED()
    ON_WM_NCPAINT()
    ON_WM_NCLBUTTONDOWN()
    ON_WM_NCHITTEST()
    ON_WM_NCCALCSIZE()
    ON_WM_LBUTTONDOWN()
    ON_WM_CAPTURECHANGED()
    ON_WM_LBUTTONDBLCLK()
	ON_WM_NCLBUTTONDBLCLK()
    ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoolDialogBar message handlers

void CCoolDialogBar::OnUpdateCmdUI(class CFrameWnd *pTarget, int bDisableIfNoHndler)
{
    UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

BOOL CCoolDialogBar::Create(CWnd* pParentWnd, CDialog *pDialog, 
							DWORD dwStyle, UINT nID) 
{
    ASSERT_VALID(pParentWnd);   // must have a parent
    ASSERT (!((dwStyle & CBRS_SIZE_FIXED) && (dwStyle & CBRS_SIZE_DYNAMIC)));
	
    // save the style -- AMENDED by Holger Thiele - Thankyou
    m_dwStyle = dwStyle & CBRS_ALL;

	// create the base window
    CString wndclass = 
		AfxRegisterWndClass(CS_DBLCLKS, LoadCursor(NULL, IDC_ARROW), m_brushBkgd, 0);
    if (!CControlBar::Create(wndclass, "", dwStyle, CRect(0,0,0,0), pParentWnd, nID))
        return FALSE;

	// create the child dialog
	m_cDialog = pDialog;
	if (!m_cDialog->Create(nID, this))
		return FALSE;
	CString str;
	m_cDialog->GetWindowText(str);
	SetWindowText(str);
	CWnd::ModifyStyle(*m_cDialog, WS_CAPTION, 0, SWP_NOSIZE);

	// use the dialog dimensions as default base dimensions
	CRect rc;
    m_cDialog->GetWindowRect(rc);
    m_sizeHorz = m_sizeVert = m_sizeFloat = rc.Size();
	m_sizeHorz.cy += m_cxEdge + m_cxBorder;
	m_sizeVert.cx += m_cxEdge + m_cxBorder;

    return TRUE;
}

BOOL CCoolDialogBar::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_cDialog->DestroyWindow();
	m_cDialog=NULL;
	return CControlBar::DestroyWindow();
}


CSize CCoolDialogBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
    CRect rc;

    m_pDockSite->GetControlBar(AFX_IDW_DOCKBAR_TOP)->GetWindowRect(rc);
    int nHorzDockBarWidth = bStretch ? 32767 : rc.Width() + 4;
    m_pDockSite->GetControlBar(AFX_IDW_DOCKBAR_LEFT)->GetWindowRect(rc);
    int nVertDockBarHeight = bStretch ? 32767 : rc.Height() + 4;

	if(IsFloating())
		return m_sizeFloat;
    else if (bHorz)
        return CSize(nHorzDockBarWidth, m_sizeHorz.cy);
    else
        return CSize(m_sizeVert.cx, nVertDockBarHeight);
}

CSize CCoolDialogBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	// Rob Wolpov 10/15/98 Added support for diagonal sizing
	if (IsFloating())
	{
		// Enable diagonal arrow cursor for resizing
		GetParent()->GetParent()->ModifyStyle(MFS_4THICKFRAME,0);
	}    
	if (dwMode & (LM_HORZDOCK | LM_VERTDOCK))
	{
		SetWindowPos(NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER |
			SWP_NOACTIVATE | SWP_FRAMECHANGED);
	 	m_pDockSite->RecalcLayout();
	    return CControlBar::CalcDynamicLayout(nLength,dwMode);
	}

    if (dwMode & LM_MRUWIDTH)
        return m_sizeFloat;

    if (dwMode & LM_COMMIT)
    {
        m_sizeFloat.cx = nLength;
        return m_sizeFloat;
    }

	// Rob Wolpov 10/15/98 Added support for diagonal sizing
	if (IsFloating())
	{
		RECT	window_rect;
		POINT	cursor_pt;
		
		GetCursorPos(&cursor_pt);
		GetParent()->GetParent()->GetWindowRect(&window_rect);
		
		switch (m_pDockContext->m_nHitTest)
		{
		case HTTOPLEFT:
			m_sizeFloat.cx = (std::max)(static_cast<int>(window_rect.right - cursor_pt.x),
				m_cMinWidth) - m_cxBorder;
			m_sizeFloat.cy = (std::max)(static_cast<int>(window_rect.bottom - m_cCaptionSize - 
				cursor_pt.y),m_cMinHeight) - 1;
			m_pDockContext->m_rectFrameDragHorz.top = (std::min)(cursor_pt.y,
				static_cast<LONG>(window_rect.bottom - m_cCaptionSize - m_cMinHeight)) - 
				m_cyBorder;
			m_pDockContext->m_rectFrameDragHorz.left = (std::min)(cursor_pt.x,
				static_cast<LONG>(window_rect.right - m_cMinWidth)) - 1;
			return m_sizeFloat;
			
		case HTTOPRIGHT:
			m_sizeFloat.cx = (std::max)(static_cast<int>(cursor_pt.x - window_rect.left),
				m_cMinWidth);
			m_sizeFloat.cy = (std::max)(static_cast<int>(window_rect.bottom - m_cCaptionSize - 
				cursor_pt.y),m_cMinHeight) - 1;
			m_pDockContext->m_rectFrameDragHorz.top = (std::min)(cursor_pt.y,
				static_cast<LONG>(window_rect.bottom - m_cCaptionSize - m_cMinHeight)) - 
				m_cyBorder;
			return m_sizeFloat;
			
		case HTBOTTOMLEFT:
			m_sizeFloat.cx = (std::max)(static_cast<int>(window_rect.right - cursor_pt.x),
				m_cMinWidth) - m_cxBorder;
			m_sizeFloat.cy = (std::max)(static_cast<int>(cursor_pt.y - window_rect.top - 
				m_cCaptionSize),m_cMinHeight);
			m_pDockContext->m_rectFrameDragHorz.left = (std::min)(cursor_pt.x,
				static_cast<LONG>(window_rect.right - m_cMinWidth)) - 1;
			return m_sizeFloat;
			
		case HTBOTTOMRIGHT:
			m_sizeFloat.cx = (std::max)(static_cast<int>(cursor_pt.x - window_rect.left),
				m_cMinWidth);
			m_sizeFloat.cy = (std::max)(static_cast<int>(cursor_pt.y - window_rect.top - 
				m_cCaptionSize),m_cMinHeight);
			return m_sizeFloat;
		}
	}
	
	if (dwMode & LM_LENGTHY)
        return CSize(m_sizeFloat.cx,
            m_sizeFloat.cy = (std::max)(static_cast<int>(m_sizeMin.cy), nLength));
    else
        return CSize((std::max)(static_cast<int>(m_sizeMin.cx), nLength), m_sizeFloat.cy);
}

void CCoolDialogBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
    CControlBar::OnWindowPosChanged(lpwndpos);

	if(!::IsWindow(m_hWnd) || m_cDialog==NULL)
		return;
	if(!::IsWindow(m_cDialog->m_hWnd))
		return;
    if (m_bInRecalcNC) 
	{
		CRect rc;
		GetClientRect(rc);
		m_cDialog->MoveWindow(rc);
		return;
	}

    // Find on which side are we docked
    UINT nDockBarID = GetParent()->GetDlgCtrlID();

    // Return if dropped at same location
    if (nDockBarID == m_nDockBarID // no docking side change
        && (lpwndpos->flags & SWP_NOSIZE) // no size change
        && ((m_dwStyle & CBRS_BORDER_ANY) != CBRS_BORDER_ANY))
        return; 

    m_nDockBarID = nDockBarID;

    // Force recalc the non-client area
    m_bInRecalcNC = TRUE;
    SetWindowPos(NULL, 0,0,0,0,
        SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    m_bInRecalcNC = FALSE;
}

BOOL CCoolDialogBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    if ((nHitTest != HTSIZE) || m_bTracking)
        return CControlBar::OnSetCursor(pWnd, nHitTest, message);

    if (IsHorz())
        SetCursor(LoadCursor(NULL, IDC_SIZENS));
    else
        SetCursor(LoadCursor(NULL, IDC_SIZEWE));
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////
// Mouse Handling
//
void CCoolDialogBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
    if (!m_bTracking)
        CControlBar::OnLButtonUp(nFlags, point);
    else
    {
        ClientToWnd(point);
        StopTracking(TRUE);
    }
}

void CCoolDialogBar::OnMouseMove(UINT nFlags, CPoint point) 
{
    if (IsFloating() || !m_bTracking)
    {
        CControlBar::OnMouseMove(nFlags, point);
        return;
    }

    CPoint cpt = m_rectTracker.CenterPoint();

    ClientToWnd(point);

    if (IsHorz())
    {
        if (cpt.y != point.y)
        {
            OnInvertTracker(m_rectTracker);
            m_rectTracker.OffsetRect(0, point.y - cpt.y);
            OnInvertTracker(m_rectTracker);
        }
    }
    else 
    {
        if (cpt.x != point.x)
        {
            OnInvertTracker(m_rectTracker);
            m_rectTracker.OffsetRect(point.x - cpt.x, 0);
            OnInvertTracker(m_rectTracker);
        }
    }
}

void CCoolDialogBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
    // Compute the rectangle of the mobile edge
    GetWindowRect(m_rectBorder);
    m_rectBorder = CRect(0, 0, m_rectBorder.Width(), m_rectBorder.Height());
    
    DWORD dwBorderStyle = m_dwStyle | CBRS_BORDER_ANY;

    switch(m_nDockBarID)
    {
    case AFX_IDW_DOCKBAR_TOP:
        dwBorderStyle &= ~CBRS_BORDER_BOTTOM;
        lpncsp->rgrc[0].left += m_cxGripper;
        lpncsp->rgrc[0].bottom += -m_cxEdge;
        lpncsp->rgrc[0].top += m_cxBorder;
        lpncsp->rgrc[0].right += -m_cxBorder;
	    m_rectBorder.top = m_rectBorder.bottom - m_cxEdge;
        break;
    case AFX_IDW_DOCKBAR_BOTTOM:
        dwBorderStyle &= ~CBRS_BORDER_TOP;
        lpncsp->rgrc[0].left += m_cxGripper;
        lpncsp->rgrc[0].top += m_cxEdge;
        lpncsp->rgrc[0].bottom += -m_cxBorder;
        lpncsp->rgrc[0].right += -m_cxBorder;
        m_rectBorder.bottom = m_rectBorder.top + m_cxEdge;
        break;
    case AFX_IDW_DOCKBAR_LEFT:
        dwBorderStyle &= ~CBRS_BORDER_RIGHT;
        lpncsp->rgrc[0].right += -m_cxEdge;
        lpncsp->rgrc[0].left += m_cxBorder;
        lpncsp->rgrc[0].bottom += -m_cxBorder;
        lpncsp->rgrc[0].top += m_cxGripper;
        m_rectBorder.left = m_rectBorder.right - m_cxEdge;
        break;
    case AFX_IDW_DOCKBAR_RIGHT:
        dwBorderStyle &= ~CBRS_BORDER_LEFT;
        lpncsp->rgrc[0].left += m_cxEdge;
        lpncsp->rgrc[0].right += -m_cxBorder;
        lpncsp->rgrc[0].bottom += -m_cxBorder;
        lpncsp->rgrc[0].top += m_cxGripper;
        m_rectBorder.right = m_rectBorder.left + m_cxEdge;
        break;
    default:
        m_rectBorder.SetRectEmpty();
        break;
    }

    SetBarStyle(dwBorderStyle);
}

void CCoolDialogBar::OnNcPaint() 
{
    EraseNonClient();

	CWindowDC dc(this);
    dc.Draw3dRect(m_rectBorder, GetSysColor(COLOR_BTNHIGHLIGHT),
                    GetSysColor(COLOR_BTNSHADOW));

	DrawGripper(dc);
	
	CRect pRect;
	GetClientRect( &pRect );
	InvalidateRect( &pRect, FALSE );
}

void CCoolDialogBar::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
    if (m_bTracking) return;

	if((nHitTest == HTSYSMENU) && !IsFloating())
        GetDockingFrame()->ShowControlBar(this, FALSE, FALSE);
    else if ((nHitTest == HTMINBUTTON) && !IsFloating())
        m_pDockContext->ToggleDocking();
	else if ((nHitTest == HTCAPTION) && !IsFloating() && (m_pDockBar != NULL))
    {
        // start the drag
        ASSERT(m_pDockContext != NULL);
        m_pDockContext->StartDrag(point);
    }
    else if ((nHitTest == HTSIZE) && !IsFloating())
        StartTracking();
    else    
        CControlBar::OnNcLButtonDown(nHitTest, point);
}

UINT CCoolDialogBar::OnNcHitTest(CPoint point) 
{
    if (IsFloating())
        return CControlBar::OnNcHitTest(point);

    CRect rc;
    GetWindowRect(rc);
    point.Offset(-rc.left, -rc.top);
	if(m_rectClose.PtInRect(point))
		return HTSYSMENU;
	else if (m_rectUndock.PtInRect(point))
		return HTMINBUTTON;
	else if (m_rectGripper.PtInRect(point))
		return HTCAPTION;
    else if (m_rectBorder.PtInRect(point))
        return HTSIZE;
    else
        return CControlBar::OnNcHitTest(point);
}

void CCoolDialogBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
    // only start dragging if clicked in "void" space
    if (m_pDockBar != NULL)
    {
        // start the drag
        ASSERT(m_pDockContext != NULL);
        ClientToScreen(&point);
        m_pDockContext->StartDrag(point);
    }
    else
    {
        CWnd::OnLButtonDown(nFlags, point);
    }
}

void CCoolDialogBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    // only toggle docking if clicked in "void" space
    if (m_pDockBar != NULL)
    {
        // toggle docking
        ASSERT(m_pDockContext != NULL);
        m_pDockContext->ToggleDocking();
    }
    else
    {
        CWnd::OnLButtonDblClk(nFlags, point);
    }
}

void CCoolDialogBar::StartTracking()
{
    SetCapture();

    // make sure no updates are pending
    RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW);
    m_pDockSite->LockWindowUpdate();

    m_ptOld = m_rectBorder.CenterPoint();
    m_bTracking = TRUE;
    
    m_rectTracker = m_rectBorder;
    if (!IsHorz()) m_rectTracker.bottom -= 4;

    OnInvertTracker(m_rectTracker);
}

void CCoolDialogBar::OnCaptureChanged(CWnd *pWnd) 
{
    if (m_bTracking && pWnd != this)
        StopTracking(FALSE); // cancel tracking

    CControlBar::OnCaptureChanged(pWnd);
}

void CCoolDialogBar::StopTracking(BOOL bAccept)
{
    OnInvertTracker(m_rectTracker);
    m_pDockSite->UnlockWindowUpdate();
    m_bTracking = FALSE;
    ReleaseCapture();
    
    if (!bAccept) return;

    int maxsize, minsize, newsize;
    CRect rcc;
    m_pDockSite->GetWindowRect(rcc);

    newsize = IsHorz() ? m_sizeHorz.cy : m_sizeVert.cx;
    maxsize = newsize + (IsHorz() ? rcc.Height() : rcc.Width());
    minsize = IsHorz() ? m_sizeMin.cy : m_sizeMin.cx;

    CPoint point = m_rectTracker.CenterPoint();
    switch (m_nDockBarID)
    {
    case AFX_IDW_DOCKBAR_TOP:
        newsize += point.y - m_ptOld.y; break;
    case AFX_IDW_DOCKBAR_BOTTOM:
        newsize += -point.y + m_ptOld.y; break;
    case AFX_IDW_DOCKBAR_LEFT:
        newsize += point.x - m_ptOld.x; break;
    case AFX_IDW_DOCKBAR_RIGHT:
        newsize += -point.x + m_ptOld.x; break;
    }

	newsize = (std::max)(minsize, (std::min)(maxsize, newsize));

    if (IsHorz())
        m_sizeHorz.cy = newsize;
    else
        m_sizeVert.cx = newsize;

    m_pDockSite->RecalcLayout();
}

void CCoolDialogBar::OnInvertTracker(const CRect& rect)
{
    ASSERT_VALID(this);
    ASSERT(!rect.IsRectEmpty());
    ASSERT(m_bTracking);

    CRect rct = rect, rcc, rcf;
    GetWindowRect(rcc);
    m_pDockSite->GetWindowRect(rcf);

    rct.OffsetRect(rcc.left - rcf.left, rcc.top - rcf.top);
    rct.DeflateRect(1, 1);

    CDC *pDC = m_pDockSite->GetDCEx(NULL,
        DCX_WINDOW|DCX_CACHE|DCX_LOCKWINDOWUPDATE);

    CBrush* pBrush = CDC::GetHalftoneBrush();
    HBRUSH hOldBrush = NULL;
    if (pBrush != NULL)
        hOldBrush = (HBRUSH)SelectObject(pDC->m_hDC, pBrush->m_hObject);

    pDC->PatBlt(rct.left, rct.top, rct.Width(), rct.Height(), PATINVERT);

    if (hOldBrush != NULL)
        SelectObject(pDC->m_hDC, hOldBrush);

    m_pDockSite->ReleaseDC(pDC);
}

BOOL CCoolDialogBar::IsHorz() const
{
    return (m_nDockBarID == AFX_IDW_DOCKBAR_TOP ||
        m_nDockBarID == AFX_IDW_DOCKBAR_BOTTOM);
}

CPoint& CCoolDialogBar::ClientToWnd(CPoint& point)
{
    if (m_nDockBarID == AFX_IDW_DOCKBAR_BOTTOM)
        point.y += m_cxEdge;
    else if (m_nDockBarID == AFX_IDW_DOCKBAR_RIGHT)
        point.x += m_cxEdge;

    return point;
}

void CCoolDialogBar::DrawGripper(CDC & dc)
{
    // no gripper if floating
	if( m_dwStyle & CBRS_FLOATING )
		return;

	// -==HACK==-
	// in order to calculate the client area properly after docking,
	// the client area must be recalculated twice (I have no idea why)
	m_pDockSite->RecalcLayout();
	// -==END HACK==-

	CRect gripper;
	GetWindowRect( gripper );
	ScreenToClient( gripper );
	gripper.OffsetRect( -gripper.left, -gripper.top );
	
	if( m_dwStyle & CBRS_ORIENT_HORZ ) {
		
		// gripper at left
		m_rectGripper.top		= gripper.top + 40;
		m_rectGripper.bottom	= gripper.bottom;
		m_rectGripper.left		= gripper.left;
		m_rectGripper.right	= gripper.left + 20;

		// draw close box
		m_rectClose.left = gripper.left + 7;
		m_rectClose.right = m_rectClose.left + 12;
		m_rectClose.top = gripper.top + 10;
		m_rectClose.bottom = m_rectClose.top + 12;
		dc.DrawFrameControl(m_rectClose, DFC_CAPTION, DFCS_CAPTIONCLOSE);

		// draw docking toggle box
		m_rectUndock = m_rectClose;
		m_rectUndock.OffsetRect(0,13);
		dc.DrawFrameControl(m_rectUndock, DFC_CAPTION, DFCS_CAPTIONMAX);

		gripper.top += 38;
		gripper.bottom -= 10;
		gripper.left += 10;
		gripper.right = gripper.left+3;
        dc.Draw3dRect( gripper, m_clrBtnHilight, m_clrBtnShadow );
		
		gripper.OffsetRect(4, 0);
        dc.Draw3dRect( gripper, m_clrBtnHilight, m_clrBtnShadow );
	}
	
	else {
		
		// gripper at top
		m_rectGripper.top		= gripper.top;
		m_rectGripper.bottom	= gripper.top + 20;
		m_rectGripper.left		= gripper.left;
		m_rectGripper.right		= gripper.right - 40;

		// draw close box
		m_rectClose.right = gripper.right - 10;
		m_rectClose.left = m_rectClose.right - 11;
		m_rectClose.top = gripper.top + 7;
		m_rectClose.bottom = m_rectClose.top + 11;
		dc.DrawFrameControl(m_rectClose, DFC_CAPTION, DFCS_CAPTIONCLOSE);


		// draw docking toggle box
		m_rectUndock = m_rectClose;
		m_rectUndock.OffsetRect(-13,0);
		dc.DrawFrameControl(m_rectUndock, DFC_CAPTION, DFCS_CAPTIONMAX);

		gripper.right -= 38;
		gripper.left += 10;
		gripper.top += 10;
		gripper.bottom = gripper.top+3;
		dc.Draw3dRect( gripper, m_clrBtnHilight, m_clrBtnShadow );
		
		gripper.OffsetRect(0, 4);
        dc.Draw3dRect( gripper, m_clrBtnHilight, m_clrBtnShadow );
	}

}

void CCoolDialogBar::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    if ((m_pDockBar != NULL) && (nHitTest == HTCAPTION))
    {
        // toggle docking
        ASSERT(m_pDockContext != NULL);
        m_pDockContext->ToggleDocking();
    }
    else
    {
        CWnd::OnNcLButtonDblClk(nHitTest, point);
    }
}

BOOL CCoolDialogBar::OnEraseBkgnd(CDC* pDC)
{
	return CControlBar::OnEraseBkgnd(pDC);
}
