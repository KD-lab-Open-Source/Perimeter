// WinVGDoc.h : interface of the CWinVGDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINVGDOC_H__BA1F4B08_E99E_41D9_83D3_7FD81259B0B8__INCLUDED_)
#define AFX_WINVGDOC_H__BA1F4B08_E99E_41D9_83D3_7FD81259B0B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CWinVGDoc : public CDocument
{
protected: // create from serialization only
	CWinVGDoc();
	DECLARE_DYNCREATE(CWinVGDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinVGDoc)
	public:
	virtual void Serialize(CArchive& ar);
	virtual void OnChangedViewList();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWinVGDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	class CWinVGView		*m_pView;
	class CHierarchyTree	*m_pHierarchyObj;
	class CHierarchyTree	*m_pHierarchyLogic;
	class CDirectoryTree	*m_pDirectoryTree;
	char					CurrentChannel[256];

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWinVGDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
extern CWinVGDoc* pDoc;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINVGDOC_H__BA1F4B08_E99E_41D9_83D3_7FD81259B0B8__INCLUDED_)
