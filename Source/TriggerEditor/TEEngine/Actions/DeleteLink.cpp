#include "stdafx.h"
//#include "Game/GameInclude.h"
#include "deletelink.h"

DeleteLink::DeleteLink(TriggerChain & chain, 
				int linkOwnerIndex, 
				int linkChildIndex):
  chain_(chain)
, linkOwnerIndex_(linkOwnerIndex)
, linkChildIndex_(linkChildIndex)
{
}

DeleteLink::~DeleteLink(void)
{}

bool DeleteLink::operator()(){
	return run(chain_, linkOwnerIndex_, linkChildIndex_);
}

bool DeleteLink::run(TriggerChain & chain, 
					 int linkOwnerIndex, int linkChildIndex)
{
	return chain.removeLink(linkOwnerIndex, linkChildIndex);
}
