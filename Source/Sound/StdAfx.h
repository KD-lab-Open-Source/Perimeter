#pragma once
#include <my_STL.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif
#include <cstdlib>
#include <cstdio>
#include <mmsystem.h>
#include <dsound.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <cmath>
#include <xutil.h>
#include "../Render/inc/RenderMT.h"


#ifndef ASSERT
#define ASSERT(f) xassert(f)
#endif //ASSERT

#define SIZE(ar) (sizeof(ar)/sizeof(ar[0]))

extern HWND snd_hWnd;

#include "xmath.h"
#include "xutil.h"

#include <map>
#include <vector>
#include <string>
using namespace std;

