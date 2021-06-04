#include "StdAfx.h"
#include "resource.h"
#include "AnimationGroup.h"

//осталось только переименование группы сделать.

DlgAnimationGroup::DlgAnimationGroup()
{
	hTreeObject=NULL;
}

int DlgAnimationGroup::DoModal(HWND hParent)
{
	return CDialog::DoModal(IDD_ANIMATION_GROUP,hParent);
}

LRESULT DlgAnimationGroup::WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
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
		case IDC_ADD_GROUP:
			OnAddGroup();
			break;
		case IDC_DELETE_GROUP:
			OnDeleteGroup();
			break;
		case IDC_ADD_OBJECT_IN_GROUP:
			OnAddObjectInGroup();
			break;
		case IDC_ADD_OBJECT_IN_GROUP_SUBTREE:
			OnAddObjectInGroupSubtree();
			break;
		}
		break;
	case WM_SIZE:
		OnSize();
		return true;
	case WM_CLOSE:
		EndDialog(hwnd,IDCANCEL);
		break;
	}
	return CDialog::WindowProc(uMsg,wParam,lParam);
}

void DlgAnimationGroup::OnInitDialog()
{
	RECT rc_main,rc;
	GetClientRect(hwnd,&rc_main);
	rc_main_base=rc_main;
	hTreeObject=GetDlgItem(hwnd,IDC_OBJECT_TREE);
	GetRect(hTreeObject,&rc);
	rcTreeObject.left=rc.left;
	rcTreeObject.right=rc_main.right-rc.right;
	rcTreeObject.top=rc.top;
	rcTreeObject.bottom=rc_main.bottom-rc.bottom;

	hTreeGroup=GetDlgItem(hwnd,IDC_TREE_GROUP);
	GetRect(hTreeGroup,&rc);
	rcTreeGroup.left=rc.left;
	rcTreeGroup.right=rc.right;
	rcTreeGroup.top=rc.top;
	rcTreeGroup.bottom=rc_main.bottom-rc.bottom;

	hGroupName=GetDlgItem(hwnd,IDC_GROUP_NAME);

	animation_group=pRootExport->animation_data.animation_group;
	BuildTreeObject();
	BuildTreeGroup();
}

void DlgAnimationGroup::OnOk()
{
	pRootExport->animation_data.animation_group=animation_group;
}

void DlgAnimationGroup::OnSize()
{
	RECT rc_main,rc;
	GetClientRect(hwnd,&rc_main);

	{
		GetRect(hTreeObject,&rc);
		rc.top=rcTreeObject.top;
		rc.bottom=rc_main.bottom-rcTreeObject.bottom;
		rc.right=rc_main.right-rcTreeObject.right;
		rc.left=rcTreeObject.left+(rc_main.right-rc_main_base.right)/2;

		SetPos(hTreeObject,rc);
	}

	{
		GetRect(hTreeGroup,&rc);
		rc.top=rcTreeGroup.top;
		rc.bottom=rc_main.bottom-rcTreeGroup.bottom;
		rc.right=rcTreeGroup.right+(rc_main.right-rc_main_base.right)/2;
		rc.left=rcTreeGroup.left;
		SetPos(hTreeGroup,rc);
	}
}

HTREEITEM InsertTree(HWND hTree,HTREEITEM hParent,LPCSTR name,DWORD lparam,int image)
{
	TVINSERTSTRUCT is;
	memset(&is,0,sizeof(is));
	is.hParent=hParent;
	is.hInsertAfter=TVI_LAST;
	is.item.mask=TVIF_PARAM|TVIF_TEXT;
	is.item.pszText=(LPSTR)name;
	is.item.lParam=lparam;
	if(image>=0)
	{
		is.item.mask|=TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		is.item.iImage=image;
		is.item.iSelectedImage=image;
	}
	HTREEITEM h=TreeView_InsertItem(hTree,&is);	
	return h;
}

char* GetTreeItemName(HWND hTree,HTREEITEM h)
{
	TVITEM item;
	static char name[256];
	name[0]=0;
	item.mask=TVIF_HANDLE|TVIF_TEXT;
	item.hItem=h;
	item.pszText=name;
	item.cchTextMax =sizeof(name);
	TreeView_GetItem(hTree,&item);
	return name;
}

DWORD GetTreeItemParam(HWND hTree,HTREEITEM h)
{
	TVITEM item;
	item.mask=TVIF_HANDLE|TVIF_PARAM;
	item.hItem=h;
	item.lParam=0;
	TreeView_GetItem(hTree,&item);
	return item.lParam;
}

void DlgAnimationGroup::BuildTreeObject()
{
	IGameScene * pIgame=pRootExport->pIgame;
	for(int loop = 0; loop <pIgame->GetTopLevelNodeCount();loop++)
	{
		IGameNode * pGameNode = pIgame->GetTopLevelNode(loop);
		AddTreeNode(pGameNode,NULL,TVI_ROOT);
	}
}
void DlgAnimationGroup::BuildTreeGroup()
{
	for(int igroup=0;igroup<animation_group.size();igroup++)
	{
		AnimationGroup& ag=animation_group[igroup];
		HTREEITEM hgroup=InsertTree(hTreeGroup,TVI_ROOT,ag.name.c_str());
		if(hgroup)
		{
			for(int iobject=0;iobject<ag.groups.size();iobject++)
			{
				string object_name=ag.groups[iobject]->GetName();

				InsertTree(hTreeGroup,hgroup,object_name.c_str());
				TreeView_Expand(hTreeGroup,hgroup,TVE_EXPAND);
			}
		}
	}
	UpdateObjectTree();
}

void DlgAnimationGroup::AddTreeNode(IGameNode* current,IGameNode* parent,HTREEITEM hparent)
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
}

void DlgAnimationGroup::OnAddGroup()
{
	char group_name[256];
	GetWindowText(hGroupName,group_name,sizeof(group_name)-1);
	if(group_name[0]==0)
		return;
	
	AddGroup(group_name);
}

HTREEITEM DlgAnimationGroup::AddGroup(string group_name)
{
	for(int i=0;i<animation_group.size();i++)
	{
		AnimationGroup& s=animation_group[i];
		if(s.name==group_name)
		{
			MessageBox(hwnd,"Такое имя группы уже существует","Error",MB_OK);
			return NULL;
		}
	}

	AnimationGroup group;
	group.name=group_name;
	animation_group.push_back(group);
	return InsertTree(hTreeGroup,TVI_ROOT,group_name.c_str());
}

void DlgAnimationGroup::OnDeleteGroup()
{
	HTREEITEM hGroup=GetSelectionGroup();
	if(hGroup==NULL)
		return;

	string name=GetTreeItemName(hTreeGroup,hGroup);
	for(int igroup=0;igroup<animation_group.size();igroup++)
	{
		AnimationGroup& ag=animation_group[igroup];
		if(name==ag.name)
		{
			TreeView_DeleteItem(hTreeGroup,hGroup);
			animation_group.erase(animation_group.begin()+igroup);
			UpdateObjectTree();
			return;
		}
	}

	assert(0 && "Group not found");
}

HTREEITEM DlgAnimationGroup::GetSelectionGroup()
{
	HTREEITEM hGroup=TreeView_GetSelection(hTreeGroup);
	if(hGroup==NULL)
	{
		MessageBox(hwnd,"Группа не выделена","Error",MB_OK);
		return NULL;
	}

	return hGroup;
}

void DlgAnimationGroup::OnAddObjectInGroup()
{
	HTREEITEM hObject=TreeView_GetSelection(hTreeObject);
	if(hObject==NULL)
	{
		MessageBox(hwnd,"Объект не выделен","Error",MB_OK);
		return;
	}

	HTREEITEM hGroup=GetSelectionGroup();
	if(hGroup==NULL)
		return;

	AddObjectInGroup(hGroup,hObject);
	UpdateObjectTree();
}

void DlgAnimationGroup::OnAddObjectInGroupSubtree()
{
	HTREEITEM hObject=TreeView_GetSelection(hTreeObject);
	if(hObject==NULL)
	{
		MessageBox(hwnd,"Объект не выделен","Error",MB_OK);
		return;
	}

	HTREEITEM hGroup=GetSelectionGroup();
	if(hGroup==NULL)
		return;

	AddObjectInGroupRecursive(hGroup,hObject);
	UpdateObjectTree();
}

void DlgAnimationGroup::AddObjectInGroupRecursive(HTREEITEM hGroup,HTREEITEM hObject)
{
	AddObjectInGroup(hGroup,hObject);
	for(HTREEITEM hcur=TreeView_GetChild(hTreeObject,hObject);hcur;
		hcur=TreeView_GetNextItem(hTreeObject,hcur,TVGN_NEXT))
	{
		AddObjectInGroupRecursive(hGroup,hcur);
	}
}

void DlgAnimationGroup::AddObjectInGroup(HTREEITEM hGroup,HTREEITEM hObject)
{
	IGameNode* pGameNode=(IGameNode*)GetTreeItemParam(hTreeObject,hObject);
	AddObjectInGroup(hGroup,pGameNode);
}

void DlgAnimationGroup::AddObjectInGroup(HTREEITEM hGroup,IGameNode* pGameNode)
{
	{//Добраться до root
		HTREEITEM hParent=hGroup;
		do
		{
			hGroup=hParent;
			hParent=TreeView_GetParent(hTreeGroup,hParent);
		}while(hParent!=NULL);
	}

	//
	string group_name=GetTreeItemName(hTreeGroup,hGroup);
	string object_name=pGameNode->GetName();
	for(int igroup=0;igroup<animation_group.size();igroup++)
	{
		AnimationGroup& ag=animation_group[igroup];
		if(ag.name==group_name)
		{
			bool found=false;
			for(int iobj=0;iobj<ag.groups.size();iobj++)
			{
				if(ag.groups[iobj]==pGameNode)
				{
					found=true;
					break;
				}
			}

			if(!found)
			{
				InsertTree(hTreeGroup,hGroup,object_name.c_str());
				ag.groups.push_back(pGameNode);
				TreeView_Expand(hTreeGroup,hGroup,TVE_EXPAND);
			}

			break;
		}
	}

	FindAndDeleteGroupObject(pGameNode,group_name);
}

void DlgAnimationGroup::FindAndDeleteGroupObject(IGameNode* pGameNode,string exclude_group_name)
{
	for(int igroup=0;igroup<animation_group.size();igroup++)
	{
		AnimationGroup& ag=animation_group[igroup];
		if(ag.name==exclude_group_name)
			continue;
		for(int iobj=0;iobj<ag.groups.size();iobj++)
		{
			if(ag.groups[iobj]==pGameNode)
			{
				DeleteGroupObject(pGameNode,ag.name);
			}
		}
	}
}

bool DlgAnimationGroup::DeleteGroupObject(IGameNode* pGameNode,string group_name_)
{
	string object_name=pGameNode->GetName();
	for(HTREEITEM hgroup=TreeView_GetChild(hTreeGroup,TVI_ROOT);hgroup;
		hgroup=TreeView_GetNextItem(hTreeGroup,hgroup,TVGN_NEXT))
	{
		string group_name=GetTreeItemName(hTreeGroup,hgroup);
		if(group_name==group_name_)
		{
			for(HTREEITEM hobject=TreeView_GetChild(hTreeGroup,hgroup);hobject;
				hobject=TreeView_GetNextItem(hTreeGroup,hobject,TVGN_NEXT))
			{
				string obj_name=GetTreeItemName(hTreeGroup,hobject);
				if(object_name==obj_name)
				{
					BOOL b=TreeView_DeleteItem(hTreeGroup,hobject);
					xassert(b);
					return b;
				}
			}
			break;
		}
	}

	return false;
}

void DlgAnimationGroup::UpdateObjectTree(HTREEITEM hparent)
{
	if(hparent!=TVI_ROOT)
	{
		IGameNode* pGameNode=(IGameNode*)GetTreeItemParam(hTreeObject,hparent);
		string group_name;

		for(int igroup=0;igroup<animation_group.size();igroup++)
		{
			AnimationGroup& ag=animation_group[igroup];
			for(int iobject=0;iobject<ag.groups.size();iobject++)
			{
				if(ag.groups[iobject]==pGameNode)
				{
					group_name=ag.name;
				}
			}

		}

		string name=pGameNode->GetName();

		if(!group_name.empty())
		{
			name+=" (";
			name+=group_name;
			name+=")";
		}

		TVITEM item;
		item.mask=TVIF_HANDLE|TVIF_TEXT;
		item.hItem=hparent;
		item.pszText=(char*)name.c_str();
		item.cchTextMax =name.size();
		TreeView_SetItem(hTreeObject,&item);
	}

	for(HTREEITEM hcur=TreeView_GetChild(hTreeObject,hparent);hcur;
		hcur=TreeView_GetNextItem(hTreeObject,hcur,TVGN_NEXT))
	{
		UpdateObjectTree(hcur);
	}
}
