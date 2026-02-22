// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently

#include "tweaks.h"

#ifdef _AFXDLL

#define VC_EXTRALEAN
#include <afxwin.h>      // MFC core and standard components

#else

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#endif

#include <stdio.h>
#include <ctype.h>
#include <iostream.h>
#include <time.h>

// STL
#include <vector> 
#include <list>
#include <string>
#include <stdexcept>
#include <hash_map>
#include <algorithm>

#include "tweaks.h"
#include "handle.h"
#include "XUtil.h"
#include "xmath.h"



				     