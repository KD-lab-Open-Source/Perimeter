#include "stdafx.h"
#include "resource.h"

#include "TEBaseWorkMode.h"
#include "TEEngine/TriggerEditorView.h"
#include "TEEngine/TriggerEditorLogic.h"

#include "TEEngine/TEGrid.h"
#include "TEEngine/te_helpers.h"
#include "TEEngine/te_consts.h"

//#include "Game/GameInclude.h"

#include "TriggerExport.h"

#include "TEEngine/TEUtils/BoundingRectCalculator.h"

#include "TEEngine/Drawers/TEBaseWorkModeDrawerBase.h"

#include "Utils/menu_helpers.h"
#include "Utils/EnsureTriggerNameOriginality.h"

#include "TEEngine/Actions/MoveLinkEnd.h"
#include "TEEngine/Actions/MakeLinkActive.h"
#include "TEEngine/Actions/MoveTEElementPos.h"
#include "TEEngine/Actions/ChangeColor.h"

#include "TEEngine/Actions/AddLink.h"
#include "TEEngine/Actions/DeleteLink.h"

#include "TEEngine/Actions/AddElement.h"
#include "TEEngine/Actions/DeleteElement.h"

#include "TEEngine/Actions/InsertColumns.h"
#include "TEEngine/Actions/DeleteColumns.h"

#include "TEEngine/Actions/InsertRows.h"
#include "TEEngine/Actions/DeleteRows.h"

#include "TEEngine/Actions/ChangeLinkType.h"
#include "TEEngine/Actions/ChangeAutoRestart.h"

#include "TEEngine/Actions/RenameElement.h"

#include "Main/ChooseName.h"

#include <algorithm>
#include <boost/utility.hpp>

#include "TEEngine/TriggerClipBuffer.h"
#include "TEEngine/SelectIfIntersect.h"

#include "TEEngine/WorkModes/WMUtils/ClipBoardDataInserter.h"
#include "TEEngine/WorkModes/WMUtils/CopyToClipboard.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using te_h::getLinkOffset;

TEBaseWorkMode::TEBaseWorkMode(TriggerEditorLogic* logic)
: TESingleChainWorkMode(logic)
, isMouseMoved_(false)
, activeLinkIndex_(-1)
, linkOwnerIndex_(-1)
, drawedLinkType_(0)
, activeLinkType_(0)
, linkAnchorTestResult_(LAT_NONE)
, betweenPlotsLayoutTestResult_(BPL_NOWERE)
, hilitedLinksTriggerIndex_(-1)
, drawer_(NULL)
, dataKeeper_(new DataKeeper(NULL))
{
	dataKeeper_->setWorkMode(this);
}

TEBaseWorkMode::~TEBaseWorkMode(void)
{
}

void TEBaseWorkMode::setMouseMoved(bool bVal){
	isMouseMoved_ = bVal;
}

bool TEBaseWorkMode::getMouseMoved() const{
	return isMouseMoved_;
}

void TEBaseWorkMode::setTriggerChain(TriggerChain* chain){
	if (getTriggerChain() != chain)
	{
		setActiveLinkIndex(-1);
		setLinkOwnerIndex(-1);
		selectionManager_.setTriggerChain(chain);
		TESingleChainWorkMode::setTriggerChain(chain);
	}
}

void TEBaseWorkMode::setLinkOwnerIndex(int idx){
	linkOwnerIndex_ = idx;
}
int TEBaseWorkMode::getLinkOwnerIndex() const{
	return linkOwnerIndex_;
}

void TEBaseWorkMode::setActiveLinkIndex(int idx){
	activeLinkIndex_ = idx;
}
int TEBaseWorkMode::getActiveLinkIndex() const{
	return activeLinkIndex_;
}

void TEBaseWorkMode::setDrawedLinkType(int newType){
	drawedLinkType_ = newType;
}
int TEBaseWorkMode::getDrawedLinkType() const{
	return drawedLinkType_;
}

void TEBaseWorkMode::setCurrentPoint(CPoint const& point){
	currentPoint_ = point;
}
CPoint const& TEBaseWorkMode::getCurrentPoint() const{
	return currentPoint_;
}

void TEBaseWorkMode::setLinkAnchorTestResult(eLinkAnchorTest eVal){
	linkAnchorTestResult_ = eVal;
}
TEBaseWorkMode::eLinkAnchorTest TEBaseWorkMode::getLinkAnchorTestResult() const{
	return linkAnchorTestResult_;
}

void TEBaseWorkMode::setActiveLinkType(int iVal){
	activeLinkType_ = iVal;
}
int TEBaseWorkMode::getActiveLinkType() const{
	return activeLinkType_;
}

void TEBaseWorkMode::setBetweenPlotsLayoutTestResult(eBetweenPlotsLayout eVal){
	betweenPlotsLayoutTestResult_ = eVal;
}
TEBaseWorkMode::eBetweenPlotsLayout 
	TEBaseWorkMode::getBetweenPlotsLayoutTestResult() const{
	return betweenPlotsLayoutTestResult_;
}

bool TEBaseWorkMode::setDrawer(TEBaseWorkModeDrawerBase* ptrDrawer){
	if (!ptrDrawer)
		return false;
	drawer_ = ptrDrawer;
	return true;
}

int TEBaseWorkMode::getHilitedLinksTriggerIndex() const{
	return hilitedLinksTriggerIndex_;
}
void TEBaseWorkMode::setHilitedLinksTriggerIndex(int idx){
	hilitedLinksTriggerIndex_ = idx;
}

Trigger const* TEBaseWorkMode::getHilitedLinksTrigger() const{
	int const index = getHilitedLinksTriggerIndex();
	if (index == -1)
		return NULL;
	return &getStrategyTrigger(getHilitedLinksTriggerIndex());
}

TEUndoManager& TEBaseWorkMode::getUndoManager(TriggerChain const*chain){
	assert(chain);
	return undoBuffers_[chain];
}

void TEBaseWorkMode::OnLButtonDown(TriggerEditorView* source, UINT nFlags, CPoint point)
{
	if (getCanDragContent())
	{
		Base::OnLButtonDown(source, nFlags, point);
		return;
	}
	CPoint p(point);
	source->scr2Log(&p, 1);

	setMouseMoved(false);
	setBasePoint(p);
	if (getActiveLinkIndex() != -1) {
		TriggerLink& link = getStrategyLink(getLinkOwnerIndex(), getActiveLinkIndex());
		//проверяем не попали ли мы в его якоря
		linkAnchorTestResult_ = findLinkAnchor(link, p);

		switch(linkAnchorTestResult_) {
		case LAT_BEGIN:
			{
				setCurrentPoint(te_h::getLinkBegin(*link.parent, link));
				undo_ = MoveLinkEnd(getActiveLinkIndex(), 
									getLinkOwnerIndex(), 
									*getTriggerChain(),
									link.parentOffset(), 
									MoveLinkEnd::MLE_OWNER_OFFSET);

			}
			return;
		case LAT_END:
			{
				setCurrentPoint(te_h::getLinkEnd(link));
				undo_ = MoveLinkEnd(getActiveLinkIndex(), 
									getLinkOwnerIndex(), 
									*getTriggerChain(),
									link.childOffset(), 
									MoveLinkEnd::MLE_CHILD_OFFSET);

			}
			return;
		}
	}

	setBaseElement(-1);

	//! first - индекс связи, second - индекс элемента
	std::pair<int, int> const res = 
		getTriggerEditorLogic()->findLink(*getTriggerChain(), p);

	if (res.first != getActiveLinkIndex()||res.second != getLinkOwnerIndex())
		activateLink(res.first, res.second);

	if (getActiveLinkIndex() != -1) 
		source->Invalidate();
	else
	{
		bool const shiftPressed = te_h::IsKeyPressed(VK_SHIFT);
		int triggerIndex = getTriggerEditorLogic()->findTrigger(*getTriggerChain(), p);
		setBaseElement(triggerIndex);
		if (triggerIndex != -1) 
		{
			if (!shiftPressed)
			{
				if (!selectionManager_.isSelected(triggerIndex))
				{
					selectionManager_.deselectAll();
					selectionManager_.select(triggerIndex);
					source->RedrawWindow();
				}
				else
					selectionManager_.reselect(triggerIndex);

				TriggerEditorLogic const& logic = *getTriggerEditorLogic();
				TEGrid const& grid = logic.getGrid();
				CPoint startMove(getStrategyTrigger(triggerIndex).boundingRect().TopLeft());
				setCurrentPoint(CPoint(INT_MIN, INT_MIN));
				source->SetCapture();
			}
			else
			{
				if (selectionManager_.getCount() != 1
					&&selectionManager_.isSelected(triggerIndex))
					selectionManager_.deselect(triggerIndex);
				else
					selectionManager_.select(triggerIndex);
				source->RedrawWindow();
			}
		}
		else
		{
			CRectTracker rectTracker;
			if (!rectTracker.TrackRubberBand(source, point))
			{
				source->SendMessage(WM_LBUTTONUP, nFlags, MAKELPARAM(point.x, point.y));
			}
			else
			{
				rectTracker.m_rect.NormalizeRect();
				source->scr2Log(&rectTracker.m_rect);

				TriggerList& triggers = getTriggerChain()->triggers;
				
				if (!te_h::IsKeyPressed(VK_SHIFT)) 
					getSelectionManager().deselectAll();
				std::for_each(triggers.begin(), triggers.end(), 
					SelectIfIntersect(rectTracker.m_rect, getSelectionManager()));

			}
			source->Invalidate();
		}
	}
}

void TEBaseWorkMode::OnLButtonUp(TriggerEditorView* source, UINT nFlags, CPoint point)
{
	BaseElementGuard baseElementGuard(this);

	if (getCanDragContent())
	{
		Base::OnLButtonUp(source, nFlags, point);
		return;
	}
	source->scr2Log(&point,1);
	if (getBaseElement() != -1) 
	{
		bool const shiftPressed = te_h::IsKeyPressed(VK_SHIFT);

		if (!shiftPressed)
		{
			ReleaseCapture();
			if (getMouseMoved())
			{
				drawDraggedTitles(source);
				moveSelection(source, point);
			}
			else
			{
				selectionManager_.deselectAll();
				selectionManager_.select(getBaseElement());
			}
		}
		source->InvalidateRect(NULL);
	}
	else 
	{
		if (!getMouseMoved()&&!te_h::IsKeyPressed(VK_SHIFT))
		{
			selectionManager_.deselectAll();
			source->InvalidateRect(NULL);
		}
		if (getActiveLinkIndex() != -1) 
		{
			if (getMouseMoved()) 
			{
				TEUndoManager::Action redo;
				TriggerLink const& link = 
					getStrategyLink(getLinkOwnerIndex(), getActiveLinkIndex());
				switch(linkAnchorTestResult_) {
				case LAT_BEGIN:
					{
						redo = MoveLinkEnd(getActiveLinkIndex(), 
											getLinkOwnerIndex(),
											*getTriggerChain(),
											link.parentOffset(),
											MoveLinkEnd::MLE_OWNER_OFFSET);
						getTriggerEditorLogic()->setDataChanged();
					}
					break;
				case LAT_END:
					{
						redo = MoveLinkEnd(getActiveLinkIndex(), 
											getLinkOwnerIndex(),
											*getTriggerChain(),
											link.childOffset(),
											MoveLinkEnd::MLE_CHILD_OFFSET);
						getTriggerEditorLogic()->setDataChanged();
					}
					break;
				}
				if (undo_) 
					getUndoManager(getTriggerChain()).addAction(undo_, redo);
				undo_ = NULL;
			}
			if (linkAnchorTestResult_ != LAT_NONE){
				setLinkAnchorTestResult(LAT_NONE);
				source->InvalidateRect(NULL);
			}
		}
	}
}

void TEBaseWorkMode::OnLButtonDblClk(TriggerEditorView* source, UINT nFlags, CPoint p)
{
	source->scr2Log(&p, 1);
	int triggerIndex = getTriggerEditorLogic()->findTrigger(*getTriggerChain(), p);
	if (triggerIndex == -1 || triggerIndex == 0)
		return;
	if (triggerInterface().editCondition(getStrategyTrigger(triggerIndex), 
		source->GetParent()->m_hWnd))
	{
		getTriggerEditorLogic()->setDataChanged();
		source->InvalidateRect(NULL);
	}
}

void TEBaseWorkMode::OnRButtonDown(TriggerEditorView* source, UINT nFlags, CPoint point)
{
	CPoint p(point);
	source->scr2Log(&p, 1);

	std::pair<int, int> const res = 
		getTriggerEditorLogic()->findLink(*getTriggerChain(), p);
	setMouseMoved(false);
	if (res.first != -1) 
	{// если попали в связь
		activateLink(res.first, res.second);
		setBasePoint(p);
		setCurrentPoint(p);
		source->Invalidate();
	}
	else
	{
		if (getActiveLinkIndex() != -1)
		{
			activateLink(-1, -1);
		}

		int triggerIndex = getTriggerEditorLogic()->findTrigger(*getTriggerChain(), p);
		setBaseElement(triggerIndex);
		if (triggerIndex != -1) {

			if (!selectionManager_.isSelected(triggerIndex))
			{	
				selectionManager_.deselectAll();
				selectionManager_.select(triggerIndex);
				source->Invalidate();
			}
			else
				selectionManager_.reselect(triggerIndex);

			setBasePoint(p);
			setCurrentPoint(p);
			source->SetCapture();
			drawer_->drawDragedLink(source, source->getDC(), 
				getCurrentPoint(), getBasePoint(),-1);
			setDrawedLinkType(-1);
		}
	}
	Base::OnRButtonDown(source, nFlags, point);
}

void TEBaseWorkMode::OnRButtonUp(TriggerEditorView* source, UINT nFlags, CPoint point)
{
	BaseElementGuard baseElementGuard(this);
	CPoint p(point);
	source->scr2Log(&p, 1);
	if (!getMouseMoved()) 
	{
		if (getBaseElement() == -1)
		{
			selectionManager_.deselectAll();
			source->Invalidate();
		}
		if (getActiveLinkIndex() != -1)
		{
			trackPopupMenu(IDR_TE_LINK, source);
			Base::OnRButtonUp(source, nFlags, point);
			return;
		}
		else if (TestForBetweenPlots(source, p))
		{
			//полагаемся потом на эту точку, потому что 
			//курсор после выбора пункта меню находится непонятно где.
			setBasePoint(p);
			trackPopupMenu(IDR_BETWEEN_PLOTS_MENU, source);
			Base::OnRButtonUp(source, nFlags, point);
			return;
		}
	}
	if (getBaseElement() != -1) 
	{//т.е. мы кликнули в элемент

		ReleaseCapture();
		drawer_->drawDragedLink(source, source->getDC(), 
			getCurrentPoint(), getBasePoint(),getDrawedLinkType());

		if (!getMouseMoved()) {//нажали и отпустили. Не тянули!
			trackPopupMenu(IDR_TE_TRIGGER_ELE, source);
		}
		else
		{
			TriggerChain &chain = *getTriggerChain();
			TriggerEditorLogic const* logic = source->getTriggerEditorLogic();
			int triggerIndex = logic->findTrigger(chain, p);

			int lastSelected = selectionManager_.getLast();
			// lastSelected совпадает с baseElement
			assert(getBaseElement() == lastSelected);
			if (triggerIndex != -1&&canPlugLink(triggerIndex, lastSelected, chain)) 
			{
				Trigger const& childTrigger = getStrategyTrigger(triggerIndex);
				Trigger const& parentTrigger = getStrategyTrigger(lastSelected);
				AddLink redo(chain, lastSelected, triggerIndex, 
							//! отступ у дочернего элемента линка
							getLinkOffset(p, getCurrentPoint(), 
								childTrigger.boundingRect()),
							//! отступ у элемента владеющего линком
							getLinkOffset(getCurrentPoint(), p, 
								parentTrigger.boundingRect()),
								0, false);
				DeleteLink undo(chain, lastSelected, triggerIndex);
				if (redo())
				{
					getUndoManager(&chain).addAction(undo, redo);
					activateLink(
								logic->findLinkByChild(parentTrigger, childTrigger), 
								lastSelected);
					getTriggerEditorLogic()->setDataChanged();
				}

				source->Invalidate();
			}
		}
	}
	Base::OnRButtonUp(source, nFlags, point);
}

void TEBaseWorkMode::OnMouseMove(TriggerEditorView* source, UINT nFlags, CPoint point)
{
	if (getCanDragContent())
	{
		Base::OnMouseMove(source, nFlags, point);
		return;
	}
	if (!(nFlags&MK_RBUTTON||nFlags&MK_LBUTTON))
		trackToolTip(source, point);

	if (linkAnchorTestResult_ != LAT_NONE)
	{
		if (nFlags&MK_LBUTTON) {
			source->scr2Log(&point, 1);

			setMouseMoved(getBasePoint()!=point);
			Trigger const&trigger = 
					getStrategyTrigger(getLinkOwnerIndex());
			TriggerLink const& link = getStrategyLink(trigger, getActiveLinkIndex());
			drawer_->drawAnchoredLink(source, source->getDC(), trigger, link);
			moveLinkEnd(point);
			drawer_->drawAnchoredLink(source, source->getDC(), trigger, link);
		}
	}
	else if (getBaseElement() != -1) 
	{
		source->scr2Log(&point, 1);

		if (nFlags&MK_RBUTTON) {
			setMouseMoved(getBasePoint() != point);
			TriggerEditorLogic const& logic = *getTriggerEditorLogic();
			TriggerChain const &chain = *getTriggerChain();
			int triggerIndex = logic.findTrigger(chain, point);

			int iType = getActiveLinkType();

			if(triggerIndex != -1
				&&!canPlugLink(triggerIndex, selectionManager_.getLast(), chain))
				iType = -1;

			drawer_->drawDragedLink(source, source->getDC(), 
				getCurrentPoint(), getBasePoint(),getDrawedLinkType());

			if (DWORD dwPlace = source->pointInScrollArea(point))
			{
				const int SCROLL_DELTA = 12;
				if (dwPlace&TriggerEditorView::SAP_LEFT)
					source->scrollView(SCROLL_DELTA, 0);
				if (dwPlace&TriggerEditorView::SAP_RIGHT)
					source->scrollView(-SCROLL_DELTA, 0);
				if (dwPlace&TriggerEditorView::SAP_TOP)
					source->scrollView(0, SCROLL_DELTA);
				if (dwPlace&TriggerEditorView::SAP_BOTTOM)
					source->scrollView(0, -SCROLL_DELTA);
			}


			setBasePoint(point);
			setDrawedLinkType(iType);
			drawer_->drawDragedLink(source, source->getDC(), 
				getCurrentPoint(), getBasePoint(),getDrawedLinkType());
		}
		else if (nFlags&MK_LBUTTON)
		{
			setMouseMoved(getBasePoint()!=point);
			TriggerEditorLogic const& logic = *getTriggerEditorLogic();
			TEGrid const& grid = logic.getGrid();
			CPoint p(grid.toCellLeft(point.x), grid.toCellTop(point.y));
			if (p != getCurrentPoint())// прешел ли курсор на другую ячейку
			{
				if (moveDraggedTitles(p))
				{
					if (getCurrentPoint() != CPoint(INT_MIN, INT_MIN))
						drawDraggedTitles(source);
					setCurrentPoint(p);
					drawDraggedTitles(source);
				}
			}

			setBasePoint(point);
		}
	}
}

void TEBaseWorkMode::moveLinkEnd(CPoint const& point)
{
	CPoint end;
	switch (linkAnchorTestResult_)
	{
	case LAT_END:
	{
		TriggerLink const& activeLink = 
			getStrategyLink(getLinkOwnerIndex(), getActiveLinkIndex());
		CRect const& boundingRect = activeLink.child->boundingRect();
		if(PtInRect(&boundingRect, point))
		{
			setBasePoint(point);
			end = 
				te_h::getLinkBegin(getStrategyTrigger(getLinkOwnerIndex()), activeLink);
			//Need Undo
			MoveLinkEnd::run(getActiveLinkIndex(), getLinkOwnerIndex(), 
				*getTriggerChain(), getLinkOffset(point, end,boundingRect),
				MoveLinkEnd::MLE_CHILD_OFFSET);

		}
		break;
	}
	case LAT_BEGIN:
	{
		Trigger const& linkOwner = getStrategyTrigger(getLinkOwnerIndex());
		RECT const& boundingRect = linkOwner.boundingRect();
		if (PtInRect(&boundingRect, point)) 
		{
			TriggerLink const& activeLink = 
				getStrategyLink(linkOwner, getActiveLinkIndex());
			setBasePoint(point);
			end = te_h::getLinkEnd(activeLink);
			//! Need Undo
			MoveLinkEnd::run(getActiveLinkIndex(), 
							getLinkOwnerIndex(),
							*getTriggerChain(),
							getLinkOffset(point, end, boundingRect),
							MoveLinkEnd::MLE_OWNER_OFFSET);
		}
		break;
	}
	}
}

void TEBaseWorkMode::OnKeyDown(TriggerEditorView* source, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	Base::OnKeyDown(source, nChar, nRepCnt, nFlags);
}

void TEBaseWorkMode::OnKeyUp(TriggerEditorView* source, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	Base::OnKeyUp(source, nChar, nRepCnt, nFlags);
}

bool TEBaseWorkMode::OnStart(){
	return Base::OnStart();
}
bool TEBaseWorkMode::OnEnd()
{
	setHilitedLinksTriggerIndex(-1);
	return true;
}

BOOL TEBaseWorkMode::OnDropData(TriggerEditorView* source, void* pData, 
								const CPoint& point)
{
	void *action = getDraggedData(pData);

	CPoint p(point);
	source->scr2Log(&p, 1);

	
	TriggerChain & chain = *getTriggerChain();
	Trigger trigger;
	if (initTrigger(source, &trigger, reinterpret_cast<int>(action)))
	{
		TriggerEditorLogic const& logic = *getTriggerEditorLogic();
		int const triggerIndex = chain.triggers.size();
		TEGrid const& grid = logic.getGrid();
		CPoint const cell = grid.getCellByPoint(p);
		AddElement redo(grid, cell, chain, trigger, triggerIndex);
		DeleteElement undo(chain, triggerIndex);
		if (redo())
		{
			getUndoManager(&chain).addAction(undo, redo);
			getTriggerEditorLogic()->setDataChanged();
		}
		else{
			undo();
		}
		source->InvalidateRect(NULL);
		return TRUE;
	}
	return FALSE;
}

bool TEBaseWorkMode::CanDrop(TriggerEditorView* source, void* pData, const CPoint& point)
{
//	void* res = getDraggedData(pData);
	//owner - содержит владельца по дереву, т.е. объект, 
	//под узлом которого
	//находится данный объект(pobj)
	TriggerChain const& chain = *getTriggerChain();

	CPoint _p(point);
	source->scr2Log(&_p, 1);
	TriggerEditorLogic const& logic = *getTriggerEditorLogic();
	logic.getGrid().toCellLeftTop(&_p);
	if (logic.findTrigger(chain, _p) == -1)
		return true;

	return false;
}

void* TEBaseWorkMode::getDraggedData(void* pData) const
{
	return *reinterpret_cast<void**>(pData);
}

void TEBaseWorkMode::OnTeLinkDelete(TriggerEditorView* source)
{
	ASSERT(getLinkOwnerIndex() != -1);
	ASSERT(getActiveLinkIndex() != -1);

	int const linkOwnerIndex = getLinkOwnerIndex();
	int const linkIndex = getActiveLinkIndex();
	activateLink(-1, -1);

	TriggerChain &chain = *getTriggerChain();
	TriggerLink const& link = getStrategyLink(linkOwnerIndex, linkIndex);
	int const childElementIndex = getStrategyTriggerIndex(chain, *link.child);

	TriggerEditorLogic& logic = *getTriggerEditorLogic();
	TEUndoManager::Action aUndo;
	TEUndoManager::Action aRedo;
	if (logic.removeLink(chain, link, linkOwnerIndex, childElementIndex, aUndo, aRedo))
	{
		getUndoManager(&chain).addAction(aUndo, aRedo);
		getTriggerEditorLogic()->setDataChanged();
	}

	source->Invalidate();
}

void TEBaseWorkMode::deleteSelection(TEUndoManager::Bunch& undoBunch,
									 TEUndoManager::Bunch& redoBunch)
{
	if (selectionManager_.getCount() == 0)
		return;

	TriggerChain &chain = *getTriggerChain();
	if (getLinkOwnerIndex() != -1 && getActiveLinkIndex() != -1)
	{
		TriggerLink const& link = 
			getStrategyLink(getLinkOwnerIndex(), getActiveLinkIndex());

		if (selectionManager_.isSelected(getLinkOwnerIndex())
			||selectionManager_.isSelected(getStrategyTriggerIndex(chain, *link.child)))
			activateLink(-1, -1);
	}

	std::vector<int> sel(selectionManager_.begin(), selectionManager_.end());
	selectionManager_.deselectAll();

	std::sort(sel.begin(), sel.end(), std::greater<int>());
	std::vector<int>::const_iterator i = sel.begin(), e = sel.end();
	for(; i != e; ++i){
		if (*i != 0)
			getTriggerEditorLogic()->deleteTrigger(chain, *i, undoBunch, redoBunch);
	}
	if (sel.back() == 0)
		selectionManager_.select(0);

}

void TEBaseWorkMode::deleteSelection()
{
	TEUndoManager::Bunch undoBunch, redoBunch;
	deleteSelection(undoBunch, redoBunch);

	if (!redoBunch.empty()&&!undoBunch.empty()){
		undoBunch.reverse();
		getUndoManager(getTriggerChain()).addAction(undoBunch, redoBunch);
		getTriggerEditorLogic()->setDataChanged();
	}
}

void TEBaseWorkMode::OnTEDeleteTrigger(TriggerEditorView* source)
{
	CWaitCursor wait;
	deleteSelection();
	source->Invalidate();
}

void TEBaseWorkMode::OnTeMakeActive(TriggerEditorView* source)
{
/*
	CWaitCursor __wait__;
	ASSERT(getActiveTriggerIndex() != -1);

	TriggerChain const& chain = *getTriggerChain();
	if (getActiveTE()->is_active())
	{
		getActiveT()->make_active(false);
		if (pchain->root_element() != getActiveTE()) 
			pchain->root_element()->make_active(true);
	}
	else
	{
		clearActive(pchain);
		getActiveTE()->make_active(true);
	}
	strategy->init_debug_check();

	source->InvalidateRect(NULL);
*/
}
void TEBaseWorkMode::OnClearStarts(TriggerEditorView* source)
{
/*
	qdTriggerChain* pc = getTriggerChain();
	clearActive(pc);
	pc->root_element()->make_active(true);
	pc->init_debug_check();
	pwnd->Invalidate();
*/
}
void TEBaseWorkMode::OnTeLinkType(UINT id, TriggerEditorView* source)
{
	ASSERT(getLinkOwnerIndex() != -1);
	ASSERT(getActiveLinkIndex() != -1);
	ColorType type = STRATEGY_RED;
	switch(id) {
	case IDC_TE_LINK_TYPE0:
		type = STRATEGY_RED;
		break;
	case IDC_TE_LINK_TYPE1:
		type = STRATEGY_GREEN;
		break;
	case IDC_TE_LINK_TYPE2:
		type = STRATEGY_BLUE;
		break;
	case IDC_TE_LINK_TYPE3:
		type = STRATEGY_YELLOW;
		break;
	case IDC_TE_LINK_TYPE4:
		type = STRATEGY_COLOR_0;
		break;
	case IDC_TE_LINK_TYPE5:
		type = STRATEGY_COLOR_1;
		break;
	case IDC_TE_LINK_TYPE6:
		type = STRATEGY_COLOR_2;
		break;
	case IDC_TE_LINK_TYPE7:
		type = STRATEGY_COLOR_3;
		break;
	case IDC_TE_LINK_TYPE8:
		type = STRATEGY_COLOR_4;
		break;
	case IDC_TE_LINK_TYPE9:
		type = STRATEGY_COLOR_5;
		break;
	default:
		return;
	}
	changeActiveLinkType(type);	
	source->Invalidate();
}

void TEBaseWorkMode::changeActiveLinkType(int type){

	ChangeLinkType redo(*getTriggerChain(), 
						getLinkOwnerIndex(), 
						getActiveLinkIndex(), 
						type);
	TriggerLink& activeLink = 
		getStrategyLink(getLinkOwnerIndex(), getActiveLinkIndex());
	ChangeLinkType undo(*getTriggerChain(), 
						getLinkOwnerIndex(), 
						getActiveLinkIndex(), 
						activeLink.getType());
	if (redo())
	{
		getUndoManager(getTriggerChain()).addAction(undo, redo);
		getTriggerEditorLogic()->setDataChanged();
	}
}

void TEBaseWorkMode::changeAutoRestart4ActiveLink(){

	TriggerLink& activeLink = 
		getStrategyLink(getLinkOwnerIndex(), getActiveLinkIndex());
	bool autoRestart = !activeLink.autoRestarted();
	ChangeAutoRestart redo(*getTriggerChain(), 
						getLinkOwnerIndex(), 
						getActiveLinkIndex(), 
						autoRestart);
	ChangeAutoRestart undo(*getTriggerChain(), 
						getLinkOwnerIndex(), 
						getActiveLinkIndex(), 
						!autoRestart);
	if (redo())
	{
		getUndoManager(getTriggerChain()).addAction(undo, redo);
		getTriggerEditorLogic()->setDataChanged();
	}
}

void TEBaseWorkMode::OnAutoRestart(TriggerEditorView* source)
{
	changeAutoRestart4ActiveLink();
	source->Invalidate();
}

void TEBaseWorkMode::OnInsertColumns(TriggerEditorView* source)
{
	InsertColumns redo(1, *getTriggerEditorLogic(), 
						*getTriggerChain(), 
						getBasePoint(),
						getTriggerEditorLogic()->getGrid());
	if (redo()) 
	{
		DeleteColumns undo(1, *getTriggerEditorLogic(), 
						*getTriggerChain(), 
						getBasePoint(),
						getTriggerEditorLogic()->getGrid());
		getUndoManager(getTriggerChain()).addAction(undo, redo);
		getTriggerEditorLogic()->setDataChanged();
		source->Invalidate();
	}
}

void TEBaseWorkMode::OnDeleteColumn(TriggerEditorView* source)
{
	DeleteColumns redo(1, *getTriggerEditorLogic(), 
					*getTriggerChain(), 
					getBasePoint(),
					getTriggerEditorLogic()->getGrid());
	if (redo())
	{
		InsertColumns undo(1, *getTriggerEditorLogic(), 
							*getTriggerChain(), 
							getBasePoint(),
							getTriggerEditorLogic()->getGrid());
		getUndoManager(getTriggerChain()).addAction(undo, redo);
		getTriggerEditorLogic()->setDataChanged();
		source->Invalidate();
	}
}

void TEBaseWorkMode::OnInsertRows(TriggerEditorView* source)
{
	InsertRows redo(1, *getTriggerEditorLogic(), 
						*getTriggerChain(), 
						getBasePoint(),
						getTriggerEditorLogic()->getGrid());
	if (redo())
	{
		DeleteRows undo(1, *getTriggerEditorLogic(), 
						*getTriggerChain(), 
						getBasePoint(),
						getTriggerEditorLogic()->getGrid());
		getUndoManager(getTriggerChain()).addAction(undo, redo);
		getTriggerEditorLogic()->setDataChanged();
		source->Invalidate();
	}
}

void TEBaseWorkMode::OnDeleteRow(TriggerEditorView* source)
{
	DeleteRows redo(1, *getTriggerEditorLogic(), 
					*getTriggerChain(), 
					getBasePoint(),
					getTriggerEditorLogic()->getGrid());
	if (redo())
	{
		InsertRows undo(1, *getTriggerEditorLogic(), 
							*getTriggerChain(), 
							getBasePoint(),
							getTriggerEditorLogic()->getGrid());
		getUndoManager(getTriggerChain()).addAction(undo, redo);
		getTriggerEditorLogic()->setDataChanged();
		source->Invalidate();
	}
}

void TEBaseWorkMode::OnTEHiliteLinks(TriggerEditorView* source)
{
	if (selectionManager_.getLast() == getHilitedLinksTriggerIndex())
		setHilitedLinksTriggerIndex(-1);
	else
		setHilitedLinksTriggerIndex(selectionManager_.getLast());
	if (selectionManager_.getCount() > 1)
	{
		int const lastSelected = selectionManager_.getLast();
		selectionManager_.deselectAll();
		selectionManager_.select(lastSelected);
	}
	source->Invalidate();
}

BOOL TEBaseWorkMode::OnCommand(TriggerEditorView* source, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam)) {
	case IDC_TE_LINK_DELETE:
		OnTeLinkDelete(source);
		break;
	case IDC_TE_TGR_ELE_DELETE:
		OnTEDeleteTrigger(source);
		break;
	case IDC_TE_MAKE_ACTIVE:
		OnTeMakeActive(source);
		break;
	case IDC_TE_LINK_TYPE0:
	case IDC_TE_LINK_TYPE1:
	case IDC_TE_LINK_TYPE2:
	case IDC_TE_LINK_TYPE3:
	case IDC_TE_LINK_TYPE4:
	case IDC_TE_LINK_TYPE5:
	case IDC_TE_LINK_TYPE6:
	case IDC_TE_LINK_TYPE7:
	case IDC_TE_LINK_TYPE8:
	case IDC_TE_LINK_TYPE9:
		OnTeLinkType(LOWORD(wParam), source);
		break;
	case IDC_AUTO_RESTART:
		OnAutoRestart(source);
		break;
	case IDC_INSERT_COLUMNS:
		OnInsertColumns(source);
		break;
	case IDC_INSERT_ROWS:
		OnInsertRows(source);
		break;
	case IDC_DELETE_COLUMN:
		OnDeleteColumn(source);
		break;
	case IDC_DELETE_ROW:
		OnDeleteRow(source);
		break;
	case IDC_TE_HILITE_LINKS:
		OnTEHiliteLinks(source);
		break;
	case ID_EDIT_UNDO:
		OnEditUndo(source);
		break;
	case ID_EDIT_REDO:
		OnEditRedo(source);
		break;
	case IDC_TE_RENAME:
		onTERename(source);
		break;
	case ID_TE_COLOR:
		onTEColor(source);
		break;
	case ID_EDIT_CUT:
		onEditCut(source);
		break;
	case ID_EDIT_COPY:
		onEditCopy(source);
		break;
	case ID_EDIT_PASTE:
		onEditPaste(source);
		break;
	case ID_EDIT_SELECT_ALL:
		onEditSelectAll(source);
		break;
	default:
		return Base::OnCommand(source, wParam, lParam);
	}
	return TRUE;
}

void TEBaseWorkMode::OnEditUndo(TriggerEditorView* source)
{
	TriggerChain* const chain = getTriggerChain();
	if (getUndoManager(chain).canUNDO())
	{
		getSelectionManager().deselectAll();
		getUndoManager(chain).undo();
		chain->buildLinks();
		source->Invalidate();
	}
}

void TEBaseWorkMode::OnEditRedo(TriggerEditorView* source)
{
	TriggerChain* const chain = getTriggerChain();
	if (getUndoManager(chain).canREDO())
	{
		getSelectionManager().deselectAll();
		getUndoManager(chain).redo();
		chain->buildLinks();
		source->Invalidate();
	}
}
void TEBaseWorkMode::onTERename(TriggerEditorView* source)
{
	int const lastSelected = selectionManager_.getLast();
	if (selectionManager_.getCount() > 1)
	{
		selectionManager_.deselectAll();
		selectionManager_.select(lastSelected);
	}

	Trigger& trigger = getStrategyTrigger(lastSelected);
	std::pair<CString, bool> res = AskUserForName(trigger.name());

	if (!res.second)
		return;

	if (res.first == trigger.name() || res.first.IsEmpty())
		return;

	res.first = getUniqueName(res.first);

	RenameElement redo(*getTriggerChain(), lastSelected, res.first);
	RenameElement undo(*getTriggerChain(), 
						lastSelected, 
						trigger.name());
	if (redo())
	{
		getUndoManager(getTriggerChain()).addAction(undo, redo);
		getTriggerEditorLogic()->setDataChanged();
	}

	source->Invalidate();
}

void TEBaseWorkMode::OnPaint(TriggerEditorView* source, CDC* pdc, CRect const& rcPaint)
{
	if (getTriggerChain())
		drawer_->draw(pdc, source, dataKeeper_.get(), rcPaint);
	if (getActiveLinkIndex() != -1)
	{
		assert(getLinkOwnerIndex() != -1);
		Trigger const& trigger = getStrategyTrigger(getLinkOwnerIndex());
		TriggerLink const& link = getStrategyLink(trigger, getActiveLinkIndex());
		drawer_->drawLinkAnchors(source, *pdc, trigger, link);
	}
}

TEBaseWorkMode::eLinkAnchorTest TEBaseWorkMode::findLinkAnchor(
															TriggerLink const& link,
															const CPoint& p) 
{
	CPoint tag(te_h::get_center(link.parent->boundingRect()));
	tag.Offset(link.parentOffset());
	CRect r(tag,tag);
	r.InflateRect(ANCHOR_SZ, ANCHOR_SZ);
	if (r.PtInRect(p)) 
		return LAT_BEGIN;

	tag = te_h::get_center(link.child->boundingRect());
	tag.Offset(link.childOffset());

	r.SetRect(tag, tag);
	r.InflateRect(ANCHOR_SZ, ANCHOR_SZ);
	if (r.PtInRect(p)) 
		return LAT_END;
	return LAT_NONE;
}

void TEBaseWorkMode::ModifyMenu(CMenu* pmenu, UINT menuResId, TriggerEditorView* source)
{
	if (menuResId == IDR_TE_TRIGGER_ELE) 
	{
		if (selectionManager_.getCount()) {
			if (0 == selectionManager_.getLast())
			{
				menu_h::disableMenuItem(pmenu, IDC_TE_SHOW_CONDITION);
				menu_h::disableMenuItem(pmenu, IDC_TE_PROPETIES);
				menu_h::disableMenuItem(pmenu, IDC_TE_RENAME);
				if (selectionManager_.getCount() == 1)
					menu_h::disableMenuItem(pmenu, IDC_TE_TGR_ELE_DELETE);
			}
			Trigger const& trigger = getStrategyTrigger(selectionManager_.getLast());
			menu_h::removeMenuItemByCommand(pmenu, IDC_TE_MAKE_ACTIVE);
			menu_h::checkMenuItem(pmenu, IDC_TE_HILITE_LINKS, 
				selectionManager_.getLast() == getHilitedLinksTriggerIndex());
		}
	}
	else if (menuResId == IDR_TE_LINK) 
	{
		assert(getActiveLinkIndex() != -1);
		TriggerLink const& link = 
			getStrategyLink(getLinkOwnerIndex(), getActiveLinkIndex());
		pmenu->CheckMenuItem(IDC_AUTO_RESTART, link.autoRestarted());

		MENUITEMINFO mii;
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_BITMAP|MIIM_DATA;
		mii.hbmpItem = HBMMENU_CALLBACK;
		const int count = pmenu->GetMenuItemCount();
		const int offset = 3;
		for(int i = offset; i < count; ++i){
			mii.dwItemData = i-offset;
			SetMenuItemInfo(pmenu->m_hMenu, i, TRUE, &mii);
		}

	}
	else if (menuResId == IDR_BETWEEN_PLOTS_MENU)
	{
		if (betweenPlotsLayoutTestResult_ == BPL_VERT)
		{
			menu_h::disableMenuItem(pmenu, IDC_INSERT_ROWS);
			menu_h::disableMenuItem(pmenu, IDC_DELETE_ROW);
		}
		else if (betweenPlotsLayoutTestResult_ == BPL_HORZ)
		{
			menu_h::disableMenuItem(pmenu, IDC_INSERT_COLUMNS);
			menu_h::disableMenuItem(pmenu, IDC_DELETE_COLUMN);
		}
	}
}
void TEBaseWorkMode::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis)
{
	ASSERT(lpdis->CtlType == ODT_MENU);
	CRect r(lpdis->rcItem);
	r.right = r.left + r.Height();
	::FillRect(lpdis->hDC, &r, drawer_->getBrushByLinkType(lpdis->itemData));

	Base::OnDrawItem(nIDCtl, lpdis);
}

void TEBaseWorkMode::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis)
{
	ASSERT(lpmis->CtlType==ODT_MENU);
	lpmis->itemWidth += lpmis->itemHeight;
	Base::OnMeasureItem(nIDCtl, lpmis);
}

bool TEBaseWorkMode::canPlugLink(int childTriggerIndex, int parentTriggerIndex,
								 TriggerChain const& chain) const
{
	if (childTriggerIndex == parentTriggerIndex)
		return false;
	Trigger const& parentTrigger = chain.triggers[parentTriggerIndex];
	Trigger const& childTrigger = chain.triggers[childTriggerIndex];
	return parentTrigger.isChild(childTrigger);
}

//! Попадает ли точка в ячейки между элементами
bool TEBaseWorkMode::TestForBetweenPlots(TriggerEditorView* source, CPoint p)
{
	TriggerEditorLogic const& logic = *source->getTriggerEditorLogic();
	TEGrid const& grid = logic.getGrid();
	int const w = grid.getFullCellWidth();
	int const h = grid.getFullCellHeight();

	int idx_x = grid.getHorzCellIndex(p.x);
	int idx_y = grid.getVertCellIndex(p.y);

	setBetweenPlotsLayoutTestResult(BPL_NOWERE);

	if (idx_x*w + grid.getCellWidth() < p.x)
		setBetweenPlotsLayoutTestResult(BPL_VERT);

	if (idx_y*h + grid.getCellHeight() < p.y)
	{
		if (betweenPlotsLayoutTestResult_ == BPL_VERT)
			setBetweenPlotsLayoutTestResult(BPL_VERT_HORZ);
		else
			setBetweenPlotsLayoutTestResult(BPL_HORZ);
	}
	return betweenPlotsLayoutTestResult_ != BPL_NOWERE;
}

void TEBaseWorkMode::clearActive(TriggerChain &chain)
{
//	const qdTriggerElementList& l = pc->elements_list();
//	qdTriggerElementList::const_iterator i = l.begin(), e = l.end();
//	for(; i != e; ++i)
//		(*i)->make_active(false);
//	pc->root_element()->make_active(false);
}

void TEBaseWorkMode::activateLink(int linkIndex, int linkOwnerIndex)
{
	MakeLinkActive undo(this, getActiveLinkIndex(), getLinkOwnerIndex());
	MakeLinkActive redo(this, linkIndex, linkOwnerIndex);
	if (redo()) 
		getUndoManager(getTriggerChain()).addAction(undo, redo);
}


bool TEBaseWorkMode::OnCommandUpdateUI(TriggerEditorView *view, CCmdUI* pCmdUI)
{
	switch (pCmdUI->m_nID) 
	{
	case ID_EDIT_UNDO:
		if (getTriggerChain()) 
			pCmdUI->Enable(getUndoManager(getTriggerChain()).canUNDO());
		return true;
	case ID_EDIT_REDO:
		if (getTriggerChain()) 
			pCmdUI->Enable(getUndoManager(getTriggerChain()).canREDO());
		return true;
	case ID_EDIT_CUT:
		onUpdateEditCut(view, pCmdUI);
		return true;
	case ID_EDIT_COPY:
		onUpdateEditCopy(view, pCmdUI);
		return true;
	case ID_EDIT_PASTE:
		onUpdateEditPaste(view, pCmdUI);
		return true;
	case ID_EDIT_SELECT_ALL:
		onUpdateEditSelectAll(view, pCmdUI);
		return true;
	}
	return false;
}


bool TEBaseWorkMode::initTrigger(TriggerEditorView* source, 
								 Trigger* trigger,int actionIndex)
{
	std::pair<CString, bool> res = AskUserForName(NULL);
	if (!res.first.IsEmpty())
		res.first = getUniqueName(res.first);
	trigger->setName(res.first);

	trigger->action = getTriggerEditorLogic()->createAction(actionIndex);
		
	
	triggerInterface().editTrigger(*trigger, source->m_hWnd);
	return true;
}

void TEBaseWorkMode::add2StrategyRect(TriggerChain &chain, 
									  RECT const& rect4Add)
{
	//прямоугольники охватывающий цепочку
	CRect rcStrategy(chain.boundingRect());
	rcStrategy.UnionRect(&rcStrategy, &rect4Add);
	chain.setBoundingRect(rcStrategy);
}

CString TEBaseWorkMode::getUniqueName(CString const& sourceName) const{
	return EnsureTriggerNameOriginality::getUniqueName(*getTriggerChain(), sourceName);
}

void TEBaseWorkMode::drawDraggedTitles(TriggerEditorView* source) const
{
	SelectionManager::const_iterator	i = selectionManager_.begin(),
										e = selectionManager_.end();
	CPoint startMove(getStrategyTrigger(selectionManager_.getLast()).boundingRect().TopLeft());
	for(; i != e; ++i)
	{
		Trigger const& trigger = getStrategyTrigger(*i);
		CPoint drawPoint = trigger.boundingRect().TopLeft();
		drawPoint += getCurrentPoint() - startMove;
		drawer_->drawDragedTitle(source, trigger.displayText(), drawPoint);
	}
}

bool TEBaseWorkMode::moveDraggedTitles(CPoint const& point) const
{
	TriggerEditorLogic const& logic = *getTriggerEditorLogic();
	int lastSelected = selectionManager_.getLast();
	CPoint startMove(getStrategyTrigger(lastSelected).boundingRect().TopLeft());
	CSize offset(point - startMove);
	SelectionManager::const_iterator	i = selectionManager_.begin(),
										e = selectionManager_.end();
	for(; i != e; ++i)
	{
		Trigger const& trigger = getStrategyTrigger(*i);
		CRect const rc = trigger.boundingRect();
		int triggerIndex = logic.findTrigger(*getTriggerChain(), rc.TopLeft() + offset);
		if (triggerIndex != -1&&triggerIndex != lastSelected
			&&!selectionManager_.isSelected(triggerIndex))
			return false;
	}
	return true;
}


void TEBaseWorkMode::moveSelection(TriggerEditorView* source, CPoint const& point)
{
	TEUndoManager::Bunch undoBunch, redoBunch;

	CPoint const& endMove = getCurrentPoint();
	CPoint startMove(getStrategyTrigger(selectionManager_.getLast()).boundingRect().TopLeft());
	SelectionManager::const_iterator i = selectionManager_.begin(),
									 e = selectionManager_.end();
	for(; i != e; ++i)
	{
		Trigger const& trigger = getStrategyTrigger(*i);
		MoveTEElementPos redo(source, *i, *getTriggerChain(), endMove - startMove);
		MoveTEElementPos undo(source, *i, *getTriggerChain(), startMove - endMove);
		if (redo()) 
		{
			getTriggerEditorLogic()->setDataChanged();
			undoBunch.addAction(undo);
			redoBunch.addAction(redo);
		}
	}
	if (!redoBunch.empty())
	{
		undoBunch.reverse();
		getUndoManager(getTriggerChain()).addAction(undoBunch, redoBunch);
	}
}

void TEBaseWorkMode::onTEColor(TriggerEditorView* source)
{
	static COLORREF customColors_[16] = {0};

	if (selectionManager_.begin() == selectionManager_.end())
		return;

	SelectionManager::const_iterator i = selectionManager_.begin(),
									 e = selectionManager_.end();
	Trigger const& trigger = getStrategyTrigger(*i);
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_FULLOPEN;
	if (dlg.DoModal() != IDOK)
		return;

	TEUndoManager::Bunch undoBunch, redoBunch;
	TriggerChain& chain = *getTriggerChain();
	for(; i != e; ++i)
	{
		ChangeColor redo(chain, *i, dlg.GetColor());
		ChangeColor undo(chain, *i, getStrategyTrigger(*i).color());
		if (redo())
		{
			getTriggerEditorLogic()->setDataChanged();
			undoBunch.addAction(undo);
			redoBunch.addAction(redo);
		}
	}

	if (!redoBunch.empty())
	{
		undoBunch.reverse();
		getUndoManager(&chain).addAction(undoBunch, redoBunch);
	}
	source->Invalidate();
}

bool TEBaseWorkMode::copySelectionToClipBuffer()
{
	CopyToClipBoard copier;
	return copier.copy(*getTriggerChain(), 
				TriggerClipBuffer::instance(),
				getSelectionManager());
}

bool TEBaseWorkMode::insertTriggerClipBuffer()
{
	ClipBoardDataInserter inserter(*getTriggerEditorLogic(), *this);
	TriggerChain& chain = *getTriggerChain();
	if (inserter.insert(TriggerClipBuffer::instance(), getUndoManager(&chain)))
	{
		getTriggerEditorLogic()->adjustTriggerChainLayout(chain);
		return true;
	}
	return false;
}

void TEBaseWorkMode::onEditCut(TriggerEditorView* source)
{
	if (copySelectionToClipBuffer())
	{
		deleteSelection();
		getTriggerEditorLogic()->setDataChanged();
		source->Invalidate();
	}
}

void TEBaseWorkMode::onEditCopy(TriggerEditorView* source)
{
	copySelectionToClipBuffer();
}

void TEBaseWorkMode::onEditPaste(TriggerEditorView* source)
{
	insertTriggerClipBuffer();
	getTriggerEditorLogic()->setDataChanged();
	source->Invalidate();
}

void TEBaseWorkMode::onUpdateEditCut(TriggerEditorView* source, CCmdUI* pCmdUI)
{
	pCmdUI->Enable(selectionManager_.getCount() > 0);
}

void TEBaseWorkMode::onUpdateEditCopy(TriggerEditorView* source, CCmdUI* pCmdUI)
{
	pCmdUI->Enable(selectionManager_.getCount() > 0);
}

void TEBaseWorkMode::onUpdateEditPaste(TriggerEditorView* source, CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!TriggerClipBuffer::instance().empty());
}

void TEBaseWorkMode::onEditSelectAll(TriggerEditorView* source)
{
	CWaitCursor wait;
	selectionManager_.deselectAll();
	int const count = getTriggerChain()->triggers.size();
	for(int i = 0; i < count; ++i)
		selectionManager_.select(i);
	source->Invalidate();
}

void TEBaseWorkMode::onUpdateEditSelectAll(TriggerEditorView* source, CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

