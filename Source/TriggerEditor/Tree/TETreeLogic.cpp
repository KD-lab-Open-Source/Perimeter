#include "StdAfx.h"
#include "../Resource.h"
#include "TETreeManager.h"
#include "TETreeLogic.h"

#include "TriggerExport.h"
#include <boost/tokenizer.hpp>

#include "Custom Controls/xTreeListCtrl.h"

#include "Tree/TreeNodes/ITreeNode.h"
#include "Tree/UITreeNodeFabric.h"

TETreeLogic::TETreeLogic():
  treeManager_(NULL)
{
}

TETreeLogic::~TETreeLogic(void)
{
}

void TETreeLogic::setTreeManager(TETreeManager* treeManager){
	treeManager_ = treeManager;
	treeManager_->setTETreeNotifyListener(this);
}

bool TETreeLogic::init()
{
	xTreeListCtrl& tree = treeManager_->getTreeListCtrl();
	tree.InsertColumn(CString(reinterpret_cast<LPTSTR>(IDS_ACTION_COLUMN)));
	CRect rc;
	tree.GetClientRect(rc);
	tree.SetColumnWidth(0, rc.Width() - 2);

	return true;
}

bool TETreeLogic::loadTree()
{
	return loadTree(treeManager_->getTreeListCtrl());
}

bool TETreeLogic::loadTree(xTreeListCtrl& tree)
{
	CWaitCursor wait;
	LPCTSTR values = triggerInterface().actionComboList();
	typedef boost::tokenizer<boost::char_separator<char>,LPCTSTR> tokenizer;
	boost::char_separator<char> sep(_T("|"));
	tokenizer tokens(values, values + _tcslen(values), sep);
	int ordinalNumber = 0;
	for (tokenizer::iterator tok_iter = tokens.begin();
		tok_iter != tokens.end(); ++tok_iter)
	{
		std::string const& token = *tok_iter;
		IUITreeNode* ptrUITreeNode = 
			UITreeNodeFabric::create(token, ordinalNumber++).release();

		ptrUITreeNode->load(tree, TLI_ROOT);
	}

	return true;
}

bool TETreeLogic::onNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	NMHDR* pHdr = reinterpret_cast<NMHDR*>(lParam);
	xTreeListCtrl& tree = treeManager_->getTreeListCtrl();
	if (pHdr->hwndFrom == tree.m_hWnd) 
	{
		
		switch(pHdr->code) {
		case NM_RCLICK:
			onRClick(pHdr, pResult);
			return true;
			return true;
		case TLN_DELETEITEM:
			onDeleteItem(pHdr, pResult);
			return true;
		case TLN_BEGINDRAG:
			onBeginDrag(pHdr, pResult);
			return true;
//		case TLN_KEYDOWN:
//			onKeyDown(pHdr, pResult);
		};
	}
	return false;
}

void TETreeLogic::adjustTreeColumnsWidth()
{
	xTreeListCtrl& tree = treeManager_->getTreeListCtrl();
	int const nColumns = tree.GetColumnCount();
	for(int i = 0; i < nColumns; ++i)
		tree.SetColumnWidth(i, TLSCW_AUTOSIZE_USEHEADER);
}

void TETreeLogic::onRClick(NMHDR* phdr, LRESULT *pResult)
{
	NMTREELIST* ptl = reinterpret_cast<NMTREELIST*>(phdr);
	if (ptl->pItem) 
	{
		IUITreeNode* pUINode = 
			reinterpret_cast<IUITreeNode*>(ptl->pItem->GetData());
//		pUINode->onRClick(*this, ptl->iCol);
	}
	*pResult = true;
}

void TETreeLogic::onDeleteItem(NMHDR* phdr, LRESULT *pResult)
{
	NMTREELIST* ptl = reinterpret_cast<NMTREELIST*>(phdr);
	ASSERT(ptl->pItem) ;
	IUITreeNode* pUINode = 
		reinterpret_cast<IUITreeNode*>(ptl->pItem->GetData());
	pUINode->onDeleteItem(*this);
}

void TETreeLogic::onKeyDown(NMHDR* phdr, LRESULT *pResult)
{
	NMTREELISTKEYDOWN *pNMKeyDown = reinterpret_cast<NMTREELISTKEYDOWN*>(phdr);
	xTreeListCtrl& tree = treeManager_->getTreeListCtrl();
	CTreeListItem* pItem = tree.GetSelectedItem();
	if (!pItem) 
		return;
//	IUITreeNode* pNode = reinterpret_cast<IUITreeNode*>(pItem->GetData());
//	pNode->onKeyDown(getDoc(), tree, 
//		pNMKeyDown->wVKey, pNMKeyDown->nRepCnt, pNMKeyDown->flags);
}

bool TETreeLogic::onCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam))
		return false;

	xTreeListCtrl& tree = treeManager_->getTreeListCtrl();
	CTreeListItem* pItem = tree.GetSelectedItem();
	if (pItem) 
	{
		IUITreeNode* pUINode = reinterpret_cast<IUITreeNode*>(pItem->GetData());
		return pUINode->onCommand(*this, wParam, lParam);
	}
	return false;
}

void TETreeLogic::onBeginDrag(NMHDR* phdr, LRESULT *pResult)
{
	NMTREELISTDROP* pNMDragDrop = 
		reinterpret_cast<NMTREELISTDROP*>(phdr);
	CTreeListItem* pItem = treeManager_->getTreeListCtrl().GetSelectedItem();
	assert (pItem);
	IUITreeNode* node = reinterpret_cast<IUITreeNode*>(pItem->GetData());
	node->onBeginDrag(*this);
	*pResult = 1;
}