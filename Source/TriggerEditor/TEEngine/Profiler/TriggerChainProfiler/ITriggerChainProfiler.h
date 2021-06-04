#ifndef __TRIGGER_CHAIN_PROFILER
#define __TRIGGER_CHAIN_PROFILER

#pragma once

interface ITriggerChainProfiler
{
	virtual int getEntriesCount() const = 0;
	virtual char const* getEntryText(int entryIndex) const = 0;
	virtual bool setCurrentEntry(int entryIndex) = 0;
	virtual bool isDataEnabled() const = 0;
};

#endif//__TRIGGER_CHAIN_PROFILER