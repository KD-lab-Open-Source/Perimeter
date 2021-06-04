/********************************************************************
	created:	2003/05/23
	created:	23:5:2003   18:11
	filename: 	d:\Projects\Quest\QuestEditor\TEBaseWorkModeDrawer.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TEBaseWorkModeDrawer
	file ext:	h
	Powerd by:	Илюха при идейном руководстве и моральной поддержке of Scmorr
	
	purpose:	Прорисовка базового режима
*********************************************************************/
#pragma once
#include "tebaseworkmodedrawerbase.h"

class TEBaseWorkMode;
class TriggerEditorView;

class TEBaseWorkModeDrawer :
	public TEBaseWorkModeDrawerBase
{
public:
	TEBaseWorkModeDrawer(TEBaseWorkMode* ptrWorkMode);
	~TEBaseWorkModeDrawer(void);

	virtual void draw(CDC* pdc, TriggerEditorView* view, 
						IDrawData* pdd, CRect const& rcPaint) const;

	virtual void drawAnchoredLink(TriggerEditorView* view, HDC dc, 
		Trigger const& link_owner, TriggerLink const& link) const;
	virtual void drawDragedLink(TriggerEditorView* view, HDC dc, 
		const CPoint& pt_from, const CPoint& pt_to, int type) const;
	virtual void drawDragedTitle(TriggerEditorView* view, 
				std::string const& strTitle, CPoint pos) const;
	void drawLinkAnchors(TriggerEditorView* view, HDC dc, 
		Trigger const& link_owner, TriggerLink const& link) const;
	virtual HBRUSH getBrushByLinkType(int type) const{
		return getLinkTypeBrush(type);
	}
protected:
	BOOL setPenCant(HPEN hpen);
	HPEN getPenCant() const;

	virtual HBRUSH	SelectLinkBrushColor(TriggerLink const& link) const;
	virtual HPEN	SelectLinkPenColor(TriggerLink const& link) const;

	BOOL setLinkOutPen(HPEN hpen);
	BOOL setLinkInPen(HPEN hpen);
	BOOL setOtherLinksPen(HPEN hpen);

	BOOL setLinkOutBrush(HPEN hbrush);
	BOOL setLinkInBrush(HPEN hbrush);
	BOOL setOtherLinksBrush(HPEN hbrush);

	HPEN getLinkOutPen() const;
	HPEN getLinkInPen() const;
	HPEN getOtherLinksPen() const;

private:
	//! Если нельзя установить связь, то этим карандашем рисуем линию
	CPen cantPen_;
	//! Карандаши для режима, когда выделяются связи идущие к/от элемента
	//! Карандаш для исходящих связей
	CPen linkOutPen_;
	//! Карандаш для входящих связей
	CPen linkInPen_;
	//Карандаш для остальных связей
	CPen otherLinksPen_;

	//! Кисти для режима, когда выделяются связи идущие к/от элемента
	//Кисть для исходящих связей
	CBrush linkOutBrush_;
	//Кисть для входящих связей
	CBrush linkInBrush_;
	//Кисть для остальных связей
	CBrush otherLinksBrush_;

	//Рабочий режим, на который мы работаем
	TEBaseWorkMode *workMode_;
};
