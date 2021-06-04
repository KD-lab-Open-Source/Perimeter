#if !defined(AFX_ABOUTPAGE_H__59B33A86_7728_4B43_9D1C_02FBB07DB46D__INCLUDED_)
#define AFX_ABOUTPAGE_H__59B33A86_7728_4B43_9D1C_02FBB07DB46D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// aboutpage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AboutPage dialog

#include "BasePage.h"

class AboutPage : public BasePage
{
// Construction
public:
	AboutPage();
	~AboutPage();

// Dialog Data
	//{{AFX_DATA(AboutPage)
	enum { IDD = IDD_PROPPAGE_ABOUT };
	CString	text_;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(AboutPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(AboutPage)
	virtual BOOL OnInitDialog();
	virtual void reloadLanguage();
	virtual LPCTSTR getLocaleKeyPostfix() const;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTPAGE_H__59B33A86_7728_4B43_9D1C_02FBB07DB46D__INCLUDED_)
