#ifndef PERIMETER_SOKOLRENDERPIPELINE_H
#define PERIMETER_SOKOLRENDERPIPELINE_H

#include "VertexFormat.h"

enum PIPELINE_TYPE {
    PIPELINE_TYPE_TRIANGLE,
    PIPELINE_TYPE_TRIANGLESTRIP,
    PIPELINE_TYPE_TERRAIN,
    PIPELINE_TYPE_MAX,
};
const PIPELINE_TYPE PIPELINE_TYPE_DEFAULT = PIPELINE_TYPE_TRIANGLE;

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

using pipeline_mode_value_t = uint16_t;
struct PIPELINE_MODE {
    eBlendMode blend = ALPHA_NONE;
    eCullMode cull = CULL_NONE;
    eCMPFUNC depth_cmp = CMP_GREATER;
    bool depth_write = true;
    
    pipeline_mode_value_t GetValue() const;
    void FromValue(pipeline_mode_value_t value);
};

const uint32_t PIPELINE_ID_MODE_MASK = 0xFFFF;
const uint32_t PIPELINE_ID_MODE_MAX = 1 << 8;
static_assert((PIPELINE_ID_MODE_MAX - 1) < PIPELINE_ID_MODE_MASK);
static_assert(sizeof(vertex_fmt_t) == 1); //Change vertex fmt mask if this fails 
const uint32_t PIPELINE_ID_VERTEX_FMT_MASK = 0xFF;
const uint32_t PIPELINE_ID_TYPE_MASK = 0xF;

const uint32_t PIPELINE_ID_MODE_SHIFT = 0;
const uint32_t PIPELINE_ID_VERTEX_FMT_SHIFT = 16;
const uint32_t PIPELINE_ID_TYPE_SHIFT = 24;
const uint32_t PIPELINE_ID_MAX = PIPELINE_TYPE_MAX << PIPELINE_ID_TYPE_SHIFT;

struct SokolPipeline {
    //Pipeline ID
    pipeline_id_t id;
    //Pipeline type
    PIPELINE_TYPE type;
    //VERTEX_FMT_* flags used as index for this pipeline
    vertex_fmt_t vertex_fmt;
    //Amount of bytes for this vertex fmt
    size_t vertex_size;
    //Created pipeline for this
    sg_pipeline pipeline;
    //Shader functions to retrieve info
    struct shader_funcs* shader_funcs;
};

#endif //PERIMETER_SOKOLRENDERPIPELINE_H
