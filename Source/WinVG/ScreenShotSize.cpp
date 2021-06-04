// ScreenShotSize.cpp : implementation file
//

#include "stdafx.h"
#include "WinVG.h"
#include "ScreenShotSize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScreenShotSize dialog


CScreenShotSize::CScreenShotSize(CWnd* pParent /*=NULL*/)
	: CDialog(CScreenShotSize::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScreenShotSize)
	x = 1024;
	//}}AFX_DATA_INIT
}


void CScreenShotSize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScreenShotSize)
	DDX_Text(pDX, IDC_EDITX, x);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScreenShotSize, CDialog)
	//{{AFX_MSG_MAP(CScreenShotSize)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreenShotSize message handlers
