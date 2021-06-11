#if !defined(AFX_COOLDIALOGBAR_H__6EB5FA61_FFAD_11D1_98D4_444553540000__INCLUDED_)
#define AFX_COOLDIALOGBAR_H__6EB5FA61_FFAD_11D1_98D4_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CoolDialogBar.h : header file
//

#include <afxext.h>
/////////////////////////////////////////////////////////////////////////////
// CCoolDialogBar window

class CCoolDialogBar : public CControlBar
{
	const COLORREF	m_clrBtnHilight;
	const COLORREF	m_clrBtnShadow;
	// Construction / destruction
public:
    CCoolDialogBar();

// Attributes
public:
    BOOL IsHorz() const;

// Operations
public:

// Overridables
    virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

// Overrides
public:
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCoolDialogBar)
	public:
    virtual BOOL Create(CWnd* pParentWnd, CDialog *pDialog, 
		 DWORD dwStyle/* = WS_CHILD | WS_VISIBLE | CBRS_LEFT*/, UINT nID);
    virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );
    virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );
	virtual BOOL DestroyWindow();
	BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CCoolDialogBar();
    void StartTracking();
    void StopTracking(BOOL bAccept);
    void OnInvertTracker(const CRect& rect);
    
    // implementation helpers
    CPoint& ClientToWnd(CPoint& point);

protected:
	void		DrawGripper(CDC &dc);

    CSize       m_sizeMin;
    CSize       m_sizeHorz;
    CSize       m_sizeVert;
    CSize       m_sizeFloat;
    CRect       m_rectBorder;
    CRect       m_rectTracker;
    UINT        m_nDockBarID;
    CPoint      m_ptOld;
    BOOL        m_bTracking;
    BOOL        m_bInRecalcNC;
    int         m_cxEdge;
	CRect		m_rectUndock;
	CRect		m_rectClose;
	CRect		m_rectGripper;
	int			m_cxGripper;
	int			m_cxBorder;
	CDialog*	m_cDialog;
    CBrush		m_brushBkgd;

	// Rob Wolpov 10/15/98 Added support for diagonal resizing
	int  m_cyBorder;
	int  m_cMinWidth;
	int  m_cMinHeight;
	int  m_cCaptionSize;

// Generated message map functions
protected:
   //{{AFX_MSG(CCoolDialogBar)
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
    afx_msg void OnNcPaint();
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
#if defined(_MSC_VER) && (_MSC_VER < 1900)
    afx_msg UINT OnNcHitTest(CPoint point);
#else
    afx_msg LRESULT OnNcHitTest(CPoint point);
#endif
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnCaptureChanged(CWnd *pWnd);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COOLDIALOGBAR_H__6EB5FA61_FFAD_11D1_98D4_444553540000__INCLUDED_)
