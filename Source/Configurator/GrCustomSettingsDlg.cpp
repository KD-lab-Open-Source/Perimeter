// GrCustomSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "configurator.h"
#include "GrCustomSettingsDlg.h"
#include "Localisation/LocaleMngr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GrCustomSettingsDlg dialog
GrCustomSettingsDlg::GrCustomSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GrCustomSettingsDlg::IDD, pParent)
	, graphSettingsSet_(UINT(-1)
	, IDC_CHECK_REFLECTION					//	 UINT reflectionCtrlId,      
	, IDC_CHECK_OCCLUSION					//	 UINT occlustionCtrlId,      
	, IDC_CHECK_POINT_LIGHT					//	 UINT pointLightCtrlId,      
	, IDC_CHECK_BUMP_MAPPING				//	 UINT bumpMappingCtrlId,     
	, IDC_CHECK_BUMP_CHAOS					//	 UINT bumpChaosCtrlId,       
	, IDC_SLIDER_PARTICLE_RATE				//	 UINT particleRateCtrlId,    
	, IDC_CHECK_FAVORITE_LOAD_DDS			//	 UINT compressTexturesCtrlId,
	, IDC_RADIO_SHADOW_TYPE1				//	 UINT shadowTypeCtrlId,      
	, IDC_RADIO_SHADOW_SAMPLES1				//	 UINT shadowSamplesCtrlId,   
	, IDC_COMBO_MAP_LEVEL_LOD				//	 UINT mapLevelLODCtrlId,     
	, IDC_COMBO_SHADOW_QUALITY)				//	 UINT shadowQualityCtrlId    
{
	//{{AFX_DATA_INIT(GrCustomSettingsDlg)
	//}}AFX_DATA_INIT
}

void GrCustomSettingsDlg::setGraphicSet(GraphSettingsSet const& set){
	graphSettingsSet_.copySettings(set);
}
GraphSettingsSet const& GrCustomSettingsDlg::getGraphicSet() const{
	return graphSettingsSet_;
}

void GrCustomSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GrCustomSettingsDlg)
	//}}AFX_DATA_MAP
	graphSettingsSet_.DDX_Exchange(pDX);
}


BEGIN_MESSAGE_MAP(GrCustomSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(GrCustomSettingsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GrCustomSettingsDlg message handlers

void GrCustomSettingsDlg::reloadCotrolCaption(UINT ctrlId, LPCTSTR ctrlPostfix)
{
	LocaleMngr const& mngr = LocaleMngr::instance();
	CString caption = mngr.getText(mngr.makeControlKey(ctrlPostfix));
	GetDlgItem(ctrlId)->SetWindowText(caption);
}

BOOL GrCustomSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();


	reloadCotrolCaption(IDC_CHECK_REFLECTION, _T("CHECK_REFLECTION"));
	reloadCotrolCaption(IDC_CHECK_OCCLUSION, _T("CHECK_OCCLUSION"));
	reloadCotrolCaption(IDC_CHECK_POINT_LIGHT, _T("CHECK_POINT_LIGHT"));
	reloadCotrolCaption(IDC_CHECK_BUMP_MAPPING, _T("CHECK_BUMP_MAPPING"));
	reloadCotrolCaption(IDC_CHECK_BUMP_CHAOS, _T("CHECK_BUMP_CHAOS"));
	reloadCotrolCaption(IDC_CHECK_FAVORITE_LOAD_DDS, _T("CHECK_FAVORITE_LOAD_DDS"));
	reloadCotrolCaption(IDC_STATIC_SHADOW_TYPE, _T("STATIC_SHADOW_TYPE"));
	reloadCotrolCaption(IDC_RADIO_SHADOW_TYPE1, _T("RADIO_SHADOW_TYPE1"));
	reloadCotrolCaption(IDC_RADIO_SHADOW_TYPE2, _T("RADIO_SHADOW_TYPE2"));
	reloadCotrolCaption(IDC_STATIC_SHADOW_SAMPLES, _T("STATIC_SHADOW_SAMPLES"));
	reloadCotrolCaption(IDC_STATIC_PARTICLE_RATE, _T("STATIC_PARTICLE_RATE"));
	reloadCotrolCaption(IDC_STATIC_SHADOW_QUALITY, _T("STATIC_SHADOW_QUALITY"));
	reloadCotrolCaption(IDC_STATIC_MAP_LEVEL_LOD, _T("STATIC_MAP_LEVEL_LOD"));
	reloadCotrolCaption(IDCANCEL, _T("IDCANCEL"));

	LocaleMngr const& mngr = LocaleMngr::instance();
	CString caption = mngr.getText(mngr.makeControlKey(_T("DLG_CUSTOM_GRAPH_CAPTION")));
	SetWindowText(caption);

	graphSettingsSet_.initControl(this, LocaleMngr::instance());	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
