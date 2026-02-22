// EffectToolView.h : interface of the CEffectToolView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTTOOLVIEW_H__65C8C9BA_9673_45FE_8F1E_C9A2D583F0E4__INCLUDED_)
#define AFX_EFFECTTOOLVIEW_H__65C8C9BA_9673_45FE_8F1E_C9A2D583F0E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CEffectToolView : public CView
{
protected: // create from serialization only
	CEffectToolView();
	DECLARE_DYNCREATE(CEffectToolView)

// Attributes
public:
	CEffectToolDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectToolView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEffectToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEffectToolView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in EffectToolView.cpp
inline CEffectToolDoc* CEffectToolView::GetDocument()
   { return (CEffectToolDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTTOOLVIEW_H__65C8C9BA_9673_45FE_8F1E_C9A2D583F0E4__INCLUDED_)
