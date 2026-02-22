#if !defined(AFX_COLORKEYCONTROL_H__4596F7B9_C8BB_489A_89E3_EDF8BD4AC5AB__INCLUDED_)
#define AFX_COLORKEYCONTROL_H__4596F7B9_C8BB_489A_89E3_EDF8BD4AC5AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorKeyControl.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CColorKeyControl window

class CColorKeyControl : public CWnd
{
	CKeyColor*	m_pKey;
	KeyColor*	m_pKeyTrack;

	bool		m_bAlpha;

	float x2t(int x);
	int   t2x(float f);

	void DeleteKeyAtPos(int x);
	KeyColor* FindKeyAtPos(int x);
	KeyColor* AddNewKey(int x);
public:
	bool		m_Valid;

	CColorKeyControl();
	virtual ~CColorKeyControl();

	void SetKey(CKeyColor* key, bool bAlpha);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorKeyControl)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CColorKeyControl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORKEYCONTROL_H__4596F7B9_C8BB_489A_89E3_EDF8BD4AC5AB__INCLUDED_)
