#include "StdAfx.h"
#include "ht.h"
#include "GameShell.h"
#include "GenericControls.h"
#include "Config.h"
#include "Universe.h"
#include "LagStatistic.h"
#include <cstdlib>
#include <thread>
#include <SDL_thread.h>

#ifdef _WIN32
#include "objbase.h"
#endif

const SDL_threadID bad_thread_id=-1;

HTManager* HTManager::self=nullptr;
HTManager::HTManager(bool ht)
{
	lag_stat=new LagStatistic;
	logic_thread_id=bad_thread_id;
	setUseHT(ht);

	restartGame_ = false;
	
	global_time.setUsePerfomance(false);
	frame_time.setUsePerfomance(false);
	scale_time.setUsePerfomance(false);
	global_time.setAverageInterval(-1);
	frame_time.setAverageInterval(-1);
	scale_time.setAverageInterval(-1);
	self=this;
	init_logic=false;
	end_logic=nullptr;

	start_timer=false;
	dtime=100;
	time=0;
	init();
}

HTManager::~HTManager()
{
    MT_SET_TYPE(MT_LOGIC_THREAD | MT_GRAPH_THREAD);
	done();
	self=nullptr;
	delete lag_stat;
}

void HTManager::setSyncroTimer(int syncro_by_clock, 
					time_type time_per_frame, time_type max_time_interval_) 
{
	syncro_timer.set(syncro_by_clock, 
			time_per_frame, 
			max_time_interval_);
}

void HTManager::setSpeedSyncroTimer(float speed)
{
	syncro_timer.setSpeed(speed);
	gb_VisGeneric->SetLogicTimePeriodInv(speed*terLogicTimePeriodInv);
}

int logic_thread_init(void*)
{
#ifdef _WIN32
    //_alloca(4096+128); //TODO is this need?
    
    //Required for VFW so game can load AVI files in logic thread (like when AI builds Officer plant)
    //VFW is used when not having FFMPEG, but just in case call it always
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
#endif

	HTManager::instance()->logic_thread();
	return 0;
}

void HTManager::setUseHT(bool use_ht_)
{
	if(PossibilityHT())
		use_ht=use_ht_;
	else
		use_ht=false;
}

void HTManager::GameStart(const MissionDescription& mission)
{
    MT_SET_TYPE(MT_LOGIC_THREAD | MT_GRAPH_THREAD);
	gameShell->GameStart(mission);
    MT_SET_TYPE(MT_GRAPH_THREAD);
	syncro_timer.skip();
	syncro_timer.next_frame();
	time=syncro_timer();
	if(use_ht)
	{
		xassert(logic_thread_id==bad_thread_id);
        SDL_Thread* thread = SDL_CreateThread(logic_thread_init, "perimeter_logic_thread", nullptr);
        if (thread == nullptr) {
            SDL_PRINT_ERROR("SDL_CreateThread perimeter_logic_thread failed");
            logic_thread_id = bad_thread_id;
        }  else {
            logic_thread_id = SDL_GetThreadID(thread);
            SDL_DetachThread(thread);
        }
	}
}

void HTManager::GameClose()
{
	MTG();
    MT_SET_TYPE(MT_LOGIC_THREAD | MT_GRAPH_THREAD);
	if(use_ht && logic_thread_id!=bad_thread_id)
	{
		end_logic=SDL_CreateSemaphore(0);

		uint32_t ret= SDL_SemWait(end_logic);
		xassert(ret==0);

        SDL_DestroySemaphore(end_logic);
		end_logic=nullptr;
		logic_thread_id=bad_thread_id;
	}

	gameShell->GameClose();
    MT_SET_TYPE(MT_GRAPH_THREAD);
}

void HTManager::logic_thread()
{	
	init_logic=true;
    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);
//	gameShell->GameStart(logic_arg.mission);
	init_logic=false;
	if(!start_timer){
		start_timer = true;
		syncro_timer.setTime(1);
		syncro_timer.skip();
		time = syncro_timer();
	}
    if (check_command_line("dump_mt_tls")) {
        debug_dump_mt_tls();
    }
    MT_SET_TYPE(MT_LOGIC_THREAD);
    if (check_command_line("dump_mt_tls")) {
        debug_dump_mt_tls();
    }

	while(end_logic== nullptr)
	{
		while(!applicationIsGo())
			Sleep(10);
		syncro_timer.next_frame();
		gameShell->NetQuant();

		while(time < syncro_timer())
		{
			if(!applicationIsGo())
				break;

			{
				MTAuto lock(&lock_logic);
				gameShell->NetQuant();
			}

			if(universe())
				lag_stat->SetLagQuant(universe()->getInternalLagQuant());
			if(LogicQuant()){
				time+=lag_stat->CalcDTime();
			}else
			{
				lag_stat->SetNetSkip();
				break;
			}

			lag_stat->NextLogicQuant();
		}

		//если logic_fps<10, то не пытаться догонять за счёт графики
		if(time < syncro_timer())
		{
			lag_stat->SetNetWait((syncro_timer()-time)*1e-3f);
			time=syncro_timer();
		}else
			Sleep(10);
	}

	SDL_SemPost(end_logic);
}

bool HTManager::Quant()
{
	if(restartGame_){
		restartGame_ = false;
		GameClose();
		GameStart(missionToStart_);
	}

	if(use_ht)
	{
		if(init_logic)
			Sleep(100);

		if(logic_thread_id==bad_thread_id)
		{
			gameShell->NetQuant();
		}

		GraphQuant();
	}
	else {
		start_timer_auto(GameQuant, STATISTICS_GROUP_OVERALL);
		if(!start_timer){
			start_timer = true;
			syncro_timer.setTime(1);
			syncro_timer.skip();
			syncro_timer.next_frame();
			time = syncro_timer();
		}

        MT_SET_TYPE(MT_LOGIC_THREAD);//Для MTG, MTL ассертов.
		syncro_timer.next_frame();
		while((time < syncro_timer()))
		{
			gameShell->NetQuant();

			if(universe())
				lag_stat->SetLagQuant(universe()->getInternalLagQuant());
			if(LogicQuant()){
				//time += dtime;
				time +=lag_stat->CalcDTime();
			}else
			{
				lag_stat->SetNetSkip();
				break;
			}

			lag_stat->NextLogicQuant();
		}

        MT_SET_TYPE(MT_GRAPH_THREAD);

		//если logic_fps<10, то не пытаться догонять за счёт графики
		if(time < syncro_timer())
		{
			lag_stat->SetNetWait((syncro_timer()-time)*1e-3f);
			time=syncro_timer();
		}

		gameShell->NetQuant();
		GraphQuant();
	}
	
	return gameShell->GameContinue;
}

void HTManager::DeleteUnit(terUnitBase* unit)
{
	MTL();
	int quant=universe()->quantCounter();

	if(DeleteList.empty() || DeleteList.back().quant!=quant)
	{
		DELETE_DATA d;
		d.quant=quant;
		DeleteList.push_back(d);
	}

	DELETE_DATA& dd=DeleteList.back();
	dd.unit.push_back(unit);
}

void HTManager::ClearDeleteUnit(bool delete_all)
{
	int quant=0;
	const int wait_to_delete=10;//Количество квантов, котороё ждётся
							//в логическом кванте, прежде, чем удалить объект

	if(!delete_all)
	{
		if(universe())
			quant=universe()->quantCounter();
		else
			quant=0;

		if(use_ht)
		{
			while(gb_VisGeneric->GetGraphLogicQuant()<quant-wait_to_delete+2)
			{
				Sleep(10);
	//			xassert(0 && "Wait graph");
			}
		}else
		{
			xassert(!(gb_VisGeneric->GetGraphLogicQuant()<quant-wait_to_delete+2));
		}
	}

	MTAuto lock(GetLockDeleteUnit());
	std::list<DELETE_DATA>::iterator itd=DeleteList.begin();
	
	while(itd!=DeleteList.end())
	{
		if(!delete_all && itd->quant>quant-wait_to_delete)
		{
			itd++;
			continue;
		}

		std::list<terUnitBase*>& lst=itd->unit;

		std::list<terUnitBase*>::iterator it;
		FOR_EACH(lst,it)
		{
			terUnitBase* p=*it;
			delete p;
		}
		lst.clear();

		itd=DeleteList.erase(DeleteList.begin());
	}
}

bool HTManager::PossibilityHT()
{
    const unsigned int processor_count = std::thread::hardware_concurrency();
    //NOTE: processor_count may be 0 if couldn't be detected
    return 1 < processor_count;
}

float HTManager::GetLogicFps()
{
	MTAuto lock(&lock_fps);
	return logic_fps.GetFPS();
}

void HTManager::GetLogicFPSminmax(float& fpsmin,float& fpsmax)
{
	MTAuto lock(&lock_fps);
	logic_fps.GetFPSminmax(fpsmin,fpsmax);
}

void HTManager::Show()
{
#ifndef _FINAL
	if(universe() && universe()->multiPlayer())
	if(debug_show_lag_stat)
		lag_stat->Show();
#endif //_FINAL
}


MTAutoSingleThread::MTAutoSingleThread() {
    real_tls = MT_GET_TYPE();
    if (real_tls == MT_GRAPH_THREAD) {
        lock = HTManager::instance()->GetLockLogic();
    }
    if (lock) {
        lock->Lock();
    }
    MT_SET_TYPE(MT_LOGIC_THREAD | MT_GRAPH_THREAD);
}

MTAutoSingleThread::~MTAutoSingleThread()
{
    MT_SET_TYPE(real_tls);
    if (lock) {
        lock->Unlock();
    }
}
