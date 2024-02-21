#include "xerrhand.h"
#include "xmath.h"
#include <sokol_gfx.h>
#include "SokolResources.h"

size_t sokol_pixelformat_bytesize(sg_pixel_format fmt) {
    //Probably the only pixel format used, so we cache it
    if (fmt == SG_PIXELFORMAT_RGBA8) {
        static int rgba8 = sg_query_pixelformat(fmt).bytes_per_pixel;
        return rgba8;
    }
    return sg_query_pixelformat(fmt).bytes_per_pixel;
}

SokolBuffer::SokolBuffer(sg_buffer_desc* desc)
: buffer(sg_make_buffer(desc)) {
    xassert(buffer.id != SG_INVALID_ID);
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
    , desc(_desc)
{
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
#ifdef PERIMETER_DEBUG
        if (!label.empty()) {
            desc->label = label.c_str();
        }
#endif
        xassert(desc->usage == SG_USAGE_IMMUTABLE || data);
        image = sg_make_image(desc);
        xassert(image.id != SG_INVALID_ID);
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
