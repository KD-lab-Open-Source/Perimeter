struct terFilthSwarmDragon;
struct terFilthDragonStruct;

struct terFilthDragon : terFilthGeneric
{
	terFilthDragon* NextPoint;

	terFilthDragon(const UnitTemplate& data);
	~terFilthDragon();

	void setPhase(float phase);

	void WayPointStart();

	void MoveQuant();
	void LinkingBody();

	void DestroyLink();
	void AvatarQuant();

	void SetNextPoint(terFilthDragon* next){ NextPoint = next; };

	void RefreshAttribute();

	void SetFreeDestroy(){}

	void DockNow();

	void Collision(terUnitBase* p);
	void MustDie();
	void MustDieFree(bool must);

	void Quant();
	void SetScale(float r);

	void SetSwarm(terFilthSwarmDragon* swarm_);
	float GetSight(){return timerDieFree.phase();}
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	terFilthSwarmDragon* swarm;
	terAnimationPhaseIteratorType ObjectAnimation;
	craterToolzerDestroyZP* crater;
	float scale;
	float crater_run;
	bool is_die;
	bool is_die_free;
	terAnimationPhaseIteratorType timerDieFree;
};

struct terFilthDragonBody : terFilthDragon
{
	terFilthDragonBody(const UnitTemplate& data);
	bool selectAble() const { return false; }
	void RefreshAttribute();
};

struct terFilthDragonHead : terFilthSpline
{
	terFilthDragon* NextPoint;

	terFilthDragonHead(const UnitTemplate& data);
	~terFilthDragonHead();
	void MoveQuant();

	void WayPointController();

	void SetFilthTarget(Vect3f& v){ TargetPosition = v;};
	void RefreshAttribute();
	void LinkingBody();

	void SetFreeDestroy();
	void addWayPoint();

	void SetSwarm(terFilthSwarmDragon* swarm_);
	void Quant();
	void MustDie();
	void MustDieFree(bool must);

	void Kill();
	float GetSight(){return timerDieFree.phase();}
	void Collision(terUnitBase* p);

	void checkField();
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	bool IsEndInvisibly();
	terAnimationPhaseIteratorType ObjectAnimation;
	terAnimationPhaseIteratorType timerDieFree;
	bool attack;
	float Height;
	bool height_flag;
	Vect3f TargetPosition;
	bool is_die;
	bool is_die_free;

	cObjectNode* fire_node;

	terFilthSwarmDragon* swarm;
	void CalcHeight();
	SND3DSound sound;
	SND3DSound attack_sound;
};

struct terFilthSwarmDragon : terFilthSwarm
{
	terFilthSwarmDragon(terFilthSpot* spot, const Vect3f& pos,int attack_period);
	~terFilthSwarmDragon();
	void Quant();
	void DestroyLink();

	void GenerationProcess();

	void SetCreatureGeneration(int creature_num,int generation_period){};
	terUnitAttributeID GetUnitID() {return UNIT_ATTRIBUTE_FILTH_DRAGON_HEAD;}

	terUnitBase* GetNextTarget();
	void SetFreeDestroy();

	void SetPrm(terFilthDragonStruct* prm_){prm=prm_;};
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	terFilthDragonHead* HeadPoint;
	struct s_WaspBirthGeoAction* BirthProcessPoint;
	bool use_geo;

	int AttackCount;
	Vect3f TargetPosition;
	int TargetCount;

	Vect3f distance_direction;
	terUnitBase* CalcDistance(terUnitBase* unit,float& dist,const Vect2f& direction);
	void FindTargetPoint(UnitSet* exclude=NULL);
	terFilthDragonStruct* prm;
	friend terFilthDragonHead;
	friend terFilthDragon;
	friend terFilthDragonBody;
};
