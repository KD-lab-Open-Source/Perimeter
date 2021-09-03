#ifdef PERIMETER_FFMPEG

#include "xerrhand.h"
#include "AVWrapper.h"

void AVWrapper::init() {
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
    av_register_all();
#endif
}

int AVWrapper::close() {
    // Free the frames
    if (videoFrame) av_frame_free(&videoFrame);
    if (audioFrame) av_frame_free(&audioFrame);
    
    // Close the codecs
    if (videoCodecCtx) avcodec_free_context(&videoCodecCtx);
    if (audioCodecCtx) avcodec_free_context(&audioCodecCtx);
    videoCodec = nullptr;
    audioCodec = nullptr;
    
    // Close the file stream
    if (formatCtx) avformat_close_input(&formatCtx);

    return 0;
}

int AVWrapper::open(const std::string& path, AVWrapperType need) {
    file_path = path;
    // Open video file
    formatCtx = nullptr;

    int ret = avformat_open_input(&formatCtx, file_path.c_str(), NULL, NULL);
    if (ret != 0) {
        char error_message[256];
        av_strerror(ret, error_message, 256);
        ErrH.Abort("Couldn't open video file:" + file_path + " " + error_message);
        return 1; // Couldn't open file
    }
    
    // Retrieve stream information
    if(avformat_find_stream_info(formatCtx, NULL) < 0) {
        ErrH.Abort("Couldn't find stream information - file:" + file_path);
        return 1; // Couldn't find stream information
    }
    
    // Find the first video or audio stream
    bool isVideo, isAudio;
    videoStream=-1;
    audioStream=-1;
    for (int i=0; i<static_cast<int>(formatCtx->nb_streams); i++) {
#if AV_CODEC_PAR
        isVideo = formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO;
        isAudio = formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO;
#else
        isVideo = formatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO;
        isAudio = formatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO;
#endif
        if (isVideo && videoStream == -1) {
            videoStream=i;
        }
        if (isAudio && audioStream == -1) {
            audioStream=i;
        }
    }

    //Attempt to load video codec
    if (videoStream == -1) {
        switch (need) {
            case AVWrapperType::Video:
            case AVWrapperType::AudioVideo:
                ErrH.Abort("Didn't find any video stream in file:" + file_path);
                return 1;
            default:
                break;
        }
    } else {
        // Get a pointer to the codec context for the video stream
#if AV_CODEC_PAR
        videoCodecCtx=avcodec_alloc_context3(nullptr);
        if (!videoCodecCtx) {
            ErrH.Abort("Unabled to allocate video codec context - file:" + file_path);
            return 1;
        }
        ret = avcodec_parameters_to_context(videoCodecCtx, formatCtx->streams[videoStream]->codecpar);
        if (ret != 0) {
            ErrH.Abort("Couldn't get the video codec context - file:" + file_path);
            return 1;
        }
#else
        videoCodecCtx=formatCtx->streams[videoStream]->codec;
#endif

        // Find the decoder for the video stream
        videoCodec=avcodec_find_decoder(videoCodecCtx->codec_id);
        if (videoCodec == nullptr) {
            ErrH.Abort("Unsupported video codec! - file:" + file_path);
            return 1;
        }
        // Open codec
#if (LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53, 8, 0))
        if (avcodec_open2(videoCodecCtx, videoCodec, nullptr) < 0) {
#else
        if (avcodec_open(videoCodecCtx, videoCodec) < 0) {
#endif
            ErrH.Abort("Could not open video codec - file:" + file_path);
            return 1;
        }

        // Allocate video frame
        videoFrame=AV_FRAME_ALLOC();
    }
        
    //Attempt to load audio codec
    if (audioStream == -1) {
        switch (need) {
            case AVWrapperType::Audio:
            case AVWrapperType::AudioVideo:
                ErrH.Abort("Didn't find any audio stream in file:" + file_path);
                return 1;
            default:
                break;
        }
    } else {
        // Get a pointer to the codec context for the audio stream
#if AV_CODEC_PAR
        audioCodecCtx=avcodec_alloc_context3(nullptr);
        if (!audioCodecCtx) {
            ErrH.Abort("Unabled to allocate audio codec context - file:" + file_path);
            return 1;
        }
        ret = avcodec_parameters_to_context(audioCodecCtx, formatCtx->streams[audioStream]->codecpar);
        if (ret != 0) {
            ErrH.Abort("Couldn't get the audio codec context - file:" + file_path);
            return 1;
        }
#else
        audioCodecCtx=formatCtx->streams[audioStream]->codec;
#endif

        // Find the decoder for the audio stream
        audioCodec=avcodec_find_decoder(audioCodecCtx->codec_id);
        if (audioCodec == nullptr) {
            ErrH.Abort("Unsupported audio codec! - file:" + file_path);
            return 1;
        }
        // Open codec
#if (LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53, 8, 0))
        if (avcodec_open2(audioCodecCtx, audioCodec, nullptr) < 0) {
#else
        if (avcodec_open(audioCodecCtx, audioCodec) < 0) {
#endif
            ErrH.Abort("Could not open audio codec - file:" + file_path);
            return 1;
        }

        // Allocate audio frame
        audioFrame=AV_FRAME_ALLOC();
    }

    return 0;
}

AVWrapperType AVWrapper::readPacket() const {
    AVWrapperType type = AVWrapperType::None;
    AVPacket* packet = av_packet_alloc();
    
    //Attempt to get a packet from file
    int ret = av_read_frame(formatCtx, packet);
    if (0 <= ret) {
        // Is this a packet from the video stream?
        if(packet->stream_index==videoStream) {
            int frameFinished = 0;
            // Decode video frame
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(57, 106, 102)
            while (true) {
                int ret = avcodec_send_packet(videoCodecCtx, packet);
                if (ret != 0 && ret != AVERROR(EAGAIN)) {
                    ErrH.Abort("Can't send video packet - file:" + file_path);
                }
                ret = avcodec_receive_frame(videoCodecCtx, videoFrame);
                if (ret == 0 || ret == AVERROR_EOF) {
                    frameFinished = 1;
                } else if (ret == AVERROR(EAGAIN)) {
                    fprintf(stderr, "Can't receive the video frame, try it again - file: %s\n", file_path.c_str());
                    continue;
                } else {
                    ErrH.Abort("Can't receive the video frame - file:" + file_path);
                }
                break;
            }
#elif LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 23, 0)
            avcodec_decode_video2(videoCodecCtx, videoFrame, &frameFinished, packet);
#else
            avcodec_decode_video(videoCodecCtx, videoFrame, &frameFinished, packet->data, packet->size);
#endif
            if (frameFinished) {
                type = AVWrapperType::Video;
            }
        } else if (packet->stream_index == audioStream) {
            //TODO
        }
        
        AV_PACKET_UNREF(packet);
    }
    av_packet_free(&packet);
    
    return type;
}

size_t AVWrapper::getVideoFrameBufferSize() const {
    if (videoFrame->format < 0) {
        ErrH.Abort("Frame has unknown pixel format - file:  " + file_path);
    }
    AVPixelFormat format = static_cast<AVPixelFormat>(videoFrame->format);
    return av_image_get_buffer_size(format, videoFrame->width, videoFrame->height, 1);
}

int AVWrapper::getVideoFrameBuffer(uint8_t** buffer) const {
    if (videoFrame->format < 0) {
        ErrH.Abort("Frame has unknown pixel format - file:  " + file_path);
    }
    size_t buffer_size = getVideoFrameBufferSize();
    AVPixelFormat format = static_cast<AVPixelFormat>(videoFrame->format);
    if (*buffer == nullptr) {
        *buffer = new uint8_t[buffer_size];
    }
    return av_image_copy_to_buffer(*buffer, static_cast<int>(buffer_size), videoFrame->data,
                                   videoFrame->linesize, format, videoFrame->width, videoFrame->height, 1);
}

#endif //PERIMETER_FFMPEG