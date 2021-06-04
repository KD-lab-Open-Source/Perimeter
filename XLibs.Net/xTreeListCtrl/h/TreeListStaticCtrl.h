#if !defined(AFX_TREELISTSTATICCTRL_H__D8B02049_F349_43DE_80FE_428B7ACEA3BC__INCLUDED_)
#define AFX_TREELISTSTATICCTRL_H__D8B02049_F349_43DE_80FE_428B7ACEA3BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeListStaticCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeListStaticCtrl window
class CTreeListCtrl;

class CTreeListStaticCtrl : public CStatic
{
// Construction
public:
	CTreeListStaticCtrl();

// Members
	CTreeListCtrl*	m_pTreeListCtrl;

// Attributes
public:

// Operations
public:

private:
	BOOL PreTranslateKeyDownMessage( MSG* pMsg );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListStaticCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeListStaticCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeListStaticCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREELISTSTATICCTRL_H__D8B02049_F349_43DE_80FE_428B7ACEA3BC__INCLUDED_)
