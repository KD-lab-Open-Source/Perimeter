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

#if defined(_MSC_VER) && (_MSC_VER < 1900)
// non-standard header : https://developercommunity.visualstudio.com/t/msvc-142328019-is-missing-include-typeinfoh/734566
#include <typeinfo.h>
#else
#include <typeinfo>
#endif
#include <float.h>
#include <mmsystem.h>
#include <commctrl.h>
#include <dplay8.h>

// STL
#include <vector> 
#include <list>
#if defined(_MSC_VER) && (_MSC_VER < 1900)
// non-standard header
#include <slist>
#else
#define slist list
#endif

// hash_map is an old and non-standard MS extension
// see https://docs.microsoft.com/en-us/cpp/standard-library/hash-map?view=msvc-160
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#include <hash_map> 
#else
#include <unordered_map>
#endif

#include <map>
#include <string>
#include <deque>
#include <algorithm>

using namespace std;

// XTool
#include "xutil.h"
#include "xmath.h"
#include "Timers.h"

#include "umath.h"
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
