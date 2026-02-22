// TreeListEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TreeListDC.h"
#include "TreeListHeaderCtrl.h"
#include "TreeListTipCtrl.h"
#include "TreeListStaticCtrl.h"
#include "TreeListEditCtrl.h"
#include "TreeListComboCtrl.h"
#include "TreeListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeListEditCtrl

CTreeListEditCtrl::CTreeListEditCtrl() : 
	m_pTreeListCtrl( NULL )
{
}

CTreeListEditCtrl::~CTreeListEditCtrl()
{
}

BEGIN_MESSAGE_MAP(CTreeListEditCtrl, CEdit)
	//{{AFX_MSG_MAP(CTreeListEditCtrl)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListEditCtrl message handlers
int CTreeListEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pTreeListCtrl = (CTreeListCtrl*)GetParent();

	return 0;
}

void CTreeListEditCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);

	m_pTreeListCtrl->SetCtrlFocus( pNewWnd, FALSE );
}
