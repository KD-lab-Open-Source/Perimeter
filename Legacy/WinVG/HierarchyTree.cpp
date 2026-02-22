// HierarchyTree.cpp : implementation file
//

#include "stdafx.h"
#include "WinVG.h"
#include "HierarchyTree.h"
#include "WinVGDoc.h"
#include "WinVGView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHierarchyTree

IMPLEMENT_DYNCREATE(CHierarchyTree, CTreeView)

CHierarchyTree::CHierarchyTree()
{
}

CHierarchyTree::~CHierarchyTree()
{
	ClearRoot();
}


BEGIN_MESSAGE_MAP(CHierarchyTree, CTreeView)
	//{{AFX_MSG_MAP(CHierarchyTree)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHierarchyTree diagnostics

#ifdef _DEBUG
void CHierarchyTree::AssertValid() const
{
	CTreeView::AssertValid();
}

void CHierarchyTree::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHierarchyTree message handlers

void BuildTree(CTreeCtrl &TreeCtrl,cIUnkClass *UObj,HTREEITEM hItem)
{
	if(UObj==0) return;
	char str[256]="\0";
	strcpy(str,UObj->GetName());

	cObjectNode* node=dynamic_cast<cObjectNode*>(UObj);
	if(node && node->IsGroup())
	{
		char strAnim[256]="\0";
		node->GetChannel(-1,strAnim);
		if(strAnim[0]) { strcat(str," \t "); strcat(str,strAnim); }
	}

	hItem=TreeCtrl.InsertItem(str,hItem);

	cIUnkClass *Current;
	for(Current=UObj->NextObject(0);Current;Current=UObj->NextObject(Current))
		BuildTree(TreeCtrl,Current,hItem);
	TreeCtrl.Expand(hItem,TVE_EXPAND);
}

void CHierarchyTree::TreeUpdate()
{
	CTreeCtrl &TreeCtrl=GetTreeCtrl();
	HTREEITEM hItemSelected=TreeCtrl.GetSelectedItem();
	char StrSelectObj[256]="";
	if(hItemSelected) GetStringObj(TreeCtrl.GetItemText(hItemSelected),StrSelectObj);
	TreeCtrl.DeleteAllItems();
	if(UObj.empty()) return;
	
	HTREEITEM hItemRoot=TreeCtrl.InsertItem(filename);
	BuildTree(TreeCtrl,UObj.front(),hItemRoot);
	TreeCtrl.Expand(hItemRoot,TVE_EXPAND);
	if(StrSelectObj[0])
	{
		hItemSelected=FindItem(StrSelectObj);
		if(hItemSelected)
			TreeCtrl.SelectItem(hItemSelected);
	}
}
HTREEITEM CHierarchyTree::FindItem(char *StrObj,HTREEITEM hItem)
{
	CTreeCtrl &TreeCtrl=GetTreeCtrl();
	if(hItem==0) hItem=TreeCtrl.GetNextItem(NULL,TVGN_ROOT);
	char str[256]="";
	GetStringObj(TreeCtrl.GetItemText(hItem),str);
	if(stricmp(str,StrObj)==0)
		return hItem;
	for(hItem=TreeCtrl.GetNextItem(hItem,TVGN_CHILD);hItem;hItem=TreeCtrl.GetNextItem(hItem,TVGN_NEXT))
	{
		HTREEITEM hItemFind;
		if(hItemFind=FindItem(StrObj,hItem))
			return hItemFind;
	}
	return 0;
}

void CHierarchyTree::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
	TreeUpdate();
}

void CHierarchyTree::GetStringObj(CString cstr,char *StrSelectObj)
{
	int finish=strlen(cstr);
	char *str=strstr(cstr," \t ");
	if(str) finish-=strlen(str);
	memcpy(StrSelectObj,cstr,finish);
	StrSelectObj[finish]=0;
}

void CHierarchyTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int flUpdate=0;
	char StrSelectObj[256];
	CTreeCtrl &TreeCtrl=GetTreeCtrl();
	HTREEITEM hItem=TreeCtrl.GetSelectedItem();
	GetStringObj(TreeCtrl.GetItemText(hItem),StrSelectObj);
	
	CWinVGDoc *pDoc=(CWinVGDoc*)GetDocument();
	if(pDoc==0) return;
	int CurrentChannel=-1;
	cIUnkClass *UObjH=0,*UObjL=0;
	char ChannelName[256];
	if(pDoc->m_pHierarchyObj&&!pDoc->m_pHierarchyObj->UObj.empty())
	{
		UObjH=dynamic_cast<cObjectNodeRoot*>(pDoc->m_pHierarchyObj->UObj.front())->FindObject(StrSelectObj);
		if(UObjH==0)
			UObjH=pDoc->m_pHierarchyObj->UObj.front();
		(dynamic_cast<cObjectNode*>(UObjH))->GetCurrentChannel(&CurrentChannel,ChannelName);
	}
	if(pDoc->m_pHierarchyLogic&&!pDoc->m_pHierarchyLogic->UObj.empty())
		UObjL=dynamic_cast<cLogicObject*>(pDoc->m_pHierarchyLogic->UObj.front())->FindObject(StrSelectObj);

	if(nChar=='A')
	{
		CurrentChannel--;
		if(UObjH)
		{
			cObjectNode* p=dynamic_cast<cObjectNode*>(UObjH);
			if(p->SetChannel(CurrentChannel,false))
				CurrentChannel++;
			else flUpdate=1;
		}

		gb_FrameWnd->UnknownSelect();
	}
	else if(nChar=='Z')
	{
		CurrentChannel++;
		if(UObjH)
		{
			cObjectNode* p=dynamic_cast<cObjectNode*>(UObjH);
			if(p->SetChannel(CurrentChannel,false))
				CurrentChannel--;
			else flUpdate=1;
		}
		gb_FrameWnd->UnknownSelect();
	}
	else
		if(pDoc->m_pView)
			pDoc->m_pView->OnKeyDown(nChar,nRepCnt,nFlags);

	if(UObjH)
		(dynamic_cast<cObjectNode*>(UObjH))->GetCurrentChannel(&CurrentChannel,ChannelName);
	if(UObjL)
		(dynamic_cast<cLogicObject*>(UObjL))->SetChannel(ChannelName,0.f);
	if(flUpdate) 
	{
		pDoc->m_pHierarchyObj->TreeUpdate();
		pDoc->m_pHierarchyLogic->TreeUpdate();
	}
	
//	CTreeView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CHierarchyTree::SetRoot(cIUnkClass* pUObj,CString _filename)
{
	ClearRoot();

	if(pUObj)
		UObj.push_back(pUObj);
	filename=_filename;
	filename.MakeUpper();
}

void CHierarchyTree::AddRoot(cIUnkClass* pUObj)
{
	if(pUObj)
		UObj.push_back(pUObj);
}

cIUnkClass* CHierarchyTree::GetRoot()
{
	if(UObj.empty())
		return NULL;
	return UObj.front();
}

void CHierarchyTree::ClearRoot()
{
	vector<cIUnkClass*>::iterator it;
	FOR_EACH(UObj,it)
	if(*it)
		(*it)->Release();
	UObj.clear();
}

void CHierarchyTree::SetSkinColor(sColor4f& c)
{
	vector<cIUnkClass*>::iterator it;
	FOR_EACH(UObj,it)
	if(*it)
		static_cast<cObjectNodeRoot*>(*it)->SetSkinColor(c);
}