#include "stdafx.h"
#include "addlink.h"
#include "TEEngine/WorkModes/TEBaseWorkMode.h"

AddLink::AddLink(TriggerChain & chain,
					int linkOwnerIndex,
					int linkChildIndex,
					CPoint const& szChildOffset,
					CPoint const& szParentOffset,
					int	type,
					bool isAutoRestarted
					):
  chain_(chain)
, linkOwnerIndex_(linkOwnerIndex)
, linkChildIndex_(linkChildIndex)
, childOffset_(szChildOffset)
, parentOffset_(szParentOffset)
, type_(type)
, isAutoRestarted_(isAutoRestarted)
{
}

AddLink::~AddLink(void)
{
}

bool AddLink::operator()()
{
	return run(chain_, linkOwnerIndex_, linkChildIndex_, 
				childOffset_, parentOffset_, type_, isAutoRestarted_);
}

bool AddLink::run(
				TriggerChain& chain,
				int linkOwnerIndex,
				int linkChildIndex,
				CPoint const& szChildOffset,
				CPoint const& szParentOffset,
				int type,
				bool isAutoRestarted
				)
{
	TriggerLink* link = chain.addLink(linkOwnerIndex, linkChildIndex, type);
	if (!link)
		return false;
	link->setChildOffset(szChildOffset);
	link->setParentOffset(szParentOffset);
	link->setType(type);
	link->setAutoRestarted(isAutoRestarted);
	return true;
}