#ifndef __IRONCLUSTERUNIT_H__
#define __IRONCLUSTERUNIT_H__

#include "EnergyConsumer.h"
#include "Interpolation.h"

enum terMonkMode
{
	MONK_MODE_SLEEP = 0,
	MONK_MODE_TO_BORDER,
	MONK_MODE_GUARD,
	MONK_MODE_TO_CORE
};

class terUnitMonk : public GridElementType
{
	friend class MonkManager;
	list<terUnitMonk*>::iterator self_it;
	terPlayer* Player;
	terInterpolationMonk avatar;
	Vect3f position_;
	bool alive_;

public:
	terUnitMonk(const UnitTemplate& data);
	~terUnitMonk();

	void Start();
	void avatarInterpolation();
	void quant();

	void setPath(const Vect2sVect& path, float pathPosition);
	void setPathPosition(float pathPosition);
	int goHome();

	terMonkMode monkMode() const { return monk_mode_; };

	float pathPosition() const { return pathPosition_; }

	void CalcBorderPosition();
	void Collision(terUnitMonk* p);

	const Vect3f& position()const {return position_;}
	void setPosition(const Vect3f& pos);

	float radius()const{return 2.5f;}
	void kill();
	void explosion_kill();
	bool alive()const{return alive_;}
	terPlayer* player()const{return Player;}
private:
	Vect3f SleepPosition;
	
	terMonkMode monk_mode_;
	int DestroyEnable;
	
	float pathPosition_;
	int pathIndex_;
	float pathRemainder_;
	
	float BorderPhase;
	Vect3f BorderPosition;
	
	const Vect2sVect* path_;
};

#endif //__IRONCLUSTERUNIT_H__