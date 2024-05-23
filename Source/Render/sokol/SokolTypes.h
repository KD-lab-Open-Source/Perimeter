#ifndef PERIMETER_SOKOLTYPES_H
#define PERIMETER_SOKOLTYPES_H

#include <tuple>

#include "RenderTypes.h"

enum SOKOL_SHADER_ID {
    SOKOL_SHADER_ID_NONE,
    SOKOL_SHADER_ID_color_tex1,
    SOKOL_SHADER_ID_color_tex2,
    SOKOL_SHADER_ID_normal,
    SOKOL_SHADER_ID_object_shadow,
    SOKOL_SHADER_ID_only_texture,
    SOKOL_SHADER_ID_tile_map,
};

enum PIPELINE_TYPE {
    PIPELINE_TYPE_MESH,
    PIPELINE_TYPE_TILE_MAP,
    PIPELINE_TYPE_OBJECT_SHADOW,
#ifdef PERIMETER_DEBUG
    PIPELINE_TYPE_LINE_STRIP,
#endif
    PIPELINE_TYPE_MAX,
};
const PIPELINE_TYPE PIPELINE_TYPE_DEFAULT = PIPELINE_TYPE_MESH;


enum SOKOL_MATERIAL_TYPE {
    SOKOL_MAT_NONE = 0,
    SOKOL_MAT_LIGHT = 1,
};

using pipeline_mode_value_t = uint16_t;
struct PIPELINE_MODE {
    eBlendMode blend = ALPHA_NONE;
    eCullMode cull = CULL_NONE;
    eCMPFUNC depth_cmp = CMP_GREATER;
    bool depth_write = true;

    pipeline_mode_value_t GetValue() const;
    void FromValue(pipeline_mode_value_t value);

    bool operator==(const PIPELINE_MODE& other) const {
        return std::tie(blend, cull, depth_cmp, depth_write) 
            == std::tie(other.blend, other.cull, other.depth_cmp, other.depth_write);
    }
};

template<typename T> class SokolResource;
using SokolResourceTexture = SokolResource<sg_image>;
using SokolResourceBuffer = SokolResource<sg_buffer>;

#endif //PERIMETER_SOKOLTYPES_H
