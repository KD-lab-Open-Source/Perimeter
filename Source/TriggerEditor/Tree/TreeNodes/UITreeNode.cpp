// UITreeNode.cpp: implementation of the UITreeNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "Custom Controls/xTreeListCtrl.h"
#include "UITreeNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UITreeNode::UITreeNode(std::string const& action, int ordinalNumber):
  action_(action)
, ordinalNumber_(ordinalNumber)
, treeListItem_(NULL)
{

}

UITreeNode::~UITreeNode()
{

}

std::string const& UITreeNode::getAction() const{
	return action_;
}

void UITreeNode::setAction(std::string const& action){
	action_ = action;
}

CTreeListItem* UITreeNode::getTreeListItem() const{
	return treeListItem_;
}

void UITreeNode::setTreeListItem(CTreeListItem* pItem){
	treeListItem_ = pItem;
}

int UITreeNode::getOrdinalNumber() const{
	return ordinalNumber_;
}
void UITreeNode::setOrdinalNumber(int iNum){
	ordinalNumber_ = iNum;
}
//! Загрузка узла в дерево
bool UITreeNode::load(xTreeListCtrl& tree, CTreeListItem* pParent)
{
	setTreeListItem(tree.InsertItem(action_.c_str(), pParent));
	if (!treeListItem_)
		return false;
	tree.SetItemData(treeListItem_, 
		reinterpret_cast<DWORD_PTR>(this));
	return true;
}
//! Обработка команд от пунктов  меню
bool UITreeNode::onCommand(TETreeLogic& logic, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void UITreeNode::onDeleteItem(TETreeLogic& logic)
{
	delete this;
}

//! Обработка начал перетаскивания
void UITreeNode::onBeginDrag(TETreeLogic& logic)
{
	std::auto_ptr<COleDataSource> source(new COleDataSource);
	CString strCFName((LPTSTR)IDS_CB_FORMAT_NAME);
	UINT clFromatId = RegisterClipboardFormat(strCFName);
	
	HGLOBAL h = ::GlobalAlloc( GMEM_SHARE, 
		sizeof(TCHAR)*(action_.size()+1) + sizeof(int));
	LPVOID p = ::GlobalLock( h );
	LPBYTE pT = (LPBYTE)p;

	*(int*)pT = ordinalNumber_;
	memcpy( pT + sizeof(int), action_.c_str(), action_.size()*sizeof(TCHAR) );

	::GlobalUnlock( h );
	source->CacheGlobalData( clFromatId, h );
	source->DoDragDrop();
}
