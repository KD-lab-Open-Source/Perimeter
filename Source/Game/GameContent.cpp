#include <SDL.h>
#include "StdAfx.h"
#include "GameContent.h"
#include "files/files.h"

namespace scripts_export {
#include "Scripts/UnitAttribute.hi"
}

#include "Runtime.h"
#include "BelligerentSelect.h"
#include "qd_textdb.h"
#include "Localization.h"

#include <map>
#include <set>

extern bool content_debug_flag;

int firstMissionNumber = 0;

///The identified content at the root of game content, this only can be one thing 
GAME_CONTENT terGameContentBase = CONTENT_NONE;
///All available contents in this installation (base + addons)
GAME_CONTENT terGameContentAvailable = CONTENT_NONE;
///Current selected content, can be several or only one in available content (when user chooses one)
GAME_CONTENT terGameContentSelect = CONTENT_NONE;

bool mapContentPath(const std::string& source, const std::string& destination, const filesystem_scan_options* options = nullptr) {
    std::string source_path = convert_path_content(source);
    const std::string& destination_path = destination.empty() ? source : destination;
    if (source_path.empty()) {
        if (content_debug_flag) {
            fprintf(stderr, "Content path %s for %s not present\n", source.c_str(), destination_path.c_str());
        }
        return false;
    }
    if (content_debug_flag) {
        printf("Mapping path: %s -> %s\n", source_path.c_str(), destination_path.c_str());
    }
    return scan_resource_paths(
            destination_path,
            source_path,
            options
    );
}

bool checkMappingPath(const std::string& path) {
    if (path.empty()) return false;
    
    std::string path_lower = convert_path_posix(string_to_lower(path.c_str()));
    bool allowed = false;
    
    //Check if contains ..
    size_t pos = path_lower.find("..");
    if (pos != std::string::npos) {
        if (content_debug_flag) {
            printf("Mapping path contains .. which is disallowed: %s\n", path.c_str());
        }
        return false;
    }
    
    //Check if path starts with any allowed way to start
    for (auto& start : {"resource/", "scripts/", "mods/"}) {
        if (startsWith(path_lower, start)) {
            allowed = true;
            break;
        }
    }

    if (content_debug_flag && !allowed) {
        printf("Mapping path doesn't start with allowed dirs: %s\n", path.c_str());
    }
    
    return allowed;
}

void loadMappings(const std::string& path, const filesystem_scan_options* options = nullptr) {
    XStream fh(0);
    if (!fh.open(path, XS_IN)) {
        fprintf(stderr, "Error loading content mapping: %s\n", path.c_str());
        return;
    }
    std::string acc_line;
    char c = 0;

    do {
        //Read char
        fh.read(&c,1);
        
        if (c == '\n') {
            //Newline, parse accumulated text
            size_t pos = acc_line.find('=');
            if (pos != std::string::npos) {
                std::string src = acc_line.substr(0, pos);
                std::string dst = acc_line.substr(pos + 1);
                if (checkMappingPath(src) && checkMappingPath(dst)) {
                    mapContentPath(src, dst, options);
                } 
            }
            
            //Clear it
            acc_line.clear();
            continue;
        } else if (c == '#' && acc_line.empty()) {
            //This is a comment, skip
            continue;
        } else {
            //Append current char
            acc_line += c;
        }
    } while(!fh.eof());

    printf("Applied content mapping: %s\n", path.c_str());
}

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

    //Path for content in preferences path
    std::string prefPath;
    const char* prefPath_ptr = GET_PREF_PATH();
    if (prefPath_ptr) {
        prefPath = prefPath_ptr;
        prefPath += "Content";
        SDL_free((void*) prefPath_ptr);
    }
    
#ifndef _WIN32
    //On other OS data is usually separated from executable so is wise to check there first
    paths.emplace_back(prefPath);
#else
    //On Windows use current dir as relative to avoid non-ascii path breaking the game
    paths.emplace_back(".");
#endif

    //Running binary directory
    const char* basePath_ptr = SDL_GetBasePath();
    if (basePath_ptr) {
        paths.emplace_back(basePath_ptr);
        SDL_free((void*) basePath_ptr);
    }

#if !defined(__APPLE__) && !defined(_WIN32)
    //Current working directory absolute path
    //Mac apps get exception when trying to scan it's own dir so don't do it
    //On Windows we use relative current directory instead of absolute (see above)
    paths.emplace_back(std::filesystem::current_path().u8string());
#endif
    
#ifdef _WIN32
    //Check pref dir on Windows after working dir
    paths.emplace_back(prefPath);
#endif

    //Path stored in settings from last run
    if (settingsPath) paths.emplace_back(settingsPath);

    //Check paths for Resource dir
    std::set<std::string> scannedPaths;
    for (std::string rootPathStr : paths) {
        if (rootPathStr.empty()) continue;
        terminate_with_char(rootPathStr, PATH_SEP);
        scannedPaths.insert(rootPathStr);
        std::filesystem::path rootPath = std::filesystem::u8path(rootPathStr);
        printf("Checking game content from: %s absolute: %s\n", rootPathStr.c_str(), std::filesystem::absolute(rootPath).u8string().c_str());
        if (std::filesystem::exists(rootPath)) {
            clear_content_entries();
            set_content_root_path(rootPath);
            //Set current path to game directory to allow relative paths inside
            //This is specially needed for MacOS as it forbids writing inside .app
            std::filesystem::current_path(rootPath);
            if (scan_resource_paths("./")) {
                if (convert_path_content("Perimeter.ini").empty()) {
                    fprintf(stderr, "Path for content doesn't contain game: %s\n", rootPathStr.c_str());
                } else {
                    break;
                }
            } else {
                fprintf(stderr, "Path for content can't be scanned: %s\n", rootPathStr.c_str());
            }
        } else {
            fprintf(stderr, "Path for content doesn't exist: %s\n", rootPathStr.c_str());
        }
    }
    
    //Detect if resource dir is present
    if (convert_path_content("Perimeter.ini").empty()) {
        std::string msg = "Game content not found. Scanned paths:\n";
        for (const std::string& rootPath : scannedPaths) {
            msg += rootPath + "\n";
        }
        ErrH.ShowErrorMessage(msg.c_str());
        ErrH.Exit();
        return;
    }
    
    printf("Using game content at: %s\n", get_content_root_path_str().c_str());
}

void addGameContent(GAME_CONTENT content) {
    terGameContentAvailable = static_cast<GAME_CONTENT>(terGameContentAvailable | content);
}

///Load ET content selectively
void loadAddonET(const std::string& addonName, const std::string& addonDir) {
    //This is ET content as addon, skip if main content is not Perimeter
    if (terGameContentBase != GAME_CONTENT::PERIMETER) {
        printf("Skipping ET content as base content is not Perimeter: %s\n", addonName.c_str());
        return;
    }
    printf("Detected ET content at: %s\n", addonName.c_str());
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
    
    //Use exodus textures for generic units so harkback can have them and dont look like toys
    std::string harktextures = "Resource/Models/Main/Harkbackhood/";
    paths["Resource/Models/Main/Exodus/state_bump.tga"] = harktextures;
    paths["Resource/Models/Main/Exodus/Gun.tga"] = harktextures;
    paths["Resource/Models/Main/Exodus/Impaler.tga"] = harktextures;
    paths["Resource/Models/Main/Exodus/efla.tga"] = harktextures;
    paths["Resource/Models/Main/Exodus/Conductor.tga"] = harktextures;

    if (terGameContentSelect != PERIMETER_ET) {
        //Map mechanical spirit (inferno) and spirit trap (catcher), for future use?
        paths["Resource/Models/Main/build_harkbackers_st.l3d"] = "build_Resource/Models/Main/inferno.l3d";
        paths["Resource/Models/Main/build_harkbackers_st.m3d"] = "build_Resource/Models/Main/inferno.m3d";
        paths["Resource/Models/Main/build_filth_navigator.l3d"] = "build_Resource/Models/Main/catcher.l3d";
        paths["Resource/Models/Main/build_filth_navigator.m3d"] = "build_Resource/Models/Main/catcher.m3d";
        paths["Resource/Models/Main/harkbackers_st.l3d"] = "Resource/Models/Main/inferno.l3d";
        paths["Resource/Models/Main/harkbackers_st.m3d"] = "Resource/Models/Main/inferno.m3d";
        paths["Resource/Models/Main/filth_navigator.l3d"] = "Resource/Models/Main/catcher.l3d";
        paths["Resource/Models/Main/filth_navigator.m3d"] = "Resource/Models/Main/catcher.m3d";
    }

    //Map infected vice
    paths["Resource/Models/Main/Frame_Imperia_Vice.l3d"] = "Resource/Models/Main/Frame_Imperia_Vice_infected.l3d";
    paths["Resource/Models/Main/Frame_Imperia_Vice.m3d"] = "Resource/Models/Main/Frame_Imperia_Vice_infected.m3d";

    //Load certain models
    std::vector<std::string> models = {
        "build_electro_st", "build_electro_st",
        "electro_gun", "electro_gun",
        "eflair", "impaler", "conductor"
    };
    if (terGameContentSelect == PERIMETER_ET) {
        models.emplace_back("build_harkbackers_st");
        models.emplace_back("build_filth_navigator");
        models.emplace_back("harkbackers_st");
        models.emplace_back("filth_navigator");
    }
    for (const std::string& name : models) {
        paths["Resource/Models/Main/" + name + ".l3d"] = "";
        paths["Resource/Models/Main/" + name + ".m3d"] = "";
    }

    //Load texts, first try current lang, then english, then russian
    std::string locpath = getLocDataPath();
    std::vector<std::string> lang_paths;
    lang_paths.emplace_back(locpath);
    lang_paths.emplace_back("Resource/LocData/English/");
    lang_paths.emplace_back("Resource/LocData/Russian/");
    for (std::string& path : lang_paths) {
        std::string texts_path = path + "Text/Texts.btdb";
        if (!convert_path_content(addonDir + texts_path).empty()) {
            //Override texts if game content selection is ET only
            if (terGameContentSelect == PERIMETER_ET) {
                paths[texts_path] = locpath + "Text/Texts_ET.btdb";
                //paths[path + "Fonts"] = locpath;
                paths[path + "Voice"] = locpath + "Voice/";
            } else {
                paths[texts_path] = locpath + "Text/Texts_ET_noreplace.btdb";
            }
            printf("Addon ET: Using locale at %s\n", path.c_str());
            break;
        }
    }

    if (terGameContentSelect == PERIMETER_ET) {
        //Load mission required data and other ET stuff
        paths["Resource/scenario.hst"] = "";
        paths["Resource/Icons"] = "";
        paths["Resource/Missions"] = "";
        paths["Resource/Music"] = "";
        paths["Resource/Video"] = "";
        paths["Scripts/Triggers"] = "";
        paths["Scripts"] = "";
    } else {
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
    }

    //Map the ET resource paths into game
    for (const auto& entry : paths) {
        const std::string& destination = entry.second.empty() ? entry.first : entry.second;
        mapContentPath(addonDir + entry.first, destination);
    }

    if (terGameContentSelect != PERIMETER_ET) {
        //Map music manually to avoid copying main menu music
        for (filesystem_entry* entry : get_content_entries_directory(addonDir + "Resource/Music")) {
            if (endsWith(entry->key, "perimeter_main.ogg")) {
                continue;
            }

            mapContentPath(entry->key, "Resource/Music/");
        }
    }

    //Set flag that we have ET content
    addGameContent(GAME_CONTENT::PERIMETER_ET);
}

///Load addon contents into root of virtual resources
void loadAddon(const std::string& addonName, const std::string& addonDir) {
    printf("Loading mod: %s\n", addonName.c_str());
    
    //Skip certain resource dirs such as saves and replays
    for (const auto& entry : get_content_entries_directory(addonDir + "Resource")) {
        std::filesystem::path entry_path = std::filesystem::u8path(entry->key);
        std::string entry_name = entry_path.filename().u8string();
        std::string destination = std::string("Resource") + PATH_SEP + entry_name;
        //Check if we should skip certain dirs
        entry_name = string_to_lower(entry_name.c_str());
        if (entry_name == "saves" || entry_name == "replay") continue;

        //Load content from entry to destination
        scan_resource_paths(
                destination,
                entry->path_content
        );
    }

    //Load scripts
    if (get_content_entry(addonDir + "Scripts") != nullptr) {
        scan_resource_paths("Scripts", addonDir + "Scripts");
    }
}

///Common addon loading code
void loadAddonCommon(const std::string& addonName, const std::string& addonDir) {
    if (!convert_path_content(addonDir + "Resource/Missions/01x4.spg").empty()) {
        loadAddonET(addonName, addonDir);
    } else {
        loadAddon(addonName, addonDir);
    }

    std::string mapping = convert_path_content(addonDir + "content_mapping.txt");
    if (!mapping.empty()) {
        loadMappings(mapping);
    }

    printf("Loaded mod: %s\n", addonName.c_str());
}

void applyWorkarounds() {
    std::map<std::string, std::string> paths;
    std::string locpath = getLocDataPath();

    if (terGameContentAvailable & GAME_CONTENT::PERIMETER_ET) {
        for (auto faction : { BELLIGERENT_FACTION::EXODUS, BELLIGERENT_FACTION::EMPIRE, BELLIGERENT_FACTION::HARKBACK }) {
            std::string factionpath = locpath + "Voice/" + getBelligerentFactionName(faction);
            
            //These audios are missing for harkback since ET doesn't have this faction
            //On english version these audios have "Gun 
            paths[factionpath + "_Voice_Building_Ready.wav"] = factionpath + "_Voice_ElectroGun_Ready.wav";
            paths[factionpath + "_Voice_Building_Destroyed.wav"] = factionpath + "_Voice_ElectroGun_Destroyed.wav";
        }
    }

    //Map the resource paths
    for (const auto& entry : paths) {
        mapContentPath(entry.first, entry.second);
    }
}

void detectGameContent() {
    //We may need to do some cleanup
    clear_content_entries();
    terGameContentAvailable = terGameContentBase = terGameContentSelect = GAME_CONTENT::CONTENT_NONE;
    
    content_debug_flag = check_command_line("content_debug") != nullptr;
    
    findGameContent();

    //Do available content identification, leave Perimeter last just in case someone mixed both folders into one
    if (!convert_path_content("Resource/Missions/01x4.spg").empty()) {
        addGameContent(GAME_CONTENT::PERIMETER_ET);
        terGameContentBase = GAME_CONTENT::PERIMETER_ET;
    }
    if (!convert_path_content("resource.pak").empty() || !convert_path_content("Resource/Missions/Tutorial.spg").empty()) {
        addGameContent(GAME_CONTENT::PERIMETER);
        terGameContentBase = GAME_CONTENT::PERIMETER;
    }

    if (terGameContentAvailable <= GAME_CONTENT::CONTENT_NONE) {
        ErrH.Abort("Couldn't identify game content type in Resource, some data may be missing.", XERR_USER, terGameContentAvailable);
        return;
    }

    //Check if we should select another content, this has to be done before loading addons
    const char* content_selection = check_command_line("content_select");
    if (content_selection) {
        for (auto const& selected : getGameContentFromEnumName(content_selection)) {
            terGameContentSelect = static_cast<GAME_CONTENT>(terGameContentSelect | selected);
        }
    }

    //Store current game content type and the path in its settings
    IniManager* ini = getSettings();
    ini->put("Global", "LastContent", getGameContentEnumName(terGameContentBase).c_str());
    putStringSettings("GameContent", get_content_root_path_str());
        
    //Detect if we have extra contents
    int loadAddons = 1;
    check_command_line_parameter("mods", loadAddons);
    std::vector<std::string> addons;
    if (loadAddons) {
        for (const auto& entry: get_content_entries_directory("mods")) {
            if (entry->is_directory) {
                std::filesystem::path entry_path = std::filesystem::u8path(entry->path_content);
                std::string addonName = entry_path.filename().u8string();
                if (endsWith(addonName, ".off")) {
                    printf("Skipping disabled mod: %s\n", addonName.c_str());
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
        std::string addonDir = std::string("mods") + PATH_SEP + addonName + PATH_SEP;
        loadAddonCommon(addonName, addonDir);
    }
    
    //Do some workarounds
    applyWorkarounds();
    
    //Ensure selected content is actually present
    if (terGameContentSelect == GAME_CONTENT::CONTENT_NONE || (terGameContentSelect & terGameContentAvailable) != terGameContentSelect) {
        terGameContentSelect = terGameContentAvailable;
    }
    
    //Since Perimeter has tutorial, we need to set first mission as 1 (second) so the briefing is skipped
    if (terGameContentSelect & GAME_CONTENT::PERIMETER) {
        firstMissionNumber = 1;
    }

    printf("Game content: base %s selected %s available %s\n",
           getGameContentEnumName(terGameContentBase).c_str(),
           getGameContentEnumName(terGameContentSelect).c_str(),
           getGameContentEnumName(terGameContentAvailable).c_str()
   );
    
    //Dump full path list if requested
    if (check_command_line("content_dump_debug")) {
        dump_filesystem_entries(convert_path_content("content_list.txt", true));
        ErrH.Exit();
    }
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
        //HarkBack has missing UI textures and crashes in ET, and their lab/superweapon are replaced by inferno and catcher
        case BELLIGERENT_HARKBACKHOOD0:
        case BELLIGERENT_HARKBACKHOOD1:
            return content == GAME_CONTENT::PERIMETER_ET;
        //Discard original Vice since it uses infected vice model on ET (which is already selectable)
        case BELLIGERENT_EMPIRE_VICE:
            return terGameContentBase == GAME_CONTENT::PERIMETER_ET;
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

std::vector<GAME_CONTENT> getGameContentEnums(const uint32_t& content) {
    std::vector<GAME_CONTENT> contentList;
    if (content) {
        contentList = getEnumDescriptor(GAME_CONTENT()).keyCombination(content);
    }
    return contentList;
}

std::vector<GAME_CONTENT> getGameContentFromEnumName(const std::string& content) {
    return getEnumDescriptor(GAME_CONTENT()).keysFromNameCombination(content);
}

std::vector<GAME_CONTENT> getMissingGameContent(const GAME_CONTENT& content, const GAME_CONTENT& required) {
    uint32_t missingFlags = (content & required) ^ required;
    return getGameContentEnums(missingFlags);
}

std::string getGameContentEnumName(const GAME_CONTENT& content) {
    std::string name;
    for (auto const& contentEnum : getGameContentEnums(content)) {
        if (!name.empty()) name += "|";
        name += getEnumName(contentEnum);
    }
    return name;
}

std::string getGameContentName(const GAME_CONTENT& content) {
    std::string key("GAME_CONTENT.");
    key += getGameContentEnumName(content);
    return qdTextDB::instance().getText(key.c_str());
}

GAME_CONTENT getGameContentCampaign() {
    if (terGameContentSelect == terGameContentAvailable) {
        return terGameContentBase;
    }
    return terGameContentSelect;
}
