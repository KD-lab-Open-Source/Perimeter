#pragma once

#include "TETreeNotifyListener.h"

class TETreeManager;

class TETreeLogic : public TETreeNotifyListener
{
public:
	TETreeLogic();
	~TETreeLogic(void);

	virtual bool onNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult);
	virtual bool onCommand(WPARAM wParam, LPARAM lParam);
	void setTreeManager(TETreeManager* treeManager);

	bool init();
	bool loadTree();
protected:
	bool loadTree(xTreeListCtrl& tree);
	//выравниваем ширину столбцов по тексту
	void adjustTreeColumnsWidth();
protected:
	//нотификация
	void onRClick(NMHDR* phdr, LRESULT *pResult);
	void onDeleteItem(NMHDR* phdr, LRESULT *pResult);
	void onKeyDown(NMHDR* phdr, LRESULT *pResult);
	void onBeginDrag(NMHDR* phdr, LRESULT *pResult);
private:
	//! Оконное представление дерева
	TETreeManager *treeManager_;
};
