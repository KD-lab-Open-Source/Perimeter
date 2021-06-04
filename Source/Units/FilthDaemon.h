#pragma once

struct terFilthDaemon:terFilthGeneric
{
	terFilthDaemon(const UnitTemplate& data);
	~terFilthDaemon();
	void setStartPhase(float start_phase_){start_phase=start_phase_;};
	void setDirection(Vect3f direction_){direction=direction_;}
	void Start();
	void setFieldDamage(const DamageData& damage);

	void AvatarQuant();
	void AvatarInterpolation();
	void MoveQuant();
	void Quant();
	void Collision(terUnitBase* p);

	bool isOnZeroplast(){return on_zeroplast;}
	void SetFreeDestroy();
	void SetBerserkMode();
	bool is_berserk(){return on_zeroplast && berserk_mode;}
	bool isDestroyed(){return free_destroy;}
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
public:
	bool free_destroy;
	bool on_zeroplast;
	bool berserk_mode;
	bool go_up;
	float start_phase;
	float mul_speed;
	Vect3f direction;

	demonToolzer* geo_effect;

	terAnimationPhaseIteratorType obj_sight;

	struct OneShadow
	{
		cObjectNodeRoot* model;
	};

	list<OneShadow> shadows;

	struct ShadowInterpolate
	{
		Se3f pos;
		float phase;
	};
	list<ShadowInterpolate> object_pos;

	void addShadow(float sight_speed);
	SND3DSound sound;
};


struct terFilthSwarmDaemon: terFilthSwarm
{
	terFilthSwarmDaemon(terFilthSpot* spot,const Vect3f& pos,int attack_period);
	~terFilthSwarmDaemon();
	void Quant();
	void DestroyLink();
	void SetCreatureGeneration(int creature_num,int generation_period);
	terUnitAttributeID GetUnitID();
	void SetFreeDestroy();
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	void Generate();

	bool must_init_pos;
	vector<Vect3f> init_pos;

	typedef list<terFilthDaemon*> FilthListType;
	FilthListType unit_list;
	Vect3f attack_pos;

	int attack_period;

	bool berserk_mode;
	int berserk_time;

	int GenerationCount;
	float GenerationFactor;
	float GenerationSpeed;

	Vect3f pin;
};
