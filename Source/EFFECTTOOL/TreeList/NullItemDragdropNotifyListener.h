#pragma once
#include "../IItemDragdropNotifyListener.h"

class NullItemDragdropNotifyListener :public IItemDragdropNotifyListener
{
public:
	NullItemDragdropNotifyListener(void);
	virtual ~NullItemDragdropNotifyListener(void);
	//! Начинаем тянуть выделенные элементы левой кнопкой мыши
	virtual bool onBeginDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! Начинаем тянуть выделенные элементы левой кнопкой мыши
	virtual bool onBeginRDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! Уведомляет о том, что курсор вошел в область окна
	virtual bool onDragEnter(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! Уведомляет о том, что курсор покинул область окна
	virtual void onDragLeave(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! Уведомляет о том, что тянем курсор над окном
	virtual bool onDragOver(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! Уведомляет о том, что отжали кнопку мыши
	virtual bool onDrop(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
};
