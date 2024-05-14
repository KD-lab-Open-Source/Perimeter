#include <unordered_set>
#include "StdAfxRD.h"
#include "xmath.h"
#include "Umath.h"
#include <sokol_gfx.h>
#include <sokol_log.h>
#include "SokolResources.h"
#include "IRenderDevice.h"
#include "SokolRender.h"
#include "SokolRenderPipeline.h"
#include "xerrhand.h"
#include "DrawBuffer.h"
#include "SokolShaders.h"
#include "RenderTracker.h"
#include <SDL_hints.h>

#ifdef SOKOL_GL
#include <SDL_opengl.h>
#endif

#ifdef SOKOL_D3D11
#define RENDERUTILS_HWND_FROM_SDL_WINDOW
#include "RenderUtils.h"
#include "SokolD3D.h"
#endif

#ifdef SOKOL_METAL
void sokol_metal_setup(SDL_Window* sdl_window, sg_desc* desc, sg_swapchain* swapchain, uint32_t ScreenHZ);
void sokol_metal_destroy(sg_swapchain* swapchain);
#endif

cSokolRender::cSokolRender() = default;

cSokolRender::~cSokolRender() {
    Done();
};

eRenderDeviceSelection cSokolRender::GetRenderSelection() const {
    return DEVICE_SOKOL;
}

uint32_t cSokolRender::GetWindowCreationFlags() const {
    uint32_t flags = cInterfaceRenderDevice::GetWindowCreationFlags();
#ifdef SOKOL_GL
    flags |= SDL_WINDOW_OPENGL;
#endif
#ifdef SOKOL_METAL
    flags |= SDL_WINDOW_METAL;
#endif
#if defined(SOKOL_D3D11) && !defined(_WIN32)
    //On non Windows we use dxvk which uses Vulkan
    flags |= SDL_WINDOW_VULKAN;
#endif
    return flags;
}

int cSokolRender::Init(int xScr, int yScr, int mode, SDL_Window* wnd, int RefreshRateInHz) {
    RenderSubmitEvent(RenderEvent::INIT, "Sokol start");

    int res = cInterfaceRenderDevice::Init(xScr, yScr, mode, wnd, RefreshRateInHz);
    if (res != 0) {
        return res;
    }

    ClearCommands();
    ClearPipelines();
    
    const char* sokol_backend = "Unknown";
    
    //Init some state
    activePipelineType = PIPELINE_TYPE_TRIANGLE;
    activePipelineMode.cull = CameraCullMode = CULL_CCW;
    
    //Prepare sokol gfx desc
    sg_desc desc = {};
    desc.pipeline_pool_size = PERIMETER_SOKOL_PIPELINES_MAX,
    desc.shader_pool_size = 8,
    desc.buffer_pool_size = 1024 * 32;
    desc.uniform_buffer_size = 1024 * 1024 * 32;
    desc.image_pool_size = 1024 * 4; //1024 is enough for PGW+PET game
    desc.logger.func = slog_func;
    
    //Setup swapchain pass
    swapchain_pass = {};
    swapchain_pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
    swapchain_pass.action.colors[0].store_action = SG_STOREACTION_STORE;
    swapchain_pass.action.colors[0].clear_value = sg_color { 0.0f, 0.0f, 0.0f, 1.0f };
    swapchain_pass.action.depth.load_action = SG_LOADACTION_CLEAR;
    swapchain_pass.action.depth.store_action = SG_STOREACTION_DONTCARE;
    swapchain_pass.action.depth.clear_value = 1.0f;
    swapchain_pass.action.stencil.load_action = SG_LOADACTION_CLEAR;
    swapchain_pass.action.stencil.store_action = SG_STOREACTION_DONTCARE;
    swapchain_pass.action.stencil.clear_value = 0;
    swapchain_pass.swapchain.width = ScreenSize.x;
    swapchain_pass.swapchain.height = ScreenSize.y;
    swapchain_pass.swapchain.sample_count = 1;

    //OpenGL / OpenGLES
#ifdef SOKOL_GL
    //Setup some attributes before context creation
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    //Fullscreen anti-aliasing
    //TODO make this configurable, or maybe use sokol?
    /*
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
    //*/

#ifdef SOKOL_GLCORE
    //Use OpenGL Core
    sokol_backend = "OpenGL Core";
    
    SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengl", SDL_HINT_OVERRIDE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif

#if defined(SOKOL_GLES3)
    //Use OpenGLES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    sokol_backend = "OpenGLES3";
    SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengles", SDL_HINT_OVERRIDE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
#endif
    
    // Create an OpenGL context associated with the window.
    sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (sdl_gl_context == nullptr) {
        ErrH.Abort("Error creating SDL GL Context", XERR_CRITICAL, 0, SDL_GetError());
    }
    printf("GPU vendor: %s, renderer: %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDER));
    
    swapchain_pass.swapchain.gl.framebuffer = 0;
#endif //SOKOL_GL

    //Direct3D
#ifdef SOKOL_D3D11
    sokol_backend = "Direct3D 11";
    SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "direct3d11", SDL_HINT_OVERRIDE);
    
    //Create context
    d3d_context = new sokol_d3d_context();

    //Get hwnd
    d3d_context->hwnd = get_hwnd_from_sdl_window(sdl_window);

    // Create D3D11 Device and Context
    D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
    };
    UINT create_flags = 0;
    //We will make d3d11 calls only from render thread so this should be safe
    create_flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
    create_flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT; 
#if defined(PERIMETER_DEBUG) && 0 
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL feature_level;
    HRESULT hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            create_flags,
            featureLevels,
            1,
            D3D11_SDK_VERSION,
            &d3d_context->device,
            &feature_level,
            &d3d_context->device_context
    );

    printf("D3D Feature Level: %" PRIu32 "\n", feature_level);
    if (!SUCCEEDED(hr) || !d3d_context->device || !d3d_context->device_context) {
        fprintf(stderr, "Error creating D3D device: HR 0x%" PRIX32 "\n", static_cast<uint32_t>(hr));
        xassert(0);
        return false;
    }

    //Get DXGI Factory (needed to create Swap Chain)
    IDXGIFactory* dxgiFactory;
    IDXGIDevice* dxgiDevice;
    hr = d3d_context->device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if (!SUCCEEDED(hr)) {
        fprintf(stderr, "Error getting DXGI device: HR 0x%" PRIX32 "\n", static_cast<uint32_t>(hr));
        xassert(0);
        return false;
    }

    IDXGIAdapter* dxgiAdapter;
    hr = dxgiDevice->GetAdapter(&dxgiAdapter);
    if (!SUCCEEDED(hr)) {
        fprintf(stderr, "Error getting DXGI adapter: HR 0x%" PRIX32 "\n", static_cast<uint32_t>(hr));
        xassert(0);
        return false;
    }
    dxgiDevice->Release();

    DXGI_ADAPTER_DESC adapterDesc;
    dxgiAdapter->GetDesc(&adapterDesc);

#ifdef PERIMETER_DEBUG
    printf("DXGI Graphics Device: %ls\n", adapterDesc.Description);
#endif

    hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));
    if (!SUCCEEDED(hr)) {
        fprintf(stderr, "Error getting DXGI factory: HR 0x%" PRIX32 "\n", static_cast<uint32_t>(hr));
        xassert(0);
        return false;
    }
    dxgiAdapter->Release();

    //Create swap chain
    uint32_t sample_count = swapchain_pass.swapchain.sample_count;
    DXGI_SWAP_CHAIN_DESC* swap_chain_desc = &d3d_context->swap_chain_desc;
    memset(&d3d_context->swap_chain_desc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
    swap_chain_desc->BufferDesc.Width = static_cast<uint32_t>(xScr);
    swap_chain_desc->BufferDesc.Height = static_cast<uint32_t>(yScr);
    swap_chain_desc->BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swap_chain_desc->BufferDesc.RefreshRate = { static_cast<uint32_t>(RefreshRateInHz), 1 };
    swap_chain_desc->OutputWindow = d3d_context->hwnd;
    swap_chain_desc->Windowed = (mode & RENDERDEVICE_MODE_WINDOW) != 0;
    swap_chain_desc->SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_desc->BufferCount = 1;
    swap_chain_desc->SampleDesc.Count = sample_count;
    swap_chain_desc->SampleDesc.Quality = 1 < sample_count ? static_cast<uint32_t>(D3D11_STANDARD_MULTISAMPLE_PATTERN) : 0;
    swap_chain_desc->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    hr = dxgiFactory->CreateSwapChain(d3d_context->device, swap_chain_desc, &d3d_context->swap_chain);
    if (!SUCCEEDED(hr) || !d3d_context->swap_chain) {
        fprintf(stderr, "Error creating swap chain: HR 0x%" PRIX32 "\n", static_cast<uint32_t>(hr));
        xassert(0);
        return false;
    }

    dxgiFactory->Release();

    // default render target and depth-stencil-buffer
    d3d_CreateDefaultRenderTarget();

    //Setup d3d context
    desc.environment.d3d11.device = d3d_context->device;
    desc.environment.d3d11.device_context = d3d_context->device_context;
#endif
    
#ifdef SOKOL_METAL
    sokol_backend = "Metal";
    SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "metal", SDL_HINT_OVERRIDE);

    //Setup metal sokol context
    sokol_metal_setup(sdl_window, &desc, &swapchain_pass.swapchain, ScreenHZ);
#endif

    const char* render_driver = SDL_GetHint(SDL_HINT_RENDER_DRIVER);
    printf("SDL / Sokol render driver: %s\n", render_driver);
    printf("Sokol render backend: %s\n", sokol_backend);
    
    //Call sokol gfx setup
    sg_setup(&desc);
#ifdef PERIMETER_DEBUG
    printf("cSokolRender::Init sg_setup done\n");
#endif

    //Create sampler
    sg_sampler_desc sampler_desc = {};
    sampler_desc.label = "SamplerLinear";
    sampler_desc.wrap_u = SG_WRAP_REPEAT;
    sampler_desc.wrap_v = SG_WRAP_REPEAT;
    sampler_desc.min_lod = 0.0f;
    sampler_desc.max_lod = 0.0f;    // for max_lod, zero-initialized means "FLT_MAX"
    //Filter must be linear for small font textures to not look unreadable
    sampler_desc.min_filter = SG_FILTER_LINEAR;
    sampler_desc.mag_filter = SG_FILTER_LINEAR;
    sampler_desc.mipmap_filter = SG_FILTER_LINEAR;
    sampler = sg_make_sampler(sampler_desc);
    
    //Create empty texture
    sg_image_desc* imgdesc = new sg_image_desc();
    imgdesc->label = nullptr;
    imgdesc->width = imgdesc->height = 64;
    imgdesc->pixel_format = SG_PIXELFORMAT_RGBA8;
    imgdesc->num_mipmaps = 1;
    imgdesc->usage = SG_USAGE_IMMUTABLE;
    size_t pixel_len = sokol_pixelformat_bytesize(imgdesc->pixel_format);
    size_t buf_len = imgdesc->height * imgdesc->width * pixel_len;
    uint8_t* buf = new uint8_t[buf_len];
    memset(buf, 0xFF, buf_len);
    imgdesc->data.subimage[0][0] = { buf, buf_len };
    emptyTexture = new SokolTexture2D(imgdesc);

#ifdef PERIMETER_DEBUG
    emptyTexture->label = "EmptySlotTexture";
#endif

    for (int i = 0; i < PERIMETER_SOKOL_TEXTURES; ++i) {
        activeTextureTransform[i] = Mat4f::ID;
    }

    RenderSubmitEvent(RenderEvent::INIT, "Sokol done");
    return UpdateRenderMode();
}

bool cSokolRender::ChangeSize(int xScr, int yScr, int mode) {
    int mode_mask = RENDERDEVICE_MODE_WINDOW;

    bool need_resize = xScr != ScreenSize.x || yScr != ScreenSize.y;

    if (!need_resize && (RenderMode&mode_mask) == mode) {
        return true; //Nothing to do
    }

    ScreenSize.x = xScr;
    ScreenSize.y = yScr;
    RenderMode &= ~mode_mask;
    RenderMode |= mode;
    
    //Update swapchain
    swapchain_pass.swapchain.width = ScreenSize.x;
    swapchain_pass.swapchain.height = ScreenSize.y;

#ifdef SOKOL_D3D11
    if (d3d_context->swap_chain && need_resize) {
        d3d_DestroyDefaultRenderTarget();
        d3d_context->swap_chain->ResizeBuffers( 
            1,
            static_cast<uint32_t>(ScreenSize.x),
            static_cast<uint32_t>(ScreenSize.y),
            DXGI_FORMAT_B8G8R8A8_UNORM, 
            0
        );
        d3d_CreateDefaultRenderTarget();
    }
#endif
    
    return UpdateRenderMode() == 0;
}

int cSokolRender::UpdateRenderMode() {
    RenderSubmitEvent(RenderEvent::UPDATE_MODE);
    orthoVP = Mat4f::ID;
    SetOrthographic(orthoVP, ScreenSize.x, -ScreenSize.y, 10, -10);
    return 0;
}

int cSokolRender::Done() {
    RenderSubmitEvent(RenderEvent::DONE, "Sokol start");
    bool do_sg_shutdown = sdl_window != nullptr;
    int ret = cInterfaceRenderDevice::Done();
    activeCommand.Clear();
    ClearCommands();
    ClearPipelines();
    shaders.clear();
    delete emptyTexture;
    emptyTexture = nullptr;
    //At this point sokol stuff should be cleaned up, is important as calling sg_* after this will result in crashes
    //Make sure is called only once, as repeated shutdowns may crash in some backends/OSes
    if (do_sg_shutdown) {
        RenderSubmitEvent(RenderEvent::DONE, "Sokol shutdown");
        sg_shutdown();
    }
#ifdef SOKOL_D3D11
    if (d3d_context) {
        RenderSubmitEvent(RenderEvent::DONE, "Sokol D3D11 shutdown");
        d3d_DestroyDefaultRenderTarget();
        RELEASE(d3d_context->swap_chain);
        RELEASE(d3d_context->device_context);
        RELEASE(d3d_context->device);
        delete d3d_context;
        d3d_context = nullptr;
    }
#endif
#ifdef SOKOL_METAL
    if (swapchain_pass.swapchain.metal.current_drawable != nullptr) {
        RenderSubmitEvent(RenderEvent::DONE, "Sokol Metal shutdown");
        sokol_metal_destroy(&swapchain_pass.swapchain);
    }
#endif
#ifdef SOKOL_GL
    if (sdl_gl_context != nullptr) {
        RenderSubmitEvent(RenderEvent::DONE, "Sokol GL shutdown");
        SDL_GL_DeleteContext(sdl_gl_context);
        sdl_gl_context = nullptr;
    }
#endif
    RenderSubmitEvent(RenderEvent::DONE, "Sokol done");
    return ret;
}

int cSokolRender::SetGamma(float fGamma, float fStart, float fFinish) {
    //TODO
    fprintf(stderr, "cSokolRender::SetGamma not implemented!\n");
    return -1;
}

void cSokolRender::DeleteVertexBuffer(VertexBuffer &vb) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    RenderSubmitEvent(RenderEvent::DELETE_VERTEXBUF, "", &vb);
#endif
    delete vb.sg;
    vb.sg = nullptr;
    vb.FreeData();
}

void cSokolRender::DeleteIndexBuffer(IndexBuffer &ib) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    RenderSubmitEvent(RenderEvent::DELETE_INDEXBUF, "", &ib);
#endif
    delete ib.sg;
    ib.sg = nullptr;
    ib.FreeData();
}

void cSokolRender::ClearCommands() {
    std::unordered_set<SokolResourceBuffer*> pooled;
    for (SokolCommand* command : commands) {
        //Reclaim resources that can be reused
        SokolResourceBuffer* vertex_buffer = command->vertex_buffer;
        if (vertex_buffer && vertex_buffer->key != SokolResourceKeyNone
        && (vertex_buffer->RefCount() == 1 || pooled.count(vertex_buffer) == 0)) {
            command->vertex_buffer = nullptr;
            xassert(0 < vertex_buffer->RefCount() && vertex_buffer->RefCount() <= 50);
            bufferPool.emplace(vertex_buffer->key, vertex_buffer);
            pooled.emplace(vertex_buffer);
        }
        SokolResourceBuffer* index_buffer = command->index_buffer;
        if (index_buffer && index_buffer->key != SokolResourceKeyNone
        && (index_buffer->RefCount() == 1 || pooled.count(index_buffer) == 0)) {
            command->index_buffer = nullptr;
            xassert(0 < index_buffer->RefCount() && index_buffer->RefCount() <= 50);
            bufferPool.emplace(index_buffer->key, index_buffer);
            pooled.emplace(index_buffer);
        }
        
        delete command;
    }
    commands.clear();
#ifdef PERIMETER_DEBUG
    //printf("%ld %ld\n", reclaimed.size(), bufferPool.size());
#endif
}

void cSokolRender::ClearPipelines() {
    for (auto pipeline : pipelines) {
        delete pipeline.second;
    }
    pipelines.clear();
}

int cSokolRender::GetClipRect(int *xmin,int *ymin,int *xmax,int *ymax) {
    *xmin = activeCommand.clip[0].x;
    *ymin = activeCommand.clip[0].y;
    *xmax = activeCommand.clip[1].x + activeCommand.clip[0].x;
    *ymax = activeCommand.clip[1].y + activeCommand.clip[0].y;
    return 0;
}

int cSokolRender::SetClipRect(int xmin,int ymin,int xmax,int ymax) {
    int w = xmax-xmin;
    int h = ymax-ymin;
    if (activeCommand.clip[0].x == xmin && activeCommand.clip[0].y == ymin
        && activeCommand.clip[1].x == w && activeCommand.clip[1].y == h) {
        //Nothing to do
        return 0;
    }
    FinishActiveDrawBuffer();
    activeCommand.clip[0].x = xmin;
    activeCommand.clip[0].y = ymin;
    activeCommand.clip[1].x = w;
    activeCommand.clip[1].y = h;
    return 0;
}

void cSokolRender::ResetViewport() {
    if (activeCommand.viewport[0].x == 0
    && activeCommand.viewport[0].y == 0
    && activeCommand.viewport[1] == ScreenSize) {
        //Nothing to do
        return;
    }
    FinishActiveDrawBuffer();
    activeCommand.viewport[0].x = 0;
    activeCommand.viewport[0].y = 0;
    activeCommand.viewport[1] = ScreenSize;;
}

bool cSokolRender::SetScreenShot(const char *fname) {
    //TODO
    fprintf(stderr, "cSokolRender::SetScreenShot not implemented!\n");
    return false;
}

bool cSokolRender::IsEnableSelfShadow() {
    //TODO
    return false;
}

#ifdef SOKOL_D3D11
void cSokolRender::d3d_CreateDefaultRenderTarget() {
    HRESULT hr;
    hr = d3d_context->swap_chain->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            reinterpret_cast<void**>(&d3d_context->render_target_texture)
    );
    assert(SUCCEEDED(hr) && d3d_context->render_target_texture);
    hr = d3d_context->device->CreateRenderTargetView(
            d3d_context->render_target_texture,
            nullptr,
            &d3d_context->render_target_view
    );
    assert(SUCCEEDED(hr) && d3d_context->render_target_view);

    D3D11_TEXTURE2D_DESC ds_desc = {};
    ds_desc.Width = static_cast<uint32_t>(ScreenSize.x);
    ds_desc.Height = static_cast<uint32_t>(ScreenSize.y);
    ds_desc.MipLevels = 1;
    ds_desc.ArraySize = 1;
    ds_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    ds_desc.SampleDesc = d3d_context->swap_chain_desc.SampleDesc;
    ds_desc.Usage = D3D11_USAGE_DEFAULT;
    ds_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    
    // MSAA render target and view
    uint32_t sample_count = ds_desc.SampleDesc.Count;
    if (1 < sample_count) {
        hr = d3d_context->device->CreateTexture2D(&ds_desc, nullptr, &d3d_context->msaa_texture);
        assert(SUCCEEDED(hr) && d3d_context->msaa_texture);
        hr = d3d_context->device->CreateRenderTargetView(d3d_context->msaa_texture, nullptr, &d3d_context->msaa_view);
        assert(SUCCEEDED(hr) && d3d_context->msaa_view);
    }

    // Depth-stencil render target and view
    ds_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    ds_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = d3d_context->device->CreateTexture2D(&ds_desc, nullptr, &d3d_context->depth_stencil_texture);
    assert(SUCCEEDED(hr) && d3d_context->depth_stencil_texture);
    hr = d3d_context->device->CreateDepthStencilView(d3d_context->depth_stencil_texture, nullptr, &d3d_context->depth_stencil_view);
    assert(SUCCEEDED(hr) && d3d_context->depth_stencil_view);
    
    //Apply into swapchain pass
    swapchain_pass.swapchain.d3d11.render_view = 1 < sample_count ? d3d_context->msaa_view : d3d_context->render_target_view;
    swapchain_pass.swapchain.d3d11.resolve_view = 1 < sample_count ? d3d_context->render_target_view : nullptr;
    swapchain_pass.swapchain.d3d11.depth_stencil_view = d3d_context->depth_stencil_view;
}

void cSokolRender::d3d_DestroyDefaultRenderTarget() {
    swapchain_pass.swapchain.d3d11.render_view = nullptr;
    swapchain_pass.swapchain.d3d11.resolve_view = nullptr;
    swapchain_pass.swapchain.d3d11.depth_stencil_view = nullptr;
    RELEASE(d3d_context->render_target_texture);
    RELEASE(d3d_context->render_target_view);
    RELEASE(d3d_context->depth_stencil_texture);
    RELEASE(d3d_context->depth_stencil_view);
    RELEASE(d3d_context->msaa_texture);
    RELEASE(d3d_context->msaa_view);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////

SokolCommand::SokolCommand() {
}

SokolCommand::~SokolCommand() {
    Clear();
}

void SokolCommand::CreateShaderParams() {
    switch (shader_id) {
        default:
        case SOKOL_SHADER_ID_NONE:
            xassert(0);
            break;
        case SOKOL_SHADER_ID_color_tex1:
        case SOKOL_SHADER_ID_color_tex2:
            vs_params = new color_texture_vs_params_t();
            fs_params = new color_texture_fs_params_t();
            vs_params_len = sizeof(color_texture_vs_params_t);            
            fs_params_len = sizeof(color_texture_fs_params_t);
            break;
        case SOKOL_SHADER_ID_normal:
            vs_params = new normal_texture_vs_params_t();
            fs_params = new normal_texture_fs_params_t();
            vs_params_len = sizeof(normal_texture_vs_params_t);            
            fs_params_len = sizeof(normal_texture_fs_params_t);
            break;
        case SOKOL_SHADER_ID_terrain:
            vs_params = new terrain_vs_params_t();
            fs_params = new terrain_fs_params_t();
            vs_params_len = sizeof(terrain_vs_params_t);            
            fs_params_len = sizeof(terrain_fs_params_t);
            break;
    }
}

void SokolCommand::ClearDrawData() {
    if (vertex_buffer) {
        vertex_buffer->DecRef();
        vertex_buffer = nullptr;
    }
    if (index_buffer) {
        index_buffer->DecRef();
        index_buffer = nullptr;
    }
    vertices = 0;
    indices = 0;
}

void SokolCommand::ClearShaderParams() {
    switch (shader_id) {
        default:
        case SOKOL_SHADER_ID_NONE:
            break;
        case SOKOL_SHADER_ID_color_tex1:
        case SOKOL_SHADER_ID_color_tex2:
            delete reinterpret_cast<color_texture_vs_params_t*>(vs_params);
            delete reinterpret_cast<color_texture_fs_params_t*>(fs_params);
            break;
        case SOKOL_SHADER_ID_normal:
            delete reinterpret_cast<normal_texture_vs_params_t*>(vs_params);
            delete reinterpret_cast<normal_texture_fs_params_t*>(fs_params);
            break;
        case SOKOL_SHADER_ID_terrain:
            delete reinterpret_cast<terrain_vs_params_t*>(vs_params);
            delete reinterpret_cast<terrain_fs_params_t*>(fs_params);
            break;
    }
    vs_params = nullptr;
    fs_params = nullptr;
    vs_params_len = 0;
    fs_params_len = 0;
}

void SokolCommand::ClearTextures() {
    for (int i = 0; i < PERIMETER_SOKOL_TEXTURES; ++i) {
        SetTexture(i, nullptr);
    }
}

void SokolCommand::Clear() {
    ClearDrawData();
    ClearShaderParams();
    ClearTextures();
}

void SokolCommand::SetTexture(size_t index, SokolResourceTexture* texture) {
    xassert(index<PERIMETER_SOKOL_TEXTURES);
    if (texture) {
        texture->IncRef();
    }
    if (sokol_textures[index]) {
        sokol_textures[index]->DecRef();
    }
    sokol_textures[index] = texture;
}
