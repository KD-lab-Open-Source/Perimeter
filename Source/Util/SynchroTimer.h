#ifndef __SYNCRO_TIMER__
#define __SYNCRO_TIMER__

typedef unsigned int time_type;
	
class SyncroTimer
{
public: 
	// Main property
	time_type operator()() const { return time; }
	// Last delta
	time_type delta() const { return out_delta; }
	float deltaf() const { return average_delta; }
	
	SyncroTimer();
	void setUsePerfomance(bool b){use_perfomance=b;}
	void setAverageInterval(double interval){average_interval=interval;}

	void set(int syncro_by_clock_, time_type time_per_frame_, time_type max_time_interval_);

	void adjust();

	void next_frame();

	void skip();

	void setTime(time_type t);
	void setSpeed(float speed) { time_speed = speed; }

private:	
	time_type time;
	time_type time_prev;
	time_type clock_prev;
	time_type max_time_interval;
	time_type time_per_frame;
	int syncro_by_clock;
	float time_speed;

	double frequency;
	bool use_perfomance;

	double average_interval;
	float average_delta;
	int out_delta;

	unsigned int int_clock();

	void calcAverageDelta();
};

#endif // __SYNCRO_TIMER__
