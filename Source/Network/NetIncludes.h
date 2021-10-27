#ifndef PERIMETER_NETINCLUDES_H
#define PERIMETER_NETINCLUDES_H

#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <climits>
#include <SDL_net.h>
#include <map>

#ifndef PERIMETER_HEADLESS
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <d3d9.h>
#endif

#include "tweaks.h"
#include "xutil.h"
#include "xmath.h"
#include "Timers.h"

#include "Umath.h"
#include "IRenderDevice.h"
#include "IVisGeneric.h"
#include "VisGenericDefine.h"
#include "RenderMT.h"

#include "DebugUtil.h"
#include "SystemUtil.h"

#include "NetConnection.h"
#include "NetComEventBuffer.h"
#include "CommonEvents.h"

#endif //PERIMETER_NETINCLUDES_H
