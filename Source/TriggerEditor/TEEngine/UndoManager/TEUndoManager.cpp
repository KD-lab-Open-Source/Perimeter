#include "stdafx.h"
#include "teundomanager.h"
#include <boost/utility.hpp>
#include <iterator>

bool TEUndoManager::Bunch::addAction(TEUndoManager::Action const& action)
{
	m_buffer.push_back(action);
	return true;
}

bool TEUndoManager::Bunch::operator()()	const
{
	ActionContainer::const_iterator i = m_buffer.begin(), e = m_buffer.end();
	for(; i != e; ++i)
		if (!((*i)()))
			return false;
	return true;
}
void TEUndoManager::Bunch::reverse()
{
	m_buffer.reverse();
}

bool TEUndoManager::Bunch::empty() const{
	return m_buffer.empty();
}
TEUndoManager::TEUndoManager(void)
{
	setUNDOPointer(m_buffer.rend());
	setREDOPointer(m_buffer.end());
}
TEUndoManager::TEUndoManager(TEUndoManager const& rhs)
{
	assert(rhs.m_buffer.empty());
	setUNDOPointer(m_buffer.rend());
	setREDOPointer(m_buffer.end());
}

TEUndoManager::~TEUndoManager(void)
{
}

bool TEUndoManager::addAction(Action const& actionDO, Action const& actionREDO)
{
	//очищаем от текущей позиции до конца
	m_buffer.erase(m_itrREDOPointer, m_buffer.end());
	setREDOPointer(m_buffer.end());
	m_buffer.insert(m_buffer.end(), std::make_pair(actionDO, actionREDO));
	setUNDOPointer(m_buffer.rbegin());
	
	return true;
}

bool TEUndoManager::undo()
{
	if (!canUNDO())
		return false;

	bool const b = undo(*m_itrUNDOPointer);
	undoMoveIterators();

	return b;
}

bool TEUndoManager::redo()
{
	if (!canREDO())
		return false;

	redoMoveIterators();
	return redo(*m_itrUNDOPointer);
}

void TEUndoManager::setUNDOPointer(reverse_iterator const& itr){
	m_itrUNDOPointer = itr;
}

TEUndoManager::reverse_iterator const& TEUndoManager::getUNDOPointer() const{
	return m_itrUNDOPointer;
}

void TEUndoManager::setREDOPointer(iterator const& itr){
	m_itrREDOPointer = itr;
}

TEUndoManager::iterator const& TEUndoManager::getREDOPointer() const{
	return m_itrREDOPointer;
}

bool TEUndoManager::undo(DOREDOPair const& doredoPair) const{
	return doredoPair.first();
}
bool TEUndoManager::redo(DOREDOPair const& doredoPair) const{
	return doredoPair.second();
}

bool TEUndoManager::canUNDO(){
	return !(m_buffer.empty()||getUNDOPointer() == m_buffer.rend());
}
bool TEUndoManager::canREDO(){
	return !(m_buffer.empty()||getREDOPointer() == m_buffer.end());;
}

//сдвигаем итераторы на операцию undo
void TEUndoManager::undoMoveIterators()
{
	++m_itrUNDOPointer;
	--m_itrREDOPointer;
//	setREDOPointer(m_itrUNDOPointer.base());
}
//сдвигаем итераторы на операцию redo
void TEUndoManager::redoMoveIterators()
{
	--m_itrUNDOPointer;
	++m_itrREDOPointer;
//	setREDOPointer(m_itrUNDOPointer.base());
}
