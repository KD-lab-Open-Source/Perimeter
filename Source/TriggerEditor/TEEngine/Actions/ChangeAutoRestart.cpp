// ChangeAutoRestart.cpp: implementation of the ChangeAutoRestart class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChangeAutoRestart.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ChangeAutoRestart::ChangeAutoRestart(TriggerChain& chain, 
					int triggerIndex, 
					int linkIndex,
					bool autoRestart):
  chain_(chain)
, triggerIndex_(triggerIndex)
, linkIndex_(linkIndex)
, autoRestart_(autoRestart)
{

}

ChangeAutoRestart::~ChangeAutoRestart()
{

}

bool ChangeAutoRestart::operator()()
{
	return run(chain_, triggerIndex_, linkIndex_, autoRestart_);
}

bool ChangeAutoRestart::run(TriggerChain& chain, 
							int triggerIndex, 
							int linkIndex,
							bool autoRestart)
{
	chain.triggers[triggerIndex].outcomingLinks()[linkIndex].setAutoRestarted(autoRestart);
	return true;
}