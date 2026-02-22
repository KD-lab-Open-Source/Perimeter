#if !defined(AFX_CONTROLVIEW_H__9972C9E8_E7FF_436C_93CC_90A8403EF0D5__INCLUDED_)
#define AFX_CONTROLVIEW_H__9972C9E8_E7FF_436C_93CC_90A8403EF0D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControlView.h : header file
//

#include "EffectToolDoc.h"
#include "ColorKeyControl.h"
#include "DrawGraph.h"
#include "OptTree.h"
/////////////////////////////////////////////////////////////////////////////
// CControlView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#define TYPE_INTEGRAL 0 
#define TYPE_SURFACE 1 
#define TYPE_SPLINE 2 
#define TYPE_LIGHT 3 

class CControlView : public CFormView
{
	friend class COptTree;
	CToolBar   m_wndToolBar;

	UINT  m_nTimelineUpdateTimer;
	int   m_nGraphNormalTop;

	void SetControlsData();
	void SaveControlsData();
	void AdjustColorControls();
	void AdjustEditControls();
	void ShowKeys();
	void EnableControls(bool bEnable);
	void EnableEmitterKeyControls();
protected:
	CControlView();
	DECLARE_DYNCREATE(CControlView)

public:
	friend class COptTree;
	int		m_nEmmiterType;
/*	bool	m_chVelocity_Mul;
	bool    m_chVelocityV;
	bool	m_chGrav;
	bool	m_chAngVel;
	bool	m_chSize;
	int		m_nVelType;
	int		m_nPoints;
	float	m_fTime;
	int		m_nParticles;
	bool	m_chNParticles;
	float	m_fSize;
	bool    m_chSizeDelta;
	float	m_fSizeDelta;
	bool	m_chLifeTimeDelta;
	float	m_fLifeTimeDelta;
	float	m_fGrav;
	float	m_fAnglVel;
	bool	m_chVelDelta;
	float	m_fVelDelta;
	float	m_fVelStart;
	float	m_fVelocity;
	bool	m_bPlayCycled;
	int		m_nSplineEnding;
	bool	m_bAngChaos;
	bool	m_bGenerateProlonged;
	bool	m_Angle_by_center;
	bool	m_Use_Force_Field;
	bool	m_Planar;
	float	m_Base_angle;
	float	m_Add_Z;
	bool	m_bUseLight;
	int m_fAlpha_min;
	int m_fAlpha_max;
	int m_fTeta_min;
	int m_fTeta_max;
	int m_Direction;
*/	
	//{{AFX_DATA(CControlView)
	enum { IDD = IDD_FORMVIEW };
	CDrawGraph	m_graph;
	CSliderCtrl	m_slider_time;
	CColorKeyControl m_wndAlpha;
	CColorKeyControl m_wndColors;
	float	m_CurTime;
	//}}AFX_DATA


	void SetPause();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CControlView();

	CEffectToolDoc* GetDocument(){
		return (CEffectToolDoc*)CFormView::GetDocument();
	}
	// Generated message map functions
	void OnCheckAngVel();
	void OnChangeEditVelStart();
	void OnCheckVelocity();
	void OnCheckPlayCycled();
	void OnSelchangeComboSplineEnding();
	void OnCheckAngChaos();
	void OnGenerateProlonged();
	void OnSelchangeComboEmmiterType();
	void OnCheckSurCenter();
	void OnCheckSurPlanar();
	void OnCheckUseForceField();
	void OnCheckGrav();
	void OnCheckSize();
	void OnChangeEditTime();
	void OnChangeEditNpoints(int n);
	void OnUselLight();
	//{{AFX_MSG(CControlView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangeComboVelType();
	afx_msg void OnButtonForall();
	//}}AFX_MSG
	LRESULT InitSlider(WPARAM w=0, LPARAM l=0);
	DECLARE_MESSAGE_MAP()
public:
	LRESULT OnChangeActivePoint(WPARAM, LPARAM);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLVIEW_H__9972C9E8_E7FF_436C_93CC_90A8403EF0D5__INCLUDED_)
