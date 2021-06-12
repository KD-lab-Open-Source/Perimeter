#pragma once
#include "../igeneralnotifylistener.h"

class NullGeneralNotifyListener :
	public IGeneralNotifyListener
{
public:
	NullGeneralNotifyListener(void);
	~NullGeneralNotifyListener(void);
	virtual void onClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	/*!
		Возрващаемое значение имеет смысл только для дерева
		\retval true - для того, чтобы позволить обработку по умолчанию
		\retval false - для того, чтобы запретить обработку по умолчанию
	*/
	virtual bool onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);

	virtual void onRClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	virtual void onRDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);

	//! Не вызывается
	virtual bool onHover(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);

	virtual void onKillFocus(CTreeListCtrl& tree);
	virtual void onSetFocus(CTreeListCtrl& tree);


	virtual void onReturn(CTreeListCtrl& tree);
	//! Не вызывается
	virtual void onReleasedCapture(CTreeListCtrl& tree);
};
