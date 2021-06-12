#pragma once

#include <my_STL.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif

#include <stdio.h>

#include <vector>
#include <list>
#include <stack>
#include <map>
#include <hash_map>
using namespace std;

#include "Umath.h"

#include "IRenderDevice.h"
#include "IVisGeneric.h"

extern cTileMap	*terMapPoint;