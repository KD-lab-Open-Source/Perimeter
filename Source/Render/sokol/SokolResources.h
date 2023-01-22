#ifndef PERIMETER_SOKOLRESOURCES_H
#define PERIMETER_SOKOLRESOURCES_H

/*
 * Stores sokol resources reference, malloc allocated memory and dirty flag
 * 
 * sokol resources can only be updated at most once per per frame, this way we can update several times
 */

//TODO copied from _sg_pixelformat_bytesize private function, remove if becomes public
size_t sokol_pixelformat_bytesize(sg_pixel_format fmt);

/**
 * Common attributes for sokol resources
 */
struct SokolResource {
    void* data = nullptr;
    size_t data_len = 0;
    bool dirty = true;
    bool locked = false;
    explicit SokolResource(size_t data_len);
    NO_COPY_CONSTRUCTOR(SokolResource)
    ~SokolResource();
    void FreeData();
};

struct SokolBuffer : SokolResource {
    sg_buffer_desc* desc = nullptr;
    sg_buffer buffer = {};

    explicit SokolBuffer(sg_buffer_desc* desc);
    NO_COPY_CONSTRUCTOR(SokolBuffer)
    ~SokolBuffer();
    
    void update(size_t len);
};

struct SokolTexture2D : SokolResource {
    sg_pixel_format pixel_format;
    sg_image_desc* desc = nullptr;
    sg_image image = {};

    explicit SokolTexture2D(sg_image_desc* desc);
    NO_COPY_CONSTRUCTOR(SokolTexture2D)
    ~SokolTexture2D();

    void update();
};

#endif //PERIMETER_SOKOLRESOURCES_H
