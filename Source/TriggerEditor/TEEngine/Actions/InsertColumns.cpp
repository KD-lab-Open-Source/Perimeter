#include "stdafx.h"
#include "insertcolumns.h"

#include "TEEngine/TriggerEditorLogic.h"
#include "TEEngine/te_helpers.h"
#include "TEEngine/del_ins_col_row_helpers.h"
#include "TriggerExport.h"

#include "TriggerExport.h"

#include <boost/iterator/filter_iterator.hpp>


//#include "Game/GameInclude.h"

InsertColumns::InsertColumns(size_t columnsCount, 
						TriggerEditorLogic const& logic,
						TriggerChain& chain,
						CPoint const& basePoint,
						TEGrid const& grid):
  columnsCount_(columnsCount)
, logic_(logic)
, chain_(chain)
, basePoint_(basePoint)
, grid_(grid)
{
}

InsertColumns::~InsertColumns(void)
{
}

bool InsertColumns::operator()(){
	return run(columnsCount_, logic_, chain_, basePoint_, grid_);
}

bool InsertColumns::run(size_t columnsCount, 
					TriggerEditorLogic const& logic,
					TriggerChain& chain,
					CPoint const& basePoint,
					TEGrid const& grid)
{
	int const idx = grid.getHorzCellIndex(basePoint.x);

	TriggerList & lst = chain.triggers;

//	typedef boost::indirect_iterator<
//		TriggerList::const_iterator, Trigger > IndirectIterator;
	typedef boost::filter_iterator<
		te_h::test_for_horz_offset&, TriggerList::iterator> FilterIter;

	te_h::test_for_horz_offset predicate(idx);
	FilterIter first(predicate, lst.begin(), lst.end());
	FilterIter last(predicate, lst.end(), lst.end());
	te_h::offset_ele offseter(CSize(columnsCount*grid.getFullCellWidth(), 0), grid);
	std::for_each(first, last, offseter);

	logic.remakeStrategyRect(chain);
//	logic.updateChainWorkArea(const_cast<TriggerChain&>(chain));
//	logic.updateChainViewRect(chain);
	return true;
}
