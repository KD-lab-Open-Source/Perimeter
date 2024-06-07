#ifndef __STDAFXTR_H__
#define __STDAFXTR_H__

#include "tweaks.h"

// STL
#include <vector> 
#include <list>
#include <unordered_map>

#include <map>
#include <algorithm>
#include <cstring>

#if (!defined(_FINAL_VERSION_) || defined(_DEBUG)) && !defined(NASSERT)
#include <iostream>
#endif

// XTool
#include "xutil.h"
#include "xmath.h"

#include "../Util/Serialization.h"
#include "terra.h"

#endif //__STDAFXTR_H__
