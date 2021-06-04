// EffectToolView.cpp : implementation of the CEffectToolView class
//

#include "stdafx.h"
#include "EffectTool.h"

#include "EffectToolDoc.h"
#include "EffectToolView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectToolView

IMPLEMENT_DYNCREATE(CEffectToolView, CView)

BEGIN_MESSAGE_MAP(CEffectToolView, CView)
	//{{AFX_MSG_MAP(CEffectToolView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectToolView construction/destruction

CEffectToolView::CEffectToolView()
{
}

CEffectToolView::~CEffectToolView()
{
}

BOOL CEffectToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CEffectToolView drawing

void CEffectToolView::OnDraw(CDC* pDC)
{
	CEffectToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CEffectToolView printing

BOOL CEffectToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CEffectToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CEffectToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CEffectToolView diagnostics

#ifdef _DEBUG
void CEffectToolView::AssertValid() const
{
	CView::AssertValid();
}

void CEffectToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEffectToolDoc* CEffectToolView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEffectToolDoc)));
	return (CEffectToolDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEffectToolView message handlers
