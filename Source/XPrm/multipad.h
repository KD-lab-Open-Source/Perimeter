// multipad.h : main header file for the Multipad application
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#include <afxext.h>
#include "resource.h"

typedef list<string> StringList;

class CMultiPadApp : public CWinApp
{
	StringList files;
	
	CMDIChildWnd* currentMDIChildWnd;
	
	BOOL InitInstance();
	//{{AFX_MSG(CMultiPadApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppExit();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int Run();
	void setFilesList(const StringList& files_) { files = files_; }
};

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNCREATE(CMainFrame)
	CStatusBar  m_StatusBar;
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CPadDoc : public CDocument
{
	DECLARE_DYNCREATE(CPadDoc)
	void Serialize(CArchive& ar);
	//{{AFX_MSG(CPadDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void SetModifiedFlag(BOOL bModified = TRUE);
};


/////////////////////////////////////////////////////////////////////////////
//	Dialogs
/////////////////////////////////////////////////////////////////////////////
class ShowTextDialog : public CDialog
{
	DECLARE_DYNCREATE(ShowTextDialog)
	//{{AFX_MSG(ShowTextDialog)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLock();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int lock;
	string prev_text;

public:
	ShowTextDialog(int sx = 100, int sy = 100);
	void setText(const char* text);
};

class ShowProfileDialog : public CDialog
{
	DECLARE_DYNCREATE(ShowProfileDialog)
	//{{AFX_MSG(ShowProfileDialog)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	BOOL OnInitDialog();
	void OnOK();

public:
	ShowProfileDialog(const char* text = "") : CDialog(IDD_SHOW_PROFILE, 0), text_buffer(text) {}

protected:
	CString text_buffer;
};

