#ifndef CHANGE_ITEM_LABEL_NOTIFY_LISTENER_H
#define CHANGE_ITEM_LABEL_NOTIFY_LISTENER_H

class CTreeListCtrl;
class CTreeListItem;

typedef struct _CHANGE_LABEL_NOTIFY_INFO{
	CTreeListItem	*pItem;
	int				iCol;
	CWnd			*pEditControl;//контрол, которым производитс€ редактирование			
	CString			text;
}CHANGE_LABEL_NOTIFY_INFO;

class IChangeItemLabelNotifyListener
{
public:
	//! ¬ызываетс€ перед началам редактировани€ текста.
	/*!
		¬ернуть false дл€ того, чтобы запретить редактирование
	*/
	virtual bool onBeginLabelEdit(CTreeListCtrl& source, 
							CHANGE_LABEL_NOTIFY_INFO* pclns) = 0;

	virtual bool onEndLabelEdit(CTreeListCtrl& source,
						CHANGE_LABEL_NOTIFY_INFO* info) = 0;

	//! —прашиваем контрол, которым будет редактироватьс€ текст
	virtual DWORD onRequestCtrl(CTreeListCtrl& source, 
								CHANGE_LABEL_NOTIFY_INFO* pclns) = 0;
	//! ”ведомление приходит перед тем, как показать контрол редактировани€
	/*!
		”ведомление приходит перед тем, как показать контрол редактировани€
		¬ ответ на это пользователь может произвести настройку контрола(вставку 
		необходимх данных) или вообще заменить на свой контрол.
		¬ случае замены необходимо самосто€тельно позиционировать контрол
		¬озвращаемое значение игнорируетс€ 
	*/
	virtual bool onBeginControl(CTreeListCtrl& source, 
								CHANGE_LABEL_NOTIFY_INFO* pclns) = 0;

	//! ”ведомление приходит после закрыти€ контрола
	/*!
		≈сли в onBeginControl воткнули свой контрол, то здесь самое врем€ его удалить
	*/
	virtual void onEndControl(CTreeListCtrl& source, 
								CHANGE_LABEL_NOTIFY_INFO* pclns) = 0;

};

#endif