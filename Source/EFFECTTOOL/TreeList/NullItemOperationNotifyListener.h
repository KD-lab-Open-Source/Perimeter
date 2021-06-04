#pragma once
#include "../iitemoperationnotifylistener.h"

class NullItemOperationNotifyListener :
	public IItemOperationNotifyListener
{
public:
	NullItemOperationNotifyListener(void);
	virtual ~NullItemOperationNotifyListener(void);

	virtual void onDeleteItem(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info);
	virtual bool onDeleteAllItems(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info);
	virtual void onInsertItem(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info);
};
