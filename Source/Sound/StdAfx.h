#pragma once
#include "tweaks.h"

#ifndef PERIMETER_HEADLESS
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif
#include <cstdlib>
#include <cstdio>

#if (!defined(_FINAL_VERSION_) || defined(_DEBUG)) && !defined(NASSERT)
#include <iostream>
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cmath>


#ifndef ASSERT
#define ASSERT(f) xassert(f)
#endif //ASSERT

#define SIZE(ar) (sizeof(ar)/sizeof(ar[0]))

#include "xmath.h"
#include "xutil.h"
#include <SDL_mixer.h>

#include <map>
#include <vector>
#include <string>
