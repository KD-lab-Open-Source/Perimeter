#ifndef __MUTATION_H__
#define __MUTATION_H__

class RigidBody;

class MutationProcess
{
	int phase;
	time_type phase_time;
	DurationTimer logic_timer;
	Vect3f center;
	int forward;
	int sound_started;
	bool started; // logic time

	typedef std::vector<RigidBody*> BodyList;
	BodyList source;
	BodyList target;
	MTSection lock_;

	struct Sphere 
	{
		cObjectNodeRoot* geometry;
		float radius0, radius;
		float scale1;
		Vect3f position, displacement;
		MatXf pose;
	
		Sphere();
		~Sphere();
		void create(float volume, const Vect3f& position, bool placeOnTheGround);
		void set(float ts, float tp);
		void setTarget(const Vect3f& target, float distance = 0);
		void move(const Vect3f& dr);
		float calcZ();
	};

	typedef std::list<Sphere> SphereList;
	SphereList spheres;

	Sphere main_sphere;

	void compact();
	
public:
	MutationProcess(){ started = false; }
	~MutationProcess(){ clear(); }
	void clear();
	
	void addSource(RigidBody* body){ xassert(lock_.is_lock()); source.push_back(body); }
	void addTarget(RigidBody* body){ xassert(lock_.is_lock()); target.push_back(body); }
	void removeSourceOrTarget(RigidBody* body);
	MTSection* getLock(){return &lock_;}

	void start(const Vect3f& position);
	void show_quant();
	void logic_quant();

	bool finished() const { return !started; }
};

#endif //__MUTATION_H__