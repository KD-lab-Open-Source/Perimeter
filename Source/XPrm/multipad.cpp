// multipad.cpp : Defines the class behaviors for the Multipad application.
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

#include "stdafx.h"
#include "multipad.h"

CMultiPadApp NEAR theApp;

BEGIN_MESSAGE_MAP(CMultiPadApp, CWinApp)
	//{{AFX_MSG_MAP(CMultiPadApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CMultiPadApp::InitInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

//	Enable3dControls();
	LoadStdProfileSettings();

	AddDocTemplate(new CMultiDocTemplate(IDR_TEXTTYPE,
		RUNTIME_CLASS(CPadDoc), RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CEditView)));
	m_pMainWnd = new CMainFrame;
	((CFrameWnd*)m_pMainWnd)->LoadFrame(IDR_MAINFRAME);
	m_pMainWnd->ShowWindow(m_nCmdShow);
	currentMDIChildWnd = 0;

	// enable file manager drag/drop and DDE Execute open
	m_pMainWnd->DragAcceptFiles();
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	return TRUE;
}

void CMultiPadApp::OnAppExit() 
{
	if(!SaveAllModified())
		return;

	currentMDIChildWnd = ((CMDIFrameWnd*)m_pMainWnd) -> MDIGetActive();

	PostQuitMessage(0);
}

int CMultiPadApp::Run()
{
	StringList::iterator i;
	FOR_EACH(files, i)
		OpenDocumentFile(i -> c_str());

	if(currentMDIChildWnd)
		((CMDIFrameWnd*)m_pMainWnd) -> MDIActivate(currentMDIChildWnd);

	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->SetWindowPos( &CWnd::wndTopMost, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE );
	int log = CWinApp::Run();
#ifdef _DEBUG	
//	m_nDisablePumpCount = 0;
#endif
	HideApplication();
	return log;
}

void CMultiPadApp::OnAppAbout()
{
	CDialog(IDD_ABOUTBOX).DoModal();
}

void CMultiPadApp::OnFileNew() 
{
}

IMPLEMENT_DYNCREATE(CMainFrame, CMDIFrameWnd)
BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR, ID_INDICATOR_CAPS, ID_INDICATOR_NUM, ID_INDICATOR_SCRL
};

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	((m_StatusBar.Create(this) &&
		m_StatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	  ? 0 : -1);
	return CMDIFrameWnd::OnCreate(lpCreateStruct);
}

void CMainFrame::OnClose() 
{
	PostMessage(WM_COMMAND, ID_APP_EXIT);
}


IMPLEMENT_DYNCREATE(CPadDoc, CDocument)
BEGIN_MESSAGE_MAP(CPadDoc, CDocument)
	//{{AFX_MSG_MAP(CPadDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPadDoc::Serialize(CArchive& ar)
{
	((CEditView*)m_viewList.GetHead())->SerializeRaw(ar);
}

void CPadDoc::SetModifiedFlag(BOOL bModified)
{
	string title = GetTitle();
	if(title[title.size() - 1] == '*')
		title.erase(title.size() - 2, 2);
	if(bModified)
		title += " *";
	SetTitle(title.c_str());
	CDocument::SetModifiedFlag(bModified);
}

/////////////////////////////////////////////////////////////////////////////
extern "C" {
__declspec(dllexport) int edit_parameters(const StringList& files)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	theApp.setFilesList(files);
	return theApp.Run();
} 
}

/////////////////////////////////////////////////////////////////////////////
// ShowProfileDialog dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(ShowProfileDialog, CDialog)
BEGIN_MESSAGE_MAP(ShowProfileDialog, CDialog)
	//{{AFX_MSG_MAP(ShowProfileDialog)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL ShowProfileDialog::OnInitDialog()
{
	ShowWindow(SW_MAXIMIZE);
	SetDlgItemText(IDC_SHOW_PROFILE_EDIT, text_buffer);
	SetWindowPos( &CWnd::wndTopMost, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	GetDlgItem(IDC_SHOW_PROFILE_EDIT) -> ShowWindow(SW_MAXIMIZE);
	return CDialog::OnInitDialog();
}

void ShowProfileDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	CWnd* wnd = GetDlgItem(IDC_SHOW_PROFILE_EDIT);
	if(wnd)
		wnd -> SetWindowPos(0, 0, 0, cx, cy, SWP_SHOWWINDOW);
}

void ShowProfileDialog::OnOK()
{
	GetDlgItemText(IDC_SHOW_PROFILE_EDIT, text_buffer);
	fstream prof("profile", ios::app | ios::binary);
	prof << "\r\n\r\n=========================================================================================\r\n";
	time_t aclock;
	time( &aclock );                 /* Get time in seconds */
	prof << asctime( localtime( &aclock ) ) << endl;
	prof << text_buffer << endl; 
	prof << "=========================================================================================\r\n";
	CDialog::OnOK();
}

extern "C" {
__declspec(dllexport) void show_profile(const char* text)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	ShowProfileDialog(text).DoModal();
}
}

/////////////////////////////////////////////////////////////////////////////
// ShowTextDialog dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(ShowTextDialog, CDialog)
BEGIN_MESSAGE_MAP(ShowTextDialog, CDialog)
	//{{AFX_MSG_MAP(ShowTextDialog)
	ON_WM_SIZE()
	ON_COMMAND(ID_LOCK_WINDOW, OnLock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

ShowTextDialog::ShowTextDialog(int sx, int sy) 
: CDialog() 
{ 
	lock = 0;
	Create(IDD_DEBUG_WINDOW, 0); 
	SetWindowPos( &CWnd::wndTopMost, 0, 0, sx, sy, SWP_SHOWWINDOW);
	GetDlgItem(IDC_SHOW_PROFILE_EDIT) -> ShowWindow(SW_MAXIMIZE);
}

void ShowTextDialog::OnLock() 
{
	lock = 1 - lock;
}
	
void ShowTextDialog::setText(const char* text)
{
	if(lock)
		return;
	string s(text);
	int pos = 0;
	while(1){
		pos = s.find("\n", pos);
		if(pos >= s.size())
			break;
		if(pos && s[pos - 1] != '\r')
			s.insert(pos, "\r");
		pos += 2;
		}
	if(prev_text != s){
		SetDlgItemText(IDC_SHOW_PROFILE_EDIT, s.c_str());
		prev_text = s;
		RedrawWindow(0,0, RDW_UPDATENOW);
		}
}

void ShowTextDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	CWnd* wnd = GetDlgItem(IDC_SHOW_PROFILE_EDIT);
	if(wnd)
		wnd -> SetWindowPos(0, 0, 0, cx, cy, SWP_SHOWWINDOW);
}

ShowTextDialog* debug_window;

extern "C" {

__declspec(dllexport) void show_debug_window(const char* text, int sx, int sy)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(!debug_window)
		debug_window = new ShowTextDialog(sx, sy);
	debug_window -> setText(text);
	debug_window -> ShowWindow(SW_SHOWNORMAL);
}

__declspec(dllexport) void hide_debug_window()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(debug_window)
		debug_window -> ShowWindow(SW_HIDE);
}

}



