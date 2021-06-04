// FindLinkInTriggerByChild.cpp: implementation of the FindLinkInTriggerByChild class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TriggerEditorApp.h"
#include "FindLinkInTriggerByChild.h"
#include "TriggerExport.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FindLinkInTriggerByChild::FindLinkInTriggerByChild(Trigger const* child)
: child_(child)
{
}

FindLinkInTriggerByChild::~FindLinkInTriggerByChild()
{

}

bool FindLinkInTriggerByChild::operator()(TriggerLink const& link) const
{
	return (link.child == child_);
}