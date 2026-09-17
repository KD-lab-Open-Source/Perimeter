#pragma once

#include "VulkanTypes.h"
#include "VulkanResources.h"
#include "VertexFormat.h"
#include "IRenderDevice.h"

#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>
#include <vector>
#include <unordered_map>

struct VulkanPipeline;
struct VulkanCommand;
struct VulkanRenderTarget;

// ---- Constants ---------------------------------------------------------

static const int VULKAN_FRAMES_IN_FLIGHT  = 2;
// Per-frame ring buffers live in HOST_VISIBLE+HOST_COHERENT memory and stay
// permanently mapped. With FRAMES_IN_FLIGHT=2 the totals are doubled.
//
// Sized for the heaviest in-game scenes (~15k draws/frame, ~100 MB of dynamic
// VB data on full-action gameplay).  On overflow CreateVulkanCommand falls
// back to a per-draw VulkanBuffer kept in frame.dynamicBuffers, freed at the
// next BeginScene for this slot - but freeing thousands of those at once
// produces an 80 ms/frame cleanup stutter, so the rings MUST cover worst case.
// Total host-visible footprint: (32+256+128+128) MB × 2 frames = 1.05 GB.
static const size_t VULKAN_UBO_POOL_SIZE      =  32 * 1024 * 1024;  //  32 MB / frame
static const size_t VULKAN_DYN_VTX_POOL_SIZE  = 256 * 1024 * 1024;  // 256 MB / frame
static const size_t VULKAN_DYN_IDX_POOL_SIZE  = 128 * 1024 * 1024;  // 128 MB / frame
static const size_t VULKAN_STAGE_POOL_SIZE    = 128 * 1024 * 1024;  // 128 MB / frame
// Per-frame descriptor set budget. Pool is reset (O(1) bump reset) at the
// start of each frame after the fence guarantees the GPU has finished with the
// previous submission for this slot.
static const uint32_t VULKAN_DESC_POOL_SETS   = 8192;
// Conservative fallback alignment if the device hasn't been queried yet.
// Real value (deviceProperties.limits.minUniformBufferOffsetAlignment) is
// typically 16-64 bytes; using the queried value halves or quarters real
// UBO ring usage compared with this fallback.
static const uint32_t VULKAN_UBO_ALIGNMENT_FALLBACK = 256;
// Descriptor range for dynamic UBO bindings: must be at least as large as the
// biggest shader's UBO block. 256 bytes covers all current shader structs
// (largest VS = VulkanMeshColorTexVsParams at 192, largest FS =
// VulkanMeshNormalTexFsParams at 160).
static const uint32_t VULKAN_UBO_MAX_VS_RANGE = 256;
static const uint32_t VULKAN_UBO_MAX_FS_RANGE = 256;

// D3D-to-Vulkan clip-space correction: Y-axis flip.
inline Mat4f GetVkClipFix() {
    static const float f[16] = {1,0,0,0, 0,-1,0,0, 0,0,1,0, 0,0,0,1};
    return Mat4f(f[0],f[1],f[2],f[3], f[4],f[5],f[6],f[7],
                 f[8],f[9],f[10],f[11], f[12],f[13],f[14],f[15]);
}

struct PendingTextureUpload {
    VkImage             image      = VK_NULL_HANDLE;
    VkImageAspectFlags  aspect     = VK_IMAGE_ASPECT_COLOR_BIT;
    struct Mip { std::vector<uint8_t> data; int w, h; };
    std::vector<Mip>    mips;
    int                 mipCount   = 0;
    bool                layoutOnly = false;
};

// Deferred VB/IB upload. Produced by PrepareStaticBuffer when active scene is
// in progress; drained onto the frame command buffer in DoVulkanRendering.
struct PendingBufferUpload {
    VkBuffer             dstBuffer = VK_NULL_HANDLE;
    std::vector<uint8_t> data;
};

// ---- Texture-combo descriptor set cache key ----------------------------
// Used to reuse descriptor sets when the same 4 textures appear in multiple
// consecutive draws (e.g. all units of the same type share the same textures).

struct VulkanTexCacheKey {
    struct VulkanTexture2D* t[4] = {};
    bool operator==(const VulkanTexCacheKey& o) const {
        return t[0]==o.t[0] && t[1]==o.t[1] && t[2]==o.t[2] && t[3]==o.t[3];
    }
};
struct VulkanTexCacheKeyHash {
    size_t operator()(const VulkanTexCacheKey& k) const {
        size_t h = 0;
        for (auto* p : k.t)
            h ^= (size_t)(uintptr_t)p + 0x9e3779b9u + (h << 6) + (h >> 2);
        return h;
    }
};

// ---- Per-frame data ----------------------------------------------------

struct VulkanFrameData {
    VkCommandBuffer commandBuffer      = VK_NULL_HANDLE;
    VkSemaphore     imageAvailSemaphore= VK_NULL_HANDLE;
    VkSemaphore     renderDoneSemaphore= VK_NULL_HANDLE;
    VkFence         fence              = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool   = VK_NULL_HANDLE;

    // Linear UBO pool (host-visible + coherent)
    VkBuffer       uboBuffer           = VK_NULL_HANDLE;
    VkDeviceMemory uboMemory           = VK_NULL_HANDLE;
    void*          uboMapped           = nullptr;
    size_t         uboOffset           = 0;

    // Dynamic vertex ring buffer (replaces per-draw-call VulkanBuffer allocs)
    VkBuffer       dynVtxBuffer        = VK_NULL_HANDLE;
    VkDeviceMemory dynVtxMemory        = VK_NULL_HANDLE;
    void*          dynVtxMapped        = nullptr;
    size_t         dynVtxOffset        = 0;

    // Dynamic index ring buffer
    VkBuffer       dynIdxBuffer        = VK_NULL_HANDLE;
    VkDeviceMemory dynIdxMemory        = VK_NULL_HANDLE;
    void*          dynIdxMapped        = nullptr;
    size_t         dynIdxOffset        = 0;

    // Staging ring buffer for texture/buffer uploads (host-visible + coherent)
    VkBuffer       stageBuffer         = VK_NULL_HANDLE;
    VkDeviceMemory stageMemory         = VK_NULL_HANDLE;
    void*          stageMapped         = nullptr;
    size_t         stageOffset         = 0;

    // Instance data ring buffer for instanced draws (host-visible + coherent).
    // Each instance is 64 bytes (4×vec4 world matrix, mat4/affine).
    VkBuffer       instBuffer          = VK_NULL_HANDLE;
    VkDeviceMemory instMemory          = VK_NULL_HANDLE;
    void*          instMapped          = nullptr;
    size_t         instOffset          = 0;

    // Per-frame texture-combo -> descriptor set cache.
    // Cleared when the descriptor pool is reset (BeginScene).
    std::unordered_map<VulkanTexCacheKey, VkDescriptorSet,
                       VulkanTexCacheKeyHash> texDescCache;

    // Staging buffers used for deferred texture uploads this frame.
    // Freed in BeginScene once the fence signals (GPU finished consuming them).
    std::vector<std::pair<VkBuffer, VkDeviceMemory>> stagingBuffers;

    // Kept for safety but no longer populated when ring buffers are used.
    std::vector<struct VulkanBuffer*> dynamicBuffers;

    // Per-frame VB/IB copy cache: maps pointer to ring-buffer offset.
    // Skips re-memcpy of identical vertex/index data for repeated draws of the
    // same mesh in a frame. Keys are unique object pointers, not data-ptr aliases.
    std::unordered_map<const VertexBuffer*, size_t> vtxCache;
    std::unordered_map<const IndexBuffer*,  size_t> idxCache;
};

// ---- cVulkanRender -----------------------------------------------------

class cVulkanRender : public cInterfaceRenderDevice {
public:
    cVulkanRender();
    ~cVulkanRender() override;

    // ---- cInterfaceRenderDevice mandatory overrides --------------------

    eRenderDeviceSelection GetRenderSelection() const override;
    uint32_t GetWindowCreationFlags() const override;

    int  Init(int xScr, int yScr, int mode, SDL_Window* wnd, int RefreshHz) override;
    int  Done() override;

    static void SetProfilingEnabled(bool on);
    static bool profilingEnabled;

    static void ProfRecordDescAlloc();
    static void ProfRecordPipeCreate();
    static void ProfRecordDrawCall();
    static void ProfRecordCmdPhase(double ubo, double desc, double pipe, double vb, double ib);
    static void ProfRecordBatchPhase(size_t groups, size_t instanced, size_t fallback,
                                      double groupUs, double uploadUs, double uboUs,
                                      double descUs, double pipeUs, double instUs, double cmdUs);
    static void ProfBeginSceneFence(double us);
    static void ProfBeginSceneCleanup(double us);
    static void ProfLogicTime(double us);
    static void ProfReport();

    int  BeginScene() override;
    int  EndScene() override;

    bool ChangeSize(int xScr, int yScr, int mode) override;

    int  Fill(int r, int g, int b, int a = 255) override;
    void ClearZBuffer() override;
    int  Flush(bool wnd = false) override;
    int  SetGamma(float fGamma, float fStart = 0.f, float fFinish = 1.f) override;

    bool SetScreenShot(const char* fname) override;

    int  GetClipRect(int* xmin, int* ymin, int* xmax, int* ymax) override;
    int  SetClipRect(int xmin, int ymin, int xmax, int ymax) override;

    void UseOrthographicProjection() override;
    void SetDrawNode(class cCamera* pDrawNode) override;
    void SetDrawTransform(class cCamera* pDrawNode) override;
    void SetWorldMat4f(const Mat4f* matrix) override;

    uint32_t GetRenderState(eRenderStateOption option) override;
    int      SetRenderState(eRenderStateOption option, uint32_t value) override;

    void DeleteVertexBuffer(class VertexBuffer& vb) override;
    void DeleteIndexBuffer(class IndexBuffer& ib) override;

    int  CreateTexture(class cTexture* Texture, class cFileImage* FileImage,
                       bool enable_assert = true) override;
    int  DeleteTexture(class cTexture* Texture) override;
    void* LockTexture(class cTexture* Texture, int& Pitch) override;
    void* LockTextureRect(class cTexture* Texture, int& Pitch, Vect2i pos, Vect2i size) override;
    void  UnlockTexture(class cTexture* Texture) override;
    void  SetTextureImage(uint32_t slot, struct TextureImage* texture_image) override;
    void  SetTextureTransform(uint32_t slot, const Mat4f& transform) override;
    uint32_t GetMaxTextureSlots() override;

    void SetGlobalFog(const sColor4f& color, const Vect2f& v) override;
    void SetGlobalLight(Vect3f* vLight, sColor4f* Ambient,
                        sColor4f* Diffuse, sColor4f* Specular) override;
    bool IsEnableSelfShadow() override;

    void SetNoMaterial(eBlendMode blend, float Phase = 0,
                       cTexture* Tex0 = nullptr, cTexture* Tex1 = nullptr,
                       eColorMode color_mode = COLOR_MOD) override;
    void SetBlendState(eBlendMode blend) override;

    void SubmitDrawBuffer(class DrawBuffer* db, DrawRange* range) override;
    void SubmitBuffers(ePrimitiveType primitive,
                       class VertexBuffer* vb, size_t vertices,
                       class IndexBuffer* ib, size_t indices,
                       DrawRange* range) override;
    void SetActiveDrawBuffer(class DrawBuffer* db) override;

    void BeginDrawMesh(bool obj_mesh, bool use_shadow) override;
    void EndDrawMesh() override;
    void SetSimplyMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) override;
    void DrawNoMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) override;

    void BeginDrawShadow(bool shadow_map) override;
    void EndDrawShadow() override;
    void SetSimplyMaterialShadow(cObjMesh* mesh, cTexture* texture) override;
    void DrawNoMaterialShadow(cObjMesh* mesh) override;
    union SurfaceImage GetShadowZBuffer() override;

    void SetRenderTarget(cTexture* target, union SurfaceImage zbuffer) override;
    void RestoreRenderTarget() override;

    void SetMaterialTilemap(class cTileMap* TileMap) override;
    void SetMaterialTilemapShadow() override;
    void SetTileColor(sColor4f color) override;

    void DrawSprite2(int x, int y, int dx, int dy,
                     float u, float v, float du, float dv,
                     float u1, float v1, float du1, float dv1,
                     cTexture* Tex1, cTexture* Tex2, float lerp_factor,
                     float alpha = 1, float phase = 0,
                     eColorMode mode = COLOR_MOD,
                     eBlendMode blend_mode = ALPHA_NONE) override;

    void OutText(int x, int y, const char* string, const sColor4f& color,
                 int align = -1, eBlendMode blend_mode = ALPHA_BLEND) override;
    void OutText(int x, int y, const char* string, const sColor4f& color,
                 int align, eBlendMode blend_mode,
                 cTexture* pTexture, eColorMode mode, Vect2f uv, Vect2f duv,
                 float phase, float lerp_factor) override;
    void SetTex2Lerp(float lerp);

    bool CreateShadowTexture(int xysize) override;
    void DeleteShadowTexture() override;
    cTexture* GetShadowMap() override;
    cTexture* GetLightMap() override;

    // ---- Internal methods (called from split .cpp files) ---------------

    void DoVulkanRendering();
    void ProcessRenderPass(VkRenderPass renderPass,
                           VkFramebuffer framebuffer,
                           VkExtent2D extent,
                           bool clearColor, bool clearDepth,
                           std::vector<VulkanCommand>& commands);

private:
    // ---- Vulkan core ---------------------------------------------------

    VkInstance               instance          = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger   = VK_NULL_HANDLE;
    bool                     validationEnabled = false;
    VkPhysicalDevice         physicalDevice  = VK_NULL_HANDLE;
    VkDevice                 device          = VK_NULL_HANDLE;
    uint32_t                 graphicsFamily  = UINT32_MAX;
    uint32_t                 presentFamily   = UINT32_MAX;
    VkQueue                  graphicsQueue   = VK_NULL_HANDLE;
    VkQueue                  presentQueue    = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties deviceProperties = {};
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties = {};
    // Cached deviceProperties.limits.minUniformBufferOffsetAlignment (e.g.
    // 16-64 bytes on most desktop GPUs). Populated in PickPhysicalDevice.
    uint32_t uboAlignment = 0;

    // ---- Surface / Swapchain -------------------------------------------

    VkSurfaceKHR    surface             = VK_NULL_HANDLE;
    VkSwapchainKHR  swapchain           = VK_NULL_HANDLE;
    VkFormat        swapchainFormat     = VK_FORMAT_UNDEFINED;
    VkColorSpaceKHR swapchainColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkExtent2D      swapchainExtent     = {};
    std::vector<VkImage>       swapchainImages;
    std::vector<VkImageView>   swapchainImageViews;
    std::vector<VkFramebuffer> swapchainFramebuffers;

    // ---- Depth buffer --------------------------------------------------

    VkFormat       depthFormat          = VK_FORMAT_UNDEFINED;
    VkImage        depthImage           = VK_NULL_HANDLE;
    VkDeviceMemory depthMemory          = VK_NULL_HANDLE;
    VkImageView    depthView            = VK_NULL_HANDLE;

    // ---- Render passes -------------------------------------------------

    VkRenderPass mainRenderPass      = VK_NULL_HANDLE;
    VkRenderPass shadowRenderPass    = VK_NULL_HANDLE;
    VkRenderPass lightmapRenderPass  = VK_NULL_HANDLE;

    // ---- Command pool / frames -----------------------------------------

    VkCommandPool   commandPool = VK_NULL_HANDLE;
    VulkanFrameData frames[VULKAN_FRAMES_IN_FLIGHT];
    uint32_t        currentFrame = 0;
    uint32_t        currentImageIndex = UINT32_MAX;
    bool            lastSubmitSucceeded  = false; // guards Flush against presenting without a real submit
    uint32_t        activeDrawRangeOffset = 0;    // range->offset from SubmitDrawBuffer, passed as firstIndex

    // ---- Descriptor set layout / pipeline layout -----------------------

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout      pipelineLayout      = VK_NULL_HANDLE;
    VkPipelineCache       pipelineCache       = VK_NULL_HANDLE;

    // ---- Samplers ------------------------------------------------------

    VkSampler sampler       = VK_NULL_HANDLE;

    // ---- Compiled shader modules (loaded from SPV at init) -------------

    VkShaderModule shaderModules[VULKAN_SHADER_ID_COUNT][2] = {};
    // [id][0] = vertex, [id][1] = fragment
    VkShaderModule shaderModulesInst[VULKAN_SHADER_ID_COUNT][2] = {};
    // Instanced variants (USE_INSTANCING), only valid for mesh shader IDs

    // ---- Pipeline cache ------------------------------------------------

    std::vector<VulkanPipeline*> pipelines;
    std::unordered_map<uint32_t, VulkanPipeline*> pipelineMap;

    // ---- Shadow / light map render targets -----------------------------

    VulkanRenderTarget* shadowMapRenderTarget = nullptr;
    VulkanRenderTarget* lightMapRenderTarget  = nullptr;
    VulkanRenderTarget* activeRenderTarget    = nullptr;

    // ---- Empty texture (used for unused slots) -------------------------

    VulkanTexture2D* emptyTexture = nullptr;

    // ---- Active scene state --------------------------------------------

    bool activeScene     = false;
    bool use_shadow      = false;
    bool needClearDepth  = false;
    bool needClearColor  = false;
    sColor4f fillColor   = { 0, 0, 0, 1 };

    std::vector<VulkanCommand> swapchainCommands;

    // Textures created mid-frame are queued here and uploaded by DoVulkanRendering
    // before the first render pass, avoiding a vkQueueWaitIdle stall.
    std::vector<PendingTextureUpload> pendingTextureUploads;

    // Static VB/IB uploads triggered mid-frame are queued here and drained by
    // DoVulkanRendering onto the frame command buffer.
    std::vector<PendingBufferUpload>  pendingBufferUploads;

    // Monotonically increasing frame counter - incremented at each BeginScene.
    // Used to compute safe-deletion windows for deferred GPU resource destruction.
    uint64_t globalFrameIndex = 0;

    // Textures queued for deferred GPU resource destruction.
    // With FRAMES_IN_FLIGHT=2, at BeginScene(N) for slot S we've only waited for
    // slot S's fence (frame N-2).  Frame N-1 (the OTHER slot) may still be
    // executing on the GPU.  So resources must stay alive for at least
    // FRAMES_IN_FLIGHT more BeginScenes after they are enqueued.
    // Each entry carries the minimum globalFrameIndex at which it is safe to
    // delete.  DeleteTexture sets safeAfterFrame = globalFrameIndex + FRAMES_IN_FLIGHT.
    struct DeferredTextureDeletion {
        struct VulkanTexture2D* tex;
        uint64_t safeAfterFrame;
    };
    std::vector<DeferredTextureDeletion> pendingTextureDeletions;

    // Same deferred-deletion pattern for VulkanBuffers freed via
    // DeleteVertexBuffer / DeleteIndexBuffer.  A buffer can be referenced by an
    // in-flight CB recorded in either frame slot, so we wait FRAMES_IN_FLIGHT
    // BeginScenes before destroying it.
    struct DeferredBufferDeletion {
        struct VulkanBuffer* buf;
        uint64_t safeAfterFrame;
    };
    std::vector<DeferredBufferDeletion> pendingBufferDeletions;

    // Active command being assembled
    VulkanCommand*    activeCommand    = nullptr;
    VULKAN_PIPELINE_TYPE activePipelineType = VULKAN_PIPELINE_TYPE_DEFAULT;
    VulkanPipelineMode   activePipelineMode;
    VULKAN_MATERIAL_TYPE activeMaterial     = VULKAN_MAT_NONE;

    // Active state (fed into next command)
    VulkanTexture2D* activeTextures[VULKAN_TEXTURES] = {};
    Mat4f activeVP;
    Mat4f activeWorld;
    Mat4f orthoVP;
    Mat4f activeVPVk;   // activeVP * VK_CLIP_FIX (lazy-init)
    Mat4f orthoVPVk;    // orthoVP  * VK_CLIP_FIX (lazy-init)
    Vect3f activeEyePos = {0, 0, 0};
    // Cached MVP within a flush-window. activeWorld/activeVP changes call
    // FlushActiveCommand, so within a window the MVP is stable; only
    // recompute when this flag is set.
    Mat4f cachedMvp;
    bool  cachedMvpDirty   = true;
    bool  isOrthographicProjSet = false;
    Mat4f activeTexTransform[VULKAN_TEXTURES];
    eColorMode activeColorMode       = COLOR_MOD;
    float      activeTex2Lerp        = -1.0f;
    eAlphaTestMode activeAlphaTest   = ALPHATEST_NONE;
    sColor4f   activeDiffuse         = { 1, 1, 1, 1 };
    sColor4f   activeAmbient         = { 0, 0, 0, 0 };
    sColor4f   activeSpecular        = { 0, 0, 0, 0 };
    sColor4f   activeEmissive        = { 0, 0, 0, 0 };
    float      activePower           = 0.0f;
    bool       activeGlobalLight     = false;
    Vect3f     activeLightDir        = { 0, -1, 0 };
    sColor4f   activeLightDiffuse    = { 1, 1, 1, 1 };
    sColor4f   activeLightAmbient    = { 0.2f, 0.2f, 0.2f, 1 };
    sColor4f   activeLightSpecular   = { 1, 1, 1, 1 };
    sColor4f   activeTileColor       = { 1, 1, 1, 1 };
    Mat4f      activeShadowMatrix    = {};
    Vect2f     activeWorldSize       = { 1, 1 };

    // Viewport / clip
    Vect2i activeViewportPos  = { 0, 0 };
    Vect2i activeViewportSize = { 0, 0 };
    Vect2i activeClipPos      = { 0, 0 };
    Vect2i activeClipSize     = { 0, 0 };
    bool   hasViewport = false;
    bool   hasClip     = false;

    // Shadow camera pointer
    class cCamera* pShadowCamera = nullptr;

    // Deferred draw batching for instancing.
    // When m_deferredMode is active, SubmitDrawBuffer and SetWorldMat4f
    // accumulate draws instead of immediately flushing.  FlushDeferredBatch
    // groups by DrawRange and emits instanced draws for groups of identical meshes.
    struct DeferredMeshDraw {
        class VertexBuffer* vb;
        size_t              vertices;
        class IndexBuffer*  ib;
        size_t              indices;
        uint32_t            dbrOffset;   // DrawRange.offset
        size_t              dbrLen;      // DrawRange.len
        Mat4f               worldMatrix;
        VULKAN_SHADER_ID    shaderId;
        size_t              vsSize;
        size_t              fsSize;
    };
    bool m_deferredMode = false;
    bool m_batchingEnabled = true;
    std::vector<DeferredMeshDraw> m_deferredDraws;
    void EnterDeferredMode();
    void FlushDeferredBatch();

    // Instance data ring buffer helpers
    size_t AllocInstData(uint32_t frame, size_t byteSize);
    void   ResetInstRing(uint32_t frame);

    // ---- Private helpers -----------------------------------------------

    // Init helpers
    bool CreateInstance();
    bool SetupDebugMessenger();
    bool CreateSurface();
    bool PickPhysicalDevice();
    bool CreateLogicalDevice();
    bool CreateSwapchain();
    bool CreateSwapchainImageViews();
    bool CreateDepthResources();
    bool CreateRenderPasses();
    bool CreateFramebuffers();
    bool CreateCommandPool();
    bool CreateFrameData();
    bool CreateDescriptorSetLayout();
    bool CreatePipelineLayout();
    bool CreateSamplers();
    bool LoadShaderModules();
    bool CreateEmptyTexture();

    // Resource helpers
    uint32_t FindMemoryType(uint32_t typeBits, VkMemoryPropertyFlags props) const;
    bool     CreateImage(uint32_t w, uint32_t h, uint32_t mips, VkFormat fmt,
                         VkImageTiling tiling, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags memProps,
                         VkImage& image, VkDeviceMemory& memory) const;
    VkImageView CreateImageView(VkImage image, VkFormat fmt,
                                VkImageAspectFlags aspect, uint32_t mips = 1) const;
    void TransitionImageLayout(VkCommandBuffer cb, VkImage image,
                               VkImageLayout oldLayout, VkImageLayout newLayout,
                               VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT,
                               uint32_t mips = 1, uint32_t baseMip = 0) const;
    VkCommandBuffer BeginSingleTimeCommands() const;
    void            EndSingleTimeCommands(VkCommandBuffer cb) const;

    // Swapchain recreation
    void CleanupSwapchain();
    bool RecreateSwapchain();

    // UBO allocation from per-frame pool
    size_t AllocUBO(uint32_t frame, size_t size);
    void   WriteUBO(uint32_t frame, size_t offset, const void* data, size_t size);

    // Dynamic VTX/IDX ring buffer allocation
    size_t AllocDynVtx(uint32_t frame, size_t byteSize, size_t alignment);
    size_t AllocDynIdx(uint32_t frame, size_t byteSize);

    // Per-frame descriptor set allocation
    VkDescriptorSet AllocDescriptorSet(uint32_t frame);

    // Command helpers
    void FlushActiveCommand();
    void FinishActiveDrawBuffer();
    // indices      = total index buffer size (for GPU buffer allocation)
    // firstIndex   = first index to draw (DrawRange offset; 0 for dynamic buffers)
    // drawIdxCount = number of indices to draw (DrawRange len; 0 = use indices)
    void CreateVulkanCommand(class VertexBuffer* vb, size_t vertices,
                              class IndexBuffer*  ib, size_t indices,
                              uint32_t firstIndex = 0, size_t drawIdxCount = 0);
    std::vector<VulkanCommand>& GetActiveCommands();
    void ClearCommands(std::vector<VulkanCommand>& cmds);
    void ClearAllCommands();

    // Shader / UBO helpers
    VULKAN_SHADER_ID ChooseShader(vertex_fmt_t vfmt,
                                   VULKAN_PIPELINE_TYPE ptype,
                                   VULKAN_PIPELINE_TARGET target) const;
    void GetUBOSizes(VULKAN_SHADER_ID id, size_t& vsSize, size_t& fsSize) const;
    void FillUBOData(VULKAN_SHADER_ID id,
                     size_t vsOff, size_t vsSize,
                     size_t fsOff, size_t fsSize);
    float GetAlphaTestValue() const;

    VkDescriptorSet BuildDescriptorSet(uint32_t frame,
        VulkanTexture2D* tex0, VulkanTexture2D* tex1,
        VulkanTexture2D* tex2, VulkanTexture2D* tex3);

    // Internal texture/material helpers (VulkanRenderDraw.cpp)
    void SetColorMode(eColorMode mode);

    // Pipeline
    void WarmupPipelines();
    void SavePipelineCache();
    VulkanPipeline* GetOrCreatePipeline(VULKAN_SHADER_ID shaderId,
                                         VULKAN_PIPELINE_TARGET target,
                                         VULKAN_PIPELINE_TYPE   pipeType,
                                         const VulkanPipelineMode& mode,
                                         vertex_fmt_t vertFmt,
                                         ePrimitiveType primitive,
                                         bool instanced = false);

    void ClearPipelines();

    // Texture upload
    void UploadTexture(VulkanTexture2D* tex2d, const void* data, size_t size);
    void UploadTextureMip(VulkanTexture2D* tex, int mip,
                          const void* data, size_t dataSize,
                          int mipW, int mipH);
    VulkanTexture2D* GetOrCreateVulkanTex(cTexture* tex, int frame = 0);

    // Static VB/IB upload: creates DEVICE_LOCAL buffer and uploads via staging.
    // One-time synchronous cost per unique buffer; zero cost on subsequent frames.
    bool PrepareStaticBuffer(VulkanBuffer*& vkBuf, const void* data,
                              size_t dataLen, VkBufferUsageFlags usage);

    // Depth format detection
    VkFormat FindDepthFormat() const;

    // Load SPV shader file
    VkShaderModule LoadShaderModule(const char* spvPath) const;

    // Destroy helpers
    void DestroyFrameData();
    void DestroyShaderModules();
};
