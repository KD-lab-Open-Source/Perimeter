#include "StdAfx.h"
#include "Player.h"
#include "RealInterpolation.h"
#include "Universe.h"

#include "Squad.h"
#include "SecondLegion.h"
#include "PerimeterSound.h"
#include "Triggers.h"
#include "AIPrm.h"
#include "EditArchive.h"
#include "XPrmArchive.h"
#include "BinaryArchive.h"

REGISTER_CLASS(AttributeBase, AttributeLegionary, "Легионер");

AttributeLegionary::AttributeLegionary()
{
	is_base_unit = 0;

	formationRadiusFactor = 1.2f;

	dynamicAttack = 0;
	attackTurnRadius = 150;
	attackAngleDeviation = 0;

	ignoreUninstalledFrame = 0;
	dontUseDamageMapPathFind = 0;

	LegionType = LEGION_GROUND;
	destroyZeroLayer = true;
    
    MilitaryUnit = true;
		
	CollisionGroup = COLLISION_GROUP_ENEMY | COLLISION_GROUP_REAL;
}

terUnitLegionary::terUnitLegionary(const UnitTemplate& data) : terUnitReal(data)
{
	localPosition_ = Vect2f::ZERO;
	speed_factor = 1.0f;
	attackTarget_ = 0;
	inSquad_ = false;
	isDisintegrating_ = false;

	int composition[] = { attr().damageMolecula[0], attr().damageMolecula[1], attr().damageMolecula[2] };
	BodyPoint->setComposition(composition);

	requestStatus_ = 0;

	SquadPoint = NULL;
	fireStatus_ = 0;

	transportPoint_ = NULL;

	MoveSoundPoint = realAvatar()->findSound(SOUND_MOVE);

	manualAttackTarget_ = false;
	hasAttackPosition_ = false;
	attackPosition_ = Vect3f(0,0,0);
	
	SpeedFactor = 0;
	deltaPath_ = 0;

	movementConsumption_ = 0;
	if(attr().movementConsumption.enabled()){
		movementConsumption_ = new EnergyConsumer;
		movementConsumption_->attach(Player, attr().movementConsumption);
	}
}

void terUnitLegionary::SetSquadPoint(terUnitSquad* squad)
{
/*
	if(squad){
		if(!SquadPoint){
			terUniverse* tu=universe();
			CSELECT_AUTOLOCK();
			UnitList::iterator i_unit = tu->select.findInSelection(this);
			if(i_unit != tu->select.GetSelectList().end()){
				tu->select.SelectSquad(squad);
			}
		}
	}
*/
	SquadPoint = squad;
}

void terUnitLegionary::DestroyLink()
{
	terUnitReal::DestroyLink();

	if(SquadPoint && !(SquadPoint->alive()))
		SquadPoint = 0;

	if(attackTarget_ && (!attackTarget_->alive() || !attackTarget_->damageMolecula().isAlive()))
		attackTarget_ = 0;

	if(transportPoint_ && !transportPoint_->alive())
		transportPoint_ = 0;
}

void terUnitLegionary::AvatarQuant()
{
	terUnitReal::AvatarQuant();
	
	if(SightFactor < 0.001f)
		avatar()->Hide();
	else{
		avatar()->setPose(BodyPoint->pose());

		if(BodyPoint->underMutation())
			return;
		
		avatar()->Show();
		realAvatar()->setSight(SightFactor);
		realAvatar()->setHeal(HealFactor);
		realAvatar()->setFreeze(FreezeFactor);
		realAvatar()->setHot(HotFactor);

		if(MoveSoundPoint){
			MoveSoundPoint->setVolume(SpeedFactor);
			MoveSoundPoint->setFrequency(SpeedFactor);

			if(SpeedFactor > 0.001f)
				MoveSoundPoint->play();
		}
	}
}

void terUnitLegionary::Start()
{ 
	terUnitReal::Start();

	setCollisionGroup(collisionGroup() & ~COLLISION_GROUP_REAL);
	setCollisionGroup(collisionGroup() | COLLISION_GROUP_ENEMY);
}

float terUnitLegionary::formationRadius() const
{
	return !attr().is_base_unit ? radius()*attr().formationRadiusFactor : getSquad()->attr().formationRadiusBase;
}

void terUnitLegionary::finalizeConstruction()
{ 
	clearDockingPoint();

	setCollisionGroup(collisionGroup() | COLLISION_GROUP_REAL);
	
	switch(attr().ID){
	case UNIT_ATTRIBUTE_TECHNIC:
	case UNIT_ATTRIBUTE_OFFICER:
	case UNIT_ATTRIBUTE_SOLDIER:
		soundEvent(SOUND_VOICE_UNIT_READY);
	break;
	}
}

void terUnitLegionary::setRequestStatus(int request)
{
	if(request & (LEGION_ACTION_STATUS_INTEGRATE | LEGION_ACTION_STATUS_DISINTEGRATE)){
		BodyPoint->setUnderMutation(1);
		setCollisionGroup(collisionGroup() & ~COLLISION_GROUP_REAL);
	}

//	if(request & LEGION_ACTION_STATUS_PRODUCTION)
//		CollisionGroup &= ~COLLISION_GROUP_REAL;

	requestStatus_ |= request;
}

void terUnitLegionary::WayPointController()
{
	terUnitReal::WayPointController();

	responseStatus_ = 0;
	fireStatus_ = 0;

	if(dockMode() != DOCK_MODE_NONE)
		return;

	requestStatus_ &= ~(LEGION_ACTION_STATUS_ATTACK | LEGION_ACTION_STATUS_MOVE);

	BodyPoint->way_points.clear();
	BodyPoint->setVelocityFactor(1);

	if(requestStatus() & LEGION_ACTION_STATUS_INTEGRATE){
		if(SquadPoint)
			responseStatus_ |= LEGION_ACTION_STATUS_INTEGRATE;

		if(RequestScriptIntegrate()){
			requestStatus_ &= ~LEGION_ACTION_STATUS_INTEGRATE;
			setCollisionGroup(collisionGroup() | COLLISION_GROUP_REAL);

			if(SquadPoint)
				soundEvent(SOUND_VOICE_SQUAD_TRANSFORM_FINISHED);
		}
		return;
	}

	if(requestStatus() & LEGION_ACTION_STATUS_DISINTEGRATE){
		if(SquadPoint)
			responseStatus_ |= LEGION_ACTION_STATUS_DISINTEGRATE;

		if(RequestScriptDisintegrate())
			Kill();
		return;
	}

//	if(requestStatus() & LEGION_ACTION_STATUS_FREEZE){
//		if(!RequestScriptFreeze())
//			requestStatus_ &= ~LEGION_ACTION_STATUS_FREEZE;
//		return;
//	}

	if(SquadPoint){
		if(transportPoint_){
			requestStatus_ |= LEGION_ACTION_STATUS_MOVE;
			if(RequestScriptMove()){
				BodyPoint->way_points.push_back(transportPoint_->position());
				BodyPoint->setVelocityFactor(BodyPoint->velocityFactor()*speed_factor);
				responseStatus_ |= LEGION_ACTION_STATUS_MOVE;
			}

			float dist = transportPoint_->radius() * 3.0f;
			if(transportPoint_->position2D().distance2(position2D()) < dist * dist && SquadPoint->addToTransport(this)){
				Kill();
				transportPoint_ = NULL;
				return;
			}
		}
		else {
			if(attr().is_base_unit && SquadPoint->isTransport())
				transportPoint_ = SquadPoint->RequestTransportPoint(this);

			if(!manualAttackTarget_ && attackTarget_ && attackTarget_->isBuilding() && attackTarget_->Player->isWorld())
				setAttackPoint(NULL);

			if(attackTarget_){
				requestStatus_ |= LEGION_ACTION_STATUS_ATTACK;
				if(fireRequest(&attackTarget_->position(),attackTarget_,fireStatus_)){
					responseStatus_ |= LEGION_ACTION_STATUS_ATTACK;
				}
				else {
					if(fireStatus_ & (LEGION_FIRE_STATUS_GROUND_OBSTACLE | LEGION_FIRE_STATUS_FRIENDLY_FIRE | LEGION_FIRE_STATUS_BORDER_ANGLE | LEGION_FIRE_STATUS_DISTANCE | LEGION_FIRE_STATUS_FIELD_OBSTACLE | LEGION_FIRE_STATUS_BAD_TARGET))
						setAttackPoint(NULL);
					if(fireStatus_ & LEGION_FIRE_STATUS_NOT_ENEMY_TARGET && !manualAttackTarget_)
						setAttackPoint(NULL);
				}
			}
			if(hasAttackPosition_){
				requestStatus_ |= LEGION_ACTION_STATUS_ATTACK;
				if(fireRequest(&attackPosition_,NULL,fireStatus_)){
					responseStatus_ |= LEGION_ACTION_STATUS_ATTACK;
				}
				else {
					if(fireStatus_ & (LEGION_FIRE_STATUS_GROUND_OBSTACLE | LEGION_FIRE_STATUS_FRIENDLY_FIRE | LEGION_FIRE_STATUS_BORDER_ANGLE | LEGION_FIRE_STATUS_DISTANCE | LEGION_FIRE_STATUS_FIELD_OBSTACLE | LEGION_FIRE_STATUS_BAD_TARGET))
						hasAttackPosition_ = false;
				}
			}

			if(!out_path.empty()){	
				requestStatus_ |= LEGION_ACTION_STATUS_MOVE;
				if(RequestScriptMove()){
					BodyPoint->way_points.push_back(To3D(out_path.front()));
					if(out_path.size() > 1){
						BodyPoint->way_points.push_back(To3D(out_path[1]));
						BodyPoint->setVelocityFactor(BodyPoint->velocityFactor()*speed_factor);
					}
					//else if(attackTarget_ && isFlying()){
					//	BodyPoint->way_points[0] = To3D(planeAttackPoint());
					//	BodyPoint->way_points.push_back(BodyPoint->way_points[0]);
					//	BodyPoint->setVelocityFactor(BodyPoint->velocityFactor());
					//}
					responseStatus_ |= LEGION_ACTION_STATUS_MOVE;
				}
			}
		}
	}
}

void terUnitLegionary::Quant()
{
	terUnitReal::Quant();

	average(SpeedFactor, BodyPoint->speedFactor(), 0.2f);

	if(attackTarget_ && !targetEventTimer_){
		targetEventTimer_.start(targetEventTime + terLogicRND(targetEventTime));
		universe()->checkEvent(EventUnitMyUnitEnemy(Event::AIM_AT_OBJECT, attackTarget_, this));
	}
	
//	if(FieldCluster::get_player_id(field_dispatcher->getIncludingCluster(position())) != Player->playerID())
//		if(!is_base_unit)
//			requestStatus_ |= LEGION_ACTION_STATUS_DISINTEGRATE;


	//bool isMoving = getSquad() && !getSquad()->noWayPoints();
	switch(attr().LegionType){
	case LEGION_FLYING:
		if(!isMoving())
			BodyPoint->setFlyingMode(1); // 0
		break;
	case LEGION_SUBTERRANEAN: {
		bool moving = true; //getSquad() && (!getSquad()->noWayPoints() || !getSquad()->noTargets());//isMoving();
		if(moving && movementConsumption_){
			movementConsumption_->requestCharge();
			if(!movementConsumption_->charged()){
				BodyPoint->stopMovement();
				//moving = false;
			}
		}

		if(!moving){
			if(BodyPoint->diggingMode() && (attr().destroyZeroLayer || !vMap.checkZeroLayer(position().xi(),position().yi()))){
				toolzerController_.requestPhase(TOOLZER_PHASE_END_MOVE);
				BodyPoint->setDiggingMode(0);
			}
		}

		if(BodyPoint->underGround())
			setUnitClass(UNIT_CLASS_UNDERGROUND);
		else
			setUnitClass(attr().UnitClass);

	}	break;
	}
}

void terUnitLegionary::setDamage(const DamageData& damage,terUnitBase* p)
{
	terUnitReal::setDamage(damage,p);

	if(SquadPoint)
		soundEvent(SOUND_VOICE_SQUAD_UNDER_ATTACK);
}

void terUnitLegionary::Kill()
{
	if(SquadPoint){
		SquadPoint->removeUnit(this);
		SquadPoint = 0;
	}
	
	transportPoint_ = 0;

	terUnitReal::Kill();
}

void terUnitLegionary::UnitDamageCheck()
{
	if(alive() && !damageMolecula().isAlive() && !(requestStatus() & (LEGION_ACTION_STATUS_INTEGRATE | LEGION_ACTION_STATUS_DISINTEGRATE))){
		explode();
		Kill();
	}
}

int terUnitLegionary::RequestScriptFreeze()
{
	if(FreezeFactor > FLT_EPS)
		return 0;
	return 1;
}

//-------------------------------------------
void terUnitLegionary::addWayPoint(const Vect2f& p)
{ 
	out_path.push_back(p); 
}

void terUnitLegionary::stop()
{
	out_path.clear();
}

void terUnitLegionary::setLocalPosition(const Vect2f& p)
{
	localPosition_ = p;
}

float terUnitLegionary::deltaPath() const
{
	if(out_path.empty())
		return 0;
	//return dot(out_path.front() - position2D(), getSquad()->forwardDirection());
	
	deltaPath_ = 0;
	Vect2f v = position2D();
	Vect2fVect::const_iterator vi;
	FOR_EACH(out_path, vi){
		deltaPath_ += vi->distance2(v);
		v = *vi;
	}
	return deltaPath_;
}

void terUnitLegionary::correctSpeed(float averagePath)
{
	//BodyPoint->setForwardVelocity(velocity_min);
	//float dk = -deltaPath()/BodyPoint->forwardVelocity()*squad_speed_correction_relaxation_time_inv;
	if(inSquad()){
		float dk = deltaPath_/averagePath;
		dk = clamp(dk, 1 - squad_speed_factor_modulation, 1 + squad_speed_factor_modulation);
		average(speed_factor, dk, squad_speed_correction_relaxation_time_inv);
	}
	else
		speed_factor = 1 + squad_speed_factor_modulation;
}

class SquadPrmAttackScanOp
{
	terUnitLegionary* unit1;
	float radius_min2;
	float radius_max2;
	terUnitBase* best_target;
	float min_distance2;
public:
	SquadPrmAttackScanOp(terUnitLegionary* unit1_, float radius_min, float radius_max)
		: unit1(unit1_), radius_min2(sqr(radius_min)), radius_max2(sqr(radius_max)), 
		best_target(0), min_distance2(FLT_INF) {}

	void operator()(terUnitBase* unit2)
	{
		if(!unit1->isEnemy(unit2) || !unit1->checkFireClass(unit2)) // нельзя стрелять
			return;
		if(unit1->includingCluster() != unit2->includingCluster()) // закрыт куполом
			return;
		if(unit2->isUnseen())
			return;

		float dist2 = unit1->position().distance2(unit2->position());
		if(dist2 >= radius_min2 && dist2 <= radius_max2 && min_distance2 < dist2)
		{
			min_distance2 = dist2;
			best_target = unit2;
		}
	}

	const terUnitBase* result()	
	{ 
		if(best_target && !unit1->GetFireControlCheck(best_target))
		{
			best_target = 0;
		}

		return best_target; 
	}
};

void terUnitLegionary::setAttackPoint(terUnitBase* attack_target, bool is_manual)
{
	if(attackTarget_)
		attackTarget_->addPossibleDamage(-estimatedDamage());

	attackTarget_ = attack_target;

	if(attackTarget_)
		attackTarget_->addPossibleDamage(estimatedDamage());

	manualAttackTarget_ = is_manual;
	hasAttackPosition_ = false;

	targetEventTimer_.stop();
}

void terUnitLegionary::setAttackPosition(const Vect3f& attack_position,bool is_manual)
{
	setAttackPoint(NULL);

	attackPosition_ = attack_position;
	manualAttackTarget_ = is_manual;
	hasAttackPosition_ = true;
}

void terUnitLegionary::quant()
{
	//xassert(inSquad() || !out_path.empty());
	if(!out_path.empty() && BodyPoint->is_point_reached(out_path.front())){
		setInSquad();
		out_path.erase(out_path.begin());
	}
}

void terUnitLegionary::setInSquad()
{
	if(!inSquad()){
		inSquad_ = true;
		DamageMolecula atom(attr().damageMolecula);
		atom += transportAtom();
		getSquad()->addSquadMutationMolecula(atom);
	}
}

void terUnitLegionary::addToTransport(int atom_id, int numAtoms)
{ 
	transportAtom_[atom_id] += numAtoms; 
	if(getSquad()){
		DamageMolecula atom;
		atom[atom_id] += numAtoms; 
		getSquad()->addSquadMutationMolecula(atom);
	}
}

void terUnitLegionary::showDebugInfo()
{
	terUnitReal::showDebugInfo();

	if(showDebugLegionary.speed_factor){
		XBuffer buf;
		buf.SetDigits(2);
		buf <= speed_factor;
		show_text(position(), buf, RED);
	}
	
	if(showDebugLegionary.attack_point && attackTarget_ && getSquad())
		show_line(position(), attackTarget_->position(), RED);
	
	if(showDebugLegionary.way_point && !out_path.empty() && getSquad()){
		show_line(position(), To3D(out_path.front()), !inSquad() ? MAGENTA : BLUE);
		show_vector(To3D(out_path.front()),2,BLUE);

		show_line(position(), To3D(out_path.front()), CYAN);
		show_vector(To3D(out_path.front()),2,CYAN);
	}

	if(showDebugLegionary.invisibility && isUnseen())
		show_text(position(), "Invisible", CYAN);
}

//---------------------------------------------
const float PERIMETER_CHECK_FILED_SAMPLE = 32.0f;

int CheckFieldLine(const Vect3f& from,const Vect3f& v,float d,int cluster_id)
{
	if(d > PERIMETER_CHECK_FILED_SAMPLE*2){
		int num = d / PERIMETER_CHECK_FILED_SAMPLE;
		Vect3f dv = v;
		dv -= from;
		dv *= 1.0f / (float)num;
		Vect3f p = from;
		for(int i = 0;i < num;i++){
			if(FieldCluster::get_cluster_id(field_dispatcher->getIncludingCluster(Vect3f(vMap.XCYCL(round(p.x)),vMap.YCYCL(round(p.y)),p.z))) != cluster_id)
				return 0;
			p += dv;
		}
	}
	return 1;
}


const int PERIMETER_PATH_FIND_SHOOT_SHIFT = 10;

int CheckShotLine(const Vect3f& from,const Vect3f& to)
{
	int x,y,z;
	int dx,dy,dz;
	int step,max_step;	
	int x0,y0,x1,y1,z0,z1;
	
	x0 = round(from.x) >> kmGrid;
	y0 = round(from.y) >> kmGrid;
	z0 = round(from.z);
	
	x1 = round(to.x) >> kmGrid;
	y1 = round(to.y) >> kmGrid;
	z1 = round(to.z);
	
	dx = x1 - x0;
	dy = y1 - y0;
	dz = z1 - z0;
	
	if(!dx && !dy)
		return 1;
	
	if(abs(dx) > abs(dy)){
		if(dx > 0){
			max_step = dx;
			dy = (dy << PERIMETER_PATH_FIND_SHOOT_SHIFT) / max_step;
			dz = (dz << PERIMETER_PATH_FIND_SHOOT_SHIFT) / max_step;
			dx = 1 << PERIMETER_PATH_FIND_SHOOT_SHIFT;
		}else{
			max_step = -dx;
			dy = (dy << PERIMETER_PATH_FIND_SHOOT_SHIFT) / max_step;
			dz = (dz << PERIMETER_PATH_FIND_SHOOT_SHIFT) / max_step;
			dx = -(1 << PERIMETER_PATH_FIND_SHOOT_SHIFT);
		}
	}else{
		if(dy > 0){
			max_step = dy;
			dx = (dx << PERIMETER_PATH_FIND_SHOOT_SHIFT) / max_step;
			dz = (dz << PERIMETER_PATH_FIND_SHOOT_SHIFT) / max_step;
			dy = 1 << PERIMETER_PATH_FIND_SHOOT_SHIFT;
		}else{
			max_step = -dy;
			dx = (dx << PERIMETER_PATH_FIND_SHOOT_SHIFT) / max_step;
			dz = (dz << PERIMETER_PATH_FIND_SHOOT_SHIFT) / max_step;
			dy = -(1 << PERIMETER_PATH_FIND_SHOOT_SHIFT);
		}
	}
	
	step = max_step;
	x = x0 << PERIMETER_PATH_FIND_SHOOT_SHIFT;
	y = y0 << PERIMETER_PATH_FIND_SHOOT_SHIFT;
	z = z0 << PERIMETER_PATH_FIND_SHOOT_SHIFT;
	
	while(step > 0){
		if(vMap.GVBuf[vMap.offsetGBufC(x >> PERIMETER_PATH_FIND_SHOOT_SHIFT, y >> PERIMETER_PATH_FIND_SHOOT_SHIFT)] > (z >> PERIMETER_PATH_FIND_SHOOT_SHIFT))
			return 0;
		
		x += dx;
		y += dy;
		z += dz;
		step--;
	}
	return 1;
}

ChainID terUnitLegionary::chainRequest() const
{
	ChainID id = terUnitReal::chainRequest();
	if(id != CHAIN_NONE) 
		return id;

	if(attr().LegionType == LEGION_SUBTERRANEAN && !BodyPoint->diggingMode() && BodyPoint->onGround())
		return CHAIN_STOP;

	if(isMoving())
		return CHAIN_MOVE;
	else
		return CHAIN_STOP;
}

int terUnitLegionary::RequestScriptMove()
{
	switch(attr().LegionType){
	case LEGION_FLYING:
		BodyPoint->setFlyingMode(1);
		break;
	case LEGION_GROUND:
		toolzerController_.requestPhase(TOOLZER_PHASE_MOVE);
		break;
	case LEGION_SUBTERRANEAN:
		if(!BodyPoint->diggingMode())
			toolzerController_.requestPhase(TOOLZER_PHASE_START_MOVE);
		else {
			if(toolzerController_.requestedPhase() == TOOLZER_PHASE_END_MOVE)
				toolzerController_.requestPhase(TOOLZER_PHASE_MOVE);
		}

		BodyPoint->setDiggingMode(1);
		break;
	}

	return 1;
}

void terUnitLegionary::setInvisibility(int time)
{
	time += invisibilityTimer_();
	invisibilityTimer_.start(min(time,terInvisibilityTimeMax));
}

SaveUnitData* terUnitLegionary::universalSave(SaveUnitData* baseData)
{
	SaveUnitLegionaryData* data = castOrCreate<SaveUnitLegionaryData>(baseData);
	terUnitReal::universalSave(data);
	
	data->transportedSoldiers = transportAtom()[MUTATION_ATOM_SOLDIER];
	data->transportedOfficers = transportAtom()[MUTATION_ATOM_OFFICER];
	data->transportedTechnics = transportAtom()[MUTATION_ATOM_TECHNIC];

	data->flyingMode = BodyPoint->flyingMode();
	data->diggingMode = BodyPoint->diggingMode();

	data->inSquad = inSquad();
	data->localPosition = localPosition();
	data->localPositionValid = true;
	data->wayPoints = out_path;

	return data;
}

void terUnitLegionary::universalLoad(const SaveUnitData* baseData)
{
	const SaveUnitLegionaryData* data = safe_cast<const SaveUnitLegionaryData*>(baseData);

	if(data->flyingMode)
		BodyPoint->setFlyingMode(true);

	if(data->diggingMode){
		BodyPoint->setDiggingMode(true);
		toolzerController_.requestPhase(TOOLZER_PHASE_MOVE);
	}

	terUnitReal::universalLoad(data);

	addToTransport(MUTATION_ATOM_SOLDIER, data->transportedSoldiers);
	addToTransport(MUTATION_ATOM_OFFICER, data->transportedOfficers);
	addToTransport(MUTATION_ATOM_TECHNIC, data->transportedTechnics);

	if(data->inSquad)
		setInSquad();
	if(data->localPositionValid)
		localPosition_ = data->localPosition;
	out_path = data->wayPoints;
}

bool terUnitLegionary::selectAble() const
{
	if(!getSquad())
		return false;
	return terUnitReal::selectAble();
}

