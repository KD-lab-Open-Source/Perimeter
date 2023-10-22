#include "StdAfxSound.h"
#include "xmath.h"
#include "SoundInternal.h"
#include "files/files.h"
#include "Sample.h"
#include "AudioPlayer.h"

///////////////// AudioPlayer /////////////////////////////

void AudioPlayer::requestPlay(bool state) {
    request_play = state;
}

///////////////// SoundPlayer /////////////////////////////

SpeechPlayer::~SpeechPlayer() {
    this->destroySample();
}

void SpeechPlayer::destroySample() {
    if (sample) {
        sample->stop();
        delete sample;
        sample = nullptr;
    }
}

bool SpeechPlayer::OpenToPlay(const char* fname, bool cycled) {
    //fprintf(stderr, "SpeechPlayer %s\n", fname);
    destroySample();
    
    sample = SNDLoadSound(fname);
    if (!sample) return false;
    sample->looped = cycled; //Tecnically not need for speeches but whatever
    sample->channel_group = SND_GROUP_SPEECH;
    sample->volume = volume;
    sample->steal_channel = true; //Just in case another speech is playing
    
    bool playing = sample->play() != SND_NO_CHANNEL;
    if (!playing) {
        fprintf(stderr, "SpeechPlayer couldn't play %s\n", fname);
    }
    request_play = false;
    return playing;
}

void SpeechPlayer::Stop() {
    if (!sample) return;
    sample->stop();
}

void SpeechPlayer::Pause() {
    if (!sample) return;
    sample->pause();
}

void SpeechPlayer::Resume() {
    if (!sample) return;
    sample->resume();
}

bool SpeechPlayer::IsPlay() {
    if (request_play) return true;
    if (!sample) return false;
    return sample->isPlaying();
}

bool SpeechPlayer::IsPause() {
    if (!sample) return false;
    return sample->isPaused();
}

void SpeechPlayer::SetVolume(float volume_) {
    this->volume = std::max(0.0f, std::min(1.0f, volume_));
    if (!sample) return;
    sample->volume = this->volume;
    sample->updateEffects();
}

float SpeechPlayer::GetLen() { 
    if (!sample) return 0.0f;
    return static_cast<float>(sample->getDuration()) / 1000.0f;
}

///////////////// MusicPlayer /////////////////////////////

//Current music playing
static Mix_Music* music = nullptr;

MusicPlayer::~MusicPlayer() {
    destroyMusic();
}

void MusicPlayer::destroyMusic() {
    if (music) {
        Stop();
        if (SND::has_sound_init) {
            Mix_FreeMusic(music);
        }
        music = nullptr;
    }
}


bool MusicPlayer::OpenToPlay(const char* fname, bool cycled) {
    this->destroyMusic();
    
    //Library not initialized
    if(!SND::has_sound_init) {
        return false;
    }

    std::string path = convert_path_content(fname);
    if (path.empty()) {
        return false;
    }

    loop = cycled;
    music = Mix_LoadMUS(path.c_str());
    if(!music) {
        fprintf(stderr, "Mix_LoadMUS error %s : %s\n", fname, Mix_GetError());
        return false;
    }

    bool ok = Mix_PlayMusic(music, loop ? -1 : 0) != -1;
    if (ok) {
        music_start_time = clockf();
    } else {
        destroyMusic();
        fprintf(stderr, "Mix_PlayMusic error: %s\n", Mix_GetError());
    }
    return ok;
}

void MusicPlayer::Stop() {
    music_start_time = 0;
    music_pause_time = 0;
    music_faded_out_pos = 0;
    loop = false;
    if (SND::has_sound_init && music) {
        if (Mix_PlayingMusic()) {
            Mix_HaltMusic();
        }
    }
}

void MusicPlayer::Pause() {
    if (!music || !Mix_PlayingMusic()) return;
    music_pause_time = clockf();
    Mix_PauseMusic();
}

void MusicPlayer::Resume() {
    if (!music) return;
    if (0 < music_pause_time) {
        music_start_time += clockf() - music_pause_time;
        music_pause_time = 0;
    }
    if (0 < music_faded_out_pos) {
        Mix_PlayMusic(music, loop ? -1 : 0);
        Mix_SetMusicPosition(music_faded_out_pos);
        music_faded_out_pos = 0;
    } else {
        Mix_ResumeMusic();
    }
}

bool MusicPlayer::IsPlay() {
    if (!music) return false;
    return Mix_PlayingMusic() || 0 < music_faded_out_pos;
}

bool MusicPlayer::IsPause() {
    if (!music) return false;
    return Mix_PausedMusic();
}

void MusicPlayer::SetVolume(float volume) {
    if (!SND::has_sound_init) return;
    volume = std::max(0.0f, std::min(1.0f, volume)) * MIX_MAX_VOLUME;
    Mix_VolumeMusic(static_cast<int>(xm::round(volume)));
}

void MusicPlayer::FadeVolume(float time, float new_volume) {
    /* TODO 
       Ion: we should implement a linear volume -> new_volume while playing without using SDL_mixer as these
       stop/start the music, which doesn't happen in original code afaik but the way FadeVolume is used this should suffice
    */
    int time_ms = static_cast<int>(xm::round(time * 1000.0f));
    if (new_volume <= 0) {
        if (0 >= music_faded_out_pos && this->IsPlay() && Mix_FadingMusic() != MIX_FADING_OUT) {
            Mix_FadeOutMusic(time_ms);
            music_faded_out_pos = (clockf() - music_start_time) / 1000.0f + time;
            music_pause_time = clockf() + time;
        }
    } else {
        if (Mix_FadingMusic() != MIX_FADING_IN) {
            SetVolume(new_volume);
        }
        if (0 < music_faded_out_pos) {
            Mix_FadeInMusicPos(music, loop ? -1 : 0, time_ms, music_faded_out_pos);
        }
        if (0 < music_pause_time) {
            music_start_time += clockf() - music_pause_time;
            music_pause_time = 0;
        }
        music_faded_out_pos = 0;
    }
}

