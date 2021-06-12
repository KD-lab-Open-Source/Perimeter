/********************************************************************
	created:	2003/05/24
	created:	24:5:2003   16:49
	filename: 	d:\Projects\Quest\QuestEditor\TEBaseWorkModeDrawerBase.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TEBaseWorkModeDrawerBase
	file ext:	h
	Powerd by:	Илюха при идейном руководстве и моральной поддержке of Scmorr
	
	purpose:	Базовый класс для художников базового режима работы
*********************************************************************/
#pragma once
//#include "TEEngine/Drawers/TEConditionColoredEleDrawer.h"
#include "TEEngine/Drawers/TESelfColoredEleDrawer.h"

class TriggerEditorView;
struct TriggerLink;

class TEBaseWorkModeDrawerBase :
//	public TEConditionColoredEleDrawer
	public TESelfColoredEleDrawer
{
public:
	TEBaseWorkModeDrawerBase(void);
	~TEBaseWorkModeDrawerBase(void);

	//рисует линк с якорями, позволяющими изменять место положение связи
	virtual void drawAnchoredLink(TriggerEditorView* view, HDC dc, 
		Trigger const& link_owner, TriggerLink const& link) const = 0;
	//рисует линк, когда мы его тянем от одного элемента к другому
	virtual void drawDragedLink(TriggerEditorView* view, HDC dc, const CPoint& pt_from, 
		const CPoint& pt_to, int type) const = 0;
	//рисует передвигаемый элемент
	virtual void drawDragedTitle(TriggerEditorView* view, 
		std::string const& strTitle, CPoint pos) const = 0;
	//возвращает кисть соответствующую связи данного тип
	virtual HBRUSH getBrushByLinkType(int type) const = 0;
	//рисуем якоря для линка
	virtual void drawLinkAnchors(TriggerEditorView* view, HDC dc, 
		Trigger const& link_owner, TriggerLink const& link) const = 0;
};
