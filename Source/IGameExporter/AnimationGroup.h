#pragma once

#include "CWnd.h"
class DlgAnimationGroup:public CDialog
{
public:
	DlgAnimationGroup();

	int DoModal(HWND hParent);
protected:
	LRESULT WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam);

	RECT rc_main_base;
	HWND hTreeObject;
	RECT rcTreeObject;
	HWND hTreeGroup;
	RECT rcTreeGroup;
	HWND hGroupName;

	vector<AnimationGroup> animation_group;

	void OnInitDialog();
	void OnOk();
	void OnSize();
	void OnAddGroup();
	void OnAddObjectInGroup();
	void OnAddObjectInGroupSubtree();
	void OnDeleteGroup();

	void BuildTreeObject();
	void BuildTreeGroup();
	void AddTreeNode(IGameNode* cur,IGameNode* parent,HTREEITEM hparent);

	void FindAndDeleteGroupObject(IGameNode* pGameNode,string exclude_group_name);
	bool DeleteGroupObject(IGameNode* pGameNode,string group_name);

	void AddObjectInGroup(HTREEITEM hgroup,HTREEITEM hobject);
	void AddObjectInGroup(HTREEITEM hgroup,IGameNode* pGameNode);
	void AddObjectInGroupRecursive(HTREEITEM hgroup,HTREEITEM hobject);
	void UpdateObjectTree(HTREEITEM hobject=TVI_ROOT);
	HTREEITEM AddGroup(string name);
	
	HTREEITEM GetSelectionGroup();
};