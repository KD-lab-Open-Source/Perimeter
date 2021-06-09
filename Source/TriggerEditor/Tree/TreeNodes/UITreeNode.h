// UITreeNode.h: interface for the UITreeNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UITREENODE_H__CB138F3B_7331_4D84_A8AB_7AA911ECD71A__INCLUDED_)
#define AFX_UITREENODE_H__CB138F3B_7331_4D84_A8AB_7AA911ECD71A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tree/TreeNodes/ITreeNode.h"
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

	//! �������� ���� � ������
	virtual bool load(xTreeListCtrl& tree, CTreeListItem* pParent);
	//! ��������� ������ �� �������  ����
	virtual bool onCommand(TETreeLogic& logic, WPARAM wParam, LPARAM lParam);
	//! ��������� ����� ��������������
	virtual void onBeginDrag(TETreeLogic& logic);
	//! ���������� ��������� �� �������� ����
	virtual void onDeleteItem(TETreeLogic& logic);
protected:
	//! ���������� ��� ��������
	std::string const& getAction() const;
	//! ��������� ��� ��������
	void setAction(std::string const& action);
	//! ���������� ���� �������� � ������
	CTreeListItem* getTreeListItem() const;
	//! ��������� ���� �������� � ������
	void setTreeListItem(CTreeListItem* pItem);
	//! ���������� ���������� ����� ��������
	int getOrdinalNumber() const;
	//! ��������� ���������� �����
	void setOrdinalNumber(int ordNum);
private:
	//! ��� ��������
	std::string action_;

	//! ���������� ����� ��������
	int ordinalNumber_;
	//! ��������� �� ���� �������� � ������
	CTreeListItem* treeListItem_;
};

#endif // !defined(AFX_UITREENODE_H__CB138F3B_7331_4D84_A8AB_7AA911ECD71A__INCLUDED_)
