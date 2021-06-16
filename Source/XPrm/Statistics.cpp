
#include "StdAfx.h"
#include "Statistics.h"

class TimerDataList: public list<TimerData*> {};


static double total_ticks;
static double total_frames;
static double total_time;
static double time_factor;
static double start_ticks;

inline int ticks2time(int64_t t) { return t ? round((t - start_ticks)*time_factor) : 0; }

TimerData::TimerData(char* title_, int group_, int dont_attach) 
{ 
	timer_data = 1;
	title = title_; 
	group = group_; 
	clear(); 
	if(!dont_attach)
		get_profiler().attach(this);
}

void TimerData::clear()
{
	t = dt_sum = 0;
	n = 0;
	dt_min = 0x7fffffff;
	dt_max = 0;
	t_min = t_max = 0;
	accumulated_alloc = AllocationStatistics();
	start();
}

TimerData& TimerData::operator += (const TimerData& t)
{
	if(!t.timer_data)
		return *this;
	dt_sum += t.dt_sum;
	n += t.n;
	if(dt_max < t.dt_max){
		dt_max = t.dt_max;
		t_max = t.t_max;
		}
	if(dt_min > t.dt_min){
		dt_min = t.dt_min;
		t_min = t.t_min;
		}
	accumulated_alloc += t.accumulated_alloc;
	return *this;
}

void TimerData::print(XBuffer& buf) 
{ 
	char str[2048];
	sprintf(str, "| %-15.15s | %7.3f | %7.3f | %7.2f | %7.3f | %8i | %7.3f | %8i | %9i | %6i | %5i |\r\n", 
		title, (double)dt_sum*100./total_ticks, n ? (double)dt_sum*time_factor/n : 0, n*1000./total_time, 
		(double)dt_max*time_factor, ticks2time(t_max), t_min ? (double)dt_min*time_factor : 0, ticks2time(t_min),
		accumulated_alloc.size, accumulated_alloc.blocks, accumulated_alloc.operations);
	buf < str;
}

void StatisticalData::print(XBuffer& buf)
{
	char str[2048];
	sprintf(str, "| %-15.15s | %7.3f | %7.3f | %7i | %7.3f | %8i | %7.3f | %8i |\r\n", 
		title, avr(), avr() ? sigma()*100/avr() : 0, n, x_max, ticks2time(t_max), x_min, ticks2time(t_min));
	buf < str;
}

static void print_separator(XBuffer& buf) 
{
	buf < "------------------------------------------------------------------------------------------------------------------------\r\n";
}

static void print_header(XBuffer& buf) 
{
	buf < "| Timing:         |  rate % | dt_avr  |  n_avr  | dt_max  |  (t_max) | dt_min  |  (t_min) |  Total memory allocations  |\r\n";
	print_separator(buf);
	buf < "| Statistics:     |  x_avr  |  err %  | counter |  x_max  |  (t_max) |  x_min  |  (t_min) |    size   | blocks | calls |\r\n";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				Profiler
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
Profiler& get_profiler()
{
	static Profiler profiler;
	return profiler;
}

Profiler::Profiler()
: total_data("Total", 0, 1),
timers(*new TimerDataList)
{
	start_ticks = getRDTSC();
	started = 0;
}

Profiler::~Profiler()
{
	delete &timers;
}

void Profiler::attach(TimerData* td)
{
	timers.push_back(td);
}

void Profiler::start_stop()
{
	if(!started){
		TimerDataList::iterator i;
		FOR_EACH(timers, i)
			(*i) -> clear();
		total_data.clear();
		
		started = 1;
		milliseconds = clock();
		ticks = getRDTSC();
		frames = 0;
		memory = total_memory_used();
		}
	else{
		milliseconds = clock() - milliseconds;
		ticks = getRDTSC() - ticks;
		
		static XBuffer buf(10000, 1);
		buf.init();
		print(buf);
		show_profile(buf);
		started = 0;
		}
}

void Profiler::quant()
{
	frames++;

	total_data.stop();
	total_data.start();
}

#include <set>
struct lessTimerData
{																	       
	bool operator()(const TimerData* c1, const TimerData* c2) const
	{
		return c1 -> group != c2 -> group ? c1 -> group < c2 -> group : c1 -> dt_sum > c2 -> dt_sum;
	}
};
			      
typedef multiset<TimerData*, lessTimerData> SortTable;

void Profiler::print(XBuffer& buf)
{
	total_ticks = ticks;
	total_frames = frames;
	total_time = milliseconds;
	time_factor = (double)milliseconds/ticks;

	char total_name[2048];

	buf < "Frames: " <= frames < "\r\n";
	buf < "Time interval: " <= milliseconds < " mS\r\n";
	buf < "Ticks: " < _i64toa(ticks, total_name, 10) < "\r\n";
	buf < "CPU: " <= (double)ticks/(milliseconds*1000.) < " MHz\r\n";
	sprintf(total_name, "%7.3f", frames*1000./milliseconds);
	buf < "FPS: " < total_name < "\r\n";
	buf < "Memory start: " <= memory < "\r\n";
	buf < "Memory end:   " <= total_memory_used() < "\r\n";

	print_separator(buf);
	print_header(buf);	
	print_separator(buf);	

	int group = -1000;
	TimerData total(total_name, 0, 1);

	SortTable sorted_counters;
	TimerDataList::iterator i;
	FOR_EACH(timers, i){
		TimerDataList::iterator j = i; ++j;
		for(; j != timers.end(); ++j)
			if((*i) -> group == (*j) -> group && !strcmp((*i) -> title, (*j) -> title)){
				**j += **i;
				goto add_and_skip;
				}
		sorted_counters.insert(*i);
add_and_skip:;
		}

	SortTable::iterator si;
	FOR_EACH(sorted_counters, si){
		TimerData& td = **si;
		if(group != td.group){
			if(group != -1000){
				sprintf(total_name, "___ Group %1i ___", group);
				total.print(buf);
				}
			total.clear();
			group = td.group;
			}

		td.print(buf);
		total += td;
		}

	sprintf(total_name, "___ Group %1i ___", group);
	total.print(buf);

	print_separator(buf);
	total_data.print(buf);
	print_separator(buf);	
}

