#pragma once
#include "CWnd.h"

class DlgAnimationChain:public CDialog
{
public:
	DlgAnimationChain();
	int DoModal(HWND hParent);
protected:
	LRESULT WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam);

	vector<AnimationChain> animation_chain;
	vector<AnimationVisibleGroup> animation_chain_group;

	RECT rc_main_base;

	HWND hListChain;
	RECT rcListChain;
	HWND hTreeGroup;
	RECT rcTreeGroup;

	enum  
	{
		LS_NAME,
		LS_BEGIN,
		LS_END,
		LS_TIME,
		LS_CYCLED
	};

	void OnInitDialog();
	void OnOk();
	void OnSize();

	void OnAddChain();
	void OnDeleteChain();
	void OnEditChain(LPNMITEMACTIVATE pnm);
	void OnKeyChain(LPNMLVKEYDOWN pnm);

	void SetItem(int index,bool insert);

	void OnAddChainGroup();
	void OnDeleteChainGroup();

	HTREEITEM FindChainGroup(const char* name,int* index=NULL);
	HTREEITEM GetSelectionGroup(int* index=NULL);

	void OnEditChainGroup(LPNMITEMACTIVATE pnm);
	void OnKeyChainGroup(LPNMTVKEYDOWN pnm);
	void OnEditVisibility();
};