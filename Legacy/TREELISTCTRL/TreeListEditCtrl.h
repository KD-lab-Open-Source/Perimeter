#if !defined(AFX_TREELISTEDITCTRL_H__6BC562F1_ED1D_4FA7_8FFB_3185A8B7148F__INCLUDED_)
#define AFX_TREELISTEDITCTRL_H__6BC562F1_ED1D_4FA7_8FFB_3185A8B7148F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeListEditCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeListEditCtrl window
class CTreeListItem;

class CTreeListEditCtrl : public CEdit
{
// Construction
public:
	CTreeListEditCtrl();

// Members
	CTreeListCtrl*	m_pTreeListCtrl;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListEditCtrl)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeListEditCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeListEditCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREELISTEDITCTRL_H__6BC562F1_ED1D_4FA7_8FFB_3185A8B7148F__INCLUDED_)
