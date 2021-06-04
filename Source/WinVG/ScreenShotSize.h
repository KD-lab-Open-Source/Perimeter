#if !defined(AFX_SCREENSHOTSIZE_H__01228B3D_0742_4567_BD26_21A1890EE9F0__INCLUDED_)
#define AFX_SCREENSHOTSIZE_H__01228B3D_0742_4567_BD26_21A1890EE9F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScreenShotSize.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScreenShotSize dialog

class CScreenShotSize : public CDialog
{
// Construction
public:
	CScreenShotSize(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScreenShotSize)
	enum { IDD = IDD_SCREENSHOT };
	int		x;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScreenShotSize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScreenShotSize)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCREENSHOTSIZE_H__01228B3D_0742_4567_BD26_21A1890EE9F0__INCLUDED_)
