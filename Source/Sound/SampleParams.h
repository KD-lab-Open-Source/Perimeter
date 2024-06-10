//
// Created by Ion Agorria on 9/06/24
//
#ifndef PERIMETER_SAMPLEPARAMS_H
#define PERIMETER_SAMPLEPARAMS_H

//Mixer channel groups
#define SND_GROUP_SPEECH 0
#define SND_GROUP_EFFECTS 1
#define SND_GROUP_EFFECTS_ONCE 2
#define SND_GROUP_EFFECTS_LOOPED 3

#define SND_NO_CHANNEL -1

//Which volume to use
enum GLOBAL_VOLUME {
    GLOBAL_VOLUME_CHANNEL = 0, //Use voice or effects volume according to current channel
    GLOBAL_VOLUME_VOICE,
    GLOBAL_VOLUME_EFFECTS,
    GLOBAL_VOLUME_IGNORE,
};

#endif //PERIMETER_SAMPLEPARAMS_H
