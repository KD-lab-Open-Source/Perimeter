
/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include <windows.h>
#include <intrin.h>
#include <cstdint>

#include "xutl.h"

#pragma comment(lib, "winmm.lib")

int64_t tick_per_sec=0;
int64_t beg_tick=0;
const unsigned int MS_PER_PERIOD=1000;


#if _MSC_VER >= 1310
__declspec (noinline)
#endif // _MSC_VER >= 1310
int64_t getRDTSC() {
    //TODO use std::chrono::high_resolution_clock::now()
    return __rdtsc();
}

inline int64_t getQPC(){
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
    int64_t rv=t.QuadPart;
	return rv;
}

void initclock()
{
	if(tick_per_sec!=0)
		return;
	timeBeginPeriod(1);

	unsigned int t2=timeGetTime(), t1 = t2;

	while(t1==t2){ 
		t2=timeGetTime();
	};

    int64_t tickb=getRDTSC();
	t1=t2;
	while(t1+MS_PER_PERIOD > t2){
		t2=timeGetTime();
	}
    int64_t ticke=getRDTSC();
	tick_per_sec = (ticke-tickb)/MS_PER_PERIOD;
	beg_tick=getRDTSC();

	timeEndPeriod(1);

}

double clockf()
{
	return (double)(getRDTSC()-beg_tick)/(double)tick_per_sec;
} 

int clocki()
{
	return (int)((getRDTSC()-beg_tick)/tick_per_sec);
}

