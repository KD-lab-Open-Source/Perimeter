
#include "StdAfx.h"
#include <algorithm>

#ifdef _WIN32
//Required for EditArchive
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif

#include "SoundScript.h"
#include "EditArchive.h"
#include "XPrmArchive.h"
#include "DebugUtil.h"

SINGLETON_PRM(SoundScriptTable, "SoundScriptTable", "Scripts\\SoundScriptTable") soundScriptTable;
