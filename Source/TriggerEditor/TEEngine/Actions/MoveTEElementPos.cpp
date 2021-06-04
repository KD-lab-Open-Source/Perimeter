#include "stdafx.h"
#include "MoveTEElementPos.h"

//#include "Game/GameInclude.h"

#include "TEEngine/TriggerEditorView.h"
#include "TEEngine/TriggerEditorLogic.h"
#include "TEEngine/te_helpers.h"


MoveTEElementPos::MoveTEElementPos(TriggerEditorView* pwnd, 
									int triggerIndex, 
									TriggerChain &chain,
									CPoint const& offset):
  window_(pwnd)
, triggerIndex_(triggerIndex)
, chain_(chain)
, offset_(offset)
{
}

MoveTEElementPos::~MoveTEElementPos(void)
{
}

bool MoveTEElementPos::operator()(){
	return run(window_, triggerIndex_, chain_, offset_);
}
bool MoveTEElementPos::run(TriggerEditorView* pwnd, 
							int triggerIndex, 
							TriggerChain &chain,
							CPoint const& offset)
{
	Trigger &trigger = 
		chain.triggers[triggerIndex];

	CRect boundingRect = trigger.boundingRect();
	boundingRect.OffsetRect(offset);
	trigger.setBoundingRect(boundingRect);

	TriggerEditorLogic& logic = *pwnd->getTriggerEditorLogic();

	logic.updateCellIndexes(trigger, boundingRect.TopLeft());

	logic.remakeStrategyRect(chain);
	pwnd->updateViewWorkArea();

	return true;
}