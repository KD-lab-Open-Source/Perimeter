#include <vector>
#include "xmath.h"
#include "Umath.h"
#include "sokol_gfx.h"
#include "SokolResources.h"
#include "IRenderDevice.h"
#include "EmptyRenderDevice.h"
#include "SokolRender.h"
#include "xerrhand.h"
#include "SokolShaders.h"
#include "VertexFormat.h"

struct SokolPipelineContext {
    uint32_t vertex_fmt;
    struct shader_funcs* shader_funcs;
};

void bind_attr_slot(SokolPipelineContext& ctx, sg_pipeline_desc& desc, const char* attr_name, sg_vertex_format sokol_format) {
    int attr_slot = ctx.shader_funcs->attr_slot(attr_name);
    if (attr_slot < 0) {
        fprintf(stderr, "bind_attr_slot: '%s' slot not found at pipeline '%s'\n", attr_name, desc.label);
    } else {
        desc.layout.attrs[attr_slot].format = sokol_format;
    }
}

void bind_vertex_fmt(SokolPipelineContext& ctx, sg_pipeline_desc& desc, uint32_t fmt_flag) {
    uint32_t fmt = ctx.vertex_fmt;
    if ((fmt & fmt_flag) == 0) return;
    
    switch (fmt_flag) {
        case VERTEX_FMT_XYZ:
            bind_attr_slot(ctx, desc, "vs_position", SG_VERTEXFORMAT_FLOAT3);
            break;
        case VERTEX_FMT_DIFFUSE:
            bind_attr_slot(ctx, desc, "vs_color", SG_VERTEXFORMAT_UBYTE4N);
            break;
        case VERTEX_FMT_TEX1:
            bind_attr_slot(ctx, desc, "vs_texcoord0", SG_VERTEXFORMAT_FLOAT2);
            break;
        case VERTEX_FMT_TEX2:
            bind_attr_slot(ctx, desc, "vs_texcoord1", SG_VERTEXFORMAT_FLOAT2);
            break;
        case VERTEX_FMT_NORMAL:
            bind_attr_slot(ctx, desc, "vs_normal", SG_VERTEXFORMAT_FLOAT2);
            break;
        case VERTEX_FMT_DOT3:
            bind_attr_slot(ctx, desc, "vs_dot3_0", SG_VERTEXFORMAT_FLOAT3);
            bind_attr_slot(ctx, desc, "vs_dot3_1", SG_VERTEXFORMAT_FLOAT3);
            bind_attr_slot(ctx, desc, "vs_dot3_2", SG_VERTEXFORMAT_FLOAT3);
            break;
        default:
            fprintf(stderr, "bind_vertex_fmt: unregistered fmt '%d'\n", fmt_flag);
            break;
    }

}

void cSokolRender::register_pipeline(uint32_t vertex_fmt, shader_funcs* shader_funcs) {
    SokolPipelineContext ctx = { vertex_fmt, shader_funcs };
    
    //Common part of pipeline desc
    sg_pipeline_desc desc = {};
    desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    desc.depth.write_enabled = true;
    desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES,
    desc.index_type = sizeof(indices_t) == 2 ? SG_INDEXTYPE_UINT16 : SG_INDEXTYPE_UINT32,
    //TODO pipeline with _BACK and put CW/CCW behind a pipeline flag to create both variants
    desc.cull_mode = SG_CULLMODE_NONE;
    desc.face_winding = SG_FACEWINDING_CW;
    
    auto& blend0 = desc.colors[0].blend;
    blend0.enabled = true;
    blend0.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    blend0.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blend0.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
    blend0.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    
    //Get shader desc and make shader
    const sg_shader_desc* shader_desc = ctx.shader_funcs->shader_desc(sg_query_backend());
    desc.shader = sg_make_shader(shader_desc);
    desc.label = shader_desc->label;
    
    //Shader sanity checks
    if (desc.shader.id == SG_INVALID_ID) {
        fprintf(stderr, "register_pipeline: invalid shader ID pipeline '%s'\n", desc.label);
        return;
    }
    if (ctx.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_VS, "vs_params") != 0) {
        fprintf(stderr, "register_pipeline: 'vs_params' uniform slot is not 0 at pipeline '%s'\n", desc.label);
        return;
    }
    if (ctx.shader_funcs->uniformblock_size(SG_SHADERSTAGE_VS, "vs_params") != sizeof(vs_params_t)) {
        fprintf(stderr, "register_pipeline: 'vs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
        return;
    }
    if (ctx.vertex_fmt & VERTEX_FMT_TEX1 && ctx.shader_funcs->image_slot(SG_SHADERSTAGE_FS, "un_tex0") < 0) {
        fprintf(stderr, "register_pipeline: 'un_tex0' image slot not found at pipeline '%s'\n", desc.label);
        return;
    }
    if (ctx.vertex_fmt & VERTEX_FMT_TEX2 && ctx.shader_funcs->image_slot(SG_SHADERSTAGE_FS, "un_tex1") < 0) {
        fprintf(stderr, "register_pipeline: 'un_tex1' image slot not found at pipeline '%s'\n", desc.label);
        return;
    }
    
    //We bind required attributes into layout of pipeline if provided fmt needs so
    bind_vertex_fmt(ctx, desc, VERTEX_FMT_XYZ);
    bind_vertex_fmt(ctx, desc, VERTEX_FMT_DIFFUSE);
    bind_vertex_fmt(ctx, desc, VERTEX_FMT_TEX1);
    bind_vertex_fmt(ctx, desc, VERTEX_FMT_TEX2);
    bind_vertex_fmt(ctx, desc, VERTEX_FMT_NORMAL);
    bind_vertex_fmt(ctx, desc, VERTEX_FMT_DOT3);
    
    //Created, store on our pipelines
    SokolPipeline* pipeline = new SokolPipeline {
            ctx.vertex_fmt,
            sg_make_pipeline(desc),
            ctx.shader_funcs
    };
    if (pipeline->pipeline.id == SG_INVALID_ID) {
        fprintf(stderr, "register_pipeline: invalid pipeline ID pipeline '%s'\n", desc.label);
        return;
    }
    if (pipeline->vertex_fmt == 0) {
        fprintf(stderr, "register_pipeline: invalid pipeline vertex format '%s'\n", desc.label);
        return;
    }
    if (pipelines[pipeline->vertex_fmt] != nullptr) {
        fprintf(stderr, "register_pipeline: '%s' pipeline already registered at '%d'\n", desc.label, pipeline->vertex_fmt);
        xassert(0);
    }
    pipelines[pipeline->vertex_fmt] = pipeline;
#ifdef PERIMETER_DEBUG
    printf("register_pipeline: '%s' registered at %d\n", desc.label, pipeline->vertex_fmt);
#endif
}

cSokolRender::cSokolRender() = default;

cSokolRender::~cSokolRender() {
    ClearCommands();
    for (auto pipeline : pipelines) {
        delete pipeline;
    }
};

int cSokolRender::Init(int xScr, int yScr, int mode, void* wnd, int RefreshRateInHz) {
    int res = cInterfaceRenderDevice::Init(xScr, yScr, mode, wnd, RefreshRateInHz);
    if (res != 0) {
        return res;
    }

    ClearCommands();
    for (auto pipeline : pipelines) {
        delete pipeline;
    }
    pipelines.clear();
    pipelines.resize(VERTEX_FMT_MAX);
    
    sdlWindow = static_cast<SDL_Window*>(wnd);
    
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
        desc.buffer_pool_size = 2048 * 2; //2048 is enough for PGW+PET game
        desc.image_pool_size = 1024 * 4; //1024 is enough for PGW+PET game
        desc.shader_pool_size = desc.pipeline_pool_size = 16,
        desc.context.color_format = SG_PIXELFORMAT_RGBA8;
        sg_setup(&desc);
    }

    //Register a pipeline for each vertex format in game
    register_pipeline(sVertexXYZD::fmt,      &shader_color);
    register_pipeline(sVertexXYZDT1::fmt,    &shader_color_tex1);
    register_pipeline(sVertexXYZDT2::fmt,    &shader_color_tex2);
    register_pipeline(sVertexXYZT1::fmt,     &shader_tex1);
    register_pipeline(sVertexXYZNT1::fmt,    &shader_normal);
    register_pipeline(sVertexDot3::fmt,      &shader_normal_dot3);

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
    orthoMat = Mat4f::ID;
    SetOrthographic(orthoMat, ScreenSize.x, -ScreenSize.y, -10, 10);
    return 0;
}

int cSokolRender::Done() {
    int ret = cInterfaceRenderDevice::Done();
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

void cSokolRender::CreateVertexBuffer(VertexBuffer& vb, int NumberVertex, int fmt, int dynamic) {
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
    return static_cast<sPolygon*>(vb.sg->data);
}

void cSokolRender::UnlockVertexBuffer(VertexBuffer &vb) {
    xassert(vb.sg->locked);
    vb.sg->locked = false;
}

void cSokolRender::CreateIndexBuffer(IndexBuffer& ib, int NumberPolygons) {
    xassert(!ib.sg);
    ib.NumberPolygons = NumberPolygons;
    ib.NumberIndices = ib.NumberPolygons * sPolygon::PN;
    
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

sPolygon* cSokolRender::LockIndexBuffer(IndexBuffer &ib) {
    if (!ib.sg) {
        xassert(0);
        return nullptr;
    }
    xassert(!ib.sg->locked);
    ib.sg->dirty = true;
    ib.sg->locked = true;
    return static_cast<sPolygon*>(ib.sg->data);
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

SokolCommand::~SokolCommand() {
    Clear();
}

void SokolCommand::Clear() {
    if (owned_buffers) {
        delete vertex_buffer;
        delete index_buffer;
        owned_buffers = false;
    }
    vertex_buffer = nullptr;
    index_buffer = nullptr;
    if (owned_mvp) {
        delete mvp;
        owned_mvp = false;
    }
    mvp = nullptr;
    vertices = 0;
    indices = 0;
}
