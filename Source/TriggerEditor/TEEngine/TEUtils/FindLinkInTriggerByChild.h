// FindLinkInTriggerByChild.h: interface for the FindLinkInTriggerByChild class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDLINKINTRIGGERBYCHILD_H__C8CFA5C0_05B4_429C_88FF_71721E211889__INCLUDED_)
#define AFX_FINDLINKINTRIGGERBYCHILD_H__C8CFA5C0_05B4_429C_88FF_71721E211889__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Trigger;
struct TriggerLink;

class FindLinkInTriggerByChild  
{
	class TestLink;
public:
	FindLinkInTriggerByChild(Trigger const* child);
	~FindLinkInTriggerByChild();
	bool operator()(TriggerLink const& link) const;
private:
	Trigger const* child_;
};

#endif // !defined(AFX_FINDLINKINTRIGGERBYCHILD_H__C8CFA5C0_05B4_429C_88FF_71721E211889__INCLUDED_)
