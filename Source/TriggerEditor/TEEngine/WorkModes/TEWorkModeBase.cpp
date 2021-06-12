#include "stdafx.h"
#include "resource.h"
#include "teworkmodebase.h"
#include "../TriggerEditorView.h"

namespace{
	inline bool IsKeyPressed(int keyID)
	{
		return static_cast<bool>(GetAsyncKeyState(keyID)>>15);
	}
}
TEWorkModeBase::TEWorkModeBase(void)
: m_bCanDragContent(false)
, m_base_point(0, 0)
, m_hCursor(NULL)
{
	m_hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}

TEWorkModeBase::~TEWorkModeBase(void)
{
}

void TEWorkModeBase::setBasePoint(CPoint const& point){
	m_base_point = point;
}

void TEWorkModeBase::setCanDragContent(bool bVal){
	m_bCanDragContent = bVal;
}

void TEWorkModeBase::setCursor(HCURSOR hCursor)
{
	m_hCursor = hCursor;
	SetCursor(m_hCursor);
}

void TEWorkModeBase::OnLButtonDown(TriggerEditorView* source, UINT nFlags, CPoint point)
{
	if (getCanDragContent()&&nFlags&MK_LBUTTON){
		setBasePoint(point);
		setCursor(AfxGetApp()->LoadCursor(IDC_CUR_DRAG_HAND));
		source->SetCapture();
	}
}
void TEWorkModeBase::OnLButtonUp(TriggerEditorView* source, UINT nFlags, CPoint point)
{
	if(getCanDragContent())
	{
		ReleaseCapture();
		setCursor(AfxGetApp()->LoadCursor(IDC_CUR_HAND));
		source->updateScrollers();
		source->Invalidate();
		return;
	}
}

void TEWorkModeBase::OnLButtonDblClk(TriggerEditorView* source, UINT nFlags, CPoint point)
{

}

void TEWorkModeBase::OnRButtonDown(TriggerEditorView* source, UINT nFlags, CPoint point)
{

}

void TEWorkModeBase::OnRButtonUp(TriggerEditorView* source, UINT nFlags, CPoint point)
{

}

void TEWorkModeBase::OnMouseMove(TriggerEditorView* source, UINT nFlags, CPoint point)
{
	if(getCanDragContent()&&(nFlags&MK_LBUTTON))
	{
		CPoint diff(point - getBasePoint());
		source->scrollView(diff.x, diff.y);
		source->extendWorkArea();
		source->updateScrollers();
		setBasePoint(point);
	}
	else
		trackToolTip(source, point);
}

void TEWorkModeBase::OnKeyDown(TriggerEditorView* source, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!getCanDragContent()&&nChar == VK_CONTROL)
	{
		if(!IsKeyPressed(VK_SHIFT)&&!IsKeyPressed(VK_LBUTTON)&&!IsKeyPressed(VK_RBUTTON))
		{
			setCanDragContent(true);
			setCursor(AfxGetApp()->LoadCursor(IDC_CUR_HAND));
		}
	}
	else if(m_bCanDragContent&&nChar == VK_SHIFT)
	{
		setCanDragContent(false);
		setCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		ReleaseCapture();
	}
}

void TEWorkModeBase::OnKeyUp(TriggerEditorView* source, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(getCanDragContent()&&nChar == VK_CONTROL)
	{
		setCanDragContent(false);
		setCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		ReleaseCapture();
	}
	else if(!getCanDragContent()&&nChar == VK_SHIFT)
	{
		if(IsKeyPressed(VK_CONTROL)&&!IsKeyPressed(VK_LBUTTON)&&!IsKeyPressed(VK_RBUTTON))
		{
			setCanDragContent(true);
			setCursor(AfxGetApp()->LoadCursor(IDC_CUR_HAND));
		}
	}
}

void TEWorkModeBase::OnPaint(TriggerEditorView* source, CDC* pdc, CRect const& rcPaint)
{

}

BOOL TEWorkModeBase::OnSetCursor(TriggerEditorView* source, CWnd* pWnd, 
								 UINT nHitTest, UINT message)
{
	SetCursor(getCursor());
	return TRUE;
}

BOOL TEWorkModeBase::OnCommand(TriggerEditorView* source, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

void TEWorkModeBase::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis)
{

}

void TEWorkModeBase::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis)
{

}

void TEWorkModeBase::OnShowWindow(TriggerEditorView* source, BOOL bShow, UINT nStatus)
{
}

void TEWorkModeBase::ModifyMenu(CMenu* pmenu, UINT menuResId, TriggerEditorView* source)
{

}

BOOL TEWorkModeBase::trackPopupMenu(UINT id, TriggerEditorView* source)
{
	CMenu menu;
	menu.LoadMenu(id);
	CPoint point;
	GetCursorPos(&point);
	BOOL res = FALSE;
	ModifyMenu(menu.GetSubMenu(0), id, source);

	if((res = menu.GetSubMenu(0)->TrackPopupMenu(
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,point.x, point.y, source)))
	{
		//если указать TPM_RETURNCMD, то эта дура не шлет нотификацию
		//но че нам кабанам, сами пошлем:)
		source->SendMessage(WM_COMMAND, (WPARAM)res, NULL);
	}
	return res;
}

void TEWorkModeBase::trackToolTip(TriggerEditorView* source, CPoint point)
{

}

bool TEWorkModeBase::OnStart()
{
	return true;
}
bool TEWorkModeBase::OnEnd()
{
	return true;
}

BOOL TEWorkModeBase::OnDropData(TriggerEditorView* source, 
								void* pData, const CPoint& point)
{
	return FALSE;
}

bool TEWorkModeBase::CanDrop(TriggerEditorView* source, 
							 void* pData, const CPoint& point)
{
	return false;
}

bool TEWorkModeBase::OnCommandUpdateUI(TriggerEditorView *source, CCmdUI* pCmdUI)
{
	return false;
}