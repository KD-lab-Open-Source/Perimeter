#include "StdAfx.h"
#include "AnimationChain.h"
#include "AddAnimationChain.h"
#include "AddChainGroup.h"
#include "VisibilityDlg.h"
#include "resource.h"

DlgAnimationChain::DlgAnimationChain()
{
}

int DlgAnimationChain::DoModal(HWND hParent)
{
	return CDialog::DoModal(IDD_ANIMATION_CHAIN,hParent);
}

LRESULT DlgAnimationChain::WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
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
		case IDC_ADD_CHAIN:
			OnAddChain();
			break;
		case IDC_DELETE_CHAIN:
			OnDeleteChain();
			break;
		case IDC_ADD_CHAIN_GROUP:
			OnAddChainGroup();
			break;
		case IDC_DELETE_CHAIN_GROUP:
			OnDeleteChainGroup();
			break;
		case IDC_EDIT_VISIBILITY:
			OnEditVisibility();
			break;
		}
		break;
	case WM_SIZE:
		OnSize();
		return true;
	case WM_CLOSE:
		EndDialog(hwnd,IDCANCEL);
		break;

	case WM_NOTIFY:
		{
			if(wParam==IDC_LIST_CHAIN)
			{
				LPNMHDR pnm=(LPNMHDR)lParam;
				switch(pnm->code)
				{
					case NM_DBLCLK:
						OnEditChain((LPNMITEMACTIVATE)pnm);
						break;
					case LVN_KEYDOWN:
						OnKeyChain((LPNMLVKEYDOWN)pnm);
						break;
				}
			}

			if(wParam==IDC_CHAIN_GROUP)
			{
				LPNMHDR pnm=(LPNMHDR)lParam;
				switch(pnm->code)
				{
					case NM_DBLCLK:
						OnEditVisibility();
//						OnEditChainGroup((LPNMITEMACTIVATE)pnm);
						break;
					case TVN_KEYDOWN:
						OnKeyChainGroup((LPNMTVKEYDOWN)pnm);
						break;
				}
			}
		}
		break;
	}
	return CDialog::WindowProc(uMsg,wParam,lParam);
}

void DlgAnimationChain::OnInitDialog()
{
	RECT rc_main,rc;
	GetClientRect(hwnd,&rc_main);
	rc_main_base=rc_main;
	hTreeGroup=GetDlgItem(hwnd,IDC_CHAIN_GROUP);
	GetRect(hTreeGroup,&rc);
	rcTreeGroup.left=rc.left;
	rcTreeGroup.right=rc_main.right-rc.right;
	rcTreeGroup.top=rc.top;
	rcTreeGroup.bottom=rc_main.bottom-rc.bottom;

	hListChain=GetDlgItem(hwnd,IDC_LIST_CHAIN);
	GetRect(hListChain,&rc);
	rcListChain.left=rc.left;
	rcListChain.right=rc.right;
	rcListChain.top=rc.top;
	rcListChain.bottom=rc_main.bottom-rc.bottom;

	{
		LVCOLUMN col;
		col.mask=LVCF_TEXT|LVCF_WIDTH;
		col.fmt=LVCFMT_RIGHT;
		col.cx=75;
		col.cchTextMax=1;
		col.iSubItem=0;
		col.iImage=0;
		col.iOrder=0;

		col.pszText="name";
		ListView_InsertColumn(hListChain,LS_NAME,&col);

		col.pszText="begin frame";
		ListView_InsertColumn(hListChain,LS_BEGIN,&col);

		col.pszText="end frame";
		ListView_InsertColumn(hListChain,LS_END,&col);

		col.pszText="time";
		ListView_InsertColumn(hListChain,LS_TIME,&col);

		col.pszText="cycled";
		ListView_InsertColumn(hListChain,LS_CYCLED,&col);
	}

	animation_chain=pRootExport->animation_data.animation_chain;
	animation_chain_group=pRootExport->animation_data.animation_chain_group;

	for(int i=0;i<animation_chain.size();i++)
	{
		SetItem(i,true);
	}

	//fill tree
	for(int igroup=0;igroup<animation_chain_group.size();igroup++)
	{
		AnimationVisibleGroup& ac=animation_chain_group[igroup];
		HTREEITEM hgroup=InsertTree(hTreeGroup,TVI_ROOT,ac.name.c_str());
	}

}

void DlgAnimationChain::OnOk()
{
	pRootExport->animation_data.animation_chain=animation_chain;
	pRootExport->animation_data.animation_chain_group=animation_chain_group;
}

void DlgAnimationChain::OnSize()
{
	RECT rc_main,rc;
	GetClientRect(hwnd,&rc_main);

	{
		GetRect(hTreeGroup,&rc);
		rc.top=rcTreeGroup.top;
		rc.bottom=rc_main.bottom-rcTreeGroup.bottom;
		rc.right=rc_main.right-rcTreeGroup.right;
		rc.left=rcTreeGroup.left+(rc_main.right-rc_main_base.right)/2;

		SetPos(hTreeGroup,rc);
	}

	{
		GetRect(hListChain,&rc);
		rc.top=rcListChain.top;
		rc.bottom=rc_main.bottom-rcListChain.bottom;
		rc.right=rcListChain.right+(rc_main.right-rc_main_base.right)/2;
		rc.left=rcListChain.left;
		SetPos(hListChain,rc);
	}
}

void DlgAnimationChain::OnAddChain()
{
	DlgAddAnimationChain dlg;
	if(dlg.DoModal(hwnd)!=IDOK)
		return;

	for(int ichain=0;ichain<animation_chain.size();ichain++)
	{
		AnimationChain& ac=animation_chain[ichain];
		if(ac.name==dlg.name)
		{
			MessageBox(hwnd,"Такая анимационная цепочка уже существует","Error",MB_OK);
			return;
		}
	}

	AnimationChain ac;
	ac.name=dlg.name;
	ac.begin_frame=dlg.begin_frame;
	ac.end_frame=dlg.end_frame;
	ac.time=dlg.delta_time;
	ac.cycled=dlg.cycled;
	animation_chain.push_back(ac);
	SetItem(animation_chain.size()-1,true);
}

void DlgAnimationChain::SetItem(int index,bool insert)
{
	xassert(index>=0 && index<animation_chain.size());
	AnimationChain& ac=animation_chain[index];
	LVITEM item;
	item.mask=LVIF_TEXT;
	item.state=0;
	item.stateMask=0;
	item.iItem=index;
	item.iSubItem=0;
	item.pszText=(char*)ac.name.c_str();

	if(insert)
	{
		int id=ListView_InsertItem(hListChain,&item);
		xassert(index==id);
	}else
	{
		ListView_SetItem(hListChain,&item);
	}

	char buf[32];

	sprintf(buf,"%i",ac.begin_frame);
	ListView_SetItemText(hListChain,index,LS_BEGIN,buf);

	sprintf(buf,"%i",ac.end_frame);
	ListView_SetItemText(hListChain,index,LS_END,buf);

	sprintf(buf,"%.2f",ac.time);
	ListView_SetItemText(hListChain,index,LS_TIME,buf);

	ListView_SetItemText(hListChain,index,LS_CYCLED,ac.cycled?"cycled":"");
}

void DlgAnimationChain::OnDeleteChain()
{
	int mark=ListView_GetSelectionMark(hListChain);
	if(mark<0 || mark>=animation_chain.size())
		return;

	ListView_DeleteItem(hListChain,mark);
	animation_chain.erase(animation_chain.begin()+mark);
}

void DlgAnimationChain::OnEditChain(LPNMITEMACTIVATE pnm)
{
	if(pnm->iItem<0 || pnm->iItem>=animation_chain.size())
		return;
	
	AnimationChain& ac=animation_chain[pnm->iItem];

	DlgAddAnimationChain dlg;
	dlg.name=ac.name;
	dlg.begin_frame=ac.begin_frame;
	dlg.end_frame=ac.end_frame;
	dlg.delta_time=ac.time;
	dlg.cycled=ac.cycled;

	if(dlg.DoModal(hwnd)!=IDOK)
		return;

	ac.name=dlg.name;
	ac.begin_frame=dlg.begin_frame;
	ac.end_frame=dlg.end_frame;
	ac.time=dlg.delta_time;
	ac.cycled=dlg.cycled;
	SetItem(pnm->iItem,false);
}

void DlgAnimationChain::OnKeyChain(LPNMLVKEYDOWN pnm)
{
	if(pnm->wVKey==VK_DELETE)
	{
		OnDeleteChain();
		return;
	}
}

void DlgAnimationChain::OnAddChainGroup()
{
	DlgAddChainGroup dlg;
	dlg.name="DialogGroup";
	if(dlg.DoModal(hwnd)!=IDOK)
		return;

	HTREEITEM hfound=DlgAnimationChain::FindChainGroup(dlg.name.c_str());

	if(hfound)
	{
		MessageBox(hwnd,"Такая группа анимаций уже существует","Error",MB_OK);
		return;
	}

	AnimationVisibleGroup ac;
	ac.name=dlg.name;
	animation_chain_group.push_back(ac);
	HTREEITEM hitem=InsertTree(hTreeGroup,TVI_ROOT,dlg.name.c_str());
	TreeView_Expand(hTreeGroup,hitem,TVE_EXPAND);
}

void DlgAnimationChain::OnDeleteChainGroup()
{
	HTREEITEM hcur=TreeView_GetSelection(hTreeGroup);
	HTREEITEM hParent=NULL;
	if(hcur)
		hParent=TreeView_GetParent(hTreeGroup,hcur);
	if(hParent)
	{//delete chain
	}else
	{//delete group
		int ifound=0;
		HTREEITEM hgroup=GetSelectionGroup(&ifound);
		if(hgroup==NULL)
			return;

		animation_chain_group.erase(animation_chain_group.begin()+ifound);
		BOOL b=TreeView_DeleteItem(hTreeGroup,hgroup);
		xassert(b);
	}
}

HTREEITEM DlgAnimationChain::FindChainGroup(const char* name,int* index)
{
	HTREEITEM hitem=NULL;
	for(HTREEITEM hcur=TreeView_GetChild(hTreeGroup,TVI_ROOT);hcur;
		hcur=TreeView_GetNextItem(hTreeGroup,hcur,TVGN_NEXT))
	{
		char* item_name=GetTreeItemName(hTreeGroup,hcur);
		if(stricmp(name,item_name)==0)
		{
			hitem=hcur;
			break;
		}
	}

	int ifound=-1;
	for(int i=0;i<animation_chain_group.size();i++)
	{
		AnimationVisibleGroup& ac=animation_chain_group[i];
		if(ac.name==name)
		{
			ifound=i;
			break;
		}
	}

	if(hitem)
	{
		xassert(ifound>=0);
	}else
	{
		xassert(ifound==-1);
	}

	if(index)
	{
		*index=ifound;
	}

	return hitem;
}

HTREEITEM DlgAnimationChain::GetSelectionGroup(int* index)
{
	HTREEITEM hgroup=TreeView_GetSelection(hTreeGroup);
	if(hgroup==NULL)
	{
		MessageBox(hwnd,"Группа не выделена","Error",MB_OK);
		if(index)
			*index=NULL;
		return NULL;
	}

	HTREEITEM hParent=TreeView_GetParent(hTreeGroup,hgroup);
	if(hParent)
		hgroup=hParent;

	char* name=GetTreeItemName(hTreeGroup,hgroup);
	int ifound=-1;
	for(int i=0;i<animation_chain_group.size();i++)
	{
		AnimationVisibleGroup& ac=animation_chain_group[i];
		if(ac.name==name)
		{
			ifound=i;
			break;
		}
	}

	xassert(ifound>=0);
	if(index)
		*index=ifound;
	return hgroup;
}

void DlgAnimationChain::OnEditChainGroup(LPNMITEMACTIVATE pnm)
{
	int ifound=0;
	HTREEITEM hgroup=GetSelectionGroup(&ifound);
	if(hgroup==NULL)
		return;

	HTREEITEM hParent=TreeView_GetParent(hTreeGroup,hgroup);
	if(hParent)
		return;

	DlgAddChainGroup dlg;
	dlg.name=animation_chain_group[ifound].name;
	if(dlg.DoModal(hwnd)!=IDOK)
		return;

	animation_chain_group[ifound].name=dlg.name;

	TVITEMEX item;
	item.mask=TVIF_HANDLE|TVIF_TEXT;
	item.hItem=hgroup;
	item.pszText=(char*)dlg.name.c_str();
	item.cchTextMax=1;
	TreeView_SetItem(hTreeGroup,&item);
}

void DlgAnimationChain::OnKeyChainGroup(LPNMTVKEYDOWN pnm)
{
	if(pnm->wVKey==VK_DELETE)
	{
		OnDeleteChainGroup();
		return;
	}
}

void DlgAnimationChain::OnEditVisibility()
{
	int ifound=0;
	HTREEITEM hgroup=GetSelectionGroup(&ifound);
	if(hgroup==NULL)
		return;

	AnimationVisibleGroup& ag=animation_chain_group[ifound];
	VisibilityDlg dlg;
	dlg.chain_group=ag;
	if(dlg.DoModal(hwnd)==IDOK)
	{
		ag=dlg.chain_group;
		TVITEMEX item;
		item.mask=TVIF_HANDLE|TVIF_TEXT;
		item.hItem=hgroup;
		item.pszText=(char*)ag.name.c_str();
		item.cchTextMax=1;
		TreeView_SetItem(hTreeGroup,&item);
	}
}