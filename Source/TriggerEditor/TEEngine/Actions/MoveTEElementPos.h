/********************************************************************
	created:	2003/07/29
	created:	29:7:2003   12:36
	filename: 	d:\Projects\Quest\QuestEditor\MoveTEElementPos.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	MoveTEElementPos
	file ext:	h
	powerd by:	Илюха
	
	purpose:	Смена положения элемента на экране и выравнивание области просмотра
*********************************************************************/
#pragma once

#include "TriggerExport.h"

class TEBaseWorkMode;
class TriggerEditorView;


class MoveTEElementPos
{
public:
	MoveTEElementPos(TriggerEditorView* pwnd, 
		int triggerIndex, 
		TriggerChain &chain,
		CPoint const& offset);
	~MoveTEElementPos(void);

	bool operator()();
	static bool run(TriggerEditorView* pwnd,
					int triggerIndex,
					TriggerChain &chain,
					CPoint const& offset);
private:
	TriggerEditorView*	window_;
	int					triggerIndex_;
	TriggerChain &chain_;
	CPoint				offset_;
};
