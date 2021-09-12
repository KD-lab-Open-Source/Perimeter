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
