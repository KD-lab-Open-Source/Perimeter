#include "stdafx.h"
#include "resource.h"
#include "triggereditorlogic.h"
//#include "Game/GameInclude.h"
#include "TriggerEditorLogic.h"
#include "TriggerEditorView.h"
#include "TriggerEditor.h"

#include "TEEngine/TEUtils/FindTrigger.h"
#include "TEEngine/TEUtils/FindLinkInTriggerByChild.h"
#include "TEEngine/TEUtils/FindLinkByPoint.h"
#include "TEEngine/TEUtils/BoundingRectCalculator.h"

#include "TriggerExport.h"

#include "TEEngine/WorkModes/TEBaseWorkMode.h"
#include "TEEngine/Drawers/TEBaseWorkModeDrawer.h"

#include "TEEngine/WorkModes/TEProfilerWorkMode.h"
#include "TEEngine/Drawers/TEStatusColoredEleDrawer.h"

#include "TEEngine/te_helpers.h"
#include "TEEngine/Profiler/ITriggerProfList.h"

#include "TEEngine/Actions/AddElement.h"
#include "TEEngine/Actions/DeleteElement.h"

#include "TEEngine/Actions/AddLink.h"
#include "TEEngine/Actions/DeleteLink.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class TriggerEditorLogic::BoundingRect{
	CRect r_;
public:
	BoundingRect():r_(0, 0, 0, 0){}
	CRect& getRect(){
		return r_;
	}
	operator CRect const&() const{
		return r_;
	}
	void operator()(Trigger const& trigger) {
		r_.UnionRect(&r_, &trigger.boundingRect());
	}
};

TriggerEditorLogic::TriggerEditorLogic(TriggerEditor* triggerEditor):
  triggerEditorView_(NULL)
, currentWorkMode_(NULL)
, triggerChain_(NULL)
, grid_(256, 32, 32, 32)
, triggerEditor_(triggerEditor)
{
	baseWorkMode_.reset(new TEBaseWorkMode(this));
	baseWorkModeDrawer_.reset(new TEBaseWorkModeDrawer(baseWorkMode_.get()));
	baseWorkMode_->setDrawer(baseWorkModeDrawer_.get());

	profilerWorkMode_.reset(new TEProfilerWorkMode(this));
	profilerWorkModeDrawer_.reset(new TEStatusColoredEleDrawer);
	profilerWorkMode_->setDrawer(profilerWorkModeDrawer_.get());

	setCurrentWorkMode(baseWorkMode_.get());
}

TriggerEditorLogic::~TriggerEditorLogic(void)
{
}

TriggerEditorView* TriggerEditorLogic::getTriggerEditorView() const{
	return triggerEditorView_;
}

void TriggerEditorLogic::setTriggerEditorView(TriggerEditorView* ptev){
	triggerEditorView_ = ptev;
}

void TriggerEditorLogic::setTriggerProfList(ITriggerProfList* ptrTriggerProfList)
{
	profilerWorkMode_->setTriggerProfList(ptrTriggerProfList);
}

ActionPtr TriggerEditorLogic::createAction(int index) const{
	return triggerInterface().createAction(index);
}

void TriggerEditorLogic::setTriggerChain(TriggerChain* chain){
	//! Важно, чтобы эта функция была вызывана, когда окно уже имеет ненулевые
	//! размеры. Важно для adjustTriggerChainLayout()
	triggerChain_ = chain;
	if (getCurrentWorkMode())
	{
		getCurrentWorkMode()->OnEnd();
		getCurrentWorkMode()->OnStart();
	}
	adjustTriggerChainLayout(*triggerChain_);
	if(chain->viewRect().valid())
		getTriggerEditorView()->ensureVisibleRect(chain->viewRect());
}

TriggerChain* TriggerEditorLogic::getTriggerChain() const{
	return triggerChain_;
}

void TriggerEditorLogic::setDataChanged(bool bValue /* = true */){
	triggerEditor_->setDataChanged(bValue);
}

bool TriggerEditorLogic::isDataSaved() const{
	return triggerEditor_->isDataSaved();
}

void TriggerEditorLogic::setCurrentWorkMode(ITEWorkMode* currentWorkMode){
	if (currentWorkMode_)
		currentWorkMode_->OnEnd();
	currentWorkMode_ = currentWorkMode;
	if (currentWorkMode_)
		currentWorkMode_->OnStart();
}

ITEWorkMode* TriggerEditorLogic::getCurrentWorkMode() const{
	assert(currentWorkMode_);
	return currentWorkMode_;
}

TEGrid const& TriggerEditorLogic::getGrid() const{
	return grid_;
}
TEGrid & TriggerEditorLogic::getGrid(){
	return grid_;
}

BOOL TriggerEditorLogic::onCommand(TriggerEditorView* source, 
								   WPARAM wParam, 
								   LPARAM lParam)
{
	switch(LOWORD(wParam)) {
	case ID_VIEW_PROFILER:
		onViewProfiler(source);
		return TRUE;
	case ID_TE_ACTIVATE_PROFILER:
		onTEActivateProfiler(source);
		return TRUE;
	}
	if (getCurrentWorkMode())
		return getCurrentWorkMode()->OnCommand(source, wParam, lParam);
	return FALSE;
}

BOOL TriggerEditorLogic::onUpdateCommandUI(TriggerEditorView* source, CCmdUI* pCmdUI)
{
	switch (pCmdUI->m_nID) 
	{
	case ID_VIEW_PROFILER:
		OnUpdateViewProfirler(source, pCmdUI);
		return TRUE;
	case ID_TE_ACTIVATE_PROFILER:
		OnUpdateTEActivateProfiler(source, pCmdUI);
		return TRUE;
	}
	if (getCurrentWorkMode())
		return getCurrentWorkMode()->OnCommandUpdateUI(source, pCmdUI);
	return FALSE;
}

void TriggerEditorLogic::onRButtonDown(TriggerEditorView* source, 
									   UINT nFlags, CPoint point) 
{
	if (getCurrentWorkMode())
		getCurrentWorkMode()->OnRButtonDown(source, nFlags, point);
}

void TriggerEditorLogic::onRButtonUp(TriggerEditorView* source, 
									 UINT nFlags, CPoint point) 
{
	if (getCurrentWorkMode())
		getCurrentWorkMode()->OnRButtonUp(source, nFlags, point);
}

void TriggerEditorLogic::onLButtonDown(TriggerEditorView* source, 
										UINT nFlags, CPoint point)
{
	if (getCurrentWorkMode())
		getCurrentWorkMode()->OnLButtonDown(source, nFlags, point);
}

void TriggerEditorLogic::onLButtonUp(TriggerEditorView* source, 
									 UINT nFlags, CPoint point)
{
	if (getCurrentWorkMode())
		getCurrentWorkMode()->OnLButtonUp(source, nFlags, point);
}

void TriggerEditorLogic::onLButtonDblClk(TriggerEditorView* source, 
										 UINT nFlags, CPoint point)
{
	if (getCurrentWorkMode())
		getCurrentWorkMode()->OnLButtonDblClk(source, nFlags, point);
}

void TriggerEditorLogic::onMouseMove(TriggerEditorView* source, 
									 UINT nFlags, CPoint point)
{
	if (getCurrentWorkMode())
		getCurrentWorkMode()->OnMouseMove(source, nFlags, point);
}

BOOL TriggerEditorLogic::onSetCursor(TriggerEditorView* source,
									 CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (getCurrentWorkMode())
		return getCurrentWorkMode()->OnSetCursor(source, pWnd, nHitTest, message);
	return FALSE;
}

void TriggerEditorLogic::onKeyDown(TriggerEditorView* source, 
								   UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (getCurrentWorkMode())
		getCurrentWorkMode()->OnKeyDown(source, nChar, nRepCnt, nFlags);
}
void TriggerEditorLogic::onKeyUp(TriggerEditorView* source, 
								 UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (getCurrentWorkMode())
		getCurrentWorkMode()->OnKeyUp(source, nChar, nRepCnt, nFlags);
}

BOOL TriggerEditorLogic::onDropData(TriggerEditorView* source, void* pData, 
									DROPEFFECT dropEffect, CPoint const& point)
{
	if (getCurrentWorkMode())
		return getCurrentWorkMode()->OnDropData(source, pData, point);
	return FALSE;
}

void TriggerEditorLogic::OnShowWindow(TriggerEditorView* source, 
									  BOOL bShow, 
									  UINT nStatus)
{
	if (getCurrentWorkMode())
		getCurrentWorkMode()->OnShowWindow(source, bShow, nStatus);
}

DROPEFFECT TriggerEditorLogic::onDragOver(TriggerEditorView* source, void* pData, 
										  DWORD dwKeyState, CPoint const& point)
{
	if (getCurrentWorkMode()&&
		getCurrentWorkMode()->CanDrop(source, pData, point))
		return DROPEFFECT_COPY;
	return DROPEFFECT_NONE;
}

int TriggerEditorLogic::findTrigger(TriggerChain const& chain, 
									CPoint const &point) const
{
	return FindTrigger::run(chain, point);
}

int TriggerEditorLogic::findLinkByChild(Trigger const& trigger, 
										Trigger const& child) const
{
	OutcomingLinksList const& lst = trigger.outcomingLinks();
	OutcomingLinksList::const_iterator res = 
		std::find_if(lst.begin(), lst.end(), FindLinkInTriggerByChild(&child));
	if (res == lst.end()) 
		return -1;
	return std::distance(lst.begin(), res);
}

std::pair<int, int> const TriggerEditorLogic::findLink(
										TriggerChain const& chain, 
										CPoint const& p) const
{
	return FindLinkByPoint::run(chain, p);
}

void TriggerEditorLogic::remakeStrategyRect(TriggerChain const& chain)
{
	TriggerList const& lst = chain.triggers;
	typedef TriggerList::const_iterator Iterator;
	
	BoundingRect br = std::for_each(Iterator(lst.begin()), 
									Iterator(lst.end()), BoundingRect());
	const_cast<TriggerChain&>(chain).setBoundingRect(br);
}

void TriggerEditorLogic::updateChainViewRect(TriggerChain& chain) const
{
	CRect rcClient;
	getTriggerEditorView()->GetClientRect(rcClient);
	getTriggerEditorView()->scr2Log(rcClient);
	chain.setViewRect(rcClient);
}

void TriggerEditorLogic::updateCellIndexes(Trigger& trigger, CPoint const& leftTop) const
{
	TEGrid const& grid = getGrid();
	trigger.setCellIndex(grid.getHorzCellIndex(leftTop.x), grid.getVertCellIndex(leftTop.y));
}

void TriggerEditorLogic::adjustTriggerChainLayout(
										TriggerChain& chain) const{
	assert (getTriggerChain());

	TriggerList& triggers = chain.triggers;

	if (!triggers.empty())
	{
		adjustTriggersLayout(triggers);
		adjustLinksLayout(triggers);
		remakeStrategyRect(chain);
	}
	getTriggerEditorView()->updateViewWorkArea();
}

void TriggerEditorLogic::adjustTriggersLayout(
						TriggerList& triggers) const
{

	// Проверяем всели триггера имеют неопределенное полжение
	// Да все, тогда расскладываем их по порядку относительно видимой области
	// Нет тогда распологаем новые под всеми столбиком

	switch(hasTriggersInvalidCellIndexes(triggers)) {
	case TCIT_NONE:
		break;
	case TCIT_ALL_TRIGGERS:
		adjustTriggersCellsForAll(triggers);
		break;
	case TCIT_SOME_TRIGGERS:
		adjustTriggersCellsForNew(triggers);
		break;
	}

	TriggerList::iterator itr = triggers.begin(), end = triggers.end();
	std::for_each(triggers.begin(), triggers.end(), BoundingRectCalculator(getGrid()));
}

TriggerEditorLogic::TriggerCellIndexesTest 
	TriggerEditorLogic::hasTriggersInvalidCellIndexes(
							TriggerList const& triggers) const
{
	assert(!triggers.empty());
	TriggerList::const_iterator itr = triggers.begin(), end = triggers.end();
	bool isPrevValid = itr->cellIndex().valid();
	for(++itr;itr != end; ++itr){
		if(itr->cellIndex().valid()){
			if (!isPrevValid)
				return TCIT_SOME_TRIGGERS;
		}
		else{
			if (isPrevValid)
				return TCIT_SOME_TRIGGERS;
		}
	}
	return (isPrevValid)? TCIT_NONE:TCIT_ALL_TRIGGERS;
}

void TriggerEditorLogic::adjustTriggersCellsForNew(
						TriggerList& triggers) const
{
	// ноходим левую нижнюю занятую ячейку

	CPoint leftBottomCell(INT_MAX, INT_MIN);
	TriggerList::iterator itr = triggers.begin(), end = triggers.end();
	for(;itr != end; ++itr)
	{
		Trigger & trigger = *itr;
		if (trigger.cellIndex().valid())
		{
			if (leftBottomCell.x > trigger.cellIndex().x)
				leftBottomCell.x = trigger.cellIndex().x;
		}
	}
	itr = triggers.begin();
	end = triggers.end();
	for(;itr != end; ++itr)
	{
		Trigger & trigger = *itr;
		int const horzIndex = trigger.cellIndex().x;
		if (horzIndex == leftBottomCell.x)
		{
			if (leftBottomCell.y < trigger.cellIndex().y)
				leftBottomCell.y = trigger.cellIndex().y;
		}
	}
	//! теперь укладываем столбиком те, что неприкаянны
	itr = triggers.begin();
	end = triggers.end();
	for(;itr != end; ++itr)
	{
		Trigger & trigger = *itr;
		if (!trigger.cellIndex().valid())
		{
			++leftBottomCell.y;
			trigger.setCellIndex(leftBottomCell);
		}
	}
}

void TriggerEditorLogic::adjustTriggersCellsForAll(
						TriggerList& triggers) const
{
	TEGrid const& grid = getGrid();
	RECT const visibleCells = getVisibleCells();
	int horzIndex = visibleCells.left;
	int vertIndex = visibleCells.top;

	TriggerList::iterator itr = triggers.begin(), end = triggers.end();
	for(;itr != end; ++itr)
	{
		Trigger & trigger = *itr;
		trigger.setCellIndex(horzIndex, vertIndex);
		++horzIndex;
		if (horzIndex >= visibleCells.right)
		{
			horzIndex = visibleCells.left;
			++vertIndex;
		}
	}
}
void TriggerEditorLogic::adjustLinksLayout(
							TriggerList& triggers) const
{
	TriggerList::iterator	i = triggers.begin(), 
												e = triggers.end();
	for(;i != e; ++i){
		Trigger & trigger =  *i;
		CRect const br1 = trigger.boundingRect();
		CPoint const ptFrom = br1.CenterPoint();
		OutcomingLinksList& links = trigger.outcomingLinks();
		OutcomingLinksList::iterator j = links.begin(), k = links.end();
		for(; j != k; ++j){
			TriggerLink& link = *j;
			CRect const br2 = link.child->boundingRect();
			CPoint const ptTo = br2.CenterPoint();
			if(!link.parentOffset().valid() || !link.childOffset().valid()){
				link.setParentOffset(te_h::getLinkOffset(ptFrom, ptTo, br1));
				link.setChildOffset(te_h::getLinkOffset(ptTo, ptFrom, br2));
			}
		}
	}
}

//! Возвращает номера левой верхней и правой нижней видимых ячеек
RECT const TriggerEditorLogic::getVisibleCells() const
{
	CRect rcClient;
	getTriggerEditorView()->GetClientRect(rcClient);
	getTriggerEditorView()->scr2Log(rcClient);
	TEGrid const& grid = getGrid();
	RECT result;
	result.left		= grid.getHorzCellIndex(rcClient.left);
	result.top		= grid.getVertCellIndex(rcClient.top);
	result.right	= grid.getHorzCellIndex(rcClient.right);
	result.bottom	= grid.getVertCellIndex(rcClient.bottom);
	//! убеждаемся, что левый и верхний край 
	//! ячейки не выходят за края видимой области
	if (rcClient.left > grid.getCellLeftByIndex(result.left))
		result.left += 1;
	if (rcClient.top > grid.getCellLeftByIndex(result.top))
		result.top += 1;
	return result;
}

void TriggerEditorLogic::onViewProfiler(TriggerEditorView* source)
{
	if (ITriggerProfList* ptrTriggerProfList = profilerWorkMode_->getTriggerProfList())
	{
		if (ptrTriggerProfList->isVisible())
			ptrTriggerProfList->hide();
		else
			ptrTriggerProfList->show();
	}
}

void TriggerEditorLogic::OnUpdateViewProfirler(TriggerEditorView* source, CCmdUI* pCmdUI)
{
	assert(profilerWorkMode_);
	if (getCurrentWorkMode() == profilerWorkMode_.get())
	{
		ITriggerProfList* ptrTriggerProfList = profilerWorkMode_->getTriggerProfList();
		if (ptrTriggerProfList)
		{
			pCmdUI->Enable(TRUE);
			pCmdUI->SetCheck(ptrTriggerProfList->isVisible());
			return;
		}
	}
	pCmdUI->SetCheck(FALSE);
	pCmdUI->Enable(FALSE);
}

void TriggerEditorLogic::onTEActivateProfiler(TriggerEditorView* source)
{
	if (getCurrentWorkMode() == profilerWorkMode_.get())
		setCurrentWorkMode(baseWorkMode_.get());
	else
		setCurrentWorkMode(profilerWorkMode_.get());
}

void TriggerEditorLogic::OnUpdateTEActivateProfiler(TriggerEditorView* source, 
													CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(getCurrentWorkMode() == profilerWorkMode_.get());
	if (ITriggerProfList* ptrTriggerProfList = profilerWorkMode_->getTriggerProfList())
	{
		pCmdUI->Enable(ptrTriggerProfList->canBeActivated());
	}
	else
		pCmdUI->Enable(FALSE);
}

void TriggerEditorLogic::deleteTrigger(TriggerChain &chain, 
									int triggerIndex, 
									TEUndoManager::Bunch& undoBunch,
									TEUndoManager::Bunch& redoBunch)
{
	TEUndoManager::Bunch uBunch, rBunch;
	removeLinks(chain, triggerIndex, uBunch, rBunch);
	DeleteElement redo(chain, triggerIndex);
	Trigger const& trigger = 
		chain.triggers[triggerIndex];
	AddElement undo(getGrid(), trigger.cellIndex(), 
					chain, trigger, triggerIndex);
	if (redo())
	{
		rBunch.addAction(redo);
		uBunch.addAction(undo);
		uBunch.reverse();

		redoBunch.addAction(rBunch);
		undoBunch.addAction(uBunch);
	}
}

void TriggerEditorLogic::removeLinks(TriggerChain &chain, 
								 int triggerIndex, 
								 TEUndoManager::Bunch& undoBunch, 
								 TEUndoManager::Bunch& redoBunch)
{
	removeOutcomingLinks(chain, triggerIndex, undoBunch, redoBunch);
	removeIncomingLinks(chain, triggerIndex, undoBunch, redoBunch);
}

void TriggerEditorLogic::removeIncomingLinks(TriggerChain &chain, 
										 int triggerIndex, 
										 TEUndoManager::Bunch& undoBunch,
										 TEUndoManager::Bunch& redoBunch)
{
	TEUndoManager::Action aUndo, aRedo;
	Trigger &trigger = chain.triggers[triggerIndex];
	IncomingLinksList& lst = trigger.incomingLinks();
	while(!lst.empty())
	{
		TriggerLink const& link = *lst.front();
		int const parentElementIndex = chain.triggerIndex(*link.parent);
		if (removeLink(chain, link, parentElementIndex, triggerIndex, aUndo, aRedo))
		{
			undoBunch.addAction(aUndo);
			redoBunch.addAction(aRedo);
			aUndo = NULL;
			aRedo = NULL;
		}
	}

}

void TriggerEditorLogic::removeOutcomingLinks(TriggerChain& chain, 
										  int triggerIndex, 
											TEUndoManager::Bunch& undoBunch,
											TEUndoManager::Bunch& redoBunch)
{
	TEUndoManager::Action aUndo, aRedo;
	Trigger &trigger = chain.triggers[triggerIndex];
	OutcomingLinksList& lst = trigger.outcomingLinks();
	while(!lst.empty())
	{
		TriggerLink const& link = lst.front();
		int const childElementIndex = chain.triggerIndex(*link.child);
		if (removeLink(chain, link, triggerIndex, childElementIndex, aUndo, aRedo))
		{
			undoBunch.addAction(aUndo);
			redoBunch.addAction(aRedo);
			aUndo = NULL;
			aRedo = NULL;
		}
	}
}

bool TriggerEditorLogic::removeLink(TriggerChain& chain, 
								TriggerLink const& link,
								int linkOwnerIndex, 
								int childElementIndex,
								TEUndoManager::Action& aUndo, 
								TEUndoManager::Action& aRedo)
{
	DeleteLink redo(chain, linkOwnerIndex, childElementIndex);
	AddLink undo(chain, 
				linkOwnerIndex, 
				childElementIndex,
				link.childOffset(),
				link.parentOffset(),
				link.getType(),
				link.autoRestarted()); 
	if (redo())
	{
		aUndo = undo;
		aRedo = redo;
		return true;
	}
	return false;
}
