// SelectionManager.h: interface for the SelectionManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELECTIONMANAGER_H__01685F73_42E8_4E8A_85BF_8DA6BAF6F0F4__INCLUDED_)
#define AFX_SELECTIONMANAGER_H__01685F73_42E8_4E8A_85BF_8DA6BAF6F0F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "TriggerExport.h"

class SelectionManager  
{

public:
	typedef std::vector<int>::const_iterator const_iterator;

	SelectionManager();
	~SelectionManager();

	void setTriggerChain(TriggerChain* ptrTriggerChain);
	TriggerChain* getTriggerChain() const;

	void select(int index);
	void deselect(int index);
	void reselect(int index);

	void deselectAll();
	//! очищает внутрений буффер
	void clear();

	int getLast() const;

	bool isSelected(int index) const;

	const_iterator begin() const;
	const_iterator end() const;
	size_t getCount() const;
	bool empty() const;
protected:
	void select(int index, bool select);
	Trigger& getTrigger(int index) const;
private:
	TriggerChain* ptrTriggerChain_;
	std::vector<int> selected_;
};

#endif // !defined(AFX_SELECTIONMANAGER_H__01685F73_42E8_4E8A_85BF_8DA6BAF6F0F4__INCLUDED_)
