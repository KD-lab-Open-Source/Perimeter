#ifndef PERIMETER_ANIFILE_H
#define PERIMETER_ANIFILE_H

bool isANIFile(const char* fname);

struct ANIFrame {
    Vect2i size = {};
    //CUR/ICO data
    uint8_t* data = nullptr;
    size_t data_len = 0;
    //Hotspot for this frame
    Vect2i hotspot = {};
};

/**
 * Decodes .ANI format files
 */
class ANIFile {
public:
    Vect2i size;
    int lowest_delay = 0; //Lowest delay of frame
    int time = 0; //Total time in ms if animated or 0
    int length = 0; //Amount of frames if is animated or 1
    int tpf = 0; //Time per frame in ms

    std::vector<ANIFrame> frames;
    //Frame index for each slice
    std::vector<size_t> slices;

    ANIFile();
    ~ANIFile();

    void close();
    int load(const char *fname, bool read_frame_data);
};

#endif //PERIMETER_ANIFILE_H
