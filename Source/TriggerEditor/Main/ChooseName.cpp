// ChooseName.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"
#include "ChooseName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ChooseName dialog
std::pair<const CString, bool> AskUserForName(LPCTSTR lpszDef)
{
	ChooseName dlg;
	dlg.set_str(lpszDef);
	bool res = (dlg.DoModal() == IDOK);
	return std::make_pair(dlg.get_str(), res);
}

ChooseName::ChooseName(CWnd* pParent /*=NULL*/)
	: CDialog(ChooseName::IDD, pParent)
{
	//{{AFX_DATA_INIT(ChooseName)
	m_strName = _T("");
	//}}AFX_DATA_INIT
}


void ChooseName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ChooseName)
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ChooseName, CDialog)
	//{{AFX_MSG_MAP(ChooseName)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ChooseName message handlers

void ChooseName::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	CEdit* ptrE = (CEdit*)GetDlgItem(IDC_EDIT_NAME);
	ptrE->SetFocus();
	ptrE->SetSel(0, -1);
}

void ChooseName::OnOK() 
{
	UpdateData();
	m_strName.TrimLeft();
	m_strName.TrimRight();
	CDialog::OnOK();
}
