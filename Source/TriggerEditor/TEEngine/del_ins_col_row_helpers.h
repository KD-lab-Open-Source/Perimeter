/********************************************************************
	created:	2003/07/30
	created:	30:7:2003   13:39
	filename: 	del_ins_col_row_helpers.h
	file base:	del_ins_col_row_helpers
	file ext:	h
	powerd by:	Илюха
	
	purpose:	функторы для удалления добавления 
*********************************************************************/
#include <limits>
#include "TEEngine/TEGrid.h"
#include "TEEngine/TriggerEditorLogic.h"

namespace te_h{
class test_for_vert_offset
{
	int idx_;
	int idx_max_;
public:
	test_for_vert_offset(int idx, int idx_max = INT_MAX)
		:idx_(idx), idx_max_(idx_max){}
	void setIdx(int idx){
		idx_ = idx;
	}
	void setIdxMax(int idx){
		idx_max_ = idx;
	}
	bool operator()(Trigger const& trigger) const{
		int const y = trigger.cellIndex().y;
		return (idx_ < y && y <= idx_max_);
	}
};

class test_for_horz_offset
{
	int idx_;
	int idx_max_;
public:
	test_for_horz_offset(int idx, int idx_max = INT_MAX):
	  idx_(idx), idx_max_(idx_max){}
	void setIdx(int idx){
		idx_ = idx;
	}
	void setIdxMax(int idx){
		idx_max_ = idx;
	}
	bool operator()(Trigger const& trigger) const{
		int const x = trigger.cellIndex().x;
		return (idx_ < x&& x <= idx_max_);
	}
};

class offset_ele{
	//! вспомогательаня переменная
	mutable CRect r_;
	//! сдвиг
	CSize offset_;
	//! полные ширина и высота ячейки
	TEGrid const& grid_;
public:
	offset_ele(CSize offset, TEGrid const& grid):
	  offset_(offset),grid_(grid){}

	  void operator()(Trigger & trigger) const
	  {
		  operator()(trigger, offset_);
	  }

	  void operator()(Trigger& trigger, const CSize &offset) const
	  {
		  r_ = trigger.boundingRect();
		  r_.OffsetRect(offset);
		  trigger.setBoundingRect(r_);
		  trigger.setCellIndex(grid_.getHorzCellIndex(r_.left), grid_.getVertCellIndex(r_.top));
	  }
};

class test_can_shift_element 
{
	mutable CRect rcTemp_;
	CSize offset_;
	TriggerEditorLogic const& logic_;
	TriggerChain const& chain_;
public:
	//для стандартной библиотеки
	typedef Trigger const argument_type;
	typedef bool result_type;

	test_can_shift_element(CSize const& offset, 
							TriggerEditorLogic const& logic,
							TriggerChain const& chain):
	offset_(offset), logic_(logic),
		chain_(chain)
	{}
	result_type operator()(argument_type& trigger) const
	{
		rcTemp_ = trigger.boundingRect();
		rcTemp_.OffsetRect(offset_.cx, offset_.cy);
		return (!logic_.findTrigger(chain_, rcTemp_.TopLeft()));
	}
};

}