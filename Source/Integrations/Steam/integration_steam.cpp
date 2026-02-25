#include <string>
#include <cstdint>
#include <filesystem>

#include <steam_api.h>

#include "SystemUtil.h"
#include "integrations.h"
#include "integration_steam.h"

integration_steam* integration_steam::init() {
    IniManager ini("Perimeter.ini", false);
    bool has_steam = *ini.get("Integrations", "Steam") != 0;
    if (has_steam && !ini.getInt("Integrations", "Steam")) {
        return nullptr;
    }
    
    SteamErrMsg errMsg;
    switch (SteamAPI_InitEx(&errMsg)) {
        case k_ESteamAPIInitResult_OK:
            printf("Steam integration init OK\n");
            break;
        case k_ESteamAPIInitResult_FailedGeneric:
            fprintf(stderr, "Steam integration init failed: %s\n", errMsg);
            return nullptr;
        case k_ESteamAPIInitResult_NoSteamClient:
            fprintf(stderr, "Steam integration init can't connect: %s\n", errMsg);
            return nullptr;
        case k_ESteamAPIInitResult_VersionMismatch:
            fprintf(stderr, "Steam integration init version mismatch: %s\n", errMsg);
            return nullptr;
    }
    
    //Store config in ini if absent
    if (!has_steam) {
        ini.put("Integrations", "Steam", "1");
    }
    if (*ini.get("Integrations", "SteamRelaunchIfDirectly") == 0) {
        ini.put("Integrations", "SteamRelaunchIfDirectly", "1");
    }
    ini.save();
    
    integration_steam* steam = new integration_steam();
    return steam;
}

void integration_steam::close() {
    SteamAPI_Shutdown();
}

std::string integration_steam::get_player_name() {
    std::string player_name = SteamFriends()->GetPersonaName();
    return player_name;
}

std::string integration_steam::get_selected_locale() {
    std::string selected_locale = SteamApps()->GetCurrentGameLanguage();
    return selected_locale;
}

bool integration_steam::game_will_relaunch() {
    IniManager ini("Perimeter.ini", false);
    if (ini.getInt("Integrations", "SteamRelaunchIfDirectly")) {
        if (SteamAPI_RestartAppIfNecessary(INTEGRATION_STEAM_APPID)) {
            printf("Steam integration detect game launched directly, restarting to run via Steam\n");
            return true;
        }
    }
    return false;
}
