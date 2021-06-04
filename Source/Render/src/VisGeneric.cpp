#include "StdAfxRD.h"
#include "VisGeneric.h"
#include "ObjLibrary.h"
#include "Scene.h"
#include "Font.h"
#include "..\3dx\Lib3dx.h"

void Init3dxshader();
void Done3dxshader();

static void get_string(char*& str,char* s)
{
	while(*str && IsCharAlphaNumeric(*str))
		*s++=*str++;

	*s=0;
}

static int get_int(char* s)
{
	while(*s && (*s=='\t' || *s==' '))
		s++;
	int i=-1;
	sscanf(s,"%i",&i);
	return i;
}
static float get_float(char* s)
{
	while(*s && (*s=='\t' || *s==' '))
		s++;
	float f=-1;
	sscanf(s,"%f",&f);
	return f;
}

// глобальные переменные
cVisGeneric	*gb_VisGeneric=0;

DebugType<int>		Option_DrawMeshBound(0);
DebugType<int>		Option_DrawMeshBoundWire(0);
DebugType<int>		Option_MipMapLevel(3);
DebugType<int>		Option_MipMapBlur(1);
DebugType<int>		Option_DrawNumberPolygon(0);
DebugType<float>	Option_NearDistanceLOD(0.05f);
DebugType<float>	Option_FarDistanceLOD(0.005f);
DebugType<int>		Option_DrawMeshShadow(true);
DebugType<int>		Option_DrawMeshScreen(true);
DebugType<float>	Option_MapLevel(0.8f);
DebugType<int>		Option_ShowRenderTextureDBG(0);
DebugType<int>		Option_DebugShowShadowVolume(0);
DebugType<int>		Option_ShadowType(false);
DebugType<int>		Option_FavoriteLoadDDS(false);
bool				Option_IsShadowMap=false;
DebugType<int>		Option_EnableBump(true);
DebugType<int>		Option_EnableLinkEffectToModel(true);
DebugType<int>		Option_EnableOcclusion(false);
float				Option_InvOcclusionFadeTime=1/0.5e3f;
DebugType<int>		Option_EnablePointLight(true);
DebugType<int>		Option_ShadowMapSelf4x4(true);//only radeon 9700
DebugType<float>	Option_ParticleRate(1);
DebugType<int>		Option_ShadowHint(0);

bool cVisGeneric::assertEnabled_ = false;

bool Option_ShowType[SHOW_MAX];

__declspec( thread ) DWORD tls_is_graph=MT_LOGIC_THREAD;

float CONVERT_PROCENT(float x,float min,float max)
{
	return x*(max-min)/100.f+min;
}

cVisGeneric* CreateIVisGeneric()
{
	if(gb_VisGeneric) 
		gb_VisGeneric->IncRef();
	else
		gb_VisGeneric=new cVisGeneric;
	return gb_VisGeneric;
}

cVisGeneric::cVisGeneric() : cUnknownClass(KIND_UI_VISGENERIC)//: SceneArray(KIND_ARRAYSCENE)
{
	MTINIT(lock_effect_library);
	logic_quant=graph_logic_quant=0;
	tls_is_graph=MT_GRAPH_THREAD;
	for(int i=0;i<SHOW_MAX;i++)
		Option_ShowType[i]=true;
	Option_ShowType[SHOW_INFO]=false;
	// инициализация глобальных переменых
	shaders=NULL;
	ObjLibrary=new cObjLibrary();
	Lib3dx=new cLib3dx;

	FILE* f=fopen("VisGeneric.cfg","rt");
	if( !f ) return;
	char buf[1024],str[1024];
	str[0]=0;
#define RDI(x) if(stricmp(str,#x)==0) Option_##x.setd(get_int(p)) 
	while(fgets(buf,1023,f))
	{
		char* p=buf;
		get_string(p,str);

		RDI(DrawMeshBound);
		RDI(DrawMeshBoundWire);
		RDI(MipMapLevel);
		RDI(MipMapBlur);
		RDI(DrawNumberPolygon);
		RDI(DrawMeshShadow);
		RDI(DrawMeshScreen);
		RDI(ShowRenderTextureDBG);
		RDI(DebugShowShadowVolume);
		RDI(ShadowType);
		RDI(FavoriteLoadDDS);
		RDI(EnableOcclusion);
		RDI(EnablePointLight);
		RDI(ShadowMapSelf4x4);
		if(stricmp(str,"NearDistanceLOD")==0)
		{
			float flt=get_float(p);
			if(1<=flt && flt<=100) 
				Option_NearDistanceLOD=CONVERT_PROCENT(flt, 0.1f, 0.01f);
		}
		else if(stricmp(str,"FarDistanceLOD")==0)
		{
			float flt=get_float(p);
			if(1<=flt && flt<=100) 
				Option_FarDistanceLOD=CONVERT_PROCENT(flt, 0.01f, 0.001f);
		}
		else if(stricmp(str,"MapLevel")==0)
		{
			float flt=get_float(p);
			if(1<=flt && flt<=100)
				Option_MapLevel=CONVERT_PROCENT(flt, 0.1f, 0.8f);
		}else if(stricmp(str,"ParticleRate")==0)
		{
			float flt=get_float(p);
			Option_ParticleRate.setd(flt);
		}
	}
	fclose(f);

	interpolation_factor=0;
	logic_time_period_inv=1/100.0f;
	CalcIsShadowMap();
}

cVisGeneric::~cVisGeneric()
{
	vector<EffectLibrary*>::iterator it;
	FOR_EACH(effect_library,it)
		delete (*it);

	Done3dxshader();
	ReleaseShaders();
	RELEASE(ObjLibrary); 
	delete Lib3dx;
	ClearData();
	gb_VisGeneric=0;
	MTDONE(lock_effect_library);
//	SaveKindObjNotFree();
}

void cVisGeneric::SetDrawMeshBound(bool p)
{
	Option_DrawMeshBound=p;
}
void cVisGeneric::SetDrawMeshBoundWire(bool p)
{
	Option_DrawMeshBoundWire=p;
}
void cVisGeneric::SetMipMapLevel(int p)
{
	Option_MipMapLevel=p;
}
void cVisGeneric::SetMipMapBlur(int p)
{
	Option_MipMapBlur=p;
}
void cVisGeneric::SetDrawNumberPolygon(bool p)
{
	Option_DrawNumberPolygon=p;
}
void cVisGeneric::SetNearDistanceLOD(float p)
{
	Option_NearDistanceLOD=CONVERT_PROCENT(p, 0.1f, 0.01f);
}
void cVisGeneric::SetFarDistanceLOD(float p)
{
	Option_FarDistanceLOD=CONVERT_PROCENT(p, 2.0f, 0.0f);
}

void cVisGeneric::SetDrawMeshScreen(bool p)
{
	Option_DrawMeshScreen=p;
}
void cVisGeneric::SetMapLevel(float p)
{
	Option_MapLevel=CONVERT_PROCENT(p, 0.1f, 0.8f);
}
void cVisGeneric::SetShowRenderTextureDBG(bool p)
{
	Option_ShowRenderTextureDBG=p;
}

void cVisGeneric::SetDebugShowShadowVolume(int p)
{
	Option_DebugShowShadowVolume=p;
}

void cVisGeneric::SetFavoriteLoadDDS(bool p)
{
	Option_FavoriteLoadDDS=p;
}

void cVisGeneric::SetShadowType(eShadowType p,int shadow_size)
{
	Option_ShadowType=p;
	Option_DrawMeshShadow=shadow_size;
	CalcIsShadowMap();

	if(gb_RenderDevice3D)
		gb_RenderDevice3D->SetAdvance();
}


//////////////////////////////////////////////////////////////////////////////////////////
cScene* cVisGeneric::CreateScene()
{
	cScene *Scene=new cScene;
	Scene->SetObjLibrary(ObjLibrary);
	return Scene;
}
//////////////////////////////////////////////////////////////////////////////////////////
void cVisGeneric::SetData(cInterfaceRenderDevice *pData)
{ // функция для работы с окном вывода
	cURenderDevice *IRenderDevice=(cURenderDevice*)pData;
	VISASSERT(IRenderDevice&&IRenderDevice->GetKind(KIND_UI_RENDERDEVICE));
	InitShaders();
	Init3dxshader();
}
void cVisGeneric::ClearData()
{ // функция для работы с окном вывода
	vector<cFontInternal*>::iterator it;
	FOR_EACH(fonts,it)
		(*it)->Release();
	fonts.clear();
}
//////////////////////////////////////////////////////////////////////////////////////////
cTexture* cVisGeneric::CreateTexture(const char *TextureName)
{
	return GetTexLibrary()->GetElement(TextureName,"NoMipMap NoBlur");
}

cTextureScale* cVisGeneric::CreateTextureScale(const char *TextureName,Vect2f scale)
{
	return GetTexLibrary()->GetElementScale(TextureName,scale);
}

cTexture* cVisGeneric::CreateRenderTexture(int width,int height,int attr,bool enable_assert)
{
	return GetTexLibrary()->CreateRenderTexture(width,height,attr,enable_assert);
}

cTexture* cVisGeneric::CreateTexture(int sizex,int sizey,bool alpha)
{
	return GetTexLibrary()->CreateTexture(sizex,sizey,alpha);
}

cTexture* cVisGeneric::CreateTextureDefaultPool(int sizex,int sizey,bool alpha)
{
	return GetTexLibrary()->CreateTextureDefaultPool(sizex,sizey,alpha);
}

cTexture* cVisGeneric::CreateBumpTexture(int sizex,int sizey)
{
	cTexture *Texture=new cTexture;

	Texture->BitMap.resize(1);
	Texture->BitMap[0]=0;
	Texture->SetWidth(sizex);
	Texture->SetHeight(sizey);

	if(GetRenderDevice()->CreateBumpTexture(Texture))
	{
		delete Texture;
		return NULL;
	}

	return Texture;
}

//////////////////////////////////////////////////////////////////////////////////////////
void cVisGeneric::CalcIsShadowMap()
{
	Option_IsShadowMap=Option_ShadowType==SHADOW_MAP ||
		Option_ShadowType==SHADOW_MAP_SELF ||
		Option_ShadowType==SHADOW_MAP_PERSPECTIVE;
}

//////////////////////////////////////////////////////////////////////////////////////////

cURenderDevice* cVisGeneric::GetRenderDevice()
{
	return gb_RenderDevice;
}

eShadowType cVisGeneric::GetShadowType()
{
	return (eShadowType)(int)Option_ShadowType;
}

////////////////////DebugMemInfo
#include <crtdbg.h>
struct _CrtMemBlockHeader{
// Pointer to the block allocated just before this one:
   struct _CrtMemBlockHeader *pBlockHeaderNext; 
// Pointer to the block allocated just after this one:
   struct _CrtMemBlockHeader *pBlockHeaderPrev; 
   char *szFileName;   // File name   
   int nLine;          // Line number
   size_t nDataSize;   // Size of user block
   int nBlockUse;      // Type of block
   long lRequest;      // Allocation number
// Buffer just before (lower than) the user's memory:
//   unsigned char gap[nNoMansLandSize];
};


int __cdecl MyAllocHook(
   int      nAllocType,
   void   * pvData,
   size_t   nSize,
   int      nBlockUse,
   long     lRequest,
   const unsigned char * szFileName,
   int      nLine
   )
{
	if(nSize==3096)
	{
		int k=0;
	}
	
	return true;
}

void InitAllockHook()
{
	_CrtSetAllocHook(MyAllocHook);
}

void DebugMemInfo()
{
#ifdef _DEBUG
	_CrtMemState pc;
	_CrtMemCheckpoint(&pc);
	_CrtMemBlockHeader* pm=pc.pBlockHeader;

	const int block_size=12;
	const int min_block_shl=4;
	struct INFO
	{
		int num;
		int size;
	};

	INFO num[block_size];
	for(int i=0;i<block_size;i++)
	{
		num[i].num=0;
		num[i].size=0;
	}

	while(pm)
	{
		if(pm->nBlockUse==_FREE_BLOCK)continue;

		int sz=pm->nDataSize>>min_block_shl;
		int out=0;
		while(sz)
		{
			out++;
			sz=sz>>1;
		}

		if(out>block_size)
			out=block_size-1;

		num[out].num++;
		num[out].size+=pm->nDataSize;

		pm=pm->pBlockHeaderNext;
	}

	static char text[2048];
	char* p=text;

	int summary_size=0,summary_num=0;
	int i;
	for(i=0;i<block_size;i++)
	{
		INFO& n=num[i];
		p+=sprintf(p,"% 6i num=% 5i, size=%i.\n",(1<<(min_block_shl+i)),n.num,n.size);
		summary_size+=n.size;
		summary_num+=n.num;
	}

	p+=sprintf(p,"summary size=%i.\n",summary_size);
	p+=sprintf(p,"summary num=%i.\n",summary_num);
	MessageBox(gb_RenderDevice->GetWindowHandle(),text,"Memory info",MB_OK);
#endif _DEBUG
}


void dprintf(char *format, ...)
{
  va_list args;
  char    buffer[512];

  va_start(args,format);

  //strcpy(buffer + vsprintf(buffer,format,args), "\r\n");
  vsprintf(buffer,format,args);

  OutputDebugString(buffer);
}


void cVisGeneric::SetShowType(eShowType type,bool show)
{
	Option_ShowType[type]=show;
}

void cVisGeneric::XorShowType(eShowType type)
{
	Option_ShowType[type]=!Option_ShowType[type];
}

bool cVisGeneric::GetShowType(eShowType type)
{
	return Option_ShowType[type];
}

bool cVisGeneric::PossibilityBump()
{
	return gb_RenderDevice3D->PossibilityBump();
}

void cVisGeneric::SetEnableBump(bool enable)
{
	Option_EnableBump=enable;
	gb_RenderDevice3D->SetAdvance();
}

bool cVisGeneric::GetEnableBump()
{
	return Option_EnableBump;
}

void cVisGeneric::SetEffectLibraryPath(const char* effect_path_,const char* texture_path)
{
	effect_path=effect_path_;
	if(!effect_path.empty())
	{
		char c=effect_path[effect_path.size()-1];
		if(c!='\\' && c!='/')
		{
			effect_path+='\\';
		}
	}

	effect_texture_path=texture_path;
}

EffectLibrary* cVisGeneric::GetEffectLibrary(const char* filename,bool quiet)
{
	MTEnter mtenter(lock_effect_library);
	string fname=effect_path+filename+".effect";

	EffectLibrary* lib=NULL;
	vector<EffectLibrary*>::iterator it;
	FOR_EACH(effect_library,it)
	if(stricmp(fname.c_str(),(*it)->GetFileName())==0)
	{
		lib=*it;
		break;
	}
	if(lib)
		return lib;
	lib=new EffectLibrary;
	if(!lib->Load(fname.c_str(),effect_texture_path.c_str()))
	{
		if(!quiet)
			VisError<<"Cannot load EffectLibrary\r\n"<<filename<<VERR_END;
		delete lib;
		return NULL;
	}
	effect_library.push_back(lib);
	return lib;
}

void cVisGeneric::SetLinkEffectToModel(bool link)
{
	Option_EnableLinkEffectToModel=link;
}

void cVisGeneric::SetAnisotropic(bool b)
{
	gb_RenderDevice3D->SetAnisotropic(b);
}

bool cVisGeneric::GetAnisotropic()
{
	return gb_RenderDevice3D->GetAnisotropic();
}

void cVisGeneric::EnableOcclusion(bool b)
{
	Option_EnableOcclusion=b;
	if(gb_RenderDevice3D)
	{
		if(!gb_RenderDevice3D->ReinitOcclusion())
			Option_EnableOcclusion=false;
	}
}

bool cVisGeneric::PossibilityOcclusion()
{
	return gb_RenderDevice3D->PossibilityOcclusion();
}

bool cVisGeneric::IsEnableOcclusion()
{
	return Option_EnableOcclusion;
}

void cVisGeneric::SetOcclusionFadeTime(float f)
{
	VISASSERT(f>0.1f);
	Option_InvOcclusionFadeTime=1/(f*1000);
}


void cVisGeneric::EnablePointLight(bool enable)
{
	Option_EnablePointLight=enable;
}
bool cVisGeneric::IsEnablePointLight()
{
	return Option_EnablePointLight;
}


cTexture* cVisGeneric::CreateTextureScreen()
{
	LPDIRECT3DDEVICE9 device=gb_RenderDevice3D->lpD3DDevice;
	HRESULT hr;
	int dx=gb_RenderDevice3D->GetSizeX();
	int dy=gb_RenderDevice3D->GetSizeY();

	int dx_plain_surface=dx;
	int dy_plain_surface=dy;
	if(!gb_RenderDevice3D->IsFullScreen())
	{
		dx_plain_surface=GetSystemMetrics(SM_CXSCREEN);
		dy_plain_surface=GetSystemMetrics(SM_CYSCREEN);
	}

	IDirect3DSurface9* sys_surface=NULL;
	hr=device->CreateOffscreenPlainSurface(
		dx_plain_surface,
		dy_plain_surface,
		D3DFMT_A8R8G8B8,
		D3DPOOL_SYSTEMMEM,
		&sys_surface,
		NULL
	);
	if(FAILED(hr))
	{
		return NULL;
	}

	hr=device->GetFrontBufferData(0,sys_surface);
	if(FAILED(hr))
	{
		RELEASE(sys_surface);
		return NULL;
	}

	int dx_real=Power2up(dx);
	int dy_real=Power2up(dy);

	cTexture* pTextureBackground=CreateTexture(dx_real,dy_real,false);

	D3DLOCKED_RECT lock_in;
	hr=sys_surface->LockRect(&lock_in,NULL,0);
	VISASSERT(SUCCEEDED(hr));
	int out_pitch=0;
	BYTE* out_data=pTextureBackground->LockTexture(out_pitch);

	for(int y=0;y<dy;y++)
	{
		BYTE* in=lock_in.Pitch*y+(BYTE*)lock_in.pBits;
		BYTE* out=out_pitch*y+out_data;
		memcpy(out,in,dx*4);
	}

	sys_surface->UnlockRect();
	pTextureBackground->UnlockTexture();

	RELEASE(sys_surface);

	return pTextureBackground;
}

void cVisGeneric::SetGlobalParticleRate(float r)
{
	xassert(r>=0 && r<=1);
	Option_ParticleRate=r;
}

bool cVisGeneric::PossibilityShadowMapSelf4x4()
{
	if(gb_RenderDevice3D->dtAdvanceOriginal)
	{
		eDrawID id=gb_RenderDevice3D->dtAdvanceOriginal->GetID();
		return id==DT_RADEON9700 || id==DT_GEFORCEFX;
	}
	return false;
}

void cVisGeneric::SetShadowMapSelf4x4(bool b4x4)
{
	Option_ShadowMapSelf4x4=b4x4;
	gb_RenderDevice3D->RestoreShader();
}

bool cVisGeneric::PossibilityBumpChaos()
{
	return gb_RenderDevice3D->DeviceCaps.TextureOpCaps|D3DTEXOPCAPS_BUMPENVMAP;
}


void cVisGeneric::SetFontRootDirectory(const char* dir)
{
	font_root_directory=dir;
}

void cVisGeneric::SetFontDirectory(const char* dir)
{
	font_directory=dir;
}

void cVisGeneric::ReloadAllFont()
{
	MTG();
	for(int i=0;i<fonts.size();i++)
	{
		fonts[i]->Reload(font_root_directory.c_str(),font_directory.c_str());
	}
}

cFont* cVisGeneric::CreateFont(const char *TextureFileName,int h,bool silentErr)
{
	if(TextureFileName==0||TextureFileName[0]==0) return NULL;

	vector<cFontInternal*>::iterator it;
	FOR_EACH(fonts,it)
	{
		cFontInternal* f=*it;
		if(_stricmp(f->font_name.c_str(),TextureFileName)==0 && 
			f->GetStatementHeight()==h)
		{
			return new cFont(f);
		}
	}

	cFontInternal *UObj=new cFontInternal;
	if(!UObj->Create(font_root_directory.c_str(),font_directory.c_str(),TextureFileName,h,silentErr))
	{
		delete UObj;
		return NULL;
	}

	fonts.push_back(UObj);
	return new cFont(UObj);
}

cFont* cVisGeneric::CreateDebugFont()
{
	if(fonts.empty())
		return NULL;

	int ioptimal=0;
	float dhoptimal=1000;
	for(int i=0;i<fonts.size();i++)
	{
		float h=fonts[i]->GetHeight();
		float dh=fabsf(h-15);
		if(dh<dhoptimal)
		{
			dhoptimal=dh;
			ioptimal=i;
		}
	}

	return new cFont(fonts[ioptimal]);
}

MTTexObjAutoLock::MTTexObjAutoLock()
:object_lock(gb_VisGeneric->GetObjLib()->GetLock()),
 texture_lock(GetTexLibrary()->GetLock())
{
}

void cVisGeneric::SetShadowHint(int hint)
{
	Option_ShadowHint=hint;
	gb_RenderDevice3D->RestoreShader();
}

int cVisGeneric::GetShadowHint()
{
	return Option_ShadowHint;
}


bool GetAllTriangle(const char* filename,vector<Vect3f>& point,vector<sPolygon>& polygon)
{
	VISASSERT(gb_VisGeneric);
	cObjectNodeRoot* root=gb_VisGeneric->GetObjLib()->GetElement(filename,NULL);
	if(!root)
		return false;
	root->GetAllTriangle(point,polygon);
	root->Release();
	return true;
}

cTexture* cVisGeneric::CreateTextureU16V16(int sizex,int sizey,bool deafultpool)
{
	cTexture *Texture=new cTexture;
	Texture->SetNumberMipMap(1);
	Texture->SetAttribute(TEXTURE_U16V16);
	if(deafultpool)
		Texture->SetAttribute(TEXTURE_D3DPOOL_DEFAULT);

	Texture->BitMap.resize(1);
	Texture->BitMap[0]=0;
	Texture->SetWidth(sizex);
	Texture->SetHeight(sizey);

	if(gb_RenderDevice3D->CreateTextureU16V16(Texture,deafultpool))
	{
		delete Texture;
		return NULL;
	}

	return Texture;
}