/********************************************************************
	created:	2003/07/30
	created:	30:7:2003   17:24
	filename: 	d:\Projects\Quest\QuestEditor\DeleteRows.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	DeleteRows
	file ext:	h
	powerd by:	Илюха
	
	purpose:	удаление строк между элементами в редакторе триггеров
*********************************************************************/
#pragma once

#include "TriggerExport.h"

class TriggerEditorLogic;
class TEGrid;

class DeleteRows
{
public:
	DeleteRows(size_t rowsCount, 
					TriggerEditorLogic const& logic, 
					TriggerChain const& chain,
					CPoint const& basePoint,
					TEGrid const& grid);
	~DeleteRows(void);
	bool operator()();
	static bool run(size_t rowsCount, 
					TriggerEditorLogic const& logic, 
					TriggerChain const& chain,
					CPoint const& basePoint,
					TEGrid const& grid);
private:
	size_t						rowsCount_;
	CPoint						basePoint_;
	TriggerEditorLogic const&	logic_;
	TriggerChain const& chain_;
	TEGrid const&				grid_;
};
