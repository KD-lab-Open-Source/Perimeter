// CopyToClipBoard.cpp: implementation of the CopyToClipBoard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TEEngine/WorkModes/WMUtils/CopyToClipboard.h"
#include "TEEngine/TriggerClipBuffer.h"
#include "TEEngine/SelectionManager.h"
#include "TEEngine/TestTriggerName.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CopyToClipBoard::CopyToClipBoard()
{

}

CopyToClipBoard::~CopyToClipBoard()
{

}

bool CopyToClipBoard::copy(TriggerChain const& chain,
						   TriggerClipBuffer& clipBuffer,
						   SelectionManager const& selMngr)
{
	clipBuffer.clear();
	SelectionManager::const_iterator i = selMngr.begin(), 
									e = selMngr.end();
	TriggerList temp_container;
	for(; i != e;++i)
	{
		if (*i != 0)
		{
			Trigger trigger = chain.triggers[*i];
			trigger.setSelected(false);
			temp_container.push_back(trigger);
		}
	}
	filterLinks(temp_container);
	makeTriggerCellIndexesRelative(temp_container);

	if (!temp_container.empty())
		std::copy(temp_container.begin(), 
					temp_container.end(), 
					std::back_inserter(clipBuffer));

	return true;
}

void CopyToClipBoard::filterLinks(TriggerList& triggers)
{
	if (triggers.size() == 1)
	{
		Trigger& trigger = triggers.front();
		trigger.incomingLinks().clear();
		trigger.outcomingLinks().clear();
		return;
	}
	TriggerList::iterator i = triggers.begin(), e = triggers.end();
	for(; i != e; ++i)
	{
		i->incomingLinks().clear();
		OutcomingLinksList& links = i->outcomingLinks();
		OutcomingLinksList::iterator li = links.begin();
		for(; li != links.end();)
		{
			if (std::find_if(
				triggers.begin(), 
				triggers.end(), 
				TestTriggerName(li->triggerName())
				) 
				== triggers.end())
			{
				li = links.erase(li);
			}
			else
				++li;
			
		}
	}
}


void CopyToClipBoard::makeTriggerCellIndexesRelative(
							TriggerList& triggers) const
{
	int minHorz = INT_MAX, minVert = INT_MAX;
	TriggerList::iterator i = triggers.begin(), e = triggers.end();
	for(; i != e; ++i)
	{												  
		if (minHorz > i->cellIndex().x)
			minHorz = i->cellIndex().x;

		if (minVert > i->cellIndex().y)
			minVert = i->cellIndex().y;
	}
	for( i = triggers.begin(); i != e; ++i)
		i->setCellIndex(i->cellIndex() - CPoint(minHorz, minVert));
}

