#include "xerrhand.h"
#include "xmath.h"
#include <sokol_gfx.h>
#include "SokolResources.h"
#include "SokolTypes.h"

static const uint32_t CHUNK_LEN_SHIFT = 6; //64 bytes

template<typename T>
uint16_t get_chunk_amount(T& len) {
    if (len < 0) {
        xassert(0);
        return 0;
    }
    size_t chunks = (len >> CHUNK_LEN_SHIFT) + 1; //Divide by LEN_SHIFT and add one extra chunk
    len = chunks * (1 << CHUNK_LEN_SHIFT);
    xassert(chunks <= 0xFFFF);
    return static_cast<uint16_t>(chunks);
}

SokolResourceKey get_sokol_resource_key_buffer(size_t& len, sg_buffer_type type) {
    SokolResourceKey key = get_chunk_amount(len);
    key <<= 16;
    key |= static_cast<uint16_t>(type & 0xFFFF);
    return key;
}

SokolResourceKey get_sokol_resource_key_texture(int& w, int& h, sg_pixel_format format) {
    xassert(0 < w && w <= 0xFFFF);
    xassert(0 < h && h <= 0xFFFF);
    SokolResourceKey key = static_cast<uint16_t>(w);
    key <<= 16;
    key |= static_cast<uint16_t>(h);
    key <<= 16;
    key |= static_cast<uint16_t>(format & 0xFFFF);
    return key;
}

size_t sokol_pixelformat_bytesize(sg_pixel_format fmt) {
    //Probably the only pixel formats used, so we cache it
    switch (fmt) {
        case SG_PIXELFORMAT_RGBA8: {
            static int bpp_rgba8 = sg_query_pixelformat(fmt).bytes_per_pixel;
            return bpp_rgba8;
        }
        case SG_PIXELFORMAT_DEPTH: {
            static int bpp_depth = sg_query_pixelformat(fmt).bytes_per_pixel;
            return bpp_depth;
        }
        default: {
            xassert(0);
            return sg_query_pixelformat(fmt).bytes_per_pixel;
        }
    }
}

template<>
void SokolResourceBuffer::destroy_res() {
    if (res.id != SG_INVALID_ID) {
        sg_destroy_buffer(res);
        res.id = SG_INVALID_ID;
    }
}

template<>
void SokolResourceImage::destroy_res() {
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
    xassert(!resource->locked);
    if (!resource->dirty) return;
    xassert(resource->data);
    if (!resource->data) return;

    xassert(len <= resource->data_len);
    if (len == 0) return;
    if (len > resource->data_len) len = resource->data_len;
    if (!buffer) {
        xassert(0);
        return;
    }
    xassert(!buffer->burned);
    buffer->burned = true;

    resource->dirty = false;

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
    if (!data) {
        xassert(0);
        return;
    }
    if (!image) {
        xassert(0);
        return;
    }
    xassert(!image->burned);
    image->burned = true;
    dirty = false;
    
    xassert(image->res.id != SG_INVALID_ID);
    sg_image_data imageData = {};
    imageData.subimage[0][0] = {data, data_len};
    sg_update_image(image->res, &imageData);
}
