// BasePage.cpp: implementation of the BasePage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "BasePage.h"

#include "Localisation/LocaleMngr.h"

#include <algorithm>
#include <functional>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BasePage::BasePage(UINT nIDTemplate, UINT nIDCaption)
: CPropertyPage(nIDTemplate, nIDCaption)
{

}

BasePage::SettingsList& BasePage::getSettingsList(){
	return settingsList_;
}

BasePage::~BasePage()
{
	std::list<ISetting*>::iterator b = settingsList_.begin(),
										e = settingsList_.end();
	for(; b != e; ++b)
		delete *b;
}

void BasePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	std::list<ISetting*>::iterator b = settingsList_.begin(),
										e = settingsList_.end();
	for(; b != e; ++b)
		(*b)->DDX_Exchange(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// BasePage message handlers

BOOL BasePage::OnApply() 
{
	if (!UpdateData())
		return FALSE;

	std::for_each(settingsList_.begin(), 
		settingsList_.end(),
		std::mem_fun(ISetting::saveToStorage));
	
	return CPropertyPage::OnApply();
}

BOOL BasePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	initSettings();
	reloadLanguage();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void BasePage::initSettings()
{
	initControls();
	std::for_each(settingsList_.begin(), settingsList_.end(),
		std::mem_fun(ISetting::readFromStorage));

}

void BasePage::onChangeInCombo(){
	SetModified();
}

void BasePage::onChangeInRadioGroup()
{
	SetModified();
}

void BasePage::onClickInCheck()
{
	SetModified();
}

CString BasePage::makeComboItemKey(LPCTSTR item)
{
	CString prefix;
	prefix.LoadString(IDS_COMBO_ITEMS_TDB_PREFIX);
					  
	prefix += item;
	return prefix;
}

CString BasePage::makeControlKey(LPCTSTR item)
{
	CString prefix;
	prefix.LoadString(IDS_CONTROLS_TDB_PREFIX);
	prefix += item;
	return prefix;
}

void BasePage::reloadCotrolCaption(UINT ctrlId, LPCTSTR ctrlPostfix)
{
	LocaleMngr const& mngr = LocaleMngr::instance();
	CString caption = mngr.getText(mngr.makeControlKey(ctrlPostfix));
	if (::IsWindow(m_hWnd))
	{
		CWnd* pwnd = GetDlgItem(ctrlId);
		if (pwnd) 
			pwnd->SetWindowText(caption);
	}
}

void BasePage::initControls()
{
	if (!::IsWindow(m_hWnd))
		return;
	std::list<ISetting*>::iterator b = settingsList_.begin(),
										e = settingsList_.end();
	for(; b != e; ++b)
		(*b)->initControl(this, LocaleMngr::instance());
}
