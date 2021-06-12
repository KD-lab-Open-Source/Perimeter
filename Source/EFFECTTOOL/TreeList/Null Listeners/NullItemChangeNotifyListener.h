#pragma once

#include "../IItemChangeNotifyListener.h"

class NullItemChangeNotifyListener : public IItemChangeNotifyListener
{
public:
	NullItemChangeNotifyListener(void);
	~NullItemChangeNotifyListener(void);

	//! Перед тем, как развернуть
	virtual bool onItemExpanding(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! После того, как развернули
	virtual void onItemExpanded(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! Перед изменением состояния
	virtual bool onItemStateChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! После изменения состояния
	virtual void onItemStateChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! Если есть стиль	TLC_CHECKBOX, то приходит перед измененим состояния
	virtual bool onItemCheckChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! Если есть стиль	TLC_CHECKBOX, то приходит после изменения состояния
	virtual void onItemCheckChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);

	//! Если есть стиль	TLC_LOCKBOX, то приходит перед измененим состояния
	virtual bool onItemLockChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! Если есть стиль	TLC_LOCKBOX, то приходит после изменения состояния
	virtual void onItemLockChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);

};
