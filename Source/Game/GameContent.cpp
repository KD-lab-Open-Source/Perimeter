#include "StdAfx.h"
#include "UnitAttribute.h"
#include "GameContent.h"

int firstMissionNumber = 0;

GAME_CONTENT terGameContent = CONTENT_NONE;

void detectGameContent() {
    //Scan resources first
    scan_resource_paths();
    
    //Detect if resource dir is present
    if (convert_path_resource("Resource").empty()) {
        ErrH.Abort("Resource directory missing, if resource.pak exists please unzip it.");
    }
    
    //Do content identification
    terGameContent = GAME_CONTENT::CONTENT_NONE;
    int content = terGameContent;
    if (!convert_path_resource("Resource/Missions/Tutorial.spg").empty()) {
        content |= GAME_CONTENT::PERIMETER;
    }
    if (!convert_path_resource("Resource/Missions/01x4.spg").empty()) {
        content |= GAME_CONTENT::PERIMETER_ET;
    }

    if (content <= GAME_CONTENT::CONTENT_NONE) {
        ErrH.Abort("Couldn't identify game content type in Resource, some data may be missing.", XERR_USER, content);
        return;
    }
    terGameContent = static_cast<GAME_CONTENT>(content);
    
    //Since Perimeter has tutorial, we need to set first mission as 1 (second)
    if (terGameContent & GAME_CONTENT::PERIMETER) {
        firstMissionNumber = 1;
    }
}

bool unavailableContentUnitAttribute(terUnitAttributeID id) {
    switch (id) {
        case UNIT_ATTRIBUTE_ELECTRO_STATION1:
        case UNIT_ATTRIBUTE_ELECTRO_STATION2:
        case UNIT_ATTRIBUTE_ELECTRO_STATION3:
        case UNIT_ATTRIBUTE_ELECTRO_CANNON:
        case UNIT_ATTRIBUTE_EFLAIR:
        case UNIT_ATTRIBUTE_IMPALER:
        case UNIT_ATTRIBUTE_CONDUCTOR:
            return !(terGameContent & GAME_CONTENT::PERIMETER_ET);
        default:
            return false;
    }
}

bool unavailableContentBelligerent(terBelligerent belligerent) {
    switch (belligerent) {
        //HarkBack has missing UI textures and crashes in ET
        case BELLIGERENT_HARKBACKHOOD0:
        case BELLIGERENT_HARKBACKHOOD1:
            //Discard original Vice since it uses infected vice model (which is already selectable)
        case BELLIGERENT_EMPIRE_VICE:
            return terGameContent == GAME_CONTENT::PERIMETER_ET;
            //Addon frames which requires ET content
        case BELLIGERENT_EXODUS2:
        case BELLIGERENT_EXODUS3:
        case BELLIGERENT_EXODUS4:
        case BELLIGERENT_EMPIRE2:
        case BELLIGERENT_EMPIRE3:
        case BELLIGERENT_EMPIRE4:
            return !(terGameContent & GAME_CONTENT::PERIMETER_ET);
        default:
            return false;
    }
}
