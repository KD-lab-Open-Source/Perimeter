#pragma once
#include "afxwin.h"

class CTreeListCtrl;

//данные из контрола должны получаться через метод GetWindowText
template<class Base>
class EditItemCtrlBase :public Base
{
public:
	EditItemCtrlBase(void):
	  m_pTreeListCtrl( NULL )
	  {}
	  virtual ~EditItemCtrlBase(void){}
protected:
	//! Должна быть вызвана из порожденного класса
	/*!
		Устанавливает указатель на TreeListCtrl
	*/
	void OnKillFocus(CWnd* pNewWnd)
	{
		Base::OnKillFocus(pNewWnd);

		m_pTreeListCtrl->SetCtrlFocus( pNewWnd, FALSE );
	}

protected:
	void setTreeListCtrl(CTreeListCtrl* p){
		m_pTreeListCtrl = p;
	}
	CTreeListCtrl* getTreeListCtrl(){
		return m_pTreeListCtrl;
	}
private:
	CTreeListCtrl*	m_pTreeListCtrl;
};
