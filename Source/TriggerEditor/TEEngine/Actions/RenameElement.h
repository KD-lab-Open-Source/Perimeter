// RenameElement.h: interface for the RenameElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENAMEELEMENT_H__F3F4399E_28E8_402D_AB70_34B689A748EC__INCLUDED_)
#define AFX_RENAMEELEMENT_H__F3F4399E_28E8_402D_AB70_34B689A748EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TriggerExport.h"

class RenameElement  
{
public:
	RenameElement(TriggerChain& chain, 
					int triggerIndex, 
					CString const& newName);
	~RenameElement();
	bool operator()() const;
private:
	TriggerChain& chain_;
	int triggerIndex_;
	CString newName_;
};

#endif // !defined(AFX_RENAMEELEMENT_H__F3F4399E_28E8_402D_AB70_34B689A748EC__INCLUDED_)
