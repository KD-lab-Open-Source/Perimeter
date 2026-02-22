#pragma once
#include "../ichangeitemlabelnotifylistener.h"

class NullChangeItemLabelNotifyListener :
	public IChangeItemLabelNotifyListener
{
public:
	NullChangeItemLabelNotifyListener(void);
	virtual ~NullChangeItemLabelNotifyListener(void);


	//! Вызывается перед началам редактирования текста.
	virtual bool onBeginLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);

	//! Уведомляет о том, что редактирование закончено
	virtual bool onEndLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);

	//! Спрашиваем контрол, которым будет редактироваться текст
	virtual DWORD onRequestCtrl(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);
	//! Уведомление приходит перед тем, как показать контрол редактирования
	virtual bool onBeginControl(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);

	//! Уведомление приходит после закрытия контрола
	virtual void onEndControl(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);
};
