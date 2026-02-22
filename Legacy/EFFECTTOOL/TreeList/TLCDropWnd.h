#if !defined(AFX_TLCDROPWND_H__9F84887C_A5FA_4B59_B86F_4C2F51F58865__INCLUDED_)
#define AFX_TLCDROPWND_H__9F84887C_A5FA_4B59_B86F_4C2F51F58865__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TLCDropWnd.h : header file
//

#define TLCDROPWND_CLASSNAME		_T("TurboDLL.TLCDropWnd")
/////////////////////////////////////////////////////////////////////////////
// CTLCDropWnd window
class CTreeListCtrl;

class CTLCDropWnd : public CWnd
{
// Construction
public:
	CTLCDropWnd();
	virtual ~CTLCDropWnd();
	BOOL Create( CTreeListCtrl* pTreeListCtrl );

// Attributes
public:
	CTreeListCtrl*	m_pTreeListCtrl;

// Members
public:

private:
	CRgn					m_rgn;

	BOOL			m_bLayeredWindows;		// has a layered window

// Attributes
public:

// Operations
public:
	BOOL	Show( CPoint pt );
	BOOL	Hide();

protected:
	BOOL	DrawWnd( CDC* pDC );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTLCDropWnd)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CTLCDropWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TLCDROPWND_H__9F84887C_A5FA_4B59_B86F_4C2F51F58865__INCLUDED_)
