// UITreeNode.h: interface for the UITreeNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UITREENODE_H__CB138F3B_7331_4D84_A8AB_7AA911ECD71A__INCLUDED_)
#define AFX_UITREENODE_H__CB138F3B_7331_4D84_A8AB_7AA911ECD71A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TREE\TREENODES\ITreeNode.h"
#include <string>

class UITreeNodeFabric;
class xTreeListCtrl;
class CTreeListItem;

class UITreeNode : public IUITreeNode  
{
	friend class UITreeNodeFabric;
protected:
	UITreeNode(std::string const& action, int ordinalNumber);
	virtual ~UITreeNode();
public:

	//! Загрузка узла в дерево
	virtual bool load(xTreeListCtrl& tree, CTreeListItem* pParent);
	//! Обработка команд от пунктов  меню
	virtual bool onCommand(TETreeLogic& logic, WPARAM wParam, LPARAM lParam);
	//! Обработка начал перетаскивания
	virtual void onBeginDrag(TETreeLogic& logic);
	//! Обработчик сообщения об удалении узла
	virtual void onDeleteItem(TETreeLogic& logic);
protected:
	//! Возвращает имя действия
	std::string const& getAction() const;
	//! Назначить имя действия
	void setAction(std::string const& action);
	//! Возвращает узел действия в дереве
	CTreeListItem* getTreeListItem() const;
	//! Назначает узел действия в дереве
	void setTreeListItem(CTreeListItem* pItem);
	//! Возвращает порядковый номер действия
	int getOrdinalNumber() const;
	//! Назначает порядковый номер
	void setOrdinalNumber(int ordNum);
private:
	//! Имя действия
	std::string action_;

	//! Порядковый номер действия
	int ordinalNumber_;
	//! Указатель на узел действия в дереве
	CTreeListItem* treeListItem_;
};

#endif // !defined(AFX_UITREENODE_H__CB138F3B_7331_4D84_A8AB_7AA911ECD71A__INCLUDED_)
