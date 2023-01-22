#include "xerrhand.h"
#include "xmath.h"
#include "sokol_gfx.h"
#include "SokolResources.h"

size_t sokol_pixelformat_bytesize(sg_pixel_format fmt) {
    switch (fmt) {
        case SG_PIXELFORMAT_R8:
        case SG_PIXELFORMAT_R8SN:
        case SG_PIXELFORMAT_R8UI:
        case SG_PIXELFORMAT_R8SI:
            return 1;

        case SG_PIXELFORMAT_R16:
        case SG_PIXELFORMAT_R16SN:
        case SG_PIXELFORMAT_R16UI:
        case SG_PIXELFORMAT_R16SI:
        case SG_PIXELFORMAT_R16F:
        case SG_PIXELFORMAT_RG8:
        case SG_PIXELFORMAT_RG8SN:
        case SG_PIXELFORMAT_RG8UI:
        case SG_PIXELFORMAT_RG8SI:
            return 2;

        case SG_PIXELFORMAT_R32UI:
        case SG_PIXELFORMAT_R32SI:
        case SG_PIXELFORMAT_R32F:
        case SG_PIXELFORMAT_RG16:
        case SG_PIXELFORMAT_RG16SN:
        case SG_PIXELFORMAT_RG16UI:
        case SG_PIXELFORMAT_RG16SI:
        case SG_PIXELFORMAT_RG16F:
        case SG_PIXELFORMAT_RGBA8:
        case SG_PIXELFORMAT_RGBA8SN:
        case SG_PIXELFORMAT_RGBA8UI:
        case SG_PIXELFORMAT_RGBA8SI:
        case SG_PIXELFORMAT_BGRA8:
        case SG_PIXELFORMAT_RGB10A2:
        case SG_PIXELFORMAT_RG11B10F:
            return 4;

        case SG_PIXELFORMAT_RG32UI:
        case SG_PIXELFORMAT_RG32SI:
        case SG_PIXELFORMAT_RG32F:
        case SG_PIXELFORMAT_RGBA16:
        case SG_PIXELFORMAT_RGBA16SN:
        case SG_PIXELFORMAT_RGBA16UI:
        case SG_PIXELFORMAT_RGBA16SI:
        case SG_PIXELFORMAT_RGBA16F:
            return 8;

        case SG_PIXELFORMAT_RGBA32UI:
        case SG_PIXELFORMAT_RGBA32SI:
        case SG_PIXELFORMAT_RGBA32F:
            return 16;

        default:
            return 0;
    }
}

SokolResource::SokolResource(size_t _data_len) {
    data_len = _data_len;
    data = malloc(data_len);
}

void SokolResource::FreeData() {
    if (data) {
        free(data);
        data = nullptr;
    }
    data_len = 0;
}

SokolResource::~SokolResource() {
    FreeData();
}

SokolBuffer::SokolBuffer(sg_buffer_desc* _desc)
: SokolResource(_desc->size)
, desc(_desc) {
}

SokolBuffer::~SokolBuffer() {
    if (buffer.id != SG_INVALID_ID) sg_destroy_buffer(buffer);
    delete desc;
}

void SokolBuffer::update(size_t len) {
    xassert(!locked);
    if (!dirty) return;
    dirty = false;
    xassert(data);
    if (!data) return;

    xassert(len && len <= data_len);
    if (len == 0) return;
    if (len > data_len) len = data_len;
    
    if (desc) {
        if (desc->usage == SG_USAGE_IMMUTABLE) {
            desc->data = {data, data_len};
        }
        buffer = sg_make_buffer(desc);
        if (desc->usage == SG_USAGE_IMMUTABLE) {
            FreeData();
        }
        delete desc;
        desc = nullptr;
    } 
    
    if (data) {
        sg_range range = {data, len};
        sg_update_buffer(buffer, &range);
    }
}

SokolTexture2D::SokolTexture2D(sg_image_desc* _desc)
: SokolResource(_desc->usage == SG_USAGE_IMMUTABLE ? 0 : _desc->width * _desc->height * sokol_pixelformat_bytesize(_desc->pixel_format))
, desc(_desc) {
    pixel_format = desc->pixel_format;
}

SokolTexture2D::~SokolTexture2D() {
    if (image.id != SG_INVALID_ID) sg_destroy_image(image);
    delete desc;
}

void SokolTexture2D::update() {
    xassert(!locked);
    if (!dirty) return;
    dirty = false;
    xassert(data);
    if (!data) return;

    if (desc) {
        image = sg_make_image(desc);
        if (desc->usage == SG_USAGE_IMMUTABLE) {
            //Cleanup buffers
            for (int i = 0; i < SG_MAX_MIPMAPS; ++i) {
                sg_range& range = desc->data.subimage[0][i];
                if (range.size) {
                    const uint8_t* buf = reinterpret_cast<const uint8_t*>(range.ptr);
                    delete[] buf;
                }
            }
            FreeData();
        }
        delete desc;
        desc = nullptr;
    } 
    
    if (data) {
        sg_image_data imageData;
        imageData.subimage[0][0] = {data, data_len};
        sg_update_image(image, &imageData);
    }
}
