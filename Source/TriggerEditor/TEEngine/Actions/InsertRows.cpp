#include "stdafx.h"
#include "insertrows.h"

//#include "Game/GameInclude.h"

#include "TriggerExport.h"
#include "TEEngine/te_helpers.h"
#include "TEEngine/TriggerEditorLogic.h"
#include "TEEngine/del_ins_col_row_helpers.h"

#include "TriggerExport.h"

#include <boost/iterator/filter_iterator.hpp>

InsertRows::InsertRows(size_t rowsCount, 
					TriggerEditorLogic const& logic, 
					TriggerChain & chain,
					CPoint const& basePoint,
					TEGrid const& grid):
  rowsCount_(rowsCount)
, logic_(logic)
, chain_(chain)
, basePoint_(basePoint)
, grid_(grid)
{
}

InsertRows::~InsertRows(void)
{
}

bool InsertRows::operator()()
{
	return run(rowsCount_, logic_, chain_, basePoint_, grid_);
}

bool InsertRows::run(size_t rowsCount, 
					TriggerEditorLogic const& logic, 
					TriggerChain& chain,
					CPoint const& basePoint,
					TEGrid const& grid)
{
	int const idx = grid.getVertCellIndex(basePoint.y);

	typedef TriggerList::iterator Iterator;
	typedef boost::filter_iterator<te_h::test_for_vert_offset&, Iterator> FilterIter;

	TriggerList& lst = chain.triggers;
	te_h::test_for_vert_offset predicate(idx);
	FilterIter first(predicate, lst.begin(), lst.end());
	FilterIter last(predicate, lst.end(), lst.end());

	int const fullCellHeight = grid.getFullCellHeight();
	te_h::offset_ele offseter(CSize(0, rowsCount*fullCellHeight), grid);
	std::for_each(first, last, offseter);

	logic.remakeStrategyRect(chain);
//	logic.updateChainWorkArea(chain);
//	logic.updateChainViewRect(chain);

	return true;
}
