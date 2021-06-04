#include "StdAfx.h"
#include "DebugDlg.h" 
#include "resource.h"

bool dbg_show_position=false;
bool dbg_show_rotation=false;
bool dbg_show_scale=false;
bool dbg_show_info_polygon=false;

int DlgDebug::DoModal(HWND hParent)
{
	return CDialog::DoModal(IDD_DEBUG_DIALOG,hParent);
}

LRESULT DlgDebug::WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			OnOk();
			EndDialog(hwnd,IDOK);
			break;
		case IDCANCEL:
			EndDialog(hwnd,IDCANCEL);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd,IDCANCEL);
		break;
	}
	return CDialog::WindowProc(uMsg,wParam,lParam);
}

void DlgDebug::OnInitDialog()
{
	CheckDlgButton(hwnd,IDC_LOG_ROTATION,dbg_show_rotation?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(hwnd,IDC_LOG_POSITION,dbg_show_position?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(hwnd,IDC_LOG_SCALE,dbg_show_scale?BST_CHECKED:BST_UNCHECKED);

	CheckDlgButton(hwnd,IDC_LOG_INFO_POLYGON,dbg_show_info_polygon?BST_CHECKED:BST_UNCHECKED);
	
}

void DlgDebug::OnOk()
{
	dbg_show_rotation=IsDlgButtonChecked(hwnd,IDC_LOG_ROTATION)==BST_CHECKED;
	dbg_show_position=IsDlgButtonChecked(hwnd,IDC_LOG_POSITION)==BST_CHECKED;
	dbg_show_scale=IsDlgButtonChecked(hwnd,IDC_LOG_SCALE)==BST_CHECKED;

	dbg_show_info_polygon=IsDlgButtonChecked(hwnd,IDC_LOG_INFO_POLYGON)==BST_CHECKED;
}
