#include <cstring>
#include <cstdio>
#include <windows.h>
#include <intrin.h>

#include "xutil.h"
#include "mmsystem.h"

//XClock

__int64 tick_per_sec=0;
__int64 beg_tick=0;
const unsigned int MS_PER_PERIOD=1000;

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

    unsigned int t1,t2;
    __int64 tickb,ticke;
    t1=t2=timeGetTime();
    while(t1==t2){
        t2=timeGetTime();
    };

    tickb=getRDTSC();
    t1=t2;
    while(t1+MS_PER_PERIOD > t2){
        t2=timeGetTime();
    }
    ticke=getRDTSC();
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

//XRnd

static unsigned int XRndValue = 83838383;

unsigned int XRnd(unsigned int m)
{ 
	XRndValue = XRndValue*214013L + 2531011L;
	if(!m)
		return 0;
	return ((XRndValue>> 16) & 0x7fff) % m; 
}

void XRndSet(unsigned int m)
{
    XRndValue = m;
}

unsigned int XRndGet()
{
    return XRndValue;
}
const char* check_command_line(const char* switch_str) {
    //TODO seems like it should find switch_str as key in cmdline and return string with the passed value 
    return NULL;
}