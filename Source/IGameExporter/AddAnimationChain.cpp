#include "StdAfx.h"
#include "AddAnimationChain.h"
#include "resource.h"

DlgAddAnimationChain::DlgAddAnimationChain()
{
	name="chain";
	begin_frame=0;
	end_frame=1000;
	delta_time=1.0f;
	cycled=false;
}

int DlgAddAnimationChain::DoModal(HWND hParent)
{
	return CDialog::DoModal(IDD_ADD_ANIMATION_CHAIN,hParent);
}

LRESULT DlgAddAnimationChain::WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
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

void DlgAddAnimationChain::OnInitDialog()
{
	SetWindowText(GetDlgItem(hwnd,IDC_PAC_NAME),name.c_str());

	int max_frame=pRootExport->GetNumFrames()-1;
	ISpinnerControl * spin;

	spin = GetISpinner(GetDlgItem(hwnd, IDC_PAC_BEGIN_TIME_SPIN)); 
	spin->LinkToEdit(GetDlgItem(hwnd,IDC_PAC_BEGIN_TIME), EDITTYPE_INT ); 
	spin->SetLimits(0, max_frame, TRUE); 
	spin->SetScale(1.0f);
	spin->SetValue(begin_frame ,FALSE);
	ReleaseISpinner(spin);

	spin = GetISpinner(GetDlgItem(hwnd, IDC_PAC_END_TIME_SPIN)); 
	spin->LinkToEdit(GetDlgItem(hwnd,IDC_PAC_END_TIME), EDITTYPE_INT ); 
	spin->SetLimits(0, max_frame, TRUE); 
	spin->SetScale(1.0f);
	spin->SetValue(min(end_frame,max_frame) ,FALSE);
	ReleaseISpinner(spin);

	spin = GetISpinner(GetDlgItem(hwnd, IDC_PAC_DELTA_TIME_SPIN)); 
	spin->LinkToEdit(GetDlgItem(hwnd,IDC_PAC_DELTA_TIME), EDITTYPE_FLOAT ); 
	spin->SetLimits(0, 20, TRUE); 
	spin->SetScale(0.1f);
	spin->SetValue(delta_time ,FALSE);
	ReleaseISpinner(spin);

	CheckDlgButton(hwnd,IDC_CHAIN_CYCLED,cycled?BST_CHECKED:BST_UNCHECKED);
	
}

void DlgAddAnimationChain::OnOk()
{
	char str_name[256];
	str_name[0]=0;
	GetWindowText(GetDlgItem(hwnd,IDC_PAC_NAME),str_name,sizeof(str_name)/sizeof(str_name[0]));
	name=str_name;
	ISpinnerControl * spin;
	spin = GetISpinner(GetDlgItem(hwnd, IDC_PAC_BEGIN_TIME_SPIN)); 
	begin_frame = spin->GetIVal(); 
	ReleaseISpinner(spin);

	spin = GetISpinner(GetDlgItem(hwnd, IDC_PAC_END_TIME_SPIN)); 
	end_frame = spin->GetIVal(); 
	ReleaseISpinner(spin);

	spin = GetISpinner(GetDlgItem(hwnd, IDC_PAC_DELTA_TIME_SPIN)); 
	delta_time = spin->GetFVal(); 
	ReleaseISpinner(spin);

	cycled=IsDlgButtonChecked(hwnd,IDC_CHAIN_CYCLED)==BST_CHECKED;
}
