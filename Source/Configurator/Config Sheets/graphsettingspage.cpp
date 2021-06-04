// graphsettingspage.cpp : implementation file
//

#include "stdafx.h"
#include "..\configurator.h"
#include "graphsettingspage.h"

#include "Localisation/LocaleMngr.h"

#include "GrCustomSettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GraphSettingsPage property page

//IMPLEMENT_DYNCREATE(GraphSettingsPage, BasePage)

const int FAST_GRAPH_SET = 0;
const int PERFORMANCE_GRAPH_SET = 1;
const int MAX_GRAPH_SET = 2;
//const int EXTREMALE_GRAPH_SET = 3;
const int CUSTOM_GRAPH_SET = 3;

GraphSettingsPage::GraphSettingsPage(ISettingFactory* factory) 
: BasePage(GraphSettingsPage::IDD)
, currentGraphSet_(IDS_SECTION_GRAPHICS)
, fastGraphSet_(IDS_SECTION_FAST_GRAPHICS)
, performanceGraphSet_(IDS_SECTION_PERFORMANCE_GRAPHICS)
, maxGraphSet_(IDS_SECTION_MAX_GRAPHICS)
//, extremaleGraphSet_(IDS_SECTION_EXTREMALE_GRAPHICS)
{
	//{{AFX_DATA_INIT(GraphSettingsPage)
	//}}AFX_DATA_INIT
	getSettingsList().push_back(
		factory->createResolutionSetting(IDC_COMBO_RESOULUTION));
	getSettingsList().push_back(
		factory->createColorDepthSetting(IDC_RADIO_COLOR_DEPTH_16));
	getSettingsList().push_back(&currentGraphSet_);

	fastGraphSet_.readFromStorage();
	performanceGraphSet_.readFromStorage();
	maxGraphSet_.readFromStorage();
//	extremaleGraphSet_.readFromStorage();
}

GraphSettingsPage::~GraphSettingsPage()
{
	SettingsList& lst = getSettingsList();
	lst.erase(std::find(lst.begin(), lst.end(), &currentGraphSet_));
}

void GraphSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	BasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GraphSettingsPage)
	DDX_Control(pDX, IDC_COMBO_PRESET, presetWnd_);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GraphSettingsPage, BasePage)
	//{{AFX_MSG_MAP(GraphSettingsPage)
	ON_CBN_SELCHANGE(IDC_COMBO_PRESET, OnSelchangeComboPreset)
	ON_CBN_SELCHANGE(IDC_COMBO_RESOULUTION, onChangeInCombo)
	ON_BN_CLICKED(IDC_RADIO_COLOR_DEPTH_16, onChangeInRadioGroup)
	ON_BN_CLICKED(IDC_RADIO_COLOR_DEPTH_32, onChangeInRadioGroup)
	ON_BN_CLICKED(IDC_BUTTON_CUSTOM_GRAPH, OnButtonCustomGraph)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GraphSettingsPage message handlers

void GraphSettingsPage::loadPresetList(LocaleMngr const& mngr)
{
	TCHAR const* const values[] = {
		_T("Fast"),
		_T("Performance"),
		_T("Max"),
//		_T("Extreme"),
		_T("Custom")
	};
	const size_t valuesCount = sizeof(values)/sizeof(TCHAR*);
	if (!::IsWindow(presetWnd_.m_hWnd))
		return;
	presetWnd_.ResetContent();
	for(int i = 0; i < valuesCount; ++i)
		presetWnd_.AddString(mngr.getText(makeComboItemKey(values[i])));

}

BOOL GraphSettingsPage::OnInitDialog() 
{
	BasePage::OnInitDialog();
	
	loadPresetList(LocaleMngr::instance());
	
	presetWnd_.SetCurSel(comparePresets());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LPCTSTR GraphSettingsPage::getLocaleKeyPostfix() const{
	return _T("PROPPAGE_GRAPH");
}

void GraphSettingsPage::reloadLanguage()
{
	reloadCotrolCaption(IDC_STATIC_RESOLUTION, _T("STATIC_RESOLUTION"));
	reloadCotrolCaption(IDC_STATIC_PRESET, _T("STATIC_PRESET"));
	reloadCotrolCaption(IDC_STATIC_COLOR_DEPTH, _T("STATIC_COLOR_DEPTH"));

	loadPresetList(LocaleMngr::instance());
	initControls();
	presetWnd_.SetCurSel(comparePresets());
	if (::IsWindow(m_hWnd))
		UpdateData(FALSE);
}

//! Сравнивает пресеты с текущим и возвращает константу для combobox'a
int GraphSettingsPage::comparePresets() const{
	if (currentGraphSet_ == fastGraphSet_)
		return FAST_GRAPH_SET;

	if (currentGraphSet_ == performanceGraphSet_)
		return PERFORMANCE_GRAPH_SET;

	if (currentGraphSet_ == maxGraphSet_)
		return MAX_GRAPH_SET;

//	if (currentGraphSet_ == extremaleGraphSet_)
//		return EXTREMALE_GRAPH_SET;

	return CUSTOM_GRAPH_SET;
}

void GraphSettingsPage::OnSelchangeComboPreset() 
{
	SetModified();
	int const curSel = presetWnd_.GetCurSel();
	switch(curSel) {
	case CB_ERR:
		break;
	case CUSTOM_GRAPH_SET:
		{
			GrCustomSettingsDlg dlg;
			dlg.setGraphicSet(currentGraphSet_);
			if (dlg.DoModal() == IDOK)
			{
				currentGraphSet_.copySettings(dlg.getGraphicSet());
			}
		}
		break;
	default:
		copyPreset(curSel);
	}
}

void GraphSettingsPage::copyPreset(int presetNum)
{
	switch(presetNum) {
	case FAST_GRAPH_SET:
		currentGraphSet_.copySettings(fastGraphSet_);
		break;
	case PERFORMANCE_GRAPH_SET:
		currentGraphSet_.copySettings(performanceGraphSet_);
		break;
	case MAX_GRAPH_SET:
		currentGraphSet_.copySettings(maxGraphSet_);
		break;
//	case EXTREMALE_GRAPH_SET:
//		currentGraphSet_.copySettings(extremaleGraphSet_);
//		break;
	}
}

void GraphSettingsPage::OnButtonCustomGraph() 
{
	GrCustomSettingsDlg dlg;
	dlg.setGraphicSet(currentGraphSet_);
	if (dlg.DoModal() == IDOK)
	{
		currentGraphSet_.copySettings(dlg.getGraphicSet());
		presetWnd_.SetCurSel(CUSTOM_GRAPH_SET);
	}
}
