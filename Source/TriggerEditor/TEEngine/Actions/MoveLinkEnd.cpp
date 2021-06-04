#include "stdafx.h"
#include "movelinkend.h"
//#include "Game/GameInclude.h"

MoveLinkEnd::MoveLinkEnd(int linkIndex, int linkParentIndex, 
						 TriggerChain const& chain, CPoint const& offset, 
						 MLESelectEnd eval):
  linkIndex_(linkIndex)
, linkParentIndex_(linkParentIndex)
, chain_(chain)
, offsetSize_(offset)
, linkEnd_(eval)
{
}

MoveLinkEnd::~MoveLinkEnd(void)
{
}

bool MoveLinkEnd::operator()()
{
	return run(linkIndex_, linkParentIndex_, chain_, offsetSize_, linkEnd_);
}

bool MoveLinkEnd::run(int linkIndex, 
					  int linkParentIndex, 
					  TriggerChain const& chain,
					  CPoint const& offset, 
					  MLESelectEnd linkEnd)
{
	Trigger const& trigger = chain.triggers[linkParentIndex];
	TriggerLink& link = const_cast<TriggerLink&>(trigger.outcomingLinks()[linkIndex]);
	if (linkEnd == MLE_CHILD_OFFSET) 
		link.setChildOffset(offset);
	else
		link.setParentOffset(offset);
	return true;
}
