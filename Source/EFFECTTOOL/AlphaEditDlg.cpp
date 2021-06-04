// AlphaEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "effecttool.h"
#include "AlphaEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlphaEditDlg dialog


CAlphaEditDlg::CAlphaEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlphaEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlphaEditDlg)
	//}}AFX_DATA_INIT

	m_bLockUpdate = false;
}

void CAlphaEditDlg::InitBrush()
{
	m_brSample.DeleteObject();
	m_brSample.CreateSolidBrush(RGB(m_alpha, m_alpha, m_alpha));

	GetDlgItem(IDC_SAMPLE)->Invalidate();
}


void CAlphaEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlphaEditDlg)
	DDX_Control(pDX, IDC_SLIDER, m_slider);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlphaEditDlg, CDialog)
	//{{AFX_MSG_MAP(CAlphaEditDlg)
	ON_WM_CTLCOLOR()
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlphaEditDlg message handlers

HBRUSH CAlphaEditDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if(pWnd->GetDlgCtrlID() == IDC_SAMPLE)
		return m_brSample;
	
	return hbr;
}


BOOL CAlphaEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitBrush();

	m_bLockUpdate = true;
	
	SetDlgItemInt(IDC_EDIT1, m_alpha);

	m_slider.SetRange(0, 255);
	m_slider.SetPos(m_alpha);

	m_bLockUpdate = false;

	return TRUE;
}

void CAlphaEditDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	m_bLockUpdate = true;

	m_alpha = m_slider.GetPos();
	SetDlgItemInt(IDC_EDIT1, m_alpha);

	m_bLockUpdate = false;

	InitBrush();
}
void CAlphaEditDlg::OnChangeEdit1() 
{
	if(m_bLockUpdate)
		return;

	BOOL _b;
	m_alpha = GetDlgItemInt(IDC_EDIT1, &_b);
	m_slider.SetPos(m_alpha);

	InitBrush();
}
