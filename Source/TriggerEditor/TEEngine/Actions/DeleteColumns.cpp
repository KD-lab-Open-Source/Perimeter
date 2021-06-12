#include "stdafx.h"
#include "deletecolumns.h"
#include "TEEngine/te_helpers.h"
#include "TEEngine/TriggerEditorLogic.h"
#include "TEEngine/del_ins_col_row_helpers.h"
#include <boost/iterator/filter_iterator.hpp>

//#include "Game/GameInclude.h"

#include "TriggerExport.h"

DeleteColumns::DeleteColumns(size_t columnsCount, 
							TriggerEditorLogic const& logic, 
							TriggerChain & chain,
							CPoint const& basePoint,
							TEGrid const& grid):
  columnsCount_(columnsCount)
, logic_(logic)
, chain_(chain)
, basePoint_(basePoint)
, grid_(grid)
{
}

DeleteColumns::~DeleteColumns(void)
{
}

bool DeleteColumns::operator()()
{
	return run(columnsCount_, logic_, chain_, basePoint_, grid_);
}

bool DeleteColumns::run(size_t columnsCount, 
					TriggerEditorLogic const& logic, 
					TriggerChain& chain,
					CPoint const& basePoint,
					TEGrid const& grid)
{
	int idx = grid.getHorzCellIndex(basePoint.x);

	TriggerList& lst = chain.triggers;

	typedef boost::filter_iterator<te_h::test_for_horz_offset&, 
		TriggerList::iterator> FilterIter;

	te_h::test_for_horz_offset predicate(idx, idx + 1);
	FilterIter first(predicate, lst.begin(), lst.end());
	FilterIter last(predicate, lst.end(), lst.end());

	//если не можем сдвинуть хотя бы один элемент, 
	//из тех, что находятся в удаляемом столбце
	//то операцию проводить не будем
	int const fullCellWidth = grid.getFullCellWidth();
	std::unary_negate<te_h::test_can_shift_element> shift_predicate(
		te_h::test_can_shift_element(CSize(columnsCount*-fullCellWidth, 0), 
							logic, chain));

	if (std::find_if(first, last, shift_predicate) != last)
		return false;

	//чтобы рассматривать все элементы находящиеся справа от удаляемого столбца
	predicate.setIdxMax(std::numeric_limits<int>::max());
	{
		FilterIter first(predicate, lst.begin(), lst.end());
		FilterIter last(predicate, lst.end(), lst.end());

		te_h::offset_ele offseter(CSize(columnsCount*-fullCellWidth, 0),grid);
		std::for_each(first, last, offseter);
	}
	logic.remakeStrategyRect(chain);
//	logic.updateChainWorkArea(const_cast<TriggerChain&>(chain));
//	logic.updateChainViewRect(const_cast<TriggerChain&>(chain));

	return true;
}