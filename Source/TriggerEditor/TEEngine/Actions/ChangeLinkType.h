// ChangeLinkType.h: interface for the ChangeLinkType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANGELINKTYPE_H__2CB04223_20B9_4F0C_86EF_C5D1F2796DB3__INCLUDED_)
#define AFX_CHANGELINKTYPE_H__2CB04223_20B9_4F0C_86EF_C5D1F2796DB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TriggerExport.h"
class ChangeLinkType  
{
public:
	ChangeLinkType(TriggerChain& chain, 
					int triggerIndex, 
					int linkIndex,
					int type);
	~ChangeLinkType();

	bool operator()();
	static bool run(TriggerChain& chain, 
					int triggerIndex, 
					int linkIndex,
					int type);
private:
	TriggerChain& chain_;
	int triggerIndex_;
	int linkIndex_;
	int type_;
};

#endif // !defined(AFX_CHANGELINKTYPE_H__2CB04223_20B9_4F0C_86EF_C5D1F2796DB3__INCLUDED_)
