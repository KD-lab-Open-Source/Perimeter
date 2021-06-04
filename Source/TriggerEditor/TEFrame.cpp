// TEFrame.cpp : implementation file
//

#include "stdafx.h"
#include "TEFrame.h"
#include "resource.h"

#include "TriggerExport.h"
#include "TriggerEditor.h"
#include "TEEngine/TriggerEditorView.h"
#include "TEEngine/Profiler//TriggerProfList.h"

#include "Tree/TETreeManager.h"
#include "Scale/ScaleMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TEFrame

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

IMPLEMENT_DYNCREATE(TEFrame, CFrameWnd)

TEFrame::TEFrame(TriggerEditor* triggerEditor):
  hParentWnd_(NULL)
, treeManager_(NULL)
, triggerEditorView_(NULL)
, triggerEditor_(triggerEditor)
{
}

TEFrame::TEFrame():
  hParentWnd_(NULL)
, treeManager_(NULL)
, triggerEditorView_(NULL)
, triggerEditor_(NULL)
{
	assert(triggerEditor_);//его необходимо задать в другом месте
}

TEFrame::~TEFrame()
{
}

void TEFrame::setParent(HWND hparent)
{
	hParentWnd_ = hparent;
}

HWND TEFrame::getParent() const{
	return hParentWnd_;
}

void TEFrame::setTETreeManager(TETreeManager* mngr){
	treeManager_ = mngr;
}

TETreeManager* TEFrame::getTETreeManager() const{
	return treeManager_;
}
				   
TriggerProfList* TEFrame::getTriggerProfList() const{
	return triggerProfList_.get();
}

BEGIN_MESSAGE_MAP(TEFrame, CFrameWnd)
	//{{AFX_MSG_MAP(TEFrame)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
//	ON_COMMAND(ID_VIEW_PROFILER, onViewProfiler)
//	ON_UPDATE_COMMAND_UI(ID_VIEW_PROFILER, onUpdateViewProfiler)

	ON_COMMAND(ID_VIEW_TOOLBAR, onViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, onUpdateViewToolbar)

	ON_COMMAND(ID_VIEW_PROJECT_TREE, onViewTree)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECT_TREE, onUpdateViewTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TEFrame message handlers

void TEFrame::OnClose() 
{
	if (!triggerEditor_->isDataSaved())
	{
		int res = 0;
		res = AfxMessageBox(IDS_SAVE_PROJ_MSG, MB_YESNOCANCEL);
		if(res == IDYES)
		{
			triggerEditor_->save();
		}
		else if(res == IDCANCEL)
			return ;
	}

	triggerProfList_->hide();
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
		SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);

//	::EnableWindow(hParentWnd_, TRUE);
//	::SetActiveWindow(hParentWnd_);
	SaveBarState(CString((LPTSTR)IDS_REG_SEC_NAME_TOOLBARS));
	
	CFrameWnd::OnClose();
}

void TEFrame::PostNcDestroy() 
{
	AfxPostQuitMessage(0);
}

int TEFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	const int SCALE_TOOLBAR_ID	= 2;

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	if (!wndStatusBar_.Create(this) ||
		!wndStatusBar_.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0(_T("Failed to create status bar\n"));
		return -1;      // fail to create
	}

	if (!mainToolBar_.CreateEx(this, TBSTYLE_FLAT, WS_CHILD /*| WS_VISIBLE */| CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, 
		CRect(0, 0, 0, 0), IDR_TE_MAIN_TOOLBAR) ||
		!mainToolBar_.LoadToolBar(IDR_TE_MAIN_TOOLBAR))
	{
		TRACE0("Failed to create main toolbar\n");
		return -1;      // fail to create
	}

	scaleMgr_.reset(new ScaleMgr);
	if (!scaleMgr_)
		return -1;
	if (!scaleMgr_->Create(this, SCALE_TOOLBAR_ID))
	{
		TRACE0(_T("Failed to create scaleMgr_ \n"));
		return -1;
	}
	ASSERT(treeManager_);
	if (!treeManager_->create(this))
	{
		TRACE0(_T("Failed to create treeManager_\n"));
		return -1;
	}

	triggerProfList_.reset(new TriggerProfList);
	if (!triggerProfList_)
	{
		TRACE0(_T("Failed to init triggerProfList_\n"));
		return -1;
	}
	if (!triggerProfList_->create(this))
	{
		TRACE0(_T("Failed to create triggerProfList_\n"));
		return -1;
	}

	EnableDocking(CBRS_ALIGN_ANY);
	mainToolBar_.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&mainToolBar_, AFX_IDW_DOCKBAR_TOP);

	scaleMgr_->dockBar(AFX_IDW_DOCKBAR_TOP);
	treeManager_->dockBar(AFX_IDW_DOCKBAR_LEFT);
	triggerProfList_->dockBar(AFX_IDW_DOCKBAR_BOTTOM);


	ShowControlBar(&mainToolBar_, TRUE, FALSE); 
	scaleMgr_->Show();
	treeManager_->show(TETreeManager::SH_SHOW);


	LoadBarState(CString((LPTSTR)IDS_REG_SEC_NAME_TOOLBARS));
		
	return 0;
}

BOOL TEFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, 
					   AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (triggerEditorView_
		&&triggerEditorView_->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	if (scaleMgr_&&scaleMgr_->getWindow()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))	
		return TRUE;
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL TEFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return TRUE;
}

bool TEFrame::initTETreeManager()
{
	ASSERT(treeManager_);

	return true;
}

BOOL TEFrame::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

TriggerEditorView* TEFrame::addTriggerEditorView()
{
	CCreateContext cc;
	memset(&cc, 0, sizeof(cc));
	cc.m_pCurrentFrame = this;
	cc.m_pNewViewClass = RUNTIME_CLASS(TriggerEditorView);

	triggerEditorView_ = static_cast<TriggerEditorView*>(CreateView(&cc));
	triggerEditorView_->SetScaleMgr(scaleMgr_.get());
	scaleMgr_->SetScalable(triggerEditorView_);
	return triggerEditorView_;
}


void TEFrame::updateViewSize()
{
	ASSERT(IsWindow(m_hWnd));
	ASSERT(triggerEditorView_);
	CRect r;
	GetClientRect(&r);
	triggerEditorView_->MoveWindow(r);
}

void TEFrame::OnFileSave()
{
	CWaitCursor wait;
	triggerEditor_->save();
}

void TEFrame::onViewToolbar()
{
	ShowControlBar(&mainToolBar_,!mainToolBar_.IsVisible(), FALSE); 
}

void TEFrame::onUpdateViewToolbar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(mainToolBar_.IsVisible());
}

void TEFrame::onViewTree()
{
	if (treeManager_->isVisible())
		treeManager_->show(TETreeManager::SH_HIDE);
	else
		treeManager_->show(TETreeManager::SH_SHOW);
}

void TEFrame::onUpdateViewTree(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(treeManager_->isVisible());
}
