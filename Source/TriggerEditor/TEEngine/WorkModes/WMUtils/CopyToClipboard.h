// CopyToClipboard.h: interface for the CopyToClipBoard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COPYTOCLIPBOARD_H__E0DB28D5_EC95_4A8E_8F7C_A8C586F8A3AD__INCLUDED_)
#define AFX_COPYTOCLIPBOARD_H__E0DB28D5_EC95_4A8E_8F7C_A8C586F8A3AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TriggerExport.h"

class SelectionManager;
class TriggerClipBuffer;

class CopyToClipBoard  
{
public:
	CopyToClipBoard();
	~CopyToClipBoard();
	bool copy(TriggerChain const& chain,
		TriggerClipBuffer& clipBuffer, 
		SelectionManager const& selMngr);
protected:
	//! Оставляет только те линки, которые связывают элементы в группе элементов
	void filterLinks(TriggerList& triggers);
	void makeTriggerCellIndexesRelative(
				TriggerList& triggers) const;

};

#endif // !defined(AFX_COPYTOCLIPBOARD_H__E0DB28D5_EC95_4A8E_8F7C_A8C586F8A3AD__INCLUDED_)
