#ifndef PERIMETER_AVWRAPPER_H
#define PERIMETER_AVWRAPPER_H

#include <string>

// FFMPEG include
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#ifdef __cplusplus
}
#endif

// compatability with newer libavcodec
#if LIBAVCODEC_VERSION_MAJOR < 57
#define AV_FRAME_ALLOC avcodec_alloc_frame
#define AV_PACKET_UNREF av_free_packet
#else
#define AV_FRAME_ALLOC av_frame_alloc
#define AV_PACKET_UNREF av_packet_unref
#endif

#define AV_CODEC_PAR (LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(57, 33, 100))

enum AVWrapperType {
    None,
    Audio,
    Video,
    AudioVideo
};

/**
 * Wraps the FFMPEG stuff like file and codec loading
 */
class AVWrapper
{
public:
    std::string file_path;
    AVFormatContext *formatCtx = nullptr;
    AVCodecContext *videoCodecCtx = nullptr;
    AVCodecContext *audioCodecCtx = nullptr;
    AVCodec *videoCodec = nullptr;
    AVCodec *audioCodec = nullptr;
    //Stores current decoded frames
    AVFrame *videoFrame = nullptr;
    AVFrame *audioFrame = nullptr;
    //Indexes for stream types
    int videoStream = -1;
    int audioStream = -1;
    
    AVWrapper() = default;

    ~AVWrapper() {
        close();
    }

    static void init();
    
    int open(const std::string& path, AVWrapperType need);
    int close();
    
    ///Loads the next packet, returns type of frame decoded
    AVWrapperType readPacket() const;
    
    ///Returns the buffer size required to contain the video frame buffer
    size_t getVideoFrameBufferSize() const;
    
    /**
     * Copies the current videoFrame content into provided buffer if any
     * If buffer pointed at is null, it allocates it before copying and sets the buffer pointer 
     * 
     * @param buffer the buffer that will contain the image data
     * @return av_image_copy_to_buffer result, negative for errors
     */
    int getVideoFrameBuffer(uint8_t** buffer) const;
};

#endif //PERIMETER_AVWRAPPER_H
