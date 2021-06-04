// EffectTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "EffectTool.h"
#include "EffectTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CEffectTreeView

IMPLEMENT_DYNCREATE(CEffectTreeView, CTreeView)

CEffectTreeView::CEffectTreeView()
{
	m_bDragging = false;
	m_hitemDrag = 0;
	m_hitemDrop = 0;
	m_pimagelist = 0;
}

CEffectTreeView::~CEffectTreeView()
{
}
bool bFillingTree = false;
void CEffectTreeView::PopulateTree()
{
	bFillingTree = true;
	CEffectToolDoc* pDoc = GetDocument();
	CTreeCtrl& tree = GetTreeCtrl();

	tree.DeleteAllItems();

	HTREEITEM first = NULL;

	for(int i=0; i<pDoc->m_group.GetSize(); i++)
	{
		// add group
		HTREEITEM hGroupItem = tree.InsertItem(((CGroupDescr*)pDoc->m_group[i])->m_name, 2, 2);
		if(!first)
			first = hGroupItem;
		tree.SetItemData(hGroupItem, (DWORD)pDoc->m_group[i]);
		tree.SetItemState(hGroupItem, 0, TVIS_STATEIMAGEMASK); //no checkbox
		pDoc->m_pActiveGroup = (CGroupDescr*)pDoc->m_group[i];

		EffectStorageType::iterator it;
		EffectStorageType& effects = ((CGroupDescr*)pDoc->m_group[i])->m_effects;
		FOR_EACH(effects, it)
		{
			// add effect
			HTREEITEM hEffectItem = tree.InsertItem((*it)->name.c_str(), 0, 0, hGroupItem);
			tree.SetItemData(hEffectItem, (DWORD)*it);
			tree.SetItemState(hEffectItem, 0, TVIS_STATEIMAGEMASK); //no checkbox

			GetDocument()->m_pActiveEffect = *it;
			
			EmitterListType::iterator i_e;
			FOR_EACH((*it)->emitters, i_e)
			{
				// add emitter
				HTREEITEM h = NULL;
				if((*i_e)->data)
					h = tree.InsertItem((*i_e)->data->name.c_str(), 1, 1, hEffectItem);
				else
					h = tree.InsertItem((*i_e)->data_light->name.c_str(), 1, 1, hEffectItem);

				tree.SetItemData(h, (DWORD)*i_e);
				tree.SetCheck(h);

				GetDocument()->m_pActiveEmitter = (CEmitterData*)*i_e;
			}
			if((*it)->bExpand)
				tree.Expand(hEffectItem, TVE_EXPAND);
		}
		if(((CGroupDescr*)pDoc->m_group[i])->m_bExpand)
			tree.Expand(hGroupItem, TVE_EXPAND);
	}
	if(first)
		tree.EnsureVisible(first);
	bFillingTree = false;
}
void CEffectTreeView::GetCheckState()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItemGroup = tree.GetNextItem(TVI_ROOT, TVGN_CHILD);
	while(hItemGroup)
	{
		HTREEITEM hItemEffect = tree.GetNextItem(hItemGroup, TVGN_CHILD);
		while(hItemEffect)
		{
			HTREEITEM hItemEmitter = tree.GetNextItem(hItemEffect, TVGN_CHILD);
			while(hItemEmitter)
			{
				CEmitterData* p = (CEmitterData*)tree.GetItemData(hItemEmitter);
				p->bActive = tree.GetCheck(hItemEmitter);

				hItemEmitter = tree.GetNextItem(hItemEmitter, TVGN_NEXT);
			}
			hItemEffect = tree.GetNextItem(hItemEffect, TVGN_NEXT);
		}
		hItemGroup = tree.GetNextItem(hItemGroup, TVGN_NEXT);
	}

	GetDocument()->UpdateAllViews(0);
}
void CEffectTreeView::CloneEffect(HTREEITEM hItemDrag, HTREEITEM hItemDrop)
{
	CTreeCtrl& tree = GetTreeCtrl();

	HTREEITEM hItemGroup = NULL;
	int nImage, nSelectedImage;
	int img = tree.GetItemImage( hItemDrop, nImage, nSelectedImage );

	switch(nImage)
	{
	case 2://group
		hItemGroup = hItemDrop;
		break;
	case 0://effect
		hItemGroup = tree.GetParentItem(hItemDrop);
		break;
	case 1://emitter
		hItemGroup = tree.GetParentItem(tree.GetParentItem(hItemDrop));
		break;
	default:
		MessageBeep(0);
		return;
		break;
	}

	if(hItemGroup!=tree.GetParentItem(hItemDrag))
		return;

	CGroupDescr* group = (CGroupDescr*)tree.GetItemData(hItemGroup);
	CEffectData* p = (CEffectData*)tree.GetItemData(hItemDrag);
	group->m_effects.push_back(p = new CEffectData(p));
	// Check Name

	//
	HTREEITEM hItem = tree.InsertItem(p->name.c_str(), 0, 0, hItemGroup);
	tree.SetItemData(hItem, (DWORD)p);
	tree.SetItemState(hItem, 0, TVIS_STATEIMAGEMASK); //no checkbox
	tree.SelectItem(hItem);

	EmitterListType::iterator i_e;
	FOR_EACH(p->emitters, i_e)
	{
		CString tmp;
		if((*i_e)->data)
			tmp = (*i_e)->data->name.c_str();
		else
			tmp = (*i_e)->data->name.c_str();

		HTREEITEM h = tree.InsertItem(tmp, 1, 1, hItem);
		tree.SetItemData(h, (DWORD)*i_e);
		tree.SetCheck(h);
	}

	tree.Expand(hItem, TVE_EXPAND);
}
void CEffectTreeView::CopyEmitter(HTREEITEM hItemFrom, HTREEITEM hItemTo)
{
	CTreeCtrl& tree = GetTreeCtrl();

	CEmitterData* pEmitter = (CEmitterData*)tree.GetItemData(hItemFrom);

	HTREEITEM hItemEffect = NULL;
	int nImage, nSelectedImage;
	int img = tree.GetItemImage( hItemTo, nImage, nSelectedImage );

	switch(nImage)
	{
	case 2://group
		hItemEffect = tree.GetChildItem(hItemTo);
		break;
	case 0://effect
		hItemEffect = hItemTo;
		break;
	case 1://emitter
		hItemEffect = tree.GetParentItem(hItemTo);
		break;
	default:
		MessageBeep(0);
		return;
		break;
	}

	CEffectData* pEffect = (CEffectData*)tree.GetItemData(hItemEffect);
	if(pEmitter->data)
		pEffect->emitters.push_back(pEmitter = new CEmitterData(pEmitter->data));
	else
		pEffect->emitters.push_back(pEmitter = new CEmitterData(pEmitter->data_light));

	// Check name
	CString name;
	if(pEmitter->data)
		name = pEmitter->data->name.c_str();
	else
		name = pEmitter->data_light->name.c_str();

	int i = 1;
	while(!pEffect->CheckName(name, pEmitter))
	{
		CString new_name;
		new_name.Format("%s %d", name, i);
		name = new_name;
		i++;
	}
	
	if(pEmitter->data)
		pEmitter->data->name = name;
	else
		pEmitter->data_light->name = name;

	//
	if(pEmitter->data)
		name = pEmitter->data->name.c_str();
	else
		name = pEmitter->data_light->name.c_str();

	HTREEITEM h = tree.InsertItem(name, 1, 1, hItemEffect);
	tree.SetItemData(h, (DWORD)pEmitter);
	tree.SetCheck(h);
}

void CEffectTreeView::MoveEmitter(HTREEITEM hItemFrom, HTREEITEM hItemTo)
{
	CTreeCtrl& tree = GetTreeCtrl();
//	CEmitterData* pEmitter = (CEmitterData*)tree.GetItemData(hItemFrom);

	HTREEITEM hItemParent = tree.GetParentItem(hItemTo);
	if(hItemParent!=tree.GetParentItem(hItemFrom))
		return;

	CEffectData* pEffect = (CEffectData*)tree.GetItemData(hItemParent);

	CEmitterData* p1 = (CEmitterData*)tree.GetItemData(hItemFrom);
	CEmitterData* p2 = (CEmitterData*)tree.GetItemData(hItemTo);

	pEffect->swap_emitters(p1, p2);

	tree.DeleteItem(hItemFrom);

	CString name;
	if(p1->data)
		name = p1->data->name.c_str();
	else
		name = p1->data_light->name.c_str();

	HTREEITEM h = tree.InsertItem(name, 1, 1, hItemParent, hItemTo);
	tree.SetItemData(h, (DWORD)p1);
	tree.SetCheck(h);
}


BEGIN_MESSAGE_MAP(CEffectTreeView, CTreeView)
	//{{AFX_MSG_MAP(CEffectTreeView)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
//	ON_UPDATE_COMMAND_UI(ID_EFFECT_DEL, OnUpdateEffectDel)
//	ON_UPDATE_COMMAND_UI(ID_EFFECT_EMITTER_DEL, OnUpdateEffectEmitterDel)
	ON_UPDATE_COMMAND_UI(ID_EFFECT_EMITTER_NEW, OnUpdateEffectEmitterNew)
	ON_UPDATE_COMMAND_UI(ID_EFFECT_NEW, OnUpdateEffectNew)
	ON_COMMAND(ID_EFFECT_NEW, OnEffectNew)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
//	ON_COMMAND(ID_EFFECT_DEL, OnEffectDel)
	ON_COMMAND(ID_EFFECT_EMITTER_NEW, OnEffectEmitterNew)
//	ON_COMMAND(ID_EFFECT_EMITTER_DEL, OnEffectEmitterDel)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_GROUP_NEW, OnUpdateGroupNew)
	ON_COMMAND(ID_GROUP_NEW, OnGroupNew)
	ON_COMMAND(ID_ELEMENT_DELETE, OnElementDelete)
	ON_UPDATE_COMMAND_UI(ID_ELEMENT_DELETE, OnUpdateElementDelete)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnTvnItemexpanded)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//

void CEffectTreeView::OnDraw(CDC* pDC)
{
}
/////////////////////////////////////////////////////////////////////////////
// CEffectTreeView message handlers

void CEffectTreeView::OnDestroy() 
{
	delete GetTreeCtrl().GetImageList(TVSIL_NORMAL);

	CTreeView::OnDestroy();
}

void CEffectTreeView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();

	GetTreeCtrl().ModifyStyle(0, TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS|TVS_CHECKBOXES|TVS_SHOWSELALWAYS);

	CImageList* pImageList = new CImageList;
	pImageList->Create(IDB_TREE, 16, 0, RGB(255, 0, 0));
	GetTreeCtrl().SetImageList(pImageList, TVSIL_NORMAL);

	PopulateTree();
}
#include "OptTree.h"
extern COptTree* tr;
void CEffectTreeView::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	*pResult = 1;

	if(pTVDispInfo->item.pszText)
	{
		CTreeCtrl& tree = GetTreeCtrl();

		HTREEITEM hItem = tree.GetSelectedItem();
		int nImage, nSelectedImage;
		int img = tree.GetItemImage( hItem, nImage, nSelectedImage );
		switch(nImage)
		{
		case 2://group
			{
				CGroupDescr* group = (CGroupDescr*)tree.GetItemData(hItem);
				if(!GetDocument()->CheckGroupName(pTVDispInfo->item.pszText))
				{
					CString s;
					s.Format("Группа '%s' уже существует", pTVDispInfo->item.pszText);
					AfxMessageBox(s, MB_OK|MB_ICONWARNING);

					*pResult = 0;
				}
				else
					group->m_name = pTVDispInfo->item.pszText;
			}
			break;
		case 0://effect
			{
				CEffectData* effect = (CEffectData*)tree.GetItemData(hItem);

				if(!GetDocument()->CheckEffectName(pTVDispInfo->item.pszText))
				{
					CString s;
					s.Format("Эффект '%s' уже существует", pTVDispInfo->item.pszText);
					AfxMessageBox(s, MB_OK|MB_ICONWARNING);

					*pResult = 0;
				}
				else
					effect->name = pTVDispInfo->item.pszText;
			}
			break;
		case 1://emitter
			{
				CEmitterData* emitter = (CEmitterData*)tree.GetItemData(hItem);

				hItem = tree.GetParentItem(hItem);
				CEffectData* pEffect = (CEffectData*)tree.GetItemData(hItem);
				if (CString(pTVDispInfo->item.pszText)=="" || CString(pTVDispInfo->item.pszText)=="не связан")
				{
					AfxMessageBox("Не допустимое имя эмиттера", MB_OK|MB_ICONWARNING);
					*pResult = 0;
				}
				else if(!pEffect->CheckName(pTVDispInfo->item.pszText))
				{
					CString s;
					s.Format("Эмиттер '%s' уже существует", pTVDispInfo->item.pszText);
					AfxMessageBox(s, MB_OK|MB_ICONWARNING);
					*pResult = 0;
				}
				else
				{
					if(emitter->data)
					{
						tr->ChangeEmitterName(emitter->data->name.c_str(),pTVDispInfo->item.pszText);
						emitter->data->name = pTVDispInfo->item.pszText;
					}
					else
						emitter->data_light->name = pTVDispInfo->item.pszText;

					emitter->bDirty = true;
				}
			}
			break;
		}
	}
}

void CEffectTreeView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	if (m_bDragging)
	{
		HTREEITEM hitem = GetTreeCtrl().GetDropHilightItem();
		if (GetTreeCtrl().GetItemState(hitem,0xffffffff)&TVIS_EXPANDED)
			GetTreeCtrl().Expand(hitem,TVE_COLLAPSE);
		else 
			GetTreeCtrl().Expand(hitem,TVE_EXPAND);
	}else
	{
		CPoint     pt, ptScreen;
		UINT       flags;
		CTreeCtrl& tree = GetTreeCtrl();

		GetCursorPos(&ptScreen); pt = ptScreen;
		tree.ScreenToClient(&pt);
		
		HTREEITEM hItem = tree.HitTest(pt, &flags);

		if(hItem){
			tree.SelectItem(hItem);
			SelChanged(hItem);
		}

		CMenu menu;
		menu.LoadMenu(IDR_MENU_POPUP);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, ptScreen.x, ptScreen.y, AfxGetMainWnd());
	}
}

void CEffectTreeView::OnGroupNew()
{
	CTreeCtrl& tree = GetTreeCtrl();
	CEffectToolDoc* pDoc = GetDocument();
	pDoc->m_pActiveGroup = pDoc->AddGroup();
	HTREEITEM hItem = tree.InsertItem(pDoc->m_pActiveGroup->m_name, 2, 2);
	tree.SetItemData(hItem, (DWORD)pDoc->m_pActiveGroup);
	tree.SetItemState(hItem, 0, TVIS_STATEIMAGEMASK); //no checkbox
	tree.SelectItem(hItem);

	OnEffectNew();
}

void CEffectTreeView::OnElementDelete()
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	int nImage, nSelectedImage;
	int img = tree.GetItemImage( hItem, nImage, nSelectedImage );
	switch(nImage)
	{
	case 2://group
		GetDocument()->DeleteCurrentGroup();
		GetTreeCtrl().DeleteItem(hItem);
		break;
	case 0://effect
		OnEffectDel();
		break;
	case 1://emitter
		OnEffectEmitterDel();
		break;
	}
}

void CEffectTreeView::OnUpdateElementDelete(CCmdUI *pCmdUI)
{
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hItem = tree.GetSelectedItem();
	int nImage, nSelectedImage;
	int img = tree.GetItemImage( hItem, nImage, nSelectedImage );
	switch(nImage)
	{
	case 2://group
		pCmdUI->Enable(GetDocument()->m_group.GetSize()>1);
		break;
	case 0://effect
		{
			CGroupDescr* group = GetDocument()->m_pActiveGroup;
			pCmdUI->Enable(GetDocument()->m_pActiveGroup->m_effects.size()>1);
		}
		break;
	case 1://emitter
		CEffectData* pEffect = (CEffectData*)tree.GetItemData(tree.GetParentItem(hItem));
		pCmdUI->Enable(pEffect->emitters.size()>1);
		break;
	}
}

void CEffectTreeView::OnEffectNew() 
{
	CTreeCtrl& tree = GetTreeCtrl();

	CEffectData* pNewEffect;
	GetDocument()->m_pActiveGroup->m_effects.push_back(pNewEffect = new CEffectData);

	HTREEITEM hItemGroup = tree.GetSelectedItem();

	int nImage, nSelectedImage;
	int img = tree.GetItemImage( hItemGroup, nImage, nSelectedImage );
	switch(nImage)
	{
	case 2://group
		break;
	case 0://effect
		hItemGroup = tree.GetParentItem(hItemGroup);
		break;
	case 1://emitter
		hItemGroup = tree.GetParentItem(tree.GetParentItem(hItemGroup));
		break;
	}

	HTREEITEM hItem = tree.InsertItem(pNewEffect->name.c_str(), 0, 0, hItemGroup);
	tree.SetItemData(hItem, (DWORD)pNewEffect);
	tree.SetItemState(hItem, 0, TVIS_STATEIMAGEMASK); //no checkbox
	tree.SelectItem(hItem);

	OnEffectEmitterNew();
}
void CEffectTreeView::OnEffectDel() 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	CEffectData* pEffect = (CEffectData*)GetTreeCtrl().GetItemData(hItem);

	EffectStorageType& effects = GetDocument()->m_pActiveGroup->m_effects;
	EffectStorageType::iterator it = find(effects.begin(), effects.end(), pEffect);
	ASSERT(it != effects.end());

	GetDocument()->Release3DModel();

	delete pEffect;
	effects.erase(it);
	GetTreeCtrl().DeleteItem(hItem);
}

void CEffectTreeView::OnEffectEmitterNew() 
{
	CTreeCtrl& tree = GetTreeCtrl();

	HTREEITEM hItem = tree.GetSelectedItem();
//	if(tree.GetParentItem(hItem) != 0)
//		hItem = tree.GetParentItem(hItem);
///
	int nImage, nSelectedImage;
	int img = tree.GetItemImage( hItem, nImage, nSelectedImage );
	switch(nImage)
	{
	case 2://group
		hItem = tree.GetChildItem(hItem);
		break;
	case 0://effect
		break;
	case 1://emitter
		hItem = tree.GetParentItem(hItem);
		break;
	}
///
	CEffectData* pEffect = (CEffectData*)tree.GetItemData(hItem);

	CEmitterData* pEmitter = pEffect->add_emitter();
/*	
	// Set sprite name
	CString sprite(FOLDER_SPRITE);
	sprite += DEFAULT_TXT_FIRE;
	pEmitter->data->texture_name = sprite;
	//
*/
	CString name;
	if(pEmitter->data)
		name = pEmitter->data->name.c_str();
	else
		name = pEmitter->data_light->name.c_str();

	hItem = tree.InsertItem(name, 1, 1, hItem);
	tree.SetItemData(hItem, (DWORD)pEmitter);
	tree.SetCheck(hItem);
	tree.SelectItem(hItem);
}

void CEffectTreeView::OnEffectEmitterDel() 
{
	CTreeCtrl& tree = GetTreeCtrl();

	HTREEITEM hItem = tree.GetSelectedItem();
	CEmitterData* pEmitter = (CEmitterData*)tree.GetItemData(hItem);
	CEffectData* pEffect = (CEffectData*)tree.GetItemData(tree.GetParentItem(hItem));

	GetDocument()->Release3DModel();

	pEffect->del_emitter(pEmitter);

	tree.DeleteItem(hItem);
}

void CEffectTreeView::OnUpdateEffectNew(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetTreeCtrl().GetSelectedItem() != 0 && !GetDocument()->m_StorePath.IsEmpty());
}
void CEffectTreeView::OnUpdateEffectDel(CCmdUI* pCmdUI) 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();

	pCmdUI->Enable(hItem && GetTreeCtrl().GetParentItem(hItem) == 0);
}
void CEffectTreeView::OnUpdateEffectEmitterDel(CCmdUI* pCmdUI) 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();

	pCmdUI->Enable(hItem && GetTreeCtrl().GetParentItem(hItem) != 0);
}
void CEffectTreeView::OnUpdateEffectEmitterNew(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetTreeCtrl().GetSelectedItem() != 0 && !GetDocument()->m_StorePath.IsEmpty());
}
void CEffectTreeView::OnUpdateGroupNew(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( !GetDocument()->m_StorePath.IsEmpty());
}

void CEffectTreeView::SelChanged(HTREEITEM hItem)
{
	CTreeCtrl& tree = GetTreeCtrl();

	CEffectToolDoc* pDoc = GetDocument();
	int nImage, nSelectedImage;
	int img = tree.GetItemImage( hItem, nImage, nSelectedImage );
	switch(nImage)
	{
	case 2://group
		pDoc->m_pActiveGroup = (CGroupDescr*)tree.GetItemData(hItem);
		if(pDoc->m_pActiveGroup->m_effects.size())
			pDoc->m_pActiveEffect = (CEffectData*)(*pDoc->m_pActiveGroup->m_effects.begin());
			pDoc->m_pActiveEmitter = NULL;
		break;
	case 0://effect
		pDoc->m_pActiveGroup = (CGroupDescr*)tree.GetItemData(tree.GetParentItem(hItem));
		pDoc->m_pActiveEffect = (CEffectData*)tree.GetItemData(hItem);
		pDoc->m_pActiveEmitter = NULL;
		break;
	case 1://emitter
		pDoc->m_pActiveGroup = (CGroupDescr*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(hItem)));
		pDoc->m_pActiveEffect = (CEffectData*)tree.GetItemData(tree.GetParentItem(hItem));
		pDoc->m_pActiveEmitter = (CEmitterData*)tree.GetItemData(hItem);
		break;
	}
	tree.Invalidate();
}

void CEffectTreeView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CEffectToolDoc* pDoc = GetDocument();
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	if(bFillingTree)
		return;

	CTreeCtrl& tree = GetTreeCtrl();

	HTREEITEM hItem = tree.GetSelectedItem();
////
	SelChanged(hItem);
////	
	pDoc->m_nCurrentGenerationPoint = 0;
	pDoc->m_nCurrentParticlePoint = 0;
	pDoc->UpdateAllViews(0);
	AfxGetMainWnd()->SendMessage(WM_UPDATE_BAR);

	theApp.scene.InitEmitters();
}

void CEffectTreeView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTreeView::OnLButtonDown(nFlags, point);

	CTreeCtrl& tree = GetTreeCtrl();

	UINT uFlags;
	if(tree.HitTest(point, &uFlags))
	{
		GetCheckState();
		theApp.scene.InitEmitters();
	}
}

void CEffectTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if(lHint == HINT_UPDATE_TREE)
		PopulateTree();
	if(!GetDocument()->m_StorePath.IsEmpty())
		GetDocument()->SetWorkingDir();

	GetDocument()->Load3DModel(MODE_FIND, TYPE_3DMODEL);
	GetDocument()->Load3DModel(MODE_FIND, TYPE_3DBACK);

	GetDocument()->SetWorkingDir();
}

void CEffectTreeView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;


	CPoint      ptAction;
	UINT        nFlags;

	m_hitemDrag = GetTreeCtrl().HitTest(ptAction, &nFlags);

	//////
	int nImage, nSelectedImage;
	int img = GetTreeCtrl().GetItemImage( m_hitemDrag, nImage, nSelectedImage );
	switch(nImage)
	{
	case 2://group
		return;
		break;
	case 0://effect
		break;
	case 1://emitter
		break;
	}
	//////

	GetCursorPos(&ptAction);
	ScreenToClient(&ptAction);
	ASSERT(!m_bDragging);
	m_bDragging = true;
	m_hitemDrag = GetTreeCtrl().HitTest(ptAction, &nFlags);
	m_hitemDrop = 0;

	ASSERT(m_pimagelist == NULL);
	m_pimagelist = GetTreeCtrl().CreateDragImage(m_hitemDrag);  // get the image list for dragging
	m_pimagelist->DragShowNolock(TRUE);
	m_pimagelist->SetDragCursorImage(0, CPoint(0, 0));
	m_pimagelist->BeginDrag(0, CPoint(0,0));
	m_pimagelist->DragMove(ptAction);
	m_pimagelist->DragEnter(this, ptAction);
	SetCapture();
}

void CEffectTreeView::OnMouseMove(UINT nFlags, CPoint point) 
{
	HTREEITEM           hitem;
	UINT                flags;
	CTreeCtrl& tree = GetTreeCtrl();
	if (m_bDragging)
	{
		ASSERT(m_pimagelist != NULL);
		m_pimagelist->DragMove(point);

		hitem = GetTreeCtrl().HitTest(point, &flags);
		m_pimagelist->DragLeave(this);

		if(nFlags & MK_CONTROL){
			if(tree.GetParentItem(m_hitemDrag)==tree.GetParentItem(m_hitemDrop)) // тот же эффект
				GetTreeCtrl().SelectDropTarget(hitem);
		}else
			GetTreeCtrl().SelectDropTarget(hitem);
		CRect rc;
		GetClientRect(rc);
		if (point.y>rc.bottom-10)
			SendMessage(WM_VSCROLL,SB_LINEDOWN,0);
		else if (point.y<10)
			SendMessage(WM_VSCROLL,SB_LINEUP,0);
		m_hitemDrop = hitem;
		m_pimagelist->DragEnter(this, point);
	}

	CTreeView::OnMouseMove(nFlags, point);
}

void CEffectTreeView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bDragging)
	{
		ASSERT(m_pimagelist != NULL);
		m_pimagelist->DragLeave(this);
		m_pimagelist->EndDrag();
		delete m_pimagelist;
		m_pimagelist = NULL;

		int nImage, nSelectedImage;
		int img = GetTreeCtrl().GetItemImage( m_hitemDrag, nImage, nSelectedImage );

		switch(nImage)
		{
		case 2://group
			MessageBeep(0);
			break;
		case 0://effect
//			if(nFlags & MK_CONTROL)
//				CloneEffect(m_hitemDrag, m_hitemDrop);
			break;
		case 1://emitter
			if(m_hitemDrop && m_hitemDrag != m_hitemDrop)
			{
				if(nFlags & MK_CONTROL)
					CopyEmitter(m_hitemDrag, m_hitemDrop);
				else
					MoveEmitter(m_hitemDrag, m_hitemDrop);
			}
			else
				MessageBeep(0);
			break;
		}

		ReleaseCapture();
		m_bDragging = FALSE;
		GetTreeCtrl().SelectDropTarget(0);
	}
	
	CTreeView::OnLButtonUp(nFlags, point);
}

DROPEFFECT CEffectTreeView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return CTreeView::OnDragOver(pDataObject, dwKeyState, point);
}

void CEffectTreeView::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult) 
{
    LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
        
	CEffectToolDoc* pDoc = GetDocument();
	*pResult = 0;
	switch(pNMCD->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
	case CDDS_ITEMPREPAINT:
			HTREEITEM hItemSpec = reinterpret_cast<HTREEITEM>(pNMCD->nmcd.dwItemSpec);
			TV_ITEM ti;
			ti.hItem = hItemSpec;
			ti.mask = TVIF_IMAGE;

			CTreeCtrl& tree = GetTreeCtrl();
			tree.GetItem(&ti);

			switch(ti.iImage)
			{
			case 2://group
				{
					CGroupDescr* group = (CGroupDescr*)tree.GetItemData(hItemSpec);
					if(pDoc->m_pActiveGroup==group)
						pNMCD->clrTextBk = RGB(255, 150, 150);
					else
						pNMCD->clrTextBk = RGB(200, 150, 150);
				}
				break;
			case 0://effect
				{
					CEffectData* effect = (CEffectData*)tree.GetItemData(hItemSpec);
					if(pDoc->m_pActiveEffect==effect)
						pNMCD->clrTextBk = RGB(150, 255, 150);
					else
						pNMCD->clrTextBk = RGB(150, 200, 150);
				}
				break;
			case 1://emitter
				{
					CEmitterData* emitter = (CEmitterData*)tree.GetItemData(hItemSpec);
					if(pDoc->m_pActiveEmitter==emitter)
						pNMCD->clrTextBk = RGB(150, 150, 255);
					else
						pNMCD->clrTextBk = RGB(150, 150, 200);
				}
				break;
			}
			break;
	} 
}

void CEffectTreeView::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	CTreeCtrl& tree = GetTreeCtrl();	
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	int action = pNMTreeView->action;

	//////
	int nImage, nSelectedImage;
	int img = GetTreeCtrl().GetItemImage( hItem, nImage, nSelectedImage );
	switch(nImage)
	{
	case 2://group
		{
			CGroupDescr* group = (CGroupDescr*)tree.GetItemData(hItem);
			group->m_bExpand = (action==TVE_EXPAND);
		}
		break;
	case 0://effect
		{
			CEffectData* p = (CEffectData*)tree.GetItemData(hItem);
			p->bExpand = (action==TVE_EXPAND);
		}
		break;
	case 1://emitter
		{
			CEmitterData* emitter = (CEmitterData*)tree.GetItemData(hItem);
		}
		break;
	}
	//////

	*pResult = 0;
}
