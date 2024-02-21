#ifndef PERIMETER_SOKOLRESOURCES_H
#define PERIMETER_SOKOLRESOURCES_H

/*
 * Stores sokol resources reference, malloc allocated memory and dirty flag
 * 
 * sokol resources can only be updated at most once per per frame, this way we can update several times
 */

#include "MemoryResource.h"

size_t sokol_pixelformat_bytesize(sg_pixel_format fmt);

struct SokolBuffer {
    sg_buffer buffer = {};

    explicit SokolBuffer(sg_buffer_desc* desc);
    NO_COPY_CONSTRUCTOR(SokolBuffer)
    ~SokolBuffer();
    
    void update(MemoryResource* resource, size_t len);
};

struct SokolTexture2D : MemoryResource {
#ifdef PERIMETER_DEBUG
    std::string label;
#endif
    sg_pixel_format pixel_format;
    sg_image_desc* desc = nullptr;
    sg_image image = {};

    explicit SokolTexture2D(sg_image_desc* desc);
    NO_COPY_CONSTRUCTOR(SokolTexture2D)
    ~SokolTexture2D();

    void update();
};

#endif //PERIMETER_SOKOLRESOURCES_H
