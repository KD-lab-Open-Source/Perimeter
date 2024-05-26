#include "xerrhand.h"
#include "xmath.h"
#include <sokol_gfx.h>
#include "SokolResources.h"
#include "SokolTypes.h"

SokolResourceKey get_sokol_resource_key_buffer(size_t& len, sg_buffer_type type) {
    static const uint32_t LEN_SHIFT = 6; //64 bytes
    size_t chunks = (len >> LEN_SHIFT) + 1; //Divide by LEN_SHIFT and add one extra chunk
    len = chunks * (1 << LEN_SHIFT);
    SokolResourceKey key = len;
    key <<= 16;
    key |= static_cast<uint16_t>(type & 0xFFFF);
    return key;
}

SokolResourceKey get_sokol_resource_key_texture(int& w, int& h, sg_pixel_format format) {
    SokolResourceKey len = static_cast<uint32_t>(w);
    len <<= 32;
    len |= static_cast<uint32_t>(h);
    len <<= 8;
    len |= static_cast<uint8_t>(sokol_pixelformat_bytesize(format) & 0xFF);
    return len;
}

size_t sokol_pixelformat_bytesize(sg_pixel_format fmt) {
    //Probably the only pixel format used, so we cache it
    if (fmt == SG_PIXELFORMAT_RGBA8) {
        static int rgba8 = sg_query_pixelformat(fmt).bytes_per_pixel;
        return rgba8;
    }
    return sg_query_pixelformat(fmt).bytes_per_pixel;
}

template<>
void SokolResourceBuffer::destroy_res() {
    if (res.id != SG_INVALID_ID) {
        sg_destroy_buffer(res);
        res.id = SG_INVALID_ID;
    }
}

template<>
void SokolResourceTexture::destroy_res() {
    if (res.id != SG_INVALID_ID) {
        sg_destroy_image(res);
        res.id = SG_INVALID_ID;
    }
}

SokolBuffer::SokolBuffer(SokolResource<sg_buffer> *buffer): buffer(buffer) {
    xassert(buffer);
}

SokolBuffer::~SokolBuffer() {
    release_buffer();
}

void SokolBuffer::release_buffer() {
    if (buffer) {
        buffer->DecRef();
        buffer = nullptr;
    }
}

void SokolBuffer::update(MemoryResource* resource, size_t len) const {
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
    if (!buffer) {
        xassert(0);
        return;
    }
    xassert(buffer->res.id != SG_INVALID_ID);
    sg_range range = {resource->data, len};
    sg_update_buffer(buffer->res, &range);
}

SokolTexture2D::SokolTexture2D(sg_image_desc* _desc)
    : MemoryResource(_desc->usage == SG_USAGE_IMMUTABLE ? 0 : _desc->width * _desc->height * sokol_pixelformat_bytesize(_desc->pixel_format))
    , desc(_desc)
{
    pixel_format = desc->pixel_format;
}

SokolTexture2D::~SokolTexture2D() {
    FreeImages();
    if (image) {
        image->DecRef();
        image = nullptr;
    }
    delete desc;
}

void SokolTexture2D::FreeImages() {
    if (!desc) {
        return;
    }
    //Cleanup subimages
    for (int ci = 0; ci < SG_CUBEFACE_NUM; ++ci) {
        for (int i = 0; i < SG_MAX_MIPMAPS; ++i) {
            sg_range& range = desc->data.subimage[ci][i];
            if (!range.ptr) {
                break;
            }
            const uint8_t* buf = reinterpret_cast<const uint8_t*>(range.ptr);
            delete[] buf;
            range.ptr = nullptr;
        }
    }
}

void SokolTexture2D::update() {
    xassert(!locked);
    if (!dirty) return;
    dirty = false;

    if (!image) {
        xassert(0);
        return;
    }
    if (data) {
        xassert(image->res.id != SG_INVALID_ID);
        sg_image_data imageData = {};
        imageData.subimage[0][0] = {data, data_len};
        sg_update_image(image->res, &imageData);
    }
}
