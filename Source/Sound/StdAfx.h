#pragma once
#include "tweaks.h"

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
