// WinVGDoc.cpp : implementation of the CWinVGDoc class
//

#include "stdafx.h"
#include "WinVG.h"

#include "WinVGDoc.h"
#include "WinVGView.h"
#include "HierarchyTree.h"
#include "DirectoryTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinVGDoc

IMPLEMENT_DYNCREATE(CWinVGDoc, CDocument)

BEGIN_MESSAGE_MAP(CWinVGDoc, CDocument)
	//{{AFX_MSG_MAP(CWinVGDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinVGDoc construction/destruction
CWinVGDoc* pDoc=NULL;

CWinVGDoc::CWinVGDoc()
{
	pDoc=this;
	m_pView=0;
	m_pHierarchyObj=m_pHierarchyLogic=0;
	strcpy(CurrentChannel,"main");
	m_pDirectoryTree=0;
}

CWinVGDoc::~CWinVGDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWinVGDoc serialization

void CWinVGDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWinVGDoc diagnostics

#ifdef _DEBUG
void CWinVGDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWinVGDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinVGDoc commands

void CWinVGDoc::OnChangedViewList() 
{
	// TODO: Add your specialized code here and/or call the base class
	CDocument::OnChangedViewList();

	m_pHierarchyObj=m_pHierarchyLogic=0; m_pView=0; m_pDirectoryTree=0;
	POSITION pos=GetFirstViewPosition();
	while(pos!=0)
	{
		CView* pView=GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CWinVGView)))
			m_pView=DYNAMIC_DOWNCAST(CWinVGView,pView);
		else if(pView->IsKindOf(RUNTIME_CLASS(CHierarchyTree)))
		{
			if(m_pHierarchyObj==0)
				m_pHierarchyObj=DYNAMIC_DOWNCAST(CHierarchyTree,pView);
			else if(m_pHierarchyLogic==0)
				m_pHierarchyLogic=DYNAMIC_DOWNCAST(CHierarchyTree,pView);
		}
		else if(pView->IsKindOf(RUNTIME_CLASS(CDirectoryTree)))
		{
			if(m_pDirectoryTree==0)
			{
				m_pDirectoryTree=DYNAMIC_DOWNCAST(CDirectoryTree,pView);
				strcpy(m_pDirectoryTree->FileExtension,"*.m3d");
			}
		}
	}
}

BOOL CWinVGDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	// TODO: Add your specialized creation code here
	m_pView->LoadObject((char*)lpszPathName);
	
	return TRUE;
}
