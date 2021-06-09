// aboutpage.cpp : implementation file
//

#include "stdafx.h"
#include "../Configurator.h"
#include "aboutpage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AboutPage property page


AboutPage::AboutPage() : BasePage(AboutPage::IDD)
{
	//{{AFX_DATA_INIT(AboutPage)
	text_ = _T("");
	//}}AFX_DATA_INIT
	AfxInitRichEdit();
}

AboutPage::~AboutPage()
{
}

void AboutPage::DoDataExchange(CDataExchange* pDX)
{
	BasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AboutPage)
	DDX_Text(pDX, IDC_EDIT_ABOUT, text_);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AboutPage, BasePage)
	//{{AFX_MSG_MAP(AboutPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AboutPage message handlers

BOOL AboutPage::OnInitDialog() 
{
	text_.LoadString(IDS_ABOUT_TEXT);	

	BasePage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AboutPage::reloadLanguage()
{

}

LPCTSTR AboutPage::getLocaleKeyPostfix() const{
	return _T("PROPPAGE_ABOUT");
}

