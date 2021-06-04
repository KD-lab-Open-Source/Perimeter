// GeoTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GeoTool.h"

#include "MainFrm.h"

#include <IndexedTexture3D.hpp>

#include "..\Terra\terra.h"
#include "..\UserInterface\chaos.h"
#include "..\PluginMax\ZIPStream.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeoToolApp

BEGIN_MESSAGE_MAP(CGeoToolApp, CWinApp)
	//{{AFX_MSG_MAP(CGeoToolApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_LOAD_MAP, OnLoadMap)
	ON_COMMAND(ID_REMOVE_DAM, OnRemoveDamLayer)
	ON_COMMAND(ID_PLANE_DAM, OnPlaneDamLayer)
	ON_COMMAND(ID_PLANE_DAM_LOCAL, OnPlaneDamLocal)
	ON_COMMAND(ID_REMOVE_DAM_LOCAL, OnRemoveDamLocal)
	ON_COMMAND(ID_SHOW_SLOT_PROPERTIES, OnShowSlotProperties)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SLOT_PROPERTIES, OnUpdateShowSlotProperties)
	ON_COMMAND(ID_SHOW_SLOTS, OnShowSlots)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SLOTS, OnUpdateShowSlots)
	ON_COMMAND(ID_LOAD_TEXTURE, OnLoadTexture)
	ON_COMMAND(ID_SAVE_TEXTURE, OnSaveTexture)
	ON_COMMAND(ID_BUTTON_TOGGLE_INDEXED, OnToggleIndexed)
	ON_COMMAND(ID_SHOW_INSTRUMENTS, OnShowInstruments)
	ON_UPDATE_COMMAND_UI(ID_SHOW_INSTRUMENTS, OnUpdateShowInstruments)
	ON_COMMAND(ID_BTN_ZOOM, OnBtnZoom)
	ON_COMMAND(ID_BUTTONZERO, OnButtonzero)
	ON_COMMAND(ID_PUBLISH, OnPublish)
	ON_COMMAND(ID_BUTTON_ZSHIFT_MINUS, OnButtonZshiftMinus)
	ON_COMMAND(ID_BUTTON_ZSHIFT_PLUS, OnButtonZshiftPlus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeoToolApp construction

CGeoToolApp::CGeoToolApp()
{
/*
	#include <CrtDBG.h>
	_CrtSetBreakAlloc(53);
*/
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGeoToolApp object

CGeoToolApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGeoToolApp initialization

BOOL CGeoToolApp::InitInstance()
{
	AfxEnableControlContainer();
	// Для ФПС
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
	SetRegistryKey(_T("GeoTool"));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources

	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);




	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	instrumentsDialog.Create(CInstrumentsDialog::IDD, m_pMainWnd);
	slotPropertiesDialog.Create(CSlotPropertiesDialog::IDD, m_pMainWnd);
	slotsDialog.Create(CSlots::IDD, m_pMainWnd);
	OnLoadMap();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGeoToolApp message handlers





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
void CGeoToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CGeoToolApp::OnIdle(LONG lCount) 
{
	runtime->setInputEnabled(GetForegroundWindow() == m_pMainWnd->m_hWnd);

    if (m_pMainWnd->IsIconic()) {
        return FALSE;
	}

	if (CWinApp::OnIdle(lCount)) {
		return TRUE;
	}

	if (runtime) {
		runtime->quant();
	}
	return TRUE;
}

int CGeoToolApp::ExitInstance() 
{
//	runtime.doneRenderDevice();
	
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CGeoToolApp message handlers



void CGeoToolApp::OnLoadMap() 
{
	if (runtime){
		if ( loadMapDialog.DoModal() == IDOK ) {
			CWaitCursor wait;
			runtime->loadMap(loadMapDialog.getSelectedMap());
		}
	}
	// TODO: Add your command handler code here
	
}

void CGeoToolApp::OnRemoveDamLayer() 
{
	if (runtime){
		VMapOperator* vMapOp = runtime->getVMapOperator();
		if (vMapOp) {
			vMapOp->removeDamLayer();
		}
	}
}

void CGeoToolApp::OnPlaneDamLayer() 
{
	if (runtime){
		VMapOperator* vMapOp = runtime->getVMapOperator();
		if (vMapOp) {
//			vMapOp->planeToZeroplast();
			vMapOp->planeDamLayer(-1);
		}
	}
}

void CGeoToolApp::OnPlaneDamLocal() 
{
	if (runtime){
		VMapOperator* vMapOp = runtime->getVMapOperator();
		ColorMapManager* cMapManager = runtime->getColorMapManager();
		if (vMapOp && cMapManager) {
			vMapOp->planeDamLayer(70, cMapManager->getExactGeoColorMapRect());
		}
	}
}

void CGeoToolApp::OnRemoveDamLocal() 
{
	if (runtime){
		VMapOperator* vMapOp = runtime->getVMapOperator();
		ColorMapManager* cMapManager = runtime->getColorMapManager();
		if (vMapOp && cMapManager) {
			vMapOp->removeDamLayer(cMapManager->getExactGeoColorMapRect());
		}
	}
}

void CGeoToolApp::setBrush(VMapOperator::BrushType type) {
	if (runtime){
		VMapOperator* vMapOp = runtime->getVMapOperator();
		if (vMapOp) {
			vMapOp->setBrush(type);
		}
	}
}

void CGeoToolApp::OnShowSlotProperties() 
{
	slotPropertiesDialog.ShowWindow(slotPropertiesDialog.IsWindowVisible() ? SW_HIDE : SW_SHOW);
}

void CGeoToolApp::OnUpdateShowSlotProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(slotPropertiesDialog.IsWindowVisible() ? 1 : 0);
}

void CGeoToolApp::OnShowSlots() 
{
	slotsDialog.ShowWindow(slotsDialog.IsWindowVisible() ? SW_HIDE : SW_SHOW);
}

void CGeoToolApp::OnUpdateShowSlots(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(slotsDialog.IsWindowVisible() ? 1 : 0);
}

void CGeoToolApp::OnLoadTexture() 
{
	CFileDialog fileDialog(
		TRUE,
		"xml",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		NULL,
		m_pMainWnd);

	if (fileDialog.DoModal() == IDOK) {
		ifstream ifsTx(fileDialog.GetPathName());
		char szBuffer[1024 * 10];
		ifsTx.get(szBuffer, 1024 * 10, '\0');

		tx3d::Texture3D* pTx = tx3d::Texture3DFactory::createTexture3D(string(szBuffer));
		if (!pTx) {
			pTx = tx3d::Texture3DFactory::createTexture3D("<texture type='Clear'/>");
		}
		pTx->referenced();
		slotsDialog.parseTexture(pTx);

		string folder = string(fileDialog.GetPathName());
		folder.resize(folder.length() - 3);

		tx3d::Vector3D pal[256];
		ifstream ifsLUT( (folder + "geoPal.xml").c_str(), ios::in | ios::nocreate );
		if (ifsLUT) {
			char szBuffer[258 * 60];
			ifsLUT.get(szBuffer, 258 * 60, '\0');
			tx3d::IndexedTexture3D::extractColorTableFromXML(string(szBuffer), pal);
//
			unsigned char indexLattice[65536];
			ifstream ifsLattice((folder + "geoLattice.bin").c_str(), ios::in | ios::nocreate);
			if (ifsLattice) {
				ifsLattice.setmode(filebuf::binary);
				ifsLattice.read(indexLattice, 65536);
			}
		}

		unsigned char indexLattice[65536];
		ifstream ifsLattice((folder + "geoLattice.bin").c_str(), ios::in | ios::nocreate);
		if (ifsLattice) {
			ifsLattice.setmode(filebuf::binary);
			ifsLattice.read(indexLattice, 65536);
		}

		runtime->setIndexingParams(pal, indexLattice);
		pTx->released();
	}
}
void CGeoToolApp::OnToggleIndexed() 
{
	runtime->toggleShowIndexed();
}

void CGeoToolApp::OnSaveTexture() 
{
	tx3d::Texture3D* tx = slotsDialog.assembleTexture();

	if (tx) {
		CFileDialog fileDialog(
			FALSE,
			"xml",
			NULL,
			OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			NULL,
			m_pMainWnd);

		if (fileDialog.DoModal() == IDOK) {
			string folder = string(fileDialog.GetPathName());
			ofstream ofs(fileDialog.GetPathName());
			string strLUT = tx3d::XMLUtils::serializableToTag("texture", *tx);

			tx3d::XMLUtils::indentXML(strLUT);
			ofs << strLUT.c_str();
			ofs.close();

			CIndexerDialog indexerDialog(m_pMainWnd);

			if (indexerDialog.DoModal() == IDOK) {
				tx3d::Vector3D pal[256];

				tx3d::IndexedTexture3D::fillColorTable(
					tx,
					pal,
					indexerDialog.getStep(),
					indexerDialog.getFrom(),
					indexerDialog.getTo(),
					indexerDialog.getTreshold(),
					false
				);

				unsigned char lattice[65536];

				tx3d::IndexedTexture3D::fillIndexLattice(pal, lattice);

//				string file = string(fileDialog.GetFileName());
//				folder.resize(folder.length() - file.length());
				folder.resize(folder.length() - 3);
				ofs.open(
						(folder + "geoPal.xml").c_str()
					);
				string strLUT = tx3d::IndexedTexture3D::colorTableToXML("colorTable", pal);
				tx3d::XMLUtils::indentXML(strLUT);
				ofs << strLUT.c_str();
				ofs.close();

				//-----
				unsigned char binPal[768];
				for (int i = 0; i < 256; i++) {
					binPal[i * 3] = Texture3DUtils::round(pal[i].x * 255);
					binPal[i * 3 + 1] = Texture3DUtils::round(pal[i].y * 255);
					binPal[i * 3 + 2] = Texture3DUtils::round(pal[i].z * 255);
				}
				ofs.open(
						(folder + "pal.pal").c_str()
					);
				ofs.setmode(filebuf::binary);
				ofs.write(binPal, 768);
				ofs.close();
				//-----


				ofs.open(
						(folder + "geoLattice.bin").c_str()
					);
				ofs.setmode(filebuf::binary);
				ofs.write(lattice, 65536);
				ofs.close();
				runtime->setIndexingParams(pal, lattice);
			}
		}
	}	
}

void CGeoToolApp::OnShowInstruments() 
{
	instrumentsDialog.ShowWindow(instrumentsDialog.IsWindowVisible() ? SW_HIDE : SW_SHOW);
}

void CGeoToolApp::OnUpdateShowInstruments(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(instrumentsDialog.IsWindowVisible() ? 1 : 0);
}

void CGeoToolApp::OnBtnZoom() 
{
	// TODO: Add your command handler code here
	tx3d::Texture3D* tx = slotsDialog.assembleTexture();
	tx->referenced();
	tx->zoom(0.5f);
	slotsDialog.parseTexture(tx);
	tx->released();
}

void CGeoToolApp::OnButtonzero() 
{
	if (runtime){
		VMapOperator* vMapOp = runtime->getVMapOperator();
		if (vMapOp) {
			vMapOp->planeToZeroplast();
		}
	}
}

void CGeoToolApp::OnPublish() 
{
	// TODO: Add your command handler code here

}

void CGeoToolApp::OnButtonZshiftMinus() 
{
	if (runtime){
		ColorMapManager* man = runtime->getColorMapManager();
		man->setZShift(man->getZShift() - 5);
		runtime->updateRect(Rect4i(0, 0, vMap.H_SIZE, vMap.V_SIZE));
	}
}

void CGeoToolApp::OnButtonZshiftPlus() 
{
	if (runtime){
		ColorMapManager* man = runtime->getColorMapManager();
		man->setZShift(man->getZShift() + 5);
		runtime->updateRect(Rect4i(0, 0, vMap.H_SIZE, vMap.V_SIZE));
	}
}
