#pragma once

#include <cstdint>
#include "Unknown.h"
#include "VertexFormat.h"

enum eRenderDeviceSelection {
    DEVICE_D3D9,
    DEVICE_HEADLESS
};

//Возвращает минимальное число,являющееся степенью двойки и не меньше, чем n
inline int Power2up(int n)
{
    int i=1;
    while(i<n)
        i=i<<1;
    return i;
}

struct sSlotVB
{
    void* p;
    char dynamic;
    bool init;
    int fmt;
    int VertexSize;
    int NumberVertex;
};
struct sSlotIB
{
    void* p;
    bool init;
};
struct sPtrVertexBuffer
{
    sSlotVB* ptr = nullptr;
    sPtrVertexBuffer() = default;
    ~sPtrVertexBuffer() {
        Destroy();
    }
    inline bool IsInit() {
        return ptr!=nullptr;
    }
    void Destroy();
};

struct sPtrIndexBuffer
{
    sSlotIB* ptr = nullptr;
    sPtrIndexBuffer() = default;
    ~sPtrIndexBuffer();
    inline bool IsInit() {
        return ptr!=nullptr;
    }
};

struct sTextureFormatData
{
    int rBitCount,gBitCount,bBitCount,aBitCount;
    int rBitShift,gBitShift,bBitShift,aBitShift;
    int BytePerPixel;
    uint32_t FormatID;

    sTextureFormatData& Set(int bpp,int rcount,int gcount,int bcount,int acount,int rshift,int gshift,int bshift,int ashift, uint32_t formatID=-1)
    {
        BytePerPixel=bpp;
        rBitCount=rcount; gBitCount=gcount; bBitCount=bcount; aBitCount=acount;
        rBitShift=rshift; gBitShift=gshift;	bBitShift=bshift; aBitShift=ashift;
        FormatID = formatID;
        return *this;
    }

    void Get(int &bpp,int &rcount,int &gcount,int &bcount,int &acount,int &rshift,int &gshift,int &bshift,int &ashift)
    {
        bpp=BytePerPixel;
        rcount=rBitCount; gcount=gBitCount; bcount=bBitCount; acount=aBitCount;
        rshift=rBitShift; gshift=gBitShift;	bshift=bBitShift; ashift=aBitShift;
    }
};

#ifdef PERIMETER_D3D9
#include "../D3D/VertexBuffer.h"
#endif

class cIUnkClass;
class cObjMesh;

void BuildMipMap(int x,int y,int bpp,int bplSrc,void *pSrc,int bplDst,void *pDst,
                 int rc,int gc,int bc,int ac,int rs,int gs,int bs,int as,int Blur=0);

enum TILEMAP_DRAW
{
    TILEMAP_ALL,
    TILEMAP_ZEROPLAST,
    TILEMAP_NOZEROPLAST,
};

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

struct sPolygon
{
    uint16_t p1,p2,p3;
    uint16_t& operator[](int i){return *(i+&p1);}
    inline void set(uint16_t i1,uint16_t i2,uint16_t i3){ p1=i1; p2=i2; p3=i3; }
};

class cFont;
class cTexture;
class cTextureScale;
class cTexLibrary;
class cCamera;

class cInterfaceRenderDevice : public cUnknownClass
{
protected:
    cTexLibrary* TexLibrary;
    cFont* CurrentFont;
    cFont* DefaultFont;
    cCamera *DrawNode;
    uint32_t RenderMode;

public:
    cInterfaceRenderDevice();
    ~cInterfaceRenderDevice() override;
    
    // Common methods

    cTexLibrary* GetTexLibrary() { return TexLibrary; }
    cTexture* GetTexture(int n);

    eModeRenderDevice GetRenderMode() { return static_cast<eModeRenderDevice>(RenderMode); }

    inline cCamera* GetDrawNode() { return DrawNode; }
    virtual void SetDrawNode(cCamera* node) { DrawNode = node; };

    virtual void SetFont(cFont *pFont);
    virtual void SetDefaultFont(cFont *pFont);
    virtual float GetFontLength(const char *string);
    virtual float GetCharLength(const char c);
    
    // Decl only methods

    virtual eRenderDeviceSelection GetRenderSelection() const = 0;

#ifdef PERIMETER_D3D9
    virtual cQuadBuffer<sVertexXYZDT1>* GetQuadBufferXYZDT1() = 0;
    virtual cVertexBuffer<sVertexXYZD>* GetBufferXYZD() = 0;
#endif

    virtual int Init(int xScr,int yScr,int mode,void *hWnd=0,int RefreshRateInHz=0) = 0;
    virtual bool ChangeSize(int xScr,int yScr,int mode) = 0;
    virtual int GetClipRect(int *xmin,int *ymin,int *xmax,int *ymax) = 0;
    virtual int SetClipRect(int xmin,int ymin,int xmax,int ymax) = 0;
    virtual void SetDrawTransform(class cCamera *pDrawNode) = 0;

    virtual bool IsFullScreen() = 0;

    virtual int Done() = 0;

    virtual int BeginScene() = 0;
    virtual int EndScene() = 0;
    virtual int Fill(int r,int g,int b,int a=255) = 0;
    virtual int Flush(bool wnd=false) = 0;
    virtual int SetGamma(float fGamma,float fStart=0.f,float fFinish=1.f) = 0;

    // вспомогательные функции, могут быть не реализованы
    virtual int	GetSizeX() = 0;
    virtual int	GetSizeY() = 0;
    virtual void* GetStripBuffer() = 0;
    virtual int GetStripBufferLen() = 0;
    virtual void DrawLine(int x1,int y1,int x2,int y2,sColor4c color) = 0;
    virtual void DrawPixel(int x1,int y1,sColor4c color) = 0;
    virtual void DrawRectangle(int x,int y,int dx,int dy,sColor4c color,bool outline=false) = 0;
    virtual void FlushPrimitive2D() = 0;
    virtual void DrawLine(const Vect3f &v1,const Vect3f &v2,sColor4c color) = 0;
    virtual void DrawPoint(const Vect3f &v1,sColor4c color) = 0;
    virtual void FlushPrimitive3D() = 0;

    /*Внутренний мтод. Использовать с крайней осторожностью.
      Перед использованием посмотреть как используется внутри Render.
    */
    virtual void ChangeTextColor(const char* &str,sColor4c& diffuse) = 0;

    virtual void OutText(int x,int y,const char *string,const sColor4f& color,int align=-1,eBlendMode blend_mode=ALPHA_BLEND) = 0;
    virtual void OutTextRect(int x,int y,const char *string,int align,Vect2f& bmin,Vect2f& bmax) = 0;
    virtual void OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
                         cTexture* pTexture,eColorMode mode,
                         Vect2f uv,//Координаты текстуры в точке x,y
                         Vect2f duv,//du,dv на один логический пиксель 
            //(лог пиксель равен графическому в разрешении 1024x768)
                         float phase=0,
                         float lerp_factor=1//0..1 Насколько сильно влияет pTexture
    ) = 0;


    virtual void OutText(int x,int y,const char *string,int r=255,int g=255,int b=255) = 0;
    virtual void OutText(int x,int y,const char *string,int r,int g,int b,char *FontName/*="Arial"*/,int size=12,int bold=0,int italic=0,int underline=0) = 0;
    virtual HWND GetWindowHandle() = 0;
    virtual bool SetScreenShot(const char *fname) = 0;
    virtual int SetRenderState(eRenderStateOption option,int value) = 0;
    virtual void DrawBound(const MatXf &Matrix,Vect3f &min,Vect3f &max,bool wireframe=0,const sColor4c& Color=sColor4c(255,255,255,255)) = 0;

    virtual int Create(class cTileMap *TileMap) = 0;
    virtual void PreDraw(cTileMap *TileMap) = 0;
    virtual void Draw(cTileMap *TileMap,eBlendMode MatMode,TILEMAP_DRAW tile_draw,bool shadow) = 0;
    virtual int Delete(class cTileMap *TileMap) = 0;
    
    virtual void DrawSprite(int x,int y,int dx,int dy,float u,float v,float du,float dv,
                            cTexture *Texture,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0,eBlendMode mode=ALPHA_NONE) = 0;
    virtual void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,
                             cTexture *Tex1,cTexture *Tex2,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0) = 0;
    virtual void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
                             cTexture *Tex1,cTexture *Tex2,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE) = 0;
    virtual void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
                             cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha=1,float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE) = 0;

    virtual void DrawSpriteScale(int x,int y,int dx,int dy,float u,float v,
                                 cTextureScale *Texture,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0,eBlendMode mode=ALPHA_NONE) = 0;
    virtual void DrawSpriteScale2(int x,int y,int dx,int dy,float u,float v,
                                  cTextureScale *Tex1,cTextureScale *Tex2,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0) = 0;
    virtual void DrawSpriteScale2(int x,int y,int dx,int dy,float u,float v,float u1,float v1,
                                  cTextureScale *Tex1,cTextureScale *Tex2,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0,eColorMode mode=COLOR_MOD) = 0;

    virtual void Draw(class cScene *Scene) = 0;

    virtual void Draw(class FieldDispatcher *ffd) = 0;
    virtual void CreateFFDData(class FieldDispatcher *rd) = 0;
    virtual void DeleteFFDData(class FieldDispatcher *rd) = 0;
    virtual void Draw(class ElasticSphere *es) = 0;

    virtual int GetDrawNumberPolygon() = 0;

    virtual int CreateTexture(class cTexture *Texture,class cFileImage *FileImage,int dxout,int dyout,bool enable_assert=true) = 0;
    virtual int DeleteTexture(class cTexture *Texture) = 0;
    virtual void* LockTexture(class cTexture *Texture, int& Pitch) = 0;
    virtual void* LockTexture(class cTexture *Texture, int& Pitch, const Vect2i& lock_min, const Vect2i& lock_size) = 0;
    virtual void UnlockTexture(class cTexture *Texture) = 0;

    virtual int CreateBumpTexture(class cTexture *Texture) = 0;
    
    virtual void SetGlobalFog(const sColor4f &color,const Vect2f &v) = 0;
    virtual void SetGlobalLight(Vect3f *vLight, sColor4f *Ambient = nullptr,
                                sColor4f *Diffuse = nullptr, sColor4f *Specular = nullptr) = 0;

    virtual bool IsEnableSelfShadow() = 0;
    
    virtual void SetNoMaterial(eBlendMode blend,float Phase=0,cTexture *Texture0=0,cTexture *Texture1=0,eColorMode color_mode=COLOR_MOD) = 0;
    virtual void DrawIndexedPrimitive(struct sPtrVertexBuffer &vb,int OfsVertex,int nVertex,const struct sPtrIndexBuffer& ib,int nOfsPolygon,int nPolygon) = 0;

    virtual void CreateVertexBuffer(struct sPtrVertexBuffer &vb,int NumberVertex,int fmt,int dynamic=0) = 0;
    virtual void DeleteVertexBuffer(struct sPtrVertexBuffer &vb) = 0;
    virtual void* LockVertexBuffer(struct sPtrVertexBuffer &vb) = 0;
    virtual void UnlockVertexBuffer(struct sPtrVertexBuffer &vb) = 0;
    virtual void CreateIndexBuffer(struct sPtrIndexBuffer& ib,int NumberIndex,int size=sizeof(sPolygon)) = 0;
    virtual void DeleteIndexBuffer(struct sPtrIndexBuffer &ib) = 0;
    virtual sPolygon* LockIndexBuffer(struct sPtrIndexBuffer &ib) = 0;
    virtual void UnlockIndexBuffer(struct sPtrIndexBuffer &ib) = 0;
};

cInterfaceRenderDevice* CreateIRenderDevice(eRenderDeviceSelection selection);

extern cInterfaceRenderDevice* gb_RenderDevice;

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

enum eMaterialMode
{
    MAT_COLOR_ADD_SPECULAR	=1<<4,

    MAT_ALPHA_ADDBLENDALPHA	=1<<5,
    MAT_ALPHA_BLEND			=1<<6,
    MAT_ALPHA_ADDBLEND		=1<<7,
    MAT_ALPHA_SUBBLEND		=1<<29,
    MAT_BUMP				=1<<9,
    MAT_NORMAL				=1<<10,

    MAT_ALPHA_TEST			=1<<8,

    MAT_IS_BLEND			= MAT_ALPHA_ADDBLENDALPHA|MAT_ALPHA_BLEND|MAT_ALPHA_ADDBLEND|MAT_ALPHA_SUBBLEND,
    // only d3d version
    // render type
    MAT_TEXMATRIX_STAGE1	=1<<16,
    MAT_TEXNORMAL_STAGE2	=1<<18,

    MAT_RENDER_SPHEREMAP	=1<<22,
    MAT_LIGHT				=1<<31
};

struct sDataRenderMaterial
{
    sColor4f	Ambient;
    sColor4f	Diffuse;
    sColor4f	Specular;
    sColor4f	Emissive;
    float		Power;

    float Phase;
    int			mat;//eMaterialMode
    cTexture	*Tex[2];
    MatXf		TexMatrix;
    float		MaterialAnimPhase;

    sDataRenderMaterial()			{ Phase=0; MaterialAnimPhase=0; }
};

bool GetAllTriangle(const char* filename, std::vector<Vect3f>& point, std::vector<sPolygon>& polygon);
