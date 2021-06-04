#ifndef _FILTH_GHOST_
#define _FILTH_GHOST_

struct terFilthGhost : terFilthGeneric
{
	terFilthGhost(const UnitTemplate& data);
	~terFilthGhost();

	void setPose(const Se3f& poseIn, bool initPose);

	void WayPointStart();
	void WayPointController();
	void AvatarQuant();

	void SetFilthAttack(bool attack_);
	void SetFreeDestroy();
	void SetFilthTarget(Vect3f& v){ target_position = v; }
	void SetFilthAttackPosition(Vect3f& v){ attack_position= v; }

	void Quant();

	void Collision(terUnitBase* p);
	void AvatarInterpolation();
	bool IsAttack(){return request_attack && last_attack;}

	void Attack(terUnitBase* TargetPoint);

	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	cLine3d* laser;
	void RequestAttackQuant();

	Vect3f target_position;
	Vect3f attack_position;

	bool destruction;

	terAnimationPhaseIteratorType ObjectAnimation;
	terAnimationPhaseIteratorType DestroyAnimation;
	terAnimationPhaseIteratorType BeginAnimation;

	bool last_attack,request_attack;
	terAnimationPhaseIteratorType AttackAnimation;

	bool destroy_flag;

	DurationTimer zeroPositionTimer_;
	class ghostToolzer* GhostHolePoint;
	float last_deltaz;
};

//----------------------------------

struct terFilthSwarmGhost : terFilthSwarm
{
	terFilthSwarmGhost(terFilthSpot* spot, const Vect3f& pos,int attack_period);
	~terFilthSwarmGhost();
	void Quant();
	void DestroyLink();
	void SetCreatureGeneration(int creature_num,int generation_period);
	terUnitAttributeID GetUnitID() {return UNIT_ATTRIBUTE_FILTH_GHOST;}
	void SetFreeDestroy();

	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	void SnakeMove();
	void GenerationProcess();

	float angle_z;

	typedef list<terFilthGhost*> FilthListType;
	FilthListType unitList;

	terGenerate gen;

	int attack_period;

	Vect3f TargetPosition;
	int TargetCount;
	SND3DSound sound;
	SND3DSound sound_attack;
};

#endif
