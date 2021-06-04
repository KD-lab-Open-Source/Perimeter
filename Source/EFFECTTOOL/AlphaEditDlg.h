#if !defined(AFX_ALPHAEDITDLG_H__82124BA3_96D0_4381_BBFC_1FB28FA24D95__INCLUDED_)
#define AFX_ALPHAEDITDLG_H__82124BA3_96D0_4381_BBFC_1FB28FA24D95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlphaEditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAlphaEditDlg dialog

class CAlphaEditDlg : public CDialog
{
	CBrush   m_brSample;
	bool     m_bLockUpdate;

	void InitBrush();
public:
	CAlphaEditDlg(CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CAlphaEditDlg)
	enum { IDD = IDD_ALPHA };
	CSliderCtrl	m_slider;
	//}}AFX_DATA

	BYTE   m_alpha;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlphaEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlphaEditDlg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALPHAEDITDLG_H__82124BA3_96D0_4381_BBFC_1FB28FA24D95__INCLUDED_)
