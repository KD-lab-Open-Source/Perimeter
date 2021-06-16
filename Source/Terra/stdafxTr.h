#ifndef __STDAFXTR_H__
#define __STDAFXTR_H__

#include "tweaks.h"

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//#define _WIN32_WINNT  0x0400
#ifdef _WIN32
#include <windows.h>
#endif

// STL
#include <vector> 
#include <list>

// hash_map is an old and non-standard MS extension
// see https://docs.microsoft.com/en-us/cpp/standard-library/hash-map?view=msvc-160
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#include <hash_map>
#else
#include <unordered_map>
#endif
#include <map>
#include <algorithm>
#include <cstring>

#if (!defined(_FINAL_VERSION_) || defined(_DEBUG)) && !defined(NASSERT)
#include <iostream>
#endif

using namespace std;

// XTool
#include "xutil.h"
#include "xmath.h"

#include "../Util/Serialization.h"
#include "terra.h"

#endif //__STDAFXTR_H__
