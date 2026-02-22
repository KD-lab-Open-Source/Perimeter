#if !defined(AFX_D3DWINDOW_H__9AB66A5F_1082_4B0D_AD14_9985DB89C9C8__INCLUDED_)
#define AFX_D3DWINDOW_H__9AB66A5F_1082_4B0D_AD14_9985DB89C9C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// D3DWindow.h : header file
//

#include "EffectToolDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CD3DWindow window

class CD3DWindow : public CView
{
	DECLARE_DYNCREATE(CD3DWindow)

	HCURSOR m_hCursorHand;
	CPoint  m_ptPrevPoint;
	bool    m_bLeftPressed;

	CEffectToolDoc* GetDocument(){
		return (CEffectToolDoc*)CView::GetDocument();
	}

void SetKeyPos(KeyPos* key_pos, const CPoint& pt, const MatXf* offs =NULL);
KeyPos* HitTestPoint(CKeyPos& key_pos, const CPoint& pt, const MatXf* offs =NULL);
Vect3f SetAllKeyPos(CKeyPos& key_pos, const CPoint& pt, const CPoint& prev_pt);
Mat3f RotateAllKeyPos(CKeyPos& key_pos, float dPsi, float dTheta);

	void SetSplineKey(const CPoint& pt);
	void PositionKeysRotate(float dPsi, float dTheta);
	void SetAllPositionKey(const CPoint& pt,const CPoint& prev_pt);
	KeyPos* HitPositionKey(const CPoint& pt);
	void SetPositionKey(const CPoint& pt, bool bNoChangePos = false);
	void SetDirectionKey(const CPoint& pt_delta);
	void SetBSDirectionKey(const CPoint& pt_delta);
	void MoveToPositionTime(float tm);
	void SetWaypointHover(const CPoint& pt);
public:
	void SetEffectCenter(CPoint pt);
	CD3DWindow();
	virtual ~CD3DWindow();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CD3DWindow)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	virtual void OnDraw(CDC* pDC){}

	// Generated message map functions
protected:
	//{{AFX_MSG(CD3DWindow)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_D3DWINDOW_H__9AB66A5F_1082_4B0D_AD14_9985DB89C9C8__INCLUDED_)
