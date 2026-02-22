// DlgBar.cpp : implementation file
//

#include "stdafx.h"
#include "effecttool.h"
#include "DlgBar.h"

#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBar dialog


CDlgBar::CDlgBar(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bInit = false;
}


void CDlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBar)
	DDX_Control(pDX, IDC_FPS, m_fps);
	DDX_Control(pDX, IDC_COMBO_POS_TYPE, m_cbType);
	DDX_Control(pDX, IDC_COMBO_OUT_TYPE, m_cbOut);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBar, CDialog)
	//{{AFX_MSG_MAP(CDlgBar)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_COMMAND(IDOK, OnCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBar message handlers

LRESULT CDlgBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT lResult;
	switch (message)
	{
	case WM_NOTIFY:
	case WM_COMMAND:
		// send these messages to the owner if not handled
		if (OnWndMsg(message, wParam, lParam, &lResult))
			return lResult;
		else
		{
			// try owner next
			lResult = GetOwner()->SendMessage(message, wParam, lParam);
		}

		return lResult;
	}

	
	return CDialog::WindowProc(message, wParam, lParam);
}
LRESULT CDlgBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
	CFrameWnd* pTarget = (CFrameWnd*)AfxGetMainWnd();
	ASSERT(pTarget);

	UpdateDialogControls(pTarget, (BOOL)wParam);
	return 0L;
}

void CDlgBar::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if(m_bInit)
	{
		CRect rcBtn, rcBar;
		GetClientRect(rcBar);
		GetDlgItem(IDC_VIEW_MODE)->GetClientRect(rcBtn);
			
		GetDlgItem(IDC_VIEW_MODE)->SetWindowPos(0, rcBar.Width() - rcBtn.Width() - 2, 2, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
	}
}

BOOL CDlgBar::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_cbOut.SetCurSel(0);
	m_cbType.SetCurSel(0);

	m_bInit = true;
	
	return TRUE;
}

void CDlgBar::SetFps(float fps)
{
	CString s;
	s.Format("%.f fps",fps);
	m_fps.SetWindowText(s);
}
