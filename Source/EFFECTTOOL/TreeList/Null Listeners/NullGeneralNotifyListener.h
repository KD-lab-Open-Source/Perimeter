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
		¬озрващаемое значение имеет смысл только дл€ дерева
		\retval true - дл€ того, чтобы позволить обработку по умолчанию
		\retval false - дл€ того, чтобы запретить обработку по умолчанию
	*/
	virtual bool onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);

	virtual void onRClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	virtual void onRDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);

	//! Ќе вызываетс€
	virtual bool onHover(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);

	virtual void onKillFocus(CTreeListCtrl& tree);
	virtual void onSetFocus(CTreeListCtrl& tree);


	virtual void onReturn(CTreeListCtrl& tree);
	//! Ќе вызываетс€
	virtual void onReleasedCapture(CTreeListCtrl& tree);
};
