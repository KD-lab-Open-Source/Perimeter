#pragma once

#include "Save.h"
#include "TreeInterface.h"

class SaveAdapter : SavePrm
{
public:
	SaveAdapter();
	~SaveAdapter();

	int playersNumber() const { 
		return manualData.players.size();
	}
	PlayerStrategy& playerStrategy(int playerIndex) {
		return manualData.players[playerIndex].strategy;
	}

	const char* actionComboList() const;  // |-separated
	const char* conditionComboList() const; // |-separated
	ActionPtr createAction(int typeIndex) const; // index in actionComboList
	ConditionPtr createCondition(int typeIndex) const; // index in conditionComboList
};
