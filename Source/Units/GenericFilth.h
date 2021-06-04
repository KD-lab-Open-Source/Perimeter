#pragma once
#include <set>

struct terFilthGeneric : terUnitReal
{
	terFilthGeneric(const UnitTemplate& data);
	void Quant();
	void AvatarQuant();
	void Collision(terUnitBase* p);

	bool clusterColliding();

	void SetAttackPeriod(int attack_period);
	virtual void SetFreeDestroy()=0;
	void UnitDamageCheck(){}

	bool IsAttackable(terUnitBase* unit){return unit->unitClass() & attr().AttackClass;}
	virtual void FieldEffect();
protected:

	bool is_fitrst_quant;
	bool cluster_colliding;
	int cur_cluster;

	int attack_period;
};

struct terSplineController
{
	terSplineController();
	void setPointTime(float point_time){delta_time=0.1f/point_time;};
	float getPointTime(){return 0.1f/delta_time;}
	float getPointSpeed(){return delta_time*10.0f;}

	void setUp(const Vect3f& up)
	{
		auto_up=false;
		user_up=Normalize(up);
	}
	void setFront(const Vect3f& front)
	{
		auto_front=false;
		user_front=Normalize(front);
	}

	Vect3f getLastDirection2();

	void GetSplineParameter(Vect3f p[4]);
	float GetSplinePhase()const{return cur_way_point;}

	void SetLinearInterpolation(bool b){interpolation_linear=true;}

	Se3f quantPosition(float dt);

	//Для нормальной работы way_points должно быть не меньше 4.
	//Удаление точек  нежелательно, так как приведёт к изменению скорости либо положения
	typedef list<Vect3f> WayPoints;
	WayPoints way_points;
protected:
	virtual const Se3f& GetPose()const=0;
	float delta_time;
	float cur_way_point;
	Vect3f acceleration;
	float k_acceleration;

	float damphing;
	bool auto_up,auto_front;
	Vect3f user_up,user_front;
	bool interpolation_linear;
};

struct terFilthSpline: terFilthGeneric,terSplineController
{
	terFilthSpline(const UnitTemplate& data);

	void AvatarQuant();
	void MoveQuant();
	SaveUnitData* universalSave(SaveUnitData* baseData);
	void universalLoad(const SaveUnitData* baseData);
protected:
	virtual const Se3f& GetPose()const {return pose();}
	virtual void addWayPoint()=0;
};

struct terFilthSpot;

struct terFilthSwarm
{
	terFilthSwarm(terFilthSpot* spot, const Vect3f& pos);
	virtual ~terFilthSwarm() {}
	virtual void Quant(){}
	virtual void DestroyLink();

	int GetAlive(){ return Alive; };


	void playDetectedSound(bool geo_mode = false);
	void SetBaseParam(struct terFilthSwarmParameter *p){BaseParam=p;}

	void SetAttackWidth(float width,float direction){attack_width=width;attack_direction=direction;}
	virtual void SetCreatureGeneration(int creature_num,int generation_period)=0;
	void SetPlayer(terPlayer* player_);
	virtual terUnitAttributeID GetUnitID()=0;

	inline terUnitBase* GetTarget()const{return  TargetPoint;}

	const Vect3f& pos() const { return position; } 

	bool IsAttackable(terUnitBase* unit){return unit->unitClass() & attribute->AttackClass;}
	void SetAttackPlayer(terFilthAttackType a){attack_player=a;}

	void SetAttackTarget(terUnitBase* AttackUnit);
	virtual void SetFreeDestroy(){};

	virtual SaveUnitData* universalSave(SaveUnitData* baseData);
	virtual void universalLoad(const SaveUnitData* baseData);
protected:
	terPlayer* player;
	const AttributeBase* attribute;
	int Alive;
	terFilthSpot* SpotPoint;
	Vect3f position;
	terUnitBase* TargetPoint;
	DurationTimer soundTimer_;
	struct terFilthSwarmParameter *BaseParam;

	typedef set<terUnitBase*> UnitSet;
	bool find_attack_target_point;

	//Заполняет TargetPoint наиболее подходящим для атаки юнитом
	virtual void FindTargetPoint(UnitSet* exclude=NULL);
	virtual void FindComplexTarget(list<terUnitBase*>& target_list,int max_target,UnitSet* exclude);
	virtual terUnitBase* CalcDistance(terUnitBase* unit,float& dist,const Vect2f& direction);

	float attack_width;
	float attack_direction;
	terFilthAttackType attack_player;
};

struct terFilthSpotParameters;

struct terFilthSpot : terUnitBase
{
	terFilthSpot(const UnitTemplate& data);
	virtual ~terFilthSpot();

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);

	void Start();
	void MoveQuant();

	void DestroyLink();

	void StartSleepMode();
	void GenerationProcess();

	void SetFilthParamID(terFilthSpotID id);
	terFilthSpotID  GetFilthParamID() { return FilthParamID; }

	void ShowInfo();

	bool IsAttackOtherSwarm(terUnitBase* unit,terFilthSwarm* swarm);

	void setActivity(bool activate);
	bool activity() const { return !sleep_; } 

	//Устанавливает параметры.
	//Заодно (так как эта функция вызывается только оружием)
	//attack_player=FILTH_ATTACK_NOSELF и возможность генерации скверны на зеропласте
	void setParameters(const terFilthSpotParameters* prm);

	void startKillTimer(int time){ killTimer_.start(time); }

	void scaleTime(float coeff)
	{ 
		first_sleep_time *= coeff;
		sleep_period *= coeff;
		delta_sleep_period *= coeff;
		attack_period *= coeff;
		delta_attack_period *= coeff;
	}

	const Vect3f& getFirstSwarmPos() const {
		return swarm_list.size() ? swarm_list.front()->pos() : Vect3f::ZERO;
	} 

	bool isSwarm() const {
		return !swarm_list.empty();
	} 

	struct HoleStruct
	{
		Vect2f pos;
		bool inited;
	};

	//Не выбирать цель, атаковать только этого юнита.
	//после его смерти включится поиск цели.
	void SetAttackTarget(terUnitBase* AttackUnit){pExclusiveAttackUnit=AttackUnit;};
	bool terCheckFilthPoint(int x,int y);

	void SetFreeDestroy();
	void KillAfterGenerate();
	bool initialGeoprocess() const {return initial_geoprocess;}
	void Kill();

	vector<HoleStruct>& GetHolePos() {return hole_pos;};
protected:
	void SoundActivate();
	void KillOfEnd();

	DurationTimer sleep_timer;

	struct terFilthSwarmParameter *BaseParam;

	terFilthSpotID FilthParamID;

	typedef list<terFilthSwarm*> FilthSwarmListType;
	FilthSwarmListType swarm_list;

	int SoundImpulse;
	float attack_width;
	float attack_direction;

	int creature_num,creature_generation_period;

	bool sleep_;
	float first_sleep_time;
	float sleep_period;
	float delta_sleep_period;
	float attack_period;
	float delta_attack_period;

	vector<HoleStruct> hole_pos;

	DurationTimer killTimer_;

	terUnitAttributeID activatingUnit_; 
	float activatingDistance_; 
	bool create_first;
	terFilthAttackType attack_player;
	bool initial_geoprocess;
	bool harkback;//Это скверна возвратников.
	bool kill_of_end;//Удалить спот, когда кночилась скверна
	bool kill_after_generate;//Подождать генерации скверны, подождать уничтожения скверны, удалить спот.

	void GetDifficity(int& creature_num,float& sleep_mul);
	int GetDifficityCreatureNum();
	float GetDifficitySleepMul();

	terUnitBase* pExclusiveAttackUnit;
	bool enable_zeroplast_generate;
};

bool terCheckFilthZero(int x,int y);
bool terCheckFilthChaos(int x,int y);

bool terCheckFilthZero(const Vect3f& pos);
bool terCheckFilthChaos(const Vect3f& pos);


//Равномерно распределяет на периодgeneration_period 
class terGenerate
{
public:
	terGenerate(){GenerationCount=GenerationSpeed=GenerationFactor=0;}
	~terGenerate(){};

	void Set(int creature_num,int generation_period);

	bool InProcess(){return GenerationCount>0;};
	int Quant();//Возвращает количество юнитов, которые создаются в этот кадр

	void Save(SaveTerGenerate& data);
	void Load(const SaveTerGenerate& data);
protected:
	int GenerationCount;
	float GenerationFactor;
	float GenerationSpeed;
};

template <class FilthListType>
void DestroyLinkList(FilthListType& unitList)
{
	FilthListType::iterator i_unit;
	i_unit = unitList.begin();
	while(i_unit != unitList.end()){
		if((*i_unit)->alive())
			++i_unit;
		else{
			i_unit = unitList.erase(i_unit);
		}
	}
}
