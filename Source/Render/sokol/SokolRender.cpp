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

cSokolRender::cSokolRender() = default;

cSokolRender::~cSokolRender() {
    Done();
};

int cSokolRender::Init(int xScr, int yScr, int mode, void* wnd, int RefreshRateInHz) {
    int res = cInterfaceRenderDevice::Init(xScr, yScr, mode, wnd, RefreshRateInHz);
    if (res != 0) {
        return res;
    }

    ClearCommands();
    ClearPipelines();
    
    sdlWindow = static_cast<SDL_Window*>(wnd);
    
    //Init some state
    activePipelineType = PIPELINE_TYPE_STANDARD;
    activePipelineBlend = ALPHA_NONE;
    activePipelineCull = CULL_CCW;
    
    //Set some attributes before context creation
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    //Use OpenGL 3.2 Core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // Create an OpenGL context associated with the window.
    sdlGlContext = SDL_GL_CreateContext(sdlWindow);
    if (sdlGlContext == nullptr) {
        ErrH.Abort("Error creating SDL GL Context", XERR_CRITICAL, 0, SDL_GetError());
    }
    
    //Init sokol gfx
    {
        sg_desc desc = {};
        desc.pipeline_pool_size = PIPELINE_ID_MAX,
        desc.shader_pool_size = 8,
        desc.buffer_pool_size = 2048 * 2; //2048 is enough for PGW+PET game
        desc.image_pool_size = 1024 * 4; //1024 is enough for PGW+PET game
        desc.context.color_format = SG_PIXELFORMAT_RGBA8;
        sg_setup(&desc);
    }
    
    //Create empty texture
    sg_image_desc desc = {};
    desc.label = "EmptySlotTexture";
    desc.width = desc.height = 64;
    desc.wrap_u = desc.wrap_v = SG_WRAP_REPEAT;
    desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    desc.num_mipmaps = 1;
    desc.min_filter = desc.mag_filter = SG_FILTER_LINEAR;
    desc.usage = SG_USAGE_IMMUTABLE;
    size_t buf_len = desc.height * desc.width * sokol_pixelformat_bytesize(desc.pixel_format);
    uint8_t* buf = new uint8_t[buf_len];
    memset(buf, 0xFF, buf_len);
    desc.data.subimage[0][0] = { buf, buf_len };
    emptyTexture = new SokolTexture2D(desc);
    delete[] buf;

    //Register a pipeline for each vertex format in game
    RegisterPipelines();

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
    orthoVP = Mat4f::ID;
    SetOrthographic(orthoVP, ScreenSize.x, -ScreenSize.y, -10, 10);
    return 0;
}

int cSokolRender::Done() {
    int ret = cInterfaceRenderDevice::Done();
    ClearCommands();
    ClearPipelines();
    shaders.clear();
    delete emptyTexture;
    emptyTexture = nullptr;
    sg_shutdown();
    if (sdlGlContext != nullptr) {
        SDL_GL_DeleteContext(sdlGlContext);
        sdlGlContext = nullptr;
    }
    return ret;
}

int cSokolRender::SetGamma(float fGamma, float fStart, float fFinish) {
    //TODO
    printf("cSokolRender::SetGamma not implemented!\n");
    return -1;
}

void cSokolRender::CreateVertexBuffer(VertexBuffer& vb, uint32_t NumberVertex, vertex_fmt_t fmt, bool dynamic) {
    xassert(!vb.sg);
    size_t size = GetSizeFromFormat(fmt);

    sg_buffer_desc desc = {};
    desc.size = NumberVertex * size;
    desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
    desc.usage = dynamic ? SG_USAGE_STREAM : SG_USAGE_DYNAMIC;
    desc.label = "CreateVertexBuffer";

    vb.VertexSize = size;
    vb.fmt = fmt;
    vb.dynamic = dynamic;
    vb.NumberVertex = NumberVertex;
    vb.sg = new SokolBuffer(desc);
}

void cSokolRender::DeleteVertexBuffer(VertexBuffer &vb) {
    delete vb.sg;
    vb.sg = nullptr;
}

void* cSokolRender::LockVertexBuffer(VertexBuffer &vb) {
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
    xassert(Start + Amount <= vb.NumberVertex);
    return &static_cast<uint8_t*>(LockVertexBuffer(vb))[vb.VertexSize * Start];
}

void cSokolRender::UnlockVertexBuffer(VertexBuffer &vb) {
    xassert(vb.sg->locked);
    vb.sg->locked = false;
}

void cSokolRender::CreateIndexBuffer(IndexBuffer& ib, uint32_t NumberIndices) {
    xassert(!ib.sg);
    ib.NumberIndices = NumberIndices;
    
    sg_buffer_desc desc = {};
    desc.size = ib.NumberIndices * sizeof(indices_t);
    desc.type = SG_BUFFERTYPE_INDEXBUFFER;
    desc.usage = SG_USAGE_DYNAMIC;
    desc.label = "CreateIndexBuffer";
    ib.sg = new SokolBuffer(desc);
}

void cSokolRender::DeleteIndexBuffer(IndexBuffer &ib) {
    delete ib.sg;
    ib.sg = nullptr;
}

indices_t* cSokolRender::LockIndexBuffer(IndexBuffer &ib) {
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
    xassert(Start + Amount <= ib.NumberIndices);
    return &LockIndexBuffer(ib)[Start];
}

void cSokolRender::UnlockIndexBuffer(IndexBuffer &ib) {
    xassert(ib.sg->locked);
    ib.sg->locked = false;
}

void cSokolRender::ClearCommands() {
    for (auto command : commands) {
        delete command;
    }
    commands.clear();
}

void cSokolRender::ClearPipelines() {
    for (auto pipeline : pipelines) {
        delete pipeline;
    }
    pipelines.clear();
    pipelines.resize(PIPELINE_ID_MAX);
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

////////////////////////////////////////////////////////////////////////////////////////////

SokolCommand::SokolCommand() {
    Clear();
}

SokolCommand::~SokolCommand() {
    Clear();
}

void SokolCommand::ClearDrawData() {
    if (owned_buffers) {
        delete vertex_buffer;
        delete index_buffer;
        owned_buffers = false;
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

void SokolCommand::Clear() {
    ClearDrawData();
    ClearMVP();
    memset(textures, 0, PERIMETER_SOKOL_TEXTURES * sizeof(SokolTexture2D*));
}
