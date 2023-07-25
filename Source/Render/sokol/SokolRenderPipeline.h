#ifndef PERIMETER_SOKOLRENDERPIPELINE_H
#define PERIMETER_SOKOLRENDERPIPELINE_H

#include "VertexFormat.h"

static PIPELINE_TYPE getPipelineType(const ePrimitiveType type) {
    switch (type) {
        default:
            xassert(0);
        case PT_TRIANGLESTRIP:
            return PIPELINE_TYPE_TRIANGLESTRIP;
        case PT_TRIANGLES:
            return PIPELINE_TYPE_TRIANGLE;
    }
}

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
    //Pipeline ID
    pipeline_id_t id = 0;
    //Pipeline type
    PIPELINE_TYPE type = PIPELINE_TYPE_DEFAULT;
    //VERTEX_FMT_* flags used as index for this pipeline
    vertex_fmt_t vertex_fmt = 0;
    //Created pipeline for this
    sg_pipeline pipeline = {};
    //Shader functions to retrieve info
    struct shader_funcs* shader_funcs = nullptr;
    //Shader ID
    SOKOL_SHADER_ID shader_id = SOKOL_SHADER_ID_NONE;
    //Mapping of texture slots in fragment shader
    int shader_fs_texture_slot[PERIMETER_SOKOL_TEXTURES] = {};
    
    SokolPipeline() = default;
    ~SokolPipeline();
};

#endif //PERIMETER_SOKOLRENDERPIPELINE_H
