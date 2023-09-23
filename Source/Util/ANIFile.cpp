#include <cstdint>
#include <cstddef>
#include <vector>
#include "xerrhand.h"
#include "files/files.h"
#include "xutl.h"
#include "xstream.h"
#include "xmath.h"
#include "ANIFile.h"

bool isANIFile(const char* fname) {
    std::string file_path = convert_path_content(fname);
    char riff[4];
    char aconanih[8];
    XStream s;
    s.open(file_path);
    s.read(&riff, 4);
    if (memcmp(riff, "RIFF", 4) != 0) {
        s.close();
        return false;
    }
    s.seek(4, XS_CUR);
    s.read(&aconanih, 8);
    s.close();
    return memcmp(aconanih, "ACONanih", 8) == 0;
}

struct RIFF_chunk
{
    char id[4];
    uint32_t size;
};

struct ANIHeader
{
    RIFF_chunk riff;
    char header_acon[4];
    RIFF_chunk chunk_anih;
    uint32_t header_size;
    uint32_t frames; //Unique frames count
    uint32_t steps; //Amount of frames to display, including duplicates
    uint32_t widt_unused; //These are usually 0
    uint32_t height_unused; //These are usually 0
    uint32_t bpp;
    uint32_t planes;
    uint32_t fps;
    uint32_t flags;
} data;

ANIFile::ANIFile() = default;

ANIFile::~ANIFile() {
    close();
}

void ANIFile::close() {
    for (auto& frame : frames) {
        if (frame.data) {
            delete[] frame.data;
            frame.data = nullptr;
        }
    }
    frames.clear();
    slices.clear();
}

int ANIFile::load(const char *fname, bool read_frame_data) {
    std::string file_path = convert_path_content(fname);
    XStream s;
    s.open(file_path);
    s.read(&data, sizeof(ANIHeader));

    //Basic checks
    if (memcmp(data.riff.id, "RIFF", 4) != 0) {
        ErrH.Abort("File doesn't contain RIFF header ID " + file_path);
    }
    if (memcmp(data.header_acon, "ACON", 4) != 0) {
        ErrH.Abort("File doesn't contain ACON header ID " + file_path);
    }
    if (memcmp(data.chunk_anih.id, "anih", 4) != 0) {
        ErrH.Abort("File doesn't contain anih chunk " + file_path);
    }
    if (data.chunk_anih.size != 36 || data.header_size != 36) {
        ErrH.Abort("File anih chunk size invalid " + file_path);
    }
    if (!(data.flags & 0b1)) {
        ErrH.Abort("Animation is not icon/cursor " + file_path);
    }

    //Setup default data unless they are specified in chunks
    std::vector<uint32_t> frame_order;
    frame_order.reserve(data.frames);
    for (int i = 0; i < data.frames; ++i) {
        frame_order.emplace_back(i);
    }
    std::vector<int> frame_delay;
    frame_delay.reserve(data.steps);
    for (int i = 0; i < data.steps; ++i) {
        frame_delay.emplace_back(data.fps);
    }

    //Parse the chunks
    char* chunk_name = new char[5];
    uint32_t chunk_size = 0;
    while (!s.eof()) {
        s.read(chunk_name, 4);
        chunk_name[4] = '\0';
        s.read(chunk_size);
        size_t chunk_end = s.tell() + chunk_size;
        std::string name = chunk_name;
        if (name == "LIST") {
            s.read(chunk_name, 4);
            if (memcmp(chunk_name, "fram", 4) != 0) {
                ErrH.Abort("Unknown LIST type in " + file_path);
            }

            //Parse each frame in list
            for (int i = 0; i < data.frames; ++i) {
                RIFF_chunk icon_chunk {};
                s.read(icon_chunk);
                if (memcmp(icon_chunk.id, "icon", 4) != 0) {
                    ErrH.Abort("Unknown frame type in " + file_path);
                }

                ANIFrame frame;
                frame.data_len = icon_chunk.size;
                
                //Load hotspot
                int64_t pos = s.tell();
                s.seek(4, XS_CUR);
                uint16_t bfCount;
                s.read(bfCount);
                if (bfCount != 1) {
                    fprintf(stderr, "Couldnt parse hotspot from frame %d in %s\n", i, file_path.c_str());
                } else {
                    s.seek(4, XS_CUR);
                    uint16_t hx;
                    uint16_t hy;
                    s.read(hx);
                    s.read(hy);
                    frame.hotspot.set(hx, hy);
                }

                if (read_frame_data) {
                    s.seek(pos, XS_BEG);
                    //Load this frame data into memory
                    frame.data = new uint8_t[frame.data_len];
                    s.read(frame.data, frame.data_len);
                } else {
                    s.seek(pos + frame.data_len, XS_BEG);
                }
                
                frames.emplace_back(frame);

                if (s.pos & 1) {
                    s.pos += 1;
                }
            }
        } else if (name == "seq ") {
            frame_order.clear();
            uint32_t v = 0;
            while (s.tell() < chunk_end) {
                s.read(v);
                frame_order.emplace_back(v);
            }
            if (frame_order.size() != data.steps) {
                ErrH.Abort("Seq chunk size mismatch in " + file_path);
            }
        } else if (name == "rate") {
            frame_delay.clear();
            uint32_t v = 0;
            while (s.tell() < chunk_end) {
                s.read(v);
                frame_delay.emplace_back(static_cast<int>(v));
            }
            if (frame_delay.size() != data.steps) {
                ErrH.Abort("Rate chunk size mismatch in " + file_path);
            }
        } else {
            ErrH.Abort("Unknown chunk type in " + file_path);
        }
        if (s.tell() != chunk_end) {
            ErrH.Abort("Chunk end " + std::to_string(chunk_end)
                       + " but current pos is " + std::to_string(s.tell())
                       + " in " + file_path);
        }
    }
    s.close();

    //Calculate some time stuff
    time = 0;
    lowest_delay = 0;
    for (int i = 0; i < data.steps; ++i) {
        //Convert jiffies (1/60 sec) to ms
        frame_delay[i] = static_cast<int>(xm::round((static_cast<float>(frame_delay[i]) / 60.0) * 1000));
        int v = frame_delay[i];
        time += v;
        if (lowest_delay == 0 || v < lowest_delay) {
            lowest_delay = v;
        }
    }

    //Use the lowest delay to "slice" as smallest step
    length = time / lowest_delay;

    if (length <= 1) {
        tpf = 1;
    } else {
        tpf = (time - 1) / (length - 1);
    }

    //Create slices
    for (int i = 0; i < data.steps; ++i) {
        uint32_t index = frame_order[i];
        int delay = frame_delay[i];
        if (index >= frames.size()) {
            ErrH.Abort("Attempted to read frame " + std::to_string(index) + " which is out of bounds " + std::to_string(frames.size()));
        }
        //We may need to repeat some frames to match lowest delay slice
        int repetitions = delay / lowest_delay;
        while (repetitions--) {
            slices.emplace_back(index);
        }
    }

    return 0;
}
