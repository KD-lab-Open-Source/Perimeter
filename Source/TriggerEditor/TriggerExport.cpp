#include "StdAfx.h"
#include "TriggerExport.h"

//----------------------------------------------------------
bool ConditionSwitcher::check(AIPlayer& aiPlayer) 
{ 
	if(conditions.empty())
		return true;

	if(conditions.size() == 1)
		return conditions.front().check(aiPlayer);

	if(type == AND){
		vector<ConditionNode>::iterator ci;
		FOR_EACH(conditions, ci)
			if(!ci->check(aiPlayer)) 
				return false;
		return true;
	}
	else{
		vector<ConditionNode>::iterator ci;
		FOR_EACH(conditions, ci)
			if(ci->check(aiPlayer)) 
				return true;
		return false;
	}
}

void ConditionSwitcher::checkEvent(AIPlayer& aiPlayer, const Event& event) 
{ 
	vector<ConditionNode>::iterator ci;
	FOR_EACH(conditions, ci)
		if(ci->condition)
			ci->condition->checkEvent(aiPlayer, event);
}

void ConditionSwitcher::clear() 
{ 
	vector<ConditionNode>::iterator ci;
	FOR_EACH(conditions, ci)
		if(ci->condition)
			ci->condition->clear();
}

void ConditionSwitcher::writeInfo(XBuffer& buffer, string offset) const
{
	if(conditions.empty())
		return;
	if(conditions.size() == 1){
		conditions.front().writeInfo(buffer, offset);
		return;
	}
	buffer < (type == AND ? "И" : "ИЛИ") < " = " < (state_ ? "1" : "0") < "\r\n";
	offset += "    ";
	vector<ConditionNode>::const_iterator ci;
	FOR_EACH(conditions, ci)
		ci->writeInfo(buffer, offset);
}

void ConditionNode::writeInfo(XBuffer& buffer, string offset) const
{
	if(!condition){
		buffer < "Нулевое условие, нужно удалить!!!\r\n";
		return;
	}

	buffer < offset.c_str();
	bool state = condition->state();
	if(type == INVERTED){
		buffer < "НЕ ";
		state = !state;
	}

	if(dynamic_cast<const ConditionSwitcher*>(condition())){
		condition->writeInfo(buffer, offset);
	}
	else{
		buffer < triggerInterface().conditionName(*condition) < " = " < (state ? "1" : "0") < "\r\n";
	}
}

//----------------------------------------------------------
TriggerLink::TriggerLink() 
{
	colorType_ = STRATEGY_RED;
	autoRestarted_ = THIN;
	active_ = false;
	child = parent = 0;
}

//----------------------------------------------------------
Trigger::Trigger() 
{
	condition = 0;
	action = 0;
	state_ = SLEEPING;
	executionCounter_ = 0;
	internalColor_ = 0;
	
	initialize();
}

void Trigger::initialize()
{
//	state_ = SLEEPING;
//	executionCounter_ = 0;
	selected_ = false;
	if(!condition)
		condition = new ConditionSwitcher;
}

void Trigger::setName(const char* name)
{
	IncomingLinksList::iterator i;
	FOR_EACH(incomingLinks_, i)
		(*i)->setTriggerName(name);
	name_ = name;
}

bool Trigger::active() const 
{ 
	if(state_ == CHECKING || state_ == WORKING)
		return true;
	
	vector<TriggerLink*>::const_iterator li;
	FOR_EACH(incomingLinks_, li)
		if((*li)->active())
			return true;

	return false;
}

const char* Trigger::displayText() const
{
	static XBuffer buffer(1024, 1);
	buffer.init();
	buffer < name();
	if(action)
		buffer < " (" < action->name() < ")";
	return buffer;
}

const char* Trigger::debugDisplayText() const
{
	static XBuffer buffer(1024, 1);
	buffer.init();
	buffer < "[" <= executionCounter_ < "] " < name();
	if(action)
		buffer < " (" < action->name() < ")";
	if(condition){
		buffer < "\r\n";
		condition->writeInfo(buffer, "");
		buffer -= 2;
		buffer < "";
	}
	return buffer;
}

bool Trigger::removeLinkByChild(Trigger * child)
{
	typedef OutcomingLinksList::iterator Iterator;
	Iterator i = outcomingLinks().begin(), e = outcomingLinks().end();
	for(; i != e; ++i)
	{
		TriggerLink& link = *i;
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

			outcomingLinks().erase(i);
			return true;
		}
	}
	return false;
}

bool Trigger::isChild(const Trigger& child) const
{
	OutcomingLinksList::const_iterator i;
	FOR_EACH(outcomingLinks(), i) 
		if(i->child == &child)
			return false;
	return true;
}

//------------------------------------------------------------
TriggerChain::TriggerChain() 
{
	initialize();
}

void TriggerChain::buildLinks()
{
	TriggerList::iterator ti;
	FOR_EACH(triggers, ti)
		ti->incomingLinks_.clear();

	FOR_EACH(triggers, ti){
		vector<TriggerLink>::iterator li;
		FOR_EACH(ti->outcomingLinks_, li){
			Trigger* trigger = find(li->triggerName());
			if(trigger && trigger != &*ti){
				trigger->incomingLinks_.push_back(&*li);
				li->parent = &*ti;
				li->child = trigger;
			}
			else{
				li = ti->outcomingLinks_.erase(li);
				--li;
			}
		}
	}

	activeTriggers_.clear();
	FOR_EACH(triggers, ti)
		if(ti->active())
			activeTriggers_.push_back(&(*ti));

	triggerEvents_.clear();
}

void TriggerChain::initialize()
{
	if(triggers.empty() || strcmp(triggers.front().name(), "START"))
		triggers.insert(triggers.begin(), Trigger());
	triggers.front().setName("START");

	if(!find("Camera")){
		triggers.push_back(Trigger());
		triggers.back().setName("Camera");
	}

	if(!find("UserCamera")){
		triggers.push_back(Trigger());
		triggers.back().setName("UserCamera");
	}

	buildLinks();
}

void TriggerChain::initTriggersState()
{
	TriggerList::iterator ti;
	FOR_EACH(triggers, ti)
		ti->state_ = Trigger::SLEEPING;
	find("START")->state_ = Trigger::CHECKING;
}

TriggerChain& TriggerChain::operator=(const TriggerChain& rhs)
{
	triggers = rhs.triggers;
	boundingRect_ = rhs.boundingRect_;
	viewRect_ = rhs.viewRect_;
	buildLinks();
	return *this;
}

bool TriggerChain::operator==(const TriggerChain& rhs) const
{	
	if(triggers.size() != rhs.triggers.size())
		return false;

	for(int i = 0; i < triggers.size(); i++)
		if(strcmp(triggers[i].name(), rhs.triggers[i].name()))
			return false;

	return true;
}

Trigger* TriggerChain::find(const char* name)
{
	TriggerList::iterator ti;
	FOR_EACH(triggers, ti)
		if(!strcmp(ti->name(), name))
			return &*ti;
	return 0;
}

TriggerLink* TriggerChain::addLink(int parentIndex, int childIndex, int type) 
{
	Trigger& parent = const_cast<Trigger&>(triggers[parentIndex]);
	Trigger& child = const_cast<Trigger&>(triggers[childIndex]);

	parent.outcomingLinks_.push_back(TriggerLink());
	TriggerLink& newLink = parent.outcomingLinks_.back();
	newLink.setTriggerName(child.name());
	newLink.autoRestarted_ = static_cast<TriggerLink::Type>(type);
	newLink.parent = &parent;
	newLink.child = &child;

	buildLinks();
	return &newLink;
}

bool TriggerChain::removeLink(int parentIndex, int childIndex) 
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

Trigger* TriggerChain::addTrigger(Trigger const& trigger)
{
	triggers.push_back(trigger);
	buildLinks();
	return &triggers.back();
}

Trigger* TriggerChain::insertTrigger(int pos, Trigger const& trigger){
	vector<Trigger>::iterator itr = triggers.insert(triggers.begin() + pos, trigger);
	if (itr == triggers.end())
		return NULL;
	buildLinks();
	return &*itr;
}

void TriggerChain::removeTrigger(int triggerIndex)
{
	TriggerList::iterator forErase = triggers.begin();
	std::advance(forErase, triggerIndex);
	triggers.erase(forErase);
	buildLinks();
}

int TriggerChain::triggerIndex(const Trigger& trigger) const
{
	if(&trigger < &triggers.front() || &triggers.back() < &trigger)
		return -1;
	return (&trigger - &triggers.front());
}


void TriggerChain::addLogRecord(const Trigger& trigger, const char* event)
{
#ifndef _FINAL_VERSION_
	triggerEvents_.push_back(TriggerEvent());
	TriggerEvent& record = triggerEvents_.back();
	XBuffer buf;
	buf < event;// < " " <= global_time();
	record.event = buf;
	record.state = trigger.state_;
	for(int i = 0; i < triggers.size(); i++)
		if(&triggers[i] == &trigger){
			record.index = i;
			break;
		}
#endif
}

void TriggerChain::setLogRecord(int index)
{
	if(index < 0 || index >= triggerEvents_.size())
		return;
	
	initTriggersState();

	for(int i = 0; i <= index; i++){
		TriggerEvent& event = triggerEvents_[i];
		triggers[event.index].state_ = event.state;
	}
}

bool TriggerChain::isLogValid() const
{
	return !triggerEvents_.empty();
}

void TriggerChain::setLogData(const TriggerEventList& data)
{
	triggerEvents_ = data;
}

//----------------------------------------------
const char* Action::name() const
{
	return triggerInterface().actionName(*this);
}

