// EnsureTriggerNameOriginality.cpp: implementation of the EnsureTriggerNameOriginality class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EnsureTriggerNameOriginality.h"
#include <algorithm>
#include "TEEngine/TestTriggerName.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


EnsureTriggerNameOriginality::EnsureTriggerNameOriginality()
{

}

EnsureTriggerNameOriginality::~EnsureTriggerNameOriginality()
{

}

bool EnsureTriggerNameOriginality::run(TriggerChain const& chain, 
									 CString const& name)
{
	TriggerList const& triggers = chain.triggers;
	TriggerList::const_iterator res = 
		std::find_if(triggers.begin(), triggers.end(), TestTriggerName(name));
	return (res == triggers.end());
}

CString EnsureTriggerNameOriginality::getUniqueName(
									TriggerChain const& chain, 
									CString const& name)
{
	CString copy(name);
	TCHAR buff[32];
	int counter = 0;
	while (!run(chain, copy)) {
		copy = name;
		copy += _itot(counter, buff, 10);
		++counter;
	}
	return copy;
}
