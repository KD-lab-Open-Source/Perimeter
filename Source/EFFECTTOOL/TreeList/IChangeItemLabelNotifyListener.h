#ifndef CHANGE_ITEM_LABEL_NOTIFY_LISTENER_H
#define CHANGE_ITEM_LABEL_NOTIFY_LISTENER_H

class CTreeListCtrl;
class CTreeListItem;

typedef struct _CHANGE_LABEL_NOTIFY_INFO{
	CTreeListItem	*pItem;
	int				iCol;
	CWnd			*pEditControl;//контрол, которым производится редактирование			
	CString			text;
}CHANGE_LABEL_NOTIFY_INFO;

class IChangeItemLabelNotifyListener
{
public:
	//! Вызывается перед началам редактирования текста.
	/*!
		Вернуть false для того, чтобы запретить редактирование
	*/
	virtual bool onBeginLabelEdit(CTreeListCtrl& source, 
							CHANGE_LABEL_NOTIFY_INFO* pclns) = 0;

	virtual bool onEndLabelEdit(CTreeListCtrl& source,
						CHANGE_LABEL_NOTIFY_INFO* info) = 0;

	//! Спрашиваем контрол, которым будет редактироваться текст
	virtual DWORD onRequestCtrl(CTreeListCtrl& source, 
								CHANGE_LABEL_NOTIFY_INFO* pclns) = 0;
	//! Уведомление приходит перед тем, как показать контрол редактирования
	/*!
		Уведомление приходит перед тем, как показать контрол редактирования
		В ответ на это пользователь может произвести настройку контрола(вставку 
		необходимх данных) или вообще заменить на свой контрол.
		В случае замены необходимо самостоятельно позиционировать контрол
		Возвращаемое значение игнорируется 
	*/
	virtual bool onBeginControl(CTreeListCtrl& source, 
								CHANGE_LABEL_NOTIFY_INFO* pclns) = 0;

	//! Уведомление приходит после закрытия контрола
	/*!
		Если в onBeginControl воткнули свой контрол, то здесь самое время его удалить
	*/
	virtual void onEndControl(CTreeListCtrl& source, 
								CHANGE_LABEL_NOTIFY_INFO* pclns) = 0;

};

#endif