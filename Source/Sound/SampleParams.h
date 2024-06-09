//
// Created by Ion Agorria on 9/06/24
//
#ifndef PERIMETER_SAMPLEPARAMS_H
#define PERIMETER_SAMPLEPARAMS_H

//Which volume to use
enum GLOBAL_VOLUME {
    GLOBAL_VOLUME_CHANNEL = 0, //Use voice or effects volume according to current channel
    GLOBAL_VOLUME_VOICE,
    GLOBAL_VOLUME_EFFECTS,
    GLOBAL_VOLUME_IGNORE,
};

#endif //PERIMETER_SAMPLEPARAMS_H
