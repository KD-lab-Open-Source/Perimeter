#pragma once
#include "PerimeterSound.h"

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }

void SNDSetupChannelCallback(bool init);
void SNDUpdateAllSoundVolume();
class SND_Sample;
SND_Sample* SNDLoadSound(const std::string& fname);

namespace SND {
extern float global_volume;
extern bool has_sound_init;
extern int deviceFrequency;
extern int deviceChannels;
extern SDL_AudioFormat deviceFormat;
///Minimum required volume to be played, this avoids starving channels by too many low volume effects
const float EFFECT_VOLUME_THRESHOLD = 0.05;
void logs(const char *format, ...);
};
