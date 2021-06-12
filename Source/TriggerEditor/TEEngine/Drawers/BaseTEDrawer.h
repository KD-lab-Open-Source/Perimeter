/********************************************************************
	created:	2003/05/23
	created:	23:5:2003   15:58
	filename: 	d:\Projects\Quest\QuestEditor\BaseTEDrawer.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	BaseTEDrawer
	file ext:	h
	Powerd by:	Илюха при идейном руководстве и моральной поддержке of Scmorr
	
	purpose:	Реализация базовых потребностей. Отрисовка элементов, связей, сетки.
*********************************************************************/

#pragma once
#include "TEEngine/Drawers/TEDrawerBase.h"
#include "TEEngine/Drawers/IDrawData.h"

class TriggerEditorView;

class BaseTEDrawer : public TEDrawerBase
{
public:
	BaseTEDrawer(void);
	virtual ~BaseTEDrawer(void);
	void draw(CDC* pdc, TriggerEditorView* source, 
				IDrawData* pdd, CRect const& rcPaint) const;
	void drawDraggedRect(HDC dc, CRect const&r, HPEN hpen) const;
	void drawNet(CDC* pdc, TriggerEditorView* source, CRect const& rcPaint) const;
protected:
	virtual HBRUSH	SelectTriggerBackBrush(Trigger const& trigger) const;
	virtual HBRUSH	SelectLinkBrushColor(TriggerLink const& link) const;
	virtual HPEN	SelectLinkPenColor(TriggerLink const& link) const;

protected://геттеры и сеттеры
//	void setStrategyOffset(CSize const& sz) const;
//	CSize const& getStrategyOffset() const;

	BOOL setNetPen(int nPenStyle, int nWidth, COLORREF crColor);
	BOOL setNetPen(HPEN hPen);
	HPEN getNetPen() const;

	BOOL setAutoRestartedLinkPen(int nPenStyle, int nWidth, COLORREF crColor);
	BOOL setAutoRestartedLinkPen(HPEN hPen);
	HPEN getAutoRestartedLinkPen() const;

	void setDrawedChain(TriggerChain const* chain) const;
	TriggerChain const* getDrawedChain() const;

	BOOL setDefaultTriggerBrush(HBRUSH hBrush);
	BOOL setDefaultTriggerBrush(COLORREF crColor);
	HBRUSH getDefaultTriggerBrush() const;

	BOOL setSelectedTriggerBrush(HBRUSH hBrush);
	BOOL setSelectedTriggerBrush(COLORREF crColor);
	HBRUSH getSelectedTriggerBrush() const;

	void setDrawedLinkOwner(Trigger const* trigger) const;
	Trigger const* getDrawedLinkOwner() const;

	void setPaintRect(CRect const& rc) const;
	CRect const& getPaintRect() const;
protected:
	//! Отрисовывает триггер
	void DrawTrigger(TriggerEditorView* source, 
					CDC* pdc, 
					Trigger const& trigger) const;
	//! Отрисовывает связь
	void DrawLink(TriggerEditorView* source, HDC dc, CPoint pt_from, CPoint pt_to, 
					TriggerLink const& link) const;
	//! Отрисовывает все связи в цепочке
	void DrawLinks(TriggerEditorView* source, CDC* pdc, 
					TriggerChain const& chain) const;
	//! Отрисовывает все связи триггера
	void DrawTriggerLinks(TriggerEditorView* source, CDC* pdc, 
		const RECT& rcEle, Trigger const& trigger) const;
	//! Отрисовывает все триггера в цепочке
	void DrawTriggers(TriggerEditorView* source, CDC* pdc, 
		const CRect& rcPaint, TriggerChain const& chain) const;
	void DrawChain(TriggerEditorView* source, CDC* pdc, const CRect& rcPaint, 
		TriggerChain const& chain) const;
private:
	static void DrawLink(HDC dc, const CPoint& pt_from, const CPoint& pt_to, 
		CPoint* ptrg, HPEN pen, HGDIOBJ brush);
private:
	//Карандаш, которым рисуется сетка
	CPen netPen_;
	CPen autoRestartedLinkPen_;
	//! цепочка отрисовываемая в текущий момент
	mutable TriggerChain const* drawedStrategy_;
	//! Текст для рисуемого элемента. для оптимизации.
	mutable CString drawedTriggerTitle_;

	//Смещение текущей цепочки от её обычноного положения.
	//используется при отрисовке всех цепочек сразу.
//	mutable CSize strategyOffset_;

	//Кисть, которой рисуется рамка обычного элемента
	CBrush defaultTriggerBrush_;
	//Кисть, которой рисуется рамка выделеннго элемента
	CBrush selectedTriggerBrush_;

	//! Владелец рисуемой связи
	mutable Trigger const* drawedLinkOwner_;
	//! Прямоугольник текущей прорисовки
	//! Устанавливается в начале прорисовки и недействителен после неё
	mutable CRect paintRect_;
};
