#ifndef __IRONBULLET_H__
#define __IRONBULLET_H__

#include "LaunchData.h"

class terSoundController;


class AttributeProjectile : public AttributeReal
{
public:
	AttributeProjectile();

private:
	RigidBodyPrm rigidBodyPrm_;
};

/// Базовый класс для снарядов, ракет, бомб и т.д.
class terProjectileBase : public terUnitReal
{
public:
	terProjectileBase(const UnitTemplate& data);

	const AttributeProjectile& attr() const {
		return safe_cast_ref<const AttributeProjectile&>(__super::attr());
	}

	void Collision(terUnitBase* p);
	void AvatarQuant();

	void DestroyLink();

	void Kill();
	void Quant();

	void WayPointStart();
	void WayPointController();

	virtual void setSource(terUnitReal* p,const Vect3f& source,const Vect3f& speed);
	virtual void setTarget(terUnitBase* p,const Vect3f& target,float target_delta = 0.0f);

	bool needCrater() const { return BodyPoint->groundColliding(); }
	
	bool selectAble() const { return false; }

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);

protected:

	/// подтверждение столкновения - надо ли взрываться
	virtual bool confirmCollision(const terUnitBase* p) const;
	bool playMoveSound();

	const Vect3f& sourcePosition() const { return sourcePosition_; }

	const Vect3f& targetPosition() const { return targetPosition_; }
	void setTargetPosition(const Vect3f& pos){ targetPosition_ = pos; }

	const terUnitBase* target() const { return target_; }
	void targetCollision(){ if(target_) Collision(target_); }

	ChainID chainRequest() const;

private:

	terSoundController* movementSound_;

	Vect3f sourcePosition_;
	Vect3f targetPosition_;

	terUnitBase* target_;

	DurationTimer killTimer_;
};

class terProjectileBullet : public terProjectileBase
{
public:
	terProjectileBullet(const UnitTemplate& data);

	void setSource(terUnitReal* p,const Vect3f& source,const Vect3f& speed);

	terUnitBase* GetIgnoreUnit(){ return ownerUnit_; };

	void Quant();
	void WayPointStart();

protected:

	bool confirmCollision(const terUnitBase* p) const;
	const Vect3f& speed() const { return speed_; }

private:

	Vect3f speed_;
};

class terProjectileDebris : public terProjectileBullet
{
public:
	terProjectileDebris(const UnitTemplate& data);
	void WayPointStart();
};

class terProjectileDebrisCrater : public terProjectileDebris
{
public:
	terProjectileDebrisCrater(const UnitTemplate& data) : terProjectileDebris(data){};
	void explode();
};

class terProjectileUnderground : public terProjectileBase
{
public:
	terProjectileUnderground(const UnitTemplate& data);

	terUnitBase* GetIgnoreUnit(){ return ownerUnit_; };

	void Start();

	void Quant();
	void WayPointStart();
	void WayPointController();

	void setDamage(const DamageData& damage,terUnitBase* agressor = NULL){ 
		terProjectileBase::setDamage(damage,agressor); 
	}

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);

private:

	Vect2f lastCraterPosition_;
};

class terProjectileMissile : public terProjectileBase
{
public:
	terProjectileMissile(const UnitTemplate& data);

	void Quant();
	void WayPointController();

	void setTarget(terUnitBase* p,const Vect3f& target,float target_delta = 0.0f);
	terUnitBase* GetIgnoreUnit(){ return DockPoint; };

protected:

	bool confirmCollision(const terUnitBase* p) const;
	ChainID chainRequest() const;

private:
	bool started_;
};

class terProjectileScumStorm : public terProjectileBase
{
public:
	terProjectileScumStorm(const UnitTemplate& data);

	void WayPointStart();
	void WayPointController();

	void setTarget(terUnitBase* p,const Vect3f& target,float target_delta = 0.0f);

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);

private:

	DurationTimer freeMovementTimer_;
	bool freeMovement_;
};

#endif //__IRONBULLET_H__