// TriggerClipBuffer.cpp: implementation of the TriggerClipBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TriggerClipBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TriggerClipBuffer& TriggerClipBuffer::instance(){
	static TriggerClipBuffer instance;
	return instance;
}

TriggerClipBuffer::TriggerClipBuffer()
{

}

TriggerClipBuffer::~TriggerClipBuffer()
{

}

bool TriggerClipBuffer::empty(){
	return list_.empty();
}

void TriggerClipBuffer::clear(){
	list_.clear();
}

TriggerClipBuffer::iterator TriggerClipBuffer::begin() const{
	return list_.begin();
}
TriggerClipBuffer::iterator TriggerClipBuffer::end() const{
	return list_.end();
}

void TriggerClipBuffer::push_back(Trigger const& trigger){
	list_.push_back(trigger);
}
