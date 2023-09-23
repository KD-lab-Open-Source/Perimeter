#ifndef PERIMETER_SOKOLRENDER_H
#define PERIMETER_SOKOLRENDER_H

#if defined(SOKOL_GLCORE33) || defined(SOKOL_GLES2) || defined(SOKOL_GLES3)
#define SOKOL_GL (1)
#endif

#include "sokol_gfx.h"
#include <SDL_video.h>
#ifdef SOKOL_METAL
#include <SDL_metal.h>
#endif
#include "SokolTypes.h"

const int PERIMETER_SOKOL_TEXTURES = 8;

#ifdef SOKOL_METAL
void sokol_metal_setup_desc(SDL_MetalView view, sg_desc* desc);
#endif

struct SokolCommand {
    SokolCommand();
    ~SokolCommand();
    void CreateShaderParams();
    void Clear();
    void ClearDrawData();
    void ClearShaderParams();
    void SetTexture(size_t index, SokolTexture2D* sokol_texture);
    void ClearTextures();
    NO_COPY_CONSTRUCTOR(SokolCommand)
    
    pipeline_id_t pipeline_id = 0;
    SOKOL_SHADER_ID shader_id = SOKOL_SHADER_ID_NONE;
    size_t base_elements = 0;
    size_t vertices = 0;
    size_t indices = 0;
    struct SokolTexture2D* sokol_textures[PERIMETER_SOKOL_TEXTURES] = {};
    bool owned_vertex_buffer = false;
    bool owned_index_buffer = false;
    struct SokolBuffer* vertex_buffer = nullptr;
    struct SokolBuffer* index_buffer = nullptr;
    void* vs_params = nullptr;
    void* fs_params = nullptr;
    size_t vs_params_len = 0;
    size_t fs_params_len = 0;
    Vect2i clipPos = Vect2i(0, 0);
    Vect2i clipSize = Vect2i(0, 0);
};

class cSokolRender: public cInterfaceRenderDevice {
private:
    //SDL context
#ifdef SOKOL_GL
    SDL_GLContext sdlGlContext = nullptr;
#endif
#ifdef SOKOL_METAL
    SDL_MetalView sdlMetalView = nullptr;
#endif
    
    //Renderer state
    bool ActiveScene = false;
    bool isOrthographicProjSet = false;
    sg_color fill_color;
    std::vector<SokolCommand*> commands;
    
    //Empty texture when texture slot is unused
    SokolTexture2D* emptyTexture = nullptr;

    //Pipelines
    std::unordered_map<std::string, sg_shader> shaders;
    std::unordered_map<pipeline_id_t, struct SokolPipeline*> pipelines;
    static pipeline_id_t GetPipelineID(PIPELINE_TYPE type, vertex_fmt_t vertex_fmt, const PIPELINE_MODE& mode);
    static void GetPipelineIDParts(pipeline_id_t id, PIPELINE_TYPE* type, vertex_fmt_t* vertex_fmt, PIPELINE_MODE* mode);
    void ClearPipelines();
    void RegisterPipeline(pipeline_id_t id);
    
    //Active pipeline/command state
    SokolCommand activeCommand;
    PIPELINE_TYPE activePipelineType = PIPELINE_TYPE_DEFAULT;
    PIPELINE_MODE activePipelineMode;
    Mat4f activeCommandVP;
    Mat4f activeCommandW;
    eColorMode activeCommandColorMode = COLOR_MOD;
    float activeCommandTex2Lerp = -1;
    eAlphaTestMode activeCommandAlphaTest = ALPHATEST_NONE;
    sColor4f activeCommandTileColor;

    //Commands handling
    void ClearCommands();
    void FinishActiveDrawBuffer();
    void CreateCommand(class VertexBuffer* vb, size_t vertices, class IndexBuffer* ib, size_t indices);
    void SetVPMatrix(const Mat4f* matrix);
    void SetTex2Lerp(float lerp);
    void SetColorMode(eColorMode color_mode);

    //Updates internal state after init/resolution change
    int UpdateRenderMode();

    //Set common VS/FS parameters
    template<typename T_VS, typename T_FS>
    void shader_set_common_params(T_VS* vs_params, T_FS* fs_params) {
        vs_params->un_mvp = isOrthographicProjSet ? orthoVP : (activeCommandW * activeCommandVP);
        switch (activeCommandAlphaTest) {
            default:
            case ALPHATEST_NONE:
                fs_params->un_alpha_test = -1.0f;
                break;
            case ALPHATEST_GT_0:
                fs_params->un_alpha_test = 0.0f;
                break;
            case ALPHATEST_GT_1:
                fs_params->un_alpha_test = (1.0f/255.0f);
                break;
            case ALPHATEST_GT_254:
                fs_params->un_alpha_test = (254.0f/255.0f);
                break;
        }
    }

public:
    cSokolRender();
    ~cSokolRender() override;

    // //// cInterfaceRenderDevice impls start ////

    eRenderDeviceSelection GetRenderSelection() const override;

    uint32_t GetWindowCreationFlags() const override;
    void SetActiveDrawBuffer(class DrawBuffer*) override;
    
    int Init(int xScr,int yScr,int mode,SDL_Window* wnd=nullptr,int RefreshRateInHz=0) override;
    bool ChangeSize(int xScr,int yScr,int mode) override;

    int GetClipRect(int *xmin,int *ymin,int *xmax,int *ymax) override;
    int SetClipRect(int xmin,int ymin,int xmax,int ymax) override;
    
    int Done() override;

    void SetDrawNode(cCamera *pDrawNode) override;
    void SetWorldMat4f(const Mat4f* matrix) override;
    void UseOrthographicProjection() override;
    void SetDrawTransform(class cCamera *DrawNode) override;

    int BeginScene() override;
    int EndScene() override;
    int Fill(int r,int g,int b,int a=255) override;
    void ClearZBuffer() override;
    int Flush(bool wnd=false) override;
    
    int SetGamma(float fGamma,float fStart=0.f,float fFinish=1.f) override;

    void DeleteVertexBuffer(class VertexBuffer &vb) override;
    void DeleteIndexBuffer(class IndexBuffer &ib) override;
    void SubmitDrawBuffer(class DrawBuffer* db, DrawRange* range) override;
    void SubmitBuffers(ePrimitiveType primitive, class VertexBuffer* vb, size_t vertices, class IndexBuffer* ib, size_t indices, DrawRange* range) override;
    int CreateTexture(class cTexture *Texture,class cFileImage *FileImage,bool enable_assert=true) override;
    int DeleteTexture(class cTexture *Texture) override;
    void* LockTexture(class cTexture *Texture, int& Pitch) override;
    void* LockTextureRect(class cTexture* Texture, int& Pitch, Vect2i pos, Vect2i size) override;
    void UnlockTexture(class cTexture *Texture) override;
    void SetTextureImage(uint32_t slot, struct TextureImage* texture_image) override;
    uint32_t GetMaxTextureSlots() override;

    void SetGlobalFog(const sColor4f &color,const Vect2f &v) override;

    void SetGlobalLight(Vect3f *vLight, sColor4f *Ambient = nullptr,
                        sColor4f *Diffuse = nullptr, sColor4f *Specular = nullptr) override;

    uint32_t GetRenderState(eRenderStateOption option) override;
    int SetRenderState(eRenderStateOption option,uint32_t value) override;

    void OutText(int x,int y,const char *string,const sColor4f& color,int align=-1,eBlendMode blend_mode=ALPHA_BLEND) override;
    void OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
                 cTexture* pTexture,eColorMode mode,Vect2f uv,Vect2f duv,float phase=0,float lerp_factor=1) override;

    bool SetScreenShot(const char *fname) override;

    void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
                     cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha=1,float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE) override;

    bool IsEnableSelfShadow() override;

    void SetNoMaterial(eBlendMode blend,float Phase=0,cTexture *Texture0=0,cTexture *Texture1=0,eColorMode color_mode=COLOR_MOD) override;
    void SetBlendState(eBlendMode blend) override;

    void BeginDrawMesh(bool obj_mesh, bool use_shadow) override;
    void EndDrawMesh() override;
    void SetSimplyMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) override;
    void DrawNoMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) override;

    void BeginDrawShadow(bool shadow_map) override;
    void EndDrawShadow() override;
    void SetSimplyMaterialShadow(cObjMesh* mesh, cTexture* texture) override;
    void DrawNoMaterialShadow(cObjMesh* mesh) override;

    void SetMaterialTilemap(cTileMap *TileMap) override;
    void SetMaterialTilemapShadow() override;
    void SetTileColor(sColor4f color) override;

    // //// cInterfaceRenderDevice impls end ////
};

#endif //PERIMETER_SOKOLRENDER_H
