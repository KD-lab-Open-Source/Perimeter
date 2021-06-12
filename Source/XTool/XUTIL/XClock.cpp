
/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include <windows.h>
#include <intrin.h>

#include "xutl.h"

#pragma comment(lib, "winmm.lib")

__int64 tick_per_sec=0;
__int64 beg_tick=0;
const unsigned int MS_PER_PERIOD=1000;


#if _MSC_VER >= 1310
__declspec (noinline)
#endif // _MSC_VER >= 1310
__int64 getRDTSC() {
    //TODO use std::chrono::high_resolution_clock::now()
    return __rdtsc();
}

inline __int64 getQPC(){
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	__int64 rv=t.QuadPart;
	return rv;
}

void initclock()
{
	if(tick_per_sec!=0)
		return;
/*	clock_t t1,t2;
	__int64 tickb,ticke;
	t1=t2=clock();
	while(t1==t2){ 
		t2=clock();
	};

	tickb=getRDTSC();
	t1=t2;
	while(t1+CLOCKS_PER_SEC > t2){
		t2=clock();
	}
	ticke=getRDTSC();
	tick_per_sec = (ticke-tickb)/1000;
	beg_tick=getRDTSC();
*/
	timeBeginPeriod(1);

	unsigned int t2=timeGetTime(), t1 = t2;

	while(t1==t2){ 
		t2=timeGetTime();
	};

	__int64 tickb=getRDTSC();
	t1=t2;
	while(t1+MS_PER_PERIOD > t2){
		t2=timeGetTime();
	}
	__int64 ticke=getRDTSC();
	tick_per_sec = (ticke-tickb)/MS_PER_PERIOD;
	beg_tick=getRDTSC();

	timeEndPeriod(1);

/*	LARGE_INTEGER TK_PER_SEC;
	QueryPerformanceFrequency(&TK_PER_SEC);

	__int64 t1,t2;
	__int64 tickb,ticke;
	t1=t2=getQPC(); 
	while(t1==t2){ 
		t2=getQPC();
	};

	tickb=getRDTSC();
	t1=t2;
	while(t1+TK_PER_SEC.QuadPart > t2){
		t2=getQPC();
	}
	ticke=getRDTSC();
	tick_per_sec = (ticke-tickb)/1000;//MS_PER_SEC;
	beg_tick=getRDTSC();*/

}

double clockf()
{
	return (double)(getRDTSC()-beg_tick)/(double)tick_per_sec;
} 

int clocki()
{
	return (int)((getRDTSC()-beg_tick)/tick_per_sec);
}

