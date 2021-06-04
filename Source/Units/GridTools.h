#ifndef __GRIDTOOLS_H__
#define __GRIDTOOLS_H__

#include "LaunchData.h"

struct Segment
{
	Vect3f begin; // начало орезка
	Vect3f direct; // единичное направление
	float length; // длина

	void set(const Vect3f& begin_, const Vect3f& end)
	{
		begin = begin_;
		direct.sub(end, begin);
		length = direct.norm();
		if(length > FLT_EPS)
			direct /= length;
	}

	void set(const Vect3f& begin_, const Vect3f& end,float d)
	{
		begin = begin_;
		direct.sub(end, begin);
		length = d;
		if(length > FLT_EPS)
			direct /= length;
	}
	
	int intersection(const Vect3f& point, float radius)
	{
		Vect3f dr, n;
		dr.sub(point, begin);
		float l = dot(dr, direct);
		if(l < -radius || l > length + radius)
			return 0;
		n.scaleAdd(dr, direct, -l);
		if(n.norm2() > (radius * radius))
			return 0;
		return 1;
	}
};

struct terUnitGridFireCheckOperator
{
	int CollisionGroup;
	Segment CheckProcess;
	terUnitBase* Contact;
	const terUnitBase* Owner;

	terUnitGridFireCheckOperator(const Vect3f& from,const Vect3f& to,float d,int collision_group,const terUnitBase* owner)
	{
		CollisionGroup = collision_group;
		Owner = owner;
		Contact = NULL;
		CheckProcess.set(from,to,d);
	}

	int operator()(terUnitBase* p)
	{
		if(p->alive() && (CollisionGroup & p->collisionGroup()) && p != Owner && CheckProcess.intersection(p->position(),p->radius())){
			Contact = (terUnitBase*)(p);
			return 0;
		}
		return 1;
	}
};

struct terUnitGridFireCheckIgnoreOperator : terUnitGridFireCheckOperator
{
	terUnitBase* IgnorePoint;

	terUnitGridFireCheckIgnoreOperator(const Vect3f& from,const Vect3f& to,float d,int collision_group,const terUnitBase* owner)
		: terUnitGridFireCheckOperator(from,to,d,collision_group,owner)
	{
		IgnorePoint = NULL;
	}

	int operator()(terUnitBase* p)
	{
		if(p->alive() && (CollisionGroup & p->collisionGroup()) && p != Owner && CheckProcess.intersection(p->position(),p->radius())){
			if(!IgnorePoint && p->attr().IgnoreTargetTrace)
				IgnorePoint = (terUnitBase*)p;
			else{
				Contact = (terUnitBase*)(p);
				return 0;
			}
		}
		return 1;
	}
};

//---------------------------------------

struct terUnitGridPenetrationDamageOperator
{
	int CollisionGroup;
	Segment CheckProcess;
	terUnitBase* Owner;
	DamageData Damage;

	terUnitGridPenetrationDamageOperator(const Vect3f& from,Vect3f& to,float d,int collision_group,terUnitBase* owner,const DamageData& damage)
	{
		CollisionGroup = collision_group;
		Owner = owner;
		CheckProcess.set(from,to,d);
		Damage = damage;
	}

	void operator()(terUnitBase* p)
	{
		if(p->alive() && (CollisionGroup & p->collisionGroup()) && Owner->isEnemy(p) && CheckProcess.intersection(p->position(),p->radius()))
			p->setDamage(Damage,Owner);
	}
};

//---------------------------------------

class terUnitGridTeamOffensiveOperator
{
public:
	terUnitGridTeamOffensiveOperator(const terUnitReal* owner) : owner_(owner),
		offensivePoint_(NULL)
	{
		position_ = owner_->position2D();
		clusterID_ = owner_->includingCluster();

		ignoreField_ = owner_->attr().checkWeaponFlag(WEAPON_IGNORE_FIELD);
		excludeHolograms_ = owner_->attr().weaponSetup.excludeHolograms;

		fireDistanceMin_ = sqr(owner->attr().fireRadiusMin());
		fireDistanceMax_ = sqr(owner->attr().fireRadius());
		sightDistance_ = sqr(owner->attr().sightRadius());

		bestFactor_ = 0;

		fireTest_ = false;
	}

	void operator()(terUnitBase* p)
	{
		if(excludeHolograms_ && p->isBuilding() && !p->isConstructed())
			return;

		if(owner_->isEnemy(p) && p->alive() && p->damageMolecula().isAlive() && owner_->checkFireClass(p) && (ignoreField_ || p->includingCluster() == clusterID_)
		  && p->GetLegionMorphing() && !p->isUnseen()){
			float dist = p->position2D().distance2(position_);
			float f = p->attr().kill_priority + 1.f/(1.f + dist);

			if(p->possibleDamage() >= p->damageMolecula().aliveElementCount())
				f /= 1000 + dist;
			else
				f -= float(p->possibleDamage()) / float(p->damageMolecula().aliveElementCount());

			if(dist > fireDistanceMin_ && dist < sightDistance_){
				bool fire_test = (dist < fireDistanceMax_) ? owner_->fireCheck(p->position(),p) : false;
				if(fire_test){
					if(!fireTest_ || bestFactor_ < f){
						fireTest_ = true;
						offensivePoint_ = p;
						bestFactor_ = f;
					}
				}
				else {
					if(!fireTest_ && bestFactor_ < f ){
						offensivePoint_ = p;
						bestFactor_ = f;
					}
				}
			}
		}
	}

	terUnitBase* offensivePoint(){ return offensivePoint_; }

private:

	const terUnitReal* owner_;
	bool excludeHolograms_;

	Vect2f position_;
	int clusterID_;
	bool ignoreField_;

	float fireDistanceMin_;
	float fireDistanceMax_;
	float sightDistance_;

	bool fireTest_;
	terUnitBase* offensivePoint_;
	float bestFactor_;
};

//---------------------------------------

class terUnitGridSplashDamageOperator
{
public:
	terUnitGridSplashDamageOperator(terUnitBase* source_unit, terUnitBase* owner_unit) 
	: sourceUnit_(source_unit), ownerUnit_(owner_unit)
	{
		radius_ = sourceUnit_->attr().unitDamage.splashDamageRadius;
		damage_ = sourceUnit_->attr().unitDamage.splashDamage;
	}

	void operator()(terUnitBase* p)
	{
		if(!radius_)
			return;

		if(p->GetRigidBodyPoint() && p->GetRigidBodyPoint()->diggingModeLagged())
			return;

		if(p->isHarmful(sourceUnit_) && !p->isDocked() && sourceUnit_->position().distance2(p->position()) < sqr(radius_ + p->radius()))
			p->setDamage(damage_,ownerUnit_);
	}

private:
	float radius_;
	DamageData damage_;

	terUnitBase* sourceUnit_;
	terUnitBase* ownerUnit_;
};

class terUnitGridDisintegratorOperator
{
public:
	terUnitGridDisintegratorOperator(terUnitBase* source_unit,float radius_min,float radius_max) : sourceUnit_(source_unit)
	{
		radius_min_ = radius_min * radius_min;
		radius_max_ = radius_max * radius_max;

		position_ = sourceUnit_->position2D();

		clusterID_ = sourceUnit_->includingCluster();
	}

	void operator()(terUnitBase* p)
	{
		if(p->attr().isLegionary() && p->unitClass() != UNIT_CLASS_BASE && sourceUnit_->isEnemy(p) && p->GetSquadPoint() && p->alive() && (clusterID_ == p->includingCluster())){
			terUnitLegionary* lp = safe_cast<terUnitLegionary*>(p);
			if(!lp->isDisintegrating()){
				float dist = p->position2D().distance2(position_);
				if(dist >= radius_min_ && dist < radius_max_){
					terUnitLegionary* lp = safe_cast<terUnitLegionary*>(p);
					TargetContainer::const_iterator it = find(targets_.begin(),targets_.end(),lp);
					if(it == targets_.end()){
						float factor = p->attr().kill_priority + 1.f/(1.f + dist);
						targets_.push_back(Target(lp,factor));
					}
				}
			}
		}
	}

	class Target
	{
	public:
		Target(terUnitLegionary* unit,float factor) : unit_(unit), factor_(factor) { }

		bool operator == (const terUnitLegionary* unit) const { return (unit_ == unit); }

		terUnitLegionary* unit() const { return unit_; }
		float factor() const { return factor_; }

	private:

		mutable terUnitLegionary* unit_;
		float factor_;
	};

	struct TargetOrderingOp
	{
		bool operator() (const Target& t0,const Target& t1){
			return t0.factor() > t1.factor();
		}
	};

	typedef list<Target> TargetContainer;
	const TargetContainer& targets() const { return targets_; }
	void sortTargets(){ targets_.sort(TargetOrderingOp()); }

private:

	float radius_min_;
	float radius_max_;

	Vect2f position_;
	int clusterID_;

	terUnitBase* sourceUnit_;

	/// Юниты, попавшие в область действия дизинтегратора.
	TargetContainer targets_;
};

class terUnitGridInvisibilityGeneratorOperator
{
public:
	terUnitGridInvisibilityGeneratorOperator(terUnitBase* source_unit,float radius_min,float radius_max) : sourceUnit_(source_unit)
	{
		radius_min_ = radius_min * radius_min;
		radius_max_ = radius_max * radius_max;

		position_ = sourceUnit_->position2D();

		clusterID_ = sourceUnit_->includingCluster();
	}

	void operator()(terUnitBase* p)
	{
		if(p->attr().isLegionary() && p->attr().ID != sourceUnit_->attr().ID && !sourceUnit_->isEnemy(p) && p->alive()){
			terUnitLegionary* lp = safe_cast<terUnitLegionary*>(p);
			lp->setInvisibility(terInvisibilityTime);
		}
	}

private:

	float radius_min_;
	float radius_max_;

	Vect2f position_;
	int clusterID_;

	terUnitBase* sourceUnit_;
};

class terUnitGridAreaDamageOperator
{
public:
	terUnitGridAreaDamageOperator(terUnitBase* owner,const Vect2f& position,float radius_min,float radius_max) : owner_(owner)
	{
		radius_min_ = sqr(radius_min);
		radius_max_ = sqr(radius_max);

		position_ = position;

		clusterID_ = owner_->includingCluster();
	}

	void operator()(terUnitBase* p)
	{
		if(owner_->isEnemy(p) && p->alive() && !p->isDocked() && owner_->checkFireClass(p) && p->includingCluster() == clusterID_ 
		  && p->GetLegionMorphing() && !p->isUnseen()){
			float dist = p->position2D().distance2(position_);

			if(dist >= radius_min_ && dist < radius_max_)
				p->setDamage(owner_->damageData(),owner_);
		}
	}

private:

	float radius_min_;
	float radius_max_;

	Vect2f position_;
	int clusterID_;

	terUnitBase* owner_;
};

class terUnitGridConductorOperator
{
public:
	terUnitGridConductorOperator(terUnitBase* owner,const Vect2f& position,float radius_min,float radius_max) : owner_(owner)
	{
		radius_min_ = sqr(radius_min);
		radius_max_ = sqr(radius_max);

		position_ = position;

		targets_.reserve(16);

		clusterID_ = owner_->includingCluster();
	}

	void operator()(terUnitBase* p)
	{
		if(canHarm(p) && p->alive() && !p->isDocked() && owner_->checkFireClass(p) && p->includingCluster() == clusterID_ 
		  && p->GetLegionMorphing() && !p->isUnseen()){
			float dist = p->position2D().distance2(position_);

			if(dist >= radius_min_ && dist < radius_max_){
				p->setDamage(owner_->damageData(),owner_);
				targets_.push_back(p->position());
			}
		}
	}

	typedef vector<Vect3f> TargetList;
	const TargetList& targets() const { return targets_; }

private:

	float radius_min_;
	float radius_max_;

	Vect2f position_;
	int clusterID_;

	terUnitBase* owner_;

	TargetList targets_;

	bool canHarm(const terUnitBase* p) const { return (owner_->isEnemy(p) || p->Player->isWorld()); }
};

#endif //__GRIDTOOLS_H__