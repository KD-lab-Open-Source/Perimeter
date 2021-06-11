#include "xTreeListCtrl\xTreeCtrlLib.h"

CTreeListCtrl::CTreeListCtrl()
{

}

CTreeListCtrl::~CTreeListCtrl()
{

}

DWORD	CTreeListCtrl::SetStyle(DWORD dwStyle)
{
    return 0;
}
DWORD	CTreeListCtrl::GetStyle() const
{
    return 0;
}

void		CTreeListCtrl::SetItemData(int nItem, DWORD_PTR dwData)
{

}

CRuntimeClass* CTreeListCtrl::GetRuntimeClass() const
{
    return NULL;
}

struct AFX_MSGMAP const* CTreeListCtrl::GetMessageMap() const
{
    return NULL;
}

CTreeListItem* CTreeListCtrl::GetChildItem(CTreeListItem const* pItem) const
{
    return NULL;
}


CTreeListItem* CTreeListCtrl::GetSelectedItem() const
{
    return NULL;
}

void	  CTreeListCtrl::SetItemData(CTreeListItem* pItem, DWORD_PTR dwData)
{

}


CTreeListItem* CTreeListCtrl::InsertItem(LPCTSTR lpszItem, CTreeListItem* pParent /*= TLI_ROOT*/, CTreeListItem* pInsertAfter /*= TLI_LAST*/)
{
    return NULL;
}


int		CTreeListCtrl::GetColumnCount()
{
    return 0;
}

bool	CTreeListCtrl::InsertColumn(LPCTSTR lpszColumnHeading, DWORD dwFormat /*= TLF_DEFAULT_LEFT*/, int nWidth /*= 0*/, int iImage /*= -1*/, int nMin /*= 0*/, int nMax /*= 0*/)
{
    return false;
}

bool			CTreeListCtrl::DeleteItem(CTreeListItem* pItem /*= TLI_ROOT*/)
{
    return false;
}


void		CTreeListCtrl(int nItem, DWORD_PTR dwData)
{

}

int		CTreeListCtrl::SetColumnWidth(int nCol, int nWidth, int nMin /*= 0*/, int nMax /*= 0*/)
{
    return 0;
}

LPCTSTR		CTreeListItem::GetText(int nIndex /*= 0*/) const
{
    return NULL;
}

DWORD_PTR	CTreeListItem::GetData() const
{
    return 0;
}


CTreeListComboCtrl::CTreeListComboCtrl()
{

}

CTreeListComboCtrl::~CTreeListComboCtrl()
{

}


struct AFX_MSGMAP const* CTreeListComboCtrl::GetMessageMap() const
{
    return NULL;
}


struct AFX_MSGMAP const* CTreeListCtrl::GetThisMessageMap()
{
    return NULL;
}


CTreeListEditCtrl::CTreeListEditCtrl()
{

}

CTreeListEditCtrl::~CTreeListEditCtrl()
{

}


struct AFX_MSGMAP const* CTreeListEditCtrl::GetMessageMap() const
{
    return NULL;
}


CTreeListStaticCtrl::CTreeListStaticCtrl()
{

}

CTreeListStaticCtrl::~CTreeListStaticCtrl()
{

}



struct AFX_MSGMAP const* CTreeListStaticCtrl::GetMessageMap() const
{
    return NULL;
}


CTreeListTipCtrl::CTreeListTipCtrl()
{

}

CTreeListTipCtrl::~CTreeListTipCtrl()
{

}



struct AFX_MSGMAP const* CTreeListTipCtrl::GetMessageMap() const
{
    return NULL;
}


CTreeListHeaderCtrl::CTreeListHeaderCtrl()
{

}

CTreeListHeaderCtrl::~CTreeListHeaderCtrl()
{

}


struct AFX_MSGMAP const* CTreeListHeaderCtrl::GetMessageMap() const
{
    return NULL;
}


int CTreeListCtrl::PreTranslateMessage(struct tagMSG*)
{
    return 0;
}

int CTreeListTipCtrl::PreTranslateMessage(struct tagMSG*)
{
    return 0;
}

int CTreeListStaticCtrl::PreTranslateMessage(struct tagMSG*)
{
    return 0;
}


void CTreeListCtrl::PreSubclassWindow()
{
}

void CTreeListHeaderCtrl::PreSubclassWindow()
{
}