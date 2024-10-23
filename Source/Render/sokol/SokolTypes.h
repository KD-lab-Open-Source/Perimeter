#ifndef PERIMETER_SOKOLTYPES_H
#define PERIMETER_SOKOLTYPES_H

#include <tuple>

#include "RenderTypes.h"

enum SOKOL_SHADER_ID {
    SOKOL_SHADER_ID_NONE,
    SOKOL_SHADER_ID_mesh_color_tex1,
    SOKOL_SHADER_ID_mesh_color_tex2,
    SOKOL_SHADER_ID_mesh_normal_tex1,
    SOKOL_SHADER_ID_mesh_tex1,
    SOKOL_SHADER_ID_shadow_tex1,
    SOKOL_SHADER_ID_shadow_normal_tex1,
    SOKOL_SHADER_ID_tile_map,
};

enum PIPELINE_TYPE {
    PIPELINE_TYPE_MESH,
    PIPELINE_TYPE_TILE_MAP,
    PIPELINE_TYPE_OBJECT_SHADOW,
    PIPELINE_TYPE_MAX,
};
const PIPELINE_TYPE PIPELINE_TYPE_DEFAULT = PIPELINE_TYPE_MESH;

enum SOKOL_PIPELINE_TARGET {
    SOKOL_PIPELINE_TARGET_SHADOWMAP,
    SOKOL_PIPELINE_TARGET_LIGHTMAP,
    SOKOL_PIPELINE_TARGET_SWAPCHAIN
};

enum SOKOL_MATERIAL_TYPE {
    SOKOL_MAT_NONE = 0,
    SOKOL_MAT_LIGHT = 1,
};

using pipeline_mode_value_t = uint16_t;
struct SokolPipelineMode {
    eBlendMode blend = ALPHA_NONE;
    eCullMode cull = CULL_NONE;
    eCMPFUNC depth_cmp = CMP_GREATER;
    bool depth_write = true;
    bool wireframe_mode = false;

    pipeline_mode_value_t GetValue() const;
    void FromValue(pipeline_mode_value_t value);

    bool operator==(const SokolPipelineMode& other) const {
        return std::tie(blend, cull, depth_cmp, depth_write, wireframe_mode) 
            == std::tie(other.blend, other.cull, other.depth_cmp, other.depth_write, other.wireframe_mode);
    }
};

template<typename T> class SokolResource;
using SokolResourceImage = SokolResource<sg_image>;
using SokolResourceBuffer = SokolResource<sg_buffer>;

#endif //PERIMETER_SOKOLTYPES_H
