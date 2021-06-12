#ifndef _FILTH_CROW_
#define _FILTH_CROW_

struct terFilthCrow : terFilthSpline
{
	terFilthCrow(const UnitTemplate& data);
	~terFilthCrow();
	void SetFilthTarget(Vect3f& v){target_position=v;};
	void SetDeltaTarget(Vect3f& v){delta_target=v;};

	void AvatarQuant();
	void Quant();
	void Start();

	terUnitBase* GetTarget(){return TargetPoint;}
	void SetTarget(terUnitBase*  p){TargetPoint=p;}
	void DestroyLink();
	void Collision(terUnitBase* p);

	void SetFreeDestroy(){free_destroy=true;};
	bool GetFreeDestroy(){return free_destroy;}
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	terUnitBase* TargetPoint;
	bool free_destroy;

	Vect3f target(){return target_position+delta_target;}
	Vect3f target_position;
	Vect3f delta_target;
	bool go_out;
	bool attack_mode;
	bool must_die;
	bool death_request;

	void addWayPoint();
	void StartDeath(bool immediate);
	void AttackMode();
	SND3DSound attack_sound;
};

struct terFilthSwarmCrow : terFilthSwarm
{
	terFilthSwarmCrow(terFilthSpot* spot,const Vect3f& pos,int attack_period);
	~terFilthSwarmCrow();
	void Quant();
	void DestroyLink();
	void SetCreatureGeneration(int creature_num,int generation_period);
	terUnitAttributeID GetUnitID();
	void SetFreeDestroy();
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	void Generate();
	typedef list<terFilthCrow*> FilthListType;
	FilthListType unit_list;

	int generate_creature_num;
	Vect3f attack_pos;

	int attack_period;
	SND3DSound sound;
};

#endif //_FILTH_CROW_
