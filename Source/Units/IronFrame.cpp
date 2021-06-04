#include "StdAfx.h"

#include "CameraManager.h"

#include "Universe.h"
#include "Player.h"

#include "IronFrame.h"
#include "IronBuilding.h"
#include "FrameCore.h"
#include "FrameField.h"

#include "Runtime.h"
#include "UniverseInterface.h"

#include "IronExplosion.h"

#include "PerimeterSound.h"
#include "Config.h"

#include "GameShell.h"
#include "IronPort.h"
#include "EditArchive.h"
#include "XPrmArchive.h"
#include "BinaryArchive.h"

REGISTER_CLASS(AttributeBase, AttributeFrame, "Фрейм");

AttributeFrame::AttributeFrame()
{
	heightMax = 30;
	accumulatedEnergyInitial = 1; // 0..1, только для вновь установленного фрейма, иначе - в редакторе миссий
	repairElementsPerQuant = FrameRepairElementsPerQuant;

	oneStepMovement = 5;
	
	energyReserveToChargeSpiral = 1000;
	
	mmpHomePositionRadius = 200;
	
	normalVelocity = 10;
	teleportationVelocity = 30;

	InstallBound = true;
	IgnoreTargetTrace = true;
}

terFrame::terFrame(const UnitTemplate& data) : 
terUnitReal(data),
installer_(1)
{
	powered_ = false;
	RequestStatus = ISOLATED;
	ValidStatus = ISOLATED;
	activeSlot_ = 0;

	dockStatus_ = DOCK_STATUS_NONE;

	MoveSoundPoint = 0;
	productionSound_ = 0;

	SpeedFactor = 0;
	SoundAttachStatus = 0;

	SquadPoint = safe_cast<terUnitSquad*>(Player->buildUnit(UNIT_ATTRIBUTE_SQUAD));
	SquadPoint->setCommander(this);

	underTeleportation_ = false;

	antigravConsumer_.attach(Player, attr().antigravConsumption);
	movementConsumption_.attach(Player, attr().movementConsumption);
	repairConsumption_.attach(Player, attr().repairConsumption);
	
	static ConsumptionData spiralData;
	spiralData.energy = gameShell->manualData().spiralChargingEnergy;
	spiralData.time = gameShell->manualData().spiralChargingTime;
	spiralData.priority = gameShell->manualData().spiralChargingPriority;
	spiralData.name="Spiral";
	if(gameShell->manualData().spiralChargingEnergy > 0)
		spiralConsumer_.attach(Player, spiralData);

	addAccumulatedEnergy(attr().accumulatedEnergyInitial*attr().energyCapacity);

	installer_.InitObject(&attr());

	spiralLevelPrev_ = 0;

	catched_ = false;

	nativeBelligerent_ = Player->belligerent();
}

void terFrame::setRealModel(int modelIndex, float scale)
{
	__super::setRealModel(modelIndex, 1);

	pillar_ = avatar()->animationGroup(ANIMATION_GROUP_PILLAR);
	animate_ = avatar()->animationGroup(ANIMATION_GROUP_ANIMATE);

	MoveSoundPoint = realAvatar()->findSound(SOUND_MOVE);
	productionSound_ = realAvatar()->findSound(SOUND_PRODUCTION);

	realAvatar()->setChain(CHAIN_ATTACHED_INPUT);
	realAvatar()->updateLogicObject();
	inputPositionOffset_ = DockList.front().position().trans() - position();
}

terFrame::~terFrame()
{
}

void terFrame::setPose(const Se3f& pose, bool initPose)
{
	terUnitReal::setPose(pose, initPose);
	if(initPose){
		Se3f poseShifted = pose;
		poseShifted.trans() += to3D(SquadPoint->attr().homePositionOffsetFactor*radius(), 0);
		SquadPoint->setPose(poseShifted, initPose);
		SquadPoint->setHomePosition(poseShifted.trans());
	}
}

void terFrame::DestroyLink()
{
	terUnitReal::DestroyLink();

	for(int i = 0;i < FRAME_SLOTS_MAX;i++){
		if(frameSlots_[i].UnitPoint && !frameSlots_[i].UnitPoint->alive()){
			if(activeSlot_ == &frameSlots_[i]){
				activeSlot_ = 0;
				finishIO();
			}
			frameSlots_[i].init();
		}
	}
}

void terFrame::MoveQuant()
{
	if(isMoving() && movementConsumption_.attached()){
		movementConsumption_.requestCharge();
		if(!movementConsumption_.charged()){
			wayPoints_.clear();
		}
	}

	BodyPoint->setForwardVelocity(!underTeleportation() ? attr().normalVelocity 
		: attr().teleportationVelocity);

	terUnitReal::MoveQuant();
}

void terFrame::Quant()
{
	terUnitReal::Quant();

	average(SpeedFactor, BodyPoint->speedFactor(), 0.2f);

	for(int i = 0;i < FRAME_SLOTS_MAX;i++){
		frameSlots_[i].quant();
		if(frameSlots_[i].status_ == terFrameSlot::STATUS_PRODUCTION && productionSound_)
			productionSound_->play();
	}

	LogicObjectPoint->Update();

	UpdateLogicPoint();
	LogicObjectPoint->Update();

	if(RequestStatus == ATTACHED){
		if(ValidStatus != ATTACHED){
			if(basementReady()){
				BodyPoint->setFlyingMode(0);
				BodyPoint->makeDynamic();
				if(position().z + BodyPoint->boxMin().z - vMap.hZeroPlast < 2){
					ValidStatus = ATTACHED;
					BodyPoint->makeStatic();
					installBasement();
					SND3DPlaySound("frame_install",&position());
				}
			}
			else{
				RequestStatus = ValidStatus = ISOLATED;
				BodyPoint->makeDynamic();
				BodyPoint->setFlyingMode(1);
			}
		}
	}
	else{
		if(ValidStatus == ATTACHED){
			ValidStatus = ISOLATED;
			BodyPoint->makeDynamic();
			BodyPoint->setFlyingMode(1);
			uninstallBasement();
			
			Player->stopField();
			
			SND3DPlaySound("frame_uninstall",&position());
		}
		if(!isBuildingEnable())
			BodyPoint->setVelocity(Vect3f(0, 0, BodyPoint->velocity().z));
	}

	switch(dockStatus_){
	case DOCK_STATUS_NONE:
		if((RequestStatus == ATTACHED ? ValidStatus == ATTACHED : ValidStatus == MOBILE || ValidStatus == ISOLATED)
		  && !activeSlot_){
			for(int i = 0;i < FRAME_SLOTS_MAX;i++){
				if(frameSlots_[i].needInput()){
					dockStatus_ = DOCK_STATUS_INPUT_PREPARE;
					realAvatar()->requestChainNode(ValidStatus == ATTACHED ? CHAIN_NODE_ATTACHED_OPENED : CHAIN_NODE_DETACHED_OPENED);
					break;
				}
			}

			for(int i = 0;i < FRAME_SLOTS_MAX;i++)
				if(frameSlots_[i].needOutput()){
					activeSlot_ = &frameSlots_[i];
					dockStatus_ = DOCK_STATUS_OUTPUT_PREPARE;
					realAvatar()->requestChainNode(ValidStatus == ATTACHED ? CHAIN_NODE_ATTACHED : CHAIN_NODE_DETACHED);
					break;
				}
		}
		break;

	case DOCK_STATUS_OUTPUT_PREPARE:
		if(realAvatar()->requestDone()){
			terDockingSlot* slot = requestDockingSlot();
			if(slot){
				terFrameChild* unit = safe_cast<terFrameChild*>(Player->buildUnit(activeSlot_->ProductionID));
				unit->setPose(Se3f(QuatF::ID, slot->position().trans()), true);
				unit->setOrientation(orientation());
				unit->constructionStart();
				unit->Start();
				unit->SetFramePoint(this);
				unit->soundEvent(SOUND_VOICE_UNIT_READY);
				activeSlot_->UnitPoint = unit;
				activeSlot_->status_ = terFrameSlot::STATUS_BUSY;
				
				unit->SetDockingPoint(this, slot, DOCK_MODE_LOCK);
				dockStatus_ = DOCK_STATUS_OUTPUT;
				realAvatar()->requestChain(ValidStatus == ATTACHED ? CHAIN_ATTACHED_OUTPUT : CHAIN_DETACHED_OUTPUT);

				for(int i = 0; i < FRAME_SLOTS_MAX; i++)
					if(&frameSlots_[i] == activeSlot_)
						unit->setSlotNumber(i);
				unit->executeCommand(UnitCommand(COMMAND_ID_POINT, To3D(unit->homePosition()), 0));
			}
		}
		break;

	case DOCK_STATUS_OUTPUT:
		if(realAvatar()->requestDone()){
			terFrameChild* unit = activeSlot_->UnitPoint;
			unit->ConstructionComplete();
			unit->clearDockingPoint();
			activeSlot_ = 0;
			finishIO();
		}
		break;
		
	case DOCK_STATUS_INPUT_PREPARE:
		if(realAvatar()->requestDone()){
			terDockingSlot& slot = DockList.front();
			terUnitReal* unit = slot.UnitPoint;
			if(!unit){
				for(int i = 0;i < FRAME_SLOTS_MAX;i++){
					if(frameSlots_[i].needInput()){
						unit = frameSlots_[i].UnitPoint;
						safe_cast<terFrameChild*>(unit)->setDockSlot(&slot);
						break;
					}
				}
				if(!unit)
					finishIO();
			}
			else{
				if(unit->dockMode() == DOCK_MODE_LOCK){
					dockStatus_ = DOCK_STATUS_INPUT;
					realAvatar()->requestChain(ValidStatus == ATTACHED ? CHAIN_ATTACHED_INPUT : CHAIN_DETACHED_INPUT);
				}
			}
		}
		break;

	case DOCK_STATUS_INPUT:
		if(realAvatar()->requestDone()){
			terDockingSlot& slot = DockList.front();
			if(slot.UnitPoint){
				terFrameSlot& frameSlot = frameSlots_[safe_cast<terFrameChild*>(slot.UnitPoint)->slotNumber()];
				frameSlot.UnitPoint = 0;
				frameSlot.status_ = terFrameSlot::STATUS_COMPLETE;
				if(frameSlot.ProductionID == slot.UnitPoint->attr().ID){
					frameSlot.ProductionID = UNIT_ATTRIBUTE_NONE;
					finishIO();
				}
				else{
					activeSlot_ = &frameSlot;
					dockStatus_ = DOCK_STATUS_OUTPUT_PREPARE;
				}
				
				slot.UnitPoint->Kill();
				slot.UnitPoint = 0;
			}
		}
		break;
	}

	if(ValidStatus == ATTACHED){
		if(realAvatar()->chainID() == CHAIN_ATTACHED){
			if(!SoundAttachStatus){
				soundEvent(SOUND_VOICE_FRAME_INSTALLED);
				SoundAttachStatus = 1;
			}
		}
	}
	else{
		if(realAvatar()->chainID() == CHAIN_DETACHED){
			if(SoundAttachStatus){
				soundEvent(SOUND_VOICE_FRAME_DEINSTALLED);
				SoundAttachStatus = 0;
			}
		}
	}

	if(spiralConsumer_.attached()){
 		if(attached() && powered() && Player->energyData().accumulated() > attr().energyReserveToChargeSpiral)
			spiralConsumer_.requestCharge();

		if(spiralLevelPrev_ == spiralLevel() && spiralLevel() < 1.f)
			Player->soundEvent(SOUND_VOICE_SPIRAL_NOT_FULLY_CHARGED);
	}

	if(spiralLevel() >= 1.f){
		if(spiralLevelPrev_ < 1.f)
			Player->soundEvent(SOUND_VOICE_SPIRAL_CHARGED);
		if(!canTeleportate())
			Player->soundEvent(SOUND_VOICE_PORTAL_OUT_OF_RANGE);
	}

	float percent = 0.05f;
	if(fmodf(spiralLevelPrev_, percent) > percent/2 && fmodf(spiralLevel(), percent) < percent/2)
		Player->soundEvent(SOUND_VOICE_SPIRAL_CHARGING);

	spiralLevelPrev_ = spiralLevel();


	if(!attached() && !underTeleportation()){
		antigravConsumer_.requestCharge();
		if(!antigravConsumer_.charged())
			setDamage(attr().fallDamage, 0);
	}

	if(powered() && damageMolecula().needRepair()){
		repairConsumption_.requestCharge();
		if(repairConsumption_.charged())
			damageMoleculaRepair(attr().repairElementsPerQuant); 
	}

	if(ValidStatus == ISOLATED && powered())
		ValidStatus = MOBILE;
	if(ValidStatus == MOBILE && !powered())
		ValidStatus = ISOLATED;

	if(Player->frame() != this && !poweredTimer_){
		setPowered(false);
		universe()->changeOwner(this, universe()->worldPlayer());
	}
}

void terFrame::finishIO()
{
	realAvatar()->requestChain(ValidStatus == ATTACHED ? CHAIN_ATTACHED : CHAIN_DETACHED);
	dockStatus_ = DOCK_STATUS_NONE;
}

void terFrame::WayPointStart()
{
	terUnitReal::WayPointStart();

	if(attached()){
		realAvatar()->setChain(CHAIN_ATTACHED);
		if(animate_)
			animate_->setChain(CHAIN_SWITCHED_ON);
	
		BodyPoint->makeStatic();
		BodyPoint->setFlyingMode(0);
		installBasement();
	}
	else{
		BodyPoint->setFlyingMode(1);
		realAvatar()->setChain(CHAIN_DETACHED);
		if(animate_)
			animate_->setChain(CHAIN_SWITCHED_OFF);
	}

	if(powered()){
		pillar_->setChain(CHAIN_SWITCHED_ON);
		poweredTimer_.start(globalAttr().changePlayerDelay);
	}
	else
		pillar_->setChain(CHAIN_SWITCHED_OFF);
}

void terFrame::WayPointController()
{
	terUnitReal::WayPointController();

	BodyPoint->way_points.clear();
	if(!attached() && !wayPoints().empty()){
		if(BodyPoint->is_point_reached(wayPoints().front()))
			wayPoints_.erase(wayPoints_.begin());
		
		if(!wayPoints().empty())
			moveToPoint(wayPoints().front());
		if(wayPoints().size() > 1)
			BodyPoint->way_points.push_back(wayPoints()[1]);
	}
}

void terFrame::setPowered(bool powered) 
{ 
	if(powered && pillar_->chainID() != CHAIN_SWITCHED_ON && pillar_->requestedChainID() != CHAIN_SWITCHED_ON)
		pillar_->requestChain(CHAIN_SWITCHED_ON);
	if(!powered && pillar_->chainID() != CHAIN_SWITCHED_OFF && pillar_->requestedChainID() != CHAIN_SWITCHED_OFF)
		pillar_->requestChain(CHAIN_SWITCHED_OFF);

	powered_ = powered; 

	if(powered)
		poweredTimer_.start(globalAttr().changePlayerDelay);
}

void terFrame::restorePosition()
{
	//Vect3f pos = BodyPoint->prev_position + (BodyPoint->prev_position - BodyPoint->position)*0.3f;
	Se3f pose = BodyPoint->posePrev();
	pose.trans().z = position().z;
	terUnitGeneric::setPose(pose, false);
	//	BodyPoint->setPosition(MatXf(BodyPoint->rotation, pos));
	BodyPoint->setPose(pose);
}

void terFrame::attach()
{
	if(underTeleportation())
		return;
	RequestStatus = ATTACHED;
	realAvatar()->requestChain(CHAIN_ATTACHED);
	if(animate_)
		animate_->requestChain(CHAIN_SWITCHED_ON);
	wayPoints_.clear();
}

void terFrame::detach()
{
	RequestStatus = ISOLATED;
	realAvatar()->requestChain(CHAIN_DETACHED);
	if(animate_)
		animate_->requestChain(CHAIN_SWITCHED_OFF);
}

void terFrame::executeCommand(const UnitCommand& command)
{
	if(underTeleportation() || catched_)
		return;

	if(command.commandID() == COMMAND_ID_POINT && (attached() || !powered()))
		return;

	terUnitReal::executeCommand(command);

	switch(command.commandID()){
		case COMMAND_ID_FRAME_ALARM_ON: {
			for(int i = 0;i < FRAME_SLOTS_MAX;i++){
				if(frameSlots_[i].UnitPoint)
					frameSlots_[i].UnitPoint->setAlarm();
				soundEvent(SOUND_VOICE_FRAME_ALARM);
			}
			break; 
		}

		case COMMAND_ID_BUILDING_START:
			xassert(command.commandData() >= 0 && command.commandData() < UNIT_ATTRIBUTE_STRUCTURE_MAX);
			Player->BuildStructure((terUnitAttributeID)(command.commandData()), command.position());
			break;

		case COMMAND_ID_BUILD_MASTER_INC:
		case COMMAND_ID_TERRAIN_MASTER_INC: {
			terFrameSlot& slot = frameSlots_[command.commandData()];
			if(slot.status_ == terFrameSlot::STATUS_BUSY && slot.UnitPoint && slot.UnitPoint->attr().ID != (command.commandID() == COMMAND_ID_TERRAIN_MASTER_INC ? UNIT_ATTRIBUTE_TERRAIN_MASTER : UNIT_ATTRIBUTE_BUILD_MASTER))
				slot.UnitPoint->executeCommand(UnitCommand(COMMAND_ID_OBJECT, this, 0));
			if(slot.status_ == terFrameSlot::STATUS_FREE){
				slot.status_ = terFrameSlot::STATUS_PRODUCTION;
				slot.productionConsumption_.attach(Player, attr().productionConsumption);
				soundEvent(SOUND_VOICE_UNIT_PRODUCTION_STARTED);
			}
			slot.ProductionID = command.commandID() == COMMAND_ID_TERRAIN_MASTER_INC ? UNIT_ATTRIBUTE_TERRAIN_MASTER : UNIT_ATTRIBUTE_BUILD_MASTER;
			break; }

		case COMMAND_ID_FRAME_ATTACH:
			if(basementReady())
				attach();
			break; 

		case COMMAND_ID_FRAME_DETACH:
			detach();
			break;

		case COMMAND_ID_FIELD_START:
			Player->startField();
			soundEvent(SOUND_VOICE_PSHIELD_GLOBAL_ACTIVE);
			break;
		case COMMAND_ID_FIELD_STOP:
			Player->stopField();
			soundEvent(SOUND_VOICE_PSHIELD_GLOBAL_OFF);
			break;

		case COMMAND_ID_STOP: 
			break; 

		case COMMAND_ID_TELEPORTATE:
			if(canTeleportate() && !underTeleportation_){
				underTeleportation_ = true;
				detach();
				safe_cast<terCorridor*>(universe()->findCorridor())->input(this);
				movementConsumption_.detach();
				soundEvent(SOUND_VOICE_FRAME_TELEPORTATION);
			}
			break;
	}
}

void terFrame::MapUpdateHit(float x0,float y0,float x1,float y1)
{
	if((ValidStatus == ATTACHED) && basementDamage() < 0.9f){
		RequestStatus = ISOLATED;
	}
}

//-----------------------------------------------------
void terFrame::AvatarQuant()
{
	terUnitReal::AvatarQuant();

	if(SightFactor < 0.001f)
		avatar()->Hide();
	else{
		avatar()->Show();
		//realAvatar()->setSight(SightFactor);

		avatar()->setPose(BodyPoint->pose());
	}

	if(MoveSoundPoint){
		MoveSoundPoint->setVolume(SpeedFactor);
		MoveSoundPoint->setFrequency(SpeedFactor);
		MoveSoundPoint->play();
	}

//	ShowEnergy();
}

void terFrame::Kill()
{
	terUnitReal::Kill();

	uninstallBasement();

	freeZeroLayer();

	installer_.Clear();

	for(int i = 0;i < FRAME_SLOTS_MAX;i++)
		if(frameSlots_[i].UnitPoint)
			frameSlots_[i].UnitPoint->Kill();

	SquadPoint->Kill();

	if(Player->frame() == this)
		Player->clearFrame();
}

void terFrame::ChangeUnitOwner(terPlayer* player)
{
	terUnitReal::ChangeUnitOwner(player);
	catched_ = true;

	antigravConsumer_.detach();
	movementConsumption_.detach();
	repairConsumption_.detach();

	antigravConsumer_.attach(Player, attr().antigravConsumption);
	movementConsumption_.attach(Player, attr().movementConsumption);
	repairConsumption_.attach(Player, attr().repairConsumption);
}

//------------------------------------------------
SaveUnitData* terFrame::universalSave(SaveUnitData* baseData)
{
	SaveUnitFrameData* data = castOrCreate<SaveUnitFrameData>(baseData);
	terUnitReal::universalSave(data);

	data->attached = attached();
	data->attaching = RequestStatus == ATTACHED && !attached();
	data->powered = powered();
	data->spiralLevel = spiralLevel();
	data->squad = SquadPoint->universalSave(0);

	for(int i = 0; i < FRAME_SLOTS_MAX; i++){
		terFrameSlot& slot = frameSlots_[i];

		if(slot.UnitPoint && slot.UnitPoint->isDocked()){
			data->frameSlots[i] = 0;
			data->slotsData[i].productionID = slot.UnitPoint->attr().ID != slot.ProductionID ? slot.ProductionID : UNIT_ATTRIBUTE_NONE;
			data->slotsData[i].status = terFrameSlot::STATUS_COMPLETE;
			data->slotsData[i].progress = 1;
		}
		else{
			data->frameSlots[i] = slot.UnitPoint ? slot.UnitPoint->universalSave(0) : 0;
			data->slotsData[i].productionID = slot.ProductionID;
			data->slotsData[i].status = slot.status_;
			data->slotsData[i].progress = slot.productionConsumption_.progress();
		}
	}

	data->belligerent = nativeBelligerent_;
	data->catched = catched_;
	
	return data;
}

void terFrame::universalLoad(const SaveUnitData* baseData)
{
	const SaveUnitFrameData* data = safe_cast<const SaveUnitFrameData*>(baseData);
	terUnitReal::universalLoad(data);
	
	spiralConsumer_.setProgress(data->spiralLevel);

	spiralLevelPrev_ = spiralLevel();

	powered_ = data->powered;
	catched_ = data->catched;

	if(data->attached)
		RequestStatus = ValidStatus = ATTACHED;
	else if(data->powered)
		RequestStatus = ValidStatus = MOBILE;
	else
		RequestStatus = ValidStatus = ISOLATED;

	if(data->attaching)
		RequestStatus = ATTACHED;
	
	if(data->squad){
		SquadPoint->universalLoad(data->squad);
		SquadPoint->Start();
	}

	for(int i = 0; i < FRAME_SLOTS_MAX; i++){
		SaveUnitData* childData = data->frameSlots[i];
		terFrameSlot& slot = frameSlots_[i];
		if(childData){
			terFrameChild* unit = safe_cast<terFrameChild*>(Player->buildUnit(childData->attributeID));
			unit->universalLoad(childData);
			unit->Start();
			unit->SetFramePoint(this);
			unit->setSlotNumber(i);

			slot.status_ = terFrameSlot::STATUS_BUSY;
			slot.UnitPoint = unit;
			slot.ProductionID = unit->attr().ID;
		}
		
		const SaveFrameSlotData& slotData = data->slotsData[i];
		if(slotData.status != -1){
			slot.status_ = (terFrameSlot::Status)slotData.status;
			slot.ProductionID = slotData.productionID;
			if(slot.status_ == terFrameSlot::STATUS_PRODUCTION)
				slot.productionConsumption_.attach(Player, attr().productionConsumption);
			slot.productionConsumption_.setProgress(slotData.progress);
		}
	}

	//if(!accumulatedEnergy())
	//	addAccumulatedEnergy(attr().accumulatedEnergyInitial*attr().energyCapacity);
}

//---------------------------------------------

void terFrame::GetInterfaceFrameProduction(int* phase,int* id,terUnitBase* unit[FRAME_SLOTS_MAX])
{
	for(int i = 0;i < FRAME_SLOTS_MAX;i++){
		switch(frameSlots_[i].status_){
			case terFrameSlot::STATUS_FREE:
				phase[i] = 0;
				id[i] = frameSlots_[i].ProductionID;
				unit[i] = 0;
				break;
			case terFrameSlot::STATUS_PRODUCTION:
				phase[i] = frameSlots_[i].productionProgress();
				id[i] = frameSlots_[i].ProductionID;
				unit[i] = 0;
				break;
			case terFrameSlot::STATUS_COMPLETE:
				phase[i] = 100;
				id[i] = frameSlots_[i].ProductionID;
				unit[i] = 0;
				break;
			case terFrameSlot::STATUS_BUSY:
				phase[i] = 100;
				xassert(frameSlots_[i].UnitPoint);
				id[i] = frameSlots_[i].UnitPoint->attr().ID;
				unit[i] = frameSlots_[i].UnitPoint;
				break;
		}
	}

	switch(lastCommand()){
	case COMMAND_ID_BUILD_MASTER_INC:
	case COMMAND_ID_TERRAIN_MASTER_INC:{
		const UnitCommand& command = *findCommand(lastCommand());
		if(frameSlots_[command.commandData()].status_ == terFrameSlot::STATUS_FREE){
			phase[command.commandData()] = 1;
			id[command.commandData()] = command.commandID() - COMMAND_ID_BUILD_MASTER_INC;
		}
		else{
			if(frameSlots_[command.commandData()].status_== terFrameSlot::STATUS_COMPLETE)
				id[command.commandData()] = command.commandID() - COMMAND_ID_BUILD_MASTER_INC;
		}
		break; }
	}
}

//---------------------------------------
int terFrame::GetInterfaceLegionMode()
{
	int stop = 0;
	int move = 0;
	switch(lastCommand()){
	case COMMAND_ID_STOP:
		stop = 1;
		break;
	case COMMAND_ID_POINT:
		move = 1;
		break;
	}

	if(stop)
		return INTERFACE_LEGION_MODE_STOP;
	else{
		if(move)
			return INTERFACE_LEGION_MODE_MOVE;
	}
	return INTERFACE_LEGION_MODE_STOP;
}

void terFrame::showDebugInfo()
{
	terUnitReal::showDebugInfo();
	
	if(showDebugFrame.status){
		string str;
		if(isBuildingEnable())
			str += "En ";

		if(realAvatar()->requestDone())
			str += "RequestDone ";

		switch(ValidStatus){
		case ISOLATED:
			str += "ISOLATED ";
			break;
		case MOBILE:
			str += "MOBILE ";
			break;
		case ATTACHED:
			str += "ATTACHED ";
			break;
		}

		switch(dockStatus_){
		case DOCK_STATUS_NONE:
			str += "DOCK_STATUS_NONE ";
			break;
		case DOCK_STATUS_INPUT_PREPARE:
			str += "DOCK_STATUS_INPUT_PREPARE ";
			break;
		case DOCK_STATUS_INPUT:
			str += "DOCK_STATUS_INPUT ";
			break;
		case DOCK_STATUS_OUTPUT_PREPARE:
			str += "DOCK_STATUS_OUTPUT_PREPARE ";
			break;
		case DOCK_STATUS_OUTPUT:
			str += "DOCK_STATUS_OUTPUT ";
			break;
		}

		show_text(position(), str.c_str(), RED);
	}

	if(showDebugFrame.inputPosition)
		show_vector(To3D(inputPosition()), 3, MAGENTA);
}

void terFrame::ShowInfo()
{
	terUnitReal::ShowInfo();

	if(Player->active() && !attached() && selected())
		installer_.SetBuildPosition(position(), angleZ(), Player);
	else
		installer_.setVisible(false);
	installer_.UpdateInfo(terCamera->GetCamera());
}

//-----------------------------------------------------------

bool terFrame::canTeleportate() const
{
	if(spiralLevel() < 1)
		return false;

	terCorridor* corridor = safe_cast<terCorridor*>(universe()->findCorridor());
	if(corridor && corridor->readyToTeleportate() && corridor->position2D().distance2(position2D()) 
		< sqr(corridor->attr().ID == UNIT_ATTRIBUTE_CORRIDOR_ALPHA ? gameShell->manualData().alphaActivationDistance : gameShell->manualData().omegaActivationDistance))
		return true;

	return false;
}

bool terFrame::analyzeTerrain()
{
	int D = clamp(round(Vect2f(BodyPoint->boxMax()).norm()) >> kmGrid, 1, 1000);
	int x0 = round(position().x) >> kmGrid;
	int y0 = round(position().y) >> kmGrid;
	int z0 = vMap.hZeroPlast;
	int dzMax = attr().heightMax;
	for(int y = -D; y <= D; y++)
		for(int x = -D; x <= D; x++){
			if(abs(vMap.GVBuf[vMap.offsetGBufC(x + x0, y + y0)] - z0) > dzMax)
				return false;
		}

	return true;
}

void terFrame::setDamage(const DamageData& damage, terUnitBase* p)
{
	if(!underTeleportation())
		terUnitReal::setDamage(damage,p);
}

bool terFrame::readyToConnect() const
{
	return pillar_->chainID() == CHAIN_SWITCHED_ON && pillar_->requestDone();
}

//-----------------------------------------------
terFrameSlot::terFrameSlot()
{ 
	init(); 
	status_ = STATUS_COMPLETE; 
}

void terFrameSlot::init()
{ 
	status_ = STATUS_FREE; 
	UnitPoint = 0; 
	ProductionID = UNIT_ATTRIBUTE_NONE; 
}

void terFrameSlot::quant()
{																																		
	if(status_ == STATUS_PRODUCTION){
		productionConsumption_.requestCharge();
		if(productionConsumption_.charged())
			status_ = STATUS_COMPLETE;
	}
}

