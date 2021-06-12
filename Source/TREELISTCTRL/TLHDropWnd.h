#if !defined(AFX_TLHDROPWND_H__B0F82D6A_D17F_4FC6_B26F_A680E6BC990B__INCLUDED_)
#define AFX_TLHDROPWND_H__B0F82D6A_D17F_4FC6_B26F_A680E6BC990B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TLHDropWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTLHDropWnd window
#define TLHDROPWND_CLASSNAME		_T("TurboDLL.TLHDropWnd")

class CTreeListHeaderCtrl;
class CTreeListCtrl;

class CTLHDropWnd : public CWnd
{
// Construction
public:
	CTLHDropWnd();
	virtual ~CTLHDropWnd();
	BOOL Create( CTreeListHeaderCtrl* pTreeListHeaderCtrl );

// Members
public:

protected:
	CTreeListHeaderCtrl*	m_pTreeListHeaderCtrl;
	int						m_iCol;

private:
	CRgn					m_rgn;

	COLORREF	m_cr3DFace;
	COLORREF	m_cr3DLight;
	COLORREF	m_cr3DShadow;
	COLORREF	m_crText;

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
	//{{AFX_VIRTUAL(CTLHDropWnd)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CTLHDropWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TLHDROPWND_H__B0F82D6A_D17F_4FC6_B26F_A680E6BC990B__INCLUDED_)
