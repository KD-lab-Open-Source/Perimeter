#ifndef PERIMETER_SOKOLSHADERS_H
#define PERIMETER_SOKOLSHADERS_H

using shader_id_t = uint8_t;

const shader_id_t shader_id_color_tex1 = 0;
const shader_id_t shader_id_color_tex2 = 1;
const shader_id_t shader_id_normal = 2;
const shader_id_t shader_id_terrain = 3;

struct shader_funcs {
    shader_id_t (*get_id)() = nullptr;
    const sg_shader_desc* (*shader_desc)(sg_backend) = nullptr;
    int (*attr_slot)(const char*) = nullptr;
    int (*image_slot)(sg_shader_stage, const char*) = nullptr;
    int (*uniformblock_slot)(sg_shader_stage, const char*) = nullptr;
    size_t (*uniformblock_size)(sg_shader_stage, const char*) = nullptr;
};

#include "sokol/shaders/color_tex1.h"
#include "sokol/shaders/color_tex2.h"
#include "sokol/shaders/normal.h"
#include "sokol/shaders/terrain.h"

#define SOKOL_SHADER(MODULE_NAME) \
extern shader_funcs shader_##MODULE_NAME;

#define SOKOL_SHADER_IMPL(MODULE_NAME) \
shader_id_t MODULE_NAME##_get_shader_id() { \
    return shader_id_##MODULE_NAME;    \
}; \
shader_funcs shader_##MODULE_NAME = { \
    MODULE_NAME##_get_shader_id, \
    MODULE_NAME##_program_shader_desc, \
    MODULE_NAME##_program_attr_slot, \
    MODULE_NAME##_program_image_slot, \
    MODULE_NAME##_program_uniformblock_slot, \
    MODULE_NAME##_program_uniformblock_size, \
};

SOKOL_SHADER(color_tex1);
SOKOL_SHADER(color_tex2);
SOKOL_SHADER(normal);
SOKOL_SHADER(terrain);

using normal_texture_vs_params_t = normal_normal_texture_vs_params_t;
//using normal_texture_fs_params_t = normal_normal_texture_fs_params_t;
//color_tex1 and color_tex2 share the params struct, so we pick tex2
using color_texture_vs_params_t = color_tex2_color_texture_vs_params_t;
using color_texture_fs_params_t = color_tex2_color_texture_fs_params_t;
using terrain_vs_params_t = terrain_terrain_vs_params_t;
using terrain_fs_params_t = terrain_terrain_fs_params_t;

#endif //PERIMETER_SOKOLSHADERS_H
