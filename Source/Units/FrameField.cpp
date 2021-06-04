#include "StdAfx.h"

#include "Runtime.h"
#include "Universe.h"
#include "FrameCore.h"
#include "FrameField.h"

#include "UniverseInterface.h"

#include "IronExplosion.h"

#include "PerimeterSound.h"
#include "Config.h"
#include "IronClusterUnit.h"

terProtector::terProtector(const UnitTemplate& data) : terBuildingEnergy(data)
{
	fieldState_ = FIELD_STOPPED;
	enableCharge_ = true;
	startWhenCharged_ = false;
	reinitZeroCounter_ = 0;
}

terProtector::~terProtector()
{
}

terUnitMonk* terProtector::createMonk()
{
	//Поривет Шуре, не забыть переписать.
	UnitTemplate data(Player->unitAttribute(UNIT_ATTRIBUTE_MONK),Player);
	terUnitMonk* p=universe()->monks.create(data);
	p->setPosition(position());
	p->Start();
	return p;
}

void terProtector::setPose(const Se3f& poseIn, bool initPose)
{
	terBuildingEnergy::setPose(poseIn, initPose);

	//Неожиданно криво, привет Шуре.
	if(initPose && monks_.empty())
	for(int i = 0; i < fieldPrm.monksPerCore; i++)
	{
		terUnitMonk* p = createMonk();
		monks_.push_back(p);
	}
}

void terProtector::Kill()
{
	terBuildingEnergy::Kill();

	freeZeroLayer();
	killMonks();
}


void terProtector::killMonks()
{
	MonkList::iterator mi;
	FOR_EACH(monks_, mi)
		(*mi)->kill();
	monks_.clear();
}

void terProtector::ChangeUnitOwner(terPlayer* player)
{
	terBuildingEnergy::ChangeUnitOwner(player);

	freeZeroLayer();
	killMonks();

	fieldState_ = FIELD_STOPPED;
	enableCharge_ = true;
	startWhenCharged_ = false;
}

void terProtector::DestroyLink()
{
	terBuildingEnergy::DestroyLink();

	removeNotAliveMonk(monks_);
//	if(removeNotAlive(monks_))
//		stopField();
}

void terProtector::monksGoHome()
{
	fieldState_ = FIELD_STOPPING;

	MonkList::const_iterator mi;
	FOR_EACH(monks(), mi)
		(*mi)->goHome();
}

void terProtector::MoveQuant()
{
	terBuildingEnergy::MoveQuant();

	if(isBuildingEnable() && enableCharge() && !(fieldState() & (FIELD_STARTING | FIELD_STARTED)) && monks().size() < fieldPrm.monksPerCore 
	  && productionCompleted()){
		terUnitMonk* p = createMonk();
		monks_.push_back(p);
		//SND2DPlaySound("core_generator_integrated");
	}

	switch(fieldState()){
	case FIELD_STARTING: 
		if(find_if(monks_.begin(), monks_.end(), 
			compose1(bind2nd(not_equal_to<int>(), MONK_MODE_GUARD), mem_fun(&terUnitMonk::monkMode))) == monks().end())
				fieldState_ = FIELD_STARTED;
		break;

	case FIELD_STARTED: 
		if(!isBuildingEnable())
			fieldState_ = FIELD_TO_STOP;
		break;

	case FIELD_STOPPING: 
		if(find_if(monks_.begin(), monks_.end(), 
			compose1(bind2nd(not_equal_to<int>(), MONK_MODE_SLEEP), mem_fun(&terUnitMonk::monkMode))) == monks().end())
				fieldState_ = FIELD_STOPPED;
		break;

	case FIELD_TO_STOP:
	case FIELD_STOPPED:
		if((startWhenCharged_ || Player->totalDefenceMode()) && canStartField())
			startField();
		break;
	}

	switch(reinitZeroCounter_){
	case 0:
		break;
	case 1:
		if(monks_.empty())
			reinitZeroCounter_ = 2;
		break;
	case 2:
		if(!monks_.empty())
			reinitZeroCounter_ = 3;
		break;
	default:
		reinitZeroCounter_ = 0;
	}
}

SaveUnitData* terProtector::universalSave(SaveUnitData* baseData)
{
	SaveUnitProtectorData* data = castOrCreate<SaveUnitProtectorData>(baseData);
	terBuildingEnergy::universalSave(data);

	data->monksNumber = monks().size();
	data->fieldState = fieldState_;
	data->enableCharge = enableCharge_;
	data->startWhenCharged = startWhenCharged_;

	return data;
}

void terProtector::universalLoad(const SaveUnitData* baseData)
{
	const SaveUnitProtectorData* data = dynamic_cast<const SaveUnitProtectorData*>(baseData);
	terBuildingEnergy::universalLoad(data);

	fieldState_ = (FieldState)data->fieldState;
	if(fieldState() == FIELD_STARTING || fieldState() == FIELD_STARTED)
		fieldState_ = FIELD_TO_START;
	enableCharge_ = data->enableCharge;
	startWhenCharged_ = data->startWhenCharged;

	killMonks();
	for(int i = 0; i < data->monksNumber; i++){
		terUnitMonk* p = createMonk();
		monks_.push_back(p);
	}
}

void terProtector::Start()
{
	terBuildingEnergy::Start();

	safe_cast<terInterpolationCore*>(avatar())->setProtectionParameterImmediately(isBuildingEnable() ? 
		(float)(monks().size())/(float)(fieldPrm.monksPerCore) : 0);
}

void terProtector::UpdateAvatar()
{
	terBuildingEnergy::UpdateAvatar();
	
	if(isBuildingEnable()){
		avatar()->SetProtectionMode(1);
		safe_cast<terInterpolationCore*>(avatar())->setProtectionParameter((float)(monks().size())/(float)(fieldPrm.monksPerCore));
	}
	else{
		avatar()->SetProtectionMode(0);
		safe_cast<terInterpolationCore*>(avatar())->setProtectionParameter(0);
	}
}

void terProtector::startField() 
{ 
	if(canStartField())
		fieldState_ = FIELD_TO_START; 
	startWhenCharged_ = true; 
}

void terProtector::stopField() 
{ 
	fieldState_ = FIELD_TO_STOP; 
	startWhenCharged_ = false; 
}

void terProtector::executeCommand(const UnitCommand& command)
{
	terBuildingEnergy::executeCommand(command);

	switch(command.commandID()){
	case COMMAND_ID_START_CHARGING:
		enableCharge_ = true;
		break;
	case COMMAND_ID_STOP_CHARGING:
		enableCharge_  = false;
		reinitZeroCounter_ = 1;
		killMonks();
		break;
	case COMMAND_ID_FIELD_START:
		startField();
		soundEvent(SOUND_VOICE_PSHIELD_LOCAL_ACTIVE);
		break;
	case COMMAND_ID_FIELD_STOP:
		stopField();
		soundEvent(SOUND_VOICE_PSHIELD_LOCAL_OFF);
		break;
	}
}

bool terProtector::isBuildingEnable() const
{
	if(reinitZeroCounter_ == 3)
		return false;
	return __super::isBuildingEnable();
}

void terProtector::showDebugInfo()
{
	terBuilding::showDebugInfo();
	
	if(show_core_generator_status){
		string str;
		switch(fieldState()){
		case FIELD_STOPPED:
			str += "STOPPED";
			break;
		case FIELD_TO_START:
			str += "TO_START";
			break;
		case FIELD_STARTING:
			str += "STARTING";
			break;
		case FIELD_STARTED:
			str += "STARTED";
			break;
		case FIELD_TO_STOP:
			str += "TO_STOP";
			break;
		case FIELD_STOPPING:
			str += "STOPPING";
			break;
		}
		if(canStartField())
			str += " canStartField";

		show_text(position(), str.c_str(), CYAN);
	}
}
