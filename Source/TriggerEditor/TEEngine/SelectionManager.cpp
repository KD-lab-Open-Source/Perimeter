// SelectionManager.cpp: implementation of the SelectionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SelectionManager.h"
#include <algorithm>
//#include "Game/GameInclude.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SelectionManager::SelectionManager()
{

}

SelectionManager::~SelectionManager()
{

}

void SelectionManager::setTriggerChain(TriggerChain* ptrTriggerChain)
{
	selected_.clear();
	ptrTriggerChain_ = ptrTriggerChain;
	if (ptrTriggerChain)
		selected_.reserve(ptrTriggerChain->triggers.size());
}

TriggerChain* SelectionManager::getTriggerChain() const{
	return ptrTriggerChain_;
}

size_t SelectionManager::getCount() const{
	return selected_.size();
}

bool SelectionManager::empty() const{
	return selected_.empty();
}
Trigger& SelectionManager::getTrigger(int index) const {
	return getTriggerChain()->triggers[index];
}

bool SelectionManager::isSelected(int index) const{
	return getTrigger(index).selected();
}

void SelectionManager::select(int index){
	assert(getTriggerChain());
	select(index, true);
}

void SelectionManager::reselect(int index){
	deselect(index);
	select(index);
}

void SelectionManager::deselect(int index){
	assert(getTriggerChain());
	select(index, false);
}

void SelectionManager::deselectAll()
{
	for(; !selected_.empty(); )
		deselect(selected_.front());
}

void SelectionManager::clear(){
	assert(getTriggerChain());
	selected_.clear();
}

int SelectionManager::getLast() const{
	if (selected_.empty())
		return -1;
	return selected_.back();
}

void SelectionManager::select(int index, bool select){
	if (select)
	{
		if (!getTrigger(index).selected())
			selected_.push_back(index);
	}
	else
	{
		selected_.erase(
			std::remove(selected_.begin(), 
						selected_.end(),
						index), 
			selected_.end());
	}
	getTrigger(index).setSelected(select);
}

SelectionManager::const_iterator SelectionManager::begin() const{
	return selected_.begin();
}
SelectionManager::const_iterator SelectionManager::end() const{
	return selected_.end();
}
