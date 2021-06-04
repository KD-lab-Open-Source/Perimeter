// SettingsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "SettingsSheet.h"
#include "SettingFactory/SettingFactory.h"
#include "Settings/LanguageSetting.h"

#include "Localisation/LocaleMngr.h"

#include "IniReader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SettingsSheet

IMPLEMENT_DYNAMIC(SettingsSheet, CPropertySheet)

SettingsSheet::SettingsSheet(CWnd* pWndParent)
: CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
, miscSettingsPage_(SettingFactory::instance())
, graphSettingsPage_(SettingFactory::instance())
, soundSettingsPage_(SettingFactory::instance())
, gamePropertyPage_(SettingFactory::instance())
{
	m_psh.dwFlags						&= ~PSH_HASHELP;
	miscSettingsPage_.m_psp.dwFlags		&= ~PSP_HASHELP;
	graphSettingsPage_.m_psp.dwFlags	&= ~PSP_HASHELP;
	soundSettingsPage_.m_psp.dwFlags	&= ~PSP_HASHELP;
	gamePropertyPage_.m_psp.dwFlags		&= ~PSP_HASHELP;
	aboutPage_.m_psp.dwFlags			&= ~PSP_HASHELP;

	loadLanguage();

	AddPage(&miscSettingsPage_);
	AddPage(&graphSettingsPage_);
	AddPage(&soundSettingsPage_);
	AddPage(&gamePropertyPage_);
	AddPage(&aboutPage_);
}

SettingsSheet::~SettingsSheet()
{
}


BEGIN_MESSAGE_MAP(SettingsSheet, CPropertySheet)
	//{{AFX_MSG_MAP(SettingsSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SettingsSheet message handlers



void SettingsSheet::onChangeLanguage(CString const& newLanguageName)
{

	if (!LocaleMngr::instance().reloadLocaleData(newLanguageName))
		return;

	int const pageCount = GetPageCount();
	for(int i = 0; i < pageCount; ++i)
		static_cast<BasePage*>(GetPage(i))->reloadLanguage();
	reloadCaptions();
}

void SettingsSheet::loadLanguage()
{
	LanguageSetting setting((UINT)-1);
	setting.readFromStorage();

	LocaleMngr::instance().reloadLocaleData(setting.getValue());
}

void SettingsSheet::reloadCaptions()
{
	LocaleMngr const& mngr = LocaleMngr::instance();
	CTabCtrl* tab = GetTabControl();
	int const pageCount = GetPageCount();
	for(int i = 0; i < pageCount; ++i)
	{
		CString cap = mngr.getText(mngr.makeControlKey(
			static_cast<BasePage*>(GetPage(i))->getLocaleKeyPostfix()));
		TCITEM item;
		item.mask = TCIF_TEXT;
		item.pszText = const_cast<LPTSTR>((LPCTSTR)cap);
		item.cchTextMax = cap.GetLength();

		tab->SetItem(i, &item);

	}
}

BOOL SettingsSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	int const pageCount = GetPageCount();
	for(int i = 0; i < pageCount; ++i)
		SetActivePage(i);
	SetActivePage(0);
	reloadCaptions();
	return bResult;
}

