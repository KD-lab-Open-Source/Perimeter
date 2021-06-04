// EnsureTriggerNameOriginality.h: interface for the EnsureTriggerNameOriginality class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENSURETRIGGERNAMEORIGINALITY_H__4955DDA6_F5EF_41E5_A706_F01D41206C18__INCLUDED_)
#define AFX_ENSURETRIGGERNAMEORIGINALITY_H__4955DDA6_F5EF_41E5_A706_F01D41206C18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TriggerExport.h"
class EnsureTriggerNameOriginality  
{
public:
	EnsureTriggerNameOriginality();
	~EnsureTriggerNameOriginality();
	static bool run(TriggerChain const& chain, CString const& name);
	static CString getUniqueName(TriggerChain const& chain, 
		CString const& name);
};

#endif // !defined(AFX_ENSURETRIGGERNAMEORIGINALITY_H__4955DDA6_F5EF_41E5_A706_F01D41206C18__INCLUDED_)
