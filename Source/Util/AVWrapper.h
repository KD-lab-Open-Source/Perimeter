#ifndef PERIMETER_AVWRAPPER_H
#define PERIMETER_AVWRAPPER_H

#include <list>
#include <string>
#include <SDL_audio.h>

// FFMPEG include
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/channel_layout.h>
#ifdef PERIMETER_FFMPEG_MOVIE
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#endif
#ifdef __cplusplus
}
#endif

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
    Video
};

class AVWrapper;
class AVWrapperFrame {
protected:
    friend class AVWrapper;
    ///Ref to wrapper
    AVWrapper* wrapper = nullptr;
    ///Frame data
    AVFrame *frame = nullptr;
    ///Frame type
    AVWrapperType type = AVWrapperType::None;
    ///Time for this frame in stream time base
    AVRational time_base = av_make_q(1, 1);
public:
    AVWrapperFrame();
    ~AVWrapperFrame();
    
    ///@return frame type
    AVWrapperType getType() const { return type; }

    ///@return buffer size required to contain the frame content
    size_t getBufferSize() const;

    ///@return frame presentation time base
    AVRational getTimeBase() const;
    
    ///@return frame presentation time
    double getPresentationTime() const;

    ///@return frame PTS value
    int64_t getPTS() const;

    /**
     * Copies the current frame content into provided buffer if any
     * If buffer pointed at is null, it allocates it before copying and sets the buffer pointer 
     * 
     * @param buffer the buffer that will contain the frame data
     * @return written bytes, negative for errors
     */
    size_t copyBuffer(uint8_t** buffer) const;
};

/**
 * Wraps the FFMPEG stuff like file and codec loading
 */
class AVWrapper
{
protected:
    friend class AVWrapperFrame;
    ///Format context
    AVFormatContext* formatCtx = nullptr;
    ///Video codec context
    AVCodecContext* videoCodecCtx = nullptr;
    ///Audio codec context
    AVCodecContext* audioCodecCtx = nullptr;
    ///Video codec
    AVCodec* videoCodec = nullptr;
    ///Audio codec
    AVCodec* audioCodec = nullptr;
    ///Packet used for stream
    AVPacket* streamPacket = nullptr; 
    ///Audio next pts for frames that lack it
    int64_t audioNextPTS = AV_NOPTS_VALUE;
#ifdef PERIMETER_FFMPEG_MOVIE
    ///Software scaler context
    SwsContext* swsCtx = nullptr;
    ///Software scaler output frame width
    int swsWidth = 0;
    ///Software scaler output frame height
    int swsHeight = 0;
    ///Software scaler output frame format
    AVPixelFormat swsFormat = AV_PIX_FMT_NONE;
    ///Audio converter output channel layout
    AVChannelLayout swrChannelLayout = {};
    ///Audio converter output rate
    int swrSampleRate = 0;
    ///Audio converter output format
    AVSampleFormat swrFormat = AV_SAMPLE_FMT_NONE;
    ///Audio converter filter graph
    AVFilterGraph *filterGraph = nullptr;
    AVFilterContext *buffersinkCtx = nullptr;
    AVFilterContext *buffersrcCtx = nullptr;
#endif

    ///Called when end is reached
    void onEnd();
    
    ///Called when a frame is decoded
    void handleFrame(AVWrapperFrame* frame);
    
    ///Pulls audio from filtergraph and creates frames
    void pullFilterAudio();
public:
    ///Index for video stream
    int videoStream = -1;
    ///Index for audio stream
    int audioStream = -1;
    ///File that was opened
    std::string file_path;
    ///True if we reached end of packets
    bool end = false;
    ///List of video frames available, must be deleted upon removing from list
    std::list<AVWrapperFrame*> videoFrames;
    ///List of audio frames available, must be deleted upon removing from list
    std::list<AVWrapperFrame*> audioFrames;
    
    AVWrapper() = default;

    ~AVWrapper() {
        close();
    }

    static void init();
    
    ///Opens file 
    int open(const std::string& path, AVWrapperType need);
    
    ///Setups software scaler for video
    bool setupVideoScaler(int width, int height, AVPixelFormat format, int flags);
    
    /////Build the audio converter to convert into desired format
    bool setupAudioConverter(AVSampleFormat dst_format, int dst_channels, int dst_frequency);
    
    ///Closes opened file and contexts
    int close();
    
    ///Loads the next packet
    ///@param drain true to flush when finishing the decoding instead of providing a packet
    void readPacket(bool drain = false);

    ///Clears audio and video frames
    void clearFrames();

    ///Get video codec width
    int getVideoCodecWidth() const;

    ///Get video codec height
    int getVideoCodecHeight() const;

    ///Get video output width
    int getVideoWidth() const;

    ///Get video output height
    int getVideoHeight() const;

    ///Get video codec bpp
    int getVideoBPP() const;

    ///Get the duration of file in seconds
    double getDuration() const;

    ///Provides approximate audio format from LIBAV to SDL
    static SDL_AudioFormat toSDLAudioFormat(AVSampleFormat fmt);
    
    ///Provides approximate audio format from SDL to LIBAV
    static AVSampleFormat fromSDLAudioFormat(SDL_AudioFormat fmt, bool planar);
};

static bool AVWrapperFrame_compare(const AVWrapperFrame* first, const AVWrapperFrame* second) {
    float f = first->getPresentationTime();
    float s = second->getPresentationTime();
    return f < s;
}

#endif //PERIMETER_AVWRAPPER_H
