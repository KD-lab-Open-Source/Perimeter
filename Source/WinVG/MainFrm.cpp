// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "WinVG.h"

#include "MainFrm.h"

#include "WinVGDoc.h"
#include "WinVGView.h"
#include "HierarchyTree.h"
#include "DirectoryTree.h"
#include "TextureDirectory.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern cCamera		*gb_Camera1;
extern cVisGeneric	*gb_IVisGeneric;
CMainFrame*			gb_FrameWnd=NULL;
extern bool			g_bPressLighing;
extern string textures_subdir;
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_BUTTON_BBOX, OnButtonBbox)
	ON_COMMAND(ID_BUTTON_BOUND, OnButtonBound)
	ON_COMMAND(ID_BUTTON_LOGIC, OnButtonLogic)
	ON_COMMAND(ID_BUTTON_OBJECT, OnButtonObject)
	ON_WM_SIZE()
	ON_COMMAND(ID_BUTTON_LIGHTING, OnButtonLighting)
	ON_COMMAND(ID_MENU_CONTROL_CAMERA, OnMenuControlCamera)
	ON_COMMAND(ID_MENU_CONTROL_OBJECT, OnMenuControlObject)
	ON_COMMAND(ID_MENU_CONTROL_DIRLIGHT, OnMenuControlDirlight)
	ON_COMMAND(ID_BUTTON_COLOR, OnButtonColor)
	ON_COMMAND(ID_BUTTON_BKCOLOR, OnButtonBkcolor)
	ON_COMMAND(ID_DEBUG_MEMORY, OnDebugMemory)
	ON_COMMAND(ID_NUM_OBJECT, OnNumObject)
	ON_UPDATE_COMMAND_UI(ID_NUM_OBJECT, OnUpdateNumObject)
	ON_COMMAND(ID_HOLOGRAM, OnHologram)
	ON_UPDATE_COMMAND_UI(ID_HOLOGRAM, OnUpdateHologram)
	ON_COMMAND(ID_SCALENORMAL, OnScalenormal)
	ON_UPDATE_COMMAND_UI(ID_SCALENORMAL, OnUpdateScalenormal)
	ON_COMMAND(ID_BUTTON_DEBUG_SHADOW_VOLUME, OnButtonDebugShadowVolume)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_DEBUG_SHADOW_VOLUME, OnUpdateButtonDebugShadowVolume)
	ON_COMMAND(ID_BUTTON_SHADOW, OnButtonShadow)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SHADOW, OnUpdateButtonShadow)
	ON_COMMAND(ID_ENABLE_BUMP, OnEnableBump)
	ON_UPDATE_COMMAND_UI(ID_ENABLE_BUMP, OnUpdateEnableBump)
	ON_COMMAND(ID_EFFECT_DIRECTORY, OnEffectDirectory)
	ON_CBN_SELCHANGE(ID_ANIMATION_LIST, OnSelchangeAnimationList)
	ON_COMMAND(ID_FILE_SETTEXTUREDIRECTORY, OnFileSettexturedirectory)
	ON_COMMAND(ID_MODEL_INFO, OnModelInfo)
	ON_CBN_SELENDOK(ID_DIRECTORY_BOX, OnSelchangeDirectoryBox)
	ON_COMMAND(ID_ENABLE_ANISOTROPIC, OnEnableAnisotropic)
	ON_UPDATE_COMMAND_UI(ID_ENABLE_ANISOTROPIC, OnUpdateEnableAnisotropic)
	ON_COMMAND(ID_BUTTON_BASEMENT, OnButtonBasement)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_BASEMENT, OnUpdateButtonBasement)
	ON_COMMAND(ID_SCREEN_SHOOT, OnScreenShoot)
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(ID_ENABLE_TILEMAP, OnEnableTilemap)
	ON_UPDATE_COMMAND_UI(ID_ENABLE_TILEMAP, OnUpdateEnableTilemap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_FPS,
	ID_INDICATOR_PHASE,
	ID_INDICATOR_MEGA_POLY,
	ID_INDICATOR_POLY,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	extern void InitAllockHook();
	InitAllockHook();
	
	m_bPressBBox=m_bPressBound=m_bPressLogic=TRUE;
	m_bPressObject=FALSE;
	m_bInit=FALSE;
	gb_FrameWnd=this;
	use_shadow=false;
	scale_normal=true;
	effect_directory=GetRegistryString("effect_directory","");
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		/*| CBRS_GRIPPER*/ | CBRS_TOOLTIPS /*| CBRS_FLYBY | CBRS_SIZE_DYNAMIC*/) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}


	EnableDocking(CBRS_ALIGN_ANY);
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar(&m_wndToolBar);
	AddListBox();
	AddDirectoryBox();

	OnButtonBbox();
	OnButtonBound();
	OnButtonLogic();
	OnButtonObject();
	OnButtonLighting();

	CheckControlSwitch();

	return 0;
}

void CMainFrame::AddListBox()
{
	TBBUTTONINFO bi;
	memset(&bi,0,sizeof(bi));
	bi.cbSize=sizeof(bi);
    bi.dwMask=TBIF_SIZE|TBIF_STYLE;
	bi.fsStyle=TBSTYLE_SEP;
    bi.cx=120;
	BOOL b=m_wndToolBar.GetToolBarCtrl().SetButtonInfo(ID_ANIMATION_LIST,&bi);

	CRect rc;
	int nIndex=m_wndToolBar.GetToolBarCtrl().CommandToIndex(ID_ANIMATION_LIST);
	m_wndToolBar.GetToolBarCtrl().GetItemRect(nIndex,rc);

	rc.bottom+=400;
	list_box.Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,
		rc,&m_wndToolBar,ID_ANIMATION_LIST);
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(!m_Splitter0.CreateStatic(this,1,3))
		return FALSE;
	if(!m_Splitter1.CreateStatic(&m_Splitter0,2,1,WS_CHILD|WS_VISIBLE,m_Splitter0.IdFromRowCol(0,0)))
		return FALSE;
	if(!m_Splitter1.CreateView(0,0,RUNTIME_CLASS(CHierarchyTree),CSize(100,100),pContext)||
		!m_Splitter1.CreateView(1,0,RUNTIME_CLASS(CHierarchyTree),CSize(100,100),pContext)||
		!m_Splitter0.CreateView(0,1,RUNTIME_CLASS(CWinVGView),CSize(100,100),pContext)||
		!m_Splitter0.CreateView(0,2,RUNTIME_CLASS(CDirectoryTree),CSize(100,100),pContext))
	{
		m_Splitter0.DestroyWindow();
		m_Splitter1.DestroyWindow();
		return FALSE;
	}
	return m_bInit=TRUE; //CFrameWnd::OnCreateClient(lpcs, pContext);
}


void CMainFrame::OnButtonBbox()
{
	m_bPressBBox=!m_bPressBBox;
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_BUTTON_BBOX,m_bPressBBox);
}

void CMainFrame::OnButtonBound() 
{
	m_bPressBound=!m_bPressBound;
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_BUTTON_BOUND,m_bPressBound);
}
void CMainFrame::OnButtonLogic() 
{
	m_bPressLogic=!m_bPressLogic;
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_BUTTON_LOGIC,m_bPressLogic);
}
void CMainFrame::OnButtonObject() 
{
	m_bPressObject=!m_bPressObject;
	pView->UpdateIgnore();
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_BUTTON_OBJECT,m_bPressObject);
}

void CMainFrame::OnButtonLighting() 
{
	g_bPressLighing=!g_bPressLighing;
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_BUTTON_LIGHTING,g_bPressLighing);

	pView->UpdateObjectLight();
/*
	if(gb_Camera1)
		if(m_bPressLighing) gb_Camera1->ClearAttr(ATTRUNKOBJ_NOLIGHT);
		else gb_Camera1->SetAttr(ATTRUNKOBJ_NOLIGHT);
*/
}
void CMainFrame::OnButtonColor() 
{
	// TODO: Add your command handler code here
	CWinVGDoc *Doc=(CWinVGDoc*)GetActiveDocument();
	CColorDialog ColorDlg;
	if(ColorDlg.DoModal()==IDCANCEL) return;
	if(Doc==0||Doc->m_pView==0||Doc->m_pHierarchyObj==0||Doc->m_pHierarchyObj->GetRoot()==0) return;
	int color=ColorDlg.GetColor();
	Doc->m_pView->SkinColor.set(((color>>0)&0xFF)/255.f,((color>>8)&0xFF)/255.f,((color>>16)&0xFF)/255.f,1);

	Doc->m_pHierarchyObj->SetSkinColor(Doc->m_pView->SkinColor);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	if(m_bInit)
	{
		m_Splitter0.SetColumnInfo(0,(int)(cx*0.2f),1);
		m_Splitter0.SetColumnInfo(1,(int)(cx*0.6f),1);
		m_Splitter1.SetRowInfo(0,(int)(cy*0.5f),1);
		m_Splitter0.SetActivePane(0,1);
		m_Splitter0.RecalcLayout();
		m_Splitter1.RecalcLayout();
	}
	CFrameWnd::OnSize(nType, cx, cy);
}

void CMainFrame::CheckControlSwitch()
{
	int s=pView->m_ControlSwitch;
	GetMenu()->CheckMenuItem(ID_MENU_CONTROL_OBJECT,s==0?MF_CHECKED:MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_MENU_CONTROL_CAMERA,s==1?MF_CHECKED:MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_MENU_CONTROL_DIRLIGHT,s==2?MF_CHECKED:MF_UNCHECKED);
}

void CMainFrame::OnMenuControlObject() 
{
	if(!pView) return;
	pView->m_ControlSwitch=0;
	CheckControlSwitch();
}
void CMainFrame::OnMenuControlCamera() 
{
	if(!pView) return;
	pView->m_ControlSwitch=1;
	CheckControlSwitch();
}
void CMainFrame::OnMenuControlDirlight() 
{
	if(!pView) return;
	pView->m_ControlSwitch=2;
	CheckControlSwitch();
}

void CMainFrame::OnButtonBkcolor() 
{
	// TODO: Add your command handler code here
	CWinVGDoc *Doc=(CWinVGDoc*)GetActiveDocument();
	CColorDialog ColorDlg;
	if(ColorDlg.DoModal()==IDCANCEL) return;
	if(Doc==0||Doc->m_pView==0) return;
	int color=ColorDlg.GetColor();
	Doc->m_pView->Color.set((color>>0)&0xFF,(color>>8)&0xFF,(color>>16)&0xFF,255);
}

void CMainFrame::OnActivateApp(BOOL bActive, DWORD hTask) 
{
	CFrameWnd::OnActivateApp(bActive, hTask);
	theApp.SetActive(bActive);
}


void CMainFrame::OnDebugMemory() 
{
	extern void DebugMemInfo();
	DebugMemInfo();
}

void CMainFrame::OnNumObject() 
{
	pView->OnNumObject();
}

void CMainFrame::OnUpdateNumObject(CCmdUI* pCmdUI) 
{
	pView->OnUpdateNumObject(pCmdUI) ;
}

void CMainFrame::OnHologram() 
{
	pView->OnHologram();
}

void CMainFrame::OnUpdateHologram(CCmdUI* pCmdUI) 
{
	pView->OnUpdateHologram(pCmdUI);
}

void CMainFrame::OnScalenormal() 
{
	scale_normal=!scale_normal;
	pView->SetScale(scale_normal);
}

void CMainFrame::OnUpdateScalenormal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(!scale_normal);
}

static int bDebugShadowVolume=0;
void CMainFrame::OnButtonDebugShadowVolume() 
{
	bDebugShadowVolume=(bDebugShadowVolume+1)%3;
	gb_IVisGeneric->SetDebugShowShadowVolume(bDebugShadowVolume);
}

void CMainFrame::OnUpdateButtonDebugShadowVolume(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(bDebugShadowVolume?1:0);
}

void SetUseShadow(bool use);
void CMainFrame::OnButtonShadow() 
{
	use_shadow=!use_shadow;
	SetUseShadow(use_shadow);
}

void CMainFrame::OnUpdateButtonShadow(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(use_shadow?1:0);
}

void CMainFrame::OnEnableBump() 
{
	gb_VisGeneric->SetEnableBump(!gb_VisGeneric->GetEnableBump());
}

void CMainFrame::OnUpdateEnableBump(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(gb_VisGeneric->GetEnableBump()?1:0);
}

void CMainFrame::LoadRegistry()
{
	use_shadow=GetRegistryInt("use_shadow", false)?true:false;
	scale_normal=GetRegistryInt("scale_normal", true)?true:false;
	gb_VisGeneric->SetEnableBump(GetRegistryInt("enable_bump",true));
	SetUseShadow(use_shadow);
	pView->SetScale(scale_normal);

	pView->m_ControlSwitch=GetRegistryInt("m_ControlSwitch",0);
	CheckControlSwitch();
	textures_subdir=GetRegistryString("textures_subdir",textures_subdir.c_str());
	pView->enable_tilemap=GetRegistryInt("enable_tilemap", false)?true:false;
}

void CMainFrame::SaveRegistry()
{
	WriteRegistryInt("use_shadow",use_shadow);
	WriteRegistryInt("scale_normal",scale_normal);
	WriteRegistryInt("enable_bump",gb_VisGeneric->GetEnableBump());
	WriteRegistryInt("m_ControlSwitch",pView->m_ControlSwitch);
	WriteRegistryString("textures_subdir",textures_subdir.c_str());
	WriteRegistryInt("enable_tilemap",pView->enable_tilemap);
}

void CMainFrame::OnEffectDirectory() 
{
	CFileDialog dlg(true,NULL,NULL/*effect_directory.c_str()*/,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"*.effect|*.effect||",this);
	if(dlg.DoModal()!=IDOK)
		return;

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	CString fin=dlg.GetPathName();
	_splitpath(fin,drive,dir,fname,ext);
	effect_directory=drive+string(dir);
	WriteRegistryString("effect_directory",effect_directory.c_str());
	pView->SetEffectDirectory(effect_directory.c_str());
}

void CMainFrame::UpdateList()
{
	list_box.ResetContent();
	CWinVGDoc *Doc=(CWinVGDoc*)GetActiveDocument();
	vector<cIUnkClass*>& obj=pDoc->m_pHierarchyObj->GetAllObj();
	if(obj.empty())
		return;

	cIUnkClass* p=obj[0];

	int num=(dynamic_cast<cObjectNode*>(p))->GetNumberChannel();
	if(num<=0)return;
	for(int i=0;i<num;i++)
	{
		char name[256];
		(dynamic_cast<cObjectNode*>(p))->GetChannel(i,name);
		list_box.AddString(name);
	}

	list_box.SetCurSel(0);
}

void SetAnim(CHierarchyTree *Tree,const char* ncannel)
{
	if(Tree==0||Tree->GetRoot()==0) return;

	vector<cIUnkClass*>& obj=Tree->GetAllObj();;
	vector<cIUnkClass*>::iterator it;
	FOR_EACH(obj,it)
	{
		cLogicObject* pl=dynamic_cast<cLogicObject*>(*it);
		if(pl)pl->SetChannel(ncannel);
		cObjectNode* pn=dynamic_cast<cObjectNode*>(*it);
		if(pn)pn->SetChannel(ncannel,true);
	}
	Tree->TreeUpdate();
}

void CMainFrame::OnSelchangeAnimationList() 
{
	int ncannel=list_box.GetCurSel();
	if(ncannel<0)
		ncannel=0;

	CWinVGDoc *Doc=(CWinVGDoc*)GetActiveDocument();
	vector<cIUnkClass*>& obj=pDoc->m_pHierarchyObj->GetAllObj();
	if(obj.empty())
		return;
	char name[256];
	(dynamic_cast<cObjectNode*>(obj.front()))->GetChannel(ncannel,name);

	SetAnim(pDoc->m_pHierarchyObj,name);
	SetAnim(pDoc->m_pHierarchyLogic,name);
}

void CMainFrame::UnknownSelect()
{
	list_box.SetCurSel(-1);
}


void CMainFrame::OnFileSettexturedirectory() 
{
	CTextureDirectory dlg;
	dlg.DoModal();

	cObjectNodeRoot* UObj=(cObjectNodeRoot*)(pDoc->m_pHierarchyObj?pDoc->m_pHierarchyObj->GetRoot():NULL);

	if(UObj)
		pView->LoadObject(UObj->GetFileName());
}

void CMainFrame::OnModelInfo() 
{
	pView->ModelInfo();
}

void CMainFrame::AddDirectoryBox()
{
	TBBUTTONINFO bi;
	memset(&bi,0,sizeof(bi));
	bi.cbSize=sizeof(bi);
    bi.dwMask=TBIF_SIZE|TBIF_STYLE;
	bi.fsStyle=TBSTYLE_SEP;
    bi.cx=180;
	BOOL b=m_wndToolBar.GetToolBarCtrl().SetButtonInfo(ID_DIRECTORY_BOX,&bi);

	CRect rc;
	int nIndex=m_wndToolBar.GetToolBarCtrl().CommandToIndex(ID_DIRECTORY_BOX);
	m_wndToolBar.GetToolBarCtrl().GetItemRect(nIndex,rc);

	rc.bottom+=400;
	directory_box.Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,
		rc,&m_wndToolBar,ID_DIRECTORY_BOX);


	/////////////////////////////
	char strPath[260];
	GetCurrentDirectory(256,strPath);
	strcat(strPath,"\\*.*");

	int nTextures=0,cur=0;

	_finddata_t fileinfo;
	int ff=_findfirst(strPath,&fileinfo);
	if(ff>0)
	while(_findnext(ff,&fileinfo)==0)
	if(fileinfo.attrib&_A_SUBDIR)
	{
		if(strcmp(fileinfo.name,".")==0)
			continue;
		if(strcmp(fileinfo.name,"..")==0)
			continue;

		_strupr(fileinfo.name);
		directory_box.AddString(fileinfo.name);
		if(stricmp(fileinfo.name,textures_subdir.c_str())==0)
		{
			nTextures=cur;
		}

		cur++;
	}
	_findclose( ff );
	directory_box.SetWindowText(textures_subdir.c_str());
	directory_box.SetCurSel(nTextures); 
}

void CMainFrame::OnSelchangeDirectoryBox()
{
	CString str;
	directory_box.GetWindowText(str);
	textures_subdir=str;

	cObjectNodeRoot* UObj=(cObjectNodeRoot*)(pDoc->m_pHierarchyObj?pDoc->m_pHierarchyObj->GetRoot():NULL);
	if(UObj)
		pView->LoadObject(UObj->GetFileName());
}

void CMainFrame::OnEnableAnisotropic() 
{
	gb_IVisGeneric->SetAnisotropic(!gb_IVisGeneric->GetAnisotropic());
	
}

void CMainFrame::OnUpdateEnableAnisotropic(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(gb_IVisGeneric->GetAnisotropic());
}

void CMainFrame::OnButtonBasement() 
{
	pView->show_basement=!pView->show_basement;
	
}

void CMainFrame::OnUpdateButtonBasement(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(pView->show_basement);
}

void CMainFrame::OnScreenShoot() 
{
	pView->OnScreenShoot();
}

void CMainFrame::OnEnableTilemap() 
{
	pView->enable_tilemap=!pView->enable_tilemap;
	gb_IVisGeneric->SetShowType(SHOW_TILEMAP,pView->enable_tilemap);
}

void CMainFrame::OnUpdateEnableTilemap(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(pView->enable_tilemap);
	
}
