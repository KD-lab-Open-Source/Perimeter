#include "StdAfx.h"
#include "PlayOgg.h"

bool MpegInitLibrary(void* LPDIRECTSOUND_pDS)
{
    return false;
}
void MpegDeinitLibrary()
{
}

MpegSound::MpegSound()
{

}

MpegSound::~MpegSound()
{

}

void MpegSound::Stop()
{

}

void MpegSound::Pause()
{

}

void MpegSound::Resume()
{

}

MpegState MpegSound::IsPlay()
{
    return MPEG_STOP;
}

void MpegSound::SetVolume(int volume)
{
    //0..255
}

bool MpegSound::OpenToPlay(const char* fname, bool cycled /*= true*/)
{
    return true;
}
int MpegSound::GetVolume()
{
    return 0;
}

float MpegSound::GetLen()
{
    return 0.0;
}

bool MpegSound::FadeVolume(float time, int new_volume /*= 0*/)
{
    return false;
}