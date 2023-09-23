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

static std::map<std::string, ModMetadata> gameMods;

///The identified content at the root of game content, this only can be one thing 
GAME_CONTENT terGameContentBase = CONTENT_NONE;
///All available contents in this installation (base + addons)
GAME_CONTENT terGameContentAvailable = CONTENT_NONE;
///Current selected content, can be several or only one in available content (when user chooses one)
GAME_CONTENT terGameContentSelect = CONTENT_NONE;

std::map<std::string, ModMetadata>& getGameMods() {
    return gameMods;
}

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


///ET specific workarounds for broken assets
void workaroundET() {
    std::map<std::string, std::string> paths;
    const std::string& locpath = getLocDataPath();

    for (auto faction : { BELLIGERENT_FACTION::EXODUS, BELLIGERENT_FACTION::EMPIRE, BELLIGERENT_FACTION::HARKBACK }) {
        std::string factionpath = locpath + "Voice/" + getBelligerentFactionName(faction);

        //These audios are missing for harkback since ET doesn't have this faction
        //On english version these audios just say "Gun", "Ready" or "Oops"
        //On russian version Exodus has Empire files for these audios
        paths[factionpath + "_Voice_Building_Ready.wav"] = factionpath + "_Voice_ElectroGun_Ready.wav";
        paths[factionpath + "_Voice_Building_Destroyed.wav"] = factionpath + "_Voice_ElectroGun_Destroyed.wav";
    }

    //Map the resource paths
    for (const auto& entry : paths) {
        mapContentPath(entry.first, entry.second);
    }
}

///Maps Icons/Video in current locale folder if any
void loadLocalizedResources(const std::string& content_path = "") {
    //Localized resources if any
    const std::string& locpath = getLocDataPath();
    for (auto& dir : {
            "Icons",
            "Video"
    }) {
        std::string path = content_path + locpath + dir;
        if (get_content_entry(path) != nullptr) {
            mapContentPath(path, std::string("Resource/") + dir);
        }
    }
}

///Detects if path is Perimeter: ET
bool isContentET(const std::string& path) {
    return get_content_entry(path + "Resource/Missions/01x4.spg")
    && get_content_entry(path + "Resource/Missions/25x2.spg");
}

///Load ET content selectively
void loadAddonET(ModMetadata& mod) {
    //This is ET content as addon, skip if main content is not Perimeter
    if (terGameContentBase != GAME_CONTENT::PERIMETER) {
        printf("Skipping ET content as base content is not Perimeter: %s\n", mod.mod_name.c_str());
        return;
    }

    bool legacy = convert_path_content(mod.path + "/Resource/Models/Main/inferno.l3d").empty();
    printf("Detected ET content at: %s type: %s\n", mod.mod_name.c_str(), legacy ? "legacy" : "reworked");
    
    //Add maps src to dst or src as dst if empty
    std::map<std::string, std::vector<std::string>> paths;
    
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
        paths[src.LogicName] = {};
        for (int j = 0; j < src.ModelNameNum; ++j) {
            paths[src.ModelNameArray[j]] = {};
        }
    }

    //Make ET campaign maps available normally since we cant unlock em
    paths["resource/battle/scenario"] = { "resource/battle" };
    
    //Add textures, these don't overlap original harkback structures textures
    paths["resource/models/main/exodus"] = {};
    paths["resource/models/main/empire"] = {};
    paths["resource/models/main/harkbackhood"] = {};
    paths["resource/models/main/textures"] = {};
    
    //More stuff
    paths["resource/fx"] = {};
    paths["resource/sounds"] = {};
    paths["resource/battle"] = {};

    if (legacy) {
        paths["Resource/controls.ini"] = {};
        paths["resource/multiplayer"] = {};
        paths["resource/geotx"] = {};
        paths["resource/worlds"] = {};
        paths["resource/models/environment"] = {};
        paths["resource/effect"] = {};
        
        //Use exodus textures for generic units so harkback can have them and dont look like toys
        std::string exodus_textures = "Resource/Models/Main/Exodus/";
        std::string harkback_textures = "Resource/Models/Main/Harkbackhood/";
        for (auto& tex: {
                "state_bump.tga"
                "Gun.tga"
                "Impaler.tga"
                "efla.tga"
                "Conductor.tga"
        }) {
            paths[exodus_textures + tex] = { harkback_textures + tex };
        }

        //Map mechanical spirit (inferno) and spirit trap (catcher), for future use?
        paths["Resource/Models/Main/build_harkbackers_st.l3d"] = { "Resource/Models/Main/build_inferno.l3d" };
        paths["Resource/Models/Main/build_harkbackers_st.m3d"] = { "Resource/Models/Main/build_inferno.m3d" };
        paths["Resource/Models/Main/build_filth_navigator.l3d"] = { "Resource/Models/Main/build_catcher.l3d" };
        paths["Resource/Models/Main/build_filth_navigator.m3d"] = { "Resource/Models/Main/build_catcher.m3d" };
        paths["Resource/Models/Main/harkbackers_st.l3d"] = { "Resource/Models/Main/inferno.l3d" };
        paths["Resource/Models/Main/harkbackers_st.m3d"] = { "Resource/Models/Main/inferno.m3d" };
        paths["Resource/Models/Main/filth_navigator.l3d"] = { "Resource/Models/Main/catcher.l3d" };
        paths["Resource/Models/Main/filth_navigator.m3d"] = { "Resource/Models/Main/catcher.m3d" };

        //Map infected vice
        paths["Resource/Models/Main/Frame_Imperia_Vice.l3d"] = { "Resource/Models/Main/Frame_Imperia_Vice_infected.l3d" };
        paths["Resource/Models/Main/Frame_Imperia_Vice.m3d"] = { "Resource/Models/Main/Frame_Imperia_Vice_infected.m3d" };
        
        //Override these models for missions too
        if (terGameContentSelect == PERIMETER_ET) {
            paths["Resource/Models/Main/build_harkbackers_st.l3d"].emplace_back("");
            paths["Resource/Models/Main/build_harkbackers_st.m3d"].emplace_back("");
            paths["Resource/Models/Main/build_filth_navigator.l3d"].emplace_back("");
            paths["Resource/Models/Main/build_filth_navigator.m3d"].emplace_back("");
            paths["Resource/Models/Main/harkbackers_st.l3d"].emplace_back("");
            paths["Resource/Models/Main/harkbackers_st.m3d"].emplace_back("");
            paths["Resource/Models/Main/filth_navigator.l3d"].emplace_back("");
            paths["Resource/Models/Main/filth_navigator.m3d"].emplace_back("");
            paths["Resource/Models/Main/Frame_Imperia_Vice.l3d"].emplace_back("");
            paths["Resource/Models/Main/Frame_Imperia_Vice.m3d"].emplace_back("");
        }
    } else {
        //This is a reworked ET
        
        paths["Resource/Models"] = {};
    }

    //Load texts, first try current lang, then english, then russian
    std::string locale = getLocale();
    std::string locpath = getLocDataPath();
    std::vector<std::string> lang_paths;
    lang_paths.emplace_back(locpath);
    lang_paths.emplace_back("Resource/LocData/English/");
    lang_paths.emplace_back("Resource/LocData/Russian/");
    for (std::string& path : lang_paths) {
        if (get_content_entry(mod.path + PATH_SEP + path)) {
            if (legacy) {
                std::string texts_path = path + "Text/Texts.btdb";
                //Override texts if game content selection is ET only
                if (terGameContentSelect == PERIMETER_ET) {
                    paths[texts_path] = {locpath + "Text/Texts_PerimeterET.btdb"};
                } else {
                    paths[texts_path] = {locpath + "Text/Texts_PerimeterET_noreplace.btdb"};
                }
            } else {
                std::string texts_path = "Text/Texts_PerimeterET_" + locale;
                //Override texts if game content selection is ET only
                if (terGameContentSelect == PERIMETER_ET) {
                    paths[path + texts_path + ".txt"] = {};
                } else {
                    paths[path + texts_path + ".txt"] = {locpath + texts_path + "_noreplace.txt"};
                }
            }

            //Override voices from ET
            if (terGameContentSelect == PERIMETER_ET) {
                paths[path + "Voice"] = {locpath + "Voice"};
            }
            printf("Addon ET: Using locale at %s\n", path.c_str());
            break;
        }
    }

    if (terGameContentSelect == PERIMETER_ET) {
        //Load mission required data and other ET stuff
        std::string path_scenario = "Resource/scenario.hst";
        if (!legacy && startsWith(locale, "russian")) {
            path_scenario = "Resource/scenario_russian.hst";
        }
        paths[path_scenario] = {
            "Resource/scenario.hst",
            //In case scenario_LOCALE.hst exists in base
            "Resource/scenario_" + locale + ".hst"
        };
        paths["Resource/Icons"] = {};
        paths["Resource/Missions"] = {};
        paths["Resource/Music"] = {};
        paths["Resource/Video"] = {};
        paths["Scripts"] = {};
    } else {
        for (int i = 1; i <= 12; ++i) {
            //Copy scripts in case maps need them
            paths["scripts/triggers/mp-" + std::to_string(i) + ".scr"] = {};
        }

        if (legacy) {
            //Try to use ET interface so we can have icons at least, non legacy the base game has the icons already
            paths["Resource/Icons/intf"] = {};
        }
    }

    //Map the ET resource paths into game
    std::string modPath = mod.path + PATH_SEP;
    for (const auto& entry : paths) {
        if (entry.second.empty()) {
            mapContentPath(modPath + entry.first, entry.first);
        } else {
            for (const auto& path : entry.second) {
                const std::string& destination = path.empty() ? entry.first : path;
                mapContentPath(modPath + entry.first, destination);
            }
        }
    }

    if (terGameContentSelect == PERIMETER_ET) {
        //Only load localized resources if set as active
        loadLocalizedResources(modPath);
    } else {
        //Map music manually to avoid copying main menu music
        for (filesystem_entry* entry : get_content_entries_directory(modPath + "Resource/Music")) {
            if (endsWith(entry->key, "perimeter_main.ogg")) {
                continue;
            }

            mapContentPath(entry->key, "Resource/Music/");
        }
    }

    //Legacy workarounds
    if (legacy) {
        workaroundET();
    }

    //Set flag that we have ET content
    addGameContent(GAME_CONTENT::PERIMETER_ET);
}

///Load addon contents into root of virtual resources
void loadMod(const ModMetadata& mod) {
    printf("Loading mod: %s\n", mod.mod_name.c_str());
    
    //Skip certain resource dirs such as saves and replays
    for (const auto& entry : get_content_entries_directory(mod.path + "/Resource")) {
        std::filesystem::path entry_path = std::filesystem::u8path(entry->key);
        std::string entry_name = entry_path.filename().u8string();
        std::string destination = std::string("Resource") + PATH_SEP + entry_name;
        //Check if we should skip certain dirs
        entry_name = string_to_lower(entry_name.c_str());
        if (entry_name == "saves" || entry_name == "replay") continue;

        //Load content from entry to destination
        mapContentPath(entry->path_content, destination);
    }

    loadLocalizedResources(mod.path + PATH_SEP);

    //Load scripts
    if (get_content_entry(mod.path + "Scripts") != nullptr) {
        mapContentPath(mod.path + "Scripts", "Scripts");
    }
}

///Common addon loading code
void loadModCommon(ModMetadata& mod) {
    mod.campaign = get_content_entry(mod.path + "/Resource/Missions") != nullptr; 
    if (isContentET(mod.path + PATH_SEP)) {
        loadAddonET(mod);
    } else {
        loadMod(mod);
    }

    std::string mapping = convert_path_content(mod.path + "/content_mapping.txt");
    if (!mapping.empty()) {
        loadMappings(mapping);
    }

    printf("Loaded mod: %s\n", mod.mod_name.c_str());
}

void applyWorkarounds() {
    if (terGameContentBase == GAME_CONTENT::PERIMETER_ET) {
        workaroundET();
    }
}

struct SortModMetadatas {
    inline bool operator ()(const ModMetadata& s1,const ModMetadata& s2) {
        return s1.mod_name < s2.mod_name;
    }
};

void detectGameContent() {
    //We may need to do some cleanup
    clear_content_entries();
    terGameContentAvailable = terGameContentBase = terGameContentSelect = GAME_CONTENT::CONTENT_NONE;
    gameMods.clear();
    
    content_debug_flag = check_command_line("content_debug") != nullptr;
    
    findGameContent();

    //Do available content identification, leave Perimeter last just in case someone mixed both folders into one
    if (isContentET("")) {
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
        terGameContentSelect = mergeGameContentEnums(getGameContentFromEnumName(content_selection));
    }

    //Store current game content type and the path in its settings
    IniManager* ini = getSettings();
    ini->put("Global", "LastContent", getGameContentEnumName(terGameContentBase).c_str());
    putStringSettings("GameContent", get_content_root_path_str());

    //Map localized resources, we do this before addons loading
    loadLocalizedResources();
        
    //Detect if we have extra contents/mods
    int loadMods = 1;
    check_command_line_parameter("mods", loadMods);
    std::vector<ModMetadata> foundMods;
    const std::string& locale = getLocale();
    for (const auto& entry: get_content_entries_directory("mods")) {
        if (entry->is_directory) {
            std::filesystem::path entry_path = std::filesystem::u8path(entry->path_content);
            
            ModMetadata data = {};
            data.available = false;
            data.path = entry_path.u8string();
            data.mod_name = entry_path.filename().u8string();

            std::string path_ini = data.path + PATH_SEP + "mod.ini";
            bool is_content_ET = isContentET(data.path + PATH_SEP);
            if (get_content_entry(path_ini)) {
                //Load mandatory .ini fields
                IniManager mod_ini = IniManager(path_ini.c_str(), true);
                data.available = true;
                data.mod_name = mod_ini.get("Mod", "name");
                data.mod_version = mod_ini.get("Mod", "version");
                if (data.mod_name.empty()) {
                    fprintf(stderr, "Missing name in Mod section at %s, not loading\n", path_ini.c_str());
                    data.available = false;
                } else if (data.mod_version.empty()) {
                    fprintf(stderr, "Missing version in Mod section at %s, not loading\n", path_ini.c_str());
                    data.available = false;
                }

                //Load optional fields
                mod_ini.check_existence = false;
                //Try loading in current locale, then description, then english
                data.mod_description = mod_ini.get("Mod", ("description_" + locale).c_str());
                if (data.mod_description.empty()) {
                    data.mod_description = mod_ini.get("Mod", "description");
                    if (data.mod_description.empty() && locale != "english") {
                        data.mod_description = mod_ini.get("Mod", "description_english");
                    }
                }
                data.mod_authors = mod_ini.get("Mod", "authors");
                data.mod_url = mod_ini.get("Mod", "url");
                
                //Check version
                data.content_game_minimum_version = mod_ini.get("Content", "game_minimum_version");
                if (data.available && !data.content_game_minimum_version.empty()) {
                    int diff = compare_versions(currentVersionNumbers, data.content_game_minimum_version.c_str());
                    if (0 < diff) {
                        fprintf(stderr, "Minimum game version '%s' requirement not satisfied for %s, not loading\n",
                                data.content_game_minimum_version.c_str(), data.path.c_str());
                        data.available = false;
                    }
                }

                //Check content requirements
                data.content_required_content = mod_ini.get("Content", "required_content");
                data.content_disallowed_content = mod_ini.get("Content", "disallowed_content");
            } else if (is_content_ET) {
                //Provide adhoc mod info for legacy ET folder
                bool isRussian = startsWith(locale, "russian");
                data.available = true;
                data.mod_name = "Perimeter: Emperor's Testament";
                data.mod_version = "2.0.0";
                data.mod_description = isRussian ? "Периметр: Завет Императора" : "Perimeter: Emperor's Testament";
                data.mod_authors = "K-D LAB";
                data.mod_url = "https://kdlab.com";
            } else {
                fprintf(stderr, "Mod folder %s has missing info file %s, not loading\n", data.path.c_str(), path_ini.c_str());
            }
            
            //Force disable all mods
            if (!loadMods) {
                data.available = false;
            }

            //Avoid possible duplicates of ET
            if (data.available && is_content_ET && (terGameContentAvailable & PERIMETER_ET)) {
                fprintf(stderr, "ET is already loaded when loading ET content at %s, not loading", data.path.c_str());
                data.available = false;
            }
            
            //Mark as enabled if available and not named with .off at end
            data.enabled = data.available && !endsWith(entry_path.filename().u8string(), ".off");
            
            //If is ET then load now so the rest of mods can act on content properly
            if (data.enabled && is_content_ET) {
                gameMods[data.mod_name] = data;
                loadModCommon(data);
            } else {
                foundMods.emplace_back(data);
            }
        }
    }

    //Ensure selected content is actually present
    if (terGameContentSelect == GAME_CONTENT::CONTENT_NONE || (terGameContentSelect & terGameContentAvailable) != terGameContentSelect) {
        terGameContentSelect = terGameContentAvailable;
    }
    
    //Sort it
    std::sort(foundMods.begin(), foundMods.end(), SortModMetadatas());

    //Load mods
    for (ModMetadata& mod : foundMods) {
        if (0 < gameMods.count(mod.mod_name)) {
            fprintf(stderr, "Mod %s at %s was already loaded, not loading", mod.mod_name.c_str(), mod.path.c_str());
            continue;
        }
        
        if (mod.enabled && !mod.content_required_content.empty()) {
            GAME_CONTENT required = mergeGameContentEnums(getGameContentFromEnumName(mod.content_required_content));
            if (!getMissingGameContent(terGameContentSelect, required).empty()) {
                if (!getMissingGameContent(terGameContentAvailable, required).empty()) {
                    fprintf(stderr, "Game content '%s' not installed which is a requirement for %s, not loading\n",
                            mod.content_required_content.c_str(), mod.path.c_str());
                } else {
                    fprintf(stderr, "Game content '%s' installed but not enabled which is a requirement for %s, not loading\n",
                            mod.content_required_content.c_str(), mod.path.c_str());
                }
                mod.available = mod.enabled = false;
            }
        }
        if (mod.enabled && !mod.content_disallowed_content.empty()) {
            GAME_CONTENT disallowed = mergeGameContentEnums(getGameContentFromEnumName(mod.content_disallowed_content));
            if (terGameContentSelect == disallowed) {
                fprintf(stderr, "Game content '%s' is enabled which is not compatible for %s, not loading\n",
                        mod.content_disallowed_content.c_str(), mod.path.c_str());
                mod.available = mod.enabled = false;
            }
        }
        
        mod.enabled &= mod.available;
        
        gameMods[mod.mod_name] = mod;
        if (!mod.enabled) {
            continue;
        }
        loadModCommon(mod);
    }
    
    //Do some workarounds
    applyWorkarounds();
    
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
        case UNIT_ATTRIBUTE_FILTH_A_ANTS:
        case UNIT_ATTRIBUTE_FILTH_A_CROW:
        case UNIT_ATTRIBUTE_FILTH_A_DAEMON:
        case UNIT_ATTRIBUTE_FILTH_A_DRAGON_HEAD:
        case UNIT_ATTRIBUTE_FILTH_A_DRAGON_BODY:
        case UNIT_ATTRIBUTE_FILTH_A_DRAGON_TAIL:
        case UNIT_ATTRIBUTE_FILTH_A_EYE:
        case UNIT_ATTRIBUTE_FILTH_A_RAT:
        case UNIT_ATTRIBUTE_FILTH_A_SPIDER:
        case UNIT_ATTRIBUTE_FILTH_A_WASP:
        case UNIT_ATTRIBUTE_FILTH_A_WORM:
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

GAME_CONTENT mergeGameContentEnums(const std::vector<GAME_CONTENT>& list) {
    GAME_CONTENT out = CONTENT_NONE;
    for (auto const& c : list) {
        out = static_cast<GAME_CONTENT>(out | c);
    }
    return out;
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
