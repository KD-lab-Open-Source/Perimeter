#include "stdafx.h"
#include "deleterows.h"
#include "TEEngine/te_helpers.h"
#include "TriggerExport.h"
#include "TEEngine/TriggerEditorLogic.h"

#include "TEEngine/del_ins_col_row_helpers.h"

#include <functional>
#include <boost/iterator/filter_iterator.hpp>

DeleteRows::DeleteRows(size_t rowsCount, 
					TriggerEditorLogic const& logic, 
					TriggerChain const &chain,
					CPoint const& basePoint,
					TEGrid const& grid):
  rowsCount_(rowsCount)
, logic_(logic)
, chain_(chain)
, basePoint_(basePoint)
, grid_(grid)
{
}

DeleteRows::~DeleteRows(void)
{
}

bool DeleteRows::operator()()
{
	return run(rowsCount_, logic_, chain_, basePoint_, grid_);
}

bool DeleteRows::run(size_t rowsCount, 
					TriggerEditorLogic const& logic, 
					TriggerChain const& chain,
					CPoint const& basePoint,
					TEGrid const& grid)
{
	int const idx = grid.getVertCellIndex(basePoint.y);

	TriggerList& lst = const_cast<TriggerList&>(chain.triggers);
	typedef TriggerList::iterator BaseIterator;
	typedef boost::filter_iterator<
		te_h::test_for_vert_offset&, BaseIterator> FilterIter;

	te_h::test_for_vert_offset predicate(idx, idx + 1);
	FilterIter first(predicate, lst.begin(), lst.end());
	FilterIter last(predicate, lst.end(), lst.end());

	//если не можем сдвинуть хотя бы один элемент, 
	//из тех, что находятся в удаляемом столбце
	//то операцию проводить не будем
	int const fullCellHeight = grid.getFullCellHeight();
	std::unary_negate<te_h::test_can_shift_element> 
		shift_predicate(te_h::test_can_shift_element(
			CSize(0, rowsCount*-fullCellHeight), logic, chain));

	if (std::find_if(first, last, shift_predicate) != last)
		return false;

	//чтобы рассматривать все элементы находящиеся справа от удаляемого столбца
	predicate.setIdxMax(std::numeric_limits<int>::max());
	{
		FilterIter first(predicate, lst.begin(), lst.end());
		FilterIter last (predicate, lst.end(), lst.end());

		te_h::offset_ele offseter(CSize(0, rowsCount*-fullCellHeight), grid);
		std::for_each(first, last, offseter);
	}

	logic.remakeStrategyRect(chain);
//	logic.updateChainWorkArea(const_cast<TriggerChain&>(chain));
//	logic.updateChainViewRect(const_cast<TriggerChain&>(chain));

	return true;
}