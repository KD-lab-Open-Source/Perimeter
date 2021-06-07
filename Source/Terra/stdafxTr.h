#ifndef __STDAFXTR_H__
#define __STDAFXTR_H__

#include <my_STL.h>

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//#define _WIN32_WINNT  0x0400
#include <windows.h>

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

using namespace std;

// XTool
#include "xutil.h"
#include "xmath.h"

#include "terra.h"

#endif //__STDAFXTR_H__
