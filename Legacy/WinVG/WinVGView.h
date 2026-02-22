// WinVGView.h : interface of the CWinVGView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINVGVIEW_H__999B7B91_AFA9_4CC9_BD5A_2C3A5EE406AF__INCLUDED_)
#define AFX_WINVGVIEW_H__999B7B91_AFA9_4CC9_BD5A_2C3A5EE406AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CWinVGView : public CView
{
protected: // create from serialization only
	CWinVGView();
	DECLARE_DYNCREATE(CWinVGView)

// Attributes
public:
	CWinVGDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinVGView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
public:
	friend class CHierarchyTree;
	double m_ScaleTime,m_OldScaleTime;
	int m_NumberAnimChain,dwScrX,dwScrY,m_ControlSwitch;
	int bWireFrame	: 1;
	int bMouseLDown	: 1;
	int bMouseRDown	: 1;
	bool show_basement;
	CPoint	PointMouseLDown;
	CPoint	PointMouseRDown;
	sColor4c Color;
	sColor4f SkinColor;

	Vect3f target_pos;
	Se3f camera_pos;
	cTexture* pLightMap;

	bool b_viewHologram;
	bool enable_tilemap;
	void DoneRenderDevice();
	int InitRenderDevice(int xScr,int yScr);
	void LoadObject(LPCSTR fname);
	void ObjectControl(Vect3f &dPos,Vect3f &dAngle,float dScale);
	virtual ~CWinVGView();

	void UpdateIgnore();

	void SetScale(bool normal);

	void UpdateObjectLight();
	void SetEffectDirectory(const char* dir);

	void ResetCameraPosition();
	void SetCameraPosition(float du,float dv,float dscale);

	void ModelInfo();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	afx_msg void OnNumObject();
	afx_msg void OnUpdateNumObject(CCmdUI* pCmdUI);
	afx_msg void OnHologram();
	afx_msg void OnUpdateHologram(CCmdUI* pCmdUI);

	void DrawLocalBorder(cObjectNodeRoot* root);
	void OnScreenShoot();
	void SynhronizeObjAndLogic(cIUnkClass *UObj,cIUnkClass *ULogic);
protected:
	void TestObject(cIUnkClass* UObj);
	void Draw(bool show_old_model);
// Generated message map functions
protected:
	//{{AFX_MSG(CWinVGView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in WinVGView.cpp
inline CWinVGDoc* CWinVGView::GetDocument()
   { return (CWinVGDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
extern CWinVGView*		pView;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINVGVIEW_H__999B7B91_AFA9_4CC9_BD5A_2C3A5EE406AF__INCLUDED_)
