#include "stdafx.h"
#include "resource.h"
#include "TEEngine/TriggerEditorView.h"
#include "teconditioncoloredeledrawer.h"

//#include "Game/GameInclude.h"

#include <Additional/qls.hpp>

int const LINK_PEN_WIDTH = 2;

TEConditionColoredEleDrawer::BrushVector_t 
	TEConditionColoredEleDrawer::linkBrushs_ = {
/*
		{TE_TYPE0_COLOR},
		{TE_TYPE1_COLOR},
		{TE_TYPE2_COLOR},
		{TE_TYPE3_COLOR},
		{TE_TYPE4_COLOR},
		{TE_TYPE5_COLOR},
		{TE_TYPE6_COLOR},
		{TE_TYPE7_COLOR},
		{TE_TYPE8_COLOR},
		{TE_TYPE9_COLOR}
*/
		CreateSolidBrush(TE_TYPE0_COLOR),
		CreateSolidBrush(TE_TYPE1_COLOR),
		CreateSolidBrush(TE_TYPE2_COLOR),
		CreateSolidBrush(TE_TYPE3_COLOR),
		CreateSolidBrush(TE_TYPE4_COLOR),
		CreateSolidBrush(TE_TYPE5_COLOR),
		CreateSolidBrush(TE_TYPE6_COLOR),
		CreateSolidBrush(TE_TYPE7_COLOR),
		CreateSolidBrush(TE_TYPE8_COLOR),
		CreateSolidBrush(TE_TYPE9_COLOR),
//		CreateSolidBrush(TE_TYPE10_COLOR)
};

TEConditionColoredEleDrawer::PenVector_t 
	TEConditionColoredEleDrawer::linkPens_ = {
/*
	{TE_TYPE0_COLOR},
	{TE_TYPE1_COLOR},
	{TE_TYPE2_COLOR},
	{TE_TYPE3_COLOR},
	{TE_TYPE4_COLOR},
	{TE_TYPE5_COLOR},
	{TE_TYPE6_COLOR},
	{TE_TYPE7_COLOR},
	{TE_TYPE8_COLOR},
	{TE_TYPE9_COLOR}
*/
		CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE0_COLOR),
		CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE1_COLOR),
		CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE2_COLOR),
		CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE3_COLOR),
		CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE4_COLOR),
		CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE5_COLOR),
		CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE6_COLOR),
		CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE7_COLOR),
		CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE8_COLOR),
		CreatePen(PS_SOLID, LINK_PEN_WIDTH, TE_TYPE9_COLOR)
};

TEConditionColoredEleDrawer::BrushIdx_t 
	TEConditionColoredEleDrawer::brushIndex_(
		TEConditionColoredEleDrawer::initBrushIdx());

/*
TEConditionColoredEleDrawer::LinkPen::LinkPen(COLORREF crColor)
:CPen(PS_SOLID, 2, crColor)
{
}

*/

TEConditionColoredEleDrawer::LinkPen::~LinkPen()
{
	DeleteObject(pen_);
}
TEConditionColoredEleDrawer::LinkPen::operator HPEN(){
	return pen_;
}
TEConditionColoredEleDrawer::Brush::operator HBRUSH(){
	return brush_;
}
TEConditionColoredEleDrawer::Brush::~Brush(){
	DeleteObject(brush_);
}

TEConditionColoredEleDrawer::TEConditionColoredEleDrawer(void)
{
}

TEConditionColoredEleDrawer::~TEConditionColoredEleDrawer(void)
{
}


HBRUSH	TEConditionColoredEleDrawer::SelectTriggerBackBrush(Trigger const& trigger) const
{
	if (Condition* condition = trigger.condition)
	{
		if (condition->inversed())
		{
			static CBrush inversedConditionBrush_(INVERSE_CONDITION_COLOR);
			return inversedConditionBrush_;
		}

		COLORREF const conditionColor = condition->color();
		if (conditionColor)
		{
			HBRUSH &brush = brushIndex_[conditionColor].second;
			if (!brush)
				brush = CreateSolidBrush(conditionColor);

			return brush;
		}
	}

	return reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
}

HBRUSH	TEConditionColoredEleDrawer::SelectLinkBrushColor(TriggerLink const& link) const
{
	return getLinkTypeBrush(link.getType());
}

HPEN	TEConditionColoredEleDrawer::SelectLinkPenColor(TriggerLink const& link) const
{
	return getLinkTypePen(link.getType());
}

TEConditionColoredEleDrawer::BrushIdx_t const TEConditionColoredEleDrawer::initBrushIdx()
{
	BrushIdx_t BrushIdx;
/*	BrushIdx.reserve(20);
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_MOUSE_CLICK,
		DEFAULT_VALUE(COND_MOUSE_CLICK_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_MOUSE_OBJECT_CLICK,
		DEFAULT_VALUE(COND_MOUSE_OBJECT_CLICK_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_IN_ZONE,
		DEFAULT_VALUE(COND_OBJECT_IN_ZONE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_PERSONAGE_WALK_DIRECTION,
		DEFAULT_VALUE(COND_PERSONAGE_WALK_DIRECTION_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_PERSONAGE_STATIC_DIRECTION,
		DEFAULT_VALUE(COND_PERSONAGE_STATIC_DIRECTION_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_TIMER,
		DEFAULT_VALUE(COND_TIMER_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_MOUSE_DIALOG_CLICK,
		DEFAULT_VALUE(COND_MOUSE_DIALOG_CLICK_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_MINIGAME_STATE,
		DEFAULT_VALUE(COND_MINIGAME_IN_STATE_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_STATE,
		DEFAULT_VALUE(COND_OBJECT_STATE_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_MOUSE_ZONE_CLICK,
		DEFAULT_VALUE(COND_MOUSE_ZONE_CLICK_COLOR)));
	BrushIdx.push_back(BRUSH_INDEX_ENTRY(qdCondition::CONDITION_MOUSE_OBJECT_ZONE_CLICK,
		DEFAULT_VALUE(COND_MOUSE_OBJECT_ZONE_CLICK_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_STATE_WAS_ACTIVATED,
		DEFAULT_VALUE(COND_OBJECT_STATE_WAS_ACTIVATED_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_STATE_WAS_NOT_ACTIVATED,
		DEFAULT_VALUE(COND_OBJECT_STATE_WAS_NOT_ACTIVATED_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECTS_DISTANCE,
		DEFAULT_VALUE(COND_OBJECTS_DISTANCE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_PERSONAGE_ACTIVE,
		DEFAULT_VALUE(COND_PERSONAGE_ACTIVE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_STATE_WAITING,
		DEFAULT_VALUE(COND_OBJECT_STATE_WAITING_COLOR)));

	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_STATE_ANIMATION_PHASE,
		DEFAULT_VALUE(COND_OBJECT_STATE_ANIMATION_PHASE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_PREV_STATE,
		DEFAULT_VALUE(COND_OBJECT_PREV_STATE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_VALUE,
		DEFAULT_VALUE(COND_STATE_TIME_GREATER_THAN_VALUE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_STATE_TIME_GREATER_THAN_STATE_TIME,
		DEFAULT_VALUE(COND_STATE_TIME_GREATER_THAN_STATE_TIME_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_STATE_TIME_IN_INTERVAL,
		DEFAULT_VALUE(COND_STATE_TIME_IN_INTERVAL_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_COUNTER_GREATER_THAN_VALUE,
		DEFAULT_VALUE(COND_COUNTER_GREATER_THAN_VALUE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_COUNTER_LESS_THAN_VALUE,
		DEFAULT_VALUE(COND_COUNTER_LESS_THAN_VALUE_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_COUNTER_GREATER_THAN_COUNTER,
		DEFAULT_VALUE(COND_COUNTER_GREATER_THAN_COUNTER_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_COUNTER_IN_INTERVAL,
		DEFAULT_VALUE(COND_COUNTER_IN_INTERVAL_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_OBJECT_ON_PERSONAGE_WAY,
		DEFAULT_VALUE(COND_OBJECT_ON_PERSONAGE_WAY_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_KEYPRESS,
		DEFAULT_VALUE(COND_KEYPRESS_COLOR)));
	BrushIdx.push_back(
		BRUSH_INDEX_ENTRY(qdCondition::CONDITION_ANY_PERSONAGE_IN_ZONE,
		DEFAULT_VALUE(COND_ANY_PERSONAGE_IN_ZONE_COLOR)));
*/
	return BrushIdx;
}
