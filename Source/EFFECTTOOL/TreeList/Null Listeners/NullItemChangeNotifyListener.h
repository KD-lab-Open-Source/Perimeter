#pragma once

#include "../IItemChangeNotifyListener.h"

class NullItemChangeNotifyListener : public IItemChangeNotifyListener
{
public:
	NullItemChangeNotifyListener(void);
	~NullItemChangeNotifyListener(void);

	//! ѕеред тем, как развернуть
	virtual bool onItemExpanding(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ѕосле того, как развернули
	virtual void onItemExpanded(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ѕеред изменением состо€ни€
	virtual bool onItemStateChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ѕосле изменени€ состо€ни€
	virtual void onItemStateChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ≈сли есть стиль	TLC_CHECKBOX, то приходит перед измененим состо€ни€
	virtual bool onItemCheckChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ≈сли есть стиль	TLC_CHECKBOX, то приходит после изменени€ состо€ни€
	virtual void onItemCheckChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);

	//! ≈сли есть стиль	TLC_LOCKBOX, то приходит перед измененим состо€ни€
	virtual bool onItemLockChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	//! ≈сли есть стиль	TLC_LOCKBOX, то приходит после изменени€ состо€ни€
	virtual void onItemLockChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);

};
