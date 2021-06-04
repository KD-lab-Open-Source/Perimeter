#include "StdAfx.h"
#include "VisibilityDlg.h"
#include "resource.h"

VisibilityDlg::VisibilityDlg()
{
}

int VisibilityDlg::DoModal(HWND hParent)
{
	return CDialog::DoModal(IDD_VISIBILITY,hParent);
}

LRESULT VisibilityDlg::WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			OnOk();
			EndDialog(hwnd,IDOK);
			break;
		}
		break;
	case WM_SIZE:
		OnSize();
		return true;
	case WM_CLOSE:
		EndDialog(hwnd,IDCANCEL);
		break;
	case WM_TIMER:
		BuildTreeObject();
		KillTimer(hwnd,1);
		break;
	}
	return CDialog::WindowProc(uMsg,wParam,lParam);
}

void VisibilityDlg::OnInitDialog()
{
	RECT rc_main;
	GetClientRect(hwnd,&rc_main);
	rc_main_base=rc_main;
	hTreeObject=GetDlgItem(hwnd,IDC_TREE_VISIBLE);
	LinkLRUB(hTreeObject,rcTree);
	hOk=GetDlgItem(hwnd,IDOK);
	LinkRight(hOk,rcOk);

	hName=GetDlgItem(hwnd,IDC_VISIBLE_NAME);
	SetWindowText(hName,chain_group.name.c_str());

	SetTimer(hwnd,1,10,NULL);
}

void VisibilityDlg::OnSize()
{
	RECT rc_main;
	GetClientRect(hwnd,&rc_main);

	MoveLRUB(hTreeObject,rcTree,rc_main);
	MoveRight(hOk,rcOk,rc_main);
}

void VisibilityDlg::OnOk()
{
	chain_group.invisible_object.clear();
	SaveObjectTree();

	char sname[256];
	GetWindowText(hName,sname,sizeof(sname));
	chain_group.name=sname;
}

void VisibilityDlg::AddTreeNode(IGameNode* current,IGameNode* parent,HTREEITEM hparent)
{
	if(current->IsTarget())
		return;
	char* name=current->GetName();
	HTREEITEM hcurrent=InsertTree(hTreeObject,hparent,name,(DWORD)current);

	for(int i=0;i<current->GetChildCount();i++)
	{
		IGameNode* node=current->GetNodeChild(i);
		AddTreeNode(node,current,hcurrent);
	}

	TreeView_Expand(hTreeObject,hcurrent,TVE_EXPAND);

	vector<IGameNode*>& invisible=chain_group.invisible_object;
	bool is_visible=true;
	for(i=0;i<invisible.size();i++)
	{
		if(invisible[i]==current)
		{
			is_visible=false;
			break;
		}
	}
	TreeView_SetCheckState(hTreeObject,hcurrent,is_visible);
}

void VisibilityDlg::BuildTreeObject()
{
	IGameScene * pIgame=pRootExport->pIgame;
	for(int loop = 0; loop <pIgame->GetTopLevelNodeCount();loop++)
	{
		IGameNode * pGameNode = pIgame->GetTopLevelNode(loop);
		AddTreeNode(pGameNode,NULL,TVI_ROOT);
	}
}

void VisibilityDlg::SaveObjectTree(HTREEITEM hparent)
{
	if(hparent!=TVI_ROOT)
	{
		IGameNode* pGameNode=(IGameNode*)GetTreeItemParam(hTreeObject,hparent);
		string name=pGameNode->GetName();
		int checked=TreeView_GetCheckState(hTreeObject,hparent);

		if(!checked)
			chain_group.invisible_object.push_back(pGameNode);
	}

	for(HTREEITEM hcur=TreeView_GetChild(hTreeObject,hparent);hcur;
		hcur=TreeView_GetNextItem(hTreeObject,hcur,TVGN_NEXT))
	{
		SaveObjectTree(hcur);
	}
}
