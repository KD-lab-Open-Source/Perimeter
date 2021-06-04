#pragma once
#include "../ichangeitemlabelnotifylistener.h"

class NullChangeItemLabelNotifyListener :
	public IChangeItemLabelNotifyListener
{
public:
	NullChangeItemLabelNotifyListener(void);
	virtual ~NullChangeItemLabelNotifyListener(void);


	//! ¬ызываетс€ перед началам редактировани€ текста.
	virtual bool onBeginLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);

	//! ”ведомл€ет о том, что редактирование закончено
	virtual bool onEndLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);

	//! —прашиваем контрол, которым будет редактироватьс€ текст
	virtual DWORD onRequestCtrl(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);
	//! ”ведомление приходит перед тем, как показать контрол редактировани€
	virtual bool onBeginControl(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);

	//! ”ведомление приходит после закрыти€ контрола
	virtual void onEndControl(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);
};
