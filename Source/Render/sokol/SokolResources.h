#ifndef PERIMETER_SOKOLRESOURCES_H
#define PERIMETER_SOKOLRESOURCES_H
 
/*
 * Stuff related to sokol resource management
 * 
 * Sokol resources can only be updated at most once per per frame,
 * by storing the data on memory we can update several times until final snapshot is sent
 * to sokol resource and then into SokolCommand
 */

#include "MemoryResource.h"
#include "RenderMT.h"

using SokolResourceKey = uint64_t;
static const SokolResourceKey SokolResourceKeyNone = 0;

SokolResourceKey get_sokol_resource_key_buffer(size_t& len, sg_buffer_type type);
SokolResourceKey get_sokol_resource_key_texture(int& w, int& h, sg_pixel_format format);

size_t sokol_pixelformat_bytesize(sg_pixel_format fmt);

/**
 * Stores sokol resource reference with a refcount
 */
template<typename T>
class SokolResource {
private:
    int32_t refcount = 1;
    
    //Has to be implemented by template specialization for each T type
    void destroy_res();

    ~SokolResource() {
        if (0 < refcount) {
            xassert(refcount == 1);
            DecRef();
        }
    }
public:
    SokolResourceKey key = SokolResourceKeyNone;
    T res;

    SokolResource(SokolResourceKey key, T res) : key(key), res(res) {
        xassert(res.id != SG_INVALID_ID);
    }

    NO_COPY_CONSTRUCTOR(SokolResource<T>)

    int32_t RefCount() const {
        return refcount;
    }
    
    int32_t IncRef() {
        return ++refcount;
    }

    int32_t DecRef() {
        if (0 < refcount) {
            refcount--;
            if (refcount == 0) {
                MTG();
                destroy_res();
                delete this;
                return 0;
            }
        }
        return refcount;
    }
};

struct SokolBuffer {
    SokolResource<sg_buffer>* buffer = nullptr;
    explicit SokolBuffer(SokolResource<sg_buffer> *buffer);
    ~SokolBuffer();

    void release_buffer();
    void update(MemoryResource* resource, size_t len) const;
};

struct SokolTexture2D : MemoryResource {
#ifdef PERIMETER_DEBUG
    std::string label;
#endif
    sg_pixel_format pixel_format;
    sg_image_desc* desc = nullptr;
    SokolResourceKey resource_key = SokolResourceKeyNone;
    SokolResource<sg_image>* image = nullptr;

    explicit SokolTexture2D(sg_image_desc* desc);
    NO_COPY_CONSTRUCTOR(SokolTexture2D)
    ~SokolTexture2D();

    void update();
};

#endif //PERIMETER_SOKOLRESOURCES_H
