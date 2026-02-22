#pragma once

#include "CWnd.h"
class DlgAddChainGroup:public CDialog
{
public:
	int DoModal(HWND hParent);
	string name;
protected:
	LRESULT WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
	void OnInitDialog();
	void OnOk();
};