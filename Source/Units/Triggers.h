#ifndef _PERIMETER_TRIGGERS_
#define _PERIMETER_TRIGGERS_

#include "Conditions.h"

class terUnitBase;
class terCameraType;
class terPlayer;

//-------------------------------

class Event
{
public:
	enum Type {
		CREATE_OBJECT,
		CREATE_BASE_UNIT,
		AIM_AT_OBJECT,
		ATTACK_OBJECT,
		KILL_OBJECT,
		DESTROY_OBJECT,
		TELEPORTATION,
		CAPTURE_BUILDING, 
		PLAYER_STATE, 
		TIME,
		COMPLETE_BUILDING,
		MOUSE_CLICK, 
		CLICK_ON_BUTTON, 
		ACTIVATE_SPOT
	};
	Event(Type type) : type_(type) {}
	Type type() const { return type_; }
	virtual ~Event(){}

protected:
	Type type_;
};

class EventUnitPlayer : public Event
{
public:
	EventUnitPlayer(Type type, terUnitBase* unit, terPlayer* player) : Event(type), unit_(unit), player_(player) {}
	terUnitBase* unit() const { return unit_; }
	terPlayer* player() const { return player_; }

private:
	terUnitBase* unit_;
	terPlayer* player_;
};

class EventUnitMyUnitEnemy : public Event
{
public:
	EventUnitMyUnitEnemy(Type type, terUnitBase* unitMy, terUnitBase* unitEnemy) : Event(type), unitMy_(unitMy), unitEnemy_(unitEnemy) {}
	terUnitBase* unitMy() const { return unitMy_; }
	terUnitBase* unitEnemy() const { return unitEnemy_; }

private:
	terUnitBase* unitMy_;
	terUnitBase* unitEnemy_;
};

class EventPlayerState : public Event
{
public:
	EventPlayerState(PlayerState playerState, bool activate) : Event(PLAYER_STATE), playerState_(playerState), activate_(activate) {}
	PlayerState playerState() const { return playerState_; }
	bool activate() const { return activate_; }

private:
	PlayerState playerState_;
	bool activate_;
};

class EventTime : public Event
{
public:
	EventTime(int time) : Event(TIME), time_(time) {}
	int time() const { return time_; }

private:
	int time_;
};

class EventClickOnButton : public Event
{
public:
	EventClickOnButton(ShellControlID controlID) : Event(CLICK_ON_BUTTON), controlID_(controlID) {}
	ShellControlID controlID() const { return controlID_; }

private:
	ShellControlID controlID_;
};

class EventActivateSpot : public Event
{
public:
	EventActivateSpot(bool isFilth) : Event(ACTIVATE_SPOT), isFilth_(isFilth) {}
	bool isFilth() const { return isFilth_; }

private:
	bool isFilth_;
};


#endif
