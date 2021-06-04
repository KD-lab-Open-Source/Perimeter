// ClipBoardDataInserter.h: interface for the ClipBoardDataInserter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIPBOUARDDATAINSERTER_H__369BDEB5_3CBB_4BCD_B3FF_B187CFECB037__INCLUDED_)
#define AFX_CLIPBOUARDDATAINSERTER_H__369BDEB5_3CBB_4BCD_B3FF_B187CFECB037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TEEngine/UndoManager/TEUndoManager.h"
#include "TriggerExport.h"

#include <map>

class TriggerEditorLogic;
class TEBaseWorkMode;
class TriggerClipBuffer;

class ClipBoardDataInserter  
{
typedef std::map<CString, std::vector<TriggerLink> > LinkIndex;
public:
	ClipBoardDataInserter(TriggerEditorLogic& logic,
							TEBaseWorkMode& mode);
	~ClipBoardDataInserter();
	bool insert(TriggerClipBuffer& clipBuffer, TEUndoManager& undoManager );
protected:
	TEBaseWorkMode& getMode();
	TriggerEditorLogic& getTriggerEditorLogic();
	TriggerEditorLogic const& getTriggerEditorLogic() const;

	//! Находит место для расположения элементов и перемещает туда элементы
	void arrangeTriggersUnderMouse(TriggerList& triggers) const;

	void addTriggers(TriggerChain& chain,
							TriggerList const& triggers,
							TEUndoManager::Bunch& undoBunch,
							TEUndoManager::Bunch& redoBunch);

	void addLink(TEUndoManager::Action& undo, 
							 TEUndoManager::Action& redo,
							 TriggerChain& chain,
							 int parentTriggerIndex, 
							 int childTriggerIndex,
							 int type,
							 CSize const& childOffset,
							 CSize const& parentOffset,
							 bool isAutoRestarted);

	void addLinksFromIndex(TriggerChain& chain,
						   LinkIndex const& index,
						   TEUndoManager::Bunch& undoBunch,
							TEUndoManager::Bunch& redoBunch);

	void renameTriggersForChain(
							TriggerChain const& chain,
							TriggerList& triggers);

	void makeAddLinkIndex(TriggerList& triggers, 
									  LinkIndex& index);

	void renameTrigger(TriggerList& triggers,
								   Trigger& trigger,
								   LPCTSTR newName);
private:
	TriggerChain& getTriggerChain();
private:
	TriggerEditorLogic& logic_;
	TEBaseWorkMode& mode_;
};

#endif // !defined(AFX_CLIPBOUARDDATAINSERTER_H__369BDEB5_3CBB_4BCD_B3FF_B187CFECB037__INCLUDED_)
