#pragma once

#include "tweaks.h"
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

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