// xTreeListCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"
#include "xTreeListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// xTreeListCtrl

xTreeListCtrl::xTreeListCtrl()
{
}

xTreeListCtrl::~xTreeListCtrl()
{
}


BEGIN_MESSAGE_MAP(xTreeListCtrl, CTreeListCtrl)
	//{{AFX_MSG_MAP(xTreeListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// xTreeListCtrl message handlers

CTreeListItem* xTreeListCtrl::FindItemByData(CTreeListItem* pParent, DWORD_PTR dwData)
{
	while (CTreeListItem * pChild = GetChildItem(pParent))
	{
		if (pChild->GetData() == dwData)
			return pChild;

	}
	return NULL;
}

void xTreeListCtrl::DeleteSubItems(CTreeListItem* pParent)
{
	while (CTreeListItem * pChild = GetChildItem(pParent))
		DeleteItem(pChild);
}

CString xTreeListCtrl::GetAllItemText(CTreeListItem* pItem) 
{
	int const nColumns = GetColumnCount();
	CString res(pItem->GetText(0));
	for (int i = 1; i < nColumns; ++i)
	{
		res += _T("|");
		res += pItem->GetText(i);
	}
	return res;
}