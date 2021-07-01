#pragma once

class terFilthShark : public terFilthSpline
{
public:
	terFilthShark(const UnitTemplate& data);
	~terFilthShark();

	void SetFreeDestroy(){};
	void addWayPoint();
	void SetTargetDirection(Vect3f pin);

	void Quant();
	void MustDie();
	void MustDieFree();
	void SetAttackPeriod(int attack_period_){attack_period=attack_period_;}
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	void EffectRun();
	Vect3f target;
	Vect3f pin;
	bool is_hmax;
	struct sGeoWave* GhostHolePoint;
	bool last_effect_up;
	bool is_die;
	bool is_die_free;
	int attack_period;
	DurationTimer timer_die_free;

	bool TestUp(const Vect3f& pos,bool last_up);
};

class terFilthSwarmShark : public terFilthSwarm
{
public:
	terFilthSwarmShark(terFilthSpot* spot, const Vect3f& pos,int attack_period);
	~terFilthSwarmShark();
	void Quant();
	void DestroyLink();

	void GenerationProcess();

	void SetCreatureGeneration(int creature_num,int generation_period);
	terUnitAttributeID GetUnitID() {return UNIT_ATTRIBUTE_FILTH_SHARK;}
	void SetFreeDestroy();
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	int attack_period;
	typedef std::vector<terFilthShark*> FilthListType;
	FilthListType units;
	terGenerate gen;
};