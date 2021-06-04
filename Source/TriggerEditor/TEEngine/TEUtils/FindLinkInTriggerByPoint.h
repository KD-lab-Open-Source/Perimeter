// FindLinkInTriggerByPoint.h: interface for the FindLinkInTriggerByPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FindLinkInTriggerByPoint_H__D78F01C2_BB80_4CE0_B456_13F4A7410DD8__INCLUDED_)
#define AFX_FindLinkInTriggerByPoint_H__D78F01C2_BB80_4CE0_B456_13F4A7410DD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Trigger;

class FindLinkInTriggerByPoint  
{
	class TestLink;
public:
	typedef Trigger const argument_type;
	typedef bool result_type;

	FindLinkInTriggerByPoint(CPoint const& testPoint, int &linkIndex);
	~FindLinkInTriggerByPoint();
	bool operator()(argument_type& trigger) ;
	int getLinkIndex() const;
private:
	int& linkIndex_;
	CPoint const& testPoint_;
};

#endif // !defined(AFX_FindLinkInTriggerByPoint_H__D78F01C2_BB80_4CE0_B456_13F4A7410DD8__INCLUDED_)
