#include <string>
#include "xmath.h"
#include "Umath.h"
#include "StdAfxRD.h"
#include "VertexFormat.h"
#include "sokol_gfx.h"
#include "SokolRender.h"
#include "SokolRenderPipeline.h"
#include "SokolShaders.h"

struct SokolPipelineContext {
    pipeline_id_t id;
    PIPELINE_TYPE pipeline_type;
    vertex_fmt_t vertex_fmt;
    sg_pipeline_desc* desc;
    struct shader_funcs* shader_funcs;
};

sg_compare_func GetSokolCompareFunc(eCMPFUNC cmpfunc) {
    switch (cmpfunc) {
        case CMP_LESSEQUAL:
            return SG_COMPAREFUNC_LESS_EQUAL;
        case CMP_GREATER:
            return SG_COMPAREFUNC_GREATER;
        case CMP_GREATEREQUAL:
            return SG_COMPAREFUNC_GREATER_EQUAL;
        default:
        case CMP_ALWAYS:
            return SG_COMPAREFUNC_ALWAYS;
    }
}

pipeline_mode_value_t PIPELINE_MODE::GetValue() const {
    pipeline_mode_value_t value = blend & 0b111;
    value |= (depth_write & 0b1) << 3;
    value |= (depth_cmp & 0b11) << 4;
    value |= (cull & 0b11) << 6;
    return value;
}

void PIPELINE_MODE::FromValue(pipeline_mode_value_t value) {
    blend = static_cast<eBlendMode>(value & 0b111);
    depth_write = static_cast<bool>((value >> 3) & 0b1);
    depth_cmp = static_cast<eCMPFUNC>((value >> 4) & 0b11);
    cull = static_cast<eCullMode>((value >> 6) & 0b11);
}

pipeline_id_t cSokolRender::GetPipelineID(PIPELINE_TYPE type, vertex_fmt_t vertex_fmt, const PIPELINE_MODE& mode) {
    pipeline_id_t id = (mode.GetValue() & PIPELINE_ID_MODE_MASK) << PIPELINE_ID_MODE_SHIFT;
    id |= (vertex_fmt & PIPELINE_ID_VERTEX_FMT_MASK) << PIPELINE_ID_VERTEX_FMT_SHIFT;
    id |= (type & PIPELINE_ID_TYPE_MASK) << PIPELINE_ID_TYPE_SHIFT;
    return id;
}

void cSokolRender::GetPipelineIDParts(pipeline_id_t id, PIPELINE_TYPE* type, vertex_fmt_t* vertex_fmt, PIPELINE_MODE* mode) {
    mode->FromValue((id >> PIPELINE_ID_MODE_SHIFT) & PIPELINE_ID_MODE_MASK);
    if (vertex_fmt) *vertex_fmt = static_cast<vertex_fmt_t>((id >> PIPELINE_ID_VERTEX_FMT_SHIFT) & PIPELINE_ID_VERTEX_FMT_MASK);
    if (type) *type = static_cast<PIPELINE_TYPE>((id >> PIPELINE_ID_TYPE_SHIFT) & PIPELINE_ID_TYPE_MASK);
}

void bind_attr_slot(SokolPipelineContext& ctx, const char* attr_name, sg_vertex_format sokol_format) {
    int attr_slot = ctx.shader_funcs->attr_slot(attr_name);
    if (attr_slot < 0) {
        fprintf(stderr, "bind_attr_slot: '%s' slot not found at pipeline '%s'\n", attr_name, ctx.desc->label);
    } else {
        ctx.desc->layout.attrs[attr_slot].format = sokol_format;
    }
}

void bind_vertex_fmt(SokolPipelineContext& ctx, uint32_t fmt_flag) {
    vertex_fmt_t fmt = ctx.vertex_fmt;
    if ((fmt & fmt_flag) == 0) return;

    switch (fmt_flag) {
        case VERTEX_FMT_DIFFUSE:
            bind_attr_slot(ctx, "vs_color", SG_VERTEXFORMAT_UBYTE4N);
            break;
        case VERTEX_FMT_TEX1:
            bind_attr_slot(ctx, "vs_texcoord0", SG_VERTEXFORMAT_FLOAT2);
            break;
        case VERTEX_FMT_TEX2:
            bind_attr_slot(ctx, "vs_texcoord1", SG_VERTEXFORMAT_FLOAT2);
            break;
        case VERTEX_FMT_NORMAL:
            bind_attr_slot(ctx, "vs_normal", SG_VERTEXFORMAT_FLOAT3);
            break;
        default:
            fprintf(stderr, "bind_vertex_fmt: unregistered fmt '%d'\n", fmt_flag);
            break;
    }
}

void cSokolRender::RegisterPipeline(pipeline_id_t id) {
    //Create sokol desc and context struct
    sg_pipeline_desc desc = {};
    SokolPipelineContext ctx = {id, PIPELINE_TYPE_DEFAULT, 0, &desc, nullptr };

    //Extract info about this pipeline
    PIPELINE_MODE mode;
    GetPipelineIDParts(id, &ctx.pipeline_type, &ctx.vertex_fmt, &mode);
    
    //Choose shader
    switch (ctx.pipeline_type) {
#ifdef PERIMETER_DEBUG
        case PIPELINE_TYPE_LINE_STRIP:
#endif
        case PIPELINE_TYPE_TRIANGLE:
        case PIPELINE_TYPE_TRIANGLESTRIP:
            switch (ctx.vertex_fmt) {
                case sVertexXYZT1::fmt:
                    ctx.shader_funcs = &shader_terrain;
                    break;
                case sVertexXYZDT1::fmt:
                    ctx.shader_funcs = &shader_color_tex1;
                    break;
                case sVertexXYZDT2::fmt:
                    ctx.shader_funcs = &shader_color_tex2;
                    break;
                case sVertexXYZNT1::fmt:
                    ctx.shader_funcs = &shader_normal;
                    break;
                default:
                    fprintf(stderr, "RegisterPipeline: unknown pipeline format '%d'\n", ctx.vertex_fmt);
                    break;
            }
            break;
        default:
            fprintf(stderr, "RegisterPipeline: unknown pipeline type '%d'\n", ctx.pipeline_type);
            break;
    }
    if (ctx.shader_funcs == nullptr) {
        fprintf(stderr, "RegisterPipeline: no shader assigned to pipeline '%d'\n", id);
        xassert(0);
        return;
    }

    //Common part of pipeline desc
    desc.depth.compare = GetSokolCompareFunc(mode.depth_cmp);
    desc.depth.write_enabled = mode.depth_write;
    switch (ctx.pipeline_type) {
        default:
        case PIPELINE_TYPE_TRIANGLE:
            desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
            break;
        case PIPELINE_TYPE_TRIANGLESTRIP:
            desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP;
            break;
#ifdef PERIMETER_DEBUG
        case PIPELINE_TYPE_LINE_STRIP:
            desc.primitive_type = SG_PRIMITIVETYPE_LINE_STRIP;
            break;
#endif
    }
    desc.index_type = sizeof(indices_t) == 2 ? SG_INDEXTYPE_UINT16 : SG_INDEXTYPE_UINT32;
    desc.cull_mode = CULL_NONE == mode.cull ? SG_CULLMODE_NONE : SG_CULLMODE_BACK;
    desc.face_winding = CULL_CCW == mode.cull ? SG_FACEWINDING_CW : SG_FACEWINDING_CCW;

    auto& blend0 = desc.colors[0].blend;
    blend0.enabled = ALPHA_NONE < mode.blend;
    if (blend0.enabled) {
        blend0.op_rgb = blend0.op_alpha = SG_BLENDOP_ADD;
        switch (mode.blend) {
            case ALPHA_SUBBLEND:
                //Probably unused
                blend0.op_rgb = blend0.op_alpha = SG_BLENDOP_REVERSE_SUBTRACT;
                blend0.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
                blend0.dst_factor_rgb = SG_BLENDFACTOR_ONE;
                break;
            case ALPHA_ADDBLENDALPHA:
                blend0.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
                blend0.dst_factor_rgb = SG_BLENDFACTOR_ONE;
                break;
            case ALPHA_ADDBLEND:
                blend0.src_factor_rgb = SG_BLENDFACTOR_ONE;
                blend0.dst_factor_rgb = SG_BLENDFACTOR_ONE;
                break;
            case ALPHA_TEST:
            case ALPHA_BLEND:
                blend0.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
                blend0.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
                break;
            case ALPHA_MUL:
                //Probably unused
                blend0.src_factor_rgb = SG_BLENDFACTOR_DST_COLOR;
                blend0.dst_factor_rgb = SG_BLENDFACTOR_ZERO;
                break;
            case ALPHA_NONE:
            default:
                break;
        }

        if (blend0.src_factor_alpha == _SG_BLENDFACTOR_DEFAULT) {
            blend0.src_factor_alpha = blend0.src_factor_rgb;
        }
        if (blend0.dst_factor_alpha == _SG_BLENDFACTOR_DEFAULT) {
            blend0.dst_factor_alpha = blend0.dst_factor_rgb;
        }
    }

    //Get shader desc and make if not cached already
    const sg_shader_desc* shader_desc = ctx.shader_funcs->shader_desc(sg_query_backend());
    if (shaders.count(shader_desc->label)) {
        desc.shader = shaders[shader_desc->label];
    } else {
        desc.shader = sg_make_shader(shader_desc);
        shaders[shader_desc->label] = desc.shader;
    }
    desc.label = shader_desc->label;

    //Shader sanity checks
    if (desc.shader.id == SG_INVALID_ID) {
        fprintf(stderr, "RegisterPipeline: invalid shader ID pipeline '%s'\n", desc.label);
        return;
    }
    SOKOL_SHADER_ID shader_id = ctx.shader_funcs->get_id();
    switch (shader_id) {
        case SOKOL_SHADER_ID_color_tex1:
        case SOKOL_SHADER_ID_color_tex2:
            if (0 > ctx.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_VS, "color_texture_vs_params")) {
                fprintf(stderr, "RegisterPipeline: 'color_texture_vs_params' uniform slot not found at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (ctx.shader_funcs->uniformblock_size(SG_SHADERSTAGE_VS, "color_texture_vs_params") != sizeof(color_texture_vs_params_t)) {
                fprintf(stderr, "RegisterPipeline: 'color_texture_vs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (0 > ctx.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_FS, "color_texture_fs_params"))  {
                fprintf(stderr, "RegisterPipeline: 'color_texture_fs_params' uniform slot not found at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (ctx.shader_funcs->uniformblock_size(SG_SHADERSTAGE_FS, "color_texture_fs_params") != sizeof(color_texture_fs_params_t)) {
                fprintf(stderr, "RegisterPipeline: 'color_texture_fs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            }
            break;
        case SOKOL_SHADER_ID_normal:
            if (0 > ctx.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_VS, "normal_texture_vs_params")) {
                fprintf(stderr, "RegisterPipeline: 'normal_texture_vs_params' uniform slot not found at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (ctx.shader_funcs->uniformblock_size(SG_SHADERSTAGE_VS, "normal_texture_vs_params") != sizeof(normal_texture_vs_params_t)) {
                fprintf(stderr, "RegisterPipeline: 'normal_texture_vs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            }
            break;
        case SOKOL_SHADER_ID_terrain:
            if (0 > ctx.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_VS, "terrain_vs_params")) {
                fprintf(stderr, "RegisterPipeline: 'terrain_vs_params' uniform slot not found at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (ctx.shader_funcs->uniformblock_size(SG_SHADERSTAGE_VS, "terrain_vs_params") != sizeof(terrain_vs_params_t)) {
                fprintf(stderr, "RegisterPipeline: 'terrain_vs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (0 > ctx.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_FS, "terrain_fs_params")) {
                fprintf(stderr, "RegisterPipeline: 'terrain_fs_params' uniform slot not found at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (ctx.shader_funcs->uniformblock_size(SG_SHADERSTAGE_FS, "terrain_fs_params") != sizeof(terrain_fs_params_t)) {
                fprintf(stderr, "RegisterPipeline: 'terrain_fs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            }
            break;
        default:
            fprintf(stderr, "RegisterPipeline: Unknown shader id '%d' at pipeline '%s'\n", shader_id, desc.label);
            return;
    }
    if (ctx.vertex_fmt & VERTEX_FMT_TEX1 && ctx.shader_funcs->image_slot(SG_SHADERSTAGE_FS, "un_tex0") < 0) {
        fprintf(stderr, "RegisterPipeline: 'un_tex0' image slot not found at pipeline '%s'\n", desc.label);
    }
    if (ctx.vertex_fmt & VERTEX_FMT_TEX2) {
        if (ctx.shader_funcs->image_slot(SG_SHADERSTAGE_FS, "un_tex1") < 0) {
            fprintf(stderr, "RegisterPipeline: 'un_tex1' image slot not found at pipeline '%s'\n", desc.label);
        }
    }
    
    //Common attributes
    bind_attr_slot(ctx, "vs_position", SG_VERTEXFORMAT_FLOAT3);

    //We bind required attributes into layout of pipeline if provided fmt needs so
    bind_vertex_fmt(ctx, VERTEX_FMT_DIFFUSE);
    bind_vertex_fmt(ctx, VERTEX_FMT_TEX1);
    bind_vertex_fmt(ctx, VERTEX_FMT_TEX2);
    bind_vertex_fmt(ctx, VERTEX_FMT_NORMAL);

    //Created, store on our pipelines
    //printf("RegisterPipeline: '%s' at '%d'\n", desc.label, id);
    SokolPipeline* pipeline = new SokolPipeline {
        ctx.id,
        ctx.pipeline_type,
        ctx.vertex_fmt,
        sg_make_pipeline(desc),
        ctx.shader_funcs,
        shader_id,
        {}
    };
    if (pipeline->pipeline.id == SG_INVALID_ID) {
        xxassert(0, "RegisterPipeline: invalid sg_pipeline ID pipeline " + std::string(desc.label));
        return;
    }
    if (pipeline->vertex_fmt == 0) {
        xxassert(0, "RegisterPipeline: invalid pipeline vertex format " + std::string(desc.label));
        return;
    }
    if (0 != pipelines.count(pipeline->id)) {
        fprintf(stderr, "RegisterPipeline: '%s' pipeline already registered with '%d'\n", desc.label, pipeline->id);
        xassert(0);
    }
    for (int i = 0; i < PERIMETER_SOKOL_TEXTURES; ++i) {
        std::string name = "un_tex" + std::to_string(i);
        pipeline->shader_fs_texture_slot[i] = ctx.shader_funcs->image_slot(SG_SHADERSTAGE_FS, name.c_str());
    }
    if (pipelines.size() + 1 > PERIMETER_SOKOL_PIPELINES_MAX) {
        fprintf(stderr, "RegisterPipeline: reached maximum amount of registered pipelines '%d'\n", pipeline->id);
        xassert(0);
        while (pipelines.size() + 10 > PERIMETER_SOKOL_PIPELINES_MAX) {
            auto it = pipelines.cbegin();
            SokolPipeline* pipeline_delete = it->second;
            pipelines.erase(it);
            delete pipeline_delete;
        }
    }
    pipelines.insert(std::make_pair(pipeline->id, pipeline));
#ifdef PERIMETER_DEBUG
    printf("RegisterPipeline: '%s' registered with '%d'\n", desc.label, pipeline->id);
#endif
}

SokolPipeline::~SokolPipeline() {
    sg_destroy_pipeline(pipeline);
}
