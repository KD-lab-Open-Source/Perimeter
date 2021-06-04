#pragma once
#include <utility>

#include "TriggerExport.h"

typedef std::pair<TriggerChain const* const*, int> DrawingData;

interface IDrawData
{
	virtual DrawingData getDrawedChains() const = 0;
};