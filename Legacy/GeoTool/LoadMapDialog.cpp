// LoadMapDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GeoTool.h"
#include "LoadMapDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoadMapDialog dialog


CLoadMapDialog::CLoadMapDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadMapDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoadMapDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLoadMapDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadMapDialog)
	DDX_Control(pDX, IDC_LIST2, mapList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadMapDialog, CDialog)
	//{{AFX_MSG_MAP(CLoadMapDialog)
	ON_LBN_SELCHANGE(IDC_LIST2, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadMapDialog message handlers

BOOL CLoadMapDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!vMap.wTable) {
		VMapOperator::prepareVMap();
	}
	for (int i = 0; i < vMap.maxWorld; i++) {
		mapList.AddString(vMap.wTable[i].name);
	}
	mapList.SetCurSel(0);
	selectedMap = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CLoadMapDialog::getSelectedMap() {
	return selectedMap;
}

void CLoadMapDialog::OnSelchange() 
{
	selectedMap = mapList.GetCurSel();
}
