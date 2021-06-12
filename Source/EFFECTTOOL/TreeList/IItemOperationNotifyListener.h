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
	Возвращаемое значение имеет мысл только для режима LIST
	\retval false - для того, чтобы разрешить последовательный вызов onDeleteItem
	\retval true - для того, чтобы запретить последовательный вызов onDeleteItem
	*/
	virtual bool onDeleteAllItems(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info) = 0;
	//! Уведомляет о том, что item был вставлен
	virtual void onInsertItem(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info) = 0;
};
