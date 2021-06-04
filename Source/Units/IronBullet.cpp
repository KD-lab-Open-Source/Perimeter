#include "StdAfx.h"

#include "Universe.h"
#include "IronBullet.h"
#include "IronExplosion.h"

#include "Config.h"

#include "GridTools.h"
#include "Squad.h"
#include "EditArchive.h"
#include "XPrmArchive.h"
#include "BinaryArchive.h"

REGISTER_CLASS(AttributeBase, AttributeProjectile, "Снаряды");

AttributeProjectile::AttributeProjectile()
{
	UnitClass = UNIT_CLASS_MISSILE;
	
	lockInitialPosition = true;
	
	ExcludeCollision = EXCLUDE_COLLISION_BULLET;
	CollisionGroup = COLLISION_GROUP_ENEMY_ONLY | COLLISION_GROUP_REAL | COLLISION_GROUP_ENEMY;
}

terProjectileBase::terProjectileBase(const UnitTemplate& data) : terUnitReal(data),
	movementSound_(NULL),
	target_(NULL),
	sourcePosition_(Vect3f::ZERO),
	targetPosition_(Vect3f::ZERO)
{
	movementSound_ = realAvatar()->findSound(SOUND_MOVE);
}

void terProjectileBase::Collision(terUnitBase* p)
{
	if(confirmCollision(p)){
		explode();
		p->setDamage(damageData(),this);
		Kill();
	}
}

void terProjectileBase::AvatarQuant()
{
	terUnitReal::AvatarQuant();

	if(SightFactor < 0.001f)
		avatar()->Hide();
	else{
		avatar()->Show();
		realAvatar()->setSight(SightFactor);
		avatar()->setPose(BodyPoint->pose());

		if(dockMode() != DOCK_MODE_LOCK)
			playMoveSound();
	}
}

void terProjectileBase::WayPointController()
{
	terUnitReal::WayPointController();

	if(dockMode() == DOCK_MODE_LOCK){
		if(!DockPoint)
			Kill();
	}
}

void terProjectileBase::setSource(terUnitReal* p,const Vect3f& source,const Vect3f& speed)
{
	ownerUnit_ = p;
	sourcePosition_ = source;
}

void terProjectileBase::setTarget(terUnitBase* p,const Vect3f& target,float target_delta)
{
	target_ = p;

	if(p){
		targetPosition_ = p->position();
		p->addPossibleDamage(attr().estimatedDamage());
	}
	else
		targetPosition_ = target;

	if(target_delta > FLT_EPS){
		float angle = M_PI * 2.0f * terLogicRNDfrand();
		float radius = terLogicRNDfrnd() * target_delta;
		targetPosition_.x += radius * cos(angle);
		targetPosition_.y += radius * sin(angle);
	}

	if(attr().LifeTime)
		killTimer_.start(attr().LifeTime);
}

bool terProjectileBase::confirmCollision(const terUnitBase* p) const 
{ 
	if(alive() && Player->clan() != p->Player->clan())//isEnemy(p))
		return true;

	return false;
}

bool terProjectileBase::playMoveSound()
{
	if(movementSound_){
		movementSound_->play();
		return true;
	}

	return false;
}

void terProjectileBase::DestroyLink()
{
	terUnitReal::DestroyLink();

	if(target_ && !target_->alive())
		target_ = NULL;
}

void terProjectileBase::Kill()
{
	if(ownerUnit_)
		ownerUnit_->removeMissileReference(this);

	terUnitReal::Kill();

	if(target_)
		target_->addPossibleDamage(-attr().estimatedDamage());

	realAvatar()->GetModelPoint()->SetAttr(ATTRUNKOBJ_IGNORE);
}

void terProjectileBase::Quant()
{
	terUnitReal::Quant();

	if(alive() && killTimer_.was_started() && !killTimer_()){
		explode();
		Kill();
	}
}

void terProjectileBase::WayPointStart()
{
	terUnitReal::WayPointStart();
}

ChainID terProjectileBase::chainRequest() const
{
	if(dockMode() == DOCK_MODE_LOCK)
		return CHAIN_STOP;

	return CHAIN_MOVE;
/*	if(isMoving())
		return CHAIN_MOVE;
	else
		return CHAIN_STOP;*/
}

SaveUnitData* terProjectileBase::universalSave(SaveUnitData* baseData)
{
	SaveUnitProjectileData* data = castOrCreate<SaveUnitProjectileData>(baseData);
	terUnitReal::universalSave(data);

	data->target = target_;
	data->owner = ownerUnit_;

	data->sourcePosition = sourcePosition_;
	data->targetPosition = targetPosition_;

	data->killTimer = killTimer_();

	return data;
}

void terProjectileBase::universalLoad(const SaveUnitData* baseData)
{
	const SaveUnitProjectileData* data = safe_cast<const SaveUnitProjectileData*>(baseData);
	terUnitReal::universalLoad(data);

	data->target.setLink((terUnitBase*&)target_);
	data->owner.setLink((terUnitBase*&)ownerUnit_);

	sourcePosition_ = data->sourcePosition;
	targetPosition_ = data->targetPosition;

	if(data->killTimer)
		killTimer_.start(data->killTimer);
}

//----------------------------------------

terProjectileBullet::terProjectileBullet(const UnitTemplate& data) : terProjectileBase(data),
	speed_(Vect3f::ZERO)
{
}

void terProjectileBullet::setSource(terUnitReal* p,const Vect3f& source,const Vect3f& speed)
{
	terProjectileBase::setSource(p,source,speed);
	setPosition(source);

	speed_ = speed;
}

void terProjectileBullet::Quant()
{
	terProjectileBase::Quant();

	if(alive() && (BodyPoint->clusterColliding() || BodyPoint->groundColliding())){
		explode();
		Kill();
	}
}

void terProjectileBullet::WayPointStart()
{
	terProjectileBase::WayPointStart();

	if(ownerUnit_ && ownerUnit_->GetRigidBodyPoint())
		BodyPoint->startMissile(*(ownerUnit_->GetRigidBodyPoint()),sourcePosition(),targetPosition(),speed_);
}

bool terProjectileBullet::confirmCollision(const terUnitBase* p) const
{
	if(terProjectileBase::confirmCollision(p) && BodyPoint && BodyPoint->missileStarted())
		return true;

	return false;
}

//-----------------------------------------------

terProjectileDebris::terProjectileDebris(const UnitTemplate& data) : terProjectileBullet(data)
{
}

void terProjectileDebris::WayPointStart()
{
	terProjectileBullet::WayPointStart();

	BodyPoint->startDebris(sourcePosition(),speed());
}

void terProjectileDebrisCrater::explode()
{
	if(BodyPoint->groundColliding()){
		terUnitBase* p = Player->buildUnit(UNIT_ATTRIBUTE_DEBRIS_CRATER);
		p->setPose(Se3f(QuatF::ID, position()), true);
		p->setRadius(radius());
		p->Start();
	}
}

//-----------------------------------------------

terProjectileMissile::terProjectileMissile(const UnitTemplate& data) : terProjectileBase(data)
{
	started_ = false;
}

ChainID terProjectileMissile::chainRequest() const
{
	return terProjectileBase::chainRequest();
}

void terProjectileMissile::Quant()
{
	terProjectileBase::Quant();

	if(dockMode() != DOCK_MODE_LOCK){
		if(!started_ && DockPoint){
			started_ = true;
			BodyPoint->startRocket(*DockPoint->GetRigidBodyPoint());
		}
		if(alive() && (BodyPoint->clusterColliding() || BodyPoint->groundColliding())){
			explode();
			Kill();
		}
	}
}

void terProjectileMissile::WayPointController()
{
	terProjectileBase::WayPointController();

	if(dockMode() != DOCK_MODE_LOCK){
		if(dockMode() == DOCK_MODE_LEAVE){
			if(!DockPoint)
				setDockMode(DOCK_MODE_NONE);
			else{
				DockPhase.Quant();
				if(DockPhase.isEnd())
					clearDockingPoint();
			}
		}

		BodyPoint->way_points.clear();
		if(target()){
			setTargetPosition(target()->position());
			BodyPoint->way_points.push_back(targetPosition());
		}
	}
}

void terProjectileMissile::setTarget(terUnitBase* p,const Vect3f& target,float target_delta)
{
	terProjectileBase::setTarget(p,target,target_delta);

	DockPhase.startPhase(1.0f,-0.25f,0);

	setDockMode(DOCK_MODE_LEAVE);

	//BodyPoint->startRocket(*DockPoint->GetRigidBodyPoint());
}

bool terProjectileMissile::confirmCollision(const terUnitBase* p) const
{
	if(terProjectileBase::confirmCollision(p) && dockMode() != DOCK_MODE_LOCK)
		return true;

	return false;
}

//--------------------------------------------------------

terProjectileUnderground::terProjectileUnderground(const UnitTemplate& data) : terProjectileBase(data)
{
	lastCraterPosition_ = Vect2f(0,0);
}

void terProjectileUnderground::Start()
{
	terProjectileBase::Start();
}

void terProjectileUnderground::Quant()
{
	terProjectileBase::Quant();

	if(attr().effectsData.movementCraterID != UNIT_ATTRIBUTE_NONE){
		Vect2f pos = position2D();
		if(lastCraterPosition_.distance2(position2D()) >= sqr(attr().effectsData.movementCraterDelta)){
			lastCraterPosition_ = position2D();

			terUnitBase* p = Player->buildUnit(attr().effectsData.movementCraterID);
			p->setPose(Se3f(QuatF::ID,position()),true);
			p->Start();

			explode();
		}
	}

//	toolzer_.quant(position().xi(),position().yi());

	if(target()){
		float radius2 = sqr(target()->radius());
		if(position2D().distance2(Vect2f(targetPosition().x,targetPosition().y)) <= radius2)
			targetCollision();
	}
	else {
		if(wayPoints().empty()){
			explode();
			Kill();
		}
	}

}

SaveUnitData* terProjectileUnderground::universalSave(SaveUnitData* baseData)
{
	SaveUnitProjectileUndergroundData* data = castOrCreate<SaveUnitProjectileUndergroundData>(baseData);
	terProjectileBase::universalSave(data);

	data->sourcePosition = position();
	data->lastCraterPosition = lastCraterPosition_;

	return data;
}

void terProjectileUnderground::universalLoad(const SaveUnitData* baseData)
{
	const SaveUnitProjectileUndergroundData* data = safe_cast<const SaveUnitProjectileUndergroundData*>(baseData);
	terProjectileBase::universalLoad(data);

	lastCraterPosition_ = data->lastCraterPosition;
}

void terProjectileUnderground::WayPointStart()
{
	terProjectileBase::WayPointStart();

	BodyPoint->startUnderGroundMissile();
	setPose(Se3f(orientation(), sourcePosition()), true);

	wayPoints().push_back(targetPosition());
//	BodyPoint->way_points.push_back(targetPosition());

	lastCraterPosition_ = position2D();

	toolzerController_.start();
}

void terProjectileUnderground::WayPointController()
{
	terProjectileBase::WayPointController();

	BodyPoint->way_points.clear();
	if(target()){
		setTargetPosition(target()->position());

		wayPoints_.clear();
		wayPoints().push_back(targetPosition());
	}

	if(!wayPoints().empty()){
		if(BodyPoint->is_point_reached(wayPoints().front()))
			wayPoints_.erase(wayPoints_.begin());
		else
			moveToPoint(wayPoints().front());
//		BodyPoint->way_points.push_back(targetPosition());
	}
}

//--------------------------------------------------------

terProjectileScumStorm::terProjectileScumStorm(const UnitTemplate& data) : terProjectileBase(data)
{
	freeMovement_ = false;
}

void terProjectileScumStorm::WayPointStart()
{
	terProjectileBase::WayPointStart();
}

void terProjectileScumStorm::setTarget(terUnitBase* p,const Vect3f& target,float target_delta)
{
	terProjectileBase::setTarget(p,target,target_delta);

	wayPoints_.push_back(targetPosition());
}

void terProjectileScumStorm::WayPointController()
{
	terProjectileBase::WayPointController();

	BodyPoint->way_points.clear();

	if(!wayPoints().empty()){
		if(BodyPoint->is_point_reached(wayPoints().front())){
			wayPoints_.erase(wayPoints_.begin());
			freeMovement_ = true;
		}
		else
			moveToPoint(wayPoints().front());
	}

	if(freeMovement_ && !freeMovementTimer_()){
		freeMovementTimer_.start(terScumStormTurnDelay);

		Vect3f pos = position();
		float angle = BodyPoint->velocity().psi();

		angle += terScumStormTurnAngle + terLogicRNDfrnd()*terScumStormTurnAngleDelta;
		pos.x += 100.0f * cos(angle);
		pos.y += 100.0f * sin(angle);

		wayPoints_.clear();
		wayPoints_.push_back(pos);
		moveToPoint(wayPoints().front());
	}
}

SaveUnitData* terProjectileScumStorm::universalSave(SaveUnitData* baseData)
{
	SaveUnitScumStormData* data = castOrCreate<SaveUnitScumStormData>(baseData);
	terProjectileBase::universalSave(data);

	data->freeMovementTimer = freeMovementTimer_();
	data->freeMovement = freeMovement_;

	return data;
}

void terProjectileScumStorm::universalLoad(const SaveUnitData* baseData)
{
	const SaveUnitScumStormData* data = safe_cast<const SaveUnitScumStormData*>(baseData);
	terProjectileBase::universalLoad(data);

	if(data->freeMovementTimer)
		freeMovementTimer_.start(data->freeMovementTimer);

	freeMovement_ = data->freeMovement;
}

