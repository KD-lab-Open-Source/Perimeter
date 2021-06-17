#include "tweaks.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <map>

#if (!defined(_FINAL_VERSION_) || defined(_DEBUG)) && !defined(NASSERT)
#include <iostream>
#endif

#include <d3d9.h>
#include <d3dx9.h>

using namespace std;

#include "../inc/Umath.h"
#include "../../PluginMAX/Src/BaseClass.h"
#include "../inc/IVisGenericInternal.h"
#include "RenderDevice.h"
#include "../src/VisError.h"
#include "../inc/VisGenericDefine.h"
#include "../inc/RenderMT.h"

#include "../src/util.h"
#include "../src/TexLibrary.h"
#include "../src/Texture.h"
#include "../src/texture.inl"
#include "../src/Material.h"
#include "../inc/IUnkObj.h"
#include "../src/Frame.h"
#include "../src/UnkObj.h"
#include "../src/ObjNode.h"
#include "../src/cCamera.h"

#include "../PluginMAX/ZIPStream.h"

#include "../saver/Saver.h"
#include "../saver/saver_render.h"

template<class TYPE>
class DebugType
{
	TYPE t;
	bool debug;
public:
	DebugType(TYPE tt=0):t(tt),debug(false){ }

	inline operator TYPE(){return t;}
	void operator=(TYPE tt){if(!debug)t=tt;}

	void setd(TYPE tt)
	{
		if(tt==-1)
		{
			debug=false;
			return;
		}

		debug=true;
		t=tt;
	}
};


extern DebugType<int>	Option_DrawMeshBound;
extern DebugType<int>	Option_DrawMeshBoundWire;
extern DebugType<int>	Option_MipMapLevel;
extern DebugType<int>	Option_MipMapBlur;
extern DebugType<int>	Option_DrawNumberPolygon;
extern DebugType<float>	Option_NearDistanceLOD;
extern DebugType<float>	Option_FarDistanceLOD;
extern DebugType<int>	Option_DrawMeshShadow;
extern DebugType<int>	Option_DrawMeshScreen;
extern DebugType<float>	Option_MapLevel;
extern DebugType<int>	Option_ShowRenderTextureDBG;
extern DebugType<int>	Option_DebugShowShadowVolume;
extern DebugType<int>	Option_ShadowType;
extern DebugType<int>	Option_FavoriteLoadDDS;
extern DebugType<int>	Option_EnableBump;
extern DebugType<int>	Option_EnableLinkEffectToModel;
extern DebugType<int>	Option_EnableOcclusion;
extern float Option_InvOcclusionFadeTime;
extern DebugType<int>	Option_EnablePointLight;
extern DebugType<int>	Option_ShadowMapSelf4x4;
extern DebugType<float>	Option_ParticleRate;
extern DebugType<int>	Option_ShadowHint;

extern bool Option_ShowType[SHOW_MAX];

extern bool Option_IsShadowMap;

extern class cVisGeneric		*gb_VisGeneric;

#include "../inc/IncTerra.h"

#ifdef _DEBUG
//#define TEXTURE_NOTFREE
#endif

#include "D3DRender.h"

int ResourceFileRead(const char *fname,char *&buf,int &size);
