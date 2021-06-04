#include "StdAfx.h"

#include "Runtime.h"
#include "CameraManager.h"
#include "Universe.h"
#include "SecondLegion.h"

#include "Config.h"

#include "GridTools.h"
#include "UniverseInterface.h"

#include "Mutation.h"
#include "Squad.h"

#include "PerimeterSound.h"
#include "GameShell.h"
#include "AIPrm.h"
#include "AIMain.h"

terUnitSquad::terUnitSquad(const UnitTemplate& data)
:terUnitBase(data),
position_generator(false)
{
	//	average_position = stable_position = Vect2f(*p->GetPosition());
	average_position = homePosition_ = Vect2f::ZERO;
	prev_forward_direction = Vect2f::ZERO;
	average_velocity = Vect2f::ZERO;
	average_position_offset = Vect2f::ZERO;
	
	setRadius(squad_described_radius_min);
	setStablePosition(Vect2f::ZERO);
	setStableOrientation(Mat2f::ID);
	stablePoseRestarted_ = true;

	unitsWayPoinsSize_ = 0;
	
	including_cluster = 0;
	
	check_readiness_to_move = false;
	
	currentMutation_ = UNIT_ATTRIBUTE_NONE;

	n_base_units = n_complex_units = 0;
	
	atomsRequested_ = atomsPaused_ = atomsProgress_ = DamageMolecula();
	
	radius_ = 30.0f;

	lastChargeLevel = 0;

	commander_ = 0;

	squadMutationMolecula_ = squadMolecula_ = DamageMolecula();

	offensiveMode_ = false;

	mutationEnergy_ = 0;

	patrolIndex_ = 0;

	flagModel_ = 0;
	flagModelPhase_ = 0;
	
	flagModel_ = terScene->CreateObject(terModelWayFlag, 0);
	float scale = flagModel_->GetBoundRadius();
	flagModel_->SetScale(Vect3f(terModelWayFlagRadius/scale, terModelWayFlagRadius/scale, terModelWayFlagRadius/scale));
	flagModel_->SetAttr(ATTRUNKOBJ_IGNORE);
	UpdateSkinColor();

	attackAction_ = 0;
	orderUnitsAction_ = 0;

	optimalRadius_ = 0;

	squadToFollow_ = 0;
}

terUnitSquad::~terUnitSquad()
{
	if(flagModel_)
		flagModel_->Release();
}

void terUnitSquad::Start()
{
	terUnitBase::Start();

	terUnitBase::Start();
	setPose(Se3f(QuatF(angleZ(), Vect3f::K), position()), true);

	AvatarQuant();
	avatar()->Start();
}

void terUnitSquad::setPose(const Se3f& pose, bool initPose)
{
	terUnitBase::setPose(pose, initPose);
	if(initPose)
		setStablePosition(position());
	average_position = position(); 
	setStableOrientation(Mat2f(angleZ()));
}

void terUnitSquad::addUnit(terUnitLegionary* unit, bool set_position)
{
	unit->SetSquadPoint(this);
	
	if(unit->attr().is_base_unit){
		Units.push_back(unit);
		++n_base_units;
	}
	else{
		Units.push_front(unit);
		++n_complex_units;
	}

	if(!unit->attr().is_base_unit)
		unit->setInSquad();

	if(!unit->attr().is_base_unit || isBase() || set_position){
		Vect2f localPos = position_generator.get(unit->formationRadius());
		unit->setLocalPosition(localPos);
		Vect2f globalPos = stablePose()*localPos;
		if(set_position){
			unit->setPose(Se3f(QuatF(angleZ() + M_PI, Vect3f::K), To3D(globalPos)), true);
			unit->setInSquad();
		}
		else
			unit->addWayPoint(globalPos);
	}
	else{
		unit->setLocalPosition(Vect2f::ZERO);
		unit->addWayPoint(stablePose().trans);
	}

	DamageMolecula atom(unit->attr().damageMolecula);
	atom += unit->transportAtom();
	squadMolecula_ += atom;
	mutationEnergy_ += atom.elementCount();
}

void terUnitSquad::removeUnit(terUnitLegionary* unit)
{
	DamageMolecula atom(unit->attr().damageMolecula);
	atom += unit->transportAtom();
	squadMolecula_ -= atom;
	mutationEnergy_ -= atom.elementCount();
	if(unit->inSquad())
		squadMutationMolecula_ -= atom;

	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui)
		if(*ui == unit) {
			terUnitLegionary& unit = **ui;
			if(unit.attr().is_base_unit)
				--n_base_units;
			else
				--n_complex_units;
			unit.SetSquadPoint(0);
			Units.erase(ui);
			return;
		}
}

void terUnitSquad::Kill()
{
	mutation_process.clear();

	while(!Units.empty()) 
		Units.front()->Kill();

	terUnitBase::Kill();
}

void terUnitSquad::UpdateSkinColor()
{
	terUnitBase::UpdateSkinColor();

	flagModel_->SetSkinColor(Player->unitColor());
}

bool terUnitSquad::isTransport() const 
{
	return currentMutation() != UNIT_ATTRIBUTE_NONE;
}

void terUnitSquad::MoveQuant()
{
	terUnitBase::MoveQuant();

	calcCenter();

	if(Empty())
		return;
	
	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui)
		(*ui)->quant();

	attackQuant();
	correctSpeed();

	setPosition(To3D(average_position));
}

void terUnitSquad::Quant()
{
	start_timer_auto(squadQuant, STATISTICS_GROUP_LOGIC);

	terUnitBase::Quant();

	SupportQuant();

	followQuant();

	if(currentMutation() != UNIT_ATTRIBUTE_NONE && !Units.empty() && Units.front()->attr().ID != currentMutation() && mutation_process.finished()){
		mutate(UNIT_ATTRIBUTE_NONE);
		goHome();
	}

	squadMolecula_ = squadMutationMolecula_ = DamageMolecula();

	float charge = 0;
	int count = 0;
	bool disable_offencive_mode = true;

	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui){
		squadMolecula_ += (*ui)->damageMolecula();
		squadMolecula_ += (*ui)->transportAtom();

		if((*ui)->inSquad()){
			squadMutationMolecula_ += (*ui)->damageMolecula();
			squadMutationMolecula_ += (*ui)->transportAtom();
		
			charge += (*ui)->chargeLevel();

			if((*ui)->attr().ID != UNIT_ATTRIBUTE_TECHNIC)
				disable_offencive_mode = false;

			count++;
		}
	}

	if(disable_offencive_mode && !Units.empty())
		offensiveMode_ = false;

	lastChargeLevel = (count) ? charge / float(count) : 0;

	disintegrateUnitsQuant();

	if(Player->frame() && Player->frame()->isBuildingEnable() && commander() && commander()->isBuildingEnable() && squadMolecula().elementCount() > 0){
		if(mutationFinished()){
			if(mutationEnergy_ < squadMolecula().elementCount()){
				if(mutationChargeConsumption_.charged())
					mutationEnergy_ += 1;
				if(!mutationChargeConsumption_.attached()){
					ConsumptionData data = attr().productionConsumption;
					float factor = squadMolecula().elementCount();
					int time = round((float)data.time/factor);
					if(time < 200){
						time = 200;
						factor = (float)data.time/time;
					}
					data.time = time;
					data.energy *= factor;
					mutationChargeConsumption_.attach(Player, data);
				}
				mutationChargeConsumption_.requestCharge();
			}
			else
				mutationEnergy_ = squadMolecula().elementCount();
		}
	}
	else
		mutationEnergy_ = 0;


//	if(currentMutation() != UNIT_ATTRIBUTE_NONE && !Player->GetMutationElement(currentMutation()).Enabled)
//		mutate(UNIT_ATTRIBUTE_NONE);

	if(!mutation_process.finished())
		mutation_process.logic_quant();

//	if(!mutation_process.finished())
//		mutation_process.show_quant();
}

void terUnitSquad::AvatarInterpolation()
{
	terUnitBase::AvatarInterpolation();

	if(!mutation_process.finished())
		mutation_process.show_quant();
}

void terUnitSquad::DestroyLink()
{
	terUnitBase::DestroyLink();

	if(commander() && !commander()->alive())
		commander_ = 0;

	if(squadToFollow_ && !squadToFollow_->alive())
		squadToFollow_ = 0;

	AttackPointList::iterator ai;
	FOR_EACH(attack_points, ai)
		if(ai->obsolete()){
			ai = attack_points.erase(ai);
			--ai;
		}
		
	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui){
		terUnitLegionary& unit = **ui;
		if(!unit.alive()){
			if(unit.attr().is_base_unit)
				--n_base_units;
			else
				--n_complex_units;
			ui = Units.erase(ui);
			--ui;
			if(!mutation_process.finished())
				mutation_process.removeSourceOrTarget(unit.GetRigidBodyPoint());
		}
	}
}

void terUnitSquad::clearOrders()													   
{
	clearWayPoints();													 	  
	clearTargets();
	patrolPoints_.clear();
	patrolIndex_ = 0;	
	setSquadToFollow(0);
	unitsWayPoinsSize_ = 0;
}

void terUnitSquad::goHome()
{
	clearOrders();
	repositionFormation(true);
	addWayPoint(homePosition_);
}

void terUnitSquad::setSquadToFollow(terUnitBase* squadToFollow) 
{ 
	squadToFollow_ = squadToFollow ? (squadToFollow->attr().ID == UNIT_ATTRIBUTE_SQUAD ? squadToFollow : squadToFollow->GetSquadPoint()) : 0; 
}

void terUnitSquad::followQuant()
{
	if(!squadToFollow_ || Empty())
		return;

	Vect2f v0 = position2D();
	Vect2f v1 = squadToFollow_->position2D();
	float dist = v0.distance(v1);
	float radiusRequired = (radius() + squadToFollow_->radius())*attr().followDistanceFactor;
	if(dist > radiusRequired){
		clearWayPoints();
		addWayPoint(v1 + (v0 - v1)*(radiusRequired/dist));
	}
}

void terUnitSquad::executeCommand(const UnitCommand& command)
{
	terUnitBase::executeCommand(command);
																		 				   
	switch(command.commandID()){
	case COMMAND_ID_OBJECT:
		if(command.selectionMode() == COMMAND_SELECTED_MODE_SINGLE)
			clearOrders();
		if(command.unit())
			if(isEnemy(command.unit())){
				addTarget(command.unit());									  
				if(command.selectionMode() == COMMAND_SELECTED_MODE_SINGLE)
					soundEvent(SOUND_VOICE_SQUAD_ATTACKS);
			}
			else {
				if(dynamic_cast<terUnitLegionary*>(command.unit()))
					setSquadToFollow(command.unit());
				else if(command.unit()->Player->isWorld())
					addTarget(command.unit());
				else
					addWayPoint(command.unit()->position());
			}
		break;
																			 
	case COMMAND_ID_PATROL:
		if(patrolPoints_.empty() || (patrolIndex_ == 1 && patrolPoints_.empty()))
			addWayPoint(command.position());
		patrolPoints_.push_back(command.position());
		break;															  
	case COMMAND_ID_ATTACK:
		if(command.selectionMode() == COMMAND_SELECTED_MODE_SINGLE){
			clearOrders();
			soundEvent(SOUND_VOICE_SQUAD_ATTACKS);
		}
		addTarget(command.position());
		break;																			  
	case COMMAND_ID_POINT:	
		if(command.selectionMode() == COMMAND_SELECTED_MODE_SINGLE){
			clearOrders();					
			soundEvent(SOUND_VOICE_SQUAD_MOVES);
		}
		addWayPoint(command.position());
		break;

	case COMMAND_ID_RETURN_TO_BASE:
		goHome();
		break;

	case COMMAND_ID_UNIT_OFFENSIVE:										  
		offensiveMode_ = command.commandData();
		break;

	case COMMAND_ID_UNIT_MORPHING:
		if(mutationFinished() && mutate((terUnitAttributeID)command.commandData()))
			soundEvent(SOUND_VOICE_SQUAD_TRANSFORM_STARTED);
		break;

	case COMMAND_ID_STOP: 
		clearOrders();
		fireStop();
		addWayPoint(position2D());
		clearOrders();
		break; 

	case COMMAND_ID_BASIC_UNIT_DESTROY://commandData - AtomIndex
		killBaseUnit(command);
		break;
		
	case COMMAND_ID_PRODUCTION_INC://commandData - AtomIndex
	case COMMAND_ID_PRODUCTION_INC_10://commandData - AtomIndex
		xassert(command.commandData() >= 0 && command.commandData() < MUTATION_ATOM_MAX);
		atomsRequested_[command.commandData()] += command.commandID() == COMMAND_ID_PRODUCTION_INC ? 1 : 10;
		if(atomsRequested_[command.commandData()] > globalAttr().baseUnitsMax)
			atomsRequested_[command.commandData()] = globalAttr().baseUnitsMax;
		break;
	case COMMAND_ID_PRODUCTION_DEC://commandData - AtomIndex
	case COMMAND_ID_PRODUCTION_DEC_10://commandData - AtomIndex
		xassert(command.commandData() >= 0 && command.commandData() < MUTATION_ATOM_MAX);
		atomsRequested_[command.commandData()] -= command.commandID() == COMMAND_ID_PRODUCTION_DEC ? 1 : 10;
		if(atomsRequested_[command.commandData()] < 0)
			atomsRequested_[command.commandData()] = 0;
		break;
	case COMMAND_ID_PRODUCTION_PAUSE_ON://commandData - AtomIndex
		xassert(command.commandData() >= 0 && command.commandData() < MUTATION_ATOM_MAX);
		atomsPaused_[command.commandData()] = 1;
		break;
	case COMMAND_ID_PRODUCTION_PAUSE_OFF://commandData - AtomIndex
		xassert(command.commandData() >= 0 && command.commandData() < MUTATION_ATOM_MAX);
		atomsPaused_[command.commandData()] = 0;
		break;

	case COMMAND_ID_ADD_SQUAD:
		addSquad(safe_cast<terUnitSquad*>(command.unit()));
		break;

	case COMMAND_ID_FOLLOW_SQUAD:
		setSquadToFollow(command.unit());
		break;
	}
}

void terUnitSquad::setDamage(const DamageData& damage, terUnitBase* p)
{
	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui)
		(*ui)->setDamage(damage, 0);
}

int terUnitSquad::countUnits(terUnitAttributeID id, bool inSquad) const 
{
	MTL();
	int counter = 0;
	SquadUnitList::const_iterator ui;
	FOR_EACH(Units,ui){
		if((inSquad ? (*ui)->inSquad() : true) && !((*ui)->requestStatus() & LEGION_ACTION_STATUS_DISINTEGRATE)){
			if((*ui)->attr().ID == id)
				counter++;
			switch(id){
			case UNIT_ATTRIBUTE_SOLDIER:
				counter += (*ui)->transportAtom()[MUTATION_ATOM_SOLDIER];
				break;
			case UNIT_ATTRIBUTE_OFFICER:
				counter += (*ui)->transportAtom()[MUTATION_ATOM_OFFICER];
				break;
			case UNIT_ATTRIBUTE_TECHNIC:
				counter += (*ui)->transportAtom()[MUTATION_ATOM_TECHNIC];
				break;
			}
		}
	}
	return counter;
}

int terUnitSquad::countPossibleUnits(terUnitAttributeID id) const
{
	return Player->GetMutationElement(id).Enabled ?	squadMutationMolecula()/DamageMolecula(Player->unitAttribute(id)->damageMolecula) : 0;
}

SaveUnitData* terUnitSquad::universalSave(SaveUnitData* baseData)
{
	SaveUnitSquadData* data = castOrCreate<SaveUnitSquadData>(baseData);
	
	terUnitBase::universalSave(data);

	data->stablePosition = stablePose().trans;
	data->currentMutation = currentMutation();

	data->curvatureRadius = position_generator.curvatureRadius();

	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui)
		data->squadMembers.push_back((*ui)->universalSave(0));

	data->wayPoints = wayPoints_;

	data->patrolPoints = patrolPoints_;
	data->patrolIndex = patrolIndex_;

	AttackPointList::const_iterator ai;
	FOR_EACH(attack_points, ai){
		data->attackPoints.push_back(SaveAttackPoint());
		ai->save(data->attackPoints.back());
	}
	data->squadToFollow = squadToFollow_;

	data->offensiveMode = offensiveMode();

	for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){
		data->atomsRequested[i] = atomsRequested()[i];
		data->atomsPaused[i] = atomsPaused()[i];
	}

	data->mutationEnergy = mutationEnergy_;

	return data;
}

void terUnitSquad::universalLoad(const SaveUnitData* baseData)
{
	if(!baseData)
		return;

	const SaveUnitSquadData* data = safe_cast<const SaveUnitSquadData*>(baseData);

	terUnitBase::universalLoad(data);

	if(data->stablePosition.norm2())
		setPose(Se3f(QuatF(angleZ(), Vect3f::K), to3D(data->stablePosition, 0)), true);

	currentMutation_ = data->currentMutation;
	position_generator.setMode(PositionGenerator::Square, data->curvatureRadius);

	SaveUnitDataList::const_iterator mi;
	FOR_EACH(data->squadMembers, mi){
		terUnitBase* unit = Player->buildUnit((*mi)->attributeID);
		addUnit(safe_cast<terUnitLegionary*>(unit), false);
		unit->universalLoad(*mi);
		unit->Start();
		unit->setCollisionGroup(unit->collisionGroup() | COLLISION_GROUP_REAL);
	}

	Vect2fVect::const_iterator vi;
	FOR_EACH(data->wayPoints, vi)
		addWayPoint(*vi);

	patrolPoints_ = data->patrolPoints;
	patrolIndex_ = data->patrolIndex;

	vector<SaveAttackPoint>::const_iterator ai;
	FOR_EACH(data->attackPoints, ai){
		attack_points.push_back(AttackPoint());
		attack_points.back().load(*ai);
	}
	data->squadToFollow.setLink(squadToFollow_);

	offensiveMode_ = data->offensiveMode;

	for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){
		atomsRequested_[i] = data->atomsRequested[i];
		atomsPaused_[i] = data->atomsPaused[i];
	}

	mutationEnergy_ = data->mutationEnergy;

	calcCenter();
	setPosition(To3D(average_position));
}

//---------------------------

void terUnitSquad::showPath(const vector<Vect2f>& wayPoints, const vector<Vect2f>& patrolPoints, AttackPoint* attackPoint) {
	flagModel_->SetAttr(ATTRUNKOBJ_IGNORE);

	if (selected()) {
		bool showFlag = true;

		if(squadMutationMolecula().elementCount()){
			if(!wayPoints.empty()){
				Vect3f posPrev = position();
				vector<Vect2f>::const_iterator i;
				FOR_EACH(wayPoints, i){
					Vect3f pos = To3D(*i);
					terRenderDevice->DrawLine(posPrev, pos, pathColor);
					posPrev = pos;
				}
			
				Vect3f w, e;
				terCamera->GetCamera()->ConvertorWorldToViewPort(&posPrev,&w,&e);
				if(e.z < 1.0f)
					terRenderDevice->DrawRectangle(round(e.x) - 2,round(e.y) - 2,4,4,pathColor,0);
			}
			else
				showFlag = false;

			if (patrolPoints.size() > 1) {
				Vect3f posPrev = To3D(patrolPoints.back());
				vector<Vect2f>::const_iterator pi;
				FOR_EACH(patrolPoints, pi){
					Vect3f w, e;
					Vect3f pos = To3D(*pi);
					terCamera->GetCamera()->ConvertorWorldToViewPort(&pos,&w,&e);
					if(e.z < 1.0f)
						terRenderDevice->DrawRectangle(round(e.x) - 2,round(e.y) - 2,4,4,patrolColor,0);
					terRenderDevice->DrawLine(posPrev, pos, patrolColor);
					posPrev = pos;
				}
				showFlag = false;
			}

			if (attackPoint) {
				terRenderDevice->DrawLine(position(), attackPoint->position(), sColor4c(255,0,0,128));
				showFlag = false;
			}
		}

		if(showFlag){
			flagModel_->SetPosition(MatXf(Mat3f::ID, To3D(wayPoints_.empty() ? stablePose().trans : wayPoints_.back())));
			flagModel_->ClearAttr(ATTRUNKOBJ_IGNORE);
			flagModel_->SetPhase(flagModelPhase_);
		}
	}
	flagModelPhase_ = fmod(flagModelPhase_ + (float)(frame_time.delta()) / terModelWayFlagSpeed,1.0f);
}

void terUnitSquad::ShowInfo()
{
	terUnitBase::ShowInfo();
/*
	flagModel_->SetAttr(ATTRUNKOBJ_IGNORE);

	if(Player->active() && selected()){
		bool showFlag = true;

		if(squadMutationMolecula().elementCount()){
			if(!wayPoints_.empty()){
				Vect3f posPrev = position();
				Vect2fVect::const_iterator i;
				FOR_EACH(wayPoints_, i){
					Vect3f pos = To3D(*i);
					terRenderDevice->DrawLine(posPrev, pos, pathColor);
					posPrev = pos;
				}
			
				Vect3f w, e;
				terCamera->GetCamera()->ConvertorWorldToViewPort(&posPrev,&w,&e);
				if(e.z < 1.0f)
					terRenderDevice->DrawRectangle(round(e.x) - 2,round(e.y) - 2,4,4,pathColor,0);
			}
			else
				showFlag = false;

			if(patrolMode()){
				Vect3f posPrev = To3D(patrolPoints_.back());
				Vect2fVect::const_iterator pi;
				FOR_EACH(patrolPoints_, pi){
					Vect3f w, e;
					Vect3f pos = To3D(*pi);
					terCamera->GetCamera()->ConvertorWorldToViewPort(&pos,&w,&e);
					if(e.z < 1.0f)
						terRenderDevice->DrawRectangle(round(e.x) - 2,round(e.y) - 2,4,4,patrolColor,0);
					terRenderDevice->DrawLine(posPrev, pos, patrolColor);
					posPrev = pos;
				}
				showFlag = false;
			}

			if(!attack_points.empty()){
				AttackPoint& attackPoint = attack_points.front();
				terRenderDevice->DrawLine(position(), attackPoint.position(), sColor4c(255,0,0,128));
				if(!attackPoint.squad() && attackPoint.target())
				{
				//	attackPoint.target()->SelectEnemy();
				}
				showFlag = false;
			}
		}

		if(showFlag){
			flagModel_->SetPosition(MatXf(Mat3f::ID, To3D(wayPoints_.empty() ? stablePose().trans : wayPoints_.back())));
			flagModel_->ClearAttr(ATTRUNKOBJ_IGNORE);
			flagModel_->SetPhase(flagModelPhase_);
		}
	}
	flagModelPhase_ = fmod(flagModelPhase_ + (float)(frame_time.delta()) / terModelWayFlagSpeed,1.0f);
*/
}

void terUnitSquad::Select()
{
	terUnitSquad::SquadUnitList::iterator ui;

	terUnitBase::Select();
	FOR_EACH(Units, ui){
		(*ui)->Select();
	}
}

void terUnitSquad::Deselect()
{
	terUnitSquad::SquadUnitList::iterator ui;

	terUnitBase::Deselect();
	FOR_EACH(Units,ui){
		(*ui)->Deselect();
	}
}

void terUnitSquad::AvatarQuant()
{
	terUnitBase::AvatarQuant();
	avatar()->setPose(pose());
}

int terUnitSquad::GetInterfaceOffensiveMode()
{
	if(lastCommand() == COMMAND_ID_UNIT_OFFENSIVE)
		return findCommand(COMMAND_ID_UNIT_OFFENSIVE)->commandData();
	return offensiveMode();
}

void terUnitSquad::ChangeUnitOwner(terPlayer* player)
{
	terUnitBase::ChangeUnitOwner(player);

	while(!Units.empty())
		Units.front()->Kill();

	atomsRequested_ = atomsPaused_ = atomsProgress_ = DamageMolecula();
}

const terUnitLegionary* terUnitSquad::RequestTransportPoint(const terUnitBase* transported_unit) const
{
	float dist, distBest = FLT_INF;
	const terUnitLegionary* transport = 0;
	SquadUnitList::const_iterator ui;
	FOR_EACH(Units, ui){
		if((*ui)->alive() && (*ui)->GetLegionMutable() && transported_unit != *ui && !(*ui)->attr().is_base_unit &&
			distBest > (dist = (*ui)->position2D().distance2(transported_unit->position2D()))){
				distBest = dist;
				transport = *ui;
		}
	}

	return transport;
}

bool terUnitSquad::addToTransport(const terUnitLegionary* unit)
{
	terUnitLegionary* transport = 0;
	int transport_count0 = 0;
	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui){
		if((*ui)->GetLegionMutable() && unit != *ui && !(*ui)->attr().is_base_unit){
			int transport_count = (*ui)->transportAtom().elementCount();
			if(!transport || transport_count < transport_count0){
				transport = *ui;
				transport_count0 = transport_count;
			}
		}
	}

	if(transport){
		transport->addToTransport(unit->attr().ID - UNIT_ATTRIBUTE_SOLDIER);
	
		DamageMolecula atom(unit->attr().damageMolecula);
		squadMolecula_ += atom;
		mutationEnergy_ += atom.elementCount();

		return true;
	}

	return false;
}

int terUnitSquad::GetInterfaceLegionMode()
{
	int stop,attack,move,patrol,back;

	back = stop = attack = move = patrol = 0;
	switch(lastCommand()){
	case COMMAND_ID_OBJECT:
		attack = 1;
		break;
	case COMMAND_ID_STOP:
		stop = 1;
		break;
	case COMMAND_ID_PATROL:
		patrol = 1;
		break;
	case COMMAND_ID_ATTACK:
		attack = 1;
		break;
	case COMMAND_ID_RETURN_TO_BASE:
		back = 1;
		break;
	case COMMAND_ID_POINT:
		move = 1;
		break;
	}

	if(stop)
		return INTERFACE_LEGION_MODE_STOP;
	else if(back)
		return INTERFACE_LEGION_MODE_BACK_TO_FRAME;
	else if(attack)
		return INTERFACE_LEGION_MODE_ATTACK;
	else if(patrol)
		return INTERFACE_LEGION_MODE_PATROL;
	else if(move)
		return INTERFACE_LEGION_MODE_MOVE;

	if(patrolMode())
		return INTERFACE_LEGION_MODE_PATROL;
	else if(!attack_points.empty())
		return INTERFACE_LEGION_MODE_ATTACK;
	else if(!wayPoints_.empty())
		return INTERFACE_LEGION_MODE_MOVE;
	else
		return INTERFACE_LEGION_MODE_STOP;
}


//-----------------------------------------------------

void terUnitSquad::SupportQuant()
{
	if(!commander() || !commander()->isBuildingEnable())
		return;

	for(int i = 0;i < MUTATION_ATOM_MAX;i++){
		if(!atomsPaused_[i]){
			float progressMax = 0;
			int requested = atomsRequested_[i];
			terBuildingList& plants = Player->buildingList(UNIT_ATTRIBUTE_SOLDIER_PLANT + i);
			terBuildingList::iterator pi;
			FOR_EACH(plants, pi){
				if(!requested-- || Player->calcBaseUnits() >= globalAttr().baseUnitsMax)
					break;

				terBuildingPlant* plant = safe_cast<terBuildingPlant*>(*pi);
				float progress = plant->requestProductionProgress(this);
				
				if(progressMax < progress)
					progressMax = progress;

				if(progress > 1 - FLT_EPS){
					terUnitLegionary* unit = safe_cast<terUnitLegionary*>(Player->buildUnit(terUnitAttributeID(UNIT_ATTRIBUTE_SOLDIER + i)));
					unit->setPose(Se3f(QuatF::ID, plant->position()), true);
					unit->setOrientation(plant->orientation());
					unit->Start();
					plant->setProduction(unit);
					addUnit(unit, false);
					atomsRequested_[i]--;
					universe()->checkEvent(EventUnitPlayer(Event::CREATE_BASE_UNIT, unit, Player));
				}
			}
			atomsProgress_[i] = progressMax*100;
		}
	}
}

void terUnitSquad::GetAtomProduction(DamageMolecula& atom_request,DamageMolecula& atom_progress,DamageMolecula& atom_enabled)
{
	atom_request = atom_progress = atom_enabled = DamageMolecula();

	if(!Player->frame())
		return;

	atom_request = atomsRequested_;
	atom_progress = atomsProgress_;

	switch(lastCommand()){
		case COMMAND_ID_PRODUCTION_INC:
			atom_request[findCommand(COMMAND_ID_PRODUCTION_INC)->commandData()]++;
			break;
		case COMMAND_ID_PRODUCTION_INC_10:
			atom_request[findCommand(COMMAND_ID_PRODUCTION_INC_10)->commandData()] += 10;
			break;
		case COMMAND_ID_PRODUCTION_DEC:
			atom_request[findCommand(COMMAND_ID_PRODUCTION_DEC)->commandData()]--;
			break;
		case COMMAND_ID_PRODUCTION_DEC_10:
			atom_request[findCommand(COMMAND_ID_PRODUCTION_DEC_10)->commandData()] -= 10;
			break;
	}

	if(commander() && commander()->isBuildingEnable())
		for(int i = 0;i < MUTATION_ATOM_MAX;i++)
			atom_enabled[i] = Player->GetEvolutionBuildingData((terUnitAttributeID)(UNIT_ATTRIBUTE_SOLDIER_PLANT + i)).Worked;
}

void terUnitSquad::GetAtomPaused(DamageMolecula& paused)
{ 
	paused = DamageMolecula(-1,-1,-1);

	switch(lastCommand()){
		case COMMAND_ID_PRODUCTION_PAUSE_ON:
			paused[findCommand(COMMAND_ID_PRODUCTION_PAUSE_ON)->commandData()] = 1;
			break;
		case COMMAND_ID_PRODUCTION_PAUSE_OFF:
			paused[findCommand(COMMAND_ID_PRODUCTION_PAUSE_OFF)->commandData()] = 0;
			break;
	}

	for(int i = 0;i < MUTATION_ATOM_MAX;i++){
		if(paused[i] == -1)
			paused[i] = atomsPaused_[i]; 
	}
}

int terUnitSquad::loadedTechnicsCount() const
{
	int count = 0;
	SquadUnitList::const_iterator ui;
	FOR_EACH(Units,ui){
		if((*ui)->attr().ID == UNIT_ATTRIBUTE_TECHNIC && (*ui)->isWeaponReady())
			count++;
	}

	return count;
}

int terUnitSquad::dischargeTechnics(int count)
{
	if(count <= 0) return 0;

	int discharged_count = 0;

	SquadUnitList::iterator it;
	FOR_EACH(Units,it){
		if((*it)->attr().ID == UNIT_ATTRIBUTE_TECHNIC && (*it)->isWeaponReady()){
			int status;
			if((*it)->fireRequest(&(*it)->position(),NULL,status)){
				discharged_count++;
				if(--count <= 0)
					break;
			}
		}
	}

	return discharged_count;
}

void terUnitSquad::initMutation(SquadUnitList& sources, SquadUnitList& destinations)
{
	xassert(!sources.empty() && !destinations.empty());
	if(sources.empty() || destinations.empty())
		return;

	setStablePosition(average_position + average_position_offset);
	check_readiness_to_move = true;

	position_generator.clear();
	SquadUnitList::iterator ui;
	FOR_EACH(destinations, ui)
		addUnit(*ui, true);

	FOR_EACH(sources, ui)
		removeUnit(*ui);

	FOR_EACH(Units, ui)
		if((*ui)->inSquad()){
			(*ui)->stop();
			(*ui)->BodyPoint->stopMovement();
		}
	
	position_generator.setMode(PositionGenerator::Square);
	position_generator.clear();

	repositionFormation(true);

	if(!wayPoints_.empty())
		recalcWayPoints();

	//-----------------------------------
	mutation_process.clear();

	{
		MTAuto lock(mutation_process.getLock());
		FOR_EACH(destinations,ui){
			(*ui)->setRequestStatus(LEGION_ACTION_STATUS_INTEGRATE);
			(*ui)->Start();
			mutation_process.addTarget((*ui)->GetRigidBodyPoint());
		}
		FOR_EACH(sources,ui){
			mutation_process.addSource((*ui)->GetRigidBodyPoint());
			(*ui)->setRequestStatus(LEGION_ACTION_STATUS_DISINTEGRATE);
		}
	}

	mutation_process.start(position());
}

bool terUnitSquad::mutate(terUnitAttributeID newMutation)
{
	start_timer_auto(mutate, STATISTICS_GROUP_LOGIC);

	bool ret_val = false;

	if(newMutation != UNIT_ATTRIBUTE_NONE){
		SquadUnitList sourceUnits = Units;
		DamageMolecula sourceMolecula = squadMutationMolecula();
		DamageMolecula targetRecipe(Player->unitAttribute(newMutation)->damageMolecula);
		int numMax = sourceMolecula/targetRecipe;
		int currentNum = currentMutation() == newMutation ? n_complex_units : 0;
		currentMutation_ = newMutation;
		if(currentNum < numMax){
			if((mutationEnergy_ -= targetRecipe.elementCount()*(numMax - currentNum)) < 0)
				mutationEnergy_ = 0;

			DamageMolecula rest = sourceMolecula - targetRecipe*numMax;
			SquadUnitList targetUnits;
			for(int i = 0; i < numMax; i++){
				terUnitLegionary* unit = safe_cast<terUnitLegionary*>(Player->buildUnit(newMutation));
				for(int j = 0; j < MUTATION_ATOM_MAX; j++){
					unit->addToTransport(j, rest[j]/numMax);
					if(rest[j] % numMax){
						unit->addToTransport(j, 1);
						--rest[j];
					}
				}
				targetUnits.push_back(unit);
			}

			initMutation(sourceUnits, targetUnits);
			ret_val = true;
		}
	}
	else{
		currentMutation_ = UNIT_ATTRIBUTE_NONE;
		SquadUnitList sourceUnits;
		SquadUnitList::iterator ui;
		FOR_EACH(Units, ui){
			if((*ui)->GetLegionMutable() && 
			   (*ui)->attr().ID != UNIT_ATTRIBUTE_TECHNIC && 
			   (*ui)->attr().ID != UNIT_ATTRIBUTE_OFFICER &&
			   (*ui)->attr().ID != UNIT_ATTRIBUTE_SOLDIER){
				sourceUnits.push_back((*ui));
			}
		}
		if(!sourceUnits.empty()){
			int count = 0;
			SquadUnitList targetUnits;
			SquadUnitList::iterator ui;
			FOR_EACH(sourceUnits, ui){
				for(int i = 0;i < MUTATION_ATOM_MAX;i++){
					int n_atom = (*ui)->damageMolecula().aliveElementCount(1 << i) + (*ui)->transportAtom()[i];
					for(int j = 0;j < n_atom;j++){
						terUnitLegionary* unit = safe_cast<terUnitLegionary*>(
							Player->buildUnit(terUnitAttributeID(UNIT_ATTRIBUTE_SOLDIER + i)));
						targetUnits.push_back(unit);
					}
				}
			}

			initMutation(sourceUnits, targetUnits);
			ret_val = true;
		}
	}

	return ret_val;
}

void terUnitSquad::disintegrateUnitsQuant()
{
	if(currentMutation() == UNIT_ATTRIBUTE_NONE || !mutationFinished()) 
		return;

	SquadUnitList sourceUnits;
	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui){
		if((*ui)->isDisintegrating() && 
		   (*ui)->attr().ID != UNIT_ATTRIBUTE_TECHNIC && 
		   (*ui)->attr().ID != UNIT_ATTRIBUTE_OFFICER &&
		   (*ui)->attr().ID != UNIT_ATTRIBUTE_SOLDIER){
			sourceUnits.push_back((*ui));
		}
	}
	if(!sourceUnits.empty()){
		int count = 0;
		SquadUnitList targetUnits;
		SquadUnitList::iterator ui;
		FOR_EACH(sourceUnits, ui){
			for(int i = 0;i < MUTATION_ATOM_MAX;i++){
				int n_atom = (*ui)->damageMolecula().aliveElementCount(1 << i) + (*ui)->transportAtom()[i];
				for(int j = 0;j < n_atom;j++){
					terUnitLegionary* unit = safe_cast<terUnitLegionary*>(
						Player->buildUnit(terUnitAttributeID(UNIT_ATTRIBUTE_SOLDIER + i)));
					targetUnits.push_back(unit);
					count++;
				}
			}
		}

		if((mutationEnergy_ -= count) < 0)
			mutationEnergy_ = 0;

		initMutation(sourceUnits, targetUnits);
	}
}

//-------------------------------------------
void terUnitSquad::AttackPoint::save(SaveAttackPoint& data) const
{
	data.unit = unit_;
	data.squad = squad_;
	data.position = position_;
	data.positionTarget = positionTarget_;
}

void terUnitSquad::AttackPoint::load(const SaveAttackPoint& data)
{
	data.unit.setLink(unit_);
	data.squad.setLink((terUnitBase*&)squad_);
	position_ = data.position;
	positionTarget_ = data.positionTarget;
}

terUnitBase* terUnitSquad::AttackPoint::target(terUnitLegionary& assailant) const 
{ 
	if(positionTarget_) 
		return 0;

	if(squad_){
		if(unit_)
			if(unit_->isUnseen())
				return 0;
			else if(assailant.checkFireClass(unit_))
				return unit_;
	
		terUnitBase* u = squad_->getBestTarget(assailant);
		return u && !u->isUnseen() ? u : 0;
	}
	else 
		return unit_ && assailant.checkFireClass(unit_) ? unit_ : 0;
}


bool terUnitSquad::AttackPoint::obsolete()
{
	if(positionTarget_) return false;

	if(squad_ && !squad_->alive())
		squad_ = 0;

	if(unit_)
		if(!unit_->alive())
			unit_ = 0;
		else
			return false;

	if(squad_)
		return !squad_->alive() || squad_->Empty();

	return !unit_;
}

terUnitBase* terUnitSquad::getBestTarget(terUnitLegionary& assailant)
{
	float d2_min = FLT_INF;
	terUnitBase* best_target = 0;
	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui){
		if(!assailant.checkFireClass(*ui) || !(*ui)->inSquad())
			continue;
		float d2 = assailant.position2D().distance2((*ui)->position2D());
		if(d2_min > d2){
			d2_min = d2;
			best_target = *ui;
		}
	}
	return best_target;
}

void terUnitSquad::calcCenter()
{
	// Calc new position
	Vect2f prev_average_position = average_position;
	average_position = Vect2f::ZERO;
	int counter = 0;
	bool allUnitsInPosition = true;
	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui){
		terUnitLegionary& unit = **ui;
		xassert(unit.inSquad() || unit.attr().is_base_unit);
		if(!unit.inSquad() || n_complex_units && unit.attr().is_base_unit) // не учитывать не дошедших и базовых, когда есть производные
			continue;
		average_position += unit.position2D();
		counter++;

		if(unit.out_path.size() == unitsWayPoinsSize_)
			allUnitsInPosition = false;
	}

	if(!wayPoints_.empty() && allUnitsInPosition){
		wayPoints_.erase(wayPoints_.begin());
		unitsWayPoinsSize_--;

		if(patrolMode() && counter){
			while(wayPoints_.size() < 2)
				addWayPoint(patrolPoints_[patrolIndex_ = (patrolIndex_ + 1) % patrolPoints_.size()]);
		}
		else if(!wayPoints_.empty())
			recalcWayPoints();
		else if(!Units.empty()){
			if(!attack_points.empty() && currentAttribute()->dynamicAttack)
				repositionToAttack(attack_points.front());
			else if(Player->active())
				SND3DPlaySound("squad_inposition", &position());
		}

		calcCenter();
		return;
	}
	else if(wayPoints_.empty() && patrolMode() && counter)
		addWayPoint(patrolPoints_[patrolIndex_ = (patrolIndex_ + 1) % patrolPoints_.size()]);

	if(!counter){
		setRadius(squad_described_radius_min);
		average_position = stablePose().trans;
		average_velocity = Vect2f::ZERO;
		if(!stablePoseRestarted_){ //  || Player->isAI()
			stablePoseRestarted_ = true;
			goHome();
		}
		return;
	}
	
	stablePoseRestarted_ = false;
	average_position /= counter;

	// Calc new radius
	float described_radius = sqr(squad_described_radius_min);
	FOR_EACH(Units, ui){
		terUnitLegionary& unit = **ui;
		if(!unit.inSquad())
			continue;
		described_radius = max(described_radius, unit.position2D().distance2(average_position) + sqr(formationRadius()));
	}
	described_radius = min(sqrtf(described_radius), squad_described_radius_max);
	setRadius(described_radius);

	// Calc including_cluster
	FOR_EACH(Units, ui){
		including_cluster = (*ui)->includingCluster();
		int cnt = 0;
		SquadUnitList::iterator uj;
		FOR_EACH(Units, uj)
			if((*uj)->includingCluster() == including_cluster)
				cnt += (*uj)->damageMolecula().elementCount();
		if(cnt >= counter/2)
			break;
	}

	average(average_velocity, average_position - prev_average_position, squad_velocity_avr_tau);
}

void terUnitSquad::correctSpeed()
{
	SquadUnitList::iterator ui;

	if(check_readiness_to_move){
		bool not_ready = false;
		SquadUnitList::iterator ui;
		FOR_EACH(Units, ui)
			if(!(*ui)->out_path.empty() && !((*ui)->responseStatus() & LEGION_ACTION_STATUS_MOVE))
				not_ready = true;
		
		if(not_ready){
			FOR_EACH(Units, ui)
				(*ui)->speed_factor = 0;
			return;
		}
		else
			check_readiness_to_move = false;
	}

//	float v,vel_min = FLT_INF;
//	FOR_EACH(Units, ui)
//		if(vel_min > (v = (*ui)->BodyPoint->forward_velocity_max))
//			vel_min = v;

	float averagePath = 0;
	int counter = 0;
	FOR_EACH(Units, ui)
		if((*ui)->inSquad()){
			averagePath += (*ui)->deltaPath();
			counter++;
		}
	
	if(counter){
		averagePath /= counter;
		averagePath += 0.1f;

		FOR_EACH(Units, ui)
			(*ui)->correctSpeed(averagePath);
	}

	if(patrolMode() && !attack_points.empty() && !patrol_mode_ignore_targets_timer){
		if(!patrol_mode_attack_timer)
			patrol_mode_ignore_targets_timer.start(patrol_mode_ignore_targets_time);

		FOR_EACH(Units, ui)
			(*ui)->speed_factor = 0;
	}
}

void terUnitSquad::recalcWayPoints()
{
	setStablePosition(wayPoints_.front());

	check_readiness_to_move = true;

	// Calc new orientation
	Vect2f delta = wayPoints_.front() - position2D();
	if(delta.norm() > FLT_EPS)
		setAngleZ(atan2(delta.y, delta.x) + M_PI/2);

	repositionFormation(false);

	MatX2f firstPose = stablePose();
	firstPose.trans += average_position_offset;
	unitsWayPoinsSize_ = 1;
	SquadUnitList::iterator ui;
	FOR_EACH(Units,ui){
		(*ui)->stop();
		(*ui)->addWayPoint(firstPose*(*ui)->localPosition());
	}

	if(wayPoints_.size() > 1){
		unitsWayPoinsSize_ = 2;
		Vect2f delta = wayPoints_[1] - wayPoints_[0];
		//MatX2f secondPose(Mat2f(atan2(delta.y, delta.x) - M_PI/2), wayPoints_[1] + average_position_offset);
		MatX2f secondPose(firstPose.rot, wayPoints_[1] + average_position_offset);
		FOR_EACH(Units,ui)
			(*ui)->addWayPoint(secondPose*(*ui)->localPosition());
	}
}

void terUnitSquad::repositionFormation(bool forceReposition)
{
	if(Empty()){
		average_position_offset = Vect2f::ZERO;
		return;
	}

	float radius = noTargets() ? 0 : optimalRadius_;
	if(position_generator.curvatureRadius() != radius){
		position_generator.setMode(PositionGenerator::Square, radius);
		forceReposition = true;
	}
	
	if(!forceReposition && Units.size() == position_generator.counter()){ 
		// Просто пересчитать без паковки позиций
		SquadUnitList::iterator ui;
		if(dot(prev_forward_direction, forwardDirection()) < 0){ // инвертировать 
			FOR_EACH(Units,ui)
				(*ui)->setLocalPosition(position_generator.invert((*ui)->localPosition()));
			position_generator.inversion();
		}
	}
	else{ 
		// паковать позиции
		if(dot(prev_forward_direction, forwardDirection()) < 0){ // сначала инвертируем
			SquadUnitList::iterator ui;
			FOR_EACH(Units,ui)
				if((*ui)->inSquad())
					(*ui)->setLocalPosition(position_generator.invert((*ui)->localPosition()));
		}

		position_generator.clear();
		SquadUnitList units = Units;
		while(!units.empty()){
			if(units.front()->inSquad()){
				Vect2f pos = position_generator.get(formationRadius());
				float dist2, min_dist2 = FLT_INF;
				SquadUnitList::iterator ui_best = units.begin();
				SquadUnitList::iterator ui;
				FOR_EACH(units, ui){
					if(min_dist2 > (dist2 = (*ui)->localPosition().distance2(pos))){
						min_dist2 = dist2;
						ui_best = ui;
					}
				}
				(*ui_best)->setLocalPosition(pos);
				units.erase(ui_best);
			}
			else
				units.erase(units.begin());
		}
	}

	Vect2f avr_pos = Vect2f::ZERO;
	SquadUnitList::iterator ui;
	FOR_EACH(Units,ui)
		avr_pos += (*ui)->localPosition();
	avr_pos /= Units.size();
	average_position_offset = stablePose().trans - stablePose()*avr_pos;

	prev_forward_direction = forwardDirection();
}

void terUnitSquad::repositionToAttack(AttackPoint& attackPoint, bool repeated)
{
	if(Empty())
		return;

	if(repeated && reposition_to_attack_timer())
		return;

	Vect2f target = attackPoint.position();
	Vect2f delta = target - (noWayPoints() ? position2D() : wayPoints_.back());
	float dist = delta.norm();
	delta /= dist + FLT_EPS;

	if(Units.front()->attr().dynamicAttack){
		terUnitLegionary& unit = *Units.front();
		delta *= Mat2f(terLogicRNDfrnd()*unit.attr().attackAngleDeviation);
		target += delta*unit.attr().attackTurnRadius;
		reposition_to_attack_timer.start(squad_reposition_to_attack_delay_flying);
	}
	else{
		float min_radius = currentAttribute()->fireRadiusMin();
		float max_radius = currentAttribute()->fireRadius();// - formationRadius()*2*clamp(position_generator.numLines() - 1, 0.5, 100);
		xassert_s(min_radius < max_radius, (string("Недостаточный радиус атаки у ") + currentAttribute()->internalName()).c_str());

		if(dist < min_radius)
			optimalRadius_ = min_radius*(1 + squad_reposition_to_attack_radius_tolerance);
		else if(dist > max_radius)
			optimalRadius_ = max_radius*(1 - squad_reposition_to_attack_radius_tolerance);
		else
			return; // Цель в допустимой области - перестраиваться не надо

		//if(attackPoint.squad()){
		//	terUnitSquad& squad = *attackPoint.squad();
		//	float t = 0.5 + clamp(dot(squad.average_velocity, delta)*squad_reposition_to_attack_velocity_factor, -0.5, 0.5);
		//	optimalRadius_ = min_radius*(1 + squad_reposition_to_attack_radius_tolerance)*t + max_radius*(1 - squad_reposition_to_attack_radius_tolerance)*(1.f - t);
		//}

		target -= delta*optimalRadius_;
		//position_generator.setMode(PositionGenerator::Square, optimalRadius_);
		reposition_to_attack_timer.start(squad_reposition_to_attack_delay);
	}

	addWayPoint(target);
}

void terUnitSquad::clearWayPoints()
{
	wayPoints_.clear();

	SquadUnitList::iterator it;
	FOR_EACH(Units,it)
		if((*it)->inSquad())
			(*it)->stop();
}

void terUnitSquad::addWayPoint(const Vect2f& point)
{
	Vect2iVect pathFindList;
	if(squadMutationMolecula().elementCount() && !isFlying() 
	  && ai_tile_map->findPath(wayPoints_.empty() ? position2D() : wayPoints_.back(), point, pathFindList, AITileMap::PATH_NORMAL)){
		Vect2iVect::iterator i;
		FOR_EACH(pathFindList, i)
			wayPoints_.push_back(*i);
	}
	else
		wayPoints_.push_back(point);

	if(unitsWayPoinsSize_ < 2)
		recalcWayPoints();
}

void terUnitSquad::clearTargets()
{
	attack_points.clear();

	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui)
		(*ui)->setAttackPoint(NULL);
}

void terUnitSquad::fireStop()
{
	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui)
		(*ui)->fireStop();
}

void terUnitSquad::addTarget(terUnitBase* target)
{
	xassert(target);
	AttackPoint attackPoint(target);
	if((n_complex_units && attackPoint.target(*Units.front())) || !Empty()){
		clearWayPoints();
		attack_points.push_back(attackPoint);
		repositionToAttack(attack_points.front());
	}
}

bool lineCircleIntersection(const Vect2f& p0, const Vect2f& p1, const Vect2f& pc, float radius, Vect2f& result)
{
	// Ищет первое пересечение отрезка (0..1) с окружностью 
	Vect2f dp = p1 - p0;
	Vect2f dc = p0 - pc;
	float dp_2 = dp.norm2();
	float dc_2 = dc.norm2();
	float dp_dc = dot(dp, dc);
	float det2 = sqr(dp_dc) + dp_2*(sqr(radius) - dc_2);
	if(det2 < FLT_EPS)
		return false;
	float t = (-dp_dc - sqrt(det2))/(dp_2 + 0.001);
	if(t > 0 && t < 1){
		result = p0 + dp*t;
		xassert(fabs(result.distance(pc) - radius) < 1);
		return true;
	}
	return false;
}

void terUnitSquad::addTarget(terUnitBase* target, const Vect2iVect& path)
{
	xassert(target);
	AttackPoint attackPoint(target);
	if((n_complex_units && attackPoint.target(*Units.front())) || !Empty()){
		clearWayPoints();
		Vect2f targetPosition = target->position2D();
		if(!currentAttribute()->dontUseDamageMapPathFind && position2D().distance2(targetPosition) > sqr(currentAttribute()->fireRadius())){
			Vect2f prevPosition = position2D();
			Vect2iVect::const_iterator pi;
			FOR_EACH(path, pi){
				Vect2f result;
				if(!lineCircleIntersection(prevPosition, *pi, targetPosition, currentAttribute()->fireRadius(), result))
					addWayPoint(*pi);
				else{
					addWayPoint(result);
					break;
				}
				prevPosition = *pi;
			}
		}

		attack_points.push_back(attackPoint);
		repositionToAttack(attack_points.front());
	}
}

void terUnitSquad::addTarget(const Vect3f& v)
{
	if(n_complex_units){
		clearWayPoints();
		attack_points.push_back(AttackPoint(v));
		repositionToAttack(attack_points.front());
	}
}

class SquadSearchTargetScanOp
{
	Vect2f position;

	float radius_min2;
	float fire_radius2;
	float radius_max2;
	float optimal_radius2;

	terUnitBase* myUnit;
	int including_cluster;
	bool ignoreField_;
	int AttackClass;

	terUnitBase* best_target;
	float best_factor;

public:

	SquadSearchTargetScanOp(const Vect2f& pos, float radius_max, terUnitSquad& squad)
	{
		position = pos;

		myUnit = squad.Units.front();
		const AttributeBase* attr = &(myUnit->attr());

		ignoreField_ = attr->checkWeaponFlag(WEAPON_IGNORE_FIELD);

		radius_min2 = sqr(attr->fireRadiusMin());
		fire_radius2 = sqr(attr->fireRadius());
		radius_max2 = sqr(radius_max);
		optimal_radius2 = sqr((attr->fireRadiusMin() + attr->fireRadius())/2);

		including_cluster = squad.including_cluster;
		AttackClass = attr->AttackClass;

		best_target = 0;
		best_factor = 0;
	}

	void operator()(terUnitBase* unit2)
	{
		if(!myUnit->isEnemy(unit2)) 
			return;
		if(!unit2->damageMolecula().isAlive())
			return;
		if(!ignoreField_ && including_cluster != unit2->includingCluster()) // закрыт куполом
			return;
		if(!(unit2->unitClass() & AttackClass)) // нельзя стрелять
			return;
		if(unit2->isUnseen())
			return;

		float dist2 = position.distance2(unit2->position2D());
		if(dist2 < radius_max2)
		{
			float f = sqr(unit2->attr().kill_priority) + 1.f/(1.f + fabs(optimal_radius2 - dist2));
			if(dist2 <= radius_min2)
				f /= 1000 + dist2;
			if(dist2 >= fire_radius2)
				f /= 1000 + dist2;
			if(best_factor < f)
			{
				best_factor = f;
				best_target = unit2;
			}
		}
	}

	terUnitBase* result()	
	{ 
		return best_target; 
	}
};

class SquadSearchTargetsScanOp
{
public:
	SquadSearchTargetsScanOp(const Vect2f& pos, float radius_max, const terUnitSquad& squad)
	{
		position_ = pos;

		basicSquadMode_ = (squad.currentMutation() == UNIT_ATTRIBUTE_NONE);

		myUnit_ = squad.squadUnits().front();
		const AttributeBase* attr = (basicSquadMode_) ? squad.Player->unitAttribute(UNIT_ATTRIBUTE_SOLDIER) : &(myUnit_->attr());

		ignoreField_ = attr->checkWeaponFlag(WEAPON_IGNORE_FIELD);
		excludeHolograms_ = attr->weaponSetup.excludeHolograms;

		radiusMax_ = sqr(radius_max);

		includingCluster_ = squad.includingCluster();
		attackClass_[0] = attackClass_[1] = attr->AttackClass;

		if(basicSquadMode_){
			attr = squad.Player->unitAttribute(UNIT_ATTRIBUTE_OFFICER);
			attackClass_[1] = attr->AttackClass;

			attr = squad.Player->unitAttribute(UNIT_ATTRIBUTE_TECHNIC);
			attackClass_[2] = attr->AttackClass;

			targets_[1].reserve(16);
			targets_[2].reserve(16);
		}

		targets_[0].reserve(16);
	}

	struct TargetData
	{
		TargetData(terUnitBase* unit,float factor) : unit_(unit), factor_(factor) { }

		bool operator > (const TargetData& data) const { return factor_ < data.factor_; }
		bool operator == (const terUnitBase* unit) const { return unit_ == unit; }

		terUnitBase* unit_;
		float factor_;
	};

	typedef vector<TargetData> TargetDataList;
	typedef TargetDataList::iterator iterator;
	typedef TargetDataList::const_iterator const_iterator;

	const TargetDataList& targets(int idx = 0) const { return targets_[idx]; }

	struct TargetOrderingOp
	{
		bool operator() (const TargetData& t0,const TargetData& t1){
			return t0.factor_ > t1.factor_;
		}
	};
	
	void sortTargets(int idx = 0){ sort(targets_[idx].begin(),targets_[idx].end(),TargetOrderingOp()); }

	void operator()(terUnitBase* unit2)
	{
		if(!unit2->damageMolecula().isAlive())
			return;
		if(!ignoreField_ && includingCluster_ != unit2->includingCluster()) // закрыт куполом
			return;
		if(excludeHolograms_ && unit2->isBuilding() && !unit2->isConstructed())
			return;

		float dist2 = position_.distance2(unit2->position2D());
		if(dist2 > radiusMax_) return;

		if(myUnit_->isEnemy(unit2) && !unit2->isUnseen() && unit2->unitClass() & attackClass_[0]){
			float f = sqr(unit2->attr().kill_priority) + 1.f/(1.f + dist2);

			if(unit2->possibleDamage() >= unit2->damageMolecula().aliveElementCount())
				f /= 1000 + dist2;
			else
				f -= float(unit2->possibleDamage()) / float(unit2->damageMolecula().aliveElementCount());

			TargetDataList::const_iterator it = find(targets_[0].begin(),targets_[0].end(),unit2);
			if(it == targets_[0].end())
				targets_[0].push_back(TargetData(unit2,f));
		}

		if(!basicSquadMode_) return;

		if(myUnit_->isEnemy(unit2) && !unit2->isUnseen() && unit2->unitClass() & attackClass_[1]){
			float f = sqr(unit2->attr().kill_priority) + 1.f/(1.f + dist2) + 1.f/(1.f + unit2->freezeFactor());

			TargetDataList::const_iterator it = find(targets_[1].begin(),targets_[1].end(),unit2);
			if(it == targets_[1].end())
				targets_[1].push_back(TargetData(unit2,f));
		}

		if(!myUnit_->isEnemy(unit2) && unit2->unitClass() & attackClass_[2] && unit2->repairRequest()){
			float f = sqr(unit2->attr().kill_priority) + 1.f/(1.f + dist2) + 1.f/unit2->damageMolecula().phase();

			TargetDataList::const_iterator it = find(targets_[2].begin(),targets_[2].end(),unit2);
			if(it == targets_[2].end())
				targets_[2].push_back(TargetData(unit2,f));
		}
	}

private:

	/// режим сканирования для базового сквада 
	/** 
		targets_[0] - цели для солдат
		targets_[1] - цели для офицеров
	*/
	bool basicSquadMode_;

	Vect2f position_;

	float radiusMax_;

	const terUnitBase* myUnit_;

	int includingCluster_;
	bool ignoreField_;
	bool excludeHolograms_;

	int attackClass_[3];

	TargetDataList targets_[3];
};

class SquadTechnicianSearchTargetsScanOp
{
public:
	SquadTechnicianSearchTargetsScanOp(const Vect2f& pos, float radius_max, const terUnitSquad& squad)
	{
		position_ = pos;

		myUnit_ = squad.squadUnits().front();
		const AttributeBase* attr = squad.Player->unitAttribute(UNIT_ATTRIBUTE_TECHNIC);

		ignoreField_ = attr->checkWeaponFlag(WEAPON_IGNORE_FIELD);
		attackClass_ = attr->AttackClass;

		radiusMax_ = sqr(radius_max);

		includingCluster_ = squad.includingCluster();

		targets_.reserve(16);
	}

	struct TargetData
	{
		TargetData(terUnitBase* unit,float factor) : unit_(unit), factor_(factor) { }

		bool operator > (const TargetData& data) const { return factor_ < data.factor_; }
		bool operator == (const terUnitBase* unit) const { return unit_ == unit; }

		terUnitBase* unit_;
		float factor_;
	};

	typedef vector<TargetData> TargetDataList;
	typedef TargetDataList::iterator iterator;
	typedef TargetDataList::const_iterator const_iterator;

	const TargetDataList& targets() const { return targets_; }

	struct TargetOrderingOp
	{
		bool operator() (const TargetData& t0,const TargetData& t1){
			return t0.factor_ > t1.factor_;
		}
	};
	
	void sortTargets(){ sort(targets_.begin(),targets_.end(),TargetOrderingOp()); }

	void operator()(terUnitBase* unit2)
	{
		if(!unit2->damageMolecula().isAlive())
			return;
		if(!ignoreField_ && includingCluster_ != unit2->includingCluster()) // закрыт куполом
			return;

		float dist2 = position_.distance2(unit2->position2D());
		if(dist2 > radiusMax_) return;

		if(!myUnit_->isEnemy(unit2) && unit2->unitClass() & attackClass_ && unit2->repairRequest()){
			float f = sqr(unit2->attr().kill_priority) + 1.f/(1.f + dist2) + 1.f/unit2->damageMolecula().phase();

			TargetDataList::const_iterator it = find(targets_.begin(),targets_.end(),unit2);
			if(it == targets_.end())
				targets_.push_back(TargetData(unit2,f));
		}
	}

private:

	Vect2f position_;

	float radiusMax_;

	const terUnitBase* myUnit_;

	int includingCluster_;
	bool ignoreField_;

	int attackClass_;

	TargetDataList targets_;
};

terUnitBase* terUnitSquad::findBestTarget(const Vect2f& pos, float radius)
{
	SquadSearchTargetScanOp op(pos, radius, *this);
	universe()->UnitGrid.Scan(pos.x, pos.y, radius, op);
	return op.result();
}

void terUnitSquad::attackQuant()
{
	if(!attack_points.empty()){ // Есть указанная цель.
		if(!attack_points.front().positionTarget()){
			terUnitBase* target = 0;
			SquadUnitList::iterator ui;
			FOR_EACH(Units, ui){
				terUnitLegionary& unit = **ui;
				if(unit.attr().ID != UNIT_ATTRIBUTE_TECHNIC){
					terUnitBase* t = attack_points.front().target(unit);
					if(t){
						target = t;
						if(!checkAttackPosition(t->position()) && !isFlying())
							t = 0;
					}
					unit.setAttackPoint(t, true);
				}
			}

			if(target){
				if(Player->isAI()){
					if(target->includingCluster() != includingCluster())
						attack_points.pop_front();
				}
				else if(!patrolMode()){
					wayPoints_.clear();
					repositionToAttack(attack_points.front(), true);
				}
			}
			else
				attack_points.pop_front();
		}
		else{
			Vect3f target = attack_points.front().position();

			SquadUnitList::iterator ui;
			FOR_EACH(Units, ui){
				terUnitLegionary& unit = **ui;
				if(unit.attr().ID != UNIT_ATTRIBUTE_TECHNIC)
					unit.setAttackPosition(target, true);
			}

			if(offensiveMode() && !patrolMode() && noWayPoints())
				repositionToAttack(attack_points.front(), true);
		}
	}
	else{ // Поиск цели
		if(offensiveMode() || currentAttribute()->isDefenciveAttackEnabled()){
			if(!targets_scan_timer()){
				float fire_radius = offensiveMode() && !patrolMode() ? currentAttribute()->sightRadius() : currentAttribute()->fireRadius();
				fire_radius += radius();
				SquadSearchTargetsScanOp op(position2D(), fire_radius, *this);
				universe()->UnitGrid.Scan(position().x, position().y, fire_radius, op);
				op.sortTargets();

				if(!targets_clean_timer()){
					SquadUnitList::iterator ui;
					FOR_EACH(Units, ui)
						(*ui)->setAttackPoint(NULL, false);

					targets_clean_timer.start(squad_targets_clean_period);
				}

				bool attack_flag = false;

				if(currentMutation() == UNIT_ATTRIBUTE_NONE){
					SquadSearchTargetsScanOp::const_iterator ti;
					FOR_EACH(op.targets(),ti){
						if(distributeAttackTarget(AttackPoint(const_cast<terUnitBase*>(ti->unit_)),UNIT_ATTRIBUTE_SOLDIER))
							attack_flag = true;
					}

					bool ret_flag = true;
					while(ret_flag){ // распределение целей по незадействованным юнитам
						ret_flag = false;
						FOR_EACH(op.targets(),ti){
							if(distributeAttackTarget(AttackPoint(const_cast<terUnitBase*>(ti->unit_)),UNIT_ATTRIBUTE_SOLDIER,true))
								ret_flag = true;
						}
					}

					op.sortTargets(1);

					FOR_EACH(op.targets(1),ti){
						if(distributeAttackTarget(AttackPoint(const_cast<terUnitBase*>(ti->unit_)),UNIT_ATTRIBUTE_OFFICER,true))
							attack_flag = true;
					}

					ret_flag = true;
					while(ret_flag){ // распределение целей по незадействованным юнитам
						ret_flag = false;
						FOR_EACH(op.targets(1),ti){
							if(distributeAttackTarget(AttackPoint(const_cast<terUnitBase*>(ti->unit_)),UNIT_ATTRIBUTE_OFFICER,true))
								ret_flag = true;
						}
					}
				}
				else {
					SquadSearchTargetsScanOp::const_iterator ti;
					FOR_EACH(op.targets(),ti){
						if(distributeAttackTarget(AttackPoint(const_cast<terUnitBase*>(ti->unit_))))
							attack_flag = true;
					}

					bool ret_flag = true;
					while(ret_flag){ // распределение целей по незадействованным юнитам
						ret_flag = false;
						FOR_EACH(op.targets(),ti){
							if(distributeAttackTarget(AttackPoint(const_cast<terUnitBase*>(ti->unit_)),UNIT_ATTRIBUTE_NONE,true))
								ret_flag = true;
						}
					}
				}

				if((!attack_flag && offensiveMode() || currentAttribute()->dynamicAttack) && wayPoints_.empty() && !patrolMode() && op.targets().size())
					repositionToAttack(AttackPoint(op.targets().front().unit_));

				targets_scan_timer.start(squad_targets_scan_period);
			}
			else {
//				SquadUnitList::iterator ui;
//				FOR_EACH(Units, ui)
//					(*ui)->setAttackPoint(NULL, false);
			}
		}
	}

	techniciansQuant();
}

void terUnitSquad::showDebugInfo()
{
	if(showDebugSquad.wayPoints){
		Vect3f posPrev = position();
		Vect2fVect::iterator i;
		FOR_EACH(wayPoints_, i){
			show_line(posPrev, To3D(*i), BLUE);
			posPrev = To3D(*i);
		}
	}

	if(showDebugSquad.position){
		show_vector(To3D(stablePose().trans), 5, GREEN);
		show_vector(position(), 4, RED);
		//show_vector(position() + to3D(average_position_offset, 0), 6, MAGENTA);
	}

	if(showDebugSquad.average_velocity)
		show_vector(position(), To3Dzero(average_velocity), BLUE);

	if(!Empty()){
		if(showDebugSquad.umbrella)
			show_vector(position(), currentAttribute()->fireRadiusMin(), BLUE);
	
		if(showDebugSquad.fire_radius)
			show_vector(position(), currentAttribute()->fireRadius(), RED);

		if(showDebugSquad.sight_radius)
			show_vector(position(), currentAttribute()->sightRadius(), YELLOW);

		if(showDebugSquad.optimalRadius)
			show_vector(position(), optimalRadius_, XCOL(RED, 200));
	}

	if(showDebugSquad.described_radius)
		show_vector(position(), radius(), GREEN);

	if(showDebugSquad.attackPoints){
		AttackPointList::iterator ai;
		FOR_EACH(attack_points, ai){
			show_line(position(),ai->position(),RED);
			show_vector(ai->position(),3,RED);
		}
	}

	if(showDebugSquad.linesNumber){
		XBuffer buf;
		buf < "Lines: " <= position_generator.numLines();
		show_text(position(), buf, BLUE);
	}

	if(showDebugSquad.attackAction && attackAction())
		show_text(position(), "Attack", BLUE);

	if(showDebugSquad.squadToFollow && squadToFollow_)
		show_line(position(), squadToFollow_->position(), MAGENTA);
}

/////////////////////////////////////////////////
//		AI Squad
/////////////////////////////////////////////////
terUnitSquad::AttackPoint::AttackPoint(terUnitBase* unit) : 
	unit_(unit), 
	squad_(unit_->attr().isLegionary() ? unit_->GetSquadPoint() : 0),
	position_(0,0,0),
	positionTarget_(false)
{
}

terUnitSquad::AttackPoint::AttackPoint(const Vect3f& position):
	unit_(NULL),
	squad_(NULL),
	position_(position),
	positionTarget_(true)
{
}

bool terUnitSquad::killBaseUnit(const UnitCommand& command)
{
	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui){
		if((*ui)->attr().ID == UNIT_ATTRIBUTE_SOLDIER + command.commandData()
			&& (*ui)->alive() && (*ui)->GetLegionMorphing() && (*ui)->damageMolecula().aliveElementCount() > 0){
				(*ui)->damageMoleculaKill();
				return true;
			}
	}

	return false;
}

void terUnitSquad::addSquad(terUnitSquad* squad)
{
	if(squad->currentMutation() != UNIT_ATTRIBUTE_NONE)
		return;
	
	while(!squad->Units.empty()){
		terUnitLegionary* unit = squad->Units.front();
		squad->removeUnit(unit);
		addUnit(unit, false);
	}
}

void terUnitSquad::ShowCircles() 
{
	if ( !Empty() ) {
		bool hasAliveUnit = false;
		SquadUnitList::iterator ui;
		FOR_EACH(Units, ui){
			terUnitLegionary* u=*ui;
			if(u->alive()){
				u->ShowCircles();
				if(u->inSquad()){
					hasAliveUnit = true;
				}
			}
		}
		if (hasAliveUnit) {
			terUnitBase::ShowCircles();
			float r = getAttackRadius();
			if (r > radius()) {
				if (avatar()) {
					terCircleShow(avatar()->pos0(), avatar()->pos1(), r, circleColors.fireRadius);
				} else {
					terCircleShow(position(), position(), r, circleColors.fireRadius);
				}
			}
		}
	}
}

float terUnitSquad::chargeLevel() const
{
//	MTL();
	return lastChargeLevel;
}

int terUnitSquad::attackClass() const
{
	int attackClass = 0;
	SquadUnitList::const_iterator ui;
	FOR_EACH(Units, ui)
		attackClass |= (*ui)->attr().AttackClass;
	return attackClass;
}

int terUnitSquad::attackEnemyClass() const
{
	int attackClass = 0;
	SquadUnitList::const_iterator ui;
	FOR_EACH(Units, ui) {
		//bad but developers have no time til release
		if ((*ui)->attr().ID != UNIT_ATTRIBUTE_TECHNIC) {
			attackClass |= (*ui)->attr().AttackClass;
		}
	}
	return attackClass;
}

float terUnitSquad::formationRadius() const
{
	if(isBase() || Empty())
		return attr().formationRadiusBase;
	return Units.front()->formationRadius();
}

void terUnitSquad::holdProduction()
{
	for(int i = 0; i < 3; i++)
		atomsPaused_[i] = 1;
}

void terUnitSquad::unholdProduction()
{
	for(int i = 0; i < 3; i++)
		atomsPaused_[i] = 0;
}

bool terUnitSquad::distributeAttackTarget(const AttackPoint& attack_point, terUnitAttributeID unit_id, bool for_free_units_only)
{
	bool ret_value = false;

	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui){
		if((unit_id == UNIT_ATTRIBUTE_NONE || (*ui)->attr().ID == unit_id) && !(*ui)->hasAttackTarget()){
			terUnitLegionary& unit = **ui;
			terUnitBase* t = attack_point.target(unit);

			if(t && checkAttackPosition(t->position())){
				ret_value = true;
				(*ui)->setAttackPoint(t, false);

				if(for_free_units_only || t->possibleDamage() >= t->damageMolecula().aliveElementCount())
					break;
			}
		}
	}

	if(for_free_units_only)
		return ret_value;

	FOR_EACH(Units, ui){
		if((unit_id == UNIT_ATTRIBUTE_NONE || (*ui)->attr().ID == unit_id) && (*ui)->needAttackTarget()){
			terUnitLegionary& unit = **ui;
			terUnitBase* t = attack_point.target(unit);

			if(t && checkAttackPosition(t->position())){
				ret_value = true;
				(*ui)->setAttackPoint(t, false);

				if(t->possibleDamage() >= t->damageMolecula().aliveElementCount())
					break;
			}
		}
	}

	return ret_value;
}

void terUnitSquad::techniciansQuant()
{
	if(currentMutation() != UNIT_ATTRIBUTE_NONE || technician_targets_scan_timer()) return;

	bool flag = false;

	SquadUnitList::iterator ui;
	FOR_EACH(Units, ui){
		if((*ui)->attr().ID == UNIT_ATTRIBUTE_TECHNIC && !(*ui)->hasAttackTarget() && (*ui)->isWeaponReady()){
			flag = true;
			break;
		}
	}

	technician_targets_scan_timer.start(squad_technician_targets_scan_period);
	if(!flag) return;

	float fire_radius = offensiveMode() && !patrolMode() ? currentAttribute()->sightRadius() : currentAttribute()->fireRadius();
	fire_radius += radius();
	SquadTechnicianSearchTargetsScanOp op(position2D(), fire_radius, *this);
	universe()->UnitGrid.Scan(position().x, position().y, fire_radius, op);
	op.sortTargets();

	if(op.targets().empty()) return;

	SquadTechnicianSearchTargetsScanOp::const_iterator ti = op.targets().begin();

	FOR_EACH(Units, ui){
		if((*ui)->attr().ID == UNIT_ATTRIBUTE_TECHNIC && !(*ui)->hasAttackTarget() && (*ui)->isWeaponReady()){
			(*ui)->setAttackPoint(const_cast<terUnitBase*>(ti->unit_), false);

			++ti;
			if(ti == op.targets().end()) break;
		}
	}
}

bool terUnitSquad::checkAttackPosition(const Vect3f& to) const
{
	float radius2 = sqr(currentAttribute()->fireRadius() + radius());
	if(position2D().distance2(Vect2f(to.x,to.y)) < radius2)
		return true;

	return false;
}

