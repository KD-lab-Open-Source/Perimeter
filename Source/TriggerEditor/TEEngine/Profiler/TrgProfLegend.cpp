
// TrgProfLegend.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TrgProfLegend.h"


// TrgProfLegend dialog

IMPLEMENT_DYNAMIC(TrgProfLegend, CDialog)
TrgProfLegend::TrgProfLegend(CWnd* pParent /*=NULL*/)
	: CDialog(TrgProfLegend::IDD, pParent)
{
}

TrgProfLegend::~TrgProfLegend()
{
}

void TrgProfLegend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TrgProfLegend, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// TrgProfLegend message handlers
void TrgProfLegend::initColors(COLORREF doned_link, COLORREF actve_link, 
				 COLORREF inactive_link, COLORREF doned_ele, 
				 COLORREF active_ele, COLORREF inactive_ele, COLORREF waiting_ele)
{
	m_brDonedLink.CreateSolidBrush(doned_link);
	m_brActiveLink.CreateSolidBrush(actve_link);
	m_brInactiveLink.CreateSolidBrush(inactive_link);

	m_brDonedEle.CreateSolidBrush(doned_ele);
	m_brActiveEle.CreateSolidBrush(active_ele);
	m_brInactiveEle.CreateSolidBrush(inactive_ele);
	m_brWaitingEle.CreateSolidBrush(waiting_ele);
}


HBRUSH TrgProfLegend::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch(pWnd->GetDlgCtrlID()) {
	case IDC_STATIC_DONED_LINK:
		pDC->SetBkMode(TRANSPARENT);
		return m_brDonedLink;
	case IDC_STATIC_ACTIVE_LINK:
		pDC->SetBkMode(TRANSPARENT);
		return m_brActiveLink;
	case IDC_STATIC_INACTIVE_LINK:
		pDC->SetBkMode(TRANSPARENT);
		return m_brInactiveLink;
	case IDC_STATIC_DONED_ELE:
		pDC->SetBkMode(TRANSPARENT);
		return m_brDonedEle;
	case IDC_STATIC_ACTIVE_ELE:
		pDC->SetBkMode(TRANSPARENT);
		return m_brActiveEle;
	case IDC_STATIC_INACTIVE_ELE:
		pDC->SetBkMode(TRANSPARENT);
		return m_brInactiveEle;
	case IDC_STATIC_WAITING_ELE:
		pDC->SetBkMode(TRANSPARENT);
		return m_brWaitingEle;
	}
	return hbr;
}

void TrgProfLegend::OnClose()
{
//	CDialog::OnClose();
	ShowWindow(SW_HIDE);
}
