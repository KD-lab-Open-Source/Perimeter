
/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include <cstdint>

#include "xutl.h"
#include "xerrhand.h"
#include <SDL.h>

uint64_t perf_freq=0;
uint64_t freq_ms=0;
uint64_t freq_us=0;
uint64_t counter_at_init=0;

uint64_t getPerformanceCounter() {
    return SDL_GetPerformanceCounter();
}

uint64_t getPerformanceFrequency() {
    if (perf_freq == 0) {
        xassert(0);
        initclock();
    }
    return perf_freq;
}

void initclock()
{
	if (perf_freq != 0) {
        return;
    }

    perf_freq = SDL_GetPerformanceFrequency();
    freq_ms = getPerformanceFrequency() / static_cast<uint64_t>(1000);
    freq_us = getPerformanceFrequency() / static_cast<uint64_t>(1000000);
    counter_at_init = getPerformanceCounter();
}

double clockf() {
    xassert(counter_at_init);
	return static_cast<double>(getPerformanceCounter() - counter_at_init) / static_cast<double>(freq_ms);
} 

int clocki() {
    xassert(counter_at_init);
    return static_cast<int>((getPerformanceCounter() - counter_at_init) / freq_ms);
}

uint64_t clock_us() {
    xassert(counter_at_init);
    return (getPerformanceCounter() - counter_at_init) / freq_us;
}

