// TriggerChainProfiler.h: interface for the TriggerChainProfiler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIGGERCHAINPROFILER_H__B2729A99_C4BE_468D_B4F0_C322FC18D5B8__INCLUDED_)
#define AFX_TRIGGERCHAINPROFILER_H__B2729A99_C4BE_468D_B4F0_C322FC18D5B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ITriggerChainProfiler.h"
#include "TriggerExport.h"

/**
 	Вспомогательный класс, для того, чтобы изолировать остальную часть кода от периметра
*/
class TriggerChainProfiler : public ITriggerChainProfiler  
{
public:
	TriggerChainProfiler();
	~TriggerChainProfiler();

	void setTriggerChain(TriggerChain* ptrChain);

	virtual int getEntriesCount() const;
	virtual char const* getEntryText(int entryIndex) const;
	virtual bool setCurrentEntry(int entryIndex);
	virtual bool isDataEnabled() const;
private:
	TriggerChain* ptrChain_;
};

#endif // !defined(AFX_TRIGGERCHAINPROFILER_H__B2729A99_C4BE_468D_B4F0_C322FC18D5B8__INCLUDED_)
