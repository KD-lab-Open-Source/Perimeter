#include <string>
#include <cstdint>

#include "xerrhand.h"

#include "integrations.h"

#ifdef PERIMETER_INTEGRATION_STEAM
#include "Steam/integration_steam_public.h"
#endif

static integration_store* store;

void integrations::init() {
    close();
#ifdef PERIMETER_INTEGRATION_STEAM
    store = integration_steam_init();
#endif
}

void integrations::close() {
    if (store) {
        store->close();
        delete store;
        store = nullptr;
    }
}

bool integrations::quant() {
    if (store) {
        store->quant();
        return !store->integration_requested_shutdown;
    }
    return true;
}

integration_store* integrations::get_store() {
    return store;
}

bool integrations::game_will_relaunch() {
#ifdef PERIMETER_INTEGRATION_STEAM
    if (integration_steam_game_will_relaunch()) return true;
#endif
    return false;
}

void integrations::set_rich_presence(
    integration_rich_presence_activity activity,
    const char* activity_extra,
    const char* group_id,
    size_t group_size
) {
    const char* activity_str = nullptr;
    switch (activity) {
        case RichPresenceActivityMenu:
            activity_str = "Menu";
            break;
        case RichPresenceActivityCampaign:
            activity_str = "Campaign";
            break;
        case RichPresenceActivityBattle:
            activity_str = "Battle";
            break;
        case RichPresenceActivityMultiplayerList:
            activity_str = "MultiplayerList";
            break;
        case RichPresenceActivityMultiplayerWaiting:
            activity_str = "MultiplayerWaiting";
            break;
        case RichPresenceActivityMultiplayerPlaying:
            activity_str = "MultiplayerPlaying";
            break;
        default:
            xassert(0);
            break;
    }
    if (!store || !activity_str) {
        return;
    }
    store->set_rich_presence(activity, activity_str, activity_extra, group_id, group_size);
}
