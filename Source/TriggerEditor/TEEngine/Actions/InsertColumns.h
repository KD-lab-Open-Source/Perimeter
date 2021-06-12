/********************************************************************
	created:	2003/07/30
	created:	30:7:2003   13:01
	filename: 	d:\Projects\Quest\QuestEditor\InsertColumns.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	InsertColumns
	file ext:	h
	powerd by:	Илюха
	
	purpose:	Вставляет столбцы  между элементами в редакторе триггеров
*********************************************************************/
#pragma once

#include "TriggerExport.h"

class TriggerEditorLogic;
class TEGrid;

class InsertColumns
{
public:
	InsertColumns(size_t columnsCount, 
					TriggerEditorLogic const& logic, 
					TriggerChain& chain,
					CPoint const& basePoint,
					TEGrid const& grid);
	~InsertColumns(void);
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
