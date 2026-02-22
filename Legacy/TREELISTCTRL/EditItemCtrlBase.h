#pragma once
#include "afxwin.h"

class CTreeListCtrl;

//данные из контрола должны получаться через метод GetWindowText
class EditItemCtrlBase :public CWnd
{
public:
	EditItemCtrlBase(void);
	virtual ~EditItemCtrlBase(void);
protected:
	//{{AFX_MSG(CTreeListEditCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
protected:
	void setTreeListCtrl(CTreeListCtrl* p);
	CTreeListCtrl* getTreeListCtrl(){
		return m_pTreeListCtrl;
	}
private:
	CTreeListCtrl*	m_pTreeListCtrl;
};
