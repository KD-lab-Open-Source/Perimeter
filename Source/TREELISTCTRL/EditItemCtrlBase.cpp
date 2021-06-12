#include "StdAfx.h"
#include "edititemctrlbase.h"
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


EditItemCtrlBase::EditItemCtrlBase(void):
m_pTreeListCtrl( NULL )
{
}

EditItemCtrlBase::~EditItemCtrlBase(void)
{
}

BEGIN_MESSAGE_MAP(EditItemCtrlBase, CWnd)
	//{{AFX_MSG_MAP(EditItemCtrlBase)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void EditItemCtrlBase::setTreeListCtrl(CTreeListCtrl* p){
	m_pTreeListCtrl = p;
}

/////////////////////////////////////////////////////////////////////////////
// EditItemCtrlBase message handlers
int EditItemCtrlBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pTreeListCtrl = reinterpret_cast<CTreeListCtrl*>(GetParent());

	return 0;
}

void EditItemCtrlBase::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

	m_pTreeListCtrl->SetCtrlFocus( pNewWnd, FALSE );
}
