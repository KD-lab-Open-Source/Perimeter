#include <SDL.h>
#include "StdAfx.h"
#include "GameContent.h"

namespace scripts_export {
#include "Scripts/UnitAttribute.hi"
}

#include "Runtime.h"

#include <map>
#include <set>

int firstMissionNumber = 0;

GAME_CONTENT terGameContentBase = CONTENT_NONE;
GAME_CONTENT terGameContent = CONTENT_NONE;

void findGameContent() {
    //Get the path where game content is located by scanning diff paths
    std::vector<std::string> paths;

    //Check cmdline first
    const char* cmdlinePath = check_command_line("content");
    if (cmdlinePath) paths.emplace_back(cmdlinePath);
    
    //Add path stored in settings if any
    IniManager* ini = getSettings();
    const char* settingsPath = nullptr;
    std::string lastContent = getStringSettings("LastContent");
    if (!lastContent.empty()) settingsPath = ini->get(lastContent.c_str(), "GameContent");

    //On Windows use app dir before remembered and prefs path, on other OS data is usually separated from executable
    std::string prefPath = GET_PREF_PATH();
    terminate_with_char(prefPath, PATH_SEP);
    prefPath += "Content";

#ifdef _WIN32
    paths.emplace_back(std::filesystem::current_path().string());
    paths.emplace_back(SDL_GetBasePath());
    paths.emplace_back(prefPath);
#else
    paths.emplace_back(prefPath);
    paths.emplace_back(SDL_GetBasePath());
    paths.emplace_back(std::filesystem::current_path().string());
#endif
    if (settingsPath) paths.emplace_back(settingsPath);

    //Check paths for Resource dir
    std::set<std::string> scannedPaths;
    for (std::string rootPath : paths) {
        terminate_with_char(rootPath, PATH_SEP);
        scannedPaths.insert(rootPath);
        printf("Checking game content from: %s\n", rootPath.c_str());
        if (std::filesystem::exists(std::filesystem::path(rootPath))) {
            clear_resource_paths();
            set_content_root_path(rootPath);
            if (scan_resource_paths()) {
                if (!convert_path_resource("Resource").empty()) {
                    break;
                }
            }
        } else {
            printf("Path doesn't exist: %s\n", rootPath.c_str());
        }
    }
    
    //Detect if resource dir is present
    if (convert_path_resource("Resource").empty()) {
        std::string msg = "Resource directory not found, if resource.pak exists please unzip it. Scanned paths:\n";
        for (const std::string& rootPath : scannedPaths) {
            msg += rootPath + "\n";
        }
        ErrH.Abort(msg);
        return;
    }
    
    printf("Using game content at: %s\n", get_content_root_path().c_str());
}

void addGameContent(GAME_CONTENT content) {
    terGameContent = static_cast<GAME_CONTENT>(terGameContent | content);
    if (terGameContent & GAME_CONTENT::PERIMETER) {
        terGameContentBase = GAME_CONTENT::PERIMETER;
    } else if (terGameContent & GAME_CONTENT::PERIMETER_ET) {
        terGameContentBase = GAME_CONTENT::PERIMETER_ET;
    } else {
        terGameContentBase = GAME_CONTENT::CONTENT_NONE;
    }
}

///Common addon loading code
void loadAddonCommon(const std::string& addonDir) {
}

///Load ET content selectively
void loadAddonET(const std::string& addonDir) {
    //This is ET content as addon, skip if main content is not Perimeter
    if (!(terGameContent & GAME_CONTENT::PERIMETER)) {
        return;
    }
    std::map<std::string, std::string> paths;
    
    //Scan the hardcoded unit attributes data
    for(int i = 0; i < scripts_export::unitAttributeDataTable.size(); i++) {
        const scripts_export::terUnitAttributeData& src = scripts_export::unitAttributeDataTable[i];
        //We are only interested in ET models
        switch (src.ID) {
            case UNIT_ATTRIBUTE_FRAME:
                switch (src.belligerent) {
                    //ET frames
                    case BELLIGERENT_EXODUS2:
                    case BELLIGERENT_EXODUS3:
                    case BELLIGERENT_EXODUS4:
                    case BELLIGERENT_EMPIRE2:
                    case BELLIGERENT_EMPIRE3:
                        break;
                    case BELLIGERENT_EMPIRE4:
                        //We want this model but at different path, so we will add manually
                        continue;
                    default:
                        continue;
                }
            //ET stations
            case UNIT_ATTRIBUTE_ELECTRO_STATION1:
            case UNIT_ATTRIBUTE_ELECTRO_STATION2:
            case UNIT_ATTRIBUTE_ELECTRO_STATION3:
            case UNIT_ATTRIBUTE_ELECTRO_CANNON:
            //ET units
            case UNIT_ATTRIBUTE_EFLAIR:
            case UNIT_ATTRIBUTE_IMPALER:
            case UNIT_ATTRIBUTE_CONDUCTOR:
                break;
            default:
                continue;
        }

        //Copy model paths
        paths[src.LogicName] = "";
        for (int j = 0; j < src.ModelNameNum; ++j) {
            paths[src.ModelNameArray[j]] = "";
        }
    }
    
    //Add maps
    paths["resource/battle"] = "";
    paths["resource/battle/scenario"] = "resource/battle"; //Make ET campaign maps available normally since we cant unlock em
    paths["resource/multiplayer"] = "";
    paths["resource/worlds"] = "";
    paths["resource/geotx"] = "";
    paths["Resource/Models/Environment"] = "";
    
    //Add textures, these don't overlap original harkback structures textures
    paths["resource/models/main/exodus"] = "";
    paths["resource/models/main/empire"] = "";
    paths["resource/models/main/harkbackhood"] = "";
    paths["resource/models/main/textures"] = "";
    
    //More stuff
    paths["resource/fx"] = "";
    paths["resource/effect"] = "";
    paths["resource/sounds/eff"] = "";
    
    //Add AI scripts for battle so it uses electro units, also ETs map may require ET scripts
    paths["scripts/triggers/battle.scr"] = "";
    paths["scripts/triggers/battle1.scr"] = "";
    paths["scripts/triggers/survival.scr"] = "";
    paths["scripts/triggers/survival1.scr"] = "";
    for (int i = 1; i <= 12; ++i) {
        paths["scripts/triggers/mp-" + std::to_string(i) + ".scr"] = "";
    }
    
    //Try to use ET interface so we can have icons at least
    paths["Resource/Icons/intf"] = "";
    
    //Use exodus textures for generic units so harkback can have them and dont look like toys
    std::string harktextures = "Resource/Models/Main/Harkbackhood/";
    paths["Resource/Models/Main/Exodus/state_bump.tga"] = harktextures;
    paths["Resource/Models/Main/Exodus/Gun.tga"] = harktextures;
    paths["Resource/Models/Main/Exodus/Impaler.tga"] = harktextures;
    paths["Resource/Models/Main/Exodus/efla.tga"] = harktextures;
    paths["Resource/Models/Main/Exodus/Conductor.tga"] = harktextures;
    
    //Map mechanical spirit (inferno) and spirit trap (catcher), for future use?
    paths["Resource/Models/Main/Harkbackers_st.l3d"] = "Resource/Models/Main/inferno.l3d";
    paths["Resource/Models/Main/Harkbackers_st.m3d"] = "Resource/Models/Main/inferno.m3d";
    paths["Resource/Models/Main/filth_navigator.l3d"] = "Resource/Models/Main/catcher.l3d";
    paths["Resource/Models/Main/filth_navigator.m3d"] = "Resource/Models/Main/catcher.m3d";
    
    //Map infected vice
    paths["Resource/Models/Main/Frame_Imperia_Vice.l3d"] = "Resource/Models/Main/Frame_Imperia_Vice_infected.l3d";
    paths["Resource/Models/Main/Frame_Imperia_Vice.m3d"] = "Resource/Models/Main/Frame_Imperia_Vice_infected.m3d";

    //Load texts, first try current lang, then english, then russian
    std::string locpath = getLocDataPath();
    std::vector<std::string> text_paths;
    text_paths.emplace_back(locpath + "Text/Texts.btdb");
    text_paths.emplace_back("Resource/LocData/English/Text/Texts.btdb");
    text_paths.emplace_back("Resource/LocData/Russian/Text/Texts.btdb");
    for (std::string& path : text_paths) {
        if (!convert_path_resource((addonDir + path).c_str()).empty()) {
            paths[path] = locpath + "Text/Texts_ET_noreplace.btdb";
            //printf("Addon ET: Using texts %s\n", path.c_str());
            break;
        }
    }

    //Map the ET resource paths into game
    for (const auto& entry : paths) {
        std::string entry_source = addonDir + entry.first;
        std::string source_path = convert_path_resource(entry_source.c_str());
        if (source_path.empty()) {
            printf("Addon ET: %s not present\n", entry_source.c_str());
            continue;
        }
        std::string destination_path = entry.second.empty() ? entry.first : entry.second;
        //printf("Addon ET: %s -> %s\n", source_path.c_str(), destination_path.c_str());
        scan_resource_paths(
            destination_path,
            source_path
        );
    }

    //Set flag that we have ET content
    addGameContent(GAME_CONTENT::PERIMETER_ET);
}

///Load addon contents into root of virtual resources
void loadAddon(const std::string& addonDir) {
    //Skip certain resource dirs such as saves and replays
    for (const auto& entry : get_resource_paths_directory(addonDir + "Resource")) {
        std::filesystem::path entry_path(entry.first);
        std::string entry_name = entry_path.filename().string();
        std::string destination = std::string("Resource") + PATH_SEP + entry_name;
        //Check if we should skip certain dirs
        strlwr(entry_name.data());
        if (entry_name == "saves" || entry_name == "replay") continue;

        //Load content from entry to destination
        scan_resource_paths(
                destination,
                entry.second
        );
    }

    //Load scripts
    scan_resource_paths("Scripts", addonDir + "Scripts");
}

void detectGameContent() {
    findGameContent();

    //Do available content identification
    terGameContent = GAME_CONTENT::CONTENT_NONE;
    if (!convert_path_resource("Resource/Missions/Tutorial.spg").empty()) {
        addGameContent(GAME_CONTENT::PERIMETER);
    }
    if (!convert_path_resource("Resource/Missions/01x4.spg").empty()) {
        addGameContent(GAME_CONTENT::PERIMETER_ET);
    }

    if (terGameContent <= GAME_CONTENT::CONTENT_NONE) {
        ErrH.Abort("Couldn't identify game content type in Resource, some data may be missing.", XERR_USER, terGameContent);
        return;
    }

    //Store current game content type and the path in its settings
    IniManager* ini = getSettings();
    ini->put("Global", "LastContent", getEnumName(terGameContentBase));
    putStringSettings("GameContent", get_content_root_path());
        
    //Detect if we have extra contents
    int loadAddons = 1;
    check_command_line_parameter("addons", loadAddons);
    std::vector<std::string> addons;
    if (loadAddons) {
        for (const auto& entry: get_resource_paths_directory("addons")) {
            std::filesystem::path entry_path(entry.second);
            if (std::filesystem::is_directory(entry_path)) {
                std::string addonName = entry_path.filename().string();
                if (endsWith(addonName, ".off") || endsWith(addonName, ".disable")) {
                    printf("Skipping disabled addon: %s\n", addonName.c_str());
                    continue;
                }
                addons.emplace_back(addonName);
            }
        }
    }
    
    //Sort it
    std::sort(addons.begin(), addons.end());

    //Load addons
    for (std::string& addonName : addons) {
        std::string addonDir = std::string("Addons") + PATH_SEP + addonName + PATH_SEP;
        loadAddonCommon(addonDir);
        if (!convert_path_resource((addonDir + "Resource/Missions/01x4.spg").c_str()).empty()) {
            loadAddonET(addonDir);
        } else {
            loadAddon(addonDir);
        }
        
        printf("Loaded addon: %s\n", addonName.c_str());
    }
    
    //Since Perimeter has tutorial, we need to set first mission as 1 (second)
    if (terGameContent & GAME_CONTENT::PERIMETER) {
        firstMissionNumber = 1;
    }

    printf("Game content: %s (%x)\n", getEnumName(terGameContentBase), terGameContent);
}

bool unavailableContentUnitAttribute(terUnitAttributeID id, GAME_CONTENT content) {
    xassert(content != GAME_CONTENT::CONTENT_NONE);
    switch (id) {
        case UNIT_ATTRIBUTE_ELECTRO_STATION1:
        case UNIT_ATTRIBUTE_ELECTRO_STATION2:
        case UNIT_ATTRIBUTE_ELECTRO_STATION3:
        case UNIT_ATTRIBUTE_ELECTRO_CANNON:
        case UNIT_ATTRIBUTE_EFLAIR:
        case UNIT_ATTRIBUTE_IMPALER:
        case UNIT_ATTRIBUTE_CONDUCTOR:
            return !(content & GAME_CONTENT::PERIMETER_ET);
        default:
            return false;
    }
}

bool unavailableContentBelligerent(terBelligerent belligerent, GAME_CONTENT content) {
    xassert(content != GAME_CONTENT::CONTENT_NONE);
    switch (belligerent) {
        //HarkBack has missing UI textures and crashes in ET
        case BELLIGERENT_HARKBACKHOOD0:
        case BELLIGERENT_HARKBACKHOOD1:
            //Discard original Vice since it uses infected vice model (which is already selectable)
        case BELLIGERENT_EMPIRE_VICE:
            return content == GAME_CONTENT::PERIMETER_ET;
            //Addon frames which requires ET content
        case BELLIGERENT_EXODUS2:
        case BELLIGERENT_EXODUS3:
        case BELLIGERENT_EXODUS4:
        case BELLIGERENT_EMPIRE2:
        case BELLIGERENT_EMPIRE3:
        case BELLIGERENT_EMPIRE4:
            return !(content & GAME_CONTENT::PERIMETER_ET);
        default:
            return false;
    }
}
