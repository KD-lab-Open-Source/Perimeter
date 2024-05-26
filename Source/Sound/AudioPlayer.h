#ifndef PERIMETER_PLAYER_H
#define PERIMETER_PLAYER_H

class AudioPlayer {
protected:
    bool request_play = false;
    
public:
    AudioPlayer() = default;
    virtual ~AudioPlayer() = default;

    virtual bool OpenToPlay(const char* fname, bool cycled) = 0;
    virtual void Stop() = 0;
    virtual void Pause() = 0;
    virtual void Resume() = 0;

    //virtual const char* GetFileName() = 0;

    virtual bool IsPlay() = 0;
    virtual bool IsPause() = 0;

    virtual void SetVolume(float volume) = 0;//0..255
    //virtual int GetVolume() = 0;

    void requestPlay(bool state);
};

class SpeechPlayer: public AudioPlayer {
private:
    float volume = 1.0f;
    class SND_Sample* sample = nullptr;

#ifdef GPX
    std::string fileName;
#endif
    
    void destroySample();
    
public:
    SpeechPlayer() = default;
    ~SpeechPlayer() override;

    bool OpenToPlay(const char* fname, bool cycled) override;
    void Stop() override;
    void Pause() override;
    void Resume() override;
    bool IsPlay() override;
    bool IsPause() override;
    void SetVolume(float volume) override;
    
    float GetLen();
};

class MusicPlayer: public AudioPlayer {
private:
    //Stores if music should be looped
    bool loop = false;
    //Clock time which music starts
    double music_start_time = 0;
    //Clock time which music pause starts
    double music_pause_time = 0;

    //TODO workaround to not play music after fading out in SDL_mixer, remove once we use linear volume fading
    double music_faded_out_pos = 0;
public:
    MusicPlayer() = default;

    ~MusicPlayer() override;

    bool OpenToPlay(const char* fname, bool cycled) override;
    void Stop() override;
    void Pause() override;
    void Resume() override;
    bool IsPlay() override;
    bool IsPause() override;
    void SetVolume(float volume) override;

    //Время считается в секундах

    //Постепенно изменить громкость с текущей до new_volume за время time
    //очищается при смене файла или вызове SetVolume
    //Криво работает с DeferredSeek
    void FadeVolume(float time, float new_volume = 0);
};

#endif // PERIMETER_PLAYER_H
