#if !defined(AFX_DLGBAR_H__A2450C71_70EA_4AB5_AE00_89CBEBEA1AF5__INCLUDED_)
#define AFX_DLGBAR_H__A2450C71_70EA_4AB5_AE00_89CBEBEA1AF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBar dialog

class CDlgBar : public CDialog
{
	bool  m_bInit;

public:
	CDlgBar(CWnd* pParent = NULL);   // standard constructor

	void SetFps(float fps);
// Dialog Data
	//{{AFX_DATA(CDlgBar)
	enum { IDD = IDR_MAINFRAME };
	CStatic	m_fps;
	CComboBox	m_cbType;
	CComboBox	m_cbOut;
	//}}AFX_DATA


	virtual void OnCancel(){}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckShowWorld();
	//}}AFX_MSG
	LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBAR_H__A2450C71_70EA_4AB5_AE00_89CBEBEA1AF5__INCLUDED_)
