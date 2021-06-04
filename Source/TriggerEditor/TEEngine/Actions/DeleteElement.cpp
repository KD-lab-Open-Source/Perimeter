#include "stdafx.h"
#include "deleteelement.h"
//#include "Game/GameInclude.h"
#include "TEEngine/TriggerEditorLogic.h"
DeleteElement::DeleteElement(TriggerChain & chain,
							 int triggerIndex):
  chain_(chain)
, triggerIndex_(triggerIndex)
{
}

DeleteElement::~DeleteElement(void)
{
}

bool DeleteElement::operator()()
{
	return run(chain_, triggerIndex_);
}

bool DeleteElement::run(TriggerChain & chain, int triggerIndex)
{
	chain.removeTrigger(triggerIndex);
	TriggerEditorLogic::remakeStrategyRect(chain);
	return true;
}
