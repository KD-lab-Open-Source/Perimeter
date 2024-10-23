#include <algorithm>
#include <string>
#include "xmath.h"
#include "Umath.h"
#include "StdAfxRD.h"
#include "VertexFormat.h"
#include "SokolIncludes.h"
#include "SokolResources.h"
#include "SokolRender.h"
#include "SokolRenderPipeline.h"
#include "SokolShaders.h"

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

pipeline_mode_value_t SokolPipelineMode::GetValue() const {
    pipeline_mode_value_t value = blend & 0b111;
    value |= (depth_write & 0b1) << 3;
    value |= (depth_cmp & 0b11) << 4;
    value |= (cull & 0b11) << 6;
    return value;
}

void SokolPipelineMode::FromValue(pipeline_mode_value_t value) {
    blend = static_cast<eBlendMode>(value & 0b111);
    depth_write = static_cast<bool>((value >> 3) & 0b1);
    depth_cmp = static_cast<eCMPFUNC>((value >> 4) & 0b11);
    cull = static_cast<eCullMode>((value >> 6) & 0b11);
}

void bind_attr_slot(SokolPipelineContext& ctx, const char* attr_name, sg_vertex_format sokol_format) {
    int attr_slot = ctx.shader_funcs->attr_slot(attr_name);
    if (attr_slot < 0) {
        fprintf(stderr, "bind_attr_slot: '%s' slot not found at pipeline '%s'\n", attr_name, ctx.desc.label);
    } else {
        ctx.desc.layout.attrs[attr_slot].format = sokol_format;
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

void cSokolRender::RegisterPipeline(SokolPipelineContext context) {
    auto& desc = context.desc;

    //Choose shader
    switch (context.pipeline_type) {
        case PIPELINE_TYPE_MESH:
            switch (context.vertex_fmt) {
                case sVertexXYZT1::fmt:
                    context.shader_funcs = &shader_mesh_tex1;
                    break;
                case sVertexXYZDT1::fmt:
                    context.shader_funcs = &shader_mesh_color_tex1;
                    break;
                case sVertexXYZDT2::fmt:
                    context.shader_funcs = &shader_mesh_color_tex2;
                    break;
                case sVertexXYZNT1::fmt:
                    context.shader_funcs = &shader_mesh_normal_tex1;
                    break;
                default:
                    fprintf(stderr, "RegisterPipeline: PIPELINE_TYPE_MESH unknown pipeline format '%d'\n", context.vertex_fmt);
                    break;
            }
            break;
        case PIPELINE_TYPE_TILE_MAP:
            context.shader_funcs = &shader_tile_map;
            break;
        case PIPELINE_TYPE_OBJECT_SHADOW:
            switch (context.vertex_fmt) {
                case sVertexXYZT1::fmt:
                    context.shader_funcs = &shader_shadow_tex1;
                    break;
                case sVertexXYZNT1::fmt:
                    context.shader_funcs = &shader_shadow_normal_tex1;
                    break;
                default:
                    fprintf(stderr, "RegisterPipeline: PIPELINE_TYPE_OBJECT_SHADOW unknown pipeline format '%d'\n", context.vertex_fmt);
                    break;
            }
            break;
        default:
            fprintf(stderr, "RegisterPipeline: unknown pipeline type '%d'\n", context.pipeline_type);
            break;
    }
    if (context.shader_funcs == nullptr) {
        fprintf(stderr, "RegisterPipeline: no shader assigned to pipeline\n");
        xassert(0);
        return;
    }
    
    //Per target type specifics
    switch (context.pipeline_target) {
        case SOKOL_PIPELINE_TARGET_SHADOWMAP:
            desc.sample_count = 1;
            desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
            desc.colors[0].pixel_format = SG_PIXELFORMAT_NONE;
            break;
        case SOKOL_PIPELINE_TARGET_LIGHTMAP:
            desc.depth.pixel_format = SG_PIXELFORMAT_NONE;
            break;
        case SOKOL_PIPELINE_TARGET_SWAPCHAIN:
            desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH_STENCIL;
            break;
    }

    //Common part of pipeline desc
    desc.depth.compare = GetSokolCompareFunc(context.pipeline_mode.depth_cmp);
    desc.depth.write_enabled = context.pipeline_mode.depth_write;
    bool lines = context.pipeline_mode.wireframe_mode;
    switch (context.primitive_type) {
        case PT_TRIANGLES:
            if (lines) {
                desc.primitive_type = SG_PRIMITIVETYPE_LINES;
            } else {
                desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
            }
            break;
        case PT_TRIANGLESTRIP:
            if (lines) {
                desc.primitive_type = SG_PRIMITIVETYPE_LINE_STRIP;
            } else {
                desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP;
            }
            break;
        default:
            xxassert(false, "Unknown primitive mode");
            break;
    }
    desc.index_type = sizeof(indices_t) == 2 ? SG_INDEXTYPE_UINT16 : SG_INDEXTYPE_UINT32;
    desc.cull_mode = CULL_NONE == context.pipeline_mode.cull ? SG_CULLMODE_NONE : SG_CULLMODE_FRONT;
    desc.face_winding = CULL_CW == context.pipeline_mode.cull ? SG_FACEWINDING_CW : SG_FACEWINDING_CCW;

    auto& blend0 = desc.colors[0].blend;
    blend0.enabled = ALPHA_NONE < context.pipeline_mode.blend;
    if (blend0.enabled) {
        blend0.op_rgb = blend0.op_alpha = SG_BLENDOP_ADD;
        switch (context.pipeline_mode.blend) {
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
    const sg_shader_desc* shader_desc = context.shader_funcs->shader_desc(sg_query_backend());
    if (shader_desc == nullptr) {
        fprintf(stderr, "RegisterPipeline: no shader desc for pipeline\n");
        xassert(0);
        return;
    }
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
    SOKOL_SHADER_ID shader_id = context.shader_funcs->get_id();
    switch (shader_id) {
        case SOKOL_SHADER_ID_mesh_color_tex1:
        case SOKOL_SHADER_ID_mesh_color_tex2:
            if (0 > context.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_VS, "mesh_color_texture_vs_params")) {
                fprintf(stderr, "RegisterPipeline: 'mesh_color_texture_vs_params' uniform slot not found at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (context.shader_funcs->uniformblock_size(SG_SHADERSTAGE_VS, "mesh_color_texture_vs_params") != sizeof(mesh_color_texture_vs_params_t)) {
                fprintf(stderr, "RegisterPipeline: 'mesh_color_texture_vs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (0 > context.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_FS, "mesh_color_texture_fs_params"))  {
                fprintf(stderr, "RegisterPipeline: 'mesh_color_texture_fs_params' uniform slot not found at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (context.shader_funcs->uniformblock_size(SG_SHADERSTAGE_FS, "mesh_color_texture_fs_params") != sizeof(mesh_color_texture_fs_params_t)) {
                fprintf(stderr, "RegisterPipeline: 'mesh_color_texture_fs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            }
            break;
        case SOKOL_SHADER_ID_mesh_normal_tex1:
            if (0 > context.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_VS, "mesh_normal_texture_vs_params")) {
                fprintf(stderr, "RegisterPipeline: 'mesh_normal_texture_vs_params' uniform slot not found at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (context.shader_funcs->uniformblock_size(SG_SHADERSTAGE_VS, "mesh_normal_texture_vs_params") != sizeof(mesh_normal_texture_vs_params_t)) {
                fprintf(stderr, "RegisterPipeline: 'mesh_normal_texture_vs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            }
            break;
        case SOKOL_SHADER_ID_shadow_tex1:
        case SOKOL_SHADER_ID_shadow_normal_tex1:
            if (0 > context.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_VS, "shadow_texture_vs_params")) {
                fprintf(stderr, "RegisterPipeline: 'shadow_texture_vs_params' uniform slot not found at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (context.shader_funcs->uniformblock_size(SG_SHADERSTAGE_VS, "shadow_texture_vs_params") != sizeof(shadow_texture_vs_params_t)) {
                fprintf(stderr, "RegisterPipeline: 'shadow_texture_vs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (0 > context.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_FS, "shadow_texture_fs_params"))  {
                fprintf(stderr, "RegisterPipeline: 'shadow_texture_fs_params' uniform slot not found at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (context.shader_funcs->uniformblock_size(SG_SHADERSTAGE_FS, "shadow_texture_fs_params") != sizeof(mesh_color_texture_fs_params_t)) {
                fprintf(stderr, "RegisterPipeline: 'shadow_texture_fs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            }
            break;
        case SOKOL_SHADER_ID_mesh_tex1:
            if (0 > context.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_VS, "mesh_texture_vs_params")) {
                fprintf(stderr, "RegisterPipeline: 'mesh_texture_vs_params' uniform slot not found at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (context.shader_funcs->uniformblock_size(SG_SHADERSTAGE_VS, "mesh_texture_vs_params") != sizeof(mesh_texture_vs_params_t)) {
                fprintf(stderr, "RegisterPipeline: 'mesh_texture_vs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            }
            break;
        case SOKOL_SHADER_ID_tile_map:
            if (0 > context.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_VS, "tile_map_vs_params")) {
                fprintf(stderr, "RegisterPipeline: 'tile_map_vs_params' uniform slot not found at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (context.shader_funcs->uniformblock_size(SG_SHADERSTAGE_VS, "tile_map_vs_params") != sizeof(tile_map_vs_params_t)) {
                fprintf(stderr, "RegisterPipeline: 'tile_map_vs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (0 > context.shader_funcs->uniformblock_slot(SG_SHADERSTAGE_FS, "tile_map_fs_params")) {
                fprintf(stderr, "RegisterPipeline: 'tile_map_fs_params' uniform slot not found at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            } else if (context.shader_funcs->uniformblock_size(SG_SHADERSTAGE_FS, "tile_map_fs_params") != sizeof(tile_map_fs_params_t)) {
                fprintf(stderr, "RegisterPipeline: 'tile_map_fs_params' uniform size doesnt match at pipeline '%s'\n", desc.label);
                xassert(0);
                return;
            }
            break;
        default:
            fprintf(stderr, "RegisterPipeline: Unknown shader id '%d' at pipeline '%s'\n", shader_id, desc.label);
            return;
    }

    //Check for any missing slots
    if (context.vertex_fmt & VERTEX_FMT_TEX1 &&
        context.shader_funcs->image_slot(SG_SHADERSTAGE_FS, "un_tex0") < 0) {
        fprintf(stderr, "RegisterPipeline: 'un_tex0' image slot not found at pipeline '%s'\n", desc.label);
    }
    if (context.vertex_fmt & VERTEX_FMT_TEX2) {
        if (context.shader_funcs->image_slot(SG_SHADERSTAGE_FS, "un_tex1") < 0) {
            fprintf(stderr, "RegisterPipeline: 'un_tex1' image slot not found at pipeline '%s'\n", desc.label);
        }
    }
    if (context.vertex_fmt & VERTEX_FMT_TEX1 && context.shader_funcs->sampler_slot(SG_SHADERSTAGE_FS, "un_sampler0") < 0) {
        fprintf(stderr, "RegisterPipeline: 'un_sampler0' sampler slot not found at pipeline '%s'\n", desc.label);
    }
    
    //Common attributes
    bind_attr_slot(context, "vs_position", SG_VERTEXFORMAT_FLOAT3);

    //We bind required attributes into layout of pipeline if provided fmt needs so
    bind_vertex_fmt(context, VERTEX_FMT_DIFFUSE);
    bind_vertex_fmt(context, VERTEX_FMT_TEX1);
    bind_vertex_fmt(context, VERTEX_FMT_TEX2);
    bind_vertex_fmt(context, VERTEX_FMT_NORMAL);

    //Created, store on our pipelines
    //printf("RegisterPipeline: '%s' at '%d'\n", desc.label, id);
    SokolPipeline* pipeline = new SokolPipeline {
        context,
        sg_make_pipeline(desc),
        shader_id,
        {}
    };
    if (pipeline->pipeline.id == SG_INVALID_ID) {
        xxassert(0, "RegisterPipeline: invalid sg_pipeline ID pipeline " + std::string(desc.label));
        return;
    }
    if (pipeline->context.vertex_fmt == 0) {
        xxassert(0, "RegisterPipeline: invalid pipeline vertex format " + std::string(desc.label));
        return;
    }
    for (int i = 0; i < PERIMETER_SOKOL_TEXTURES; ++i) {
        std::string name = "un_tex" + std::to_string(i);
        pipeline->shader_fs_texture_slot[i] = context.shader_funcs->image_slot(SG_SHADERSTAGE_FS, name.c_str());
    }
    pipeline->shader_fs_sampler_slot = context.shader_funcs->sampler_slot(SG_SHADERSTAGE_FS, "un_sampler0");
    pipeline->shader_fs_shadow_sampler_slot = context.shader_funcs->sampler_slot(SG_SHADERSTAGE_FS, "un_sampler1");
    if (pipelines.size() + 1 > PERIMETER_SOKOL_PIPELINES_MAX) {
        fprintf(stderr, "RegisterPipeline: reached maximum amount of registered pipelines\n");
        xassert(0);
    }
    pipelines.push_back(pipeline);
#ifdef PERIMETER_DEBUG
    printf("RegisterPipeline: '%s' registered\n", desc.label);
#endif
}

struct SokolPipeline* cSokolRender::GetPipeline(const SokolPipelineContext& context) {
    auto p = std::find_if(
        pipelines.begin(), pipelines.end(),
        [&context](const auto& pipeline) -> bool {
            return pipeline->context == context;
        }
    );
    if (p != pipelines.end()) {
        return *p;
    }

    RegisterPipeline(context);
    return pipelines.back();
}

SokolPipeline::~SokolPipeline() {
    sg_destroy_pipeline(pipeline);
}
