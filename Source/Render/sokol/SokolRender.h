#ifndef PERIMETER_SOKOLRENDER_H
#define PERIMETER_SOKOLRENDER_H

#define PERIMETER_SOKOL_SHARE_SHADERS

#include "sokol_gfx.h"
#include <SDL_video.h>
#include "SokolRenderPipeline.h"

const eColorMode PERIMETER_SOKOL_COLOR_MODE_MOD_COLOR_ADD_ALPHA = static_cast<const eColorMode>(1 << 7);

const int PERIMETER_SOKOL_TEXTURES = 2;

struct SokolCommand {
    SokolCommand();
    ~SokolCommand();
    void Clear();
    void ClearDrawData();
    void ClearMVP();
    void SetTexture(size_t index, cTexture* texture, SokolTexture2D* sokol_texture);
    void ClearTextures();
    NO_COPY_CONSTRUCTOR(SokolCommand)
    
    pipeline_id_t pipeline_id = 0;
    size_t vertices = 0;
    size_t indices = 0;
    struct SokolTexture2D* sokol_textures[PERIMETER_SOKOL_TEXTURES] = {};
    class cTexture* texture_handles[PERIMETER_SOKOL_TEXTURES] = {};
    bool owned_buffers = false;
    struct SokolBuffer* vertex_buffer = nullptr;
    struct SokolBuffer* index_buffer = nullptr;
    bool owned_mvp = false;
    Mat4f* vs_mvp = nullptr;
    int fs_mode = 0;
};

//TODO once implemented all use inherit from cInterfaceRenderDevice and remove this include
#include "EmptyRenderDevice.h"
class cSokolRender: public cEmptyRender {
private:
    //SDL context
    SDL_Window* sdlWindow = nullptr;
    SDL_GLContext sdlGlContext = nullptr;
    
    //Renderer state
    bool ActiveScene = false;
    sColor4f fill_color;
    std::vector<SokolCommand*> commands;
    Vect2i viewportPos;
    Vect2i viewportSize;
    Mat4f orthoVP;
    
    //Empty texture when texture slot is unused
    SokolTexture2D* emptyTexture = nullptr;
#ifdef PERIMETER_DEBUG
    SokolTexture2D* testTexture = nullptr;
#endif

    //Pipelines
#ifdef PERIMETER_SOKOL_SHARE_SHADERS
    std::map<std::string, sg_shader> shaders;
#endif
    std::vector<struct SokolPipeline*> pipelines;
    static pipeline_id_t GetPipelineID(PIPELINE_TYPE type, vertex_fmt_t vertex_fmt, uint8_t mode);
    static pipeline_id_t GetPipelineID(PIPELINE_TYPE type, vertex_fmt_t vertex_fmt, eBlendMode blend, eCullMode cull);
    static void GetPipelineIDParts(pipeline_id_t id, PIPELINE_TYPE* type, vertex_fmt_t* vertex_fmt, eBlendMode* blend, eCullMode* cull);
    void ClearPipelines();
    void RegisterPipelines();
    void RegisterPipeline(pipeline_id_t id, struct shader_funcs* shader_funcs);
    
    //Active pipeline/command state
    SokolCommand activeCommand;
    PIPELINE_TYPE activePipelineType = PIPELINE_TYPE_DEFAULT;

    eBlendMode activePipelineBlend = ALPHA_NONE;
    eCullMode activePipelineCull = CULL_NONE;
    Mat4f activeCommandVP;
    Mat4f activeCommandW;

    //Commands handling
    void ClearCommands();
    void FinishCommand();
    void SetVPMatrix(const Mat4f* matrix);

    //Updates internal state after init/resolution change
    int UpdateRenderMode();
    
public:
    cSokolRender();
    ~cSokolRender() override;

    // //// cInterfaceRenderDevice impls start ////

    eRenderDeviceSelection GetRenderSelection() const override {
        return DEVICE_SOKOL;
    }
    
    void SetActiveDrawBuffer(class DrawBuffer*) override;
    
    int Init(int xScr,int yScr,int mode,void *hWnd=0,int RefreshRateInHz=0) override;
    bool ChangeSize(int xScr,int yScr,int mode) override;

    int GetClipRect(int *xmin,int *ymin,int *xmax,int *ymax) override;
    int SetClipRect(int xmin,int ymin,int xmax,int ymax) override;
    
    int Done() override;

    void SetDrawNode(cCamera *pDrawNode) override;
    void SetWorldMat4f(const Mat4f* matrix) override;
    void UseOrthographicProjection();
    void SetDrawTransform(class cCamera *DrawNode) override;

    int BeginScene() override;
    int EndScene() override;
    int Fill(int r,int g,int b,int a=255) override;
    int Flush(bool wnd=false) override;
    
    int SetGamma(float fGamma,float fStart=0.f,float fFinish=1.f) override;

    void CreateVertexBuffer(class VertexBuffer &vb, uint32_t NumberVertex, vertex_fmt_t fmt, bool dynamic) override;
    void DeleteVertexBuffer(class VertexBuffer &vb) override;
    void* LockVertexBuffer(class VertexBuffer &vb) override;
    void* LockVertexBuffer(class VertexBuffer &vb, uint32_t Start, uint32_t Amount) override;
    void UnlockVertexBuffer(class VertexBuffer &vb) override;
    void CreateIndexBuffer(class IndexBuffer& ib, uint32_t NumberIndices, bool dynamic) override;
    void DeleteIndexBuffer(class IndexBuffer &ib) override;
    indices_t* LockIndexBuffer(class IndexBuffer &ib) override;
    indices_t* LockIndexBuffer(class IndexBuffer &ib, uint32_t Start, uint32_t Amount) override;
    void UnlockIndexBuffer(class IndexBuffer &ib) override;
    void SubmitDrawBuffer(class DrawBuffer* db) override;
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
    */

    int GetRenderState(eRenderStateOption option) override;
    int SetRenderState(eRenderStateOption option,int value) override;

    /*
    void DrawLine(int x1,int y1,int x2,int y2,sColor4c color) override;
    void DrawPixel(int x1,int y1,sColor4c color) override;
    */
    void DrawRectangle(int x,int y,int dx,int dy,sColor4c color,bool outline=false) override;
    void FlushPrimitive2D() override;

    void OutText(int x,int y,const char *string,const sColor4f& color,int align=-1,eBlendMode blend_mode=ALPHA_BLEND) override;
    void OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
                 cTexture* pTexture,eColorMode mode,Vect2f uv,Vect2f duv,float phase=0,float lerp_factor=1) override;

    /*
    bool SetScreenShot(const char *fname) override;
    */

    void DrawSprite(int x,int y,int dx,int dy,float u,float v,float du,float dv,
                    cTexture *Texture,const sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0,eBlendMode mode=ALPHA_NONE) override;
    void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
                     cTexture *Tex1,cTexture *Tex2,const sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE) override;
    void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
                     cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha=1,float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE) override;

    /*
    bool IsEnableSelfShadow() override;
    */

    void SetNoMaterial(eBlendMode blend,float Phase=0,cTexture *Texture0=0,cTexture *Texture1=0,eColorMode color_mode=COLOR_MOD) override;
    
    /*
    void DrawLine(const Vect3f &v1,const Vect3f &v2,sColor4c color) override;
    void DrawPoint(const Vect3f &v1,sColor4c color) override;

    void FlushPrimitive3D() override;
    */
    // //// cInterfaceRenderDevice impls end ////
};

#endif //PERIMETER_SOKOLRENDER_H
