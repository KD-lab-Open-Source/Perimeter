/********************************************************************
	created:	2003/07/30
	created:	30:7:2003   13:00
	filename: 	d:\Projects\Quest\QuestEditor\DeleteColumns.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	DeleteColumns
	file ext:	h
	powerd by:	Илюха
	
	purpose:	Удаляет столбцы между элементами в редаткоре триггеров
*********************************************************************/
#pragma once

#include "TriggerExport.h"

class TriggerEditorLogic;
class TEGrid;

class DeleteColumns
{
public:
	DeleteColumns(size_t columnsCount, 
					TriggerEditorLogic const& logic, 
					TriggerChain & chain,
					CPoint const& basePoint,
					TEGrid const& grid);
	~DeleteColumns(void);
	bool operator()();
	static bool run(size_t columnsCount, 
					TriggerEditorLogic const& logic, 
					TriggerChain& chain,
					CPoint const& basePoint,
					TEGrid const& grid);
private:
	int							columnsCount_;
	CPoint						basePoint_;
	TriggerEditorLogic const&	logic_;
	TriggerChain& chain_;
	TEGrid const&				grid_;
};
