#pragma once

/* В одном потоке работает логика, в другом - графика.
GameShell в графическом потоке.
*/
#include "fps.h"


class HTManager
{
public:
	HTManager(bool ht);
	~HTManager();

	bool Quant();

	static HTManager* instance(){return self;};

	void initSyncroTimer(){ start_timer = false; }
	void setSyncroTimer(int syncro_by_clock, 
		time_type time_per_frame, time_type max_time_interval_);

	void setSpeedSyncroTimer(float speed);
	void startSyncroTimer() { start_timer = false; }
	void setLogicTimePeriod(int logicTimePeriod) { dtime=logicTimePeriod; }

	void setMissionToStart(const MissionDescription& mission) { missionToStart_ = mission; restartGame_ = true; }

	void GameStart(const MissionDescription& mission);
	void GameClose();

	void setUseHT(bool use_ht_);
	bool IsUseHT()const{return use_ht;};
	bool PossibilityHT();
	float interpolationFactor() const { return interpolation_factor_; }

	MTSection* GetLockDeleteUnit(){return &lock_delete;};
	void DeleteUnit(terUnitBase* unit);
	float GetLogicFps();
	void GetLogicFPSminmax(float& fpsmin,float& fpsmax);
	void ClearDeleteUnit(bool delete_all);

	MTSection* GetLockLogic(){return &lock_logic;}

	void Show();
protected:
	static HTManager* self;

	bool LogicQuant();
	void GraphQuant();

	bool use_ht;
	cFont* pDefaultFont;
	void init();
	void done();
	void initGraphics();
	void finitGraphics();

	SyncroTimer syncro_timer;
	bool start_timer;
	time_type time;
	time_type dtime;

	DWORD logic_thread_id;
	struct LogicThreadArg
	{
		MissionDescription mission;
	};
	void logic_thread();
	friend void logic_thread( void * argument);

	bool init_logic;

	int interpolation_timer_;
	float interpolation_factor_;

	HANDLE end_logic;

	struct DELETE_DATA
	{
		list<terUnitBase*> unit;
		int quant;
	};
	list<DELETE_DATA> DeleteList;
	FPS logic_fps;

	MissionDescription missionToStart_;
	bool restartGame_;
	MTSection lock_logic;
	MTSection lock_fps;
	MTSection lock_delete;

	class LagStatistic* lag_stat;
};