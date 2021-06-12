#pragma once

class CTreeListCtrl;
class CTreeListItem;

typedef struct _GENERAL_NOTIFY_INFO {
	CTreeListItem* item;///< узел, по-которому попали
	int iSubItem;///< номер столбца
	UINT nFlags;///< Флаги, полученные из HitTest
}GENERAL_NOTIFY_INFO;
class IGeneralNotifyListener
{
public:
	virtual void onClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info) = 0;
	/*!
		Возрващаемое значение имеет смысл только для дерева
		\retval true - для того, чтобы позволить обработку по умолчанию
		\retval false - для того, чтобы запретить обработку по умолчанию
	*/
	virtual bool onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info) = 0;

	virtual void onRClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info) = 0;
	//! Не вызывается
	virtual void onRDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info) = 0;

	//! Не вызывается
	virtual bool onHover(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info) = 0;

	virtual void onKillFocus(CTreeListCtrl& tree) = 0;
	virtual void onSetFocus(CTreeListCtrl& tree) = 0;

	virtual void onReleasedCapture(CTreeListCtrl& tree) = 0;

	virtual void onReturn(CTreeListCtrl& tree) = 0;
};
