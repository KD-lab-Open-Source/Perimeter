#include <unordered_set>
#include "StdAfx.h"
#include "files/files.h"
#include "Localization.h"

#ifdef GPX
#include <c/gamepix.h>
#endif

bool isLocaleInit = false;
std::string localeCurrent;
std::string localeDefaultFont;
std::string localePath;
std::vector<std::string> localesAvailable;

void saveLocale(const std::string& locale) {
    //Windows players are used to changing it in Locale, so keep behavior consistent if is present in Perimeter.ini
    if (IniManager("Perimeter.ini", false).get("Game", "Language")) {
        IniManager("Perimeter.ini", false).put("Game", "Language", locale.c_str());
    }
    putStringSettings("Locale", locale);
}

void addLocaleEntry(std::unordered_set<std::string>& locales, filesystem_entry* entry) {
    if (entry && entry->is_directory) {
        std::filesystem::path path = std::filesystem::u8path(entry->path_content);
        locales.emplace(path.filename().u8string());
    }
}

void scanLocales() {
    std::unordered_set<std::string> locales;

    //NOTE: when this is run the mods might not be loaded thus may only return folders in real Resource/LocData
    //and not those from mods when scanning it, so we scan mod manually too
    
    //Scan game folder
    for (filesystem_entry* entry : get_content_entries_directory("Resource/LocData")) {
        addLocaleEntry(locales, entry);
    }
    
    //Scan mods LocData
    for (filesystem_entry* mod_entry : get_content_entries_directory("Mods")) {
        for (filesystem_entry* entry : get_content_entries_directory(mod_entry->path_content + PATH_SEP + "Resource/LocData")) {
            addLocaleEntry(locales, entry);
        }
    }
    
    for (auto& locale : locales) {
        localesAvailable.emplace_back(locale);
    }
}

void initLocale() {
    scanLocales();

#ifndef GPX
    const char* cmdlineLocale = check_command_line("locale");
    if (cmdlineLocale) {
        localeCurrent = cmdlineLocale;
    }
#else
    localeCurrent = gpx()->sys()->getLanguage() == "ru" ? "russian" : "english";
#endif

    if (localeCurrent.empty()) {
        localeCurrent = IniManager("Perimeter.ini", false).get("Game", "Language");
        if (!localeCurrent.empty()) {
            fprintf(stdout, "Using game data locale: %s\n", localeCurrent.c_str());
        }
    }
    if (localeCurrent.empty()) {
        localeCurrent = getStringSettings("Locale");
        if (!localeCurrent.empty()) {
            fprintf(stdout, "Using settings locale: %s\n", localeCurrent.c_str());
        }
    }
    //Clear language if requested
    if (check_command_line("clearlocale") || IniManager("Perimeter.ini", false).getInt("Game","ClearLocale")) {
        fprintf(stdout, "Clearing previously selected locale\n");
        localeCurrent = "";
        saveLocale(localeCurrent);
    }

    //Check if locale is actually available
    if (!localeCurrent.empty()) {
        localeCurrent = string_to_lower(localeCurrent.c_str());
        bool found = false;
        for (auto& locale : localesAvailable) {
            found |= stricmp(locale.c_str(), localeCurrent.c_str()) == 0;
            if (found) break;
        }
        if (!found) {
            localeCurrent = "";
        }
    }

    //Show selector if there is more than 1 locales available and none is currently active
    if (check_command_line("chooselocale") || (localeCurrent.empty() && 1 < localesAvailable.size())) {
        int choice = 0;
        if (2 < localesAvailable.size()) {
            choice = MessageBoxChoice("Perimeter", "Select language:", localesAvailable);
            if (0 < choice && choice <= localesAvailable.size()) {
                localeCurrent = string_to_lower(localesAvailable[choice - 1].c_str());

                //Save user selection
                saveLocale(localeCurrent);
                fprintf(stdout, "User selected locale: %s\n", localeCurrent.c_str());
            }
        }
        //If failed go to next choice
        if (choice == 0 && !localeCurrent.empty()) {
            for (int i = 0; i < localesAvailable.size(); ++i) {\
                if (stricmp(localesAvailable[i].c_str(), localeCurrent.c_str()) == 0) {
                    if (i < (localesAvailable.size() - 1)) {
                        localeCurrent = string_to_lower(localesAvailable[i + 1].c_str());
                    } else {
                        localeCurrent = string_to_lower(localesAvailable[0].c_str());
                    }

                    //Save auto selection
                    if (!localeCurrent.empty()) {
                        saveLocale(localeCurrent);
                        fprintf(stdout, "Selected next locale: %s\n", localeCurrent.c_str());
                        break;
                    }
                }
            }
        }
    }  
    if (localeCurrent.empty()) {
        if (localesAvailable.empty()) {
            //No languages in locales dir ???
            localeCurrent = "?";
            fprintf(stderr, "No locale available!");
        } else {
            //Use english if available, since everybody knows it
            for (auto& locale : localesAvailable) {
                if (stricmp(locale.c_str(), "english") == 0) {
                    localeCurrent = "english";
                    break;
                }
            }
            //Just choose first available lang as last ditch effort
            if (localeCurrent.empty()) {
                localeCurrent = string_to_lower(localesAvailable.front().c_str());
                fprintf(stdout, "No locale selected, using first available: %s\n", localeCurrent.c_str());
            }
            saveLocale(localeCurrent);
            fprintf(stdout, "Selected fallback locale: %s\n", localeCurrent.c_str());
        }
    }

    ErrH.SetLocale(localeCurrent);

    fprintf(stdout, "Current locale: %s\n", localeCurrent.c_str());
    
    //Find the folder of locale, the locale name in filesystem might not be lowercase
    localePath.clear();
    for (auto& locale : localesAvailable) {
        if (stricmp(locale.c_str(), localeCurrent.c_str()) == 0) {
            localePath = getLocRootPath() + locale + "/";
            break;
        }
    }
    if (localePath.empty()) {
        fprintf(stdout, "Current locale path not found!\n");
    } else {
        fprintf(stdout, "Current locale path: %s\n", localePath.c_str());
    }
    
    //TODO workaround to fix multiplayer games with mixed russian locale and non russian locale players, remove when UTF8 is supported
    localeDefaultFont.clear();
    if (localeCurrent == "english") {
        for (auto& locale : localesAvailable) {
            if (stricmp(locale.c_str(), "russian") == 0) {
                localeDefaultFont = "russian";
                break;
            }
        }
    }
    
    isLocaleInit = true;
}

const std::string& getLocale() {
    if (!isLocaleInit) {
        initLocale();
    }
    return localeCurrent;
}

const std::string& getDefaultFontLocale() {
    if (!isLocaleInit) {
        initLocale();
    }
    if (localeDefaultFont.empty()) {
        return localeCurrent;
    } else {
        return localeDefaultFont;
    }
}

const std::vector<std::string>& getLocales() {
    if (!isLocaleInit) {
        initLocale();
    }
    return localesAvailable;
}

const char* getLocRootPath() {
    return "Resource/LocData/";
}

const std::string& getLocDataPath() {
    if (!isLocaleInit) {
        initLocale();
    }
    return localePath;
}
