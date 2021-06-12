#ifndef _PERIMETER_SECOND_GUN_
#define _PERIMETER_SECOND_GUN_

/// Оружие - базовый класс.
class terWeapon
{
public:
	terWeapon(terUnitReal* owner);
	virtual ~terWeapon();

	virtual bool init();

	virtual void quant();

	virtual void moveQuant();

	virtual void avatarQuant() = 0;
	virtual void avatarInterpolation() = 0;

	virtual bool fire(const Vect3f& to,terUnitBase* target) = 0;
	virtual bool fireRequest(const Vect3f* to,terUnitBase* target,int& status);
	virtual bool fireTest(const Vect3f& to,terUnitBase* target,int& status) const;

	bool fireDistanceCheck(const Vect3f& to) const;

	virtual bool isLoaded() const 
	{ 
		xassert(owner_); 
		if(reloaded() && !fireWaitingMode_ && (!setup_.missileLimit || missileCount_ < setup_.missileLimit) &&
			(!setup_.checkFlag(WEAPON_READY_ANIMATION) || (owner_->realAvatar()->chainID() == CHAIN_SWITCHED_ON && owner_->realAvatar()->requestDone()))) return true; 
		else
			return false;
	}

	virtual void unload(){ fireFlag_ = false; fireAnimationMode_ = false; fireWaitingMode_ = false; }
	void reloadStart();

	bool isReady() const { return (isEnabled() && isLoaded() && !fireWaitingMode_); }

	bool isEnabled() const { return isEnabled_; }
	virtual void enable(){ if(!isEnabled_){ isEnabled_ = true; } }
	virtual void disable(){ switchOff(); isEnabled_ = false; unload(); }

	bool needCharge() const { return setup_.needCharge(); }

	bool isSwitchedOn() const { if(!setup_.needCharge()) return true; return isOn_; }
	void switchOn(){ isOn_ = true; }
	void switchOff();
	
	virtual void updateLogic() = 0;

	virtual void destroyLink();

	void removeMissileReference();

	/// оценка количества элементов, вышибаемых оружием с одного выстрела
	virtual int estimatedDamage() const = 0;

	static terWeapon* create(WeaponType type,terUnitReal* owner);
	static void release(terWeapon* p){ delete p; }

	float chargeLevel() const { return reloadConsumption_.progress(); }
	void setChargeLevel(float level) { reloadConsumption_.setProgress(level); }

	/// Возвращает ID желаемой цепочки анимации.
	ChainID chainRequest() const;

	void changePlayer(terPlayer* player) { reloadConsumption_.changePlayer(player); }
	terUnitBase* fireTarget(){ return fireTarget_; }

	bool isFiring() const { return fireAnimationMode(); }

	virtual void kill(){ }

	virtual SaveWeaponData* universalSave(SaveWeaponData* data);
	virtual void universalLoad(const SaveWeaponData* data);

	int missileCount() const { return missileCount_; }

protected:
	terUnitReal* owner(){ return owner_; }
	const terUnitReal* owner() const { return owner_; }

	terUnitAttributeID missileID() const { return setup_.missileID; }

	virtual bool aim(const Vect3f& to,int& status) = 0;
	virtual void aimDefault() = 0;

	bool reloaded() const { return reloaded_; }
	int reloadTime() const { return setup_.reloadConsumption.time; }

	bool fireFlag() const { return fireFlag_; }
	void dropFireFlag(){ fireFlag_ = false; }

//	float fireDistance() const { return fireDistance_; }
	
	void updateOwner();

	const terWeaponSetup& setup() const { return setup_; }
	const EnergyConsumer& reloadConsumption() const { return reloadConsumption_; }

	void addMissileReference(){ missileCount_++; }

	bool fireAnimationMode() const { return fireAnimationMode_; }
	void setFireAnimationMode(bool mode){ fireAnimationMode_ = mode; }

	bool discharge(float energy){ return reloadConsumption_.discharge(energy); }
	float dischargeSpeed() const { return dischargeSpeed_; }
	void setDischargeSpeed(float speed){ dischargeSpeed_ = speed; }

	bool fireEnabled() const { return !fireDelayTimer_(); } 
	void startFireDelay(){ fireDelayTimer_.start(setup_.fireDelay); }

	const Vect3f& firePosition() const { return firePosition_; }

	void clearOwnerTarget(){ xassert(owner_); owner_->clearAttackTarget(); }

	bool isFireAnimationPlaying() const;

private:
	NodeController* fireController_;

	/// время перезарядки, в миллисекундах
	EnergyConsumer reloadConsumption_;
	bool reloaded_;

	/// расстояние до цели
//	float fireDistance_;
	/// координаты цели
	Vect3f firePosition_;
	/// цель
	terUnitBase* fireTarget_;

	/// количество выстреленных снарядов
	int missileCount_;

	/// true если оружие наводится на цель, false если надо развернуть по-умолчанию
	bool aimFlag_;
	DurationTimer aimTimer_;

	DurationTimer fireDisableTimer_;

	DurationTimer fireDelayTimer_;

	/// true если оружие стреляет
	bool fireFlag_;
	/// true если ожидается анимация выстрела
	bool fireWaitingMode_;
	/// true если включена анимация выстрела
	bool fireAnimationMode_;

	bool isEnabled_;

	bool isOn_;

	// скорость разрядки - сколько отнимается от reloadConsumption_ в логический квант
	float dischargeSpeed_;

	terUnitReal* owner_;
	const terWeaponSetup& setup_;

	void fireEvent(const Vect3f& to,terUnitBase* target);

	virtual bool allowCharge() const { xassert(owner()); return (!setup_.needCharge() || owner()->realAvatar()->chainID() == CHAIN_MOVE || owner()->realAvatar()->chainID() == CHAIN_STOP || owner()->realAvatar()->chainID() == CHAIN_PRODUCTION || owner()->realAvatar()->chainID() == CHAIN_SWITCHED_ON); }
	virtual bool allowDischarge() const { return true; }
};

#endif