#if !defined(AFX_HIERARCHYTREE_H__EEE3EE74_83CF_4195_9247_035904F5D4D8__INCLUDED_)
#define AFX_HIERARCHYTREE_H__EEE3EE74_83CF_4195_9247_035904F5D4D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HierarchyTree.h : header file
//

#include <vector>
/////////////////////////////////////////////////////////////////////////////
// CHierarchyTree view

class CHierarchyTree : public CTreeView
{
protected:
	CHierarchyTree();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CHierarchyTree)

// Attributes
public:
// Operations
	void TreeUpdate();
	void GetStringObj(CString cstr,char *StrSelectObj);
	HTREEITEM FindItem(char *StrObj,HTREEITEM hItemParent=0);

	void SetRoot(cIUnkClass* pUObj,CString filename);
	void AddRoot(cIUnkClass* pUObj);

	cIUnkClass* GetRoot();
	void ClearRoot();
	inline vector<cIUnkClass*>& GetAllObj(){return UObj;};

	CString GetFileName(){return filename;};
	void SetSkinColor(sColor4f& c);
protected:
	vector<cIUnkClass*> UObj;
	CString filename;
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHierarchyTree)
	public:
	virtual void OnInitialUpdate();
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHierarchyTree();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	//{{AFX_MSG(CHierarchyTree)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HIERARCHYTREE_H__EEE3EE74_83CF_4195_9247_035904F5D4D8__INCLUDED_)
