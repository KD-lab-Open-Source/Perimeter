// This file imports sokol header with implementation def set
#define SOKOL_IMPL
#include <sokol_gfx.h>

//Also implement shaders
#include "xmath.h"
#include "SokolShaders.h"
SOKOL_SHADER_IMPL(color_tex1);
SOKOL_SHADER_IMPL(color_tex2);
SOKOL_SHADER_IMPL(normal);
SOKOL_SHADER_IMPL(terrain);
