#include "stdafx.h"
#include "TEEngine/WorkModes/TEBaseWorkMode.h"
#include "makelinkactive.h"
//#include "Game/GameInclude.h"

MakeLinkActive::MakeLinkActive(TEBaseWorkMode* pmode, 
							int linkIndex, 
							int linkOwnerIndex):
  workMode_(pmode)
, linkIndex_(linkIndex)
, linkOwnerIndex_(linkOwnerIndex)
{
}

MakeLinkActive::~MakeLinkActive(void)
{
}

bool MakeLinkActive::operator()()
{
	return run(workMode_, linkIndex_, linkOwnerIndex_);
}
bool MakeLinkActive::run(TEBaseWorkMode* pmode, 
						int linkIndex, 
						int linkOwnerIndex)
{
	//! если уже активный, ...
	if (linkOwnerIndex == pmode->getLinkOwnerIndex()
		&&linkIndex == pmode->getActiveLinkIndex()) 
		return false;

	pmode->setActiveLinkIndex(linkIndex);
	pmode->setLinkOwnerIndex(linkOwnerIndex);
	return true;
}
