/********************************************************************
	created:	2003/05/21
	created:	21:5:2003   15:30
	filename: 	d:\Projects\Quest\QuestEditor\ITEWorkMode.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	ITEWorkMode
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	Интерфейс рабочего режима для редактора триггеров
*********************************************************************/
#pragma once

class TriggerEditorView;
class TriggerEditorLogic;

interface ITEWorkMode
{
	virtual void OnLButtonDown(TriggerEditorView* source, 
								UINT nFlags, CPoint point)					= 0;
	virtual void OnLButtonUp(TriggerEditorView* source, 
								UINT nFlags, CPoint point)					= 0;
	virtual void OnLButtonDblClk(TriggerEditorView* source, 
								UINT nFlags, CPoint point)					= 0;

	virtual void OnRButtonDown(TriggerEditorView* source, 
								UINT nFlags, CPoint point)					= 0;
	virtual void OnRButtonUp(TriggerEditorView* source, 
								UINT nFlags, CPoint point)					= 0;

	virtual void OnMouseMove(TriggerEditorView* source, 
								UINT nFlags, CPoint point)					= 0;
	virtual BOOL OnSetCursor(TriggerEditorView* source, 
								CWnd* pWnd, UINT nHitTest, UINT message)	= 0;

	virtual void OnKeyDown(TriggerEditorView* source, 
								UINT nChar, UINT nRepCnt, UINT nFlags)		= 0;
	virtual void OnKeyUp(TriggerEditorView* source, 
								UINT nChar, UINT nRepCnt, UINT nFlags)		= 0;

	virtual void OnPaint(TriggerEditorView* source, 
								CDC* pdc, CRect const& rcPaint)				= 0;
	virtual BOOL OnCommand(TriggerEditorView* source, 
								WPARAM wParam, LPARAM lParam)				= 0;

	virtual void OnDrawItem(int nIDCtl, 
								LPDRAWITEMSTRUCT lpdis)						= 0;
	virtual void OnMeasureItem(int nIDCtl, 
								LPMEASUREITEMSTRUCT lpmis)					= 0;
	virtual void OnShowWindow(TriggerEditorView* source, 
								BOOL bShow, UINT nStatus)					= 0;

	//начал работу
	virtual bool OnStart()													= 0;
	//закончил
	virtual bool OnEnd()													= 0;

	virtual BOOL OnDropData(TriggerEditorView* source, 
								void* pData, const CPoint& point)			= 0;
	virtual bool CanDrop(TriggerEditorView* source, 
								void* pData, const CPoint& point)			= 0;
	virtual bool OnCommandUpdateUI(TriggerEditorView *source, 
								CCmdUI* pCmdUI)								= 0;
protected:
	//чтобы нельзя было удалять напрямую указатель на этот интрефейс
	inline virtual ~ITEWorkMode()											= 0;
};
ITEWorkMode::~ITEWorkMode(){}