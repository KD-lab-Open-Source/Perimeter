#pragma once

class terFilthVolcano: public terFilthGeneric
{
public:
	terFilthVolcano(const UnitTemplate& data);
	~terFilthVolcano();

	void Collision(terUnitBase* p);
	void AvatarQuant();
	void Quant();
	void SetFreeDestroy(){}

	void setRealModel(int modelIndex, float scale);
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
	void SetPrm(struct terFilthVolcanoStruct* prm_){prm=prm_;}
protected:
	terFilthVolcanoStruct* prm;
	float model_scale;
	int begin_wait_destroy;
	float time_from_last_damage;
	SND3DSound sound;
};

class terFilthSwarmVolcano:public terFilthSwarm
{
public:
	terFilthSwarmVolcano(terFilthSpot* spot,const Vect3f& pos,bool first);
	~terFilthSwarmVolcano();

	void Quant();

	terUnitAttributeID GetUnitID();
	void SetCreatureGeneration(int creature_num,int generation_period);

	void SetFreeDestroy();
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);

	void DestroyLink();

	void SetPrm(struct terFilthVolcanoStruct* prm_);
protected:
	terFilthVolcanoStruct* prm;
	int generation_period;
	int creature_num;

	typedef std::vector<terFilthVolcano*> FilthListType;
	FilthListType unitList;

	//class CGeoInfluence* BirthProcessPoint;
	struct sTVolcano* BirthProcessPoint;
	cEffect* effect;
	cEffect* lava;
	InterpolatorPose effect_pos;
	bool first;

	void GenerationProcess();
};