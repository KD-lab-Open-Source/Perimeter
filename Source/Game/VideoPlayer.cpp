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
    if (terMusicEnable | terSoundEnable) {
        while (SNDcomputeAudioLengthS(len) < seconds) {
            len *= 2;
        }
    }
    realloc(len);
    clear();
}

void AudioBuffer::updateAvailable() {
    if (head <= offset) {
        written = offset - head;
    } else {
        written = size - head + offset;
    }
}

void AudioBuffer::writeAudio(const uint8_t* inbuf, size_t len) {
    SDL_LockAudio();

    //printf("writeAudio %f\n", SNDcomputeAudioLengthS(len));
    
    if (offset + len > size) {
        //Write the len until reaches end and then continue from start
        size_t first = size - offset; 
        size_t second = len - first;
        if (second > offset) {
            fprintf(stderr, "Audio buffer second part overflow %ld %ld > %ld\n", first, second, offset);
            second = offset;
            xassert(0);
        }
        if (inbuf) {
            memcpy(buf + offset, inbuf, first);
            memcpy(buf, inbuf + first, second);
        }
        bool head_ahead = head > offset;
        offset = second;
        if (head_ahead && head < offset) {
            fprintf(stderr, "Audio buffer overflow %ld %ld\n", head, offset);
        }
    } else {
        if (inbuf) {
            memcpy(buf + offset, inbuf, len);
        }
        bool head_ahead = head > offset;
        offset += len;
        if (head_ahead && head < offset) {
            fprintf(stderr, "Audio buffer overflow %ld %ld\n", head, offset);
        }
    }

    updateAvailable();
    
    write_counter += len;
    
    SDL_UnlockAudio();
}

size_t AudioBuffer::readAudio(uint8_t* outbuf, size_t len) {
    //We assume this is called from audio thread and doesn't need locking as it already does beforehand
    
    len = std::min(len, written);
    if (0 < len) {
        if (head + len > size) {
            size_t first = size - head;
            size_t second = len - first;
            memcpy(outbuf, buf + head, first);
            memcpy(outbuf + first, buf, second);
            head = second;
        } else {
            memcpy(outbuf, buf + head, len);
            head += len;
        }

        updateAvailable();
        read_counter += len;
    }
    
    return len;
}

void AudioBuffer::clear() {
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
    if (terMusicEnable | terSoundEnable) {
        sample = new SND_Sample(nullptr);
        sample->channel_group = SND_GROUP_SPEECH;
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
    
    wrapper->setupVideoScaler(
        wrapper->getVideoWidth(),
        wrapper->getVideoHeight(),
        gb_RenderDevice->GetRenderSelection() == DEVICE_D3D9 ? AVPixelFormat::AV_PIX_FMT_BGRA : AVPixelFormat::AV_PIX_FMT_RGBA,
        SWS_BILINEAR
    );
    
	int dx_p2=Power2up(wrapper->getVideoWidth());
	int dy_p2=Power2up(wrapper->getVideoHeight());

	RELEASE(pTexture);
    pTexture=gb_VisGeneric->CreateTexture(dx_p2,dy_p2,false);

    //Only if there is audio output
    if (sample) {
        //Tell wrapper to output audio in device format
        AVSampleFormat format = AVWrapper::fromSDLAudioFormat(SNDDeviceFormat(), false);
        //wrapper->setupAudioConverter(format, SNDDeviceChannels(), 0);
        wrapper->setupAudioConverter(format, SNDDeviceChannels(), SNDDeviceFrequency());
    
        //Load a silent sound buffer, so we can get a looping sample that will poll our buffer
        //This is a workaround since SDL_mixer doesn't have a way to stream audio continuously
        //But Channel effects allows us to achieve a similar effect by passing audio data each time channel
        //audio is processed by SDL_mixer
        sample->looped = true;
        const int samples = SNDDeviceFrequency() / 10;
        size_t buf_len = SNDformatSampleSize(SNDDeviceFormat()) * SNDDeviceChannels() * samples;
        void* buf = SDL_calloc(buf_len, 1);
        sample->loadRawData(static_cast<uint8_t*>(buf), buf_len, false);
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
	bool end = wrapper->end && wrapper->getDuration() <= getPlayerTime();
    if (end) {
        if (!wrapper->videoFrames.empty()) {
            fprintf(stderr, "Remaining video frames!\n");
        }
        if (!wrapper->audioFrames.empty()) {
            fprintf(stderr, "Remaining audio frames!\n");
        }
    }
    return end;
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
    } else {
        startAudioPlayer();
    }

    paused = state;
}

void VideoPlayer::startAudioPlayer() {
    if (!sample) return;
    int channel = sample->play();
    if (channel != SND_NO_CHANNEL) {
        if (!Mix_RegisterEffect(channel, VideoPlayer::channelBufferEffect, nullptr, audioBuffer)) {
            fprintf(stderr, "VideoPlayer Mix_RegisterEffect error: %s\n", Mix_GetError());
        }
    }
}

double VideoPlayer::getPlayerTime() const {
    return currentTime;
}

bool VideoPlayer::InternalUpdate() {
    if (!paused && !applicationHasFocus()) {
        setPause(true);
    }

    if (paused) {
        Sleep(500);
        return true;
    }

    //printf("Buffered audio %f\n", SNDcomputeAudioLengthS(audioBuffer->written));

    //Decode frames
    decodeFrames();
    
    double audioTime = SNDcomputeAudioLengthS(audioBuffer->read_counter);

    //Update current time by update time
    if (lastUpdateTime) {
        double elapsed = static_cast<double>(getPerformanceCounter() - lastUpdateTime);
        elapsed /= static_cast<double>(getPerformanceFrequency());
        //printf("FPS: %f\n", 1.0 / elapsed);
        elapsed = std::min(TIME_MAX_STEP, elapsed);
        if (0 == audioBuffer->written || 0 <= wrapper->audioStream) {
            //If video has no audio we just keep 
            currentTime = currentTime + elapsed;
        } else {
            double nextTime = currentTime + elapsed;
            if (audioTime > nextTime + TIME_AUDIO_AHEAD) {
                //Audio is ahead of Video, use audio time and add our elapsed time
                currentTime = audioTime + elapsed;
            } else if (nextTime > audioTime + TIME_VIDEO_AHEAD) {
                //Video is ahead of audio, slowdown
                currentTime = currentTime + elapsed / 2.0;
            } else {
                //Synced
                currentTime = nextTime;
            }
        }
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

    if (0 < audioBuffer->written && sample && !sample->isPlaying()) {
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
    uint8_t* ptr = pTexture->LockTexture(pitch);

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
    //No sample to output, just do nothing
    if (!sample) return;
    
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
            buf = static_cast<uint8_t*>(SDL_calloc(buf_size, 1));
            audioBuffer->writeAudio(buf, buf_size);
            SDL_free(buf);
            buf = nullptr;
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
    double read_audio = SNDcomputeAudioLengthS(audioBuffer->read_counter);
    
    //Get enough video and audio frames
    while (!wrapper->end && (
            wrapper->videoFrames.size() < VIDEO_DECODE_AHEAD
        || (wrapper->audioFrames.empty() || (wrapper->audioFrames.back()->getPresentationTime() - read_audio) < AUDIO_DECODE_AHEAD))
    ) {
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
        //printf("Audio frame into buffer %ld %ld\n", write_loop, audioBuffer->written);
        if (AUDIO_DECODE_AHEAD < SNDcomputeAudioLengthS(std::max(write_loop, audioBuffer->written))) {
            break;
        }
        AVWrapperFrame* audioFrame = wrapper->audioFrames.front();
        wrapper->audioFrames.pop_front();
        WriteAudioFrame(audioFrame);
        delete audioFrame;
    }
    //printf("decodedFrames %ld %f\n", wrapper->videoFrames.size(), SNDcomputeAudioLengthS(audioBuffer->written));
}

void VideoPlayer::channelBufferEffect(int _channel, void *stream, int len, void *udata) {
    AudioBuffer* audioBuffer = static_cast<AudioBuffer*>(udata);
    if (!audioBuffer->written) return;
    
    size_t need = static_cast<size_t>(len);

#ifdef PERIMETER_DEBUG
    //printf("CBE %ld %f -> %ld %f\n", audioBuffer->written, SNDcomputeAudioLengthS(audioBuffer->written), need, SNDcomputeAudioLengthS(need));
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
