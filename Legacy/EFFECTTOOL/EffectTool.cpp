// EffectTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "EffectTool.h"

#include "MainFrm.h"
#include "EffectToolDoc.h"
#include "EffectToolView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectToolApp

BEGIN_MESSAGE_MAP(CEffectToolApp, CWinApp)
	//{{AFX_MSG_MAP(CEffectToolApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standart
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectToolApp construction

CEffectToolApp::CEffectToolApp()
{
	bActive=true;
}

CEffectToolApp::~CEffectToolApp()
{
	scene.SaveSceneSettings();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEffectToolApp object

CEffectToolApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CEffectToolApp initialization

BOOL CEffectToolApp::InitInstance()
{
	AfxEnableControlContainer();
	initclock();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("K-D LAB"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	scene.LoadSceneSettings();

	CheckCreateEmtrDir();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CEffectToolDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CEffectToolView));
	AddDocTemplate(pDocTemplate);

	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();

	_pDoc->UpdateAllViews(0);

	return TRUE;
}

static LPCTSTR szEmittersDir = "Emitters";

void CEffectToolApp::CheckCreateEmtrDir()
{
	char _cb[MAX_PATH];
	GetModuleFileName(0, _cb, MAX_PATH);

	m_szDir = _cb;
	int n = m_szDir.ReverseFind('\\');
	m_szDir = m_szDir.Left(n+1);

	m_szDirEmitters = m_szDir + szEmittersDir;
	m_szDirEmitters += "\\";

	if(GetFileAttributes(m_szDirEmitters) == 0xFFFFFFFF)
	{
		_mkdir(m_szDirEmitters);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CEffectToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CEffectToolApp::OnIdle(LONG lCount) 
{
	if(bActive)
		scene.Quant();
	
	return CWinApp::OnIdle(lCount);
}

int CEffectToolApp::ExitInstance() 
{
	scene.DoneRenderDevice();
	
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////


