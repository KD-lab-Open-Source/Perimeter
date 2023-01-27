#include "StdAfxRD.h"
#include "xmath.h"
#include "Umath.h"
#include "sokol_gfx.h"
#include "SokolResources.h"
#include "IRenderDevice.h"
#include "SokolRender.h"
#include "SokolRenderPipeline.h"
#include "xerrhand.h"
#include "DrawBuffer.h"
#include "SokolShaders.h"
#include "RenderTracker.h"
#include <SDL_hints.h>

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
    return flags;
}

int cSokolRender::Init(int xScr, int yScr, int mode, void* wnd, int RefreshRateInHz) {
    RenderSubmitEvent(RenderEvent::INIT, "Sokol start");
    int res = cInterfaceRenderDevice::Init(xScr, yScr, mode, wnd, RefreshRateInHz);
    if (res != 0) {
        return res;
    }

    ClearCommands();
    ClearPipelines();
    
    sdlWindow = static_cast<SDL_Window*>(wnd);
    
    //Init some state
    activePipelineType = PIPELINE_TYPE_TRIANGLE;
    activePipelineMode.cull = CameraCullMode = CULL_CCW;
    
    //Prepare sokol gfx desc
    sg_desc desc = {};
    desc.pipeline_pool_size = PERIMETER_SOKOL_PIPELINES_MAX,
    desc.shader_pool_size = 8,
    desc.buffer_pool_size = 2048 * 2; //2048 is enough for PGW+PET game
    desc.image_pool_size = 1024 * 4; //1024 is enough for PGW+PET game
    desc.context.color_format = SG_PIXELFORMAT_RGBA8;
    desc.context.depth_format = SG_PIXELFORMAT_DEPTH_STENCIL;

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

#ifdef SOKOL_GLCORE33
    //Use OpenGL 3.3 Core
    SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengl", SDL_HINT_OVERRIDE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif

#if defined(SOKOL_GLES2) || defined(SOKOL_GLES3)
    //Use OpenGLES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#ifdef SOKOL_GLES2
    SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengles2", SDL_HINT_OVERRIDE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
#else
    SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengles", SDL_HINT_OVERRIDE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
#endif
#endif
#endif

#ifdef SOKOL_D3D11
    SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "direct3d", SDL_HINT_OVERRIDE);
#endif
    
#ifdef SOKOL_METAL
    SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "metal", SDL_HINT_OVERRIDE);
#endif

    const char* render_driver = SDL_GetHint(SDL_HINT_RENDER_DRIVER);
    printf("SDL render driver: %s\n", render_driver);

#ifdef SOKOL_METAL
    // Obtain Metal device by creating a SDL Metal View, also useful to retain the Metal device
    sdlMetalView = SDL_Metal_CreateView(sdlWindow);
    if (sdlMetalView == nullptr) {
        ErrH.Abort("Error creating SDL Metal View", XERR_CRITICAL, 0, SDL_GetError());
    }
    sokol_metal_setup_desc(sdlMetalView, &desc);
#endif

#ifdef SOKOL_GL
    // Create an OpenGL context associated with the window.
    sdlGlContext = SDL_GL_CreateContext(sdlWindow);
    if (sdlGlContext == nullptr) {
        ErrH.Abort("Error creating SDL GL Context", XERR_CRITICAL, 0, SDL_GetError());
    }
#endif
    
    //Call sokol gfx setup
    sg_setup(&desc);
#ifdef PERIMETER_DEBUG
    printf("cSokolRender::Init sg_setup done\n");
#endif
    
    //Create empty texture
    sg_image_desc* imgdesc = new sg_image_desc();
    imgdesc->label = "EmptySlotTexture";
    imgdesc->width = imgdesc->height = 64;
    imgdesc->wrap_u = imgdesc->wrap_v = SG_WRAP_REPEAT;
    imgdesc->pixel_format = SG_PIXELFORMAT_RGBA8;
    imgdesc->num_mipmaps = 1;
    imgdesc->min_filter = imgdesc->mag_filter = SG_FILTER_NEAREST;
    imgdesc->usage = SG_USAGE_IMMUTABLE;
    size_t pixel_len = sokol_pixelformat_bytesize(imgdesc->pixel_format);
    size_t buf_len = imgdesc->height * imgdesc->width * pixel_len;
    uint8_t* buf = new uint8_t[buf_len];
    memset(buf, 0xFF, buf_len);
    imgdesc->data.subimage[0][0] = { buf, buf_len };
    emptyTexture = new SokolTexture2D(imgdesc);

    RenderSubmitEvent(RenderEvent::INIT, "Sokol done");
    return UpdateRenderMode();
}

bool cSokolRender::ChangeSize(int xScr, int yScr, int mode) {
    int mode_mask = RENDERDEVICE_MODE_WINDOW;

    if (xScr == ScreenSize.x && yScr == ScreenSize.y && (RenderMode&mode_mask) == mode) {
        return true; //Nothing to do
    }

    ScreenSize.x = xScr;
    ScreenSize.y = yScr;
    RenderMode &= ~mode_mask;
    RenderMode |= mode;
    
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
    int ret = cInterfaceRenderDevice::Done();
    ClearCommands();
    ClearPipelines();
    shaders.clear();
    delete emptyTexture;
    emptyTexture = nullptr;
    if (sdlWindow) {
        //Make sure is called only once, as it may crash in some backends/OSes
        sg_shutdown();
    }
#ifdef SOKOL_METAL
    if (sdlMetalView != nullptr) {
        SDL_Metal_DestroyView(sdlMetalView);
        sdlMetalView = nullptr;
    }
#endif
#ifdef SOKOL_GL
    if (sdlGlContext != nullptr) {
        SDL_GL_DeleteContext(sdlGlContext);
        sdlGlContext = nullptr;
    }
#endif
    sdlWindow = nullptr;
    RenderSubmitEvent(RenderEvent::DONE, "Sokol done");
    return ret;
}

int cSokolRender::SetGamma(float fGamma, float fStart, float fFinish) {
    //TODO
    fprintf(stderr, "cSokolRender::SetGamma not implemented!\n");
    return -1;
}

void cSokolRender::CreateVertexBuffer(VertexBuffer& vb, uint32_t NumberVertex, vertex_fmt_t fmt, bool dynamic) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    std::string label = "Len: " + std::to_string(NumberVertex)
                      + " Fmt: " + std::to_string(fmt)
                      + " Dyn: " + std::to_string(dynamic);
    RenderSubmitEvent(RenderEvent::CREATE_VERTEXBUF, label.c_str(), &vb);
#endif
    xassert(!vb.sg);
    xassert(NumberVertex <= std::numeric_limits<indices_t>().max());
    size_t size = GetSizeFromFormat(fmt);

    sg_buffer_desc* desc = new sg_buffer_desc();
    desc->size = NumberVertex * size;
    desc->type = SG_BUFFERTYPE_VERTEXBUFFER;
    desc->usage = dynamic ? SG_USAGE_STREAM : SG_USAGE_IMMUTABLE;
    desc->label = "CreateVertexBuffer";

    vb.VertexSize = size;
    vb.fmt = fmt;
    vb.dynamic = dynamic;
    vb.NumberVertex = NumberVertex;
    vb.sg = new SokolBuffer(desc);
}

void cSokolRender::DeleteVertexBuffer(VertexBuffer &vb) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    RenderSubmitEvent(RenderEvent::DELETE_VERTEXBUF, "", &vb);
#endif
    delete vb.sg;
    vb.sg = nullptr;
}

void* cSokolRender::LockVertexBuffer(VertexBuffer &vb) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::LOCK_VERTEXBUF, "", &vb);
#endif
    if (!vb.sg) {
        xassert(0);
        return nullptr;
    }
    xassert(!vb.sg->locked);
    vb.sg->dirty = true;
    vb.sg->locked = true;
    return vb.sg->data;
}

void* cSokolRender::LockVertexBuffer(VertexBuffer &vb, uint32_t Start, uint32_t Amount) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    std::string label = "Idx: " + std::to_string(Start) + " Len: " + std::to_string(Amount);
    RenderSubmitEvent(RenderEvent::LOCK_VERTEXBUF, label.c_str(), &vb);
#endif
    xassert(Start + Amount <= vb.NumberVertex);
    return &static_cast<uint8_t*>(LockVertexBuffer(vb))[vb.VertexSize * Start];
}

void cSokolRender::UnlockVertexBuffer(VertexBuffer &vb) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::UNLOCK_VERTEXBUF, "", &vb);
#endif
    xassert(vb.sg->locked);
    vb.sg->locked = false;
}

void cSokolRender::CreateIndexBuffer(IndexBuffer& ib, uint32_t NumberIndices, bool dynamic) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    std::string label = "Len: " + std::to_string(NumberIndices)
                      + " Dyn: " + std::to_string(dynamic);
    RenderSubmitEvent(RenderEvent::CREATE_INDEXBUF, label.c_str(), &ib);
#endif
    xassert(!ib.sg);
    ib.NumberIndices = NumberIndices;
    ib.dynamic = dynamic;
    
    sg_buffer_desc* desc = new sg_buffer_desc();
    desc->size = ib.NumberIndices * sizeof(indices_t);
    desc->type = SG_BUFFERTYPE_INDEXBUFFER;
    desc->usage = dynamic ? SG_USAGE_STREAM : SG_USAGE_IMMUTABLE;
    desc->label = "CreateIndexBuffer";
    ib.sg = new SokolBuffer(desc);
}

void cSokolRender::DeleteIndexBuffer(IndexBuffer &ib) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    RenderSubmitEvent(RenderEvent::DELETE_INDEXBUF, "", &ib);
#endif
    delete ib.sg;
    ib.sg = nullptr;
}

indices_t* cSokolRender::LockIndexBuffer(IndexBuffer &ib) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::LOCK_INDEXBUF, "", &ib);
#endif
    if (!ib.sg) {
        xassert(0);
        return nullptr;
    }
    xassert(!ib.sg->locked);
    ib.sg->dirty = true;
    ib.sg->locked = true;
    return static_cast<indices_t*>(ib.sg->data);
}

indices_t* cSokolRender::LockIndexBuffer(IndexBuffer &ib, uint32_t Start, uint32_t Amount) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    std::string label = "Idx: " + std::to_string(Start) + " Len: " + std::to_string(Amount);
    RenderSubmitEvent(RenderEvent::LOCK_INDEXBUF, label.c_str(), &ib);
#endif
    xassert(Start + Amount <= ib.NumberIndices);
    return &LockIndexBuffer(ib)[Start];
}

void cSokolRender::UnlockIndexBuffer(IndexBuffer &ib) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::UNLOCK_INDEXBUF, "", &ib);
#endif
    xassert(ib.sg->locked);
    ib.sg->locked = false;
}

void cSokolRender::ClearCommands() {
    for (SokolCommand* command : commands) {
        delete command;
    }
    commands.clear();
}

void cSokolRender::ClearPipelines() {
    for (auto pipeline : pipelines) {
        delete pipeline.second;
    }
    pipelines.clear();
}

int cSokolRender::GetClipRect(int *xmin,int *ymin,int *xmax,int *ymax) {
    *xmin=viewportPos.x; *xmax=viewportSize.x + viewportPos.x;
    *ymin=viewportPos.y; *ymax=viewportSize.y + viewportPos.y;
    return 0;
}

int cSokolRender::SetClipRect(int xmin,int ymin,int xmax,int ymax) {
    viewportPos.x = xmin;
    viewportPos.y = ymin;
    viewportSize.x = xmax-xmin;
    viewportSize.y = ymax-ymin;
    return 0;
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

////////////////////////////////////////////////////////////////////////////////////////////

SokolCommand::SokolCommand() {
    Clear();
}

SokolCommand::~SokolCommand() {
    Clear();
}

void SokolCommand::ClearDrawData() {
    if (owned_vertex_buffer) {
        delete vertex_buffer;
        owned_vertex_buffer = false;
    }
    if (owned_index_buffer) {
        delete index_buffer;
        owned_index_buffer = false;
    }
    if (pass_action) {
        delete pass_action;
        pass_action = nullptr;
    }
    vertex_buffer = nullptr;
    index_buffer = nullptr;
    vertices = 0;
    indices = 0;
    
}

void SokolCommand::ClearMVP() {
    if (owned_mvp) {
        delete vs_mvp;
        owned_mvp = false;
    }
    vs_mvp = nullptr;
}

void SokolCommand::ClearTextures() {
    for (int i = 0; i < PERIMETER_SOKOL_TEXTURES; ++i) {
        if (texture_handles[i]) texture_handles[i]->DecRef();
        sokol_textures[i] = nullptr;
        texture_handles[i] = nullptr;
    }
}

void SokolCommand::Clear() {
    ClearDrawData();
    ClearMVP();
    ClearTextures();
}

void SokolCommand::SetTexture(size_t index, cTexture* texture, SokolTexture2D* sokol_texture) {
    if (sokol_texture == nullptr) {
        texture = nullptr;
    }
    if (texture) texture->IncRef();
    cTexture* old_tex = texture_handles[index];
    sokol_textures[index] = sokol_texture;
    texture_handles[index] = texture;
    if (old_tex) old_tex->DecRef();
}
