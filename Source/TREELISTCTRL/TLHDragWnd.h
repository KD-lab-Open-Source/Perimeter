#if !defined(AFX_TLHDRAGWND_H__7A430AC6_EEC6_42AE_B06E_921A51407091__INCLUDED_)
#define AFX_TLHDRAGWND_H__7A430AC6_EEC6_42AE_B06E_921A51407091__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TLHDragWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTLHDragWnd window
#define TLHDRAGWND_CLASSNAME		_T("TurboDLL.TLHDragWnd")

class CTreeListHeaderCtrl;
class CTreeListCtrl;

class CTLHDragWnd : public CWnd
{
// Construction
public:
	CTLHDragWnd();
	virtual ~CTLHDragWnd();
	BOOL Create(CRect rcHeader, CTreeListHeaderCtrl* pTreeListHeaderCtrl, int iCol );

// Members
public:

protected:
	CTreeListHeaderCtrl*	m_pTreeListHeaderCtrl;
	int						m_iCol;

private:
	COLORREF	m_cr3DFace;
	COLORREF	m_cr3DLight;
	COLORREF	m_cr3DShadow;
	COLORREF	m_crText;

	BOOL		m_bLayeredWindows;		// has a layered window

// Attributes
public:

// Operations
public:
	BOOL	Show();
	BOOL	Hide();

protected:
	BOOL	DrawWnd( CDC* pDC );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTLHDragWnd)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CTLHDragWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TLHDRAGWND_H__7A430AC6_EEC6_42AE_B06E_921A51407091__INCLUDED_)
