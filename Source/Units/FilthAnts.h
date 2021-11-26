#ifndef _FILTH_ANTS_
#define _FILTH_ANTS_

class terFilthAnt : public terFilthGeneric
{
public:
	terFilthAnt(const UnitTemplate& data);
	~terFilthAnt();

	void WayPointStart();
	void WayPointController();
	void AvatarQuant();

	void SetFilthTarget(Vect3f& v){ TargetPosition = v; };
	void SetFilthScale(float scale){ SpeedScale = scale; };
	void SetFilthParameter(int attack_period);

	bool isRun()const {return !BirthProcessPoint && !DestroyProcessPoint;}
	void SetFreeDestroy(){free_destroy=true;};
	void setFieldDamage(const DamageData& damage);

	void setInitialGeoprocess(bool b){initial_geoprocess=b;}
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(SaveUnitData* baseData);
protected:
	class c3DSGeoAction* BirthProcessPoint;
	class c3DSGeoAction* DestroyProcessPoint;

	Vect3f TargetPosition;
	float SpeedScale;
	float SpeedFactor;

	bool free_destroy;
	bool death_requested;
	bool initial_geoprocess;
};

//----------------------------------

class terFilthSwarmAnt : public terFilthSwarm
{
public:
	terFilthSwarmAnt();

	terFilthSwarmAnt(terFilthSpot* spot,const Vect3f& pos,int attack_period);
	~terFilthSwarmAnt();
	void Quant();
	void DestroyLink();

	void AddFilthPoint(terFilthAnt* p);
	void GenerationProcess();

	float GetFollow(Vect3f& v1,Vect3f& v2,float a,Vect3f& result);
	void SetCreatureGeneration(int creature_num,int generation_period);
	terUnitAttributeID GetUnitID();
	void SetFreeDestroy();
	virtual SaveUnitData* universalSave(SaveUnitData* baseData);
	virtual void universalLoad(SaveUnitData* baseData);
protected:
	typedef std::list<terFilthAnt*> FilthListType;
	FilthListType unitList;

	terGenerate gen;

	float DeltaAngle;
	float ChangeAngleCount;

	int TargetCount;
	Vect3f TargetPosition;

	int attack_period;
	struct terFilthAntsStruct* prm;

	SND3DSound sound;
};

#endif