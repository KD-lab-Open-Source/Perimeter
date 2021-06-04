#include "stdafx.h"
#include "resource.h"
#include "ScaleBar.h"
#include "Utils/AdjustComboboxWidth.h"


//должны совпадать, чтобы текст подсказки был правильный
int const COMBO_ID = IDS_SCALE_COMBO_TOOLTIP_TEXT;

CScaleBar::CScaleBar():
m_pScalable(NULL)
{}

BEGIN_MESSAGE_MAP(CScaleBar, CToolBar)
	ON_CBN_SELCHANGE( COMBO_ID, OnComboSelChange)
	ON_COMMAND(ID_TBTN_SCALE_MINUS, OnScaleMinus)
	ON_COMMAND(ID_TBTN_SCALE_PLUS, OnScalePlus)
	ON_UPDATE_COMMAND_UI(ID_TBTN_SCALE_MINUS, OnUpdateScaleMinus)
	ON_UPDATE_COMMAND_UI(ID_TBTN_SCALE_PLUS,  OnUpdateScalePlus)
END_MESSAGE_MAP()

void CScaleBar::AddCombo()
{
	CRect r;
	GetToolBarCtrl().GetItemRect(3, r);
	r.right = r.left + r.Width()*3;
	r.left -= 2;
	r.bottom += 100;
	m_wndCombo.Create(WS_VISIBLE|WS_CHILD|WS_VSCROLL|CBS_DROPDOWN, r, this, COMBO_ID);
	m_font.Attach( ::GetStockObject( DEFAULT_GUI_FONT ));
	m_wndCombo.SetFont(&m_font, FALSE);
}

bool CScaleBar::Create(CWnd* pparent, DWORD id)
{
	if (!CreateEx(pparent, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, 
		CRect(0, 0, 0, 0), id))
		return false;

	if (!LoadToolBar(IDR_TOOLBAR_SCALE))
		return false;

	AddCombo();
	InitCombo();	
	return true;
}

void CScaleBar::SetScalable(IScalable* psclbl)
{
	m_pScalable = psclbl;
}

void CScaleBar::UpdateScaleInfo(){
	ASSERT(m_pScalable);
	int scale = static_cast<int>(m_pScalable->CurrentScale()*100);
	CString str;
	str.Format(IDS_SCALE_INFO_FORMAT, scale);
	m_wndCombo.SetWindowText(str);
}

void CScaleBar::OnComboSelChange()
{
	int iCurSel = m_wndCombo.GetCurSel();
	if (iCurSel != CB_ERR)
	{
		DWORD_PTR dwData = m_wndCombo.GetItemData(iCurSel);
		ASSERT(m_pScalable);
		m_pScalable->SetScale(dwData/100.f);
	}
}

void CScaleBar::InitCombo()
{
	int i = m_wndCombo.AddString(_T("500%"));
	if (i != CB_ERR)
		m_wndCombo.SetItemData(i, 500);
	i = m_wndCombo.AddString(_T("200%"));
	if (i != CB_ERR)
		m_wndCombo.SetItemData(i, 200);
	i = m_wndCombo.AddString(_T("100%"));
	if (i != CB_ERR)
		m_wndCombo.SetItemData(i, 100);
	i = m_wndCombo.AddString(_T("75%"));
	if (i != CB_ERR)
		m_wndCombo.SetItemData(i, 75);
	i = m_wndCombo.AddString(_T("50%"));
	if (i != CB_ERR)
		m_wndCombo.SetItemData(i, 50);
	i = m_wndCombo.AddString(_T("25%"));
	if (i != CB_ERR)
		m_wndCombo.SetItemData(i, 25);
	m_wndCombo.SelectString(-1, _T("100%"));
	AdjustComboboxWidth::adjust(m_wndCombo);
}

void CScaleBar::OnScaleMinus()
{
	m_pScalable->DecreaseScale();	
}

void CScaleBar::OnScalePlus()
{
	m_pScalable->IncreaseScale();
}

void CScaleBar::OnUpdateScaleMinus(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pScalable != NULL);
	m_wndCombo.EnableWindow(m_pScalable != NULL);
}

void CScaleBar::OnUpdateScalePlus(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pScalable != NULL);
}


BOOL CScaleBar::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
	{
		if (::GetParent(pMsg->hwnd) == m_wndCombo.m_hWnd)
		{
			ReadAndSetScale();
			return TRUE;
		}
	}
	return CToolBar::PreTranslateMessage(pMsg);
}

void CScaleBar::ReadAndSetScale()
{
	CString str;
	m_wndCombo.GetWindowText(str);

	int pos = str.Find(_T('%'));
	if (pos != -1)
		str = str.Left(pos);
	str.TrimLeft();
	str.TrimRight();
	TCHAR* end = NULL;
	int scale = _tcstol(str, &end, 10);
	if   (*end != _T('\0')||scale <= 0)
	{
		AfxMessageBox(IDS_INVALID_SCALE_INPUT);
		UpdateScaleInfo();
		return;
	}
	m_pScalable->SetScale(scale/100.f);
}
