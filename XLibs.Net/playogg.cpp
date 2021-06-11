#include "PlayOgg.h"

bool MpegInitLibrary(void* LPDIRECTSOUND_pDS) {}
void MpegDeinitLibrary() {
    
}

MpegSound::MpegSound() {
}

MpegSound::~MpegSound() {
}

bool MpegSound::OpenToPlay(const char* fname,bool cycled) {
    return true;
}

void MpegSound::Stop() {
}

void MpegSound::Pause() {
}

void MpegSound::Resume() {
}

const char* MpegSound::GetFileName() {
    return "";
}

void MpegSound::SetPauseIfNullVolume(bool set) {
}

MpegState MpegSound::IsPlay() {
    return MPEG_STOP;
}

void MpegSound::SetVolume(int volume) {
}

int MpegSound::GetVolume() {
    return 0;
}

bool MpegSound::FadeVolume(float time,int new_volume) {
    return false;
}

float MpegSound::GetLen() {
    return 0.0f;
}

float MpegSound::GetCurPos() {
    return 0.0f;
}
