#ifndef PERIMETER_SOKOLRESOURCES_H
#define PERIMETER_SOKOLRESOURCES_H

/*
 * Stores sokol resources reference, malloc allocated memory and dirty flag
 * 
 * sokol resources can only be updated at most once per per frame, this way we can update several times
 */

//TODO copied from _sg_pixelformat_bytesize private function, remove if becomes public
size_t sokol_pixelformat_bytesize(sg_pixel_format fmt);

struct SokolBuffer {
    size_t elements;
    sg_buffer buffer = {};
    void* data = nullptr;
    size_t data_len = 0;
    bool dirty = true;
    bool locked = false;

    explicit SokolBuffer(size_t elements, const sg_buffer_desc& desc);
    SokolBuffer(const SokolBuffer&) = delete;
    SokolBuffer& operator=(SokolBuffer const&) = delete;

    ~SokolBuffer();
    
    void update();
};

struct SokolTexture2D {
    sg_pixel_format pixel_format;
    sg_image image = {};
    void* data = nullptr;
    size_t data_len = 0;
    bool dirty = true;
    bool locked = false;

    explicit SokolTexture2D(const sg_image_desc& desc);
    SokolTexture2D(const SokolTexture2D&) = delete;
    SokolTexture2D& operator=(SokolTexture2D const&) = delete;

    ~SokolTexture2D();

    void update();
};

#endif //PERIMETER_SOKOLRESOURCES_H
