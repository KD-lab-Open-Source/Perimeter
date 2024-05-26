#include <cstdint>
#include <cstddef>
#include <vector>
#include "xerrhand.h"
#include "files/files.h"
#include "xstream.h"
#include "AVIFile.h"
#include "xmath.h"

//const uint32_t AVI_FLAG_HAS_INDEX = 1 << 4;
const uint32_t AVI_FLAG_USE_INDEX = 1 << 5;
const uint32_t AVI_FLAG_INTERLEAVED = 1 << 8;

struct RIFF_chunk {
    char id[4];
    uint32_t size;
};

struct LIST_chunk {
    char list[4];
    uint32_t size;
    char type[4];
};

struct AVIMainHeader {
    //RIFF header
    char riff[4];
    uint32_t size;
    char type[4];
    
    //LIST hdrl
    LIST_chunk chunk_hdrl;
    
    //Main header
    RIFF_chunk avih;
    uint32_t microsecs_per_frame;
    uint32_t max_bytes_per_sec_unused;
    uint32_t padding_granularity_unused;
    uint32_t flags;
    uint32_t total_frames;
    uint32_t initial_frames;
    uint32_t streams;
    uint32_t suggested_buffer_size_unused;
    uint32_t width;
    uint32_t height;
    uint32_t reserved_unused[4];
    
    //LIST strl
    LIST_chunk chunk_strl;
};

struct AVIStreamHeader {
    RIFF_chunk strh;
    char type[4];
    char handler[4];
    uint32_t flags_unused;
    uint16_t priority_unused;
    uint16_t language_unused;
    uint32_t initial_frames;
    uint32_t scale;
    uint32_t rate;
    uint32_t start;
    uint32_t length;
    uint32_t suggested_buffer_size_unused;
    uint32_t quality_unused;
    uint32_t sample_size_unused;
    int16_t  frame_rect_unused[4];
};

struct AVIStreamFormat {
    RIFF_chunk strf;
    uint32_t size_unused;
    int32_t width;
    int32_t height;
    int16_t planes;
    int16_t bpp;
    uint32_t compression;
    uint32_t size_image_unused;
    int32_t x_pels_per_meter_unused;
    int32_t y_pels_per_meter_unused;
    uint32_t color_used_unused;
    uint32_t color_important_unused;
};

AVIFile::AVIFile() = default;

AVIFile::~AVIFile() {
    close();
}

void AVIFile::close() {
    for (auto& frame : frames) {
        delete[] frame;
    }
    frames.clear();
}

int AVIFile::load(const char *fname) {
    std::string file_path = convert_path_content(fname);
    XStream s;
    s.open(file_path);

    //Main header
    AVIMainHeader main_header = {};
    s.read(&main_header, sizeof(AVIMainHeader));
#ifdef PERIMETER_DEBUG
    if (memcmp(main_header.riff, "RIFF", 4) != 0) {
        ErrH.Abort("File doesn't contain RIFF header ID " + file_path);
    }
    if (memcmp(main_header.type, "AVI ", 4) != 0) {
        ErrH.Abort("File doesn't contain AVI ID " + file_path);
    }
    if (memcmp(main_header.chunk_hdrl.list, "LIST", 4) != 0) {
        ErrH.Abort("File doesn't contain hdrl LIST " + file_path);
    }
    if (memcmp(main_header.chunk_hdrl.type, "hdrl", 4) != 0) {
        ErrH.Abort("File doesn't contain hdrl type " + file_path);
    }
    if (main_header.chunk_hdrl.size != 192) {
        ErrH.Abort("File hdrl chunk size invalid " + file_path);
    }
    if (main_header.avih.size != 56) {
        ErrH.Abort("File avih chunk size invalid " + file_path);
    }
#endif
    if (memcmp(main_header.avih.id, "avih", 4) != 0) {
        ErrH.Abort("File doesn't contain avih ID " + file_path);
    }
    if ((main_header.flags & AVI_FLAG_INTERLEAVED) && main_header.initial_frames != 0) {
        ErrH.Abort("Interleaved AVI not supported " + file_path);
    }
    if (main_header.streams != 1) {
        ErrH.Abort("Only AVI with 1 stream is supported " + file_path);
    }
    if (main_header.flags & AVI_FLAG_USE_INDEX) {
        ErrH.Abort("Indexed AVI not supported " + file_path);
    }
#ifdef PERIMETER_DEBUG
    if (memcmp(main_header.chunk_strl.list, "LIST", 4) != 0) {
        ErrH.Abort("File doesn't contain strl LIST " + file_path);
    }
    if (memcmp(main_header.chunk_strl.type, "strl", 4) != 0) {
        ErrH.Abort("File doesn't contain strl type " + file_path);
    }
#endif

    //strl's strh Stream Header
    AVIStreamHeader stream_handler = {};
    s.read(&stream_handler, sizeof(AVIStreamHeader));
    if (memcmp(stream_handler.strh.id, "strh", 4) != 0) {
        ErrH.Abort("File doesn't contain strh ID " + file_path);
    }
    if (stream_handler.strh.size != 56) {
        ErrH.Abort("File first stream strh chunk size invalid " + file_path);
    }
    if (memcmp(stream_handler.type, "vids", 4) != 0) {
        ErrH.Abort("File first stream is not a video stream " + file_path);
    }
    if (memcmp(stream_handler.handler, "DIB ", 4) != 0) {
        ErrH.Abort("File first stream is not a uncompressed video stream " + file_path);
    }
    if (stream_handler.initial_frames != 0) {
        ErrH.Abort("Interleaved AVI not supported (stream initial frame) " + file_path);
    }
    if (stream_handler.initial_frames != 0) {
        ErrH.Abort("Interleaved AVI not supported (stream initial frame) " + file_path);
    }
    if (stream_handler.start != 0) {
        ErrH.Abort("Interleaved AVI not supported (stream start) " + file_path);
    }

#ifdef PERIMETER_DEBUG
    //Sanity check the samples
    float samples_per_sec = static_cast<float>(stream_handler.rate) / static_cast<float>(stream_handler.scale);
    int microsecs_per_frame = xm::round(1000000.0f / samples_per_sec);
    if (microsecs_per_frame != main_header.microsecs_per_frame) {
        ErrH.Abort("AVI framerate mismatch between main and stream headers " + file_path);
    }
    if (main_header.total_frames != stream_handler.length) {
        ErrH.Abort("AVI frame count mismatch between main and stream headers " + file_path);
    }
#endif

    //strl's strf Stream Format
    AVIStreamFormat stream_format = {};
    s.read(&stream_format, sizeof(AVIStreamFormat));
    if (memcmp(stream_format.strf.id, "strf", 4) != 0) {
        ErrH.Abort("File doesn't contain strh ID " + file_path);
    }
    if (stream_format.strf.size != 40) {
        ErrH.Abort("File first video stream strf chunk size invalid " + file_path);
    }
#ifdef PERIMETER_DEBUG
    if (stream_format.planes != 1) {
        ErrH.Abort("File first video stream planes is not 1 " + file_path);
    }
#endif
    if (stream_format.bpp != 24 && stream_format.bpp != 32) {
        ErrH.Abort("File first video stream bbp is not 24 or 32 " + file_path);
    }
    if (stream_format.compression != 0) {
        ErrH.Abort("File first video stream format is not uncompressed RGB " + file_path);
    }
    if (main_header.width != stream_format.width) {
        ErrH.Abort("AVI frame width mismatch between main header and stream format " + file_path);
    }
    if (main_header.height != stream_format.height) {
        ErrH.Abort("AVI frame height mismatch between main header and stream format " + file_path);
    }
    
    //Parse the chunks
    size_t frame_length_expected = main_header.width * main_header.height
                                 * (stream_format.bpp == 32 ? 4 : stream_format.bpp == 24 ? 3 : 0);
    char chunk_name[5];
    chunk_name[4] = '\0';
    uint32_t chunk_size = 0;
    bool done = false;
    while (!done && !s.eof()) {
        s.read(chunk_name, 4);
        s.read(chunk_size);
        size_t chunk_end = s.tell() + chunk_size;
        std::string name = chunk_name;
        if (name == "LIST") {
            s.read(chunk_name, 4);
            if (memcmp(chunk_name, "movi", 4) != 0) {
                ErrH.Abort("LIST movi expected, unknown LIST type in " + file_path);
            }
            size_t frames_start = s.tell();
            for (size_t frame_i = 0; frame_i < main_header.total_frames; ++frame_i) {
                s.read(chunk_name, 4);
                if (memcmp(chunk_name, "00db", 4) != 0) {
                    ErrH.Abort("Video subchunk 00db at movi expected, unknown chunk in " + file_path);
                }
                s.read(chunk_size);
                if (frame_length_expected != chunk_size) {
                    ErrH.Abort("Video subchunk length doesn't match expected size in " + file_path);
                }
                uint8_t* data = new uint8_t[chunk_size];
                s.read(data, chunk_size);
                frames.push_back(data);
#ifdef PERIMETER_DEBUG
                size_t frame_end = frames_start + (8 + frame_length_expected) * (frame_i + 1);
                if (s.tell() != frame_end) {
                    ErrH.Abort("Frame end " + std::to_string(frame_end)
                               + " but current pos is " + std::to_string(s.tell())
                               + " in " + file_path);
                }
#endif
            }
            done = true;
        } else if (name == "JUNK") {
            s.seek(chunk_size, XS_CUR);
        } else if (name == "idx1") {
            s.seek(chunk_size, XS_CUR);
            done = true;
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

    tpf = main_header.microsecs_per_frame / 1000;
    length = main_header.total_frames;
    time = length * tpf;
    width = main_header.width;
    height = main_header.height;
    bpp = stream_format.bpp;

    return 0;
}
