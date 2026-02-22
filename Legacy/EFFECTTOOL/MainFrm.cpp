// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "EffectTool.h"

#include "MainFrm.h"
#include "EffectTreeView.h"
#include "ControlView.h"
#include "OptTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_TOOLS_POS, OnUpdateTools)
	ON_COMMAND(ID_TOOLS_BS_DIR, OnToolsBsDir)
	ON_COMMAND(ID_TOOLS_DIR, OnToolsDir)
	ON_COMMAND(ID_TOOLS_POS, OnToolsPos)
	ON_COMMAND(ID_FULLSCREEN,OnToolsFullScreen)
	ON_COMMAND(ID_SPLINE_EDIT,OnSplineEdit)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_DIR, OnUpdateToolsDir)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BS_DIR, OnUpdateToolsInt)
	ON_UPDATE_COMMAND_UI(ID_BUT_ONE, OnUpdateToolsOptEnb)
	ON_UPDATE_COMMAND_UI(ID_BUT_TWO, OnUpdateToolsOptEnb)
	ON_UPDATE_COMMAND_UI(ID_FULLSCREEN, OnUpdateToolsOptEnb)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_FORALL, OnUpdateForAll)
	ON_UPDATE_COMMAND_UI(ID_FINDCENTER,OnUpdateToolsOptEnb)
	ON_UPDATE_COMMAND_UI(ID_SPLINE_EDIT,OnUpdateSplineEdit)
	ON_COMMAND(ID_FINDCENTER,OnFindCenter)
	ON_WM_ACTIVATEAPP()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_BAR, OnUpdateBar)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// 

void CMainFrame::SaveToReg()
{
	int t,t1;
	m_wndSplitter.GetColumnInfo(0,t,t1);
	theApp.WriteProfileInt("MainFrm","Splitter_X", t);
	m_wndSplitter1.GetRowInfo(0,t,t1);	
	theApp.WriteProfileInt("MainFrm","Splitter1_Y",t);
	m_wndSplitter2.GetRowInfo(0,t,t1);	
	theApp.WriteProfileInt("MainFrm","Splitter2_Y",t);
}
void CMainFrame::LoadFromReg()
{
	int t;
	t = theApp.GetProfileInt("MainFrm","Splitter_X",200);
	m_wndSplitter.SetColumnInfo(0,t,0);
	t = theApp.GetProfileInt("MainFrm","Splitter1_Y",180);
	m_wndSplitter1.SetRowInfo(0,t,0);
	t = theApp.GetProfileInt("MainFrm","Splitter2_Y",400);
	m_wndSplitter2.SetRowInfo(0,t,0);
}
CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

CEffectToolDoc* CMainFrame::GetDocument()
{
	return (CEffectToolDoc*)GetActiveDocument();
}
CD3DWindow* CMainFrame::GetD3DWindow()
{
	return (CD3DWindow*)m_wndSplitter2.GetPane(0, 0);
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS|CBRS_FLYBY );
	m_wndToolBar.LoadToolBar(IDR_MAINFRAME);
	m_wndToolBar.SetButtonStyle(5, TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(6, TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(7, TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(8, TBBS_CHECKBOX);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_FULLSCREEN,true);

/*	m_wndToolBarTools.CreateEx(this, TBSTYLE_FLAT, WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS|CBRS_FLYBY );//| TBSTYLE_ALTDRAG |CCS_ADJUSTABLE|CBRS_GRIPPER
	m_wndToolBarTools.LoadToolBar(IDR_TOOLBAR_TOOLS);
	m_wndToolBar.SetButtonStyle(0, TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(1, TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(2, TBBS_CHECKBOX);
*/
	OnTools();
	
	m_wndStatusBar.Create(this);
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	SetMenu(0);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


CControlView* ctrv = NULL;
COptTree* tr = NULL;
CMainFrame* mf = NULL;
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	VERIFY(m_wndSplitter.CreateStatic(this, 1, 2));
	m_wndSplitter1.CreateStatic(&m_wndSplitter,	2, 1, WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndSplitter.IdFromRowCol(0, 0));
	m_wndSplitter1.CreateView(0, 0, RUNTIME_CLASS(CEffectTreeView), CSize(150, 0), pContext);
	m_wndSplitter1.CreateView(1, 0, RUNTIME_CLASS(COptTree), CSize(150, 0), pContext);
	m_wndSplitter2.CreateStatic(&m_wndSplitter,	2, 1, WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndSplitter.IdFromRowCol(0, 1));
	m_wndSplitter2.CreateView(0, 0, RUNTIME_CLASS(CD3DWindow), CSize(0, 400), pContext);
	m_wndSplitter2.CreateView(1, 0, RUNTIME_CLASS(CControlView), CSize(0, 0), pContext);
	LoadFromReg();
	tr = (COptTree*)m_wndSplitter1.GetPane(1,0);
	ctrv = (CControlView*)m_wndSplitter2.GetPane(1,0);
	mf = this;
//	tr->ctrv = cc;
	return TRUE;
}
void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);

}

LRESULT CMainFrame::OnUpdateBar(WPARAM, LPARAM)
{
	return 0;
}


void CMainFrame::UpdateControlBar()
{
}

void CMainFrame::OnToolsFullScreen()
{
	bool t = !m_wndToolBar.GetToolBarCtrl().IsButtonChecked(ID_FULLSCREEN);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_FULLSCREEN, t);
	theApp.scene.SetMultiviewMode(!t);
}

void CMainFrame::OnFindCenter()
{
	theApp.scene.CameraDefault();
}

void CMainFrame::OnTools() 
{
	//Tools
	if(m_wndToolBar.GetToolBarCtrl().IsButtonChecked(ID_TOOLS_DIR))
		theApp.scene.m_ToolMode = CD3DScene::TOOL_DIRECTION;
	else if(m_wndToolBar.GetToolBarCtrl().IsButtonChecked(ID_TOOLS_POS))
		theApp.scene.m_ToolMode = CD3DScene::TOOL_POSITION;
	else if(m_wndToolBar.GetToolBarCtrl().IsButtonChecked(ID_TOOLS_BS_DIR))
		theApp.scene.m_ToolMode = CD3DScene::TOOL_DIRECTION_BS;
	else if(m_wndToolBar.GetToolBarCtrl().IsButtonChecked(ID_SPLINE_EDIT))
		theApp.scene.m_ToolMode = CD3DScene::TOOL_SPLINE;
	else
		theApp.scene.m_ToolMode = CD3DScene::TOOL_NONE;
	if (tr) tr->ShowOptEmiter();
}

void CMainFrame::OnToolsBsDir() 
{
//	m_wndToolBarTools.GetToolBarCtrl().CheckButton(ID_TOOLS_DIR, FALSE);
//	m_wndToolBarTools.GetToolBarCtrl().CheckButton(ID_TOOLS_POS, FALSE);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_TOOLS_DIR, FALSE);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_TOOLS_POS, FALSE);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_SPLINE_EDIT, FALSE);

	OnTools();
}
void CMainFrame::OnToolsDir() 
{
//	m_wndToolBarTools.GetToolBarCtrl().CheckButton(ID_TOOLS_POS, FALSE);
//	m_wndToolBarTools.GetToolBarCtrl().CheckButton(ID_TOOLS_BS_DIR, FALSE);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_TOOLS_POS, FALSE);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_TOOLS_BS_DIR, FALSE);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_SPLINE_EDIT, FALSE);

	GetDocument()->SetPause();
	OnTools();
}
void CMainFrame::OnToolsPos() 
{
//	m_wndToolBarTools.GetToolBarCtrl().CheckButton(ID_TOOLS_DIR, FALSE);
//	m_wndToolBarTools.GetToolBarCtrl().CheckButton(ID_TOOLS_BS_DIR, FALSE);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_TOOLS_DIR, FALSE);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_TOOLS_BS_DIR, FALSE);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_SPLINE_EDIT, FALSE);

	GetDocument()->SetPause();
	OnTools();
}

void CMainFrame::OnUpdateTools(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetDocument()->m_pActiveEmitter != 0 /*&& theApp.scene.m_bPause*/);
}
void CMainFrame::OnUpdateToolsInt(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->m_pActiveEmitter != 0 &&
		(GetDocument()->m_pActiveEmitter->Class() == EMC_INTEGRAL || GetDocument()->m_pActiveEmitter->Class() == EMC_INTEGRAL_Z));
}
void CMainFrame::OnUpdateToolsOptEnb(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}
void CMainFrame::OnUpdateForAll(CCmdUI* pCmdUI)
{
	if (GetDocument()) pCmdUI->Enable(GetDocument()->m_EnableButForAll );
}
void CMainFrame::OnUpdateSplineEdit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()&&GetDocument()->m_pActiveEmitter&&
		GetDocument()->m_pActiveEmitter->Class()==EMC_SPLINE);
}
void CMainFrame::SplineEndEdit()
{
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_SPLINE_EDIT, FALSE);
	theApp.scene.m_ToolMode = CD3DScene::TOOL_NONE;
}
void CMainFrame::OnSplineEdit()
{
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_TOOLS_BS_DIR, FALSE);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_TOOLS_DIR, FALSE);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_TOOLS_POS, FALSE);
	OnTools();
}
void CMainFrame::OnUpdateToolsDir(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(	GetDocument()->m_pActiveEmitter &&
					GetDocument()->m_pActiveEmitter->data &&
					(GetDocument()->m_pActiveEmitter->data->particle_position.type != EMP_3DMODEL));
}


void CMainFrame::OnActivateApp(BOOL bActive, DWORD hTask) 
{
	CFrameWnd::OnActivateApp(bActive, hTask);
	theApp.SetActive(bActive);
}
void CMainFrame::SetFps(float fps) {tr->SetFps(fps);};//m_wndDlgBar.SetFps(fps);};

void CMainFrame::OnClose()
{
	SaveToReg();
	CFrameWnd::OnClose();
}
