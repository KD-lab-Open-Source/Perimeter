#pragma once

#ifdef PERIMETER_FFMPEG_MOVIE
class AVWrapper;
class AVWrapperFrame;
class SND_Sample;
#endif

///Circular buffer for audio purposes
class AudioBuffer : protected XBuffer {
protected:
    size_t head = 0;
    void updateAvailable();
public:
    ///Amount of bytes read in total
    size_t read_counter = 0;
    ///Amount of bytes written in total
    size_t write_counter = 0;
    size_t written = 0;
    
    explicit AudioBuffer(double seconds);
    void writeAudio(const uint8_t* buf, size_t len);
    size_t readAudio(uint8_t* buf, size_t len);
    void clear();
    size_t space() const { return size - written; }
};

class VideoPlayer
{
public:
    VideoPlayer();
    ~VideoPlayer();

    bool Init(const char* path);
    ///Advances player state and returns true if video frame has been processed
    bool Update();
    ///Draws the current video frame
    void Draw(int x,int y,int dx,int dy, int alpha = 255);
    ///Player reached end?
    bool IsEnd();
    ///Get video frames size in pixels
    void getSize(Vect2i& vec);
    ///Set volume
    void SetVolume(float vol);
    ///Sets pause state
    void setPause(bool state);

#ifdef PERIMETER_FFMPEG_MOVIE
protected:
    SND_Sample* sample = nullptr;
    AudioBuffer* audioBuffer = nullptr;
    AVWrapper* wrapper = nullptr;
    cTexture* pTexture = nullptr;
    bool paused = false;
    ///Internal time for video time in case audio is not available
    uint64_t lastUpdateTime = 0;
    ///Current time for player
    double currentTime = 0;
    ///Last written audio time
    double audioBufferTime = 0;
    
    void startAudioPlayer();
    double getPlayerTime() const;
    bool InternalUpdate();
    void WriteVideoFrame(AVWrapperFrame* frame);
    void WriteAudioFrame(AVWrapperFrame* frame);
    AVWrapperFrame* popCurrentFrame(std::list<AVWrapperFrame*>& frames) const;
    void Done();
    void decodeFrames();
    static void channelBufferEffect(int chan, void *stream, int len, void *udata);
#endif
};
