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

SokolBuffer::SokolBuffer(sg_buffer_desc* desc)
: buffer(sg_make_buffer(desc)) {
}

SokolBuffer::~SokolBuffer() {
    if (buffer.id != SG_INVALID_ID) {
        sg_destroy_buffer(buffer);
    }
}

void SokolBuffer::update(MemoryResource* resource, size_t len) {
    xassert(!resource->burned);
    xassert(!resource->locked);
    if (!resource->dirty) return;
    xassert(resource->data);
    if (!resource->data) return;

    xassert(len <= resource->data_len);
    if (len == 0) return;
    if (len > resource->data_len) len = resource->data_len;

    resource->burned = true;
    resource->dirty = false;
    sg_range range = {resource->data, len};
    sg_update_buffer(buffer, &range);
}

SokolTexture2D::SokolTexture2D(sg_image_desc* _desc)
: MemoryResource(_desc->usage == SG_USAGE_IMMUTABLE ? 0 : _desc->width * _desc->height * sokol_pixelformat_bytesize(_desc->pixel_format))
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

    if (desc) {
        xassert(desc->usage == SG_USAGE_IMMUTABLE || data);
        image = sg_make_image(desc);
        if (desc->usage == SG_USAGE_IMMUTABLE) {
            //Cleanup subimages
            for (int ci = 0; ci < SG_CUBEFACE_NUM; ++ci) {
                for (int i = 0; i < SG_MAX_MIPMAPS; ++i) {
                    sg_range& range = desc->data.subimage[ci][i];
                    if (!range.ptr) {
                        break;
                    }
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
