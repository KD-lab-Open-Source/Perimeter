#if !defined(AFX_XTREELISTCTRL_H__9BB64D45_1D06_4D18_A874_471C32A02560__INCLUDED_)
#define AFX_XTREELISTCTRL_H__9BB64D45_1D06_4D18_A874_471C32A02560__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// xTreeListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// xTreeListCtrl window
#include <xTreeListCtrl/xTreeCtrlLib.h>

class xTreeListCtrl : public CTreeListCtrl
{
// Construction
public:
	xTreeListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(xTreeListCtrl)
	//}}AFX_VIRTUAL

	CTreeListItem* FindItemByData(CTreeListItem* pParent, DWORD_PTR dwData);
	void DeleteSubItems(CTreeListItem* pParent);
	CString GetAllItemText(CTreeListItem* pItem);
		// Implementation
public:
	virtual ~xTreeListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(xTreeListCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XTREELISTCTRL_H__9BB64D45_1D06_4D18_A874_471C32A02560__INCLUDED_)
