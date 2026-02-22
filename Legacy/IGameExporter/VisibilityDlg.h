#pragma once
#include "CWnd.h"

class VisibilityDlg:public CDialog
{
public:
	VisibilityDlg();
	int DoModal(HWND hParent);

	AnimationVisibleGroup chain_group;
protected:
	LRESULT WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam);

	RECT rc_main_base;
	HWND hTreeObject;
	RECT rcTree;
	HWND hOk;
	RECT rcOk;
	HWND hName;

	void OnInitDialog();
	void OnOk();
	void OnSize();

	void BuildTreeObject();
	void AddTreeNode(IGameNode* current,IGameNode* parent,HTREEITEM hparent);
	void SaveObjectTree(HTREEITEM hparent=TVI_ROOT);
};