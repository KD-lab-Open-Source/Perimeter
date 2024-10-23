#ifndef PERIMETER_SOKOLRENDERPIPELINE_H
#define PERIMETER_SOKOLRENDERPIPELINE_H

#include "VertexFormat.h"

const uint32_t PERIMETER_SOKOL_PIPELINES_MAX = 128;

struct SokolPipeline {
    SokolPipelineContext context = {};
    //Created pipeline for this
    sg_pipeline pipeline = {};
    //Shader ID
    SOKOL_SHADER_ID shader_id = SOKOL_SHADER_ID_NONE;
    //Mapping of texture slots in fragment shader
    int shader_fs_texture_slot[PERIMETER_SOKOL_TEXTURES] = {};
    //Sampler slot
    int shader_fs_sampler_slot = 0;
    int shader_fs_shadow_sampler_slot = 0;
    
    SokolPipeline() = default;
    ~SokolPipeline();
};

#endif //PERIMETER_SOKOLRENDERPIPELINE_H
