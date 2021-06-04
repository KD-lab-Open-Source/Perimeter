#pragma once

#include "CWnd.h"
class DlgDebug:public CDialog
{
public:
	int DoModal(HWND hParent);
protected:
	LRESULT WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
	void OnInitDialog();
	void OnOk();
};

extern bool dbg_show_position;
extern bool dbg_show_rotation;
extern bool dbg_show_scale;
extern bool dbg_show_info_polygon;
