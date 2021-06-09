#pragma once
#include <my_STL.h>

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT  0x0400
#include <windows.h>

// Standart includes
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <direct.h>

#include <typeinfo.h>
#include <float.h>
#include <mmsystem.h>
#include <commctrl.h>
#include <dplay8.h>

// STL
#include <vector> 
#include <list>
#include <slist>
#include <hash_map>
#include <map>
#include <string>
#include <deque>
#include <algorithm>

using namespace std;

// XTool
#include "xutil.h"
#include "xmath.h"
#include "Timers.h"

#include "Umath.h"
#include "IRenderDevice.h"
#include "IVisGeneric.h"
#include "VisGenericDefine.h"

#include "SystemUtil.h"
#include "DebugUtil.h"

#include "ConnectionDP.h"
#include "EventBufferDP.h"
#include "CommonEvents.h"

#define _STARFORCE_

#ifdef _STARFORCE_
#define STARFORCE_API extern "C" __declspec(dllexport)
#define STARFORCE_API_NEW extern "C" __declspec(dllexport)
#else
#define STARFORCE_API 
#define STARFORCE_API_NEW 
#endif

#ifdef small
#undef small
#endif
