// ChangeLinkType.cpp: implementation of the ChangeLinkType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChangeLinkType.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ChangeLinkType::ChangeLinkType(TriggerChain& chain, 
					int triggerIndex, 
					int linkIndex,
					int type):
  chain_(chain)
, triggerIndex_(triggerIndex)
, linkIndex_(linkIndex)
, type_(type)
{

}

ChangeLinkType::~ChangeLinkType()
{

}

bool ChangeLinkType::operator()()
{
	return run(chain_, triggerIndex_, linkIndex_, type_);
}

bool ChangeLinkType::run(TriggerChain& chain, 
				int triggerIndex, 
				int linkIndex,
				int type)
{
	chain.triggers[triggerIndex].outcomingLinks()[linkIndex].setType(type);
	return true;
}
