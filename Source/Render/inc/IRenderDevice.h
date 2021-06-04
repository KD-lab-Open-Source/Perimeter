#pragma once

#include "unknown.h"

enum eModeRenderDevice
{
	RENDERDEVICE_MODE_WINDOW	=	1<<0,	// вывод в окно

	//d3d only
	RENDERDEVICE_MODE_RGB16		=	1<<8,	// 16 битный цвет
	RENDERDEVICE_MODE_RGB32		=	1<<9,	// 32 битный цвет
	RENDERDEVICE_MODE_COMPRESS	=	1<<10,	// компресованные текстуры
	RENDERDEVICE_MODE_VSYNC		=	1<<11,	// использовать вертикальную синхронизацию
	RENDERDEVICE_MODE_STRENCIL	=	1<<12,
	RENDERDEVICE_MODE_Z24		=	1<<13,	// 24 битный  
	RENDERDEVICE_MODE_REF		=	1<<14,
	RENDERDEVICE_MODE_MULTITHREAD = 1<<15,
	RENDERDEVICE_MODE_ALPHA		=   1<<16,  //При создании rendertarget использовать alpha 

	//Internal
	RENDERDEVICE_MODE_RETURNERROR=	1<<17, //Только для ChangeSize
	RENDERDEVICE_MODE_ONEBACKBUFFER=1<<18,
};

enum eRenderStateOption
{
	RS_ZENABLE                  = 7,    /* or TRUE/FALSE for legacy */
	RS_FILLMODE                 = 8,    /* FILL_POINT, FILL_WIREFRAME or FILL_SOLID */
	RS_ZWRITEENABLE             = 14,   /* TRUE to enable z writes */
    RS_ZFUNC                    = 23,   /* CMPFUNC */
    RS_CULLMODE                 = 22,   /* D3DCULL */
	RS_ALPHAREF                 = 24,   /* FIXED */
	RS_ALPHAFUNC				= 25,	/* CMPFUNC */
    RS_DITHERENABLE             = 26,   /* TRUE to enable dithering */
    RS_ALPHABLENDENABLE         = 27,   /* TRUE to enable alpha blending */
    RS_FOGENABLE                = 28,   /* TRUE to enable fog blending */
    RS_FOGCOLOR                 = 34,   /* D3DCOLOR */
    RS_FOGSTART                 = 36,   /* Fog start (for both vertex and pixel fog) */
    RS_FOGEND                   = 37,   /* Fog end      */
	RS_BILINEAR					= 1024,	/* SetTextureStageState(0, D3DTSS_MINFILTER|D3DTSS_MAGFILTER, value ) */
};

enum eFillMode
{
    FILL_POINT					= 1,
    FILL_WIREFRAME				= 2,
    FILL_SOLID					= 3, 
};

enum eCMPFUNC
{
    CMP_NEVER					= 1,
    CMP_LESS					= 2,
    CMP_EQUAL					= 3,
    CMP_LESSEQUAL				= 4,
    CMP_GREATER					= 5,
    CMP_NOTEQUAL				= 6,
    CMP_GREATEREQUAL			= 7,
    CMP_ALWAYS					= 8,
};

enum eColorMode
{
	COLOR_MOD,
	COLOR_ADD,
	COLOR_MOD2,
	COLOR_MOD4,
};
enum eBlendMode
{
	ALPHA_NONE,
	ALPHA_TEST,
	ALPHA_ADDBLENDALPHA,
	ALPHA_BLEND,
	ALPHA_ADDBLEND,
	ALPHA_SUBBLEND,
	ALPHA_MUL,
};

enum PRIMITIVETYPE 
{
    PT_POINTLIST = 1,
    PT_LINELIST = 2,
    PT_LINESTRIP = 3,
    PT_TRIANGLELIST = 4,
    PT_TRIANGLESTRIP = 5,
    PT_TRIANGLEFAN = 6,
};

struct sPolygon
{
	WORD p1,p2,p3;
	WORD& operator[](int i){return *(i+&p1);}
	inline void set(WORD i1,WORD i2,WORD i3){ p1=i1; p2=i2; p3=i3; }
};

struct sPtrVertexBuffer
{
	struct sSlotVB*	ptr;
	int			size;
	sPtrVertexBuffer()		{ptr=NULL; size=0;}
	~sPtrVertexBuffer()		{Destroy();};
	inline bool IsInit()	{ return ptr!=NULL; }
	void Destroy();
};
struct sPtrIndexBuffer
{
	struct sSlotIB*	ptr;
	sPtrIndexBuffer()		{ ptr=NULL; }
	~sPtrIndexBuffer();
	inline int IsInit()		{ return ptr!=NULL; }
};

#include "VertexFormat.h"
#include "..\D3D\VertexBuffer.h"

class cFont;
class cTexture;
class cTextureScale;

class cInterfaceRenderDevice : public cUnknownClass
{
public:
	cInterfaceRenderDevice():cUnknownClass(KIND_UI_RENDERDEVICE)	{ }
	~cInterfaceRenderDevice()													{ }

	virtual int Init(int xScr,int yScr,int mode,void *hWnd=0,int RefreshRateInHz=0)	{ return -1; }
	virtual bool ChangeSize(int xScr,int yScr,int mode){return false;};
	virtual int GetClipRect(int *xmin,int *ymin,int *xmax,int *ymax)			{ return -1; }
	virtual int SetClipRect(int xmin,int ymin,int xmax,int ymax)				{ return -1; }
	virtual void SetDrawTransform(class cCamera *pDrawNode)						{ }
	virtual bool CheckDeviceType(int xscr,int yscr,int Mode)=0;

	virtual bool IsFullScreen() {return false;}
	
	virtual int Done()															{ return -1; } // закрытие графического режима

	virtual int BeginScene()													{ return -1; }
	virtual int EndScene()														{ return -1; }
	virtual int Fill(int r,int g,int b,int a=255)								{ return -1; }
	virtual int Flush(HWND wnd=NULL)											{ return -1; }
	virtual int SetGamma(float fGamma,float fStart=0.f,float fFinish=1.f)		{ return -1; }

	// вспомогательные функции, могут быть не реализованы
	virtual int	GetSizeX()														{ return 0; }
	virtual int	GetSizeY()														{ return 0; }
	virtual void* GetStripBuffer()												{	return NULL;}
	virtual int GetStripBufferLen()												{	return 0;}
	virtual int GetInfo(int *graph3d)											{ return 0; }
	virtual void DrawLine(int x1,int y1,int x2,int y2,sColor4c color)			{ }
	virtual void DrawPixel(int x1,int y1,sColor4c color)						{ }
	virtual void DrawRectangle(int x,int y,int dx,int dy,sColor4c color,bool outline=false) { }
	virtual void FlushPrimitive2D()												{ }
	virtual void DrawLine(const Vect3f &v1,const Vect3f &v2,sColor4c color)		{ }
	virtual void DrawPoint(const Vect3f &v1,sColor4c color)						{ }
	virtual void FlushPrimitive3D()												{ }

	virtual void SetFont(cFont *pFont)=0;
	virtual void SetDefaultFont(cFont *pFont)=0;
	virtual float GetFontLength(const char *string)									{ return -1; }
	virtual float GetCharLength(const char c)=0;

	/*Внутренний мтод. Использовать с крайней осторожностью.
	  Перед использованием посмотреть как используется внутри Render.
	*/
	virtual void ChangeTextColor(const char* &str,sColor4c& diffuse)=0;


	virtual void OutText(int x,int y,const char *string,const sColor4f& color,int align=-1,eBlendMode blend_mode=ALPHA_BLEND)	{ }
	virtual void OutTextRect(int x,int y,const char *string,int align,Vect2f& bmin,Vect2f& bmax)	{ }
	virtual void OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
		                 cTexture* pTexture,eColorMode mode,
						 Vect2f uv,//Координаты текстуры в точке x,y
						 Vect2f duv,//du,dv на один логический пиксель 
						//(лог пиксель равен графическому в разрешении 1024x768)
						float phase=0,
						float lerp_factor=1//0..1 Насколько сильно влияет pTexture
						 ) { }


	virtual void OutText(int x,int y,const char *string,int r=255,int g=255,int b=255)	{ }
	virtual void OutText(int x,int y,const char *string,int r,int g,int b,char *FontName/*="Arial"*/,int size=12,int bold=0,int italic=0,int underline=0) { }
	virtual HWND GetWindowHandle()												{ return 0; }
	virtual bool SetScreenShot(const char *fname)										{ return false; }
	virtual int SetRenderState(eRenderStateOption option,int value)				{ return -1; }
	virtual void DrawBound(const MatXf &Matrix,Vect3f &min,Vect3f &max,bool wireframe=0,sColor4c &Color=sColor4c(255,255,255,255))	{ }
	virtual void DrawSprite(int x,int y,int dx,int dy,float u,float v,float du,float dv,
		cTexture *Texture,sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0,eBlendMode mode=ALPHA_NONE)	{ }
	virtual void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,
		cTexture *Tex1,cTexture *Tex2,sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0)	{ }
	virtual void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
		cTexture *Tex1,cTexture *Tex2,sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE)	{ }
	virtual void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
		cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha=1,float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE)	{ }
	
	virtual void DrawSpriteScale(int x,int y,int dx,int dy,float u,float v,
		cTextureScale *Texture,sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0,eBlendMode mode=ALPHA_NONE) { }
	virtual void DrawSpriteScale2(int x,int y,int dx,int dy,float u,float v,
		cTextureScale *Tex1,cTextureScale *Tex2,sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0) { }
	virtual void DrawSpriteScale2(int x,int y,int dx,int dy,float u,float v,float u1,float v1,
		cTextureScale *Tex1,cTextureScale *Tex2,sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0,eColorMode mode=COLOR_MOD) { }

	virtual int GetDrawNumberPolygon()											{return 0;}

	virtual int DeleteTexture(class cTexture *Texture)							{ return -1; }

	virtual void SetGlobalFog(const sColor4f &color,const Vect2f &v)			{}

	virtual bool IsEnableSelfShadow()											{return false;}

	virtual cVertexBuffer<sVertexXYZDT1>* GetBufferXYZDT1(){return NULL;};
	virtual cVertexBuffer<sVertexXYZD>* GetBufferXYZD(){return NULL;};
	virtual cVertexBuffer<sVertexXYZWD>* GetBufferXYZWD(){return NULL;};
	virtual cQuadBuffer<sVertexXYZDT1>* GetQuadBufferXYZDT1(){return NULL;}
	virtual cVertexBuffer<sVertexXYZDT2>* GetBufferXYZDT2(){return NULL;};
	virtual cVertexBuffer<sVertexXYZWDT1>* GetBufferXYZWDT1(){return NULL;};
	virtual cVertexBuffer<sVertexXYZWDT2>* GetBufferXYZWDT2(){return NULL;};

	virtual void SetNoMaterial(eBlendMode blend,float Phase=0,cTexture *Texture0=0,cTexture *Texture1=0,eColorMode color_mode=COLOR_MOD)=0;
	virtual void SetWorldMatrix(const MatXf& pos)=0;
	virtual void DrawIndexedPrimitive(sPtrVertexBuffer &vb,int OfsVertex,int nVertex,const sPtrIndexBuffer& ib,int nOfsPolygon,int nPolygon)=0;

	virtual void CreateVertexBuffer(struct sPtrVertexBuffer &vb,int NumberVertex,int fmt,int dynamic=0)=0;
	virtual void DeleteVertexBuffer(struct sPtrVertexBuffer &vb)=0;
	virtual void* LockVertexBuffer(struct sPtrVertexBuffer &vb)=0;
	virtual void UnlockVertexBuffer(struct sPtrVertexBuffer &vb)=0;
	virtual void CreateIndexBuffer(sPtrIndexBuffer& ib,int NumberIndex,int size=sizeof(sPolygon))=0;
	virtual void DeleteIndexBuffer(struct sPtrIndexBuffer &ib)=0;
	virtual sPolygon* LockIndexBuffer(struct sPtrIndexBuffer &ib)=0;
	virtual void UnlockIndexBuffer(struct sPtrIndexBuffer &ib)=0;
};

cInterfaceRenderDevice* CreateIRenderDevice();


//Из RenderDevice.h
enum eSurfaceFormat
{
	SURFMT_COLOR		=	0,
	SURFMT_COLORALPHA,
	SURFMT_RENDERMAP16,
	SURFMT_RENDERMAP32,
	SURFMT_BUMP,
	SURFMT_COLOR32,
	SURFMT_COLORALPHA32,
	SURFMT_RENDERMAP_FLOAT,
	SURFMT_GRAYALPHA,//A8L8 - в лудшем случае
	SURFMT_UV,
	SURFMT_U16V16,
	SURFMT_NUMBER
};

bool GetAllTriangle(const char* filename,vector<Vect3f>& point,vector<sPolygon>& polygon);
