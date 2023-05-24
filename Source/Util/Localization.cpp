#include "StdAfx.h"
#include "files/files.h"
#include "Localization.h"

bool isLocaleInit = false;
std::string localeCurrent;
std::string localePath;
std::vector<std::string> localesAvailable;

void initLocale() {
    for (filesystem_entry* entry : get_content_entries_directory("Resource/LocData")) {
        if (entry->is_directory) {
            std::filesystem::path path = std::filesystem::u8path(entry->path_content);
            localesAvailable.emplace_back(path.filename().u8string());
        }
    }
    
    const char* cmdlineLocale = check_command_line("locale");
    
    if (cmdlineLocale) {
        localeCurrent = cmdlineLocale;
    }
    if (localeCurrent.empty()) {
        localeCurrent = IniManager("Perimeter.ini", false).get("Game", "Language");
    }
    if (localeCurrent.empty()) {
        localeCurrent = getStringSettings("Locale");
        if (!localeCurrent.empty()) {
            fprintf(stdout, "Previously selected locale: %s\n", localeCurrent.c_str());
        }
    }
    //Clear language if requested
    if (check_command_line("clearlocale") || IniManager("Perimeter.ini", false).getInt("Game","ClearLocale")) {
        fprintf(stdout, "Clearing previously selected locale\n");
        localeCurrent = "";
        putStringSettings("Locale", localeCurrent);
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
    if (localeCurrent.empty() && 1 < localesAvailable.size()) {
        int choice = MessageBoxChoice("Perimeter", "Select language:", localesAvailable);
        if (0 < choice && choice <= localesAvailable.size()) {
            localeCurrent = string_to_lower(localesAvailable[choice - 1].c_str());
            
            //Save user selection
            putStringSettings("Locale", localeCurrent);
            fprintf(stdout, "User selected locale: %s\n", localeCurrent.c_str());
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
            putStringSettings("Locale", localeCurrent);
        }
    }

    fprintf(stdout, "Current locale: %s\n", localeCurrent.c_str());
    
    localePath = getLocRootPath();
    localePath += localeCurrent + "/";
    
    isLocaleInit = true;
}

const std::string& getLocale() {
    if (!isLocaleInit) {
        initLocale();
    }
    return localeCurrent;
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
