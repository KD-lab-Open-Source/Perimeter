// TabDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WinVG.h"
#include "TabDialog.h"
#include "tabdialog.h"
#include "WinVGDoc.h"
#include "WinVGView.h"

void CTabDialog::GetRect(CWnd& wnd,CRect& rc)
{
	wnd.GetWindowRect(rc);
	ScreenToClient(rc);
}
void CTabDialog::SetPos(CWnd& wnd,CRect& rc)
{
	wnd.SetWindowPos(NULL,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,0);
}

// CTabDialog

IMPLEMENT_DYNCREATE(CTabDialog, CFormView)

CTabDialog::CTabDialog()
	: CFormView(CTabDialog::IDD)
{
	logic_obj=NULL;
}

CTabDialog::~CTabDialog()
{
	ClearRoot();
}

void CTabDialog::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, treeList);
	DDX_Control(pDX, IDC_VISIBILITY_GROUP, visibility_group);
	DDX_Control(pDX, IDC_CHAIN, chains);
	if(!pDX->m_bSaveAndValidate)
	{
		SetHeadAnimationGroup();
	}
}

BEGIN_MESSAGE_MAP(CTabDialog, CFormView)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_VISIBILITY_GROUP, OnCbnSelchangeVisibilityGroup)
	ON_CBN_SELCHANGE(IDC_CHAIN, OnCbnSelchangeChains)
END_MESSAGE_MAP()


// CTabDialog diagnostics

#ifdef _DEBUG
void CTabDialog::AssertValid() const
{
	CFormView::AssertValid();
}

void CTabDialog::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CTabDialog message handlers
void CTabDialog::TreeUpdate()
{
	UpdateAnimationGroup();
	UpdateVisibilityGroup();
	UpdateAnimationGroupBox();
}

void CTabDialog::SetRoot(cObject3dx* pUObj,CString _filename)
{
	ClearRoot();

	if(pUObj)
		UObj.push_back(pUObj);
	filename=_filename;
	filename.MakeUpper();
}

void CTabDialog::AddRoot(cObject3dx* pUObj)
{
	if(pUObj)
		UObj.push_back(pUObj);
}

void CTabDialog::SetLogicObj(cObject3dx* pUObj)
{
	RELEASE(logic_obj);
	logic_obj=pUObj;
}

cObject3dx* CTabDialog::GetRoot()
{
	if(UObj.empty())
		return NULL;
	return UObj.front();
}

void CTabDialog::ClearRoot()
{
	vector<cObject3dx*>::iterator it;
	FOR_EACH(UObj,it)
	if(*it)
		(*it)->Release();
	UObj.clear();

	RELEASE(logic_obj);
}

void CTabDialog::OnSize(UINT nType,int cx,int cy)
{
	CFormView::OnSize(nType,cx,cy);

	CRect rc_main,rc_list;
	GetClientRect(rc_main);
	if(treeList.m_hWnd)
	{
		GetRect(treeList,rc_list);
//		rc_list.top=20;
		rc_list.left=rc_main.left;
		rc_list.right=rc_main.right;
		rc_list.bottom=rc_main.bottom;
		SetPos(treeList,rc_list);
	}
}

void CTabDialog::SetHeadAnimationGroup()
{
	treeList.SetStyle
		( 0
//		| TLC_TREELIST								// TreeList or List
		//		| TLC_BKGNDIMAGE							// image background
		//		| TLC_BKGNDCOLOR							// colored background ( not client area )
		| TLC_DOUBLECOLOR							// double color background

		| TLC_MULTIPLESELECT						// single or multiple select
		| TLC_SHOWSELACTIVE							// show active column of selected item
		| TLC_SHOWSELALWAYS							// show selected item always
		//		| TLC_SHOWSELFULLROWS						// show selected item in fullrow mode

		| TLC_HEADER								// show header
		| TLC_HGRID									// show horizonal grid lines
		| TLC_VGRID									// show vertical grid lines
		| TLC_TGRID									// show tree horizonal grid lines ( when HGRID & VGRID )
		| TLC_HGRID_EXT								// show extention horizonal grid lines
		| TLC_VGRID_EXT								// show extention vertical grid lines
		| TLC_HGRID_FULL							// show full horizonal grid lines
		//		| TLC_READONLY								// read only

		| TLC_TREELINE								// show tree line
		| TLC_ROOTLINE								// show root line
		| TLC_BUTTON								// show expand/collapse button [+]
		//		| TLC_CHECKBOX								// show check box
		//		| TLC_LOCKBOX								// show lock box
		| TLC_HOTTRACK								// show hover text 

		| TLC_DROPTHIS								// drop self support
		| TLC_DROPROOT								// drop on root support

		//		| TLC_HEADDRAGDROP							// head drag drop
		//		| TLC_HEADFULLDRAG							// head funn drag
		| TLC_NOAUTOCHECK							// do NOT auto set checkbox of parent & child
		//		| TLC_NOAUTOLOCK							// do NOT auto set lockbox of parent & child
		);

	treeList.InsertColumn("Animation group", TLF_DEFAULT_LEFT, 100);
	treeList.InsertColumn("chain", TLF_DEFAULT_LEFT, 100);
	treeList.SetColumnModify(1, TLM_COMBO);
	treeList.setChangeItemLabelNotifyListener(this);
	treeList.setItemChangeNotifyListener(this);
}

void CTabDialog::UpdateAnimationGroup()
{
	treeList.DeleteAllItems();
	cObject3dx* p3dx=GetRoot();
	if(p3dx==NULL)
		return;

	int ag_size=p3dx->GetAnimationGroupNumber();
	for(int iag=0;iag<ag_size;iag++)
	{
		const char* ag_name=p3dx->GetAnimationGroupName(iag);
		treeList.InsertItem(iag,ag_name);
		int ichain=p3dx->GetAnimationGroupChain(iag);
		cAnimationChain* chain=p3dx->GetChain(ichain);

		treeList.SetItemText( iag,1,chain->name.c_str());
	}
}

bool CTabDialog::onBeginControl(CTreeListCtrl& source, 
						CHANGE_LABEL_NOTIFY_INFO* pclns)
{
	CComboBox* combo=(CComboBox*)pclns->pEditControl;
	if(!combo)
		return true;
	cObject3dx* p3dx=GetRoot();
	if(p3dx==NULL)
		return false;

	combo->ResetContent();
	int chain_size=p3dx->GetChainNumber();
	for(int ichain=0;ichain<chain_size;ichain++)
	{
		cAnimationChain* chain=p3dx->GetChain(ichain);
		combo->AddString(chain->name.c_str());

	}

	int ag_size=p3dx->GetAnimationGroupNumber();
	CString group_name=pclns->pItem->GetText(0);
	for(int iag=0;iag<ag_size;iag++)
	{
		const char* ag_name=p3dx->GetAnimationGroupName(iag);
		if(strcmp(ag_name,(LPCSTR)group_name)==0)
		{
			int ichain=p3dx->GetAnimationGroupChain(iag);
			combo->SetCurSel(ichain);
		}
	}

	return true;
}

bool CTabDialog::onEndLabelEdit(CTreeListCtrl& source, CHANGE_LABEL_NOTIFY_INFO* pclns)
{
	cObject3dx* p3dx=GetRoot();
	if(p3dx==NULL)
		return true;
	if(pclns->text[0]==0)
		return true;

	int ag_size=p3dx->GetAnimationGroupNumber();
	CString group_name=pclns->pItem->GetText(0);
	for(int iag=0;iag<ag_size;iag++)
	{
		const char* ag_name=p3dx->GetAnimationGroupName(iag);
		if(strcmp(ag_name,(LPCSTR)group_name)==0)
		{
			vector<cObject3dx*>::iterator ito;
			FOR_EACH(UObj,ito)
				(*ito)->SetAnimationGroupChain(iag,pclns->text);
			if(logic_obj)
				logic_obj->SetAnimationGroupChain(iag,pclns->text);
		}
	}
	return true;
}

void CTabDialog::OnCbnSelchangeVisibilityGroup()
{
	cObject3dx* p3dx=GetRoot();
	if(p3dx==NULL)
		return;

	int igroup=visibility_group.GetCurSel();

	if(igroup<0)
	{
		xassert(0);
		return;
	}

	CString name;
	visibility_group.GetLBText(igroup,name);
	vector<cObject3dx*>::iterator ito;
	FOR_EACH(UObj,ito)
		(*ito)->SetVisibilityGroup(name);
}

void CTabDialog::UpdateVisibilityGroup()
{
	visibility_group.ResetContent();
	cObject3dx* p3dx=GetRoot();
	if(p3dx==NULL)
		return;

	vector<cStaticVisibilityChainGroup*>& vg=p3dx->GetStatic()->visibility_groups;
	cStaticVisibilityChainGroup* select=p3dx->GetVisibilityGroup();
	for(int i=0;i<vg.size();i++)
	{
		cStaticVisibilityChainGroup* cur=vg[i];
		visibility_group.AddString(cur->name.c_str());
		if(cur==select)
			visibility_group.SetCurSel(i);
	}


}
void CTabDialog::OnCbnSelchangeChains()
{
	cObject3dx* p3dx=GetRoot();
	if(p3dx==NULL)
		return;

	int ichain=chains.GetCurSel();
	if(ichain<0)
	{
		xassert(0);
		return;
	}

	int ag_size=p3dx->GetAnimationGroupNumber();
	for(int iag=0;iag<ag_size;iag++)
	{
		p3dx->SetAnimationGroupChain(iag,ichain);
	}
	UpdateAnimationGroup();
	pView->UpdateFramePeriod();
}

void CTabDialog::UpdateAnimationGroupBox()
{
	chains.ResetContent();
	cObject3dx* p3dx=GetRoot();
	if(p3dx==NULL)
		return;

	int ch_size=p3dx->GetChainNumber();
	for(int ich=0;ich<ch_size;ich++)
	{
		cAnimationChain* cur=p3dx->GetChain(ich);
		chains.AddString(cur->name.c_str());
	}

	if(p3dx->GetAnimationGroupNumber()>0)
	{
		int ichain=p3dx->GetAnimationGroupChain(0);
		chains.SetCurSel(ichain);
	}
}
