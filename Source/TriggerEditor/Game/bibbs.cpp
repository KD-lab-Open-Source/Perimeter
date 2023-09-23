#if 0
#include "StdAfx.h"
#include "GameInclude.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CustomFunctions customFunctions;

SyncroTimer::SyncroTimer(void)
{
}

//-------------------------------------------------

void ConditionCreateObject::checkEvent(AIPlayer& aiPlayer, const Event* event) { }
void ConditionCaptureBuilding::checkEvent(AIPlayer& aiPlayer, const Event* event) {  }
void ConditionTeleportation::checkEvent(AIPlayer& aiPlayer, const Event* event) {  }
void ConditionUnitClassUnderAttack::checkEvent(AIPlayer& aiPlayer, const Event* event) {  }
void ConditionKillObjectByLabel::checkEvent(class AIPlayer &,class Event const &) {  }

void ConditionPlayerState::checkEvent(class AIPlayer &,class Event const &){}
void ConditionTimeMatched::checkEvent(class AIPlayer &,class Event const &){}

void ConditionKillObject::checkEvent(class AIPlayer &,class Event const &){}
void ConditionUnitClassIsGoingToBeAttacked::checkEvent(class AIPlayer &,
													   class Event const &){}
void ConditionMouseClick::checkEvent(class AIPlayer &,class Event const &){}
void ConditionClickOnButton::checkEvent(class AIPlayer &,class Event const &){}
void ConditionActivateSpot::checkEvent(class AIPlayer &,class Event const &){}

bool ConditionDifficultyLevel::check(AIPlayer& aiPlayer) { return true; }
bool ConditionOnlyMyClan::check(AIPlayer& aiPlayer) { return true; }
bool ConditionSkipCutScene::check(AIPlayer& aiPlayer) { return true; }
bool ConditionCutSceneWasSkipped::check(AIPlayer& aiPlayer) { return true; }
bool ConditionEnegyLevelLowerReserve::check(AIPlayer& aiPlayer) { return true; }
bool ConditionEnegyLevelUpperReserve::check(AIPlayer& aiPlayer) { return true; }
bool ConditionSquadSufficientUnits::check(AIPlayer& aiPlayer) { return true; }
bool ConditionObjectExists::check(AIPlayer& aiPlayer) { return true; }
bool ConditionObjectByLabelExists::check(AIPlayer& aiPlayer) { return true; }
bool ConditionIsPlayerAI::check(class AIPlayer &) { return true;}
bool ConditionSquadGoingToAttack::check(class AIPlayer &) { return true;}
bool ConditionCorridorOmegaUpgraded::check(class AIPlayer &) { return true;}
bool ConditionMutationEnabled::check(class AIPlayer &) { return true;}
bool ConditionBuildingNearBuilding::check(class AIPlayer &) { return true;}
bool ConditionIsFieldOn::check(class AIPlayer &) { return true;}
bool ConditionObjectNearObjectByLabel::check(class AIPlayer &) { return true;}
bool ConditionToolzerSelectedNearObjectByLabel::check(class AIPlayer &) { return true;}
bool ConditionTerrainLeveledNearObjectByLabel::check(class AIPlayer &) { return true;}
bool ConditionSetSquadWayPoint::check(class AIPlayer &) { return true;}
bool ConditionEnegyLevelBelowMaximum::check(class AIPlayer &) { return true; }
bool ConditionOutOfEnergyCapacity::check(class AIPlayer &) { return true; }
bool ConditionNumberOfBuildingByCoresCapacity::check(class AIPlayer &) { return true; }
bool ConditionFrameState::check(class AIPlayer &) { return true; }
bool ConditionCheckBelligerent::check(AIPlayer& aiPlayer) { return true; }
bool ConditionWeaponIsFiring::check(AIPlayer& aiPlayer) { return true; }

void ConditionCreateObject::initialize(void){}
void ConditionEnegyLevelBelowMaximum::initialize(void){}
void ConditionPlayerState::initialize(void){}

void ConditionKillObject::initialize(void){}
void ConditionBuildingNearBuilding::initialize(void){}
void ConditionClickOnButton::initialize(void){}

//--------------------------------------------------
void ActionSquadAttack::initialize(void){}

void ActionDelay::activate(AIPlayer& aiPlayer){}
void ActionOscillateCamera::activate(class AIPlayer &){}
void ActionUpgradeOmega::activate(class AIPlayer &){}
void ActionAttackBySpecialWeapon::activate(class AIPlayer &){}
void ActionActivateObjectByLabel::activate(class AIPlayer &){}
void ActionDeactivateObjectByLabel::activate(class AIPlayer &){}
void ActionActivateAllSpots::activate(class AIPlayer &){}
void ActionDeactivateAllSpots::activate(class AIPlayer &){}
void ActionSetControlEnabled::activate(class AIPlayer &){}
void ActionTask::activate(class AIPlayer &){}
void ActionSetCameraAtObject::activate(class AIPlayer &){}
void ActionSetControls::activate(class AIPlayer &){}
void ActionSelectUnit::activate(class AIPlayer &){}
void ActionSetInterface::activate(class AIPlayer &){}

void ActionHoldBuilding::activate(class AIPlayer &){}
void ActionSellBuilding::activate(class AIPlayer &){}
void ActionSetCamera::activate(class AIPlayer &) {}
void ActionVictory::activate(class AIPlayer &) {}
void ActionDefeat::activate(class AIPlayer &) {}
void ActionTeleportationOut::activate(class AIPlayer &) {}
void ActionInstallFrame::activate(class AIPlayer &) {}
void ActionOrderBuilding::initialize() {}
void ActionChargeCores::activate(class AIPlayer &) {}
void ActionSwitchFieldOn::activate(class AIPlayer &) {}
void ActionSquadAttack::activate(class AIPlayer &) {}
void ActionSquadMove::activate(class AIPlayer &){}
void ActionProduceBrigadierOrProrab::activate(class AIPlayer &){}
void ActionSwitchGuns::activate(AIPlayer& aiPlayer){}
void ActionRepareObjectByLabel::activate(AIPlayer& aiPlayer) {}

bool ActionOrderBuilding::workedOut(class AIPlayer &){ return true;}
bool ActionKillObject::workedOut(class AIPlayer &){ return true;}
bool ActionTask::workedOut(class AIPlayer &){ return true;}
bool ActionInstallFrame::workedOut(class AIPlayer &){ return true; }
bool ActionSquadAttack::workedOut(class AIPlayer &){ return true; }
bool ActionSwitchFieldOn::workedOut(class AIPlayer &){ return true; }
bool ActionDelay::workedOut(class AIPlayer &){ return true; }
bool ActionSetCamera::workedOut(class AIPlayer &){ return true; }
void ActionSquadOrderUnits::activate(class AIPlayer &) {}
bool ActionSquadOrderUnits::workedOut(class AIPlayer &){ return true; }
bool ActionMessage::workedOut(class AIPlayer &){ return true; }
bool ActionSetCameraAtObject::workedOut(AIPlayer& aiPlayer){ return true; }

bool ActionSwitchGuns::automaticCondition(class AIPlayer &) const { return true; }
bool ActionSquadAttack::automaticCondition(class AIPlayer &) const { return true; }
bool ActionOrderBuilding::automaticCondition(class AIPlayer &) const { return true; }
bool ActionChargeCores::automaticCondition(class AIPlayer &) const { return true; }
bool ActionSwitchFieldOn::automaticCondition(class AIPlayer &) const { return true; }
bool ActionSquadOrderUnits::automaticCondition(class AIPlayer &) const { return true; }
bool ActionTeleportationOut::automaticCondition(class AIPlayer &)const  { return true;}
bool ActionUpgradeOmega::automaticCondition(class AIPlayer &)const  { return true;}
bool ActionAttackBySpecialWeapon::automaticCondition(class AIPlayer &)const  { return true;}
bool ActionSellBuilding::automaticCondition(class AIPlayer &)const  { return true; }

void ActionSellBuilding::initialize(){}

void SaveCameraSplineData::initialize(){}

class terUnitBase * ActionKillObject::findObject() const{
	return NULL;
};

bool Trigger::active() const { return true; }

void SavePrm::initialize() {}


char const * editTextMultiLine(void* hwnd, const TreeNode* treeNode){
	return customFunctions.editTextMultiLine(hwnd, treeNode);
}
char const* editMessageID(void* hwnd, const TreeNode* treeNode){
	return customFunctions.editMessageID(hwnd, treeNode);
}

char const* editTaskID(void* hwnd, const TreeNode* treeNode){
	return customFunctions.editTaskID(hwnd, treeNode);
}

char const* editLabelDialog(void* hwnd, const TreeNode* treeNode){
	return customFunctions.editLabelDialog(hwnd, treeNode);
}

char const* editCameraSplineName(void* hwnd, const TreeNode* treeNode){
	return customFunctions.editCameraSplineName(hwnd, treeNode);
}

#endif