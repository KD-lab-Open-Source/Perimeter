#include "StdAfx.h"

#include "GenericControls.h"
#include "Universe.h"
#include "Interpolation.h"

#include "PerimeterSound.h"
#include "GameShell.h"
#include "Triggers.h"
#include "Config.h"

terUnitBase::terUnitBase(const UnitTemplate& data) 
{
	attr_.setKey(AttributeIDBelligerent(data.attribute()->ID, data.attribute()->belligerent));

	Player = data.player();
	alive_ = true;
	dead_ = false;

	radius_ = 1;
	
	damageMolecula_ = attr().damageMolecula;

	avatar_ = terCreateUnitInterpolation(attr().InterpolationType, this);

	setPose(Se3f::ID, false);

	RealCollisionCount = 0;
	MapUpdatedCount = 0;

	collisionGroup_ = attr().CollisionGroup;

	possibleDamage_ = 0;

	repairRequested_ = 0;

	selected_ = marked_ = false; 
	unitClass_ = attr().UnitClass;

	includingCluster_ = 0;
}

terUnitBase::~terUnitBase()
{
	delete avatar_;
}

void terUnitBase::Kill()
{
	MTL();
	alive_ = false;
}

void terUnitBase::ShowCircles() 
{
	float r=radius() < 5.0f ? 5.0f : radius();
	CircleColor color(circleColors.selectRadius);
	float health = life();
	color.color[2] = 0;
	if (health > 0.5f) {
		color.color[0] = 510.0f * (1.0f - health);
		color.color[1] = 255;
	} else {
		color.color[0] = 255;
		color.color[1] = 510.0f * health;
	}
	if(avatar())
	{
		terCircleShow(avatar()->pos0(),avatar()->pos1(),r,color);
	}else
	{
		terCircleShow(position(),position(),r,color);
	}
}

bool terUnitBase::selectAble() const 
{ 
	return gameShell->missionEditor() || attr().SelectAble; 
}

bool terUnitBase::isEnemy(const terUnitBase* unit) const 
{ 
	return Player->clan() != unit->Player->clan() ? 
		(!unit->Player->isWorld() || unit->attr().enemyWorld) :
	(Player->isWorld() && attr().enemyWorld && !unit->attr().enemyWorld);
}

bool terUnitBase::isHarmful(const terUnitBase* unit) const 
{
	//return Player != unit->Player && unit->alive() && (collisionGroup() & unit->collisionGroup()) && includingCluster() == unit->includingCluster();
	return unit->alive() && unit->isEnemy(this) && includingCluster() == unit->includingCluster();
}

void terUnitBase::setDamage(const DamageData& damage,terUnitBase* agressor)
{
	int shield = 0;
	bool wasAlive = damageMolecula_.isAlive();
	damageMolecula_.hit(damage.width,damage.power,damage.attackFilter,damage.damageFilter,shield);
	if(wasAlive && !damageMolecula_.isAlive())
		universe()->checkEvent(EventUnitMyUnitEnemy(Event::KILL_OBJECT, this, agressor));

	if(damageMolecula_.isAlive()){
		if(attr().isBuilding())
			soundEvent(SOUND_VOICE_BUILDING_UNDER_ATTACK);
		else
			soundEvent(SOUND_VOICE_UNIT_UNDER_ATTACK);
	}

	universe()->checkEvent(EventUnitMyUnitEnemy(Event::ATTACK_OBJECT, this, agressor));
}

void terUnitBase::setPose(const Se3f& pose, bool initPose) 
{
	pose_.trans().x = clamp(pose.trans().x, radius(), (vMap.H_SIZE - 1) - radius());
	pose_.trans().y = clamp(pose.trans().y, radius(), (vMap.V_SIZE - 1) - radius());
	pose_.trans().z = pose.trans().z;
	pose_.rot() = pose.rot();

	updateIncludingCluster();

	log_var(attr().internalName());
	log_var(pose);
}

void terUnitBase::updateIncludingCluster()
{
	includingCluster_ = field_dispatcher->getIncludingCluster(position());
}

void terUnitBase::ChangeUnitOwner(terPlayer* player)
{
	Player->removeUnit(this);
	player->addUnit(this);

	UpdateSkinColor();
}

void terUnitBase::DestroyLink()
{
	CommandList::iterator i_command = commandList().begin();
	while(i_command != commandList().end()){
		if(i_command->unit() && !(i_command->unit()->alive()))
			i_command = commandList().erase(i_command);
		else
			++i_command;
	}
}

float terUnitBase::angleZ() const 
{ 
	if(orientation().z() > 0)
		return orientation().angle(); 
	else{
		QuatF q = orientation();
		q.negate();
		return q.angle(); 
	}
} 

void terUnitBase::damageMoleculaKill()
{
	damageMolecula_.kill();
}

//---------------------------------------------------------------------------
void terUnitBase::AvatarQuant() 
{ 
	if(avatar()) 
		avatar()->quant(); 
}

void terUnitBase::AvatarInterpolation()
{
	if(avatar()) 
		avatar()->interpolate();
//	if(gameShell->interfaceShowFlag())
//		Show();
}

void terUnitBase::UpdateSkinColor() 
{ 
	if(avatar()) 
		avatar()->UpdateSkinColor(); 
}

//----------------------------------------------------------------
UnitCommand::UnitCommand(CommandID commandID, int commandData, CommandSelectionMode selectionMode)
{
	selectionMode_ = selectionMode;
	commandID_ = commandID;
	commandData_ = commandData;
	position_ = Vect3f::ZERO;
	unit_ = 0;
}

UnitCommand::UnitCommand(CommandID commandID, const Vect3f& position, int commandData, CommandSelectionMode selectionMode)
{
	selectionMode_ = selectionMode;
	commandID_ = commandID;
	commandData_ = commandData;
	position_ = position;
	unit_ = 0;
}

UnitCommand::UnitCommand(CommandID commandID, terUnitBase* unit, int commandData, CommandSelectionMode selectionMode)
{
	selectionMode_ = selectionMode;
	commandID_ = commandID;
	commandData_ = commandData;
	position_ = Vect3f::ZERO;
	unit_ = unit;
}

void UnitCommand::prepareToSend()
{
	if(unit()) {
		//unitID_ = *unit();
		unitID_ = static_cast<terUnitID>(*unit());
	}
}

void UnitCommand::prepareAfterSend()
{
	if(!(unitID_ == terUnitID())){
		unit_ = universe()->findUnit(unitID_);
		if(unit_ && !unit_->alive())
			unit_ = 0;
	}
}

void terUnitBase::commandIncoming(const UnitCommand& command)
{
	if(!commandList().empty() && commandList().front() == command)
		commandList().pop_front();
	if(command.commandID() == COMMAND_ID_OBJECT && !command.unit())
		return;
	executeCommand(command);
}

void terUnitBase::commandOutcoming(const UnitCommand& commandIn)
{
	MTG();
	if ( !Player->shouldIgnoreIntfCommands() ) {
		commandOutcomingLogic(commandIn);
	}
}

void terUnitBase::commandOutcomingLogic(const UnitCommand& commandIn)
{
	if(Player->controlEnabled()) {
		if(!universe()->multiPlayer()){
			//executeCommand(commandIn);
			UnitCommand command = commandIn;
			command.prepareToSend();
			universe()->sendCommand(netCommand4G_UnitCommand(*this, command));
		}
		else{
			UnitCommand command = commandIn;
			command.prepareToSend();
			commandList().push_back(command);
			universe()->sendCommand(netCommand4G_UnitCommand(*this, command));
		}
	}
}

void terUnitBase::executeCommand(const UnitCommand& command)
{
	MTL();
	switch(command.commandID()){
	case COMMAND_ID_EXPLODE_UNIT: {
		DamageData damage;
		damage.clear();
		if(attr().ID != UNIT_ATTRIBUTE_SQUAD){
			damage.width = damageMolecula().elementCount();
			damage.power = damage.width*2;
		}
		else{
			damage.width = 2;
			damage.power = 4;
		}

		setDamage(damage, NULL);
		break; }
	}
}

const UnitCommand* terUnitBase::findCommand(CommandID commandID) const
{
	for(CommandList::const_reverse_iterator rci = commandList_.rbegin(); rci != commandList_.rend(); ++rci)
		if(rci->commandID() == commandID)
			return &*rci;
	return 0;
}

bool terUnitBase::soundEvent(SoundEventID event_id)
{
	if(attr().hasSoundSetup()){
		if(const SoundEventSetup* ev = attr().soundSetup.findEvent(event_id))
			return soundEvent(ev);
	}

	return Player->soundEvent(event_id);
}

bool terUnitBase::soundEvent(const SoundEventSetup* ev) const
{
	if(!ev->isVoice){
		if(ev->activePlayer && !Player->active())
			return false;

		if(ev->is3D)
			return SND3DPlaySound(ev->name,&position());
		else
			return SND2DPlaySound(ev->name);
	}
	else {
		if(Player->active())
			return Player->soundEvent(ev);
	}

	return false;
}

//--------------------------------------------------
SaveUnitData* terUnitBase::universalSave(SaveUnitData* data)
{
	if(!data)
		data = new SaveUnitData;
	data->attributeID = attr().ID;
	data->position = position();
	data->orientaion = orientation();
	data->label = label_;
	data->damageMolecula = damageMolecula();
	data->unitID = unitID();
	return data;
}

void terUnitBase::universalLoad(const SaveUnitData* data)
{
	setPose(Se3f(data->orientaion, data->position), true);
	setPose(Se3f(data->orientaion, data->position), false);
	label_ = data->label;
	damageMolecula_ = data->damageMolecula;
	if(data->unitID)
		(terUnitID&)*this = terUnitID(Player->registerUnitID(data->unitID), playerID());
}

void terUnitBase::showDebugInfo()
{
	if(show_unit_base_radius){
		show_vector(position(), 1, GREEN);
		show_vector(position(), radius(), GREEN);
	}
	if(show_node_controller_status && avatar())
		avatar()->showDebugInfo();

	if(showIncludingCluster){
		XBuffer msg;
		msg < "Cluster: " <= includingCluster();
		show_text(position(), msg, GREEN);
	}
}

void terUnitBase::DeleteInterpolator()
{
	if(avatar_)
	{
		delete avatar_;
		avatar_=NULL;
	}
}

void terUnitBase::setFieldDamage(const DamageData& damage)
{
	setDamage(damage, 0);
}

/////////////////////////////////////////////////////
#include "EditArchive.h"
#include "XPrmArchive.h"

/////////////////////////////////////////////////////
//REGISTER_CLASS(PropertyBase, PropertyBase, "PropertyBase");

/////////////////////////////////////////////////////
//REGISTER_CLASS(terUnitBase, terUnitBase, "terUnitBase");
