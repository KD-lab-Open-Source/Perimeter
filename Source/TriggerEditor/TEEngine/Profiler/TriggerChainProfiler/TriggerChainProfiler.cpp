// TriggerChainProfiler.cpp: implementation of the TriggerChainProfiler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TriggerChainProfiler.h"
//#include "Game/GameInclude.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TriggerChainProfiler::TriggerChainProfiler():
  ptrChain_(NULL)
{

}

TriggerChainProfiler::~TriggerChainProfiler()
{

}

void TriggerChainProfiler::setTriggerChain(TriggerChain* ptrChain){
	ptrChain_ = ptrChain;
}

int TriggerChainProfiler::getEntriesCount() const{
	assert(ptrChain_);
	return ptrChain_->numLogRecords();
}

char const* TriggerChainProfiler::getEntryText(int entryIndex) const{
	assert(ptrChain_);
	return ptrChain_->logRecordText(entryIndex);
}

bool TriggerChainProfiler::setCurrentEntry(int entryIndex){
	assert(ptrChain_);
	ptrChain_->setLogRecord(entryIndex);
	return true;
}

bool TriggerChainProfiler::isDataEnabled() const{
	return ptrChain_->isLogValid();
}
