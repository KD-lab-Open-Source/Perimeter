#ifndef _FILTH_WASP_
#define _FILTH_WASP_
struct terFilthSwarmWasp;

struct terFilthWasp : terFilthSpline
{
	terFilthWasp(const UnitTemplate& data);
	void AvatarQuant();
	void Quant();
	void Start();

	void SetFilthTarget(const Vect3f& v,float radius){ target_position = v; target_radius=radius;};
	void SetFilthPos(const Vect3f& v){ center_position = v; };
	void SetFreeDestroy();

	float GetLastZ();
	void SetSummaryZ(float z){summary_z=z;}
	void SetSwarmHeight(float swarm_height_){swarm_height=swarm_height_;}
	void SetAttackMode(bool attack_){attack=attack_;};
	bool GetAttackMode(){return attack;}

	void Collision(terUnitBase* p);
	void SetSwarm(terFilthSwarmWasp* swarm_){swarm=swarm_;}
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	terFilthSwarmWasp* swarm;
	bool free_destroy;
	Vect3f target_position;
	Vect3f center_position;
	Vect3f delta_center;
	bool begin_move;
	float summary_z;
	float swarm_height;
	bool attack;
	float target_radius;
	bool no_add_point;

	void addWayPoint();
};

//----------------------------------

struct terFilthSwarmWasp : terFilthSwarm
{
	terFilthSwarmWasp();

	terFilthSwarmWasp(terFilthSpot* spot, const Vect3f& pos,int attack_period);
	~terFilthSwarmWasp();
	void SetHole(vector<terFilthSpot::HoleStruct>& hole_position_);

	void Quant();
	void DestroyLink();
	void SetCreatureGeneration(int creature_num,int generation_period);
	terUnitAttributeID GetUnitID() {return unit_id;}
	void SetPrm(terUnitAttributeID id_);
	void SetFreeDestroy();

	struct terFilthWaspStruct* prm;
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	terUnitAttributeID unit_id;
	vector<struct s_WaspBirthGeoAction*> wasp_hole_point;
	vector<Vect2f> hole_position;

	typedef list<terFilthWasp*> FilthListType;
	FilthListType unit_list;

	terGenerate gen;

	int attack_period;
	Vect3f TargetPosition;

	float attack_count;

	void GenerationProcess();
	SND3DSound sound;
};

#endif //_FILTH_WASP_
