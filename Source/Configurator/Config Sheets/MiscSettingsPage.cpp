// MiscSettingsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MiscSettingsPage.h"
#include "Settings/LanguageSetting.h"

#include "Localisation/LocaleMngr.h"

#include "SettingsSheet.h"

#include <algorithm>
#include <functional>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//IMPLEMENT_DYNCREATE(MiscSettingsPage, BasePage)


/////////////////////////////////////////////////////////////////////////////
// MiscSettingsPage property page

MiscSettingsPage::MiscSettingsPage(ISettingFactory* factory) 
: BasePage(MiscSettingsPage::IDD)
, languageSetting_(NULL)
{
	//{{AFX_DATA_INIT(MiscSettingsPage)
	//}}AFX_DATA_INIT
	
	languageSetting_ = new LanguageSetting(IDC_COMBO_LANGUAGE);
	getSettingsList().push_back(languageSetting_);
//	getSettingsList().push_back(
//		settingFactory_->createLanguageSetting(IDC_COMBO_LANGUAGE));
	getSettingsList().push_back(
		factory->createMultiprocSetting(IDC_RADIO_MULTIPROC_NO));
}

MiscSettingsPage::~MiscSettingsPage()
{
}

void MiscSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	BasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MiscSettingsPage)
	DDX_Control(pDX, IDC_COMBO_LANGUAGE, languageWnd_);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MiscSettingsPage, BasePage)
	//{{AFX_MSG_MAP(MiscSettingsPage)
	ON_BN_CLICKED(IDC_RADIO_MULTIPROC_NO, onChangeInRadioGroup)
	ON_BN_CLICKED(IDC_RADIO_MULTIPROC_AUTO, onChangeInRadioGroup)
	ON_CBN_SELCHANGE(IDC_COMBO_LANGUAGE, onChangeInCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL MiscSettingsPage::OnApply() 
{
	SettingsSheet* parent = reinterpret_cast<SettingsSheet*>(GetParent());
	parent->onChangeLanguage(languageSetting_->getValue());
	return BasePage::OnApply();
}

BOOL MiscSettingsPage::OnInitDialog() 
{
	BasePage::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MiscSettingsPage::reloadLanguage()
{
	reloadCotrolCaption(IDC_STATIC_LANGUAGE, _T("STATIC_LANGUAGE"));
	reloadCotrolCaption(IDC_STATIC_HT, _T("STATIC_HT"));
	reloadCotrolCaption(IDC_RADIO_MULTIPROC_NO, _T("RADIO_MULTIPROC_NO"));
	reloadCotrolCaption(IDC_RADIO_MULTIPROC_AUTO, _T("RADIO_MULTIPROC_AUTO"));

	initControls();
	if (::IsWindow(m_hWnd))
		UpdateData(FALSE);
}

LPCTSTR MiscSettingsPage::getLocaleKeyPostfix() const{
	return _T("PROPPAGE_MISC");
}

