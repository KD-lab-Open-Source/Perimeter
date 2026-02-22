#if !defined(AFX_TLCDRAGWND_H__F6816584_AB9D_4C95_82E3_F69CCD0E9CC4__INCLUDED_)
#define AFX_TLCDRAGWND_H__F6816584_AB9D_4C95_82E3_F69CCD0E9CC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TLCDragWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTLCDragWnd window
#define TLCDRAGWND_CLASSNAME		_T("TurboDLL.TLCDragWnd")

class CTreeListCtrl;

class CTLCDragWnd : public CWnd
{
// Construction
public:
	CTLCDragWnd();
	virtual ~CTLCDragWnd();
	BOOL Create( CRect rcWindow, CTreeListCtrl* pTreeListCtrl );

// Members
public:

protected:
	CTreeListCtrl* m_pTreeListCtrl;

public:
	BOOL		m_bLayeredWindows;		// has a layered window

private:
	CDC			m_dcMem;
	COLORREF*	m_pDIB;
	CBitmap*	m_pBitmap;
	CBitmap*	m_pBitmap2;

// Attributes
public:

// Operations
public:
	BOOL	Show();
	BOOL	Hide();

protected:
	void	OnPaintBMP();
	BOOL	DrawWnd( CDC* pDC );
	BOOL	DrawDragWnd( CDC* pDC );
	void	DrawItem( CDC* pDC, CRect rcItem, CTreeListItem* pItem );
	void	DrawGrid( CDC* pDC, CRect rcItem );
	void	DrawItemBkgnd( CDC* pDC, CRect rcItem, CTreeListItem* pItem );
	void	DrawItemExclude( CDC* pDC, CRect rcItem, CTreeListItem* pItem );
	void	DrawItemTree( CDC* pDC, CRect rcColumn, CTreeListItem* pItem, int iCol );
	void	DrawItemMain( CDC* pDC, CRect rcColumn, CTreeListItem* pItem, int iCol );
	void	DrawItemList( CDC* pDC, CRect rcColumn, CTreeListItem* pItem, int iCol );
	void	DrawItemTreeText( CDC* pDC, CRect rcBkgnd, CTreeListItem* pItem, int iCol );
	void	DrawItemListText( CDC* pDC, CRect rcBkgnd, CTreeListItem* pItem, int iCol );
	void	DrawItemText( CDC* pDC, CRect rcBkgnd, CRect rcText, CTreeListItem* pItem, int iCol );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTLCDragWnd)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CTLCDragWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TLCDRAGWND_H__F6816584_AB9D_4C95_82E3_F69CCD0E9CC4__INCLUDED_)
