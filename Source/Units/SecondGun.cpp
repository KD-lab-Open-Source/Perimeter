#include "StdAfx.h"

#include "Universe.h"
#include "Config.h"
#include "SecondGun.h"
#include "IronBullet.h"
#include "GridTools.h"
#include "runtime.h"

#include "GenericFilth.h"
#include "filth.hi"

int CheckShotLine(const Vect3f& from,const Vect3f& to);
int CheckFieldLine(const Vect3f& from,const Vect3f& v,float d,int cluster_id);

//-------------------------------------------------------

class AngleLimitPsiOp {
public:
	float operator()(float angle,float angle_min,float angle_max) const
	{
		angle = cycleAngle(angle);

		if(angle < angle_min)
			angle = angle_min;

		if(angle > angle_max)
			angle = angle_max;

		return angle;
	}
};

class AngleLimitThetaOp {
public:
	float operator()(float angle,float angle_min,float angle_max) const
	{
		angle = cycleAngle(angle);
		angle = getDeltaAngle(angle,0);

		if(angle < angle_min)
			angle = angle_min;

		if(angle > angle_max)
			angle = angle_max;

		return angle;
	}
};

class AngleValuePsiOp {
public:
	float operator()(float value) const { return value; }
};

class AngleValueThetaOp {
public:
	float operator()(float value) const { return -value; }
};

template<class AngleLimitOp,class AngleValueConvertOp>
class AimAngleController
{
public:
	AimAngleController() : object_(NULL), logicTile_(NULL)
	{
		value_ = valueDefault_ = 0.0f;
		speed_ = M_PI * 0.2f;
		valuePrecision_ = M_PI * 0.001f;
		valueInterpolator_ = 0;

		valueMin_ = 0.0f;
		valueMax_ = M_PI * 2.0f;
	}

	bool operator == (float val) const { return (fabs(getDeltaAngle(val,value_)) <= valuePrecision_); }
	bool operator != (float val) const { return (fabs(getDeltaAngle(val,value_)) > valuePrecision_); }

	bool checkAngle(float angle) const
	{
		angle = cycleAngle(angle);
		if(angle >= valueMin_ && angle <= valueMax_)
			return true;
		else
			return false;
	}

	void setSpeed(float speed){ speed_ = speed; }
	void setLimits(float min,float max){ valueMin_ = min; valueMax_ = max; }
	void setPrecision(float precision){ valuePrecision_ = precision; }

	void setValue(float angle)
	{ 
		value_ = AngleLimitOp()(angle,valueMin_,valueMax_); 
		valueInterpolator_ = AngleValueConvertOp()(value_);
	}

	void setDefaultValue(float angle){ valueDefault_ = angle; }

	void change(float target_angle)
	{
		float delta = getDeltaAngle(target_angle,value_);

		if(delta < 0){
			if(delta < -speed_)
				delta = -speed_;
		}
		else {
			if(delta > speed_)
				delta = speed_;
		}

		value_ = AngleLimitOp()(value_ + delta,valueMin_,valueMax_);
	}

	void changeToDefault(){ change(valueDefault_); }

	void setObject(cObjectNode* object)
	{
		object_ = object;
	}

	void setLogicObject(cLogicTile* logic_tile)
	{
		logicTile_ = logic_tile;
	}

	void setChain(const char* name)
	{
		if(!object_ || !logicTile_) return;

		object_->SetChannel(name,0);
	}

	void updateLogic(eAxis axis)
	{
		if(!logicTile_) return;

		float val = AngleValueConvertOp()(value_);
		logicTile_->SetRotate(&Mat3f(val,axis));
	}

	bool logicObjectPosition(Vect3f& pos) const
	{
		if(logicTile_){
			pos = logicTile_->GetGlobalMatrix().trans();
			return true;
		}

		return false;
	}

	void avatarQuant()
	{
		valueInterpolator_ = AngleValueConvertOp()(value_);
	}

	void avatarInterpolation(eAxis axis)
	{
		if(!object_) return;
		valueInterpolator_(object_,axis);
	}

	float operator()() const { return value_; }

private:

	float value_;
	float valueMin_;
	float valueMax_;

	float valuePrecision_;

	float valueDefault_;

	float speed_;

	cObjectNode* object_;
	cLogicTile* logicTile_;
	InterpolatorAngle valueInterpolator_;
};

/// Базовый класс для управления наведением оружия.
class AimControllerBase
{
public:
	AimControllerBase(terUnitBase* owner) : owner_(owner){ }
	virtual ~AimControllerBase(){ }

	terUnitBase* owner() const { xassert(owner_); return owner_; }

	virtual bool init(cLogicObject* logic_root,cObjectNodeRoot* model_root,const terWeaponControllerSetup& setup) = 0;

	virtual void updateLogic() = 0;

	virtual void avatarQuant() = 0;
	virtual void avatarInterpolation() = 0;

	virtual void aim(float psi,float theta) = 0;
	virtual bool isAimed(float psi,float theta) const = 0;
	virtual bool isAimed(float psi,float theta,int& fire_status) const = 0;

	virtual void aimDefault() = 0;

	virtual bool getTargetingPosition(Vect3f& targeting0,Vect3f& targeting1) const = 0;
	virtual void getGunPosition(Vect3f& pos) const  = 0;

private:

	mutable terUnitBase* owner_;
};

/// Управление наведением для оружия, не требующего наведения на цель.
class AimControllerOmnidirectional : public AimControllerBase
{
public:
	AimControllerOmnidirectional(terUnitBase* owner) : AimControllerBase(owner) { }
	~AimControllerOmnidirectional(){ }

	bool init(cLogicObject* logic_root,cObjectNodeRoot* model_root,const terWeaponControllerSetup& setup){ return true; }

	void updateLogic(){ }

	void avatarQuant(){ }
	void avatarInterpolation(){ }

	void aim(float psi,float theta){ }
	bool isAimed(float psi,float theta) const { return true; }
	bool isAimed(float psi,float theta,int& fire_status) const { return true; }

	void aimDefault(){ }

	bool getTargetingPosition(Vect3f& targeting0,Vect3f& targeting1) const { targeting0 = owner()->position(); targeting1 = owner()->position() + Vect3f::K; return true; }
	void getGunPosition(Vect3f& pos) const { pos = owner()->position(); }
private:
};

/// Управление наведением для оружия, требующего наведения на цель.
class AimControllerDirectional : public AimControllerBase
{
public:
	AimControllerDirectional(terUnitBase* owner) : AimControllerBase(owner), targetingObject_(NULL) { }
	~AimControllerDirectional(){ }

	bool init(cLogicObject* logic_root,cObjectNodeRoot* model_root,const terWeaponControllerSetup& setup);

	void updateLogic(){ psi_.updateLogic(Z_AXIS); theta_.updateLogic(X_AXIS); }

	void avatarQuant(){ psi_.avatarQuant(); theta_.avatarQuant(); }
	void avatarInterpolation(){ psi_.avatarInterpolation(Z_AXIS); theta_.avatarInterpolation(X_AXIS); }

	void aim(float psi,float theta){ psi_.change(psi); theta_.change(theta); }
	bool isAimed(float psi,float theta) const { return (psi_ == psi && theta_ == theta); }
	bool isAimed(float psi,float theta,int& fire_status) const;

	void aimDefault(){ psi_.changeToDefault(); theta_.changeToDefault(); }

	bool getTargetingPosition(Vect3f& targeting0,Vect3f& targeting1) const;
	void getGunPosition(Vect3f& pos) const 
	{ 
		owner()->avatar()->updateLogicObject();
		if(theta_.logicObjectPosition(pos)) return;

		pos = owner()->position();
		return;
	}

private:

	AimAngleController<AngleLimitPsiOp,AngleValuePsiOp> psi_;
	AimAngleController<AngleLimitThetaOp,AngleValueThetaOp> theta_;

	mutable cLogicTile* targetingObject_;
};

template<class AimType>
class WeaponDerivedBase : public terWeapon
{
public:
	WeaponDerivedBase(terUnitReal* owner) : terWeapon(owner), aimController_(owner), turnSuggestPrm_(NULL){ turnSuggestPrm_ = owner->attr().weaponSetup.turnSuggestPrm; }
	~WeaponDerivedBase(){ }

	bool init(){
		xassert(owner()->attr().weaponSetup.enabled());

		if(!terWeapon::init()) return false;
		return aimController_.init(owner()->LogicObjectPoint,owner()->avatar()->GetModelPoint(),owner()->attr().weaponSetup.controllerSetup);
	}

	void updateLogic(){ if(isEnabled()) aimController_.updateLogic(); }

	void avatarQuant(){ if(isEnabled()) aimController_.avatarQuant(); }
	void avatarInterpolation(){ if(isEnabled()) aimController_.avatarInterpolation(); }

protected:

	const AimControllerBase* aimController() const { return &aimController_; }

	bool aim(const Vect3f& to,int& status){
		float psi,theta;

		if(turnSuggestPrm_)
			RigidBody::suggestMissileTurnAngels(*turnSuggestPrm_,*owner()->BodyPoint,owner()->position(),to,psi,theta);
		else {
			Vect3f pos;
			aimController_.getGunPosition(pos);
			RigidBody::suggestMissileTurnAngels(*owner()->BodyPoint,pos,to,psi,theta); 
		}

		if(setup().thetaForced > -90)
			theta = setup().thetaForced;

		bool ret = aimController_.isAimed(psi,theta,status);

		aimController_.aim(psi,theta);

		return ret;
	}

	void aimDefault(){ aimController_.aimDefault(); }

	const RigidBodyPrm* turnSuggestPrm() const { return turnSuggestPrm_; }

private:
	/// управление наведением оружия
	AimType aimController_;

	/// используется для расчёта углов при наведении
	const RigidBodyPrm* turnSuggestPrm_;
};

typedef WeaponDerivedBase<AimControllerDirectional> WeaponDirectionalBase;
typedef WeaponDerivedBase<AimControllerOmnidirectional> WeaponOmnidirectionalBase;

/// Оружие, стреляющее снарядами по навесной траектории.
class terWeaponArtillery : public WeaponDirectionalBase
{
public:
	terWeaponArtillery(terUnitReal* owner) : WeaponDirectionalBase(owner) { }
	bool fire(const Vect3f& to,terUnitBase* target);
	int estimatedDamage() const;
};

/// Оружие, стреляющее ракетами.
class terWeaponLauncher : public WeaponDirectionalBase
{
public:
	terWeaponLauncher(terUnitReal* owner);
	bool fire(const Vect3f& to,terUnitBase* target);
	int estimatedDamage() const;

	void moveQuant();

	bool isLoaded() const { return (WeaponDirectionalBase::isLoaded() && owner()->isDockReady(missileIndex_)); }
	void enable();

	void unload();

private:

	/// индекс для управления пуском ракет
	int missileIndex_;
};

/// Лазерное оружие.
class terWeaponLaser : public WeaponDirectionalBase
{
public:
	terWeaponLaser(terUnitReal* owner);

	void avatarQuant();

	bool fire(const Vect3f& to,terUnitBase* target);
	bool fireTest(const Vect3f& to,terUnitBase* target,int& status) const;

	int estimatedDamage() const;

	void disable();
};

/// Замораживающее оружие - стреляет как лазер, лишает на время способности атаковать.
class terWeaponFreezeLaser : public terWeaponLaser
{
public:
	terWeaponFreezeLaser(terUnitReal* owner) : terWeaponLaser(owner) { }

	bool fire(const Vect3f& to,terUnitBase* target);
};

class terWeaponHealLaser : public terWeaponLaser
{
public:
	terWeaponHealLaser(terUnitReal* owner) : terWeaponLaser(owner) { }

	bool fire(const Vect3f& to,terUnitBase* target);
};

/// Электроразрядник - бьёт врага молнией.
class terWeaponLighting : public WeaponDirectionalBase
{
public:
	terWeaponLighting(terUnitReal* owner);
	~terWeaponLighting();

	void quant();

	bool fire(const Vect3f& to,terUnitBase* target);
	bool fireTest(const Vect3f& to,terUnitBase* target,int& status) const;

	int estimatedDamage() const;
	
	void destroyLink();

	void kill();
	void disable();

private:
	/// визуализация молнии
	cLighting* lighting_;

	/// цель
	terUnitBase* target_;

	bool releaseLighting()
	{
		if(lighting_){
			lighting_->Release();
			lighting_ = 0;

			return true;
		}

		return false;
	}
};

/// Спецоружие - piercer.
/**
Юнит-владелец после задания цели превращается в неуправляемую подземную торпеду.
*/
class terWeaponPiercer : public WeaponOmnidirectionalBase
{
public:
	terWeaponPiercer(terUnitReal* owner) : WeaponOmnidirectionalBase(owner), targetPos_(0,0,0) { fire_ = false; }

	void quant();

	bool fire(const Vect3f& to,terUnitBase* target);
	int estimatedDamage() const { return 0; }
	
private:

	bool fire_;
	Vect3f targetPos_;
};

/// Спецоружие - disintegrator.
/**
Разваливает все вражеские юниты в зоне действия на базовые.
*/
class terWeaponDisintegrator : public WeaponOmnidirectionalBase
{
public:
	terWeaponDisintegrator(terUnitReal* owner) : WeaponOmnidirectionalBase(owner){ }

	void quant();

	bool fire(const Vect3f& to,terUnitBase* target);
	int estimatedDamage() const { return 0; }
};

/// Спецоружие - Scum Heater
class terWeaponScumHeater : public WeaponOmnidirectionalBase
{
public:
	terWeaponScumHeater(terUnitReal* owner);

	void quant();

	bool fire(const Vect3f& to,terUnitBase* target);
	int estimatedDamage() const { return 0; }
};

/// Электрооружие - Conductor, бьёт молниями сразу много целей
class terWeaponConductor : public WeaponOmnidirectionalBase
{
public:
	terWeaponConductor(terUnitReal* owner);

	void quant();

	bool fire(const Vect3f& to,terUnitBase* target);
	int estimatedDamage() const { return 0; }

	void kill();
	void disable();

private:

	cLighting* lighting_;

	bool releaseLighting()
	{
		if(lighting_){
			lighting_->Release();
			lighting_ = 0;

			return true;
		}

		return false;
	}
};

/// Спецоружие - Scum Twister
class terWeaponScumTwister : public WeaponOmnidirectionalBase
{
public:
	terWeaponScumTwister(terUnitReal* owner);
	~terWeaponScumTwister();

	void quant();

	bool fire(const Vect3f& to,terUnitBase* target);
	int estimatedDamage() const { return 0; }

	void unload();
	void kill();
	
	void destroyLink();

	SaveWeaponData* universalSave(SaveWeaponData* data);
	void universalLoad(const SaveWeaponData* data);

private:

	Vect3f targetPos_;
	terUnitBase* missile_;
};

class terWeaponScumSplitter : public WeaponOmnidirectionalBase
{
public:
	terWeaponScumSplitter(terUnitReal* owner) : WeaponOmnidirectionalBase(owner), missile_(NULL), targetPos_(0,0,0) { }

	void quant();

	bool fire(const Vect3f& to,terUnitBase* target);
	int estimatedDamage() const { return 0; }
	
	void destroyLink();

private:

	Vect3f targetPos_;
	terUnitBase* missile_;
};

/// Спецоружие - Leech, присасывается к вражеской энергосистеме и парализует ядра.
class terWeaponLeech : public WeaponOmnidirectionalBase
{
public:
	terWeaponLeech(terUnitReal* owner);
	~terWeaponLeech();

	void quant();

	bool fire(const Vect3f& to,terUnitBase* target);
	bool fireTest(const Vect3f& to,terUnitBase* target,int& status) const;

	int estimatedDamage() const { return 0; }
	
	void destroyLink();

private:
	/// воруемая у врага энергия
	EnergyConsumer weaponConsumption_;

	/// визуализация энергосвязи
	cLine3d* link_;

	/// цель
	terUnitBase* target_;

	bool allowDischarge() const { xassert(owner()); return (owner()->realAvatar()->chainID() == CHAIN_FIRE); }
};

/// Спецоружие - Scum Disruptor, коробит поверхность в случайных местах в заданном направлении.
class terWeaponScumDisruptor : public WeaponOmnidirectionalBase
{
public:
	terWeaponScumDisruptor(terUnitReal* owner);

	bool fire(const Vect3f& to,terUnitBase* target);
	int estimatedDamage() const { return 0; }

	void disable();
	void quant();
	
	void destroyLink();

	SaveWeaponData* universalSave(SaveWeaponData* data);
	void universalLoad(const SaveWeaponData* data);

private:

	terFilthSpot* missile_;

	bool allowDischarge() const { xassert(owner()); return (owner()->realAvatar()->chainID() == CHAIN_FIRE); }
};

/// Спецоружие - Filth Navigator, вызывает скверну в заданной точке.
class terWeaponFilthNavigator : public WeaponOmnidirectionalBase
{
public:
	terWeaponFilthNavigator(terUnitReal* owner);

	bool fire(const Vect3f& to,terUnitBase* target);
	int estimatedDamage() const { return 0; }

	void quant();

	SaveWeaponData* universalSave(SaveWeaponData* data);
	void universalLoad(const SaveWeaponData* data);

private:

	/// позиция, в которой взывалась скверна в прошлый раз
	Vect3f targetPos_;
	/// вызванная в прошлый раз скверна - индекс terFilthNavigatorPrm[]
	int prmIndex_;

	bool allowDischarge() const { xassert(owner()); return (owner()->realAvatar()->chainID() == CHAIN_FIRE); }
};

/// Спецоружие - создает спот скверны.
class terWeaponFilthSpot : public WeaponOmnidirectionalBase
{
public:
	terWeaponFilthSpot(terUnitReal* owner);

	bool fire(const Vect3f& to,terUnitBase* target);
	int estimatedDamage() const { return 0; }
};

/// Спецоружие - делает невидимыми юнитов вокруг себя.
class terWeaponInvisibilityGenerator : public WeaponOmnidirectionalBase
{
public:
	terWeaponInvisibilityGenerator(terUnitReal* owner) : WeaponOmnidirectionalBase(owner){ }

	void quant();

	bool fire(const Vect3f& to,terUnitBase* target);
	int estimatedDamage() const { return 0; }
};

//-------------------------------------------------------

bool AimControllerDirectional::init(cLogicObject* logic_root,cObjectNodeRoot* model_root,const terWeaponControllerSetup& setup)
{
	bool ret = true;
	if(setup.objectName[0]){
		if(cObjectNode* obj = model_root->FindSubObject(setup.objectName[0]))
			psi_.setObject(obj);
		else
			ret = false;
		xassert(ret);
	}

	if(setup.logicObjectName[0]){
		if(cLogicTile* logic_obj = logic_root->FindObject(setup.logicObjectName[0]))
			psi_.setLogicObject(logic_obj);
		else
			ret = false;
		xassert(ret);
	}

	if(setup.objectName[1]){
		if(cObjectNode* obj = model_root->FindSubObject(setup.objectName[1]))
			theta_.setObject(obj);
		else
			ret = false;
		xassert(ret);
	}

	if(setup.logicObjectName[1]){
		if(cLogicTile* logic_obj = logic_root->FindObject(setup.logicObjectName[1]))
			theta_.setLogicObject(logic_obj);
		else
			ret = false;
		xassert(ret);
	}

	if(setup.targetingObjectName){
		targetingObject_ = logic_root->FindObject(setup.targetingObjectName);

		xassert(targetingObject_);

		if(!targetingObject_)
			ret = false;

		xassert(ret);
	}

	psi_.setSpeed(setup.turnSpeed[0]*M_PI);
	theta_.setSpeed(setup.turnSpeed[1]*M_PI);
	
	psi_.setLimits(setup.psiLimit[0]*M_PI,setup.psiLimit[1]*M_PI);
	theta_.setLimits(setup.thetaLimit[0]*M_PI,setup.thetaLimit[1]*M_PI);

	psi_.setPrecision(setup.targetingPrecision[0]*M_PI);
	theta_.setPrecision(setup.targetingPrecision[1]*M_PI);

	psi_.setDefaultValue(setup.defaultAngles[0]*M_PI);
	theta_.setDefaultValue(setup.defaultAngles[1]*M_PI);

	psi_.setValue(setup.defaultAngles[0]*M_PI);
	theta_.setValue(setup.defaultAngles[1]*M_PI);

	psi_.setChain("main");
	theta_.setChain("main");
	xassert(ret);
	return ret;
}

bool AimControllerDirectional::isAimed(float psi,float theta,int& status) const
{
	bool ret = true;

	if(psi_ != psi){
		status |= LEGION_FIRE_STATUS_HORIZONTAL_ANGLE;
		ret = false;
	}

	if(theta_ != theta){
		status |= LEGION_FIRE_STATUS_VERTICAL_ANGLE;
		ret = false;

		if(!theta_.checkAngle(theta))
			status |= LEGION_FIRE_STATUS_BORDER_ANGLE;
	}

	return ret;
}

bool AimControllerDirectional::getTargetingPosition(Vect3f& targeting0,Vect3f& targeting1) const
{
	xassert(targetingObject_);

	int visibility;
	MatXf trg0;

	targetingObject_->GetAnimation(&trg0,&visibility);

	targeting0 = trg0.trans();
	targeting1 = trg0*Vect3f(0,1,0);

	return true;
}

//-------------------------------------------------------

terWeapon::terWeapon(terUnitReal* owner) : owner_(owner),
	fireController_(NULL),
	fireTarget_(NULL),
	setup_(owner_->attr().weaponSetup)
{
	xassert(owner_ && owner_->attr().weaponSetup.enabled());

	missileCount_ = 0;

	isEnabled_ = false;
	isOn_ = false;

	reloaded_ = false;

	fireFlag_ = false;
	fireAnimationMode_ = false;
	fireWaitingMode_ = false;

//	fireDistance_ = 0.0f;

	dischargeSpeed_ = setup_.dischargeSpeed;

	reloadStart();
}

terWeapon::~terWeapon()
{
}

terWeapon* terWeapon::create(WeaponType type,terUnitReal* owner)
{
	switch(type){
	case WEAPON_LASER:
		return (new terWeaponLaser(owner));
	case WEAPON_FREEZE_LASER:
		return (new terWeaponFreezeLaser(owner));
	case WEAPON_HEAL_LASER:
		return (new terWeaponHealLaser(owner));
	case WEAPON_LAUNCHER:
		return (new terWeaponLauncher(owner));
	case WEAPON_ARTILLERY:
		return (new terWeaponArtillery(owner));
	case WEAPON_PIERCER:
		return (new terWeaponPiercer(owner));
	case WEAPON_DISINTEGRATOR:
		return (new terWeaponDisintegrator(owner));
	case WEAPON_SCUM_SPLITTER:
		return (new terWeaponScumSplitter(owner));
	case WEAPON_SCUM_TWISTER:
		return (new terWeaponScumTwister(owner));
	case WEAPON_SCUM_HEATER:
		return (new terWeaponScumHeater(owner));
	case WEAPON_LEECH:
		return (new terWeaponLeech(owner));
	case WEAPON_SCUM_DISRUPTOR:
		return (new terWeaponScumDisruptor(owner));
	case WEAPON_FILTH_NAVIGATOR:
		return (new terWeaponFilthNavigator(owner));
	case WEAPON_FILTH_SPOT:
		return (new terWeaponFilthSpot(owner));
	case WEAPON_INVISIBILITY_GENERATOR:
		return (new terWeaponInvisibilityGenerator(owner));
	case WEAPON_LIGHTING:
		return (new terWeaponLighting(owner));
	case WEAPON_CONDUCTOR:
		return (new terWeaponConductor(owner));
	default:
		xassert(0 && "Undefined weapon type");
		break;
	}

	return NULL;
}

SaveWeaponData* terWeapon::universalSave(SaveWeaponData* data)
{
	if(data){
		data->target = fireTarget_;
		data->targetPosition = firePosition_;
		data->isSwitchedOn = isOn_;
		data->fireDelayTimer = fireDelayTimer_();
	}

	return data;
}

void terWeapon::universalLoad(const SaveWeaponData* data)
{
	if(!data) return;

	data->target.setLink((terUnitBase*&)fireTarget_);
	firePosition_ = data->targetPosition;

	if(data->isSwitchedOn){
		switchOn();
		setFireAnimationMode(true);
		reloaded_ = true;
		fireDelayTimer_.start(data->fireDelayTimer);
	}
}

void terWeapon::switchOff()
{ 
	isOn_ = false;
	
	if(isEnabled()){
		if(setup_.hasFireController())
			fireController_->requestChain(CHAIN_SWITCHED_OFF);
	}

	setFireAnimationMode(false); 
}

void terWeapon::removeMissileReference()
{ 
	if(missileCount_){
		missileCount_--;

		if(setup_.cleanTargetAfterFire())
			clearOwnerTarget();
	}
}

bool terWeapon::init()
{
	xassert(owner_);

	if(setup_.hasFireController()){
		fireController_ = owner_->avatar()->animationGroup(ANIMATION_GROUP_FIRE);
		xassert(fireController_);
	}

	return true;
}

void terWeapon::reloadStart()
{ 
	if(!reloadConsumption_.attached())
		reloadConsumption_.attach(owner()->Player, setup_.reloadConsumption);
	else
		reloadConsumption_.start(setup_.reloadConsumption);
	reloaded_ = false;
}

ChainID terWeapon::chainRequest() const 
{ 
	if(setup_.rootFireAnimation()){ 
		if(fireAnimationMode_ || fireWaitingMode_)
			return CHAIN_FIRE;

		if(setup_.checkFlag(WEAPON_CHARGE_ANIMATION) && !reloaded())
			return CHAIN_PRODUCTION;
	}

	return CHAIN_NONE;
}


bool terWeapon::isFireAnimationPlaying() const
{
	if(setup_.hasFireController()) return (fireController_->chainID() == CHAIN_FIRE);
	if(setup_.rootFireAnimation()) return (owner()->realAvatar()->chainID() == CHAIN_FIRE);
	return false;
}

void terWeapon::quant()
{
	if(isEnabled_){
		if(!isOn_){
			if(!reloaded_ && allowCharge()){
				reloadConsumption_.requestCharge();
				if(reloadConsumption_.charged()){
					reloaded_ = true;

					xassert(owner());
					if(setup_.needCharge())
						owner()->soundEvent(SOUND_VOICE_WEAPON_CHARGED);
				}
			}
		}
		else {
			if(setup_.needCharge() && allowDischarge()){
				if(reloadConsumption_.discharge(dischargeSpeed_)){
					switchOff();
					reloadStart();

					if(setup_.cleanTargetAfterFire())
						clearOwnerTarget();

					fireAnimationMode_ = false;
				}
				else 
					owner_->realAvatar()->playSound(SOUND_SHOT);
			}
		}

		if(fireAnimationMode_){
			if(setup_.hasFireController()){
				if(!setup_.cycleFireAnimation() && fireController_->chainID() == CHAIN_FIRE && fireController_->requestDone()){
					fireController_->requestChain(CHAIN_SWITCHED_OFF);
					fireAnimationMode_ = false;
				}
			}
			else {
				if(setup_.rootFireAnimation()){
					xassert(owner());
					if(!setup_.cycleFireAnimation() && owner()->realAvatar()->chainID() == CHAIN_FIRE && owner()->realAvatar()->requestDone())
						fireAnimationMode_ = false;
				}
				else
					fireAnimationMode_ = false;
			}
		}

		if(fireWaitingMode_){
			bool call_fire = true;
			if(setup_.hasFireController()){
				call_fire = (fireController_->chainID() == CHAIN_FIRE);
			}
			else {
				if(setup_.rootFireAnimation()){
					call_fire = (owner_->realAvatar()->chainID() == CHAIN_FIRE);
				}
			}
			if(call_fire){
				updateOwner();
				if(fire(firePosition_,fireTarget_)){
					owner_->realAvatar()->playSound(SOUND_SHOT);
					fireDisableTimer_.start(setup().fireDisableTime);
				}
				fireFlag_ = true;
				fireWaitingMode_ = false;
			}
		}

		if(fireDisableTimer_()){
			if(fireTarget_)
				firePosition_ = fireTarget_->position();
			int status = 0;

			aim(firePosition_,status);
		}

		if(!aimTimer_())
			aimDefault();
	}
}

void terWeapon::moveQuant()
{
	updateLogic();
}

bool terWeapon::fireRequest(const Vect3f* to,terUnitBase* target,int& status)
{
	xassert(owner_);

	if(fireDisableTimer_()){
		status |= LEGION_FIRE_STATUS_RELOAD_AMMO;
		return false;
	}

	Vect3f pos = (target) ? pos = target->position() : pos = *to;

	if(fireDistanceCheck(pos)){
		if(!isLoaded())
			status |= LEGION_FIRE_STATUS_RELOAD_AMMO;
		else {
			bool aim_flag = aim(pos,status);
			aimTimer_.start(setup_.aimResetDelay);

			if(aim_flag){
				updateOwner();
				
				if(fireTest(pos,target,status)){
					fireEvent(pos,target);
					return true;
				}
			}
		}
	}
	else
		status |= LEGION_FIRE_STATUS_DISTANCE;

	fireFlag_ = false;
	return false;
}

bool terWeapon::fireDistanceCheck(const Vect3f& to) const
{
	xassert(owner());

	if(!setup().squadMode){
		Vect3f dr = to - owner_->position();
		dr.z = 0;
		float dist2 = dr.norm2();

		if(dist2 >= sqr(setup_.fireRadius[0]) && dist2 < sqr(setup_.fireRadius[1]))
			return true;
	}
	else {
		const terUnitLegionary* lp = safe_cast<const terUnitLegionary*>(owner());
		if(const terUnitSquad* sp = lp->getSquad())
			return sp->checkAttackPosition(to);
	}

	return false;
}

bool terWeapon::fireTest(const Vect3f& to,terUnitBase* target,int& status) const
{
	xassert(owner_);

	if(!setup_.checkFlag(WEAPON_IGNORE_FIELD)){
		if((target && target->includingCluster() != owner_->includingCluster()) || (!target && field_dispatcher->getIncludingCluster(to) != owner_->includingCluster())){
			status |= LEGION_FIRE_STATUS_FIELD_OBSTACLE;
			return false;
		}
	}

	if(target){
		if(!owner_->isEnemy(target))
			status |= LEGION_FIRE_STATUS_NOT_ENEMY_TARGET;

		if(!owner_->checkFireClass(target)){
			status |= LEGION_FIRE_STATUS_BAD_TARGET;
			return false;
		}
	}

	return true;
}

void terWeapon::updateOwner()
{
	owner()->UpdateLogicPoint();
	owner()->LogicObjectPoint->SetChannel("main",0);
	updateLogic();
	owner()->LogicObjectPoint->Update();
}

void terWeapon::fireEvent(const Vect3f& to,terUnitBase* target)
{
	xassert(owner_);

	firePosition_ = to;
	fireTarget_ = target;

	bool call_fire = true;
	if(setup_.hasFireController()){
		fireController_->requestChain(CHAIN_FIRE);
		call_fire = (fireController_->chainID() == CHAIN_FIRE);
	}
	else {
		if(setup_.rootFireAnimation())
			call_fire = false;
	}

	fireAnimationMode_ = true;
	fireWaitingMode_ = true;

	updateOwner();

	if(call_fire){
		fire(to,target);
		owner_->realAvatar()->playSound(SOUND_SHOT);
		fireDisableTimer_.start(setup().fireDisableTime);
		fireFlag_ = true;
		fireWaitingMode_ = false;
	}
}

void terWeapon::destroyLink()
{
	if(fireTarget_ && (!fireTarget_->alive() || !fireTarget_->damageMolecula().isAlive()))
		fireTarget_ = NULL;
}

//-------------------------------------------------------

bool terWeaponArtillery::fire(const Vect3f& to,terUnitBase* target)
{
	xassert(owner() && missileID() != UNIT_ATTRIBUTE_NONE);
	
	Vect3f trg0,trg1;
	aimController()->getTargetingPosition(trg0,trg1);

	terProjectileBase* p = safe_cast<terProjectileBase*>(owner()->Player->buildUnit(missileID()));
	p->setSource(owner(),trg1,(trg1 - trg0));
	p->setTarget(target,to,setup().accuracyRadius);
	p->Start();

	addMissileReference();
	reloadStart();

	return true;
}

int terWeaponArtillery::estimatedDamage() const
{
	xassert(owner() && missileID() != UNIT_ATTRIBUTE_NONE);
	if(const AttributeBase* attr = owner()->Player->unitAttribute(missileID()))
		return attr->estimatedDamage();

	return 0;
}

//-------------------------------------------------------

terWeaponLauncher::terWeaponLauncher(terUnitReal* owner) : WeaponDirectionalBase(owner)
{
	missileIndex_ = 0;
}

void terWeaponLauncher::unload()
{
	if(setup().checkFlag(WEAPON_UNLOAD_WHEN_DISABLED)){
		if(owner() && owner()->docksCount()){
			for(int i = 0; i < owner()->docksCount(); i++){
				if(terDockingSlot* sp = owner()->dockingSlot(i)){
					if(sp->UnitPoint)
						sp->UnitPoint->Kill();
				}
			}
		}
	}
}

void terWeaponLauncher::enable()
{
	if(!isEnabled()){
		WeaponDirectionalBase::enable();

		xassert(owner() && owner()->docksCount());

		for(int i = 0; i < owner()->docksCount(); i++){
			if(terDockingSlot* sp = owner()->dockingSlot(i)){
				if(!sp->UnitPoint)
					sp->startTimer(setup().missileLoadDelay);
			}
		}
	}
}

void terWeaponLauncher::moveQuant()
{
	xassert(owner() && missileID() != UNIT_ATTRIBUTE_NONE);

	updateOwner();

	if(isEnabled()){
		for(int i = 0; i < owner()->docksCount(); i++){
			if(terDockingSlot* sp = owner()->dockingSlot(i)){
				if(!sp->UnitPoint && sp->timer()){
					terUnitReal* p = safe_cast<terUnitReal*>(owner()->Player->buildUnit(missileID()));
					p->SetDockingPoint(owner(),sp,DOCK_MODE_LOCK);
					p->Start();
				}
			}
		}
	}
}

bool terWeaponLauncher::fire(const Vect3f& to,terUnitBase* target)
{
	xassert(owner());

	if(terDockingSlot* sp = owner()->dockingSlot(missileIndex_)){
		if(sp->UnitPoint && sp->UnitPoint->dockMode() == DOCK_MODE_LOCK){
			terProjectileBase* p = safe_cast<terProjectileBase*>(sp->UnitPoint);

			p->setTarget(target,to);

			p->setDockMode(DOCK_MODE_LEAVE);
			missileIndex_ = ++missileIndex_ % owner()->docksCount();

			if(turnSuggestPrm()){ // Баллистическая ракета
				Vect3f trg0,trg1;
				aimController()->getTargetingPosition(trg0,trg1);

				p->clearDockingPoint();
				p->setSource(owner(),trg1,(trg1 - trg0));
				p->setTarget(target,to,setup().accuracyRadius);
				p->Start();
				p->setDockMode(DOCK_MODE_LEAVE);
			}

			sp->startTimer(setup().missileLoadDelay);
			addMissileReference();

			reloadStart();
			return true;
		}
	}

	return false;
}

int terWeaponLauncher::estimatedDamage() const
{
	xassert(owner() && missileID() != UNIT_ATTRIBUTE_NONE);
	if(const AttributeBase* attr = owner()->Player->unitAttribute(missileID()))
		return attr->estimatedDamage();

	return 0;
}

//-------------------------------------------------------

terWeaponLaser::terWeaponLaser(terUnitReal* owner) : WeaponDirectionalBase(owner)
{
}

void terWeaponLaser::avatarQuant()
{
	WeaponDirectionalBase::avatarQuant();

	if(isEnabled()){
		if(fireFlag()){
			terInterpolationLaser* lp = safe_cast<terInterpolationLaser*>(owner()->avatar());

			lp->SetGunEnable(1);
			lp->setLaserLine(firePosition());
		}
		else
			owner()->avatar()->SetGunEnable(0);
	}

	dropFireFlag();
}

bool terWeaponLaser::fire(const Vect3f& to,terUnitBase* target)
{
	xassert(owner());

	if(target){
		target->SetHotCount(5);
		target->setDamage(owner()->damageData(),owner());
		owner()->DestroyLink();
		destroyLink();
	}

	reloadStart();

	return true;
}

bool terWeaponLaser::fireTest(const Vect3f& to,terUnitBase* target,int& status) const
{
	if(!WeaponDirectionalBase::fireTest(to,target,status)) return false;

	xassert(owner());

	Vect3f pos;
	aimController() -> getGunPosition(pos);

	if(terShootMapTest){
		if(!CheckShotLine(pos,to)){
			status |= LEGION_FIRE_STATUS_GROUND_OBSTACLE;
			return false;
		}
	}

	Vect3f dr = pos - to;
	float d = dr.norm();

	if(!CheckFieldLine(pos,to,d,owner()->includingCluster())){
		status |= LEGION_FIRE_STATUS_GROUND_OBSTACLE;
		return false;
	}

	if(terShootUnitTest){
		terUnitGridFireCheckIgnoreOperator op(pos,to,d,COLLISION_GROUP_ENEMY, owner());

		if(!(universe()->UnitGrid.ConditionLine(pos.xi(),pos.yi(),to.xi(),to.yi(),op,2))){
			if(owner()->isEnemy(op.Contact))
				return true;
			else {
				if(op.IgnorePoint && op.IgnorePoint->position2D().distance2((const Vect2f&)pos) > 
				  op.Contact->position2D().distance2((const Vect2f&)pos))
					return true;
				else {
					status |= LEGION_FIRE_STATUS_FRIENDLY_FIRE;
					return false;
				}
			}
		}
		else {
			if(op.IgnorePoint && op.IgnorePoint->position2D().distance2((const Vect2f&)pos) < sqr(d)){
				status |= LEGION_FIRE_STATUS_FRIENDLY_FIRE;
				return false;
			}
		}
	}

	return true;
}

int terWeaponLaser::estimatedDamage() const
{
	xassert(owner());
	return owner()->attr().estimatedDamage();
}

void terWeaponLaser::disable()
{
	if(isEnabled()){
		xassert(owner());
		terInterpolationLaser* lp = safe_cast<terInterpolationLaser*>(owner()->avatar());
		lp->SetGunEnable(0);
	}

	WeaponDirectionalBase::disable();
}

//-------------------------------------------------------

bool terWeaponFreezeLaser::fire(const Vect3f& to,terUnitBase* target)
{
	xassert(owner());

	if(target){
		target->SetFreezeCount(round(float(terOfficerFreezeTime)/float(target->damageMolecula().elementCount())));
		owner()->DestroyLink();
	}

	reloadStart();

	return true;
}

//-------------------------------------------------------

bool terWeaponHealLaser::fire(const Vect3f& to,terUnitBase* target)
{
	xassert(owner());

	if(target){
		target->damageMoleculaRepair();
		safe_cast<terUnitReal*>(target)->setHealTimer(terTechnicHealTime);
	}

	reloadStart();

	return true;
}

//-------------------------------------------------------

void terWeaponPiercer::quant()
{
	WeaponOmnidirectionalBase::quant();

	if(fire_){
		xassert(owner() && missileID() != UNIT_ATTRIBUTE_NONE);

		terProjectileBase* p = safe_cast<terProjectileBase*>(owner()->Player->buildUnit(missileID()));
		p->setSource(owner(),owner()->position(),Vect3f::I);
		p->setPose(owner()->pose(),true);

		Vect3f dr = targetPos_ - owner()->position();
		dr.z = 0;

		dr.normalize(setup().accuracyRadius);

		p->setTarget(NULL,owner()->position() + dr);
		p->Start();

		owner()->Kill();

		fire_ = false;
	}
}

bool terWeaponPiercer::fire(const Vect3f& to,terUnitBase* target)
{
	fire_ = true;

	targetPos_ = (target) ? target->position() : to;

	reloadStart();

	return true;
}

//-------------------------------------------------------

void terWeaponDisintegrator::quant()
{
	WeaponOmnidirectionalBase::quant();

	if(isEnabled() && isLoaded() && isSwitchedOn() && fireEnabled()){
		xassert(owner());

		terUnitGridDisintegratorOperator op(owner(),0,setup().accuracyRadius);
		universe()->UnitGrid.Scan(owner()->position().xi(),owner()->position().yi(),setup().accuracyRadius,op);

		op.sortTargets();

		terUnitGridDisintegratorOperator::TargetContainer::const_iterator it;
		FOR_EACH(op.targets(),it){
			it->unit()->toggleDisintegrate();
			if(discharge(dischargeSpeed() * float(it->unit()->damageMolecula().aliveElementCount(DAMAGE_FILTER_BASE))))
				break;
		}

		startFireDelay();
	}
}

bool terWeaponDisintegrator::fire(const Vect3f& to,terUnitBase* target)
{
	switchOn();
	return true;
}

//-------------------------------------------------------

terWeaponScumHeater::terWeaponScumHeater(terUnitReal* owner) : WeaponOmnidirectionalBase(owner)
{
}

void terWeaponScumHeater::quant()
{
	WeaponOmnidirectionalBase::quant();

	if(isEnabled() && isSwitchedOn() && fireEnabled()){
		xassert(owner());

		terUnitGridAreaDamageOperator op(owner(),owner()->position2D(),0,setup().accuracyRadius);
		universe()->UnitGrid.Scan(owner()->position().xi(),owner()->position().yi(),setup().accuracyRadius,op);

		if(missileID() != UNIT_ATTRIBUTE_NONE){ // crater
			Vect3f pos = owner()->position();

			float angle = terLogicRNDfrand()*M_PI*2;
			float dist = terLogicRNDfrand()*setup().accuracyRadius;

			pos.x += dist * cos(angle);
			pos.y += dist * sin(angle);
			pos.z = 0;

			if(field_dispatcher->getIncludingCluster(pos) == owner()->includingCluster()){
				terUnitBase* p = owner()->Player->buildUnit(missileID());
				p->setPose(Se3f(QuatF::ID,pos),true);
				p->Start();
			}
		}

		startFireDelay();
	}
}

bool terWeaponScumHeater::fire(const Vect3f& to,terUnitBase* target)
{
	switchOn();
	return true;
}

//-------------------------------------------------------

terWeaponConductor::terWeaponConductor(terUnitReal* owner) : WeaponOmnidirectionalBase(owner),
	lighting_(0)
{
}

void terWeaponConductor::quant()
{
	WeaponOmnidirectionalBase::quant();

	if(isEnabled() && isSwitchedOn() && owner()->alive()){
		if(!lighting_){
			lighting_ = new cLighting;
			terScene->AttachObj(lighting_);
		}
		Vect3f v0;
		aimController()->getGunPosition(v0);
		vector<Vect3f> vect;

		if(fireEnabled()){
			xassert(owner() && lighting_);

			terUnitGridConductorOperator op(owner(),owner()->position2D(),0,setup().accuracyRadius);
			universe()->UnitGrid.Scan(owner()->position().xi(),owner()->position().yi(),setup().accuracyRadius,op);

			if(missileID() != UNIT_ATTRIBUTE_NONE){ // crater
				Vect3f pos = owner()->position();

				float angle = terLogicRNDfrand()*M_PI*2;
				float dist = terLogicRNDfrand()*setup().accuracyRadius;

				pos.x += dist * cos(angle);
				pos.y += dist * sin(angle);
				pos.z = 0;

				if(field_dispatcher->getIncludingCluster(pos) == owner()->includingCluster()){
					terUnitBase* p = owner()->Player->buildUnit(missileID());
					p->setPose(Se3f(QuatF::ID,pos),true);
					p->Start();
				}
			}
			vect.insert(vect.end(), op.targets().begin(), op.targets().end());
			startFireDelay();
		}

		if(vect.empty()){
			for(int i = 0; i < 5; i++){
				Vect2f pos = owner()->position2D();

				float angle = terLogicRNDfrand()*M_PI*2;
				float dist = terLogicRNDfrand()*setup().accuracyRadius;

				pos.x += dist * cos(angle);
				pos.y += dist * sin(angle);

				vect.push_back(To3D(pos));
			}
		}

		lighting_->Init(v0, vect);
	}
	else
		releaseLighting();
}

bool terWeaponConductor::fire(const Vect3f& to,terUnitBase* target)
{
	switchOn();
	return true;
}

void terWeaponConductor::kill()
{
	WeaponOmnidirectionalBase::kill();
	releaseLighting();
}

void terWeaponConductor::disable()
{
	WeaponOmnidirectionalBase::disable();
	releaseLighting();
}

//-------------------------------------------------------

terWeaponScumTwister::terWeaponScumTwister(terUnitReal* owner) : WeaponOmnidirectionalBase(owner), missile_(NULL)
{
	targetPos_ = Vect3f(0,0,0);
}

terWeaponScumTwister::~terWeaponScumTwister()
{
}

void terWeaponScumTwister::quant()
{
	WeaponOmnidirectionalBase::quant();

	if(isSwitchedOn() && owner()->alive()){
		if(isEnabled() && fireEnabled()){
			if(!missile_){
				missile_ = owner()->Player->buildUnit(UNIT_ATTRIBUTE_SCUM_STORM);
				missile_->setPose(Se3f(QuatF(0.0f,Vect3f::K),owner()->position()),true);

				Vect3f pos = targetPos_;

				terProjectileBase* p = safe_cast<terProjectileBase*>(missile_);
				p->setTarget(NULL,targetPos_);

				missile_->Start();
			}

			terUnitGridAreaDamageOperator op(owner(),missile_->position2D(),0,setup().accuracyRadius);
			universe()->UnitGrid.Scan(missile_->position().xi(),missile_->position().yi(),setup().accuracyRadius,op);

			if(missileID() != UNIT_ATTRIBUTE_NONE){ // crater
				terUnitBase* p = owner()->Player->buildUnit(missileID());

				Vect3f pos = missile_->position();

				float angle = terLogicRNDfrand()*M_PI*2;
				float dist = terLogicRNDfrand()*setup().accuracyRadius;

				pos.x += dist * cos(angle);
				pos.y += dist * sin(angle);

				p->setPose(Se3f(QuatF::ID,pos),true);
				p->Start();
			}

			startFireDelay();
		}
	}
	else {
		if(missile_){
			missile_->Kill();
			missile_ = NULL;
		}
	}
	
	if(!missile_)
		setFireAnimationMode(false);
}

SaveWeaponData* terWeaponScumTwister::universalSave(SaveWeaponData* data)
{
	if(isSwitchedOn() && missile_){
		if(!data)
			data = new SaveWeaponData;

		WeaponOmnidirectionalBase::universalSave(data);

		data->targetPosition = targetPos_;
		data->missile = missile_;
	}

	return data;
}

void terWeaponScumTwister::universalLoad(const SaveWeaponData* data)
{
	WeaponOmnidirectionalBase::universalLoad(data);

	if(!data) return;

	targetPos_ = data->targetPosition;
	data->missile.setLink((terUnitBase*&)missile_);
}

bool terWeaponScumTwister::fire(const Vect3f& to,terUnitBase* target)
{
	switchOn();
	targetPos_ = (target) ? target->position() : to;
	return true;
}

void terWeaponScumTwister::destroyLink()
{
	WeaponOmnidirectionalBase::destroyLink();

	if(missile_ && !missile_->alive()){
		missile_ = NULL;
		reloadStart();

		setFireAnimationMode(false);
	}
}

void terWeaponScumTwister::unload()
{
	WeaponOmnidirectionalBase::unload();

	if(isSwitchedOn()){
		if(missile_)
			missile_->Kill();

		missile_ = NULL;

		reloadStart();
	}
}

void terWeaponScumTwister::kill()
{
	if(missile_)
		missile_->Kill();

	missile_ = NULL;
}

//-------------------------------------------------------

void terWeaponScumSplitter::quant()
{
	WeaponOmnidirectionalBase::quant();

	if(isSwitchedOn()){
		if(isEnabled() && fireEnabled()){
			if(!missile_){
				xassert(owner() && missileID() != UNIT_ATTRIBUTE_NONE);

				if(!missile_){
					missile_ = owner()->Player->buildUnit(missileID());

					Vect3f pos = targetPos_;
					float angle = terLogicRNDfrand()*M_PI*2;
					float radius = setup().accuracyRadius * terLogicRNDfrand();

					pos.x += radius * cos(angle);
					pos.y += radius * sin(angle);

					missile_->setPose(Se3f(QuatF(angle,Vect3f::K),pos),true);
					missile_->Start();
				}
			}

			startFireDelay();
		}
	}
	else {
		if(missile_){
			missile_->Kill();
			missile_ = NULL;
		}
	}
	
	if(!missile_)
		setFireAnimationMode(false);
}

void terWeaponScumSplitter::destroyLink()
{
	WeaponOmnidirectionalBase::destroyLink();

	if(missile_ && !missile_->alive()){
		missile_ = NULL;
		reloadStart();

		setFireAnimationMode(false);
	}
}

bool terWeaponScumSplitter::fire(const Vect3f& to,terUnitBase* target)
{
	switchOn();
	targetPos_ = (target) ? target->position() : to;
	return true;
}

//-------------------------------------------------------

terWeaponLeech::terWeaponLeech(terUnitReal* owner) : WeaponOmnidirectionalBase(owner),
	target_(NULL),
	link_(NULL)
{
	link_ = safe_cast<cLine3d*>(terScene->CreateLine3d(terTextureEnergyLinkTrail));
	link_->SetAttr(ATTRUNKOBJ_ADDBLEND | ATTRUNKOBJ_REFLECTION);
	link_->SetAnimation(8.0f,0);
	link_->SetAttr(ATTRUNKOBJ_IGNORE);
}

terWeaponLeech::~terWeaponLeech()
{
	link_->Release();
}

void terWeaponLeech::quant()
{
	WeaponOmnidirectionalBase::quant();

	if(isSwitchedOn() && target_){
		xassert(owner());

		if(!safe_cast<const terBuilding*>(target_)->isConnected())
			switchOff();

		if(owner()->realAvatar()->chainID() == CHAIN_FIRE){
			xassert(owner() && owner()->attr().productionConsumption.enabled());

			if(!weaponConsumption_.attached())
				weaponConsumption_.attach(target_->Player, owner()->attr().productionConsumption);

			weaponConsumption_.requestCharge();

			Vect3f v0 = safe_cast<terInterpolationLeech*>(owner()->avatar())->getConnectionPosition();
			Vect3f v1 = safe_cast<terInterpolationConnection*>(target_->avatar())->GetConnectionPosition(owner()->position());

			sVertexLine line[2];
			line[0].pos=v0;
			line[1].pos=v1;
			line[0].color=line[1].color=sColor4c(255,255,255,255);
			line[0].v=0;
			line[1].v=line[0].pos.distance(line[1].pos)/64.0f;
			line[0].width=line[1].width=2.0f;

			link_->UpdateVertex(2, line);
			link_->ClearAttr(ATTRUNKOBJ_IGNORE);
		}
		else
			link_->SetAttr(ATTRUNKOBJ_IGNORE);
	}
	else {
		if(weaponConsumption_.attached()) weaponConsumption_.detach();
		link_->SetAttr(ATTRUNKOBJ_IGNORE);
		target_ = NULL;
	}
}

bool terWeaponLeech::fire(const Vect3f& to,terUnitBase* target)
{
	switchOn();
	target_ = target;

	return true;
}

bool terWeaponLeech::fireTest(const Vect3f& to,terUnitBase* target,int& status) const
{
	if(!WeaponOmnidirectionalBase::fireTest(to,target,status)) return false;

	if(target && !safe_cast<const terBuilding*>(target)->isConnected()){
		status |= LEGION_FIRE_STATUS_BAD_TARGET;
		return false;
	}

	return true;
}

void terWeaponLeech::destroyLink()
{
	WeaponOmnidirectionalBase::destroyLink();

	if(target_ && !target_->alive())
		target_ = NULL;
}

//-------------------------------------------------------

terWeaponScumDisruptor::terWeaponScumDisruptor(terUnitReal* owner) : WeaponOmnidirectionalBase(owner),
	missile_(NULL)
{
}

SaveWeaponData* terWeaponScumDisruptor::universalSave(SaveWeaponData* data)
{
	if(isSwitchedOn() && missile_){
		if(!data)
			data = new SaveWeaponData;

		WeaponOmnidirectionalBase::universalSave(data);

		data->missile = missile_;
	}

	return data;
}

void terWeaponScumDisruptor::universalLoad(const SaveWeaponData* data)
{
	WeaponOmnidirectionalBase::universalLoad(data);

	if(!data) return;

	data->missile.setLink((terUnitBase*&)missile_);
}

void terWeaponScumDisruptor::quant()
{
	WeaponOmnidirectionalBase::quant();

	if(!isSwitchedOn()){
		if(missile_){
			missile_->SetFreeDestroy();
			missile_ = NULL;
		}
	}
}

void terWeaponScumDisruptor::disable()
{
	if(isSwitchedOn()){
		if(missile_)
			missile_->SetFreeDestroy();

		missile_ = NULL;

		reloadStart();
	}

	WeaponOmnidirectionalBase::disable();
}

void terWeaponScumDisruptor::destroyLink()
{
	WeaponOmnidirectionalBase::destroyLink();

	if(missile_ && !missile_->alive())
		missile_ = NULL;
}

bool terWeaponScumDisruptor::fire(const Vect3f& to,terUnitBase* target)
{
	xassert(owner());
	if(missile_) return false;

	missile_ = safe_cast<terFilthSpot*>(owner()->Player->buildUnit(UNIT_ATTRIBUTE_FILTH_SPOT));

	Vect3f trg_pos = (target) ? target->position() : to;

	float dist_factor = owner()->position().distance(trg_pos) / 1000.0f;
	float life_time = reloadConsumption().energy() / (10.0f * setup().dischargeSpeed * dist_factor);

	setDischargeSpeed(setup().dischargeSpeed * dist_factor);

	missile_->setParameters(&terScumDisruptorPrm);
	missile_->setPosition(trg_pos);
	missile_->Start();

	if(setup().cleanTargetAfterFire())
		clearOwnerTarget();

	switchOn();

	return true;
}

//-------------------------------------------------------

terWeaponFilthNavigator::terWeaponFilthNavigator(terUnitReal* owner) : WeaponOmnidirectionalBase(owner),
	targetPos_(0,0,0)
{
	prmIndex_ = 0;
}

void terWeaponFilthNavigator::quant()
{
	WeaponOmnidirectionalBase::quant();

	if(isEnabled() && isLoaded() && isSwitchedOn() && fireEnabled()){
		xassert(owner());

		terFilthSpot* p = safe_cast<terFilthSpot*>(owner()->Player->buildUnit(UNIT_ATTRIBUTE_FILTH_SPOT));

		p->setParameters(&terFilthNavigatorPrm[prmIndex_]);
		p->scaleTime(1000.0f / targetPos_.distance(owner()->position()));

		p->setPosition(targetPos_);

		p->Start();

		if(prmIndex_ < terFilthNavigatorPrmCount - 1)
			prmIndex_++;

		startFireDelay();
	}
}

bool terWeaponFilthNavigator::fire(const Vect3f& to,terUnitBase* target)
{
	if(!isSwitchedOn())
		prmIndex_ = 0;
	targetPos_ = (target) ? target->position() : to;

	switchOn();

	return true;
}

SaveWeaponData* terWeaponFilthNavigator::universalSave(SaveWeaponData* base_data)
{
	if(isSwitchedOn()){
		SaveWeaponFilthNavigatorData* data = castOrCreate<SaveWeaponFilthNavigatorData>(base_data);

		WeaponOmnidirectionalBase::universalSave(data);

		data->prmIndex = prmIndex_;
		return data;
	}

	return base_data;
}

void terWeaponFilthNavigator::universalLoad(const SaveWeaponData* base_data)
{
	WeaponOmnidirectionalBase::universalLoad(base_data);

	if(const SaveWeaponFilthNavigatorData* data = dynamic_cast<const SaveWeaponFilthNavigatorData*>(base_data)){
		prmIndex_ = data->prmIndex;
	}
}

//-------------------------------------------------------

terWeaponFilthSpot::terWeaponFilthSpot(terUnitReal* owner) : WeaponOmnidirectionalBase(owner)
{
}

bool terWeaponFilthSpot::fire(const Vect3f& to,terUnitBase* target)
{
	xassert(owner());

	terFilthSpot* p = safe_cast<terFilthSpot*>(owner()->Player->buildUnit(UNIT_ATTRIBUTE_FILTH_SPOT));

	Vect3f trg_pos = (target) ? target->position() : to;

	float angle = terLogicRNDfrand()*M_PI*2;
	float radius = setup().accuracyRadius * terLogicRNDfrand();

	trg_pos.x += radius * cos(angle);
	trg_pos.y += radius * sin(angle);

	p->setParameters(&terFilthMutationsPrm[setup().weaponIndex]);
	p->setPosition(trg_pos);

	if(target)
		p->SetAttackTarget(target);

	p->Start();

	reloadStart();

	return true;
}

//-------------------------------------------------------

void terWeaponInvisibilityGenerator::quant()
{
	WeaponOmnidirectionalBase::quant();

	if(isEnabled() && isLoaded()){
		if(isSwitchedOn()){
			if(fireEnabled()){
				xassert(owner());

				terUnitGridInvisibilityGeneratorOperator op(owner(),0,setup().accuracyRadius);
				universe()->UnitGrid.Scan(owner()->position().xi(),owner()->position().yi(),setup().accuracyRadius,op);

				startFireDelay();
			}
		}
		else {
			int status = 0;
			Vect3f v(owner()->position());
			fireRequest(&v,NULL,status);
		}
	}
}

bool terWeaponInvisibilityGenerator::fire(const Vect3f& to,terUnitBase* target)
{
	switchOn();
	return true;
}

//-------------------------------------------------------

terWeaponLighting::terWeaponLighting(terUnitReal* owner) : WeaponDirectionalBase(owner),
	target_(NULL),
	lighting_(NULL)
{
}

terWeaponLighting::~terWeaponLighting()
{
}

void terWeaponLighting::quant()
{
	WeaponDirectionalBase::quant();

	if(isSwitchedOn() && target_ && owner()->alive()){
		xassert(owner());

		if(isFireAnimationPlaying()){
			if(fireEnabled()){
				target_->SetHotCount(5);
				target_->setDamage(owner()->damageData(),owner());
				owner()->DestroyLink();
				destroyLink();
				if(!target_)
					return;
			}

			if(!lighting_){
				lighting_ = new cLighting;
				terScene->AttachObj(lighting_);
			}

			Vect3f v0,v1;
			aimController()->getTargetingPosition(v0,v1);
			v1 = target_->position();

			vector<Vect3f> vect;
			vect.push_back(v1);
			lighting_->Init(v0, vect);
		}
	}
	else
		releaseLighting();
}

bool terWeaponLighting::fire(const Vect3f& to,terUnitBase* target)
{
	switchOn();
	target_ = target;

	return true;
}

bool terWeaponLighting::fireTest(const Vect3f& to,terUnitBase* target,int& status) const
{
	if(!WeaponDirectionalBase::fireTest(to,target,status)) return false;

	xassert(owner());

	Vect3f pos;
	aimController() -> getGunPosition(pos);

	if(terShootMapTest){
		if(!CheckShotLine(pos,to)){
			status |= LEGION_FIRE_STATUS_GROUND_OBSTACLE;
			return false;
		}
	}

	Vect3f dr = pos - to;
	float d = dr.norm();

	if(!CheckFieldLine(pos,to,d,owner()->includingCluster())){
		status |= LEGION_FIRE_STATUS_GROUND_OBSTACLE;
		return false;
	}

	if(terShootUnitTest){
		terUnitGridFireCheckIgnoreOperator op(pos,to,d,COLLISION_GROUP_ENEMY, owner());

		if(!(universe()->UnitGrid.ConditionLine(pos.xi(),pos.yi(),to.xi(),to.yi(),op,2))){
			if(owner()->isEnemy(op.Contact))
				return true;
			else {
				if(op.IgnorePoint && op.IgnorePoint->position2D().distance2((const Vect2f&)pos) > 
				  op.Contact->position2D().distance2((const Vect2f&)pos))
					return true;
				else {
					status |= LEGION_FIRE_STATUS_FRIENDLY_FIRE;
					return false;
				}
			}
		}
		else {
			if(op.IgnorePoint && op.IgnorePoint->position2D().distance2((const Vect2f&)pos) < sqr(d)){
				status |= LEGION_FIRE_STATUS_FRIENDLY_FIRE;
				return false;
			}
		}
	}

	return true;
}

int terWeaponLighting::estimatedDamage() const
{
	xassert(owner());
	return owner()->attr().estimatedDamage();
}
	
void terWeaponLighting::destroyLink()
{
	WeaponDirectionalBase::destroyLink();

	if(target_ && !target_->alive())
		target_ = NULL;
}

void terWeaponLighting::kill()
{
	WeaponDirectionalBase::kill();
	releaseLighting();
}

void terWeaponLighting::disable()
{
	WeaponDirectionalBase::disable();
	releaseLighting();
}
