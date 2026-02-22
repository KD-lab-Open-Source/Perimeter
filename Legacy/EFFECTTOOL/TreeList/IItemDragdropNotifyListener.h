#if !defined (ITEM_DRAGDROP_LISTENER_H)
#define ITEM_DRAGDROP_LISTENER_H

#pragma once

class CTreeListCtrl;
class CTreeListItem;

typedef struct _TREELISTDRAGDROP
{
	//! контрол, из которого тянут
	CTreeListCtrl*	pSource;
	/*!
		Перетаскиваются выделенные элементы
	*/
	//! Узел над, которым пролетаем
	CTreeListItem*	hoverItem;									// item pointer
	HCURSOR hCursor;
// TODO: Добавить поле с курсором
} TREELISTDRAGDROP, FAR* LPTREELISTDRAGDROP;


class IItemDragdropNotifyListener
{
public:
	//! Уведомляет о том, что начали тянуть выделенные элементы левой кнопкой мыши
	/*!
		TREELISTDRAGDROP.hoverItem == NULL
		TREELISTDRAGDROP.hCursor - ignored
		\retval true - можно начинать операцию
		\retval false - нельзя начинать операцию
	*/
	virtual bool onBeginDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd) = 0;
	//! Уведомляет о том, что начали тянуть выделенные элементы правой кнопкой мыши
	/*!
		\a !!!На текущий момент не вызывается!!!
		TREELISTDRAGDROP.hoverItem == NULL
		TREELISTDRAGDROP.hCursor - ignored
		true - можно начинать операцию
		false - нельзя начинать операцию
	*/
	virtual bool onBeginRDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd) = 0;

	//! Уведомляет о том, что курсор вошел в область окна
	/*!
		Уведомляет о том, что курсор вошел в область окна. 
		TREELISTDRAGDROP.hoverItem == NULL
		\retval true - source позволяет выполнять операцию Drag&Drop
		\retval false - source не позволяет выполнять операцию Drag&Drop. Операция
		не прерывается. В source не приходят дальнейшие уведомления
	*/
	virtual bool onDragEnter(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd) = 0;
	//! Уведомляет о том, что курсор покинул область окна
	/*!
		Уведомляет о том, что курсор вышел из области окна. Не приходит в окно, 
		в котором была чата операция Drag&Drop
		TREELISTDRAGDROP.hoverItem == NULL
	*/
	virtual void onDragLeave(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd) = 0;
	//! Уведомляет о том, что тянем курсор над окном
	/*!
		TREELISTDRAGDROP.hoverItem == узел, над которым тянем
		false - кидать нельзя
		true  - кинуть можно
	*/
	virtual bool onDragOver(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd) = 0;
	//! Уведомляет о том, что отжали кнопку мыши
	/*!
		TREELISTDRAGDROP.hCursor - ignored
		\retval true - операция успешно выполнена
		\retval false - операция не удалась
		Возвращаемое значение сейчас игнорируется
	*/
	virtual bool onDrop(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd) = 0;

};

#endif//ITEM_DRAGDROP_LISTENER_H