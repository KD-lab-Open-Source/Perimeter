// TriggerClipBuffer.h: interface for the TriggerClipBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIGGERCLIPBUFFER_H__AE8E7454_A033_4A9E_A473_AFC1C876C671__INCLUDED_)
#define AFX_TRIGGERCLIPBUFFER_H__AE8E7454_A033_4A9E_A473_AFC1C876C671__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "TriggerExport.h"

class TriggerClipBuffer  
{
private:
	typedef std::list<Trigger> Container;
public:
	TriggerClipBuffer();
	~TriggerClipBuffer();

	typedef Container::const_iterator iterator;
	typedef Container::value_type value_type;

	bool empty();
	void clear();

	iterator begin() const;
	iterator end() const;

	void push_back(Trigger const& trigger);
	static TriggerClipBuffer& instance();
private:
	Container list_;
};

#endif // !defined(AFX_TRIGGERCLIPBUFFER_H__AE8E7454_A033_4A9E_A473_AFC1C876C671__INCLUDED_)
