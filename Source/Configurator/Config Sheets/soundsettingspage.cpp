// soundsettingspage.cpp : implementation file
//

#include "stdafx.h"
#include "../Configurator.h"
#include "soundsettingspage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SoundSettingsPage property page

SoundSettingsPage::SoundSettingsPage(ISettingFactory* factory) 
: BasePage(SoundSettingsPage::IDD)
{
	//{{AFX_DATA_INIT(SoundSettingsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	getSettingsList().push_back(
		factory->createCheckSetting(IDC_CHECK_SOUND_EFFECTS,
									IDS_SECTION_SOUND,
									IDS_KEY_SOUNDS_EFFECT));
	getSettingsList().push_back(
		factory->createSliderSetting(IDC_SLIDER_SOUND_VOLUME,
									IDS_SECTION_SOUND,
									IDS_KEY_SOUND_VOLUME));
	getSettingsList().push_back(
		factory->createCheckSetting(IDC_CHECK_MUSIC,
									IDS_SECTION_SOUND,
									IDS_KEY_MUSIC));
	getSettingsList().push_back(
		factory->createSliderSetting(IDC_SLIDER_MUSIC_VOLUME,
									IDS_SECTION_SOUND,
									IDS_KEY_MUSIC_VOLUME));
}

SoundSettingsPage::~SoundSettingsPage()
{
}

void SoundSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	BasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SoundSettingsPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SoundSettingsPage, BasePage)
	//{{AFX_MSG_MAP(SoundSettingsPage)
	ON_BN_CLICKED(IDC_CHECK_MUSIC, onClickInCheck)
	ON_BN_CLICKED(IDC_CHECK_SOUND_EFFECTS, onClickInCheck)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SoundSettingsPage message handlers


void SoundSettingsPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch(pScrollBar->GetDlgCtrlID()) {
	case IDC_SLIDER_MUSIC_VOLUME:
	case IDC_SLIDER_SOUND_VOLUME:
		{
			if (nSBCode == TB_ENDTRACK)
				SetModified();
		}
	}
	BasePage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void SoundSettingsPage::reloadLanguage()
{
	reloadCotrolCaption(IDC_STATIC_SOUND_VOLUME, _T("STATIC_SOUND_VOLUME"));
	reloadCotrolCaption(IDC_STATIC_MUSIC_VOLUME, _T("STATIC_MUSIC_VOLUME"));
	reloadCotrolCaption(IDC_CHECK_MUSIC, _T("CHECK_MUSIC"));
	reloadCotrolCaption(IDC_CHECK_SOUND_EFFECTS, _T("CHECK_SOUND_EFFECTS"));

	initControls();
	if (::IsWindow(m_hWnd))
		UpdateData(FALSE);
}

LPCTSTR SoundSettingsPage::getLocaleKeyPostfix() const{
	return _T("PROPPAGE_SOUND");
}
