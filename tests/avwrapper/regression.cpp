#include "AVWrapper.h"
#include "xerrhand.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

XErrorHandler ErrH;
XErrorHandler::XErrorHandler() = default;
XErrorHandler::~XErrorHandler() = default;
void XErrorHandler::Abort(const std::string& s, int, int, const char*) { std::fprintf(stderr, "%s\n", s.c_str()); std::exit(2); }
void XErrorHandler::Abort(const char* s, int, int, const char*) { std::fprintf(stderr, "%s\n", s); std::exit(2); }
void ErrH_RTC(const char*, unsigned, const char*) { std::exit(3); }
static void check(bool ok, const char* what) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", what); std::exit(1); }
}

// Expose fixture setup only; copying and sizing use the production implementation.
struct AudioFixture : AVWrapperFrame {
    AudioFixture(AVSampleFormat format, int channels) {
        type = AVWrapperType::Audio;
        frame = av_frame_alloc();
        frame->format = format;
        frame->nb_samples = 7; // Deliberately shorter than an aligned FFmpeg plane.
        av_channel_layout_default(&frame->ch_layout, channels);
        check(av_frame_get_buffer(frame, 32) == 0, "audio allocation");
        if (av_sample_fmt_is_planar(format)) {
            for (int c = 0; c < channels; ++c)
                for (int s = 0; s < 7; ++s)
                    reinterpret_cast<int16_t*>(frame->extended_data[c])[s] = s * channels + c;
        } else {
            for (int i = 0; i < 7 * channels; ++i)
                reinterpret_cast<int16_t*>(frame->data[0])[i] = i;
        }
    }
};
int main(int argc, char** argv) {
    AVWrapper::init();
    for (auto format : {AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P}) {
        for (int channels : {1, 2, 10}) {
            AudioFixture f(format, channels);
            uint8_t* data = nullptr;
            check(f.getBufferSize() == 7u * channels * 2, "no alignment bytes in audio size");
            check(f.copyBuffer(&data) == 7u * channels * 2, "audio copied size");
            for (int i = 0; i < 7 * channels; ++i)
                check(reinterpret_cast<int16_t*>(data)[i] == i, "interleaved samples");
            delete[] data;
        }
    }
    check(argc == 2, "expected generated fixture path");
    AVWrapper w;
    check(w.open(argv[1], AVWrapperType::Video) == 0, "open fixture");
    check(w.setupVideoScaler(65, 33, AV_PIX_FMT_BGRA, SWS_BILINEAR), "odd-sized scaler");
    check(w.setupAudioConverter(AV_SAMPLE_FMT_S16, 2, 48000), "resampler");
    unsigned frames = 0;
    size_t audioBytes = 0;
    double videoPTS = -1, audioPTS = -1;
    while (!w.end) {
        w.readPacket();
        for (auto* f : w.videoFrames) {
            check(f->getPresentationTime() >= videoPTS, "video PTS monotonic");
            videoPTS = f->getPresentationTime();
            uint8_t* data = nullptr;
            check(f->copyBuffer(&data) == 65 * 33 * 4, "full odd-sized frame");
            check(data[(65 * 33 - 1) * 4 + 3] == 255, "last pixel written");
            delete[] data;
            ++frames;
        }
        for (auto* f : w.audioFrames) {
            check(f->getPresentationTime() >= audioPTS, "audio PTS monotonic");
            audioPTS = f->getPresentationTime();
            uint8_t* data = nullptr;
            audioBytes += f->copyBuffer(&data);
            delete[] data;
        }
        w.clearFrames();
    }
    check(frames == 30, "all 30 video frames including EOF");
    check(audioBytes == 48000 * 2 * 2, "one second resampled audio including filter tail");
    std::printf("PASS: packed/planar audio, 10 channels, odd dimensions, timestamps, EOF (%u frames, %zu audio bytes)\n", frames, audioBytes);
}
