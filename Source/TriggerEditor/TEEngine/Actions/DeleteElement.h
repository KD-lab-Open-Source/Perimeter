#pragma once

#include "TriggerExport.h"

class TEBaseWorkMode;

class DeleteElement
{
public:
	DeleteElement(TriggerChain & chain, int triggerIndex);
	bool operator()();
	~DeleteElement(void);
	static bool run(TriggerChain & chain, int triggerIndex);
private:
	TriggerChain & chain_;
	int triggerIndex_;
};
