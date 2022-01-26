#include <SDL.h>
#include "StdAfx.h"
#include "SynchroTimer.h"

SyncroTimer::SyncroTimer()
{
	use_perfomance=false;
	frequency=SDL_GetPerformanceFrequency();

	set(1, 15, 100);
	time_prev = time = 1; 
	clock_prev = 0;
	time_speed = 1;
	average_interval=-1;
	out_delta=average_delta=20;
}

void SyncroTimer::set(int syncro_by_clock_, time_type time_per_frame_, time_type max_time_interval_) 
{ 
	syncro_by_clock = syncro_by_clock_; 
	time_per_frame = time_per_frame_;
	max_time_interval = max_time_interval_;
}

void SyncroTimer::adjust()
{
	time_prev = time;

	if(syncro_by_clock){
		time_type clock = int_clock();
		time_type dt = xm::round((clock - clock_prev) * time_speed);
		if(dt > max_time_interval)
			dt = max_time_interval;
		time += dt;
		clock_prev = clock;
	}
	calcAverageDelta();
}

void SyncroTimer::next_frame()
{
	if(syncro_by_clock)
		adjust();									 
	else{
		time_prev = time;
		time += xm::round(time_per_frame * time_speed);
	}
	calcAverageDelta();
}

void SyncroTimer::skip()
{
	if(syncro_by_clock){
		clock_prev = int_clock();
		time_prev = time;
	}
	calcAverageDelta();
}

void SyncroTimer::setTime(time_type t) 
{
	time_prev = time = t; clock_prev = syncro_by_clock ? int_clock() : 0;
	calcAverageDelta();
}

unsigned int SyncroTimer::int_clock()
{
	if(use_perfomance)
	{
		double timediff=static_cast<double>(getPerformanceCounter())/frequency;
		return (time_type)(timediff*1000);
	}

	return clocki();
}

void SyncroTimer::calcAverageDelta()
{
	double dt=time - time_prev;
	if(average_interval>0)
	{
		if(dt>average_interval)
			dt=average_interval;
		double tt=max(average_delta,0.02f)/average_interval;
		average_delta=average_delta*(1-tt)+dt*tt;
	}else
	{
		average_delta=dt;
	}

	out_delta= xm::round(average_delta);
}
