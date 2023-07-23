#ifndef PERIMETER_SOKOLTYPES_H
#define PERIMETER_SOKOLTYPES_H

#include "RenderTypes.h"

enum SOKOL_SHADER_ID {
    SOKOL_SHADER_ID_NONE,
    SOKOL_SHADER_ID_color_tex1,
    SOKOL_SHADER_ID_color_tex2,
    SOKOL_SHADER_ID_normal,
    SOKOL_SHADER_ID_terrain,
};

enum PIPELINE_TYPE {
    PIPELINE_TYPE_TRIANGLE,
    PIPELINE_TYPE_TRIANGLESTRIP,
    PIPELINE_TYPE_TERRAIN,
#ifdef PERIMETER_DEBUG
    PIPELINE_TYPE_LINE_STRIP,
#endif
    PIPELINE_TYPE_MAX,
};
const PIPELINE_TYPE PIPELINE_TYPE_DEFAULT = PIPELINE_TYPE_TRIANGLE;

using pipeline_id_t = uint32_t;
using pipeline_mode_value_t = uint16_t;
struct PIPELINE_MODE {
    eBlendMode blend = ALPHA_NONE;
    eCullMode cull = CULL_NONE;
    eCMPFUNC depth_cmp = CMP_GREATER;
    bool depth_write = true;

    pipeline_mode_value_t GetValue() const;
    void FromValue(pipeline_mode_value_t value);
};

#endif //PERIMETER_SOKOLTYPES_H
