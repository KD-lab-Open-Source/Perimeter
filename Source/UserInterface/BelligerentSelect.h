#ifndef PERIMETER_BELLIGERENTSELECT_H
#define PERIMETER_BELLIGERENTSELECT_H

#include "UnitAttribute.h"

class CShellWindow;

enum BELLIGERENT_FACTION {
    FACTION_NONE = -1,
    EXODUS,
    EMPIRE,
    HARKBACK
};

static terBelligerent SelectableBelligerents[] = {
        //BELLIGERENT_NONE
        BELLIGERENT_EXODUS0, //Cluster
        BELLIGERENT_EXODUS1, //Ark-9
        BELLIGERENT_EXODUS2, //Addon: Bunch,Organ,Pioneer
        BELLIGERENT_EXODUS3, //Addon: Ancient Frame
        BELLIGERENT_EXODUS4, //Addon: Enclave
        BELLIGERENT_EMPIRE0, //Router
        BELLIGERENT_EMPIRE1, //Existor
        BELLIGERENT_EMPIRE_VICE, //Vice
        BELLIGERENT_EMPIRE2, //Addon: Vice ET
        //BELLIGERENT_EMPIRE3, //Unused old Vice ET, not usable
        BELLIGERENT_EMPIRE4, //Infected Vice
        BELLIGERENT_HARKBACKHOOD0, //Freeborn
        BELLIGERENT_HARKBACKHOOD1, //Zodiac
};

uint8_t getBelligerentIndex(terBelligerent belligerent);
uint8_t getSelectableBelligerentIndex(terBelligerent belligerent);
terBelligerent getBelligerentFromIndex(uint8_t index);
std::string getBelligerentName(terBelligerent belligerent);
BELLIGERENT_FACTION getBelligerentFaction(terBelligerent belligerent);
void setBelligerentFactionSound(BELLIGERENT_FACTION faction);
std::string getBelligerentFactionName(BELLIGERENT_FACTION faction);
std::string getBelligerentAndFactionName(terBelligerent belligerent, const std::string& separator = "\n");
void setupFrameButton(CShellWindow* pWnd, InterfaceEventCode code, int number, bool sendNetCommand);

#endif //PERIMETER_BELLIGERENTSELECT_H
