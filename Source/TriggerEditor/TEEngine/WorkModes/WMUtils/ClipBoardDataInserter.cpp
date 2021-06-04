// ClipBoardDataInserter.cpp: implementation of the ClipBoardDataInserter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TEEngine/WorkModes/WMUtils/ClipBoardDataInserter.h"
#include "TEEngine/WorkModes/TEBaseWorkMode.h"
#include "TEEngine/TriggerEditorLogic.h"
#include "TEEngine/TriggerEditorView.h"
#include "TEEngine/TriggerClipBuffer.h"

#include "TEEngine/Actions/AddElement.h"
#include "TEEngine/Actions/DeleteElement.h"

#include "TEEngine/Actions/AddLink.h"
#include "TEEngine/Actions/DeleteLink.h"

#include "TEEngine/TestTriggerName.h"
#include "Utils/EnsureTriggerNameOriginality.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ClipBoardDataInserter::ClipBoardDataInserter(TriggerEditorLogic& logic,
											 TEBaseWorkMode& mode)
											 : logic_(logic)
											 , mode_(mode)
{

}

ClipBoardDataInserter::~ClipBoardDataInserter()
{

}

TriggerChain& ClipBoardDataInserter::getTriggerChain(){
	assert(getMode().getTriggerChain());
	return *getMode().getTriggerChain();
}

TriggerEditorLogic& ClipBoardDataInserter::getTriggerEditorLogic(){
	return logic_;
}
TriggerEditorLogic const& ClipBoardDataInserter::getTriggerEditorLogic() const{
	return logic_;
}

TEBaseWorkMode& ClipBoardDataInserter::getMode(){
	return mode_;
}

bool ClipBoardDataInserter::insert(TriggerClipBuffer& clipBuffer,
								   TEUndoManager& undoManager)
{
	if (clipBuffer.empty())
		return false;

	TriggerClipBuffer::iterator i = clipBuffer.begin(), e = clipBuffer.end();
	TriggerList temp_container;
	for(; i != e; ++i)
		temp_container.push_back(*i);
	
	TriggerChain& chain = getTriggerChain();

	TEUndoManager::Bunch undoDeleteBunch, redoDeleteBunch;
	getMode().deleteSelection(undoDeleteBunch, redoDeleteBunch);

	// Удаляем для того, чтобы имена зменяемых не мешали при получении уникальных имен
	renameTriggersForChain(chain, temp_container);
	arrangeTriggersUnderMouse(temp_container);

	LinkIndex linkIndex;
	makeAddLinkIndex(temp_container, linkIndex);

	TEUndoManager::Bunch undoAddTriggersBunch, redoAddTriggersBunch;

	addTriggers(chain, temp_container, undoAddTriggersBunch, redoAddTriggersBunch);

	TEUndoManager::Bunch undoAddLinksBunch, redoAddLinksBunch;
	addLinksFromIndex(getTriggerChain(), linkIndex, 
		undoAddLinksBunch, redoAddLinksBunch);	

	TEUndoManager::Bunch undoBunch, redoBunch;
	if (!undoDeleteBunch.empty()&&!redoDeleteBunch.empty())
	{
		undoDeleteBunch.reverse();
		undoBunch.addAction(undoDeleteBunch);
		redoBunch.addAction(redoDeleteBunch);
	}

	if (!undoAddTriggersBunch.empty()&&!redoAddTriggersBunch.empty())
	{
		undoAddTriggersBunch.reverse();
		undoBunch.addAction(undoAddTriggersBunch);
		redoBunch.addAction(redoAddTriggersBunch);
	}

	if (!undoAddLinksBunch.empty()&&!redoAddLinksBunch.empty())
	{
		undoAddLinksBunch.reverse();
		undoBunch.addAction(undoAddLinksBunch);
		redoBunch.addAction(redoAddLinksBunch);
	}
	if (!undoBunch.empty()&&!redoBunch.empty())
	{
		undoBunch.reverse();
		undoManager.addAction(undoBunch, redoBunch);
	}

	return true;
}

void ClipBoardDataInserter::arrangeTriggersUnderMouse(
									TriggerList& triggers) const
{
	TriggerEditorLogic const& logic = getTriggerEditorLogic();
	CPoint cursorPos;
	GetCursorPos(&cursorPos);
	TriggerEditorView & view = *logic.getTriggerEditorView();
	view.ScreenToClient(&cursorPos);

	CRect clientRect;
	view.GetClientRect(clientRect);
	if (!clientRect.PtInRect(cursorPos))
		cursorPos = CPoint(0, 0);
	view.scr2Log(&cursorPos, 1);


	TEGrid const& grid = logic.getGrid();
	int leftIndex = grid.getHorzCellIndex(cursorPos.x);
	int topIndex = grid.getVertCellIndex(cursorPos.y);
	TriggerList::iterator i = triggers.begin(), e = triggers.end();
	for( i = triggers.begin(); i != e; ++i)
		i->setCellIndex(i->cellIndex() + CPoint(leftIndex, topIndex));
}

void ClipBoardDataInserter::addTriggers(TriggerChain& chain,
							TriggerList const& triggers,
							TEUndoManager::Bunch& undoBunch,
							TEUndoManager::Bunch& redoBunch)
{
	TriggerList const& chainTriggers = chain.triggers;

	TriggerEditorLogic const& logic = getTriggerEditorLogic();
	TEGrid const& grid = logic.getGrid();
	TriggerList::const_iterator it = triggers.begin(), end = triggers.end();
	for(; it != end; ++it)
	{
		AddElement addElement(grid, it->cellIndex(),
			 chain, *it, chainTriggers.size());
		DeleteElement deleteElement(chain, chainTriggers.size());
		if (addElement())
		{
			undoBunch.addAction(deleteElement);
			redoBunch.addAction(addElement);
		}
	}
}

void ClipBoardDataInserter::addLinksFromIndex(TriggerChain& chain,
									   LinkIndex const& index,
									   TEUndoManager::Bunch& undoBunch,
										TEUndoManager::Bunch& redoBunch)
{
	LinkIndex::const_iterator it = index.begin(), end = index.end();
	TriggerList& triggers = chain.triggers;
	for(; it != end; ++it)
	{
		
		TriggerList::iterator res = 
			std::find_if(triggers.begin(), triggers.end(), 
			TestTriggerName((*it).first));

		assert(res != triggers.end());

		int parentTriggerIndex = std::distance(triggers.begin(), res);

		std::vector<TriggerLink > const& childs = (*it).second;
		std::vector<TriggerLink >::const_iterator i = childs.begin(), 
																e = childs.end();
		for(; i != e; ++i)
		{
			TriggerLink const& link = *i;
			TriggerList::iterator res = 
				std::find_if(triggers.begin(), triggers.end(), 
				TestTriggerName(link.triggerName()));
			assert(res != triggers.end());

			int childTriggerIndex = std::distance(triggers.begin(), res);

			TEUndoManager::Action undo, redo;
			addLink(undo, redo, chain, 
				parentTriggerIndex, childTriggerIndex, 
				link.getType(),
				link.childOffset(),
				link.parentOffset(),
				link.autoRestarted());
			if (undo&&redo)
			{
				undoBunch.addAction(undo);
				redoBunch.addAction(redo);
			}
		}
	}
}

void ClipBoardDataInserter::addLink(TEUndoManager::Action& undo, 
							 TEUndoManager::Action& redo,
							 TriggerChain& chain,
							 int parentTriggerIndex, 
							 int childTriggerIndex,
							 int type,
							 CSize const& childOffset,
							 CSize const& parentOffset,
							 bool isAutoRestarted)
{
	AddLink addLink(chain, parentTriggerIndex, childTriggerIndex, 
					childOffset,parentOffset,type, isAutoRestarted);
	DeleteLink delLink(chain, parentTriggerIndex, childTriggerIndex);
	if (addLink())
	{
		redo = addLink;
		undo = delLink;
	}
}

void ClipBoardDataInserter::renameTriggersForChain(
									TriggerChain const& chain,
									TriggerList& triggers)
{
	TriggerList::iterator i = triggers.begin(), e = triggers.end();
	for(; i != e;++i)
	{
		CString const oldName = i->name();
		CString newName = EnsureTriggerNameOriginality::getUniqueName(chain, oldName);
		if (newName != oldName)
			renameTrigger(triggers, *i, newName);
	}
}

void ClipBoardDataInserter::renameTrigger(TriggerList& triggers,
								   Trigger& trigger,
								   LPCTSTR newName)
{
	TriggerList::iterator i = triggers.begin(), e = triggers.end();
	CString oldName(trigger.name());
	for(; i != e; ++i)
	{
		if (&trigger != &*i)
		{
			OutcomingLinksList& links = i->outcomingLinks();
			OutcomingLinksList::iterator res = links.begin(),
														end = links.end();
			while ((res = std::find_if(res, 
										end, 
										TestTriggerName(oldName))) != end)
			{
				res->setTriggerName(newName);
				++res;
			}
		}
	}
	trigger.setName(newName);
}

void ClipBoardDataInserter::makeAddLinkIndex(TriggerList& triggers, 
									  LinkIndex& index)
{
	TriggerList::const_iterator i = triggers.begin(), e = triggers.end();
	for(; i != e; ++i)
	{					   
		CString const triggerName = i->name();
		OutcomingLinksList const& list = i->outcomingLinks();
		std::vector<TriggerLink> &links = index[triggerName];
		links.insert(links.end(), list.begin(), list.end());
	}
}

