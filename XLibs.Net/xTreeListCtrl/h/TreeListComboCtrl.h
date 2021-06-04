#if !defined(AFX_TREELISTCOMBOCTRL_H__A0F04344_C334_4FD0_BE59_7928735BD3A4__INCLUDED_)
#define AFX_TREELISTCOMBOCTRL_H__A0F04344_C334_4FD0_BE59_7928735BD3A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeListComboCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeListComboCtrl window

class CTreeListComboCtrl : public CComboBox
{
// Construction
public:
	CTreeListComboCtrl();

// Members
	CTreeListCtrl*	m_pTreeListCtrl;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListComboCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeListComboCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeListComboCtrl)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREELISTCOMBOCTRL_H__A0F04344_C334_4FD0_BE59_7928735BD3A4__INCLUDED_)
