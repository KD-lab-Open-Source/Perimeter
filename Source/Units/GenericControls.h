#ifndef __PERIMETER_GENERIC_CONTROLS_
#define __PERIMETER_GENERIC_CONTROLS_

#include "UnitAttribute.h"
#include "Grid2D.h"
#include "CommonCommands.h"

class terPlayer;
class terInterpolationBase;
struct SaveUnitData;

//-----------------------------------------
//		Creation Template
class UnitTemplate
{
public:
	UnitTemplate(const AttributeBase* attribute, terPlayer* player) : attribute_(attribute), player_(player) {}

	const AttributeBase* attribute() const { return attribute_; }
	terPlayer* player() const { return player_; }

private:
	const AttributeBase* attribute_;
	terPlayer* player_;
};

class terUnitBase : public terUnitID, public ShareHandleBase
{
public:
	terUnitBase(const UnitTemplate& data);
	virtual ~terUnitBase();

	virtual void MoveQuant() {}
	virtual void Quant() {}
	virtual void Kill();
	virtual void DestroyLink();

	bool alive() const { return alive_; }
	
	bool dead() const { return dead_; }
	void makeDead() { dead_ = true;	}

	const char* label() const { return label_.c_str(); }

	//----------------------------------------------------
	int collisionGroup() const { return collisionGroup_; }
	void setCollisionGroup(int group) { collisionGroup_ = group; }
	int excludeCollision() const { return attr().ExcludeCollision; }

	int GetMapUpdatedCount() const { return MapUpdatedCount; }
	void SetMapUpdatedCount(int count){ MapUpdatedCount = count; }

	//-----------------------------------------------------
	// Команды
	typedef list<UnitCommand> CommandList;
	CommandList& commandList(){ return commandList_; }
	CommandID lastCommand() const { return commandList_.empty() ? COMMAND_ID_NONE : commandList_.back().commandID(); }
	const UnitCommand* findCommand(CommandID commandID) const;
	
	void commandOutcoming(const UnitCommand& command);
	void commandOutcomingLogic(const UnitCommand& command);
	void commandIncoming(const UnitCommand& command);
	virtual void executeCommand(const UnitCommand& command);

	//------------------------------------------
	//	Интерфейс
	virtual bool selectAble() const;
	bool selected() const { return selected_; }
	float life() const { return damageMolecula().elementCount() ? clamp(float(damageMolecula().aliveElementCount())/float(damageMolecula().elementCount()), 0, 1) : 0; }
	float maxHealth() const { return damageMolecula().elementCount(); }
	bool marked() const { return marked_; }

	virtual void Select(){ selected_ = true; }
	virtual void Deselect(){ selected_ = false; }
	
	virtual void Mark(){ marked_ = true; }
	virtual void Unmark(){ marked_ = false; }
	
	virtual void ShowInfo() {} // Вывод интерфейсной инфы: путь, лайф-бар

	//-----------------------------------------------------
	//	Интерполяция
	terInterpolationBase* avatar() const { return avatar_; }
	
	virtual void AvatarQuant();//Специально для Seeler: ни в коем случае не вызывать явно AvatarQuant()
	virtual void AvatarInterpolation();
	
	virtual void UpdateSkinColor();
	
	//-----------------------------------------------------
	virtual void Start() {}

	virtual void setRealModel(int modelIndex, float scale) {} 

	virtual SaveUnitData* universalSave(SaveUnitData* data);
	virtual void universalLoad(const SaveUnitData* data);

	virtual void Collision(terUnitBase* p) {}

	virtual void RefreshAttribute() {}

	virtual void showDebugInfo();
	virtual void WriteDebugInfo(XBuffer& buf) {}

	//-----------------------------------------------------
	//	Координаты
	virtual void setPose(const Se3f& pose, bool initPose); // true - инициализация - выставление в первый раз с изменением z
														   // основная функция координатных фич, все остальные сеттеры - просто short-cuts	
	const Se3f& pose() const { return pose_; }

	const Vect3f& position() const { return pose_.trans(); }
	const Vect2f& position2D() const { return *((const Vect2f*)&pose_.trans()); } 
	void setPosition(const Vect3f& position) { setPose(Se3f(orientation(), position), false); }
	void setPositionXY(const Vect2f& position2D) { setPose(Se3f(orientation(), Vect3f(position2D.x, position2D.y, position().z)), false); }
	void setPositionZ(float z) { setPose(Se3f(orientation(), Vect3f(position().x, position().y, z)), false); }

	const QuatF& orientation() const { return pose_.rot(); }
	void setOrientation(const QuatF& orientation) { setPose(Se3f(orientation, position()), false); }

	float angleZ() const; // slow!!!
	void setAngleZ(float angle) { setOrientation(QuatF(angle, Vect3f::K)); }

	float radius() const { return radius_; } 
	virtual void setRadius(float radius) { radius_ = radius; }

	//-----------------------------------------------------
	// Damage System
	const DamageMolecula& damageMolecula() const { return damageMolecula_; }
	void damageMoleculaZeroize(){ damageMolecula_.zeroize(); }
	void setDamageMolecula(const DamageMolecula& p) { damageMolecula_ = p; }

	void damageMoleculaKill();
	virtual int damageMoleculaRepair(int element_count = 1){ return damageMolecula_.repair(element_count); }
	virtual void setDamage(const DamageData& damage,terUnitBase* p = 0);

	int repairRequested() const { return repairRequested_; }
	virtual int repairRequest() const { int count = damageMolecula_.deadElementCount() - repairRequested_; if(count > 0) return count; return 0; }
	void changeRepairRequest(int delta){ repairRequested_ += delta; if(repairRequested_ < 0) repairRequested_ = 0; }
	void clearRepairRequest(){ repairRequested_ = 0; }

	/// возвращает true, если юнит ремонтируется как здание (строительными блоками)
	virtual bool needBuildingRepair() const { return false; }

	//-----------------------------------------------------
	virtual class RigidBody* GetRigidBodyPoint() const { return 0; }

	virtual bool isDocked() const { return false; }
	 
	//-----------------------------------------------------
	int includingCluster() const { return includingCluster_; }
	void updateIncludingCluster();

	//-----------------------------------------------------
	virtual void setModelName(const char* name){}
	virtual const char* GetModelName(){ return 0; }

	//-------------------------------------
	virtual class terUnitSquad* GetSquadPoint(){ return 0; }

	virtual terUnitBase* GetIgnoreUnit(){ return 0; }

	virtual int isSingleSelection(){ return 0; }

	int unitClass() const { return unitClass_; }
	void setUnitClass(int unit_class){ unitClass_ = unit_class; }

	//-------------------------------------
	virtual bool isConstructed() const { return true; }
	virtual bool isUpgrading() const { return false; }

	virtual bool isBuilding() const { return attr().isBuilding(); }

	virtual bool isBuildingEnable() const { return false; }
	virtual bool readyToConnect() const { return false; }
	
	//------------------------------------------------
	virtual float chargeLevel() const { return 0; }

	//------------------------------------------------
	virtual void MapUpdateHit(float x0,float y0,float x1,float y1){}

	//---------------------------------------------
	virtual void ShowCircles();

	virtual int LegionFireCheck(Vect3f& from,Vect3f& to,Vect3f& center,terUnitBase* target){ return 0; }
	virtual int LegionFireEvent(Vect3f& from,Vect3f& to,Vect3f& center,terUnitBase* target,int& fire_status){ return 0; }

	bool isEnemy(const terUnitBase* unit) const; // this should attack unit
	bool isHarmful(const terUnitBase* unit) const; // this take damage from unit
	bool checkFireClass(const terUnitBase* p) const { return p->unitClass() & attr().AttackClass; }

	virtual void setActivity(bool activate) {} // To activate/deactivates some props by triggers
	virtual bool activity() const { return true; } 
	
	virtual void SetFreezeCount(int count){ }
	virtual void SetHotCount(int count){ }
	virtual float freezeFactor() const { return 0.0f; }

	virtual void ChangeUnitOwner(terPlayer* player);

	//-----------------------------------------
	virtual int GetLegionMorphing()const{ return 1; }
	virtual int GetInterfaceLegionMode(){ return 0; }

	virtual bool isUnseen() const { return false; }

	virtual int isBuildingPowerOn(){ return 0; }
	virtual int CanBuildingEnable(){ return 0; }
	
	const DamageData& damageData() const { return attr().unitDamage.mainDamage; }

	int possibleDamage() const { return possibleDamage_; }
	void clearPossibleDamage(){ possibleDamage_ = 0; }
	void addPossibleDamage(int damage_delta){ possibleDamage_ += damage_delta; if(possibleDamage_ < 0) possibleDamage_ = 0; }

	//-----------------------------------------------------
	int GetRealCollisionCount() const { return RealCollisionCount; }
	void SetRealCollisionCount(int count){ RealCollisionCount = count; }
	
	//-----------------------------------------

	virtual bool soundEvent(SoundEventID event_id);
	bool soundEvent(const SoundEventSetup* ev) const;

	//struct AttributeBase& Attribute;
	terPlayer* Player;

	void DeleteInterpolator();
	virtual bool needCrater() const { return true; }

	const AttributeBase& attr() const {
		return *attr_;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_NAME(attr_, "attribute", "Свойство");
//		ar & WRAP_OBJECT(position_);
//		ar & WRAP_OBJECT(orientaion);
//		ar & WRAP_OBJECT(radius);
		ar & TRANSLATE_OBJECT(label, "Метка");
	}

protected:
	float radius_;
	
	virtual void setFieldDamage(const DamageData& damage);

private:
	AttributeLibrary::Reference attr_;

	bool alive_;
	bool dead_;

	int unitClass_;

	string label_;

	Se3f pose_;

	int RealCollisionCount;
	int MapUpdatedCount;

	terInterpolationBase* avatar_;

	CommandList commandList_;

	int collisionGroup_;

	DamageMolecula damageMolecula_;

	/// количество элементов, которое потенциально может быть выбито
	/** 
	рассчитывается на основе того, сколько в данный момент в юнит целится
	ракет, пушек и т.д.
	*/
	int possibleDamage_;

	/// заказанный ремонт
	/**
	для зданий и фрейма - количество элементов
	в летящих к ним строительных блоках
	*/
	int repairRequested_;

	// Interface
	bool selected_;
	bool selectedEnemy_;
	bool marked_;

	int includingCluster_;
};

typedef TypeLibrary<string, terUnitBase> UnitLibrary;

template<class UnitList>
bool removeNotAlive(UnitList& unitList) 
{
	UnitList::iterator i = remove_if(unitList.begin(), unitList.end(), not1(mem_fun(&terUnitBase::alive)));
	if(i != unitList.end()){
		unitList.erase(i, unitList.end());
		return true;
	}
	return false;
}

typedef list<terUnitBase*> UnitList; // используется свойство сохранения итераторов

#endif
