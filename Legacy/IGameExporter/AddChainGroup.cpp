#include "StdAfx.h"
#include "AddChainGroup.h" 
#include "resource.h"

int DlgAddChainGroup::DoModal(HWND hParent)
{
	return CDialog::DoModal(IDD_ADD_CHAIN_GROUP,hParent);
}

LRESULT DlgAddChainGroup::WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
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

void DlgAddChainGroup::OnInitDialog()
{
	SetWindowText(GetDlgItem(hwnd,IDC_ACG_NAME),name.c_str());
}

void DlgAddChainGroup::OnOk()
{
	char str_name[256];
	str_name[0]=0;
	GetWindowText(GetDlgItem(hwnd,IDC_ACG_NAME),str_name,sizeof(str_name)/sizeof(str_name[0]));
	name=str_name;
}
