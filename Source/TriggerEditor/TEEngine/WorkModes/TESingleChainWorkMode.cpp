#include "stdafx.h"
#include "resource.h"

//#include "Game/GameInclude.h"

#include "../TriggerEditorView.h"
#include "Custom Controls/ToolTip.h"
#include "TEEngine/TriggerEditorLogic.h"

#include "TESingleChainWorkmode.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TESingleChainWorkMode::TESingleChainWorkMode(TriggerEditorLogic* logic):
  triggerChain_(NULL)
, triggerEditorLogic_(logic)
, baseElement_(-1)
{
}

TESingleChainWorkMode::~TESingleChainWorkMode(void)
{
}

SelectionManager& TESingleChainWorkMode::getSelectionManager(){
	return selectionManager_;
}

void TESingleChainWorkMode::setBaseElement(int baseElement){
	baseElement_ = baseElement;
}
int  TESingleChainWorkMode::getBaseElement() const{
	return baseElement_;
}

Trigger& TESingleChainWorkMode::getStrategyTrigger(int idx){
	assert (getTriggerChain());
	return getTriggerChain()->triggers[idx];
}

Trigger const& TESingleChainWorkMode::getStrategyTrigger(int idx) const{
	assert (getTriggerChain());
	return getTriggerChain()->triggers[idx];
}

int TESingleChainWorkMode::getStrategyTriggerIndex(Trigger const& trigger)
{
	return getStrategyTriggerIndex(*getTriggerChain(), trigger);
}

int TESingleChainWorkMode::getStrategyTriggerIndex(
											TriggerChain const&chain, 
											Trigger const& trigger)
{
	TriggerList const& triggers = chain.triggers;
	Trigger const* const first	= &triggers.front();
	Trigger const* const back	= &triggers.back();
	if (&trigger < first || back < &trigger)
		return -1;
	return (&trigger - first);
}

TriggerLink& TESingleChainWorkMode::getStrategyLink(Trigger& trigger, int linkIndex){
	return trigger.outcomingLinks()[linkIndex];
}

TriggerLink const& TESingleChainWorkMode::getStrategyLink(Trigger const& trigger, 
														   int linkIndex) const
{
	return trigger.outcomingLinks()[linkIndex];
}

TriggerLink& TESingleChainWorkMode::getStrategyLink(int triggerIndex, int linkIndex)
{
	assert(getTriggerChain());
	return getStrategyTrigger(triggerIndex).outcomingLinks()[linkIndex];
}

TriggerLink const& TESingleChainWorkMode::getStrategyLink(int triggerIndex, 
														   int linkIndex) const
{
	assert(getTriggerChain());
	return getStrategyTrigger(triggerIndex).outcomingLinks()[linkIndex];
}

TriggerEditorLogic* TESingleChainWorkMode::getTriggerEditorLogic() const{
	return triggerEditorLogic_;
}
void TESingleChainWorkMode::setTriggerEditorLogic(TriggerEditorLogic* logic){
	assert(logic);
	if (!logic)
		return;
	triggerEditorLogic_ = logic;
}

void TESingleChainWorkMode::setTriggerChain(TriggerChain* chain){
	if (chain != triggerChain_)
		selectionManager_.deselectAll();
	triggerChain_ = chain;
	selectionManager_.setTriggerChain(chain);
}
TriggerChain* TESingleChainWorkMode::getTriggerChain() const{
	return triggerChain_;
}

bool TESingleChainWorkMode::OnStart()
{
	setTriggerChain(getTriggerEditorLogic()->getTriggerChain());
	return (getTriggerChain() != NULL);
}

BOOL TESingleChainWorkMode::OnCommand(TriggerEditorView* source, 
									  WPARAM wParam, 
									  LPARAM lParam)
{
	switch(LOWORD(wParam)) {
	case IDC_TE_SHOW_CONDITION:
		OnTeShowCondition(source);
		break;
	case IDC_TE_PROPETIES:
		OnTeProperties(source);
		break;
	default:
		return TEWorkModeBase::OnCommand(source, wParam, lParam);
	}
	return TRUE;
}

void TESingleChainWorkMode::OnTeProperties(TriggerEditorView* source) 
{
	assert(selectionManager_.getCount());
	int const lastSelected = selectionManager_.getLast();
	if (selectionManager_.getCount() > 1)
	{
		selectionManager_.deselectAll();
		selectionManager_.select(lastSelected);
	}

	if(triggerInterface().editTrigger(getStrategyTrigger(lastSelected), source->m_hWnd)) 
		getTriggerEditorLogic()->setDataChanged();
	source->InvalidateRect(NULL);
}

void TESingleChainWorkMode::OnTeShowCondition(TriggerEditorView* source) 
{
	assert(selectionManager_.getCount());
	int const lastSelected = selectionManager_.getLast();
	if (selectionManager_.getCount() > 1)
	{
		selectionManager_.deselectAll();
		selectionManager_.select(lastSelected);
	}
	if (triggerInterface().editCondition(getStrategyTrigger(lastSelected), source->m_hWnd))
		getTriggerEditorLogic()->setDataChanged();

	source->InvalidateRect(NULL);
}

void TESingleChainWorkMode::trackToolTip(TriggerEditorView* source, CPoint point)
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
			toolTip->setToolTipText(trigger.displayText());
		}
		else
			toolTip->setToolTipText(CString());
		predTriggerIndex = triggerIndex;
	}
	::ClientToScreen(*source, &scr_p);
	toolTip->trackToolTip(scr_p);
}

void TESingleChainWorkMode::OnShowWindow(TriggerEditorView* source, 
										 BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		CPoint scr_p;
		GetCursorPos(&scr_p);
		trackToolTip(source, scr_p);	
	}
	else
		source->getToolTip()->setToolTipText(CString());
}
