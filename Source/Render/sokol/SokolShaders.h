#ifndef PERIMETER_SOKOLSHADERS_H
#define PERIMETER_SOKOLSHADERS_H

#include "SokolTypes.h"

struct shader_funcs {
    SOKOL_SHADER_ID (*get_id)() = nullptr;
    const sg_shader_desc* (*shader_desc)(sg_backend) = nullptr;
    int (*attr_slot)(const char*) = nullptr;
    int (*image_slot)(sg_shader_stage, const char*) = nullptr;
    int (*sampler_slot)(sg_shader_stage, const char*) = nullptr;
    int (*uniformblock_slot)(sg_shader_stage, const char*) = nullptr;
    size_t (*uniformblock_size)(sg_shader_stage, const char*) = nullptr;
};

#include "sokol/shaders/mesh_color_tex1.h"
#include "sokol/shaders/mesh_color_tex2.h"
#include "sokol/shaders/mesh_normal_tex1.h"
#include "sokol/shaders/mesh_tex1.h"
#include "sokol/shaders/shadow_normal_tex1.h"
#include "sokol/shaders/shadow_tex1.h"
#include "sokol/shaders/tile_map.h"

#define SOKOL_SHADER(MODULE_NAME) \
extern shader_funcs shader_##MODULE_NAME;

#define SOKOL_SHADER_IMPL(MODULE_NAME) \
SOKOL_SHADER_ID MODULE_NAME##_get_shader_id() { \
    return SOKOL_SHADER_ID_##MODULE_NAME;    \
}; \
shader_funcs shader_##MODULE_NAME = { \
    MODULE_NAME##_get_shader_id, \
    MODULE_NAME##_program_shader_desc, \
    MODULE_NAME##_program_attr_slot, \
    MODULE_NAME##_program_image_slot, \
    MODULE_NAME##_program_sampler_slot, \
    MODULE_NAME##_program_uniformblock_slot, \
    MODULE_NAME##_program_uniformblock_size, \
};

SOKOL_SHADER(mesh_color_tex1);
SOKOL_SHADER(mesh_color_tex2);
SOKOL_SHADER(mesh_normal_tex1);
SOKOL_SHADER(mesh_tex1);
SOKOL_SHADER(shadow_tex1);
SOKOL_SHADER(shadow_normal_tex1);
SOKOL_SHADER(tile_map);

using mesh_normal_texture_vs_params_t = mesh_normal_tex1_mesh_normal_texture_vs_params_t;
using mesh_normal_texture_fs_params_t = mesh_normal_tex1_mesh_normal_texture_fs_params_t;
//mesh_color_tex1 and mesh_color_tex2 share the params struct, so we pick tex2
using mesh_color_texture_vs_params_t = mesh_color_tex2_mesh_color_texture_vs_params_t;
using mesh_color_texture_fs_params_t = mesh_color_tex2_mesh_color_texture_fs_params_t;
static_assert(sizeof(mesh_color_tex1_mesh_color_texture_vs_params_t) == sizeof(mesh_color_tex2_mesh_color_texture_vs_params_t));
static_assert(sizeof(mesh_color_tex1_mesh_color_texture_fs_params_t) == sizeof(mesh_color_tex2_mesh_color_texture_fs_params_t));
using mesh_texture_vs_params_t = mesh_tex1_mesh_texture_vs_params_t;
using tile_map_vs_params_t = tile_map_tile_map_vs_params_t;
using tile_map_fs_params_t = tile_map_tile_map_fs_params_t;
//shadow_tex1 and shadow_normal_tex1 share the params struct, so we pick shadow_tex1
using shadow_texture_vs_params_t = shadow_tex1_shadow_texture_vs_params_t;
using shadow_texture_fs_params_t = shadow_tex1_shadow_texture_fs_params_t;
static_assert(sizeof(shadow_tex1_shadow_texture_vs_params_t) == sizeof(shadow_normal_tex1_shadow_texture_vs_params_t));
static_assert(sizeof(shadow_tex1_shadow_texture_fs_params_t) == sizeof(shadow_normal_tex1_shadow_texture_fs_params_t));

#endif //PERIMETER_SOKOLSHADERS_H
