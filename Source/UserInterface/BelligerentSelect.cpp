#include "StdAfx.h"
#include "GameShellSq.h"
#include "qd_textdb.h"
#include "BelligerentSelect.h"
#include "GameShell.h"
#include "MainMenu.h"
#include "PerimeterShellUI.h"
#include "GameContent.h"

extern std::string getItemTextFromBase(const char *keyStr);

uint8_t getBelligerentIndex(terBelligerent belligerent) {
    return belligerent;
}

uint8_t getSelectableBelligerentIndex(terBelligerent belligerent) {
    int i = 0;
    for (terBelligerent selectable : SelectableBelligerents) {
        if (belligerent == selectable) {
            return i;
        }
        i++;
    }
    return 0;
}

terBelligerent getBelligerentFromIndex(uint8_t index) {
    switch(index){
        case 0:     return BELLIGERENT_NONE;
        case 1:     return BELLIGERENT_EXODUS0;
        case 2:     return BELLIGERENT_EXODUS1;
        case 3:     return BELLIGERENT_HARKBACKHOOD0;
        case 4:     return BELLIGERENT_HARKBACKHOOD1;
        case 5:     return BELLIGERENT_EMPIRE0;
        case 6:     return BELLIGERENT_EMPIRE1;
        case 7:     return BELLIGERENT_EMPIRE_VICE;
        case 8:     return BELLIGERENT_EXODUS2;
        case 9:     return BELLIGERENT_EXODUS3;
        case 10:    return BELLIGERENT_EXODUS4;
        case 11:    return BELLIGERENT_EMPIRE2;
        case 12:    return BELLIGERENT_EMPIRE3;
        case 13:    return BELLIGERENT_EMPIRE4;
        default:    return BELLIGERENT_EXODUS0;
    }
}

std::string getBelligerentName(terBelligerent belligerent) {
    std::string key = aiNameIDs[belligerent];
    std::string text = qdTextDB::instance().getText(key.c_str());
    if (text.empty()) {
        //If there is no name use the key name as workaround
        text = key;
        size_t pos = key.find('.');
        if (pos != std::string::npos) {
            text.erase(0, pos + 1);
        }
    }
    return text;
}

BELLIGERENT_FACTION getBelligerentFaction(terBelligerent belligerent) {
    switch (belligerent) {
        case BELLIGERENT_EXODUS0:
        case BELLIGERENT_EXODUS1:
        case BELLIGERENT_EXODUS2:
        case BELLIGERENT_EXODUS3:
        case BELLIGERENT_EXODUS4:
            return EXODUS;
        case BELLIGERENT_HARKBACKHOOD0:
        case BELLIGERENT_HARKBACKHOOD1:
            return HARKBACK;
        case BELLIGERENT_EMPIRE0:
        case BELLIGERENT_EMPIRE1:
        case BELLIGERENT_EMPIRE2:
        case BELLIGERENT_EMPIRE3:
        case BELLIGERENT_EMPIRE_VICE:
            return EMPIRE;
        case BELLIGERENT_EMPIRE4:
            //Infected vice acts as Empire in ET, harkback otherwise
            return terGameContentSelect == GAME_CONTENT::PERIMETER_ET ? EMPIRE : HARKBACK;
        case BELLIGERENT_NONE:
            break;
    }
    return FACTION_NONE;
}

void setBelligerentFactionSound(BELLIGERENT_FACTION faction) {
    switch(faction){
        default:
        case EXODUS:
            SNDSetBelligerentIndex(0);
            break;
        case HARKBACK:
            SNDSetBelligerentIndex(1);
            break;
        case EMPIRE:
            SNDSetBelligerentIndex(2);
            break;
    }
}

std::string getBelligerentFactionName(BELLIGERENT_FACTION faction) {
    switch (faction) {
        case EXODUS:
            return "Exodus";
        case EMPIRE:
            return "Empire";
        case HARKBACK:
            return "Harkback";
        case FACTION_NONE:
            break;
    }
    return "";
}

std::string getBelligerentAndFactionName(terBelligerent belligerent, const std::string& separator) {
    std::string text = getBelligerentName(belligerent);
    BELLIGERENT_FACTION faction = getBelligerentFaction(belligerent);
    bool isAddonFrame = false;
    switch (belligerent) {
        case BELLIGERENT_EXODUS2:
        case BELLIGERENT_EXODUS3:
        case BELLIGERENT_EXODUS4:
        case BELLIGERENT_EMPIRE2:
        case BELLIGERENT_EMPIRE3:
        case BELLIGERENT_EMPIRE4:
            isAddonFrame = true;
            break;
        default:
            break;
    }
    if (faction != FACTION_NONE || isAddonFrame) {
        text += separator + "&777777";
        if (faction != FACTION_NONE) {
            text += getItemTextFromBase(getBelligerentFactionName(faction).c_str());
            if (isAddonFrame) text += ", ";
        }
        if (isAddonFrame) {
            text += "ET";
        }
    }
    return text;
}

void setupFrameHandler(CComboWindow* combo, int number, bool sendNetCommand, bool direction) {
    int pos = combo->pos;

    //Change pos if passive
    if (combo->m_attr->passive) {
        if (direction) {
            pos++;
            if (pos >= combo->size) pos = 0;
        } else {
            pos--;
            if (pos < 0) pos = combo->size - 1;
        }
    }
    
    //Discard certain belligerents
    while (true) {
        terBelligerent selected = SelectableBelligerents[pos];
        if (unavailableContentBelligerent(selected, terGameContentSelect)) {
            if (direction) {
                pos++;
                if (pos >= combo->size) pos = 0;
            } else {
                pos--;
                if (pos < 0) pos = combo->size - 1;
            }
            //Don't apply pos if passive
            if (!combo->m_attr->passive) {
                combo->pos = pos;
            }
        } else {
            break;
        }
    }
    
    //Send to players
    if (sendNetCommand && gameShell->getNetClient()) {
        terBelligerent selected = SelectableBelligerents[pos];
        gameShell->getNetClient()->changePlayerBelligerent(number, selected);
    }
}

void setupFrameButton(CShellWindow* pWnd, InterfaceEventCode code, int number, bool sendNetCommand) {
    CComboWindow *pCombo = (CComboWindow*) pWnd;
    if( code == EVENT_CREATEWND ) {
        for (terBelligerent belligerent : SelectableBelligerents) {
            pCombo->Array.emplace_back(getBelligerentAndFactionName(belligerent));
        }
        pCombo->size = pCombo->Array.size();
        pCombo->pos = 0;
    } else if ((code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED)&& intfCanHandleInput()) {
         setupFrameHandler(pCombo, number, sendNetCommand, code == EVENT_UNPRESSED);
    }
}
