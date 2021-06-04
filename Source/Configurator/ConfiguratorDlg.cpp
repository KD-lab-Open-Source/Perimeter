// ConfiguratorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Configurator.h"
#include "ConfiguratorDlg.h"
#include "CollectLangDirs.h"

#include <atlbase.h>

#include <boost/tokenizer.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TCHAR const GENERAL_DELIMETER[] = _T(",");
TCHAR const DEFAULT_SCR_X_SIZE [] = _T("800");
TCHAR const DEFAULT_SCR_Y_SIZE [] = _T("600");
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfiguratorDlg dialog

CConfiguratorDlg::CConfiguratorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfiguratorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfiguratorDlg)
	multiProc_ = -1;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CConfiguratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfiguratorDlg)
	DDX_Control(pDX, IDC_COMBO_RESOULUTION, resolutionWnd_);
	DDX_Control(pDX, IDC_COMBO_LANGUAGE, languageWnd_);
	DDX_Radio(pDX, IDC_RADIO_MULTIPROC_NO, multiProc_);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfiguratorDlg, CDialog)
	//{{AFX_MSG_MAP(CConfiguratorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfiguratorDlg message handlers

BOOL CConfiguratorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	loadLanguages();
	loadResolutions();
	readCurrentLanguage();
	readCurrentResolution();
	readMultiproc();

	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CConfiguratorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CConfiguratorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CConfiguratorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CConfiguratorDlg::loadLanguages()
{
	CString strIniSection;
	strIniSection.LoadString(IDS_MAIN_INI_MAIN_SECTION);
	CString strLocDir;
	strLocDir.LoadString(IDS_MAIN_INI_VALUE_LOCDIR);

	CString buffer;
	DWORD res = GetPrivateProfileString(strIniSection, strLocDir, 
		_T("Resource\\LocData"), 
		buffer.GetBufferSetLength(INI_READ_BUFFER), 
		INI_READ_BUFFER,
		mainIniFileName);
	buffer.ReleaseBuffer();
	std::list<std::basic_string<TCHAR> > dirs;
	CollectLangDirs collector;
	collector.collect(dirs, buffer);
	std::list<std::basic_string<TCHAR> >::const_iterator b = dirs.begin(), 
														e = dirs.end();
	for(;b != e;++b)
		languageWnd_.AddString((*b).c_str());
}

void CConfiguratorDlg::loadResolutions()
{
	CString strDefault;
	strDefault.LoadString(IDS_RESOLUTIONS);

	CString strIniSection;
	strIniSection.LoadString(IDS_MAIN_INI_MAIN_SECTION);
	CString strIniLanguages;
	strIniLanguages.LoadString(IDS_MAIN_INI_VALUE_RESOLUTIONS);

	CString buffer;
	DWORD res = GetPrivateProfileString(strIniSection, strIniLanguages, 
		strDefault, buffer.GetBufferSetLength(INI_READ_BUFFER), INI_READ_BUFFER,
		mainIniFileName);
	buffer.ReleaseBuffer();

	boost::char_separator<TCHAR> sep(GENERAL_DELIMETER);
	typedef boost::tokenizer<boost::char_separator<TCHAR>, 
							LPCTSTR,
							std::basic_string<TCHAR> > tokenizer;
	LPCTSTR const begin(buffer);
	LPCTSTR const end(begin + buffer.GetLength());

	tokenizer toks(begin, end, sep);
	tokenizer::const_iterator b = toks.begin(), e = toks.end();
	for(;b != e;++b)
		resolutionWnd_.AddString((*b).c_str());
}

CString CConfiguratorDlg::readLanguageValue()
{
	DWORD BUFFER_LEN = 128;

	CRegKey languageKey;
	if (languageKey.Open(HKEY_CURRENT_USER,
		_T("SOFTWARE\\1C\\PerimeterET\\Intf"),KEY_READ) != ERROR_SUCCESS)
		return CString();
	CString valueName;
	valueName.LoadString(IDS_VALUE_LANGUAGE);
	CString buffer;
	DWORD dwCount = BUFFER_LEN;
	if (languageKey.QueryValue(buffer.GetBufferSetLength(BUFFER_LEN),
								valueName, &dwCount) != ERROR_SUCCESS)
		return CString();

	buffer.ReleaseBuffer(dwCount);
	return buffer;
}

void CConfiguratorDlg::readCurrentLanguage()
{
	CString language = readLanguageValue();
	if (language.IsEmpty())
		language.LoadString(IDS_LANGUAGE_DEFAULT);
	languageWnd_.SelectString(-1, language);
}

void CConfiguratorDlg::readCurrentResolution()
{
	TCHAR buffer[32];
	CString section;
	section.LoadString(IDS_SECTION_GRAPHICS);
	CString key;
	key.LoadString(IDS_KEY_SCR_SIZE_X);
	GetPrivateProfileString(section, key, DEFAULT_SCR_X_SIZE, buffer, 32, iniFileName);
	CString resolution(buffer);
	resolution += _T("x");
	key.LoadString(IDS_KEY_SCR_SIZE_Y);
	GetPrivateProfileString(section, key, DEFAULT_SCR_Y_SIZE, buffer, 32, iniFileName);
	resolution += buffer;
	resolutionWnd_.SelectString(-1, resolution);
}

void CConfiguratorDlg::readMultiproc()
{
	CString section;
	section.LoadString(IDS_SECTION_GAME);
	CString key;
	key.LoadString(IDS_KEY_MULTIPROC);
	multiProc_ = GetPrivateProfileInt(section, key, 0, iniFileName);
}

void CConfiguratorDlg::saveOptions()
{
	saveMultiproc();
	saveCurrentLanguage();
	saveResolution();
}

void CConfiguratorDlg::saveCurrentLanguage()
{
	CString language;
	languageWnd_.GetLBText(languageWnd_.GetCurSel(), language);
	writeLanguageValue(language);
}

void CConfiguratorDlg::writeLanguageValue(CString const& language)
{
	DWORD BUFFER_LEN = 128;

	CRegKey languageKey;
	if (languageKey.Create(HKEY_CURRENT_USER,
		_T("SOFTWARE\\1C\\PerimeterET\\Intf")) != ERROR_SUCCESS)
		return ;

	CString valueName;
	valueName.LoadString(IDS_VALUE_LANGUAGE);
	CString buffer;
	DWORD dwCount = 0;
	if (languageKey.SetValue(language, valueName) != ERROR_SUCCESS)
		return ;

}

void CConfiguratorDlg::writeMultiproc(int value)
{
	CString section;
	section.LoadString(IDS_SECTION_GAME);
	CString key;
	key.LoadString(IDS_KEY_MULTIPROC);
	CString strValue;
	strValue.Format(_T("%d"), value);
	WritePrivateProfileString(section, key, strValue, iniFileName);
}

void CConfiguratorDlg::saveMultiproc()
{
	writeMultiproc(multiProc_);
}

void CConfiguratorDlg::saveResolution()
{
	CString resolution;
	resolutionWnd_.GetLBText(resolutionWnd_.GetCurSel(), resolution);

	int delimPos = resolution.Find(_T("x"));
	CString sizeX = resolution.Left(delimPos);
	CString sizeY = resolution.Mid(delimPos + 1);

	CString section;
	section.LoadString(IDS_SECTION_GRAPHICS);

	CString key;
	key.LoadString(IDS_KEY_SCR_SIZE_X);
	WritePrivateProfileString(section, key, sizeX, iniFileName);

	key.LoadString(IDS_KEY_SCR_SIZE_Y);
	WritePrivateProfileString(section, key, sizeY, iniFileName);
}


void CConfiguratorDlg::OnOK() 
{
	UpdateData();
	saveOptions();
	CDialog::OnOK();
}
