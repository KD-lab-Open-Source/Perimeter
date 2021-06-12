#include "StdAfx.h"
#include "GameInclude.h"
#include <new.h>
#include "EditArchive.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//--------------------------------------------
SaveAdapter::SaveAdapter()
{
}

SaveAdapter::~SaveAdapter()
{
}

const char* SaveAdapter::actionComboList() const 
{  
	return EditClassDescriptor<Action>::comboListAlt().c_str();
}
const char* SaveAdapter::conditionComboList() const 
{  
	return EditClassDescriptor<Condition>::comboListAlt().c_str();
}

ActionPtr SaveAdapter::createAction(int typeIndex) const
{
	ActionPtr action;
	EditIArchive(EditClassDescriptor<Action>::treeNodeFunc(typeIndex)) >> WRAP_NAME(action, 0);
	return action;
}

ConditionPtr SaveAdapter::createCondition(int typeIndex) const
{
	ConditionPtr condition;
	EditIArchive(EditClassDescriptor<Condition>::treeNodeFunc(typeIndex)) >> WRAP_NAME(condition, 0);
	return condition;
}

//-----------------------------------------------
SyncroTimer  global_time;

const char* setStrategyLinkTriggerName(HWND hwnd, const TreeNode* treeNode)
{
	return 0;
}

const char* editUnitAttributeIDs(HWND hwnd, const TreeNode* treeNode)
{
	return 0;
}
const char* editModelNameDialog(HWND hwnd, const TreeNode* treeNode)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
StrategyLink* PlayerStrategy::addLink(int parentIndex, int childIndex, int type) 
{
	Trigger& parent = const_cast<Trigger&>(triggers[parentIndex]);
	Trigger& child = const_cast<Trigger&>(triggers[childIndex]);

	parent.outcomingLinks.push_back(StrategyLink());
	StrategyLink& newLink = parent.outcomingLinks.back();
	newLink.triggerName = child.name;
	newLink.type = static_cast<StrategyLink::Type>(type);
	newLink.parent = &parent;
	newLink.child = &child;

	buildLinks();
	return &newLink;
}

bool PlayerStrategy::removeLink(int parentIndex, int childIndex) 
{
	Trigger& parent = const_cast<Trigger&>(triggers[parentIndex]);
	Trigger& child = const_cast<Trigger&>(triggers[childIndex]);
	if (parent.removeLinkByChild(&child))
	{
		//! перед удалением исправить removeLinkByChild(&child))
		buildLinks();
		return true;
	}
	return false;
}

int PlayerStrategy::addTriggersBunch(TriggerList const& bunch)
{
	triggers.insert(triggers.end(), bunch.begin(), bunch.end());
	buildLinks();
	return bunch.size();
}

Trigger* PlayerStrategy::addTrigger(Trigger const& trigger)
{
	triggers.push_back(trigger);
	// перестройку связей запрещаем, если добавляем элемент со связями.
	// Пр.: из буффера обмена
	//if (buildLinks)
		this->buildLinks();	//! в случае, если произошла реалокация - это необходимо

	return &triggers.back();
}

Trigger* PlayerStrategy::insertTrigger(int pos, Trigger const& trigger){
	vector<Trigger>::iterator itr = triggers.insert(triggers.begin() + pos, trigger);
	if (itr == triggers.end())
		return NULL;
	buildLinks();
	return &*itr;
}
void PlayerStrategy::removeTrigger(int triggerIndex)
{
	TriggerList::iterator forErase = triggers.begin();
	std::advance(forErase, triggerIndex);
	triggers.erase(forErase);
	buildLinks();
}

//////////////////////////////////////////////////////////////////////////
bool Trigger::removeLinkByChild(Trigger * child)
{
	typedef OutcomingLinksList::iterator Iterator;
	Iterator i = outcomingLinks.begin(), e = outcomingLinks.end();
	for(; i != e; ++i)
	{
		StrategyLink& link = *i;
		if (link.child == child)
		{
/*!
	раскоментировать, если 
	из remove_link удален вызов метода initialize()

//			typedef IncomingLinksList::iterator Iterator;
//			Iterator res = find(child->incomingLinks_.begin(), 
//								child->incomingLinks_.end(), &link);
//			assert (res != child->incomingLinks_.end());
//			child->incomingLinks_.erase(res);

*/

			outcomingLinks.erase(i);
			return true;
		}
	}
	return false;
}

