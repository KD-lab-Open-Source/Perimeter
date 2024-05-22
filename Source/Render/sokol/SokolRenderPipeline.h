#ifndef PERIMETER_SOKOLRENDERPIPELINE_H
#define PERIMETER_SOKOLRENDERPIPELINE_H

#include "VertexFormat.h"

const uint32_t PIPELINE_ID_MODE_MASK = 0xFFFF;
const uint32_t PIPELINE_ID_MODE_BITS = 8;
static_assert((1 << PIPELINE_ID_MODE_BITS) - 1 < PIPELINE_ID_MODE_MASK);
const uint32_t PIPELINE_ID_VERTEX_FMT_BITS = (VERTEX_FMT_BITS - 1); //Remove 1 dedicated for Dot3 flag in D3D
const uint32_t PIPELINE_ID_VERTEX_FMT_MASK = (1 << PIPELINE_ID_VERTEX_FMT_BITS) - 1;
const uint32_t PIPELINE_ID_TYPE_MASK = 0xF;

const uint32_t PIPELINE_ID_MODE_SHIFT = 0;
const uint32_t PIPELINE_ID_VERTEX_FMT_SHIFT = 16;
const uint32_t PIPELINE_ID_TYPE_SHIFT = PIPELINE_ID_VERTEX_FMT_SHIFT + PIPELINE_ID_VERTEX_FMT_BITS;

const uint32_t PERIMETER_SOKOL_PIPELINES_MAX = 128;

struct SokolPipeline {
    SokolPipelineContext context{};
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
