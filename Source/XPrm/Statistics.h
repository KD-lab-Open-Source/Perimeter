#ifndef	__STATISTICS_H__
#define __STATISTICS_H__

// Use to profile memory by start_timers
#ifdef _DEBUG
#define USE_TIMERS_TO_PROFILE_MEMORY 
#endif

struct AllocationStatistics
{
	int size;
	int blocks;
	int operations;
	
	AllocationStatistics() { size = blocks = operations = 0; }
	AllocationStatistics& operator += (const AllocationStatistics& data) { size += data.size; blocks += data.blocks; operations += data.operations; return *this; }
	AllocationStatistics& operator -= (const AllocationStatistics& data) { size -= data.size; blocks -= data.blocks; operations -= data.operations; return *this; }

	static AllocationStatistics dbg_hook_data;
};


/////////////////////////////////////////
//		Memory allocation monitoring
// Using:
// Switch "memory" in command line -> file
// memory dump.
/////////////////////////////////////////
#ifndef _FINAL_VERSION_

struct AllocationAccumulator 
{
	int size, blocks;
	int total_size;
	AllocationAccumulator() { init(); }
	void init() { total_size = size = blocks = 0; }
};

void allocation_tracking(char* title);
void allocation_tracking(char* title, AllocationAccumulator* state);

#else // _FINAL_VERSION_

inline void allocation_tracking(char* title){}
inline void allocation_tracking(char* title, struct AllocationAccumulator* state){}

#endif // _FINAL_VERSION_

int total_memory_used(); // count by pages, works under release and finale


/////////////////////////////////////////
//		Profiler
/////////////////////////////////////////
#ifdef _FINAL_VERSION_

#define start_timer(title, group) 
#define stop_timer(title, group) 
#define start_timer_auto(title, group) 
#define create_timer(title, group) 
#define start_created_timer(title, group) 
#define start_autostop_timer(title, group) 
#define statistics_add(title, group, x) 

inline void profiler_start_stop(){}
inline void profiler_quant(){}
inline void show_profile(const char* text) {}
inline void show_debug_window(const char* text, int sx, int sy) {}
inline void hide_debug_window() {}

#else //_FINAL_VERSION_

__int64 getRDTSC(void);

struct TimerData 
{
	__int64 t;
	__int64 dt_sum;
	int n;
	__int64 dt_max, dt_min;
	__int64 t_min, t_max;

	char* title;
	int group;
	int timer_data;

	AllocationStatistics accumulated_alloc;
	AllocationStatistics last_alloc;

	TimerData(char* title_ = 0, int group_ = 0, int dont_attach = 0);

	__forceinline void start() 
	{ 
		t = getRDTSC(); 
		#ifdef USE_TIMERS_TO_PROFILE_MEMORY
			last_alloc = AllocationStatistics::dbg_hook_data;
		#endif
	}
	__forceinline void stop() 
	{ 
		t = getRDTSC() - t; 
		dt_sum += t;

		if(dt_max < t){
			dt_max = t;
			t_max = getRDTSC();
			}
		if(dt_min > t){
			dt_min = t;
			t_min = getRDTSC();
			}
		n++;

		#ifdef USE_TIMERS_TO_PROFILE_MEMORY
			accumulated_alloc += AllocationStatistics::dbg_hook_data;
			accumulated_alloc -= last_alloc;
		#endif
	}
	virtual void clear();
	TimerData& operator += (const TimerData& t);
	virtual void print(XBuffer& buf);
};

class StatisticalData : public TimerData
{
	double x_sum, x2_sum, x_min, x_max;
public:
	StatisticalData(char* title_ = 0, int group_ = 0) : TimerData(title_, group_, 0) { timer_data = 0; clear(); }
	void clear() { TimerData::clear(); x_sum = x2_sum = 0; x_max = -1e15; x_min = 1e15; }
	void add(double x) { n++; x_sum += x; if(x_min > x){ x_min = x; t_min = getRDTSC(); } if(x_max < x){ x_max = x; t_max = getRDTSC(); } x2_sum += x*x; }
	void print(XBuffer& buf);
	double avr() const { return n ? x_sum/n : 0; }
	double sigma() const { double d2 = (x2_sum - x_sum*x_sum/n); return n > 1 && d2 > 0 ? sqrt(d2/((double)n*(n - 1))) : 0; }
};

class Profiler
{
	class TimerDataList& timers;
	__int64 ticks;
	int frames;
	int milliseconds;
	TimerData total_data;
	int started;
	int memory;
public:
	Profiler();
	~Profiler();
	void attach(TimerData* td);
	void start_stop();
	void quant();
	void print(XBuffer& buf);
};

Profiler& get_profiler();	

class AutoStopTimer
{
	TimerData& timer;
public:
	AutoStopTimer(TimerData& timer_) : timer(timer_) {}
	~AutoStopTimer() { timer.stop(); }
};
	
#define start_timer(title, group) static TimerData timer_##title##group(#title, group); timer_##title##group.start(); 
#define stop_timer(title, group) timer_##title##group.stop();
#define start_timer_auto(title, group) static TimerData timer_##title##group(#title, group); timer_##title##group.start(); AutoStopTimer autostop_timer_##title##group(timer_##title##group); 
#define create_timer(title, group) static TimerData timer_##title##group(#title, group); 
#define start_created_timer(title, group) timer_##title##group.start(); 
#define start_autostop_timer(title, group) static TimerData timer_##title##group(#title, group); timer_##title##group.start(); AutoStopTimer autostop_timer_##title##group(timer_##title##group); 
#define statistics_add(title, group, x) { static StatisticalData timer_##title##group(#title, group); timer_##title##group.add(x); }

inline void profiler_start_stop() { get_profiler().start_stop(); }
inline void profiler_quant() { get_profiler().quant(); }
void show_profile(const char* text);
void show_debug_window(const char* text, int sx, int sy);
void hide_debug_window();

#endif //_FINAL_VERSION_


#endif // __STATISTICS_H__
