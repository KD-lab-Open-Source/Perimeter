#ifdef PERIMETER_FFMPEG

#include <tweaks.h>
#include "xerrhand.h"
#include "AVWrapper.h"

static bool avwrapper_init = false;

AVWrapperFrame::AVWrapperFrame() {
}

AVWrapperFrame::~AVWrapperFrame() {
    if (frame) {
        av_frame_free(&frame);
    }
}

size_t AVWrapperFrame::getBufferSize() const {
    if (frame->format < 0) {
        ErrH.Abort("Frame has unknown format - file:  " + wrapper->file_path);
    }
    if (type == AVWrapperType::Video) {
        AVPixelFormat format = static_cast<AVPixelFormat>(frame->format);
        return av_image_get_buffer_size(format, frame->width, frame->height, 1);
    } else if (AVWrapperType::Audio) {
        AVSampleFormat format = static_cast<AVSampleFormat>(frame->format);
        int channels = 1;
        if (av_sample_fmt_is_planar(format)) {
            channels = frame->ch_layout.nb_channels;
        }
        return frame->linesize[0] * channels;
    } else {
        xassert(0);
        return 0;
    }
}

size_t AVWrapperFrame::copyBuffer(uint8_t** buffer) const {
    if (frame->format < 0) {
        ErrH.Abort("Frame has unknown format - file:  " + wrapper->file_path);
    }
    size_t buffer_size = getBufferSize();
    if (*buffer == nullptr) {
        *buffer = new uint8_t[buffer_size];
    }
    if (type == AVWrapperType::Video) {
        AVPixelFormat format = static_cast<AVPixelFormat>(frame->format);
        int written = av_image_copy_to_buffer(*buffer, static_cast<int>(buffer_size), frame->data,
                                              frame->linesize, format, frame->width, frame->height, 1);
        xassert(written == buffer_size);
        return buffer_size;
    } else if (AVWrapperType::Audio) {
        AVSampleFormat format = static_cast<AVSampleFormat>(frame->format);
        int channels = 1;
        if (av_sample_fmt_is_planar(format)) {
            channels = frame->ch_layout.nb_channels;
        };
        if (1 < channels) {
            //We need to pack planar audio
            size_t bytes_sample = av_get_bytes_per_sample(format);
            xassert(buffer_size == frame->linesize[0] * channels);
            //Store each plane sample as interleaved
            for (int si = 0; si < frame->nb_samples; ++si) {
                size_t srcoff = si * bytes_sample;
                xassert(srcoff + bytes_sample <= frame->linesize[0]);
                for (int pi = 0; pi < channels; pi++) {
                    size_t dstoff = (si * channels + pi) * bytes_sample;
                    xassert(dstoff + bytes_sample <= buffer_size);
                    memcpy(*buffer + dstoff, frame->data[pi] + srcoff, bytes_sample);
                }
            }
        } else {
            //Copy all buffer directly
            xassert(buffer_size == frame->linesize[0]);
            memcpy(*buffer, frame->data[0], buffer_size);
        }
        return buffer_size;
    } else {
        xassert(0);
        return 0;
    }
}

AVRational AVWrapperFrame::getTimeBase() const {
    return time_base;
}

double AVWrapperFrame::getPresentationTime() const {
    double pts = static_cast<double>(getPTS());
    if (0 < pts) pts *= av_q2d(time_base);
    return pts;
}

int64_t AVWrapperFrame::getPTS() const {
    if (frame->pts == AV_NOPTS_VALUE) {
        return -1;
    }
    return frame->pts;
}

void AVWrapper::init() {
    if (avwrapper_init) return;
    avwrapper_init = true;
}

int AVWrapper::close() {
    end = true;
    
    clearFrames();
    
#ifdef PERIMETER_FFMPEG_MOVIE
    // Close the scaler
    if (swsCtx) {
        sws_freeContext(swsCtx);
    }
    swsCtx = nullptr;
    swsWidth = 0;
    swsHeight = 0;
    swsFormat = AV_PIX_FMT_NONE;
    
    // Close the resampler
    if (filterGraph) {
        avfilter_graph_free(&filterGraph);
    }
    swrChannelLayout = {};
    swrSampleRate = 0;
    swrFormat = AV_SAMPLE_FMT_NONE;
#endif

    //Close packet
    av_packet_free(&streamPacket);
    
    // Close the codecs
    if (videoCodecCtx) avcodec_free_context(&videoCodecCtx);
    if (audioCodecCtx) avcodec_free_context(&audioCodecCtx);
    videoCodec = nullptr;
    audioCodec = nullptr;
    videoStream = -1;
    audioStream = -1;
    
    // Close the file stream
    if (formatCtx) avformat_close_input(&formatCtx);
    
    file_path.clear();

    return 0;
}

void AVWrapper::pullFilterAudio() {
#ifdef PERIMETER_FFMPEG_MOVIE
    if (!filterGraph) {
        return;
    }
    AVWrapperFrame* converted = nullptr;
    while (true) {
        converted = new AVWrapperFrame();
        converted->frame = AV_FRAME_ALLOC();
        int ret = av_buffersink_get_frame(buffersinkCtx, converted->frame);
        if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
            break;
        }
        if (ret < 0) {
            fprintf(stderr, "Error reading audio buffer from buffersink\n");
            break;
        }

        int nb_channels = swrChannelLayout.nb_channels;
        int bytes_per_sample = av_get_bytes_per_sample(swrFormat);
        int data_size = converted->frame->nb_samples * nb_channels * bytes_per_sample;
        converted->frame->linesize[0] = data_size;
        converted->type = AVWrapperType::Audio;
        converted->time_base = av_buffersink_get_time_base(buffersinkCtx);
        converted->wrapper = this;
        audioFrames.emplace_back(converted);
        //printf("Converted %f %" PRIi64 "\n", converted->getPresentationTime(), converted->frame->pts);
        converted = nullptr;
    }
    //Leftover
    if (converted) {
        delete converted;
        converted = nullptr;
    }
#endif
}

void AVWrapper::onEnd() {
    readPacket(true);
    pullFilterAudio();
}

int AVWrapper::open(const std::string& path, AVWrapperType need) {
    end = false;
    file_path = path;
    formatCtx = nullptr;
    audioNextPTS = AV_NOPTS_VALUE;

    // Open video file
    int ret = avformat_open_input(&formatCtx, file_path.c_str(), NULL, NULL);
    if (ret != 0) {
        char error_message[256];
        av_strerror(ret, error_message, 256);
        ErrH.Abort("Couldn't open video file: " + file_path + " " + error_message);
        return 1; // Couldn't open file
    }
    
    // Retrieve stream information
    if(avformat_find_stream_info(formatCtx, NULL) < 0) {
        ErrH.Abort("Couldn't find stream information - file: " + file_path);
        return 1; // Couldn't find stream information
    }
    
    // Find video and/or audio stream
    videoStream = av_find_best_stream(formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, const_cast<const AVCodec**>(&videoCodec), 0);
    audioStream = av_find_best_stream(formatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, const_cast<const AVCodec**>(&audioCodec), 0);

    //Attempt to load video codec
    if (videoStream < 0) {
        videoCodec = nullptr;
        if (videoStream == AVERROR_STREAM_NOT_FOUND) {
            fprintf(stderr, "Didn't find any video stream in file: %s\n", file_path.c_str());
        }
        if (need == AVWrapperType::Video && videoStream == AVERROR_DECODER_NOT_FOUND) {
            fprintf(stderr, "Didn't find codec for video stream in file: %s\n", file_path.c_str());
        }
    } else {
        // Get a pointer to the codec context for the video stream
#if AV_CODEC_PAR
        videoCodecCtx=avcodec_alloc_context3(nullptr);
        if (!videoCodecCtx) {
            ErrH.Abort("Unabled to allocate video codec context - file: " + file_path);
            return 1;
        }
        ret = avcodec_parameters_to_context(videoCodecCtx, formatCtx->streams[videoStream]->codecpar);
        if (ret != 0) {
            ErrH.Abort("Couldn't get the video codec context - file: " + file_path);
            return 1;
        }
#else
        videoCodecCtx=formatCtx->streams[videoStream]->codec;
#endif

        // Open the decoder for the video stream
        if (videoCodec == nullptr) {
            ErrH.Abort("Unsupported video codec! - file:" + file_path);
            return 1;
        }

#if (LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53, 8, 0))
        if (avcodec_open2(videoCodecCtx, videoCodec, nullptr) < 0) {
#else
        if (avcodec_open(videoCodecCtx, videoCodec) < 0) {
#endif
            ErrH.Abort("Could not open video codec - file:" + file_path);
            return 1;
        }
    }
        
    //Attempt to load audio codec
    if (audioStream < 0) {
        audioCodec = nullptr;
        if (audioStream == AVERROR_DECODER_NOT_FOUND) {
            fprintf(stderr, "Didn't find codec for audio stream in file: %s\n", file_path.c_str());
        }
        if (need == AVWrapperType::Audio && audioStream == AVERROR_STREAM_NOT_FOUND) {
            fprintf(stderr, "Didn't find any audio stream in file: %s\n", file_path.c_str());
        }
    } else {
        // Get a pointer to the codec context for the audio stream
#if AV_CODEC_PAR
        audioCodecCtx = avcodec_alloc_context3(nullptr);
        if (!audioCodecCtx) {
            ErrH.Abort("Unabled to allocate audio codec context - file: " + file_path);
            return 1;
        }
        ret = avcodec_parameters_to_context(audioCodecCtx, formatCtx->streams[audioStream]->codecpar);
        if (ret != 0) {
            ErrH.Abort("Couldn't get the audio codec context - file: " + file_path);
            return 1;
        }
#else
        audioCodecCtx=formatCtx->streams[audioStream]->codec;
#endif

        // Open the decoder for the audio stream
        if (audioCodec == nullptr) {
            ErrH.Abort("Unsupported audio codec! - file:" + file_path);
            return 1;
        }

#if (LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53, 8, 0))
        if (avcodec_open2(audioCodecCtx, audioCodec, nullptr) < 0) {
#else
        if (avcodec_open(audioCodecCtx, audioCodec) < 0) {
#endif
            ErrH.Abort("Could not open audio codec - file:" + file_path);
            return 1;
        }
    }

    streamPacket = av_packet_alloc();

    return 0;
}
    
bool AVWrapper::setupVideoScaler(int width, int height, AVPixelFormat format, int flags) {
    if (!videoCodecCtx) {
        //Nothing to setup since there is no codec
        return false;
    }
#ifdef PERIMETER_FFMPEG_MOVIE
    swsWidth = width != 0 ? width : videoCodecCtx->width;
    swsHeight = height != 0 ? height : videoCodecCtx->height;
    swsFormat = format != AV_PIX_FMT_NONE ? format : videoCodecCtx->pix_fmt;
    if (swsCtx) {
        sws_freeContext(swsCtx);
    }
    swsCtx = sws_getContext(
            //Source
            videoCodecCtx->width, videoCodecCtx->height, videoCodecCtx->pix_fmt,
            //Destination
            swsWidth, swsHeight, swsFormat,
            //Params
            flags, nullptr, nullptr, nullptr
    );
    return true;
#else
    return false;
#endif
}

#ifdef PERIMETER_FFMPEG_MOVIE
/** Sources:
 * Old: https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/filtering_audio.c
 * New: https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/filter_audio.c
 */
static int init_filter_graph(
    AVFilterGraph *filter_graph, AVFilterContext **abuffersrc_ctx_ptr, AVFilterContext **abuffersink_ctx_ptr,
    AVFilterInOut **inputs_ptr, AVFilterInOut **outputs_ptr,
    const char* src_args, const char* filter_descr
) {
    AVFilterContext* abuffersrc_ctx;
    const AVFilter* abuffersrc;
    AVFilterContext* abuffersink_ctx;
    const AVFilter* abuffersink;
    AVFilterInOut* inputs = *inputs_ptr;
    AVFilterInOut* outputs = *outputs_ptr;

    int err;
    
    if (!filter_graph || !inputs || !outputs) {
        fprintf(stderr, "init_filter_graph Not enough memory\n");
        return AVERROR(ENOMEM);
    }

    /* Create the abuffer filter;
     * it will be used for feeding the data into the graph. */
    abuffersrc = avfilter_get_by_name("abuffer");
    if (!abuffersrc) {
        fprintf(stderr, "init_filter_graph Could not find the abuffer filter.\n");
        return AVERROR_FILTER_NOT_FOUND;
    }

    abuffersrc_ctx = avfilter_graph_alloc_filter(filter_graph, abuffersrc, "src");
    if (!abuffersrc_ctx) {
        fprintf(stderr, "init_filter_graph Could not allocate the abuffer instance.\n");
        return AVERROR(ENOMEM);
    }

    /* Now initialize the filter */
    err = avfilter_init_str(abuffersrc_ctx, src_args);
    if (err < 0) {
        fprintf(stderr, "init_filter_graph Could not initialize the abuffer filter.\n");
        return err;
    }

    /* Finally create the abuffersink filter;
     * it will be used to get the filtered data out of the graph. */
    abuffersink = avfilter_get_by_name("abuffersink");
    if (!abuffersink) {
        fprintf(stderr, "init_filter_graph Could not find the abuffersink filter.\n");
        return AVERROR_FILTER_NOT_FOUND;
    }

    abuffersink_ctx = avfilter_graph_alloc_filter(filter_graph, abuffersink, "sink");
    if (!abuffersink_ctx) {
        fprintf(stderr, "init_filter_graph Could not allocate the abuffersink instance.\n");
        return AVERROR(ENOMEM);
    }

    /* This filter takes no options. */
    err = avfilter_init_str(abuffersink_ctx, nullptr);
    if (err < 0) {
        fprintf(stderr, "init_filter_graph Could not initialize the abuffersink instance.\n");
        return err;
    }
    
    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = abuffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = nullptr;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = abuffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = nullptr;
    
    if (!outputs->name || !inputs->name) {
        return AVERROR(ENOMEM);
    }

    /* Configure the graph. */
    if ((err = avfilter_graph_parse_ptr(filter_graph, filter_descr, inputs_ptr, outputs_ptr, nullptr)) < 0) {
        fprintf(stderr, "init_filter_graph Error occurred at avfilter_graph_parse_ptr\n");
        return err;
    }
    err = avfilter_graph_config(filter_graph, nullptr);
    if (err < 0) {
        fprintf(stderr, "init_filter_graph Error configuring the filter graph\n");
        return err;
    }

    *abuffersrc_ctx_ptr = abuffersrc_ctx;
    *abuffersink_ctx_ptr = abuffersink_ctx;

    return 0;
}
#endif

bool AVWrapper::setupAudioConverter(AVSampleFormat dst_format, int dst_channels, int dst_frequency) {
    if (!audioCodecCtx) {
        //Nothing to setup since there is no codec
        return false;
    }
    if (filterGraph) {
        avfilter_graph_free(&filterGraph);
    }
    filterGraph = nullptr;
    buffersrcCtx = nullptr;
    buffersinkCtx = nullptr;
#ifdef PERIMETER_FFMPEG_MOVIE
    swrFormat = dst_format != 0 ? dst_format : audioCodecCtx->sample_fmt;
    int num_channel = dst_channels != 0 ? dst_channels : audioCodecCtx->ch_layout.nb_channels;
    av_channel_layout_default(&swrChannelLayout, num_channel);
    swrSampleRate = dst_frequency != 0 ? dst_frequency : audioCodecCtx->sample_rate;
    if (swrFormat == AV_SAMPLE_FMT_NONE) {
        fprintf(stderr, "setupAudioConverter unsupported device output format %d at %s\n", dst_format, file_path.c_str());
        return false;
    }
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    const AVFilterLink *inlink;
    const AVFilterLink *outlink;
    AVRational time_base = audioCodecCtx->time_base;

    char inChannelLayout_str[128];
    char swrChannelLayout_str[128];
    av_channel_layout_describe(&audioCodecCtx->ch_layout, inChannelLayout_str, sizeof(inChannelLayout_str));
    av_channel_layout_describe(&swrChannelLayout, swrChannelLayout_str, sizeof(swrChannelLayout_str));

    std::string src_args = "time_base=" + std::to_string(time_base.num) + "/" + std::to_string(time_base.den) +
                       ":sample_rate=" + std::to_string(audioCodecCtx->sample_rate) +
                       ":sample_fmt=" + av_get_sample_fmt_name(audioCodecCtx->sample_fmt) +
                       ":channel_layout=" + inChannelLayout_str;
    std::string filters_descr;

    //Add aresample filter if sample rates mismatch
    if (audioCodecCtx->sample_rate != swrSampleRate) {
        if (!filters_descr.empty()) filters_descr += ",";
        filters_descr += "aresample=" + std::to_string(swrSampleRate);
        //Soft compensation
        filters_descr += ":min_comp=0.01";
        filters_descr += ":min_hard_comp=0.5";
        filters_descr += ":async=" + std::to_string(swrSampleRate / 2);
    } 

    int ret = 0;

    //Convert into SDL audio format
    if (!filters_descr.empty()) filters_descr += ",";
    const char* swrFormatName = av_get_sample_fmt_name(swrFormat);
    if (!swrFormatName) {
        fprintf(stderr, "setupAudioConverter not enough memory\n");
        goto end;
    }
    filters_descr += std::string("aformat=sample_fmts=") + swrFormatName + ":channel_layouts=" + swrChannelLayout_str;
    
    fprintf(
            stdout, "AVWrapper audio conv:\n- Input Args: %s\n- Filter args: %s\n",
            src_args.c_str(), //Input Args
            filters_descr.c_str() //Filter args
    );
    
    /* Set up the filtergraph. */
    filterGraph = avfilter_graph_alloc();
    ret = init_filter_graph(
        filterGraph,
        &buffersrcCtx, &buffersinkCtx,
        &inputs, &outputs,
        src_args.c_str(), filters_descr.c_str()
    );
    if (ret < 0 || !buffersrcCtx || !buffersinkCtx) {
        fprintf(stderr, "setupAudioConverter Unable to init filter graph: %d\n", ret);
        goto end;
    }

    /* Print summary of the sink buffer */
    inlink = buffersrcCtx->outputs[0];
    outlink = buffersinkCtx->inputs[0];
    av_channel_layout_describe(&inlink->ch_layout, inChannelLayout_str, sizeof(inChannelLayout_str));
    av_channel_layout_describe(&outlink->ch_layout, swrChannelLayout_str, sizeof(swrChannelLayout_str));
    fprintf(
            stdout, "- Input link: %dHz %s %s\n- Output link: %dHz %s %s\n",
            //Input link
            inlink->sample_rate,
            static_cast<char*>(av_x_if_null(av_get_sample_fmt_name(static_cast<AVSampleFormat>(inlink->format)), "?")),
            inChannelLayout_str,
            //Output link
            outlink->sample_rate,
            static_cast<char*>(av_x_if_null(av_get_sample_fmt_name(static_cast<AVSampleFormat>(outlink->format)), "?")),
            swrChannelLayout_str
    );
    
    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    if (0 > ret) {
        if (filterGraph) {
            avfilter_graph_free(&filterGraph);
        }
        fprintf(stderr, "setupAudioConverter Error occurred when setting up audio conv\n");
        xassert(0);
        return false;
    } else {
        return true;
    }
#else
    return false;
#endif
}

void AVWrapper::readPacket(bool drain) {    
    int ret = 0;
    AVPacket* packet = nullptr;
    if (!drain) {
        packet = streamPacket;
        //Attempt to get a packet
        ret = av_read_frame(formatCtx, packet);
        if (0 <= ret) {
            end = false;
        }
    }
    if (0 <= ret) {
        if (0 <= videoStream && (drain || packet->stream_index == videoStream)) {
            // Extract video frame
            ret = avcodec_send_packet(videoCodecCtx, packet);
            if (ret != 0) {
                if (ret == AVERROR_EOF) {
                    fprintf(stderr, "Already reached video EOF - file: %s\n", file_path.c_str());
                } else if (ret != AVERROR(EAGAIN)) {
                    fprintf(stderr, "Can't send video packet %d - file: %s\n", ret, file_path.c_str());
                } else {
                    ret = 0;
                }
            }
            while (ret == 0) {
                AVWrapperFrame* frame = new AVWrapperFrame();
                frame->type = AVWrapperType::Video;
                frame->frame = AV_FRAME_ALLOC();
                ret = avcodec_receive_frame(videoCodecCtx, frame->frame);
                if (ret == 0) {
                    handleFrame(frame);
                    ret = 0;
                    continue;
                } else if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                    fprintf(stderr, "Can't receive the video frame - file: %s\n", file_path.c_str());
                }
                delete frame;
                break;
            }
        }
        
        if (0 <= audioStream && (drain || packet->stream_index == audioStream)) {
            // Extract audio frame
            ret = avcodec_send_packet(audioCodecCtx, packet);
            if (ret != 0) {
                if (ret == AVERROR_EOF) {
                    fprintf(stderr, "Already reached audio EOF - file: %s\n", file_path.c_str());
                } else if (ret != AVERROR(EAGAIN)) {
                    fprintf(stderr, "Can't send audio packet %d - file: %s\n", ret, file_path.c_str());
                } else {
                    ret = 0;
                }
            }
            while (true) {
                AVWrapperFrame* frame = new AVWrapperFrame();
                frame->type = AVWrapperType::Audio;
                frame->frame = AV_FRAME_ALLOC();
                ret = avcodec_receive_frame(audioCodecCtx, frame->frame);
                if (ret == 0) {
                    handleFrame(frame);
                    ret = 0;
                    continue;
                } else if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                    fprintf(stderr, "Can't receive the audio frame - file: %s\n", file_path.c_str());
                }
                delete frame;
                break;
            }
        }
        
        if (!drain) {
            AV_PACKET_UNREF(packet);
        }
    } else {
        if (!end) {
            onEnd();
        }
        end = true;
    }
        
    //pull filtered audio from the filtergraph
    pullFilterAudio();
}
    
void AVWrapper::handleFrame(AVWrapperFrame* frame) {
    frame->wrapper = this;
    
    switch (frame->type) {
        case AVWrapperType::Video: {
            frame->time_base = formatCtx->streams[videoStream]->time_base;
            frame->frame->pts = frame->frame->best_effort_timestamp;

            if (frame->frame->pts == AV_NOPTS_VALUE) {
                fprintf(stderr, "Video frame without PTS\n");
            }

#ifdef PERIMETER_FFMPEG_MOVIE
            //Do video scaling if scaler is setup
            if (swsCtx) {
                AVWrapperFrame original;
                original.frame = frame->frame;
                frame->frame = AV_FRAME_ALLOC();
                frame->frame->pts = original.frame->pts;
                frame->frame->width = swsWidth;
                frame->frame->height = swsHeight;
                frame->frame->format = swsFormat;
                av_frame_make_writable(frame->frame);
                int ret = av_frame_get_buffer(frame->frame, 1);
                if (ret < 0) {
                    ErrH.Abort("Can't alloc sws image - file: " + file_path);
                }
                sws_scale(
                        swsCtx,
                        original.frame->data, original.frame->linesize,
                        0, videoCodecCtx->height,
                        frame->frame->data, frame->frame->linesize
                );
            }
#endif

            videoFrames.emplace_back(frame);
            break;
        }
        case AVWrapperType::Audio: {
            frame->time_base = formatCtx->streams[audioStream]->time_base;
            AVRational tb = av_make_q(1, frame->frame->sample_rate);
            if (frame->frame->pts != AV_NOPTS_VALUE) {
                frame->frame->pts = av_rescale_q(frame->frame->pts, frame->time_base, tb);
            } else if (audioNextPTS != AV_NOPTS_VALUE) {
                frame->frame->pts = av_rescale_q(audioNextPTS, frame->time_base, tb);
            }
            
            if (frame->frame->pts != AV_NOPTS_VALUE) {
                audioNextPTS = frame->frame->pts + frame->frame->nb_samples;
            } else {
                fprintf(stderr, "Audio frame without PTS\n");
            }
            
            //Do audio conversion
#ifdef PERIMETER_FFMPEG_MOVIE
            if (filterGraph) {
                //printf("Audio %f %" PRIi64 " %" PRIi64 "\n", frame->getPresentationTime(), frame->frame->pts, packet->pts);
                // push the audio data from decoded frame into the filtergraph
                int ret = av_buffersrc_write_frame(buffersrcCtx, frame->frame);
                if (ret < 0) {
                    fprintf(stderr, "Error writing audio frame to buffersrc\n");
                }
                delete frame;
                frame = nullptr;
            }
#endif

            //Put frame into list
            if (frame) {
                audioFrames.emplace_back(frame);
            }
            break;
        }
        default:
            delete frame;
            break;
    }
}

void AVWrapper::clearFrames() {
    // Clear list
    for (auto& frame : videoFrames) {
        delete frame;
    }
    videoFrames.clear();
    for (auto& frame : audioFrames) {
        delete frame;
    }
    audioFrames.clear();
}

int AVWrapper::getVideoCodecWidth() const {
    return videoCodecCtx->width;
}

int AVWrapper::getVideoCodecHeight() const {
    return videoCodecCtx->height;
}

int AVWrapper::getVideoWidth() const {
    return swsCtx ? swsWidth : videoCodecCtx->width;
}

int AVWrapper::getVideoHeight() const {
    return swsCtx ? swsHeight : videoCodecCtx->height;
}

int AVWrapper::getVideoBPP() const {
    return videoCodecCtx->bits_per_coded_sample;
}

double AVWrapper::getDuration() const {
    return static_cast<double>(formatCtx->duration) / static_cast<double>(AV_TIME_BASE);
}

SDL_AudioFormat AVWrapper::toSDLAudioFormat(AVSampleFormat fmt) {
    switch (fmt) {
        case AV_SAMPLE_FMT_U8:
            return AUDIO_U8;
        case AV_SAMPLE_FMT_S16:
            return AUDIO_S16;
        case AV_SAMPLE_FMT_S32:
            return AUDIO_S32;
        case AV_SAMPLE_FMT_FLT:
            return AUDIO_F32;
        case AV_SAMPLE_FMT_U8P:
            return AUDIO_U8;
        case AV_SAMPLE_FMT_S16P:
            return AUDIO_S16;
        case AV_SAMPLE_FMT_S32P:
            return AUDIO_S32;
        case AV_SAMPLE_FMT_FLTP:
            return AUDIO_F32;
        default:
        case AV_SAMPLE_FMT_DBL:
        case AV_SAMPLE_FMT_DBLP:
        case AV_SAMPLE_FMT_S64:
        case AV_SAMPLE_FMT_S64P:
        case AV_SAMPLE_FMT_NONE:
            return 0;
    }
}

AVSampleFormat AVWrapper::fromSDLAudioFormat(SDL_AudioFormat fmt, bool planar) {
    AVSampleFormat result = AV_SAMPLE_FMT_NONE;
    switch (fmt) {
        case AUDIO_U8:
            result = planar ? AV_SAMPLE_FMT_U8P : AV_SAMPLE_FMT_U8;
            break;
        case AUDIO_S16:
            result = planar ? AV_SAMPLE_FMT_S16P : AV_SAMPLE_FMT_S16;
            break;
        case AUDIO_S32:
            result = planar ? AV_SAMPLE_FMT_S32P : AV_SAMPLE_FMT_S32;
            break;
        case AUDIO_F32:
            result = planar ? AV_SAMPLE_FMT_FLTP : AV_SAMPLE_FMT_FLT;
            break;
        default:
            break;
    }
    return result;
}

#endif //PERIMETER_FFMPEG