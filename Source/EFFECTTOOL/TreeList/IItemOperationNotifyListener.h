#pragma once

class CTreeListCtrl;
class CTreeListItem;

typedef struct _ITEM_OPERATION_INFO {
	CTreeListItem* item;
}ITEM_OPERATION_INFO;
class IItemOperationNotifyListener
{
public:
	virtual void onDeleteItem(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info) = 0;
	/*!
	¬озвращаемое значение имеет мысл только дл€ режима LIST
	\retval false - дл€ того, чтобы разрешить последовательный вызов onDeleteItem
	\retval true - дл€ того, чтобы запретить последовательный вызов onDeleteItem
	*/
	virtual bool onDeleteAllItems(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info) = 0;
	//! ”ведомл€ет о том, что item был вставлен
	virtual void onInsertItem(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info) = 0;
};
