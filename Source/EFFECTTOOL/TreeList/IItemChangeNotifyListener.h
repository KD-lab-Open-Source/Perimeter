#if !defined ITEM_CHANGE_NOTIFY_LISTENER_H

#define ITEM_CHANGE_NOTIFY_LISTENER_H

#pragma once

class CTreeListCtrl;
class CTreeListItem;

typedef struct _ITEM_CHANGE_INFO
{
	CTreeListItem* item;
	int iOldSubItem;///< если установлен стиль TLC_SHOWSELACTIVE, предыдущий активный столбец
	int iNewSubItem;///< если установлен стиль TLC_SHOWSELACTIVE, новый активный столбец
	UINT uNewState;
	UINT uOldState;
}ITEM_CHANGE_INFO;

class IItemChangeNotifyListener
{
public:
	/************************************************************************/
	/* возвращение false запрещает изменение состо€ни€
	/************************************************************************/
	//! ѕеред тем, как развернуть
	virtual bool onItemExpanding(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;
	//! ѕосле того, как развернули
	virtual void onItemExpanded(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;
	//! ѕеред изменением состо€ни€
	virtual bool onItemStateChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;
	//! ѕосле изменени€ состо€ни€
	virtual void onItemStateChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;

	//! ≈сли есть стиль	TLC_CHECKBOX, то приходит перед измененим состо€ни€
	virtual bool onItemCheckChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;
	//! ≈сли есть стиль	TLC_CHECKBOX, то приходит после изменени€ состо€ни€
	virtual void onItemCheckChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;

	//! ≈сли есть стиль	TLC_LOCKBOX, то приходит перед измененим состо€ни€
	virtual bool onItemLockChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;
	//! ≈сли есть стиль	TLC_LOCKBOX, то приходит после изменени€ состо€ни€
	virtual void onItemLockChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci) = 0;
};


#endif