#include <thread>
#include <SDL_mixer.h>
#include "StdAfx.h"
#include "SystemUtil.h"
#include "files/files.h"
#include "Runtime.h"
#include "Sample.h"
#include "VideoPlayer.h"
#include "PerimeterSound.h"

#ifdef PERIMETER_FFMPEG_MOVIE
#include "AVWrapper.h"

extern bool terEnableGDIPixel;

const int VIDEO_DECODE_AHEAD = 30; //Video frames
const double AUDIO_DECODE_AHEAD = 0.5; //Secs
const double AUDIO_BUFFER_SIZE = 4.0; //Secs
const double TIME_MAX_STEP = 1.0/10.0; //Secs
const double TIME_TOO_OLD_FRAMES = 1.5; //Secs
const double TIME_VIDEO_AHEAD = 0.25; //Secs
const double TIME_AUDIO_AHEAD = 0.25; //Secs
const double AUDIO_MIN_SILENCE = 0.005; //Secs

static void sleep_us(int64_t microseconds) {
  static int64_t total_sleep=0;
  static int64_t slept_in_advance=0;

  double frequency = static_cast<double>(getPerformanceFrequency()) / (1000000); 

  total_sleep += microseconds;

  // Have we exceeded our reserve of slept microseconds?
  if (( total_sleep - slept_in_advance ) > 1000)
  {
    uint64_t start, end;
    total_sleep -= slept_in_advance;

    // Do the timed sleep.
    start = getPerformanceCounter();
    std::this_thread::sleep_for(std::chrono::microseconds(total_sleep));
    end = getPerformanceCounter();

    // Calculate delta time in microseconds.
    double elapsed = static_cast<double>(end - start) / frequency;

    // Keep track of how much extra we slept.
    slept_in_advance = static_cast<int64_t>(elapsed) - total_sleep;
    total_sleep %= 1000;
  }
}

AudioBuffer::AudioBuffer(double seconds) : XBuffer(0, false) {
    size_t len = 1024;
    //Len might return 0 if audio device is not inited 
    if (terAudioEnable) {
        while (SNDcomputeAudioLengthS(len) < seconds) {
            len *= 2;
        }
    }
    realloc(len);
    clear();
}

void AudioBuffer::writeAudio(const uint8_t* inbuf, size_t len) {
    std::lock_guard<std::mutex> lock(mutex);
    if (len > size - written) {
        fprintf(stderr, "Video audio buffer overflow\n");
        return;
    }
    size_t first = std::min(len, size - offset);
    if (inbuf) {
        memcpy(buf + offset, inbuf, first);
        memcpy(buf, inbuf + first, len - first);
    } else {
        memset(buf + offset, SNDDeviceFormat() == AUDIO_U8 ? 128 : 0, first);
        memset(buf, SNDDeviceFormat() == AUDIO_U8 ? 128 : 0, len - first);
    }
    offset = (offset + len) % size;
    written += len;
    write_counter += len;
}

size_t AudioBuffer::readAudio(uint8_t* outbuf, size_t len) {
    std::lock_guard<std::mutex> lock(mutex);
    len = std::min(len, written);
    size_t first = std::min(len, size - head);
    memcpy(outbuf, buf + head, first);
    memcpy(outbuf + first, buf, len - first);
    head = (head + len) % size;
    written -= len;
    read_counter += len;
    return len;
}

void AudioBuffer::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    memset(buf, 0, size);
    head = 0;
    written = 0;
    offset = 0;
    read_counter = 0;
    write_counter = 0;
}

VideoPlayer::VideoPlayer() {
    AVWrapper::init();
    wrapper = new AVWrapper();
    audioBuffer = new AudioBuffer(AUDIO_BUFFER_SIZE);
    
    //Setup sample if audio is enabled
    if (terAudioEnable) {
        sample = new SND_Sample(nullptr);
        sample->channel_group = SND_GROUP_SPEECH;
        sample->global_volume_select = GLOBAL_VOLUME_IGNORE;
        sample->steal_channel = true; //Just in case another speech or audio is playing
    }    
}

VideoPlayer::~VideoPlayer() {
	Done();
    
    if (sample) {
        delete sample;
        sample = nullptr;
    }

    if (audioBuffer) {
        delete audioBuffer;
        audioBuffer = nullptr;
    }

    if (wrapper) {
        delete wrapper;
        wrapper = nullptr;
    }
}

void VideoPlayer::getSize(Vect2i& vec) {
    vec.set(wrapper->getVideoWidth(), wrapper->getVideoHeight());
}

bool VideoPlayer::Init(const char* path) {
	Done();

    paused = focusPaused = audioPlaybackFailed = false;
    lastUpdateTime = 0;
    audioBufferTime = currentTime = 0;
    
    std::string path_str = convert_path_content(path);
    if (path_str.empty()) {
        fprintf(stderr,  "Video file not found %s\n", path);
        //xassert(0);
        return false;
    }

    int ret = wrapper->open(path_str, AVWrapperType::Video);
    if (ret) {
        fprintf(stderr,  "Cannot open video file %s\n", path);
		xassert(0);
		return false;
	}

    IniManager cfg("Perimeter.ini", false);
    int scalerWidth = 1280;
    int scalerHeight = 720;
    cfg.getInt("Graphics", "MaxVideoResolutionWidth", scalerWidth);
    cfg.getInt("Graphics", "MaxVideoResolutionHeight", scalerHeight);
    if (!wrapper->setupVideoScaler(
        min(scalerWidth, wrapper->getVideoCodecWidth()),
        min(scalerHeight, wrapper->getVideoCodecHeight()),
        gb_RenderDevice->GetRenderSelection() == DEVICE_D3D9 ? AVPixelFormat::AV_PIX_FMT_BGRA : AVPixelFormat::AV_PIX_FMT_RGBA,
        SWS_BILINEAR
    )) {
        Done();
        return false;
    }
    
	int dx_p2=Power2up(wrapper->getVideoWidth());
	int dy_p2=Power2up(wrapper->getVideoHeight());

	RELEASE(pTexture);
    pTexture=gb_VisGeneric->CreateTexture(dx_p2,dy_p2,false);

    //Only if there is audio output
    if (sample) {
        //Tell wrapper to output audio in device format
        AVSampleFormat format = AVWrapper::fromSDLAudioFormat(SNDDeviceFormat(), false);
        if (wrapper->audioStream >= 0 &&
            !wrapper->setupAudioConverter(format, SNDDeviceChannels(), SNDDeviceFrequency())) {
            Done();
            return false;
        }
    
        //Load a silent sound buffer, so we can get a looping sample that will poll our buffer
        //This is a workaround since SDL_mixer doesn't have a way to stream audio continuously
        //But Channel effects allows us to achieve a similar effect by passing audio data each time channel
        //audio is processed by SDL_mixer
        sample->looped = true;
        const int samples = SNDDeviceFrequency() / 10;
        size_t buf_len = SNDformatSampleSize(SNDDeviceFormat()) * SNDDeviceChannels() * samples;
        void* buf = SDL_malloc(buf_len);
        if (!buf) {
            Done();
            return false;
        }
        SDL_memset(buf, SNDDeviceFormat() == AUDIO_U8 ? 128 : 0, buf_len);
        sample->loadRawData(static_cast<uint8_t*>(buf), buf_len, false, path_str);
    }
    
#if 0 && defined(PERIMETER_DEBUG)
    while (!wrapper->end) {
        wrapper->readPacket();
    }
#endif
    
	return true;
}

void VideoPlayer::Done() {
    if (sample) {
        sample->stop();
    }
    if (audioBuffer) {
        audioBuffer->clear();
    }
    wrapper->close();
	RELEASE(pTexture);
}

bool VideoPlayer::Update() {
    if (IsEnd()) {
        return false;
    }
    
    return InternalUpdate();
}

void VideoPlayer::Draw(int x, int y, int dx, int dy, int alpha) {
    terRenderDevice->DrawSprite(x, y, dx, dy, 0, 0,
                                static_cast<float>(wrapper->getVideoWidth()) / static_cast<float>(pTexture->GetWidth()),
                                static_cast<float>(wrapper->getVideoHeight()) / static_cast<float>(pTexture->GetHeight()),
                                pTexture,
                                sColor4c(255,255,255,alpha)
    );
}

bool VideoPlayer::IsEnd() {
    return wrapper->end && wrapper->videoFrames.empty()
        && wrapper->audioFrames.empty() && audioBuffer->buffered() == 0;
}

void VideoPlayer::SetVolume(float vol) {
    if (!sample) return;
    vol=clamp(vol,0.0f,2.0f);
    sample->volume = vol;
    sample->updateEffects();
}

void VideoPlayer::setPause(bool state) {
    if (paused == state) return;
    
    if (state) {
        lastUpdateTime = 0;
        if (sample && sample->isPlaying()) {
            sample->stop();
        }
    } else if (!focusPaused) {
        startAudioPlayer();
    }

    paused = state;
}

void VideoPlayer::startAudioPlayer() {
    if (!sample || audioPlaybackFailed || sample->isPlaying() || !audioBuffer->buffered()) return;
    int channel = sample->play();
    if (channel == SND_NO_CHANNEL ||
        !Mix_RegisterEffect(channel, VideoPlayer::channelBufferEffect, nullptr, audioBuffer)) {
        fprintf(stderr, "Video audio unavailable: %s\n", Mix_GetError());
        sample->stop();
        audioPlaybackFailed = true;
        audioBuffer->clear();
    }
}

double VideoPlayer::getPlayerTime() const {
    return currentTime;
}

bool VideoPlayer::InternalUpdate() {
    bool lostFocus = !applicationHasFocus();
    if (focusPaused != lostFocus) {
        focusPaused = lostFocus;
        lastUpdateTime = 0;
        if (focusPaused && sample) sample->stop();
        else if (!paused) startAudioPlayer();
    }
    if (paused || focusPaused) {
        Sleep(10);
        return false;
    }

    //printf("Buffered audio %f\n", SNDcomputeAudioLengthS(audioBuffer->buffered()));

    //Decode frames
    decodeFrames();
    
    double audioTime = SNDcomputeAudioLengthS(audioBuffer->consumed());

    //Update current time by update time
    if (lastUpdateTime) {
        double elapsed = static_cast<double>(getPerformanceCounter() - lastUpdateTime);
        elapsed /= static_cast<double>(getPerformanceFrequency());
        elapsed = std::min(TIME_MAX_STEP, elapsed);
        // Follow the consumed audio while it is playing; retain wall time for
        // silent videos or an exhausted audio track. Limit correction to avoid jumps.
        if (sample && sample->isPlaying() && audioBuffer->buffered()) {
            double drift = audioTime - currentTime;
            elapsed *= drift > 0.05 ? 1.25 : (drift < -0.05 ? 0.75 : 1.0);
        }
        currentTime += elapsed;
    }
    //printf("Time: %f %f\n", currentTime, audioTime);

    lastUpdateTime = getPerformanceCounter();
    
    bool updatedTexture = false;
    
    //Check if we have video frames in our time
    AVWrapperFrame* videoFrame = popCurrentFrame(wrapper->videoFrames);
    if (videoFrame) {
        WriteVideoFrame(videoFrame);
        delete videoFrame;
        updatedTexture = true;
    }

    if (0 < audioBuffer->buffered() && sample && !sample->isPlaying()) {
        startAudioPlayer();
    }
        
    if (!updatedTexture) {
        sleep_us(250);
    }

    return updatedTexture;
}

void VideoPlayer::WriteVideoFrame(AVWrapperFrame* frame) {
    //Lock texture
    int pitch=0;
    static Vect2i size;
    getSize(size);
    uint8_t* ptr = static_cast<uint8_t*>(gb_VisGeneric->GetRenderDevice()->LockTextureRect(
            pTexture,
            pitch,
            Vect2i::ZERO,
            size
    ));

    if (!ptr || pitch < size.x * 4) {
        ErrH.Abort("Cannot lock video texture");
    }
    //Dump frame into texture
    frame->copyBuffer(&ptr);

    //Fix pitch, since texture itself has extra padding we need to relocate the scanlines to corresponding position
    int bufferPitch = size.x * 4;
    if (bufferPitch < pitch) {
        for (int y = size.y - 1; 0 <= y; y--) {
            memmove(
                    ptr + pitch * y,
                    ptr + bufferPitch * y,
                    bufferPitch
            );
            //Avoid texture bleeding due to old relocated image data
            memset(
                ptr + pitch * y + bufferPitch,
                0x0,
                pitch - bufferPitch
            );
        }
    }

    //Unlock
    pTexture->UnlockTexture();
}

void VideoPlayer::WriteAudioFrame(AVWrapperFrame* frame) {
    if (!sample || audioPlaybackFailed) return;
    
    size_t buf_size;
    uint8_t* buf = nullptr;
    double pts = frame->getPresentationTime();
    if (pts + TIME_TOO_OLD_FRAMES < this->getPlayerTime()) {
        if (0 < pts) {
            fprintf(stderr, "Too old audio frame: %f\n", pts);
        }
        return;
    }
    double distance = 0;
    if (0 < audioBufferTime) {
        distance = pts - audioBufferTime;
    }
    if (AUDIO_MIN_SILENCE < distance) {
        //We need to add empty buffer for silence
        double timebase = av_q2d(frame->getTimeBase());
        buf_size = 0 < timebase ? static_cast<size_t>(distance / timebase) : 0;
        buf_size *= SNDDeviceChannels() * SNDformatSampleSize(SNDDeviceFormat());
        if (0 < buf_size) {
            printf("Silence audio %f\n", SNDcomputeAudioLengthS(buf_size));
            audioBuffer->writeAudio(nullptr, buf_size);
        }
    }
    buf_size = frame->copyBuffer(&buf);
    audioBuffer->writeAudio(buf, buf_size);
    if (0 < frame->getPTS()) {
        audioBufferTime = pts + SNDcomputeAudioLengthS(buf_size);
    }
    delete[] buf;
}

AVWrapperFrame* VideoPlayer::popCurrentFrame(std::list<AVWrapperFrame*>& frames) const {
    std::vector<AVWrapperFrame*> toremove;
    AVWrapperFrame* oldest = nullptr;
    auto it = frames.begin();
    while (it != frames.end()) {
        AVWrapperFrame* frame = *it;
        double pts = frame->getPresentationTime();
        
        //Remove too old frames
        if (pts + TIME_TOO_OLD_FRAMES < this->getPlayerTime()) {
            if (0 < pts) {
                fprintf(stderr, "Too old frame: %f\n", pts);
            }
            it = frames.erase(it);
            delete frame;
            continue;
        }

        //Get the oldest frame
        bool present = pts <= this->getPlayerTime();
        if (present) {
            if (oldest == nullptr || pts < oldest->getPresentationTime()) {
                oldest = frame;
            }
            if (pts == 0) {
                //Immediate presentation
                break;
            }
        } else {
            break;
        }
        
        it++;
    }
     
    it = std::find(frames.begin(), frames.end(), oldest);
    if (it != frames.end()) {
        frames.erase(it);
    }

    return oldest;
}

void VideoPlayer::decodeFrames() {
    //Get enough video frames
    while (!wrapper->end && wrapper->videoFrames.size() < VIDEO_DECODE_AHEAD) {
        wrapper->readPacket();
    }

    //Sort them
    wrapper->videoFrames.sort(AVWrapperFrame_compare);
    wrapper->audioFrames.sort(AVWrapperFrame_compare);

    //Write audio frames into audio buffer, the SDL mixer channel will read it from another thread
    size_t write_old = audioBuffer->write_counter;
    while (!wrapper->audioFrames.empty()) {
        //Check if we wrote enough in this loop or buffer is already full enough
        size_t write_loop = audioBuffer->write_counter - write_old;
        //printf("Audio frame into buffer %" PRIsize " %" PRIsize "\n", write_loop, audioBuffer->buffered());
        if (AUDIO_DECODE_AHEAD < SNDcomputeAudioLengthS(std::max(write_loop, audioBuffer->buffered()))) {
            break;
        }
        AVWrapperFrame* audioFrame = wrapper->audioFrames.front();
        wrapper->audioFrames.pop_front();
        WriteAudioFrame(audioFrame);
        delete audioFrame;
    }
    //printf("decodedFrames %" PRIsize " %f\n", wrapper->videoFrames.size(), SNDcomputeAudioLengthS(audioBuffer->buffered()));
}

void VideoPlayer::channelBufferEffect(int _channel, void *stream, int len, void *udata) {
    AudioBuffer* audioBuffer = static_cast<AudioBuffer*>(udata);
    memset(stream, SNDDeviceFormat() == AUDIO_U8 ? 128 : 0, len);
    size_t need = static_cast<size_t>(len);

#ifdef PERIMETER_DEBUG
    //printf("CBE %" PRIsize " %f -> %" PRIsize " %f\n", audioBuffer->buffered(), SNDcomputeAudioLengthS(audioBuffer->buffered()), need, SNDcomputeAudioLengthS(need));
#endif

    audioBuffer->readAudio(static_cast<uint8_t*>(stream), need);
}

#else

//Dummy video player

VideoPlayer::VideoPlayer() = default;
VideoPlayer::~VideoPlayer() = default;

bool VideoPlayer::Init(const char* path) { return true; }
void VideoPlayer::Draw(int x,int y,int dx,int dy, int alpha) {}
bool VideoPlayer::Update() { return false; }
bool VideoPlayer::IsEnd() { return true; }
void VideoPlayer::getSize(Vect2i& vec) { vec.set(10, 10); }
void VideoPlayer::SetVolume(float vol) {}

#endif
