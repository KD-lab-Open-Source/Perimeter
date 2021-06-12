#pragma once

#include "TriggerExport.h"

class MoveLinkEnd
{
public:
	enum MLESelectEnd{
		MLE_CHILD_OFFSET, MLE_OWNER_OFFSET
	};
	MoveLinkEnd(int linkIndex, int linkParentIndex, 
		TriggerChain const& chain,
		CPoint const& offset, MLESelectEnd eval);
	~MoveLinkEnd(void);
	bool operator()();
	static bool run(int linkIndex, int linkParentIndex, 
		TriggerChain const& chain, 
		CPoint const& offset, MLESelectEnd eval);
private:
	//! Порядковый номер связи
	int linkIndex_;
	//! Пордяковый номер триггера, из которого связь выходит
	int linkParentIndex_;
	//! Стратегия, которой принадлежит триггер
	TriggerChain const& chain_;
	//! Отступ
	CPoint offsetSize_;
	//! Какой конец связи изменяли
	MLESelectEnd linkEnd_;
};
