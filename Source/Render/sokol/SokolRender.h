#ifndef PERIMETER_SOKOLRENDER_H
#define PERIMETER_SOKOLRENDER_H

#include "sokol_gfx.h"
#include <SDL_video.h>

struct SokolPipeline {
    //VERTEX_FMT_* flags used as index for this pipeline
    uint32_t vertex_fmt;
    //Created pipeline for this
    sg_pipeline pipeline;
    //Shader functions to retrieve info
    struct shader_funcs* shader_funcs;
};

struct SokolCommand {
    uint32_t vertex_fmt = 0;
    size_t elements = 0;
    struct SokolTexture2D* texture_1 = nullptr;
    struct SokolTexture2D* texture_2 = nullptr;
    struct SokolBuffer* vertex_buffer = nullptr;
    struct SokolBuffer* index_buffer = nullptr;
};

//TODO once implemented all use inherit from cInterfaceRenderDevice
class cSokolRender: public cEmptyRender {
private:    
    SDL_Window* sdlWindow = nullptr;
    SDL_GLContext sdlGlContext = nullptr;
    
    bool ActiveScene = false;
    
    sColor4f fill_color;
    std::vector<SokolCommand> commands;
    std::vector<SokolPipeline> pipelines;
    
public:
    cSokolRender();
    ~cSokolRender() override;

    // //// cInterfaceRenderDevice impls start ////

    eRenderDeviceSelection GetRenderSelection() const override {
        return DEVICE_SOKOL;
    }
    
    int Init(int xScr,int yScr,int mode,void *hWnd=0,int RefreshRateInHz=0) override;
    bool ChangeSize(int xScr,int yScr,int mode) override;

    /*
    int GetClipRect(int *xmin,int *ymin,int *xmax,int *ymax) override;
    int SetClipRect(int xmin,int ymin,int xmax,int ymax) override;
     */
    
    int Done() override;

    int BeginScene() override;
    int EndScene() override;
    int Fill(int r,int g,int b,int a=255) override;
    int Flush(bool wnd=false) override;
    
    int SetGamma(float fGamma,float fStart=0.f,float fFinish=1.f) override;

    void CreateVertexBuffer(class VertexBuffer &vb, int NumberVertex, int fmt, int dynamic=0) override;
    void DeleteVertexBuffer(VertexBuffer &vb) override;
    void* LockVertexBuffer(VertexBuffer &vb) override;
    void UnlockVertexBuffer(VertexBuffer &vb) override;
    void CreateIndexBuffer(IndexBuffer& ib, int NumberPolygon) override;
    void DeleteIndexBuffer(IndexBuffer &ib) override;
    sPolygon* LockIndexBuffer(IndexBuffer &ib) override;
    void UnlockIndexBuffer(IndexBuffer &ib) override;
    int CreateTexture(class cTexture *Texture,class cFileImage *FileImage,bool enable_assert=true) override;
    int DeleteTexture(class cTexture *Texture) override;
    void* LockTexture(class cTexture *Texture, int& Pitch) override;
    void UnlockTexture(class cTexture *Texture) override;

    /*
    void SetGlobalFog(const sColor4f &color,const Vect2f &v) override;

    void SetGlobalLight(Vect3f *vLight, sColor4f *Ambient = nullptr,
                        sColor4f *Diffuse = nullptr, sColor4f *Specular = nullptr) override;

    void Draw(class cScene *Scene) override;
    void Draw(class FieldDispatcher *rd) override;
    void CreateFFDData(class FieldDispatcher *rd) override;
    void DeleteFFDData(class FieldDispatcher *rd) override;

    void Draw(class ElasticSphere *es) override;

    void DrawBound(const MatXf &Matrix,Vect3f &min,Vect3f &max,bool wireframe=0,const sColor4c &Color=sColor4c(255,255,255,255)) override;

    int Create(class cTileMap *TileMap) override;
    void PreDraw(cTileMap *TileMap) override;
    void Draw(cTileMap *TileMap,eBlendMode MatMode,TILEMAP_DRAW tile_draw,bool shadow) override;
    int Delete(class cTileMap *TileMap) override;

    int SetRenderState(eRenderStateOption option,int value) override;

    void DrawLine(int x1,int y1,int x2,int y2,sColor4c color) override;
    void DrawPixel(int x1,int y1,sColor4c color) override;
    void DrawRectangle(int x,int y,int dx,int dy,sColor4c color,bool outline=false) override;
    void FlushPrimitive2D() override;

    void ChangeTextColor(const char* &str,sColor4c& diffuse) override;

    void OutText(int x,int y,const char *string,const sColor4f& color,int align=-1,eBlendMode blend_mode=ALPHA_BLEND) override;
    void OutTextRect(int x,int y,const char *string,int align,Vect2f& bmin,Vect2f& bmax) override;
    void OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
                 cTexture* pTexture,eColorMode mode,Vect2f uv,Vect2f duv,float phase=0,float lerp_factor=1) override;

    void OutText(int x,int y,const char *string,int r=255,int g=255,int b=255) override;
    void OutText(int x,int y,const char *string,int r,int g,int b,char *FontName="Arial",int size=12,int bold=0,int italic=0,int underline=0) override;
    
    bool SetScreenShot(const char *fname) override;

    void DrawSprite(int x,int y,int dx,int dy,float u,float v,float du,float dv,
                    cTexture *Texture,const sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0,eBlendMode mode=ALPHA_NONE) override;
    void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
                     cTexture *Tex1,cTexture *Tex2,const sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE) override;
    void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
                     cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha=1,float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE) override;

    void DrawIndexedPrimitive(sPtrVertexBuffer &vb,int OfsVertex,int nVertex,const sPtrIndexBuffer& ib,int nOfsPolygon,int nPolygon) override;

    bool IsEnableSelfShadow() override;

    void SetNoMaterial(eBlendMode blend,float Phase=0,cTexture *Texture0=0,cTexture *Texture1=0,eColorMode color_mode=COLOR_MOD) override;

    void SetDrawTransform(class cCamera *DrawNode) override;
    void DrawLine(const Vect3f &v1,const Vect3f &v2,sColor4c color) override;
    void DrawPoint(const Vect3f &v1,sColor4c color) override;

    void FlushPrimitive3D() override;
    */
    // //// cInterfaceRenderDevice impls end ////
};

#endif //PERIMETER_SOKOLRENDER_H
