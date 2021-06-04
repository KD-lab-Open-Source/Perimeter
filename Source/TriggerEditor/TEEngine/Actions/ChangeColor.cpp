// ChangeColor.cpp: implementation of the ChangeColor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChangeColor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ChangeColor::ChangeColor(TriggerChain& chain, 
						 int triggerIndex, 
						 COLORREF color):
  chain_(chain)
, triggerIndex_(triggerIndex)
, color_(color)
{

}

ChangeColor::~ChangeColor()
{

}

bool ChangeColor::operator()()
{
	return run(chain_, triggerIndex_, color_);
}

bool ChangeColor::run(TriggerChain& chain, 
					  int triggerIndex, 
					  COLORREF color)
{
	chain.triggers[triggerIndex].setColor(color);
	return true;
}