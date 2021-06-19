#pragma once

class terFilthWorm : public terFilthGeneric
{
public:
	terFilthWorm(const UnitTemplate& data);
	~terFilthWorm();

	void Start();
	void AvatarQuant();
	void Quant();
	void MoveQuant();

	void Collision(terUnitBase* p);

	const Vect3f& GetEndPos()const{return end_tail->GetGlobalPosition().trans();};

	void EndCollision(terUnitBase* p);
	void SetFreeDestroy();

	bool isAttack() const {return is_attack;};
	void StartAttack();

	void ChangePos(Vect3f pos);
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	void setAttack(bool a);
	cLogicTile* end_tail;
	CWormOut* pWormOut;
	bool is_death;
	bool is_attack;
	void CalcLogic();
	void StartDeath();

	SND3DSound sound;
	SND3DSound attack_sound;
};

class terFilthSwarmWorm : public terFilthSwarm
{
public:
	terFilthSwarmWorm(terFilthSpot* spot,const Vect3f& pos,int attack_period);
	~terFilthSwarmWorm();
	void Quant();
	void DestroyLink();

	virtual void SetCreatureGeneration(int creature_num,int generation_period){};
	virtual terUnitAttributeID GetUnitID();

	bool isMoveToChaos();
	void SetFreeDestroy();
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	void Move();
	int attack_period;
	CGeoWorm* pWorm;
	terFilthWorm* pWormModel;

	std::list<terUnitBase*> exclude_list;

	int change_angle;
	float angle,speed;
	bool first;

	bool first_create;
//	DamageMolecula molecula;
};
