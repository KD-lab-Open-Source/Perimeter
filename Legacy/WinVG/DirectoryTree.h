#if !defined(AFX_DIRECTORYTREE_H__C3CE8C5D_EA67_4470_97BD_A269AD2B7206__INCLUDED_)
#define AFX_DIRECTORYTREE_H__C3CE8C5D_EA67_4470_97BD_A269AD2B7206__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirectoryTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirectoryTree view

class CDirectoryTree : public CTreeView
{
protected:
	CDirectoryTree();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDirectoryTree)

// Attributes
public:
	enum
	{
		I_DIRECTORY,
		I_DIRECTORY_EXPAND,
		I_FILE,
	};
// Operations
public:
	CString FileDoubleClick;
	HTREEITEM FindItem(char *StrFile,HTREEITEM hItem=0);
	char FileExtension[256];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirectoryTree)
	public:
	virtual void OnInitialUpdate();
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDirectoryTree();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	void ExpandDir(HTREEITEM hItemRoot,LPCSTR dir);
	void ExpandDir(HTREEITEM hItemRoot);
	CString GetPath(HTREEITEM hItemRoot);

	//{{AFX_MSG(CDirectoryTree)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline const char* GetFileExtension(const char *fname)
{
	for(int l=strlen(fname)-1;l>=0&&fname[l]!='\\';l--)
		if(fname[l]=='.')
			return &fname[l+1];
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRECTORYTREE_H__C3CE8C5D_EA67_4470_97BD_A269AD2B7206__INCLUDED_)
