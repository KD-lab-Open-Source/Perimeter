// BrushDialog.cpp : implementation file
//

#include "StdAfx.h"
#include "GeoTool.h"
#include "BrushDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrushDialog dialog


CBrushDialog::CBrushDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CBrushDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBrushDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBrushDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrushDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrushDialog, CDialog)
	//{{AFX_MSG_MAP(CBrushDialog)
	ON_BN_CLICKED(IDC_PIT_BRUSH, OnPitBrush)
	ON_BN_CLICKED(IDC_PIT_BRUSH_BIG, OnPitBrushBig)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrushDialog message handlers

void CBrushDialog::OnPitBrush() 
{
	theApp.setBrush(VMapOperator::PIT_BRUSH);
}

void CBrushDialog::OnPitBrushBig() 
{
	theApp.setBrush(VMapOperator::PIT_BRUSH_BIG);
}


BOOL CBrushDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CBitmap bmp;
	bmp.LoadBitmap(IDB_PIT_BRUSH);
	CButton* btn = static_cast<CButton*>(GetDlgItem(IDC_PIT_BRUSH));
	btn->SetBitmap(bmp);
	bmp.Detach();
	btn->SetCheck(1);

	bmp.LoadBitmap(IDB_PIT_BRUSH_BIG);
	static_cast<CButton*>(GetDlgItem(IDC_PIT_BRUSH_BIG))->SetBitmap(bmp);
	bmp.Detach();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
