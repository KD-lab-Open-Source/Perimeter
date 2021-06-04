#if !defined(AFX_EFFECTTREEVIEW_H__635533D7_AF75_4D42_A1B5_28427040D758__INCLUDED_)
#define AFX_EFFECTTREEVIEW_H__635533D7_AF75_4D42_A1B5_28427040D758__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffectTreeView.h : header file
//

#include "EffectToolDoc.h"


/////////////////////////////////////////////////////////////////////////////
// CEffectTreeView view

class CEffectTreeView : public CTreeView
{
	bool        m_bDragging;
	HTREEITEM   m_hitemDrag;
	HTREEITEM   m_hitemDrop;
	CImageList* m_pimagelist;

	void PopulateTree();
	void GetCheckState();

	void CloneEffect(HTREEITEM hItemDrag, HTREEITEM hItemDrop);
	void CopyEmitter(HTREEITEM hItemFrom, HTREEITEM hItemTo);
	void MoveEmitter(HTREEITEM hItemFrom, HTREEITEM hItemTo);
protected:
	CEffectTreeView();
	DECLARE_DYNCREATE(CEffectTreeView)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectTreeView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CEffectTreeView();

	CEffectToolDoc* GetDocument(){
		return (CEffectToolDoc*)CTreeView::GetDocument();
	}

	// Generated message map functions
protected:
	//{{AFX_MSG(CEffectTreeView)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateEffectDel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEffectEmitterDel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEffectEmitterNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEffectNew(CCmdUI* pCmdUI);
	afx_msg void OnEffectNew();
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEffectDel();
	afx_msg void OnEffectEmitterNew();
	afx_msg void OnEffectEmitterDel();
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	afx_msg void OnUpdateGroupNew(CCmdUI *pCmdUI);
	afx_msg void OnGroupNew();
	afx_msg void OnElementDelete();
	afx_msg void OnUpdateElementDelete(CCmdUI *pCmdUI);
	afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
	void SelChanged(HTREEITEM hItem);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTTREEVIEW_H__635533D7_AF75_4D42_A1B5_28427040D758__INCLUDED_)
