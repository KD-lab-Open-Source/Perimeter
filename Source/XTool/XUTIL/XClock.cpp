
/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include <cstdint>

#include "xutl.h"
#include <SDL.h>

uint64_t tick_per_sec=0;
uint64_t beg_tick=0;

uint64_t getPerformanceCounter() {
    return SDL_GetPerformanceCounter();
}

uint64_t getPerformanceFrequency() {
    return SDL_GetPerformanceFrequency();
}

void initclock()
{
	if (tick_per_sec!=0) {
        return;
    }

	tick_per_sec = getPerformanceFrequency() / static_cast<uint64_t>(1000);
    beg_tick = getPerformanceCounter();
}

double clockf()
{
	return static_cast<double>(getPerformanceCounter() - beg_tick) / static_cast<double>(tick_per_sec);
} 

int clocki()
{
    return static_cast<int>((getPerformanceCounter() - beg_tick) / tick_per_sec);
}

