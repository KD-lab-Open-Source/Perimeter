#ifndef PERIMETER_AVIFILE_H
#define PERIMETER_AVIFILE_H

/**
 * Decodes .AVI format texture files
 * 
 * This decoder only supports a subset of specs used in the
 * game resources and will error on non supported features
 */
class AVIFile {
public:
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t time = 0; //Total time in ms
    uint32_t length = 0; //Amount of frames
    uint32_t tpf = 0; //Time per frame in ms
    uint8_t bpp = 0; //Bits per pixel
    std::vector<uint8_t*> frames;

    AVIFile();
    ~AVIFile();

    void close();
    int load(const char *fname);
};

#endif //PERIMETER_AVIFILE_H
