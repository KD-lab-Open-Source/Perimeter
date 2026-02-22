#pragma once

#include "CWnd.h"
class DlgAddAnimationChain:public CDialog
{
public:
	DlgAddAnimationChain();

	int DoModal(HWND hParent);

	string name;
	int begin_frame;
	int end_frame;
	float delta_time;
	bool cycled;
protected:
	LRESULT WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
	void OnInitDialog();
	void OnOk();
};
