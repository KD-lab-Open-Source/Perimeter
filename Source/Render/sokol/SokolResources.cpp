#include "xerrhand.h"
#include "xmath.h"
#include "sokol_gfx.h"
#include "SokolResources.h"
#include "SokolShaders.h"

SOKOL_SHADER_IMPL(color);
SOKOL_SHADER_IMPL(color_tex1);
SOKOL_SHADER_IMPL(color_tex2);
SOKOL_SHADER_IMPL(tex1);
SOKOL_SHADER_IMPL(normal);
SOKOL_SHADER_IMPL(normal_dot3);

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

SokolBuffer::SokolBuffer(const sg_buffer_desc& desc) {
    buffer = sg_make_buffer(desc);
    if (desc.usage != SG_USAGE_IMMUTABLE) {
        data_len = desc.size;
        data = malloc(data_len);
    }
}

SokolBuffer::~SokolBuffer() {
    if (buffer.id != SG_INVALID_ID) sg_destroy_buffer(buffer);
    if (data) free(data);
}

void SokolBuffer::update() {
    xassert(!locked);
    if (dirty) {
        dirty = false;
        sg_range range = {data, data_len};
        sg_update_buffer(buffer, &range);
    }
}

SokolTexture2D::SokolTexture2D(const sg_image_desc& desc) {
    image = sg_make_image(desc);
    pixel_format = desc.pixel_format;
    //For non-immutable textures we need somewhere to hold CPU side data until is flushed into GPU
    if (desc.usage != SG_USAGE_IMMUTABLE) {
        data_len = desc.width * desc.height * sokol_pixelformat_bytesize(pixel_format);
        data = malloc(data_len);
    }
}

SokolTexture2D::~SokolTexture2D() {
    if (image.id != SG_INVALID_ID) sg_destroy_image(image);
    if (data) free(data);
}

void SokolTexture2D::update() {
    xassert(!locked);
    if (dirty) {
        dirty = false;
        sg_image_data imageData;
        imageData.subimage[0][0] = {data, data_len};
        sg_update_image(image, &imageData);
    }
}
