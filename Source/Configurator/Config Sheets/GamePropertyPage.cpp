// GamePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "configurator.h"
#include "GamePropertyPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GamePropertyPage property page

GamePropertyPage::GamePropertyPage(ISettingFactory* factory) 
: BasePage(GamePropertyPage::IDD)
{
	//{{AFX_DATA_INIT(GamePropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	getSettingsList().push_back(
		factory->createMouseLookRateSetting(IDC_SLIDER_MOUSE_LOOK_RATE));
	getSettingsList().push_back(
		factory->createScrollRateSetting(IDC_SLIDER_SCROLL_RATE));

	getSettingsList().push_back(
		factory->createCheckSetting(
						IDC_CHECK_TOOL_TIPS,
						IDS_SECTION_GAME,
						IDS_KEY_TOOL_TIPS));
}

GamePropertyPage::~GamePropertyPage()
{
}

void GamePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	BasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GamePropertyPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GamePropertyPage, BasePage)
	//{{AFX_MSG_MAP(GamePropertyPage)
	ON_BN_CLICKED(IDC_CHECK_TOOL_TIPS, onClickInCheck)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GamePropertyPage message handlers


void GamePropertyPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch(pScrollBar->GetDlgCtrlID()) {
	case IDC_SLIDER_MOUSE_LOOK_RATE:
	case IDC_SLIDER_SCROLL_RATE:
		{
			if (nSBCode == TB_ENDTRACK)
				SetModified();
		}
	}	
	BasePage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void GamePropertyPage::reloadLanguage()
{
	reloadCotrolCaption(IDC_STATIC_SCROLL_RATE, _T("STATIC_SCROLL_RATE"));
	reloadCotrolCaption(IDC_STATIC_MOUSE_LOOK_RATE, _T("STATIC_MOUSE_LOOK_RATE"));
	reloadCotrolCaption(IDC_CHECK_TOOL_TIPS, _T("CHECK_TOOL_TIPS"));

	initControls();
	if (::IsWindow(m_hWnd))
		UpdateData(FALSE);
}


LPCTSTR GamePropertyPage::getLocaleKeyPostfix() const{
	return _T("PROPPAGE_GAME");
}
