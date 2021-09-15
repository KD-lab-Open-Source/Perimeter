#include <SDL.h>
#include "StdAfx.h"
#include "GameContent.h"

int firstMissionNumber = 0;

GAME_CONTENT terGameContent = CONTENT_NONE;

void findGameContent() {
    //Get the path where game content is located by scanning diff paths
    std::vector<std::string> paths;

    //Check cmdline first
    const char* cmdlinePath = check_command_line("content");
    if (cmdlinePath) paths.emplace_back(cmdlinePath);

    //On Windows use app dir before prefs path, on other OS data is usually separated from executable
    std::string prefPath = SDL_GetPrefPath("KranX Productions", "Perimeter");
#ifndef _WIN32
    paths.emplace_back(prefPath);
#endif
    paths.emplace_back(std::string(".") + PATH_SEP);
    paths.emplace_back(SDL_GetBasePath());
#ifdef _WIN32
    paths.emplace_back(prefPath);
#endif

    //Check paths for Resource dir
    for (std::string& rootPath : paths) {
        if (std::filesystem::exists(rootPath)) {
            printf("Checking game content from: %s\n", rootPath.c_str());
            clear_resource_paths();
            set_content_root_path(rootPath);
            if (scan_resource_paths()) {
                if (!convert_path_resource("Resource").empty()) {
                    break;
                }
            }
        }
    }
    
    //Detect if resource dir is present
    if (convert_path_resource("Resource").empty()) {
        std::string msg = "Resource directory not found, if resource.pak exists please unzip it. Scanned paths:\n";
        for (const std::string& rootPath : paths) {
            msg += rootPath + "\n";
        }
        ErrH.Abort(msg);
        return;
    }
    printf("Using game content at: %s\n", get_content_root_path().c_str());
    
}

void addGameContent(GAME_CONTENT content) {
    terGameContent = static_cast<GAME_CONTENT>(terGameContent | content);
}

///Common addon loading code
void loadAddonCommon(const std::string& addonDir) {
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
        } else {
            loadAddon(addonDir);
        }
        
        printf("Loaded addon: %s\n", addonName.c_str());
    }
    
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
