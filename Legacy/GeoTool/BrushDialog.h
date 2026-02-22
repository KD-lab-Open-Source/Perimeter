#if !defined(AFX_BRUSHDIALOG_H__EA78A87C_3B63_4B77_8691_F25F27DEC9D6__INCLUDED_)
#define AFX_BRUSHDIALOG_H__EA78A87C_3B63_4B77_8691_F25F27DEC9D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BrushDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBrushDialog dialog

class CBrushDialog : public CDialog
{
// Construction
public:
	CBrushDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBrushDialog)
	enum { IDD = IDD_BRUSHES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrushDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBrushDialog)
	afx_msg void OnPitBrush();
	afx_msg void OnPitBrushBig();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRUSHDIALOG_H__EA78A87C_3B63_4B77_8691_F25F27DEC9D6__INCLUDED_)
