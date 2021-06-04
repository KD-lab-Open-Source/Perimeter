#ifndef __IRONBUILDING_H__
#define __IRONBUILDING_H__

#include "Scripts\Config.hi"

class terIconBuilding 
{
public:
	terIconBuilding(const ModelProperty& model_prop);
	~terIconBuilding();
	void quant(); // hides the icon, call before show
	void show(const Vect3f& pos);
	
private:
	cObjectNodeRoot* model_;
	float phase_;
};

//--------------------------------------
class AttributeBuilding : public AttributeReal
{
public:
	vector<EnumWrapper<terUnitAttributeID> > EnableStructure;
	vector<EnumWrapper<terUnitAttributeID> > Downgrades; // для сканирования, в _обратном_ порядке
	EnumWrapper<terUnitAttributeID> Upgrade; // Только ближайший апгрейд

    int constructionPriority; // приоритет строительства
	bool disconnectAnimation; // есть анимация для отключенного состояния (иначе просто останавливается текущая анимация)
   
	AttributeBuilding();

	template<class Archive>
	void serialize(Archive& ar) {
		__super::serialize(ar);
		
		if(ar.openBlock("attributeBuilding", "Здание")){
			ar & TRANSLATE_OBJECT(EnableStructure, "Требуемые для работы строения");
			ar & TRANSLATE_OBJECT(Downgrades, "Даунгрейды для сканирования, в _обратном_ порядке");
			ar & TRANSLATE_OBJECT(Upgrade, "Ближайший апгрейд");
			ar & TRANSLATE_OBJECT(isUpgrade, "isUpgrade");

			ar & TRANSLATE_OBJECT(constructionSpeedCoeff, "constructionSpeedCoeff");
			ar & TRANSLATE_OBJECT(constructionPriority, "Приоритет строительства");
			ar & TRANSLATE_OBJECT(disconnectAnimation, "есть анимация для отключенного состояния (иначе просто останавливается текущая анимация)");

			ar & TRANSLATE_OBJECT(iconDistanceFactor, "iconDistanceFactor");
			ar.closeBlock();
		}
	}

	bool hasDowngrade() const { 
		return downgrade() != UNIT_ATTRIBUTE_NONE; 
	}
	const char* buildModelName() const {
		return additionalModelsData.empty() ? modelData.modelName : additionalModelsData.front().modelName;
	}	
	terUnitAttributeID downgrade() const {
		return !Downgrades.empty() ? Downgrades.front() : UNIT_ATTRIBUTE_NONE; 
	}
};

class terBuilding : public terUnitReal
{
public:
	terBuilding(const UnitTemplate& data);
	virtual ~terBuilding();

	const AttributeBuilding& attr() const {
		return safe_cast_ref<const AttributeBuilding&>(terUnitReal::attr());
	}

	void Start();
	void Kill();

	void AvatarQuant();
	void AvatarInterpolation();
	void MoveQuant(){ weaponQuant(); updateIncludingCluster(); }
	void Quant();

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* parser);

	virtual void checkConnection() {}

	void setPose(const Se3f& pose, bool initPose);
	void WayPointStart();

	void explode();

	void executeCommand(const UnitCommand& command);

	int repairRequest() const;

	void constructionStart();

	terBuilding* upgrade();
	terBuilding* downgrade();
	void uninstall();

	void ChangeUnitOwner(terPlayer* player);

	float upgradePhase() const;

	void MapUpdateHit(float x0,float y0,float x1,float y1);

	int buildingStatus() const { return buildingStatus_; }
	void setBuildingStatus(int st){ buildingStatus_ = st; }
	
	int isBuildingPowerOn(){ return buildingStatus() & BUILDING_STATUS_POWERED; }
	int isSingleSelection(){ return 0; }
	int CanBuildingEnable();
	bool isBuildingEnable() const;
	bool readyToConnect() const { return isBuildingEnable(); }

	bool isUpgrading() const { return buildingStatus() & BUILDING_STATUS_UPGRADING; }
	bool isConstructed() const { return buildingStatus() & BUILDING_STATUS_CONSTRUCTED; }
	bool isConnected() const { return buildingStatus() & BUILDING_STATUS_CONNECTED; }

	int isPluggedIn();
	
	int GetProductionProgress();

	virtual bool BuildingEnableRequest(){ return true; }

	void setRealModel(int modelIndex, float scale);
	virtual void UpdateAvatar();

	void CreateHologram();

	virtual bool canUpgrade() const;
	virtual bool canStartField() const { return 0; }

	void showDebugInfo();

	virtual int damageMoleculaRepair(int element_count = 1);

	void ShowInfo();

	bool needBuildingRepair() const { return true; }

	void setCanselConstructionTime(int time);

	bool effectsEnabled() const { return isConstructed(); }

	void setActivity(bool activate) { visible_ = activate;	}
	bool activity() const { return visible_; } 
	bool isUnseen() const { return !visible_; }

protected:
	/// Возвращает ID желаемой цепочки анимации.
	ChainID chainRequest() const;
	bool needWeaponDisable() const { return (!isConnected() || !(buildingStatus() & BUILDING_STATUS_POWERED) || terUnitReal::needWeaponDisable()); }

private:
	int buildingStatus_;
	DelayTimer cancelConstructionTimer_;

	AverageConditionTimer changePlayerTimer_;

	terIconBuilding connection_icon_;
	terIconBuilding energy_icon_;

	int cancelConstructionTime_;

	bool visible_;
};

class terBuildingPowered : public terBuilding
{
public: 
	terBuildingPowered(const UnitTemplate& data) : terBuilding(data) {}
	void checkConnection();
	int repairRequest() const;
};

/// "труп" здания
class terFallStructure : public terUnitCorpse
{
public:
	terFallStructure(const UnitTemplate& data);

	void Quant();
	void AvatarQuant();

	void setParent(terUnitBase* p);

private:

	bool soundFlag_;

	int fallDelay_;
	float fallSpeed_;
	float fallAcceleration_;
};

class terBuildingHologram : public terUnitBase
{
public:
	terBuildingHologram(const UnitTemplate& data);

	void Start();
	void Quant();
	void DestroyLink();
	void AvatarQuant();

	void SetBuildingPoint(terBuilding* p){ MasterPoint = p; }

private:

	terBuilding* MasterPoint;

	float ConstructionScale;
};

class terBuildingUninstall : public terUnitBase
{
public:
	terBuildingUninstall(const UnitTemplate& data);

	void Start();
	void Quant();
	void setParent(terBuilding* p);

	void setLifeTime(int time){ lifeTimer_.start(time); }

private:

	float sight_;
	float hologram_;
	DurationTimer lifeTimer_;
};

class terBuildingEnvironment : public terBuildingPowered
{
public:
	terBuildingEnvironment(const UnitTemplate& data);

	ChainID chainRequest() const { return CHAIN_NONE; }
};

#endif //__IRONBUILDING_H__