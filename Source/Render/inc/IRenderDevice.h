#pragma once

#include <cstdint>
#include "Unknown.h"
#include "RenderTypes.h"
#include "MemoryResource.h"

struct DrawRange {
    size_t offset = 0;
    size_t len = 0;
};

enum eRenderDeviceSelection {
    DEVICE_D3D9,
    DEVICE_HEADLESS,
    DEVICE_SOKOL
};

//Возвращает минимальное число,являющееся степенью двойки и не меньше, чем n
inline int Power2up(int n)
{
    int i=1;
    while(i<n)
        i=i<<1;
    return i;
}

class VertexBuffer: public MemoryResource {
public:
    union {
        void* ptr = nullptr;
#ifdef PERIMETER_D3D9
        struct IDirect3DVertexBuffer9* d3d;
#endif
#ifdef PERIMETER_SOKOL
        struct SokolBuffer* sg;
#endif
    };
    bool dynamic = false;
    vertex_fmt_t fmt = 0;
    uint16_t VertexSize = 0;
    uint32_t NumberVertex = 0;
    
    VertexBuffer();
    ~VertexBuffer() {
        Destroy();
    }
    NO_COPY_CONSTRUCTOR(VertexBuffer)
    
    void Destroy();
};

class IndexBuffer: public MemoryResource {
public:
    union {
        void* ptr = nullptr;
#ifdef PERIMETER_D3D9
        struct IDirect3DIndexBuffer9* d3d;
#endif
#ifdef PERIMETER_SOKOL
        struct SokolBuffer* sg;
#endif
    };
    bool dynamic = false;
    uint32_t NumberIndices = 0;
    
    IndexBuffer();
    ~IndexBuffer() {
        Destroy();
    }
    NO_COPY_CONSTRUCTOR(IndexBuffer)

    void Destroy();
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

class cIUnkClass;
class cObjMesh;

unsigned int ColorByNormal(Vect3f n);
Vect3f NormalByColor(uint32_t d);
void BuildMipMap(int x,int y,int bpp,int bplSrc,void *pSrc,int bplDst,void *pDst,
                 int rc,int gc,int bc,int ac,int rs,int gs,int bs,int as,int Blur=0);

struct sPolygon {
    static const size_t PN = 3;
    indices_t p1, p2, p3;
    inline void set(indices_t i1,indices_t i2,indices_t i3) { p1=i1; p2=i2; p3=i3; }
};

class cFont;
class cTexture;
class cTexLibrary;
class cCamera;

struct sDataRenderMaterial
{
    sColor4f	Ambient;
    sColor4f	Diffuse;
    sColor4f	Specular;
    sColor4f	Emissive;
    float		Power;

    float       Phase;
    uint32_t    mat;//eMaterialMode
    cTexture	*Tex[2];
    MatXf		TexMatrix;
    float		MaterialAnimPhase;

    sDataRenderMaterial()			{ Phase=0; MaterialAnimPhase=0; }
};

using ColorConversionFunc = uint32_t (*)(const sColor4c&);

class cInterfaceRenderDevice : public cUnknownClass
{
protected:
    struct SDL_Window* sdl_window = nullptr;
    cTexLibrary* TexLibrary = nullptr;
    cFont* CurrentFont = nullptr;
    cFont* DefaultFont = nullptr;
    cCamera *DrawNode = nullptr;
    Vect2i ScreenSize = { 0, 0 };
    Vect2i MaxScreenSize = { 0, 0 };
    uint32_t ScreenHZ = 0;
    uint32_t RenderMode = 0;
    class DrawBuffer* activeDrawBuffer = nullptr;
    std::vector<class DrawBuffer*> drawBuffers;
    Mat4f orthoVP;
    eCullMode CameraCullMode = CULL_NONE;
    bool WireframeMode = false;

    virtual void Draw(class FieldDispatcher *ffd, uint8_t transparent);

public:
    cInterfaceRenderDevice();
    ~cInterfaceRenderDevice() override;
    
    // Common methods

    virtual uint32_t GetWindowCreationFlags() const { return 0; }
    virtual int Init(int xScr,int yScr,int mode, SDL_Window* wnd = nullptr, int RefreshRateInHz=0);
    virtual int Done();
    virtual int BeginScene();
    virtual int EndScene();

    cTexLibrary* GetTexLibrary() { return TexLibrary; }
    cTexture* GetTexture(int n);

    eModeRenderDevice GetRenderMode() { return static_cast<eModeRenderDevice>(RenderMode); }

    virtual bool IsFullScreen() { return (RenderMode&RENDERDEVICE_MODE_WINDOW) == 0; }
    
    virtual int GetSizeX() { return ScreenSize.x; };
    virtual int GetSizeY() { return ScreenSize.y; };
    virtual int GetMaxSizeX() { return MaxScreenSize.x; }
    virtual int GetMaxSizeY() { return MaxScreenSize.y; }

    virtual void SetTexture(uint32_t slot, class cTexture* texture, float Phase);

    inline cCamera* GetDrawNode() { return DrawNode; }
    virtual void SetDrawNode(cCamera* node) { DrawNode = node; };
    virtual void SetWorldMatXf(const MatXf& matrix);

    virtual void SetFont(cFont *pFont);
    virtual void SetDefaultFont(cFont *pFont);
    virtual float GetFontLength(const char *string, size_t* count = nullptr);
    virtual float GetCharLength(const char c);

    //Back in the good-ish old days a line was just one pixel width and everybody was happy, over years due to
    //technological progress the screen resolutions have become higher and higher each year such as a 2004 1 pixel line
    //is too thin to be even noticed in super pro whatever 4k screen unless you stare close enough to feel the screen
    //warmth in your face
    //Therefore this function returns what width a thin line should have based on 2004 era screen and 1~px ratio
    float getThinLineWidth() const;

    virtual size_t GetSizeFromFormat(vertex_fmt_t fmt) const;

    virtual void CreateVertexBuffer(class VertexBuffer &vb, uint32_t NumberVertex, vertex_fmt_t fmt, bool dynamic);
    virtual void DeleteVertexBuffer(class VertexBuffer &vb);
    virtual void CreateIndexBuffer(class IndexBuffer& ib, uint32_t NumberIndices, bool dynamic);
    virtual void DeleteIndexBuffer(class IndexBuffer &ib);
    virtual void* LockVertexBuffer(class VertexBuffer &vb);
    virtual void* LockVertexBufferRange(class VertexBuffer &vb, uint32_t Start, uint32_t Amount);
    virtual void UnlockVertexBuffer(class VertexBuffer &vb);
    virtual indices_t* LockIndexBuffer(class IndexBuffer &ib);
    virtual indices_t* LockIndexBufferRange(class IndexBuffer &ib, uint32_t Start, uint32_t Amount);
    virtual void UnlockIndexBuffer(class IndexBuffer &ib);

    virtual class DrawBuffer* GetDrawBuffer(vertex_fmt_t fmt, ePrimitiveType primitive);
    virtual void SetActiveDrawBuffer(class DrawBuffer*);

    /*Внутренний мтод. Использовать с крайней осторожностью.
      Перед использованием посмотреть как используется внутри Render.
    */
    virtual void ChangeTextColor(const char* &str,sColor4c& diffuse);

    virtual void OutTextRect(int x,int y,const char *string,int align,Vect2f& bmin,Vect2f& bmax);

    virtual void OutText(int x,int y,const char *string,const sColor4f& color,int align=-1,eBlendMode blend_mode=ALPHA_BLEND);
    virtual void OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
                         cTexture* pTexture,eColorMode mode,
                         Vect2f uv,//Координаты текстуры в точке x,y
                         Vect2f duv,//du,dv на один логический пиксель 
            //(лог пиксель равен графическому в разрешении 1024x768)
                         float phase=0,
                         float lerp_factor=1//0..1 Насколько сильно влияет pTexture
    );

    virtual void CreateFFDData(class FieldDispatcher *rd);
    virtual void DeleteFFDData(class FieldDispatcher *rd);

    virtual int CreateTilemap(class cTileMap *TileMap);
    virtual int DeleteTilemap(class cTileMap *TileMap);

    virtual void Draw(class ElasticSphere *es);
    virtual void Draw(class cScene *Scene);
    virtual void DrawBound(const MatXf &Matrix, const Vect3f &min, const Vect3f &max, bool wireframe=0, const sColor4c& Color=sColor4c(255,255,255,255));
    virtual void Draw(class FieldDispatcher *ffd);

    virtual void DrawSprite(int x,int y,int dx,int dy,float u,float v,float du,float dv,
                            cTexture *Texture,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0,eBlendMode mode=ALPHA_NONE);
    virtual void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,
                             cTexture *Tex1,cTexture *Tex2,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0);
    virtual void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
                             cTexture *Tex1,cTexture *Tex2,const sColor4c& ColorMul=sColor4c(255,255,255,255),float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE);
    virtual void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
                             cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha=1,float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE);
    
    virtual void DrawLine(int x1,int y1,int x2,int y2,const sColor4c& color, float width = 1.0);
    virtual void DrawPixel(int x1,int y1, const sColor4c& color);
    virtual void DrawRectangle(int x,int y,int dx,int dy,const sColor4c& color, float outline = 0.0);
    virtual void FlushPrimitive2D();
    
    virtual void DrawLine(const Vect3f &v1,const Vect3f &v2,const sColor4c& color);
    virtual void DrawPoint(const Vect3f &v1, const sColor4c& color);
    virtual void FlushPrimitive3D();

    // Decl only methods

    ColorConversionFunc ConvertColor = nullptr;

    virtual eRenderDeviceSelection GetRenderSelection() const = 0;

    virtual bool ChangeSize(int xScr,int yScr,int mode) = 0;
    virtual int GetClipRect(int *xmin,int *ymin,int *xmax,int *ymax) = 0;
    virtual int SetClipRect(int xmin,int ymin,int xmax,int ymax) = 0;
    virtual void UseOrthographicProjection() = 0;
    virtual void SetDrawTransform(class cCamera *pDrawNode) = 0;
    virtual void SetWorldMat4f(const Mat4f* matrix) = 0;

    virtual int Fill(int r,int g,int b,int a=255) = 0;
    virtual void ClearZBuffer() = 0;
    virtual int Flush(bool wnd=false) = 0;
    virtual int SetGamma(float fGamma,float fStart=0.f,float fFinish=1.f) = 0;

    virtual bool SetScreenShot(const char *fname) = 0;
    virtual uint32_t GetRenderState(eRenderStateOption option) = 0;
    virtual int SetRenderState(eRenderStateOption option,uint32_t value) = 0;

    virtual int CreateTexture(class cTexture *Texture,class cFileImage *FileImage,bool enable_assert=true) = 0;
    virtual int DeleteTexture(class cTexture *Texture) = 0;
    virtual void* LockTexture(class cTexture *Texture, int& Pitch) = 0;
    virtual void* LockTextureRect(class cTexture* Texture, int& Pitch, Vect2i pos, Vect2i size) = 0;
    virtual void UnlockTexture(class cTexture *Texture) = 0;
    virtual void SetTextureImage(uint32_t slot, struct TextureImage* texture_image) = 0;
    virtual uint32_t GetMaxTextureSlots() = 0;
    
    virtual void SetGlobalFog(const sColor4f &color,const Vect2f &v) = 0;
    virtual void SetGlobalLight(Vect3f *vLight, sColor4f *Ambient = nullptr,
                                sColor4f *Diffuse = nullptr, sColor4f *Specular = nullptr) = 0;

    virtual bool IsEnableSelfShadow() = 0;
    
    virtual void SetNoMaterial(eBlendMode blend,float Phase=0,cTexture *Texture0=0,cTexture *Texture1=0,eColorMode color_mode=COLOR_MOD) = 0;
    virtual void SetBlendState(eBlendMode blend) = 0;

    virtual void SubmitDrawBuffer(class DrawBuffer* db, DrawRange* range) = 0;
    virtual void SubmitBuffers(ePrimitiveType primitive, class VertexBuffer* vb, size_t vertices, class IndexBuffer* ib, size_t indices, DrawRange* range) = 0;

    virtual void BeginDrawMesh(bool obj_mesh, bool use_shadow) = 0;
    virtual void EndDrawMesh() = 0;
    virtual void SetSimplyMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) = 0;
    virtual void DrawNoMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) = 0;

    virtual void BeginDrawShadow(bool shadow_map) = 0;
    virtual void EndDrawShadow() = 0;
    virtual void SetSimplyMaterialShadow(cObjMesh* mesh, cTexture* texture) = 0;
    virtual void DrawNoMaterialShadow(cObjMesh* mesh) = 0;

    virtual void SetMaterialTilemap(cTileMap *TileMap) = 0;
    virtual void SetMaterialTilemapShadow() = 0;
    virtual void SetTileColor(sColor4f color) = 0;

    /*
    virtual bool CreateShadowTexture(int xysize);
    virtual void DeleteShadowTexture();

    virtual cTexture* GetShadowMap();
    virtual ???* GetZBuffer();
    virtual cTexture* GetLightMap();
    */
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
