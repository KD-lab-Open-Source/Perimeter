#ifndef _FILTH_EYE_
#define _FILTH_EYE_

struct terFilthEye : terFilthGeneric
{
	terFilthEye(const UnitTemplate& data);
	~terFilthEye();

	void WayPointStart();
	void WayPointController();
	void AvatarQuant();
	void Quant();

	void SetFilthAttack(bool attack_){ request_attack = attack_; }
	void SetFreeDestroy(){};
	void SetDestroy();
	void SetFilthTarget(Vect3f& v);
	void SetFilthAttackPosition(Vect3f& v){ attack_position= v; }
	void SetFilthHeight(float h){ height_terra=h;}
	float GetFilthHeight(){ return height_terra;}
	void SetFilthSpeedAngle(float f){speed_angle_z=f;}
	void SetFilthTop(bool b);

	void Collision(terUnitBase* p);
	void AvatarInterpolation();
	bool IsAttack(){return request_attack && last_attack;}

	void setPose(const Se3f& poseIn, bool initPose);

	bool IsEnd(){return set_way_point && BodyPoint->way_points.empty();};

	bool is_point_reached(const Vect3f& point);
	void MoveQuant();
protected:
	void RequestAttackQuant();

	Vect3f target_position;
	Vect3f attack_position;

	bool destruction;
	bool pre_destruction;
	int pre_destruction_time;
	float height_terra;

	terAnimationPhaseIteratorType ObjectAnimation;
	terAnimationPhaseIteratorType DestroyAnimation;
	terAnimationPhaseIteratorType BeginAnimation;
	terAnimationPhaseIteratorType TopAnimation;
	Se3f begin_destruct_pos;
	Vect3f begin_destruct_terra;

	bool last_attack,request_attack;
	terAnimationPhaseIteratorType AttackAnimation;

	bool destroy_flag;
	DurationTimer zeroPositionTimer_;

	bool set_way_point;
	cObjectNode* sphere_node;

	sTBubble* bubble;
	float angle_z,speed_angle_z;
	InterpolatorAngle interpolate_angle_z;

	bool is_top;
};

//----------------------------------

struct terFilthSwarmEye : terFilthSwarm
{
	terFilthSwarmEye(terFilthSpot* spot, const Vect3f& pos,int attack_period);
	~terFilthSwarmEye();
	void Quant();
	void DestroyLink();

	void SetCreatureGeneration(int creature_num,int generation_period);
	terUnitAttributeID GetUnitID() {return UNIT_ATTRIBUTE_FILTH_EYE;}
protected:
	void SnakeMove();
	void GenerationProcess();

	Vect3f center_pos;

	typedef list<terFilthEye*> FilthListType;
	FilthListType unitList;

	terGenerate gen;

	int attack_period;

	Vect3f TargetPosition;
	int TargetCount;
	bool destruction;
};

#endif
