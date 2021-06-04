#pragma once

class TETreeLogic;
class xTreeListCtrl;
class CTreeListItem;

interface IUITreeNode
{
	//! Загрузка узла в дерево
	virtual bool load(xTreeListCtrl& tree, CTreeListItem* pParent) = 0;
	//! Обработка команд от пунктов  меню
	virtual bool onCommand(TETreeLogic& logic, WPARAM wParam, LPARAM lParam) = 0;
	//! Обработка начал перетаскивания
	virtual void onBeginDrag(TETreeLogic& logic) = 0;
	//! Обработчик сообщения об удалении узла
	virtual void onDeleteItem(TETreeLogic& logic) = 0;
};