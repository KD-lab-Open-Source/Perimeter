#pragma once
#define WIN32_LEAN_AND_MEAN
#define _INC_WINDOWSX
#include "IGame.h"
#include "IGameObject.h"
#include "IGameProperty.h"
#include "IGameControl.h"
#include "IGameModifier.h"
#include "IConversionManager.h"
#include "IGameError.h"
#include "IGameExporter.h"
#include "decomp.h"

#undef X_AXIS
#undef Y_AXIS
#undef Z_AXIS

#include <my_stl.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <list>
#include <xutil.h>
#include <xmath.h>
using namespace std;

#ifdef _DEBUG
#define verify(x) xassert(x)
#else
#define verify(x) x
#endif

#include "3dx.h"
#include "Console.h"
#include "../Render/saver/Saver.h"

#include "RootExport.h"

HTREEITEM InsertTree(HWND hTree,HTREEITEM hParent,LPCSTR name,DWORD lparam=0,int image=-1);
char* GetTreeItemName(HWND hTree,HTREEITEM h);
DWORD GetTreeItemParam(HWND hTree,HTREEITEM h);