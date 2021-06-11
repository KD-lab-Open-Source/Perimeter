#include "PlayMpp.h"

/* Already implemented in playogg.cpp, both headers share same class name
MpegSound::MpegSound() {
}

MpegSound::~MpegSound() {
}

bool MpegSound::OpenToPlay(const char* fname,bool cycled) {
    return false;
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
*/

MpegSamplePoint::MpegSamplePoint() {
}

MpegSamplePoint::~MpegSamplePoint() {
}

bool MpegSamplePoint::Load(LPCSTR fname) {
    return false;
}
