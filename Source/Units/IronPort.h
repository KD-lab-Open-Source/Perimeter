#include "IronBuilding.h"
#include "Nature.h"

class AttributeCorridor : public AttributeBuilding
{
public:
	int disappearingTime;
	int disappearingHeight;
	float distanceThreshould;
	bool moveFrame;

	float potentialToAcceleration;
	float accelerationDenominator;
	float pathVisualizationDelta;

	AttributeCorridor();

	template<class Archive>
	void serialize(Archive& ar) {
		__super::serialize(ar);
		if(ar.openBlock("attributeCorridor", "Коридор")){
			ar & TRANSLATE_OBJECT(disappearingTime, "disappearingTime");
			ar & TRANSLATE_OBJECT(disappearingHeight, "disappearingHeight");
			ar & TRANSLATE_OBJECT(distanceThreshould, "distanceThreshould");
			ar & TRANSLATE_OBJECT(moveFrame, "moveFrame");
			ar & TRANSLATE_OBJECT(potentialToAcceleration, "potentialToAcceleration");
			ar & TRANSLATE_OBJECT(accelerationDenominator, "accelerationDenominator");
			ar & TRANSLATE_OBJECT(pathVisualizationDelta, "pathVisualizationDelta");
			ar.closeBlock();
		}
	}
};

class terCorridor : public terBuildingPowered
{
public:
	terCorridor(const UnitTemplate& data);

	const AttributeCorridor& attr() const {
		return safe_cast_ref<const AttributeCorridor&>(terUnitReal::attr());
	}

	virtual bool readyToTeleportate() const = 0;
	virtual void input(terFrame* frame);

	void DestroyLink();
	void Quant();

	void setDamage(const DamageData& damage,terUnitBase* p) {}

	ChainID chainRequest() const { return CHAIN_NONE; }

protected:
	terFrame* frame_;

	enum Mode {
			IDLE,
			OPENING,
			FRAME_APPROACHING,
			FRAME_DISAPPEARING
	};
	Mode mode_;
	InterpolationTimer disappearingTimer_;
	float PositionZ_;
};

class terCorridorAlpha : public terCorridor
{
public:
	terCorridorAlpha(const UnitTemplate& data);
	
	void Start();
	void makeFree();
	
	void setPose(const Se3f& pose, bool initPose);

	bool isBuilding() const { return !free_; }
	bool readyToTeleportate() const { return free_; }

	void MoveQuant();
	void Quant();
	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);
	void checkConnection();

	void ShowInfo();
	void showDebugInfo();

	void putPath(list<Vect2f>& miniMapPath);

private:
	bool free_;
	Vect2fVect potentials_;
	MTSection potentials_lock_;
	int passTime_;
	int timeOffset_;
	MeasurementTimer timer_;
	bool first_quant_;

	const Vect2f& get(int i) const { xassert(potentials_lock_.is_lock());return potentials_[(i + potentials_.size()) % potentials_.size()]; }
	Vect2f potentialPosition();
	Vect2f potentialPosition(float t);
	void refreshPotentials();
};

class terCorridorOmega : public terCorridor
{
public:
	terCorridorOmega(const UnitTemplate& data);

	bool canUpgrade() const { return true; }
	int CanBuildingEnable(){ return !upgraded_ && isBuildingEnable(); }
	bool readyToTeleportate() const;

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);
	
	void checkConnection();

	void ShowInfo();

	bool upgraded() const { return upgraded_ && isConstructed(); }
	bool mined() const;

private:
	bool upgraded_;
	terIconBuilding minedIcon_;
};


class terUnitAplhaPotential : public terUnitBase
{
public:
	terUnitAplhaPotential(const UnitTemplate& data);

	void ShowInfo();
	void setIndex(int index) { index_ = index; }

private:
	int index_;
	static int indexLast_;
};