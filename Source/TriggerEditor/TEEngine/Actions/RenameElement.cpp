// RenameElement.cpp: implementation of the RenameElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RenameElement.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RenameElement::RenameElement(TriggerChain& chain, 
					int triggerIndex, 
					CString const& newName):
  chain_(chain)
, triggerIndex_(triggerIndex)
, newName_(newName)
{

}

RenameElement::~RenameElement()
{

}

bool RenameElement::operator()() const{
	chain_.triggers[triggerIndex_].setName(newName_);
	return true;
}

