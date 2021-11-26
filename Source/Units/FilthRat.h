#ifndef _FILTH_RATS_
#define _FILTH_RATS_

class terFilthRat : public terFilthGeneric
{
public:
	terFilthRat(const UnitTemplate& data);
	~terFilthRat();

	void WayPointStart();
	void WayPointController();
	void AvatarQuant();
	void Quant();

//	void Collision(terUnitBase* p);
	void SetFilthTarget(Vect3f& v){ TargetPosition = v; };
	void SetFilthScale(float scale){ SpeedScale = scale; };
	void SetFreeDestroy();

	void SetAttack(terUnitGeneric* enemy_unit);

	void setPose(const Se3f& poseIn, bool initPose);
	void setFieldDamage(const DamageData& damage);
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(SaveUnitData* baseData);
protected:
	enum terFilthRatStatusType
	{
		FILTH_RAT_STATUS_STOP,
		FILTH_RAT_STATUS_RUN,
		FILTH_RAT_STATUS_DEATH
	};
	terFilthRatStatusType FilthStatus;

	Vect3f TargetPosition;
	float SpeedScale;
	float SpeedFactor;

	bool free_destroy;
	DurationTimer zeroPositionTimer_;

	bool first_z_negate;
	float last_x_angle;
	float intrerpolate_x_angle;

	class c3DSGeoAction* DeathProcessPoint;

	void StartDeath();
};

//----------------------------------

class terFilthSwarmRat : public terFilthSwarm
{
public:
	terFilthSwarmRat(terFilthSpot* spot,const Vect3f& pos,int attack_period);
	~terFilthSwarmRat();
	void Quant();
	void DestroyLink();

	void AddFilthPoint(terFilthRat* p);
	void GenerationProcess();

	float GetFollow(Vect3f& v1,Vect3f& v2,float a,Vect3f& result);
	void SetCreatureGeneration(int creature_num,int generation_period);
	terUnitAttributeID GetUnitID();
	void SetFreeDestroy();
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(SaveUnitData* baseData);
protected:
	struct s_WaspBirthGeoAction* BirthProcessPoint;

	typedef std::vector<terFilthRat*> FilthListType;
	FilthListType unitList;

	terGenerate gen;

	float DeltaAngle;
	float ChangeAngleCount;

	int TargetCount;
	Vect3f TargetPosition;

	int attack_period;
	SND3DSound sound;
};

#endif