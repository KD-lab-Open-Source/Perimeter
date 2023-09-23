#ifndef PERIMETER_MEMORYRESOURCE_H
#define PERIMETER_MEMORYRESOURCE_H

/**
 * Common attributes for resources that stay in system memory
 */
class MemoryResource {
public:
    void* data = nullptr;
    size_t data_len = 0;
    bool dirty = true;
    bool locked = false;
    bool burned = false;

    explicit MemoryResource(size_t _data_len) {
        AllocData(_data_len);
    }

    explicit MemoryResource() {
        AllocData(0);
    }
    
    NO_COPY_CONSTRUCTOR(MemoryResource)

    ~MemoryResource() {
        FreeData();
    }

    void AllocData(size_t _data_len);
    void FreeData();
};

#endif //PERIMETER_MEMORYRESOURCE_H
