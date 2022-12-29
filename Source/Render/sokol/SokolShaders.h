#ifndef PERIMETER_SOKOLSHADERS_H
#define PERIMETER_SOKOLSHADERS_H

struct shader_funcs {
    const sg_shader_desc* (*shader_desc)(sg_backend) = nullptr;
    int (*attr_slot)(const char*) = nullptr;
    int (*image_slot)(sg_shader_stage, const char*) = nullptr;
    int (*uniformblock_slot)(sg_shader_stage, const char*) = nullptr;
    size_t (*uniformblock_size)(sg_shader_stage, const char*) = nullptr;
};

#include "sokol/shaders/color_tex1.h"
#include "sokol/shaders/color_tex2.h"
#include "sokol/shaders/normal.h"

#define SOKOL_SHADER(MODULE_NAME) extern shader_funcs shader_##MODULE_NAME; 
#define SOKOL_SHADER_IMPL(MODULE_NAME) \
shader_funcs shader_##MODULE_NAME = { \
    MODULE_NAME##_program_shader_desc, \
    MODULE_NAME##_program_attr_slot, \
    MODULE_NAME##_program_image_slot, \
    MODULE_NAME##_program_uniformblock_slot, \
    MODULE_NAME##_program_uniformblock_size, \
};

SOKOL_SHADER(color_tex1);
SOKOL_SHADER(color_tex2);
SOKOL_SHADER(normal);

//All shaders should use same vs_params_t and fs_params_t format, so we take one as reference
using vs_params_t = color_tex2_vs_params_t;
using fs_params_t = color_tex2_fs_params_t;

#endif //PERIMETER_SOKOLSHADERS_H
