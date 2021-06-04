#include "stdafx.h"
#include "resource.h"
#include "teprofilerworkmode.h"

#include "Utils/menu_helpers.h"
#include "TEEngine/Drawers/IDrawData.h"
#include "TEEngine/Drawers/ITEDrawer.h"
#include "TEEngine/TriggerEditorView.h"
#include "TEEngine/TriggerEditorLogic.h"

#include "TEEngine/Profiler/ITriggerProfList.h"
#include "Custom Controls/ToolTip.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


TEProfilerWorkMode::TEProfilerWorkMode(TriggerEditorLogic* plogic)
: TESingleChainWorkMode(plogic)
, drawer_(NULL)
, drawDataKeeper_(new DataKeeper(NULL))
, ptrTriggerProfList_(NULL)
{
	drawDataKeeper_->setWorkMode(this);
}

TEProfilerWorkMode::~TEProfilerWorkMode(void)
{
}

void TEProfilerWorkMode::setDrawer(ITEDrawer* ptrDrawer){
	drawer_ = ptrDrawer;
}

void TEProfilerWorkMode::setTriggerProfList(ITriggerProfList* ptrTriggerProfList){
	ptrTriggerProfList_ = ptrTriggerProfList;
}

void TEProfilerWorkMode::OnPaint(TriggerEditorView* source, 
								 CDC* pdc, 
								 CRect const& rcPaint)
{
	drawer_->draw(pdc, source, drawDataKeeper_.get(), rcPaint);
}

bool TEProfilerWorkMode::OnStart()
{

	ASSERT(ptrTriggerProfList_);
	ptrTriggerProfList_->show();

	return TESingleChainWorkMode::OnStart()&&ptrTriggerProfList_->activate();
}
bool TEProfilerWorkMode::OnEnd()
{
	ptrTriggerProfList_->hide();
	return true;
}

void TEProfilerWorkMode::OnKeyUp(TriggerEditorView *source, 
								 UINT nChar, 
								 UINT nRepCnt, 
								 UINT nFlags)
{
	if (nChar == VK_UP)
	{
		ptrTriggerProfList_->prev();
//		if (!ptrTriggerProfList_->prev());
//			Beep(1000, 100);
	}
	else if (nChar == VK_DOWN)
	{
		ptrTriggerProfList_->next();
//		if (!ptrTriggerProfList_->next());
//			Beep(1000, 100);
	}

	TESingleChainWorkMode::OnKeyUp(source, nChar, nRepCnt, nFlags);
}

void TEProfilerWorkMode::OnLButtonDblClk(TriggerEditorView* source, 
										 UINT nFlags, 
										 CPoint point)
{
	source->scr2Log(&point, 1);
	TriggerEditorLogic const& logic = *getTriggerEditorLogic();
	TriggerChain& chain = *getTriggerChain();
	int triggerIndex = logic.findTrigger(chain, point);
	if (triggerIndex == -1)
		return;
	Trigger& trigger = getStrategyTrigger(triggerIndex);
	if (triggerInterface().editCondition(trigger, source->m_hWnd))
		getTriggerEditorLogic()->setDataChanged();
	source->Invalidate();
}

void TEProfilerWorkMode::OnRButtonUp(TriggerEditorView* source, 
									 UINT nFlags, 
									 CPoint point)
{
	CPoint p(point);
	source->scr2Log(&p, 1);
	TriggerEditorLogic const& logic = *getTriggerEditorLogic();
	TriggerChain& chain = *getTriggerChain();
	selectionManager_.deselectAll();
	int triggerIndex = logic.findTrigger(chain, point);
	if (triggerIndex != -1)
	{
		selectionManager_.select(triggerIndex);
	}
	source->Invalidate();

//	setActiveTriggerIndex(logic.findTrigger(chain, point));
//	if (getActiveTriggerIndex() != -1)
//		trackPopupMenu(IDR_TE_TRIGGER_ELE1, source);
}

void TEProfilerWorkMode::ModifyMenu(CMenu* pmenu, 
									UINT menuResId, 
									TriggerEditorView* source)
{
	if (menuResId == IDR_TE_TRIGGER_ELE1)
	{
		if (!selectionManager_.empty())
//		if (getActiveTriggerIndex() != -1) 
		{
			menu_h::removeMenuItemByCommand(pmenu, IDC_TE_PROPETIES);
		}
	}
}

void TEProfilerWorkMode::OnLButtonDown(TriggerEditorView* source, 
									   UINT nFlags, 
									   CPoint point)
{
	if (getCanDragContent())
	{
		TESingleChainWorkMode::OnLButtonDown(source, nFlags, point);
		return;
	}
	source->scr2Log(&point, 1);
}

ITriggerProfList* TEProfilerWorkMode::getTriggerProfList() const{
	return ptrTriggerProfList_;
}

void TEProfilerWorkMode::trackToolTip(TriggerEditorView* source, CPoint point)
{
	static int predTriggerIndex = -1;
	ToolTip const* toolTip = source->getToolTip();
	if (!toolTip)
		return;

	CPoint scr_p(point);
	source->scr2Log(&point, 1);
	TriggerChain const& chain = *getTriggerChain();
	int triggerIndex = getTriggerEditorLogic()->findTrigger(chain, point);

	if (triggerIndex != predTriggerIndex){ 
		if (triggerIndex != -1)
		{
			Trigger const& trigger = 
				chain.triggers[triggerIndex];
			static CString strText;
			strText = trigger.debugDisplayText();
			toolTip->setToolTipText(strText);
		}
		else
			toolTip->setToolTipText(CString());
		predTriggerIndex = triggerIndex;
	}
	::ClientToScreen(*source, &scr_p);
	toolTip->trackToolTip(scr_p);
}

